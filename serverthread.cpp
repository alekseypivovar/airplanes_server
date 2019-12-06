#include "serverthread.h"


ServerThread::ServerThread(QTcpServer* m_ptcpServer, QObject *parent)
    : QThread(parent), blockSize_map(0), blockSize(0)
{
    this->m_ptcpServer  = m_ptcpServer;
    this->pClientSocket = m_ptcpServer->nextPendingConnection();
    this->crypto = SimpleCrypt(4815162342);
}

void ServerThread::run()
{
//    QTcpSocket* pClientSocket = m_ptcpServer->nextPendingConnection();

    connect(pClientSocket, &QTcpSocket::disconnected, [=]() {
        emit clientDisconneted(pClientSocket);
        pClientSocket->deleteLater();
    } );

    connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()), Qt::QueuedConnection);
    qDebug() << "Connected!";

    emit newPlayerConnected(pClientSocket);
}

void ServerThread::slotReadClient()
{
    qDebug() << "Read data...";
    QTcpSocket* pClientSocket = qobject_cast<QTcpSocket*>(sender());
    QDataStream in(pClientSocket);

    if (blockSize == 0){
        if (pClientSocket->bytesAvailable() < int(sizeof (quint16)))
            return;
        in >> blockSize;
    }

    if (pClientSocket->bytesAvailable() < blockSize)
        return;

    blockSize = 0;

    SendInfoType type;
    int buffer;
    in >> buffer;
    type = SendInfoType(buffer);

    if (type == COORDS) {
        QByteArray bytes;
        in >> bytes;
        bytes = crypto.decryptToByteArray(bytes);
        PlayerInfo player;
        QDataStream ds(&bytes, QIODevice::ReadWrite);
        ds >> player;

        emit playerParamsChanged(player);
        qDebug() << "Data RECEIVED!";
    }
    else if (type == BULLET) {
        QByteArray bytes;
        in >> bytes;
        bytes = crypto.decryptToByteArray(bytes);
        PlayerInfo player;
        QDataStream ds(&bytes, QIODevice::ReadWrite);
        ds >> player;

        emit bulletReceived(player);
        qDebug() << "Data RECEIVED!";
    }
}

void ServerThread::sendCoordsToClient(QTcpSocket *pSocket, const QVector<PlayerInfo> players)
{
    if (pSocket->socketDescriptor() != this->pClientSocket->socketDescriptor())
        return;
    QByteArray data;
    QDataStream ds(&data, QIODevice::ReadWrite);
    ds << players;
    data = crypto.encryptToByteArray(data);

    QByteArray block;
    QDataStream out (&block, QIODevice::WriteOnly);

    out << quint16(0) << COORDS << data;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof (quint16));
    pClientSocket->write(block);
    qDebug() << "Coords SENDED!";
}


void ServerThread::sendIdAndMapToClient(QTcpSocket *pSocket, idAndMap info)
{
    if (pSocket->socketDescriptor() != this->pClientSocket->socketDescriptor())
        return;
    QByteArray data;
    QDataStream ds(&data, QIODevice::ReadWrite);
    ds << info;
    data = crypto.encryptToByteArray(data);

    QByteArray block;
    QDataStream out (&block, QIODevice::WriteOnly);

    out << quint32(0) << data;
    out.device()->seek(0);
    out << quint32(block.size() - sizeof (quint32));
    pClientSocket->write(block);
    qDebug() << "MAP SENDED!";
}

void ServerThread::sendBulletToClient(QTcpSocket *pSocket, BulletInfo bullet)
{
    if (pSocket->socketDescriptor() != this->pClientSocket->socketDescriptor())
        return;
    QByteArray data;
    QDataStream ds(&data, QIODevice::ReadWrite);
    ds << bullet;
    data = crypto.encryptToByteArray(data);

    QByteArray block;
    QDataStream out (&block, QIODevice::WriteOnly);

    out << quint16(0) << BULLET << data;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof (quint16));
    pClientSocket->write(block);
    qDebug() << "BULLET SENDED!";
}
