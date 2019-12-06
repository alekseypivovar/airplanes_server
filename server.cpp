#include "server.h"


Server::Server(int port, QWidget *parent)
    : QTcpServer(parent)
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

void Server::sendCoordsToClient(QTcpSocket *pSocket, const QVector<PlayerInfo> players)
{
    emit sendCoords(pSocket, players);
}

void Server::sendIdAndMapToClient(QTcpSocket *pSocket, idAndMap info)
{
    emit sendIdAndMap(pSocket, info);
}

void Server::sendBulletToClient(QTcpSocket *pSocket, BulletInfo bullet)
{
    emit sendBullet(pSocket, bullet);
}

void Server::slotReadClient()
{
    emit readClient();
}

void Server::slotNewConnection(){
    ServerThread* thread = new ServerThread(m_ptcpServer, this);

//    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    connect(thread, &ServerThread::newPlayerConnected, [=](QTcpSocket* pClientSocket) {
        emit newPlayerConnected(pClientSocket);
    } );
    connect(thread, &ServerThread::playerParamsChanged, [=](PlayerInfo player) {
        emit playerParamsChanged(player);
    } );
    connect(thread, &ServerThread::bulletReceived, [=](PlayerInfo player) {
        emit bulletReceived(player);
    } );
    connect(thread, &ServerThread::clientDisconneted, [=](QTcpSocket* pClientSocket) {
        emit clientDisconneted(pClientSocket);
    } );

    connect(this, SIGNAL(sendCoords(QTcpSocket*, const QVector<PlayerInfo>)),
            thread, SLOT(sendCoordsToClient(QTcpSocket*, const QVector<PlayerInfo>)), Qt::QueuedConnection);
    connect(this, SIGNAL(sendIdAndMap(QTcpSocket*, idAndMap)),
            thread, SLOT(sendIdAndMapToClient(QTcpSocket*, idAndMap)), Qt::QueuedConnection);
    connect(this, SIGNAL(sendBullet(QTcpSocket*, BulletInfo)),
            thread, SLOT(sendBulletToClient(QTcpSocket*, BulletInfo)), Qt::QueuedConnection);
    connect(this, SIGNAL(readClient()),
            thread, SLOT(slotReadClient()), Qt::QueuedConnection);

    thread->start();
}







