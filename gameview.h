#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QTcpSocket>
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QRandomGenerator>
#include "server.h"
#include "plane.h"
#define FRAMES_PER_SEC 40
#define TILE_SIZE 32
#define PLANE_SPEED 5

class GameView : public QGraphicsView
{
    Q_OBJECT
public:
    GameView(Server* server, QVector <QString> map, QWidget* parent = nullptr);

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
    QTimer* updateParamsTimer;
    QTimer* animationTimer;

    void drawMap() const;
    QPointF getRandomPos() const;


public slots:
    void createNewPlayer(QTcpSocket* pClientSocket);
    void updatePlayerParams(PlayerInfo player);
    void sendParamsForAllPlayers();

};

#endif // GAMEVIEW_H
