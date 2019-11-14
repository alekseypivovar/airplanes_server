#include "server.h"



Server::Server(int port, QWidget *parent)
    : QTcpServer(parent), blockSize_map(0), blockSize(0)
{
    m_ptcpServer = new QTcpServer(this);
    if(!m_ptcpServer->listen(QHostAddress::Any, quint16(port))){

        qDebug() << "Unable to start server: " << m_ptcpServer->errorString();

        m_ptcpServer->close();
        return;
    }
    else
        qDebug() << "Listening port: " << port;

    connect(m_ptcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
}

Server::~Server()
{
}

void Server::slotNewConnection(){

    QTcpSocket* pClientSocket = m_ptcpServer->nextPendingConnection();
    connect(pClientSocket, SIGNAL(disconnected()), pClientSocket, SLOT(deleteLater()));
    connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));
    qDebug() << "Connected!";

    emit newPlayerConnected(pClientSocket);
}

void Server::slotReadClient()
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
    PlayerInfo player;
    in >> player;

    emit playerParamsChanged(player);
    qDebug() << "Data RECEIVED!";
}

void Server::sendCoordsToClient(QTcpSocket *pSocket, const QVector<PlayerInfo> players)
{
    QByteArray block;
    QDataStream out (&block, QIODevice::WriteOnly);

    out << quint16(0) << players;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof (quint16));
    pSocket->write(block);
    //qDebug() << "Coords SENDED!";
}


void Server::sendIdAndMapToClient(QTcpSocket *pSocket, idAndMap info)
{
    QByteArray block;
    QDataStream out (&block, QIODevice::WriteOnly);

    out << quint32(0) << info;
    out.device()->seek(0);
    out << quint32(block.size() - sizeof (quint32));
    pSocket->write(block);
    qDebug() << "MAP SENDED!";
}



