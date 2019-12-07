#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H
#include <QThread>
#include <QTcpSocket>
#include <QTcpServer>
#include "server.h"
#include "simplecrypt.h"
#include "playerinfo.h"
#include "bullet.h"


class ServerThread : public QThread
{
    Q_OBJECT
public:
    ServerThread(QTcpServer* m_ptcpServer, QObject *parent);

    void run() override;

signals:
    void error(QTcpSocket::SocketError socketError);
    void newPlayerConnected(QTcpSocket* pClientSocket);
    void playerParamsChanged(PlayerInfo player);
    void bulletReceived(PlayerInfo player);
    void clientDisconneted(QTcpSocket* pClientSocket);

private:
    QTcpSocket* pClientSocket;
    QTcpServer* m_ptcpServer;
    quint32 blockSize_map;
    quint16 blockSize;
    SimpleCrypt crypto;

public slots:
    void sendCoordsToClient(QTcpSocket* pSocket, const QVector<PlayerInfo> players);
    void sendIdAndMapToClient(QTcpSocket* pSocket, idAndMap info);
    void sendBulletToClient(QTcpSocket *pSocket, BulletInfo bullet);
    void slotReadClient();
};

#endif // SERVERTHREAD_H
