#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>
#include <QDebug>
#include <QVector>
#include "playerinfo.h"

class Server : public QTcpServer
{
    Q_OBJECT
private:
    QTcpServer* m_ptcpServer;
    quint32 blockSize;

public:
    Server(int port, QWidget *parent = nullptr);
    ~Server();

    void sendCoordsToClient(QTcpSocket* pSocket, const QVector<PlayerInfo>& players);
    void sendIdAndMapToClient(QTcpSocket* pSocket, idAndMap info);

private slots:
    void slotReadClient();
    virtual void slotNewConnection();

signals:
    void newPlayerConnected(QTcpSocket* pClientSocket);
    void playerParamsChanged(PlayerInfo player);
};

#endif // SERVER_H
