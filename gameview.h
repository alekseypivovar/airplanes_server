#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QTcpSocket>
#include <QTimer>
#include <QGraphicsPixmapItem>
//#include <QRandomGenerator>
#include <QDateTime>
#include "server.h"
#include "plane.h"
#include "bullet.h"


class GameView : public QGraphicsView
{
    Q_OBJECT
public:
    GameView(Server* server, QVector <QString> map, QWidget* parent = nullptr);

    void connectGameViewAndServer();
private:
    struct PlayerInfo_SERVER {
        qint32 id;
        bool isEnabled;
        QTcpSocket* socket;
    };

    Server* server;
    QVector <QString> map;
    QVector <PlayerInfo> players;
    QVector <PlayerInfo_SERVER> players_SERVER;
    QVector <Plane*> planes;
    QTimer* updateParamsTimer;
    QTimer* animationTimer;

    void drawMap() const;
    QPointF getRandomPos() const;
//    void checkCollisions(Plane *plane);
    void respawn(Plane *plane, qint32 time = 5000);



public slots:
    void createNewPlayer(QTcpSocket* pClientSocket);
    void updatePlayerParams(PlayerInfo player);
    void sendParamsForAllPlayers();
    void planeAndBulletCollided(Plane* plane, Bullet* bullet);
    void planeAndPlaneCollided(Plane* plane1, Plane* plane2);

private slots:
    void updatePlanePos(Plane* plane);
    void createBullet(PlayerInfo player);
//    void makePlaneAlive(Plane* plane);
    void disconnectClient(QTcpSocket* pClientSocket);

signals:
    void sendIdAndMapToClient(QTcpSocket *pSocket, idAndMap info);
    void sendCoordsToClient(QTcpSocket *pSocket, const QVector<PlayerInfo> players);
    void sendBulletToClient(QTcpSocket *pSocket, BulletInfo bullet);

};

#endif // GAMEVIEW_H
