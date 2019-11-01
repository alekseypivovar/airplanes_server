#include "gameview.h"

GameView::GameView(Server* server, QVector <QString> map, QWidget* parent) : QGraphicsView(parent)
{
    this->server = server;
    this->map = map;
    QGraphicsScene* scene = new QGraphicsScene(parent);
    this->setScene(scene);

    drawMap();

    connect(server, SIGNAL(newPlayerConnected(QTcpSocket*)), this, SLOT(createNewPlayer(QTcpSocket*)));
    connect(server, SIGNAL(playerParamsChanged(PlayerInfo)), this, SLOT(updatePlayerParams(PlayerInfo)));


    updateParamsTimer = new QTimer;
    updateParamsTimer->start(100);
    if (scene != nullptr)
        connect(updateParamsTimer, SIGNAL(timeout()), this, SLOT(sendParamsForAllPlayers()));

    animationTimer = new QTimer;
    animationTimer->start(1000 / FRAMES_PER_SEC);
    if (scene != nullptr)
        connect(animationTimer, SIGNAL(timeout()), this->scene(), SLOT(advance()));
}

void GameView::drawMap() const
{
    // Устанавливаем размер сцены на весь мир
    this->scene()->setSceneRect(0,0, map.at(0).length() * TILE_SIZE, map.length() * TILE_SIZE);

    // Перебор и расстановка тайлов. j - номер тайла по оси Х, i - номер по оси Y
    for (int i = 0; i < map.length(); i++) {
        for (int j = 0; j < map.at(0).length(); j++) {
            QPixmap tile;
            if (map.at(i).at(j) == "0"){
                tile = QPixmap(":/images/0.png");
            }
            // else if остальные тайлы ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


            //           tile = tile.scaled(TILE_SIZE, TILE_SIZE);
            QGraphicsPixmapItem* pItem = this->scene()->addPixmap(tile);
            pItem->setPos(QPointF(j * TILE_SIZE, i * TILE_SIZE));
        }
    }
}

QPointF GameView::getRandomPos() const
{
    quint32 x = QRandomGenerator::global()->bounded(quint32(scene()->width()  * 0.8)) + quint32(scene()->width()  * 0.1);
    quint32 y = QRandomGenerator::global()->bounded(quint32(scene()->height() * 0.8)) + quint32(scene()->height() * 0.1);
    return QPointF(x, y);
}

void GameView::createNewPlayer(QTcpSocket *pClientSocket)
{
    qint32 id = players.length();
    QPointF pos = getRandomPos();
    PlayerInfo newPlayer(id, pos, 90, PlayerInfo::PLANE, PLANE_SPEED, 0, 5);
    PlayerInfo_SERVER newPlayer_SERVER;
    newPlayer_SERVER.id = id;
    newPlayer_SERVER.socket = pClientSocket;
    newPlayer_SERVER.isEnabled = true;

    players << newPlayer;
    players_SERVER << newPlayer_SERVER;

    Plane* plane = new Plane(id, newPlayer.getType(), newPlayer.getAngle(), newPlayer.getSpeed(), newPlayer.getAngleSpeed(), newPlayer.getHealth());
    this->scene()->addItem(plane);
    plane->setPos(pos);

    idAndMap id_map;
    id_map.id  = id;
    id_map.map = map;
    server->sendIdAndMapToClient(pClientSocket, id_map);
    sendParamsForAllPlayers();
}

void GameView::updatePlayerParams(PlayerInfo player)
{
    qint32 number = player.getId();
    players[number].setSpeed(player.getSpeed());
    players[number].setAngleSpeed(player.getAngleSpeed());

    sendParamsForAllPlayers();
}

void GameView::sendParamsForAllPlayers()
{
    for (int i = 0; i < players_SERVER.length(); i++) {
        if (players_SERVER.at(i).socket) {           // ПРОВЕРИТЬ УСЛОВИЕ ПРИ УДАЛЕНИИ ИГРОКОВ +++++++++++++++++++++
            server->sendCoordsToClient(players_SERVER.at(i).socket, players);
        }
    }
}

