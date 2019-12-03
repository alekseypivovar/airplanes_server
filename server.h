#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>
#include <QDebug>
#include <QVector>
#include "simplecrypt.h"
#include "playerinfo.h"
#include "bullet.h"

class Server : public QTcpServer
{
    Q_OBJECT
private:
    QTcpServer* m_ptcpServer;
    quint32 blockSize_map;
    quint16 blockSize;
    SimpleCrypt crypto;

public:
    Server(int port, QWidget *parent = nullptr);
    ~Server();
public slots:
    void sendCoordsToClient(QTcpSocket* pSocket, const QVector<PlayerInfo> players);
    void sendIdAndMapToClient(QTcpSocket* pSocket, idAndMap info);
    void sendBulletToClient(QTcpSocket *pSocket, BulletInfo bullet);

private slots:
    void slotReadClient();
    virtual void slotNewConnection();

signals:
    void newPlayerConnected(QTcpSocket* pClientSocket);
    void playerParamsChanged(PlayerInfo player);
    void bulletReceived(PlayerInfo player);
    void clientDisconneted(QTcpSocket* pClientSocket);
};

#endif // SERVER_H
