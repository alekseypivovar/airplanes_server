#include "gameview.h"

GameView::GameView(Server* server, QVector <QString> map, QWidget* parent) : QGraphicsView(parent)
{
    this->server = server;
    this->map = map;
    QGraphicsScene* scene = new QGraphicsScene(parent);
    this->setScene(scene);

    drawMap();



    updateParamsTimer = new QTimer;
    updateParamsTimer->start(2000);
    if (scene != nullptr)
        connect(updateParamsTimer, SIGNAL(timeout()), this, SLOT(sendParamsForAllPlayers()), Qt::QueuedConnection);

    animationTimer = new QTimer;
    animationTimer->start(1000 / FRAMES_PER_SEC);
    if (scene != nullptr)
        connect(animationTimer, SIGNAL(timeout()), this->scene(), SLOT(advance()));
}

void GameView::drawMap() const
{
    // Устанавливаем размер сцены на весь мир
    this->scene()->setSceneRect(0,0, (map.at(0).length() - 1) * TILE_SIZE, map.length() * TILE_SIZE);

    // Перебор и расстановка тайлов. j - номер тайла по оси Х, i - номер по оси Y
    for (int i = 0; i < map.length(); i++) {
        for (int j = 0; j < map.at(0).length(); j++) {
            QPixmap tile;
            if (map.at(i).at(j) == "0"){
                tile = QPixmap(":/images/0.bmp");
            }
            else if (map.at(i).at(j) == "1"){
                tile = QPixmap(":/images/1.bmp");
            }
            else if (map.at(i).at(j) == "2"){
                tile = QPixmap(":/images/2.bmp");
            }
            else if (map.at(i).at(j) == "3"){
                tile = QPixmap(":/images/3.bmp");
            }
            else if (map.at(i).at(j) == "4"){
                tile = QPixmap(":/images/4.bmp");
            }
            else if (map.at(i).at(j) == "5"){
                tile = QPixmap(":/images/5.bmp");
            }
            else if (map.at(i).at(j) == "6"){
                tile = QPixmap(":/images/6.bmp");
            }
            else if (map.at(i).at(j) == "7"){
                tile = QPixmap(":/images/7.bmp");
            }
            else if (map.at(i).at(j) == "8"){
                tile = QPixmap(":/images/8.bmp");
            }else if (map.at(i).at(j) == "A"){
                tile = QPixmap(":/images/A.bmp");
            }
            else if (map.at(i).at(j) == "B"){
                tile = QPixmap(":/images/B.bmp");
            }
            else if (map.at(i).at(j) == "C"){
                tile = QPixmap(":/images/C.bmp");
            }
            else if (map.at(i).at(j) == "D"){
                tile = QPixmap(":/images/D.bmp");
            }
            else {
                tile = QPixmap(":/images/E.bmp");
            }



            //           tile = tile.scaled(TILE_SIZE, TILE_SIZE);
            QGraphicsPixmapItem* pItem = this->scene()->addPixmap(tile);
            pItem->setPos(QPointF(j * TILE_SIZE, i * TILE_SIZE));
        }
    }
}

QPointF GameView::getRandomPos() const
{
    // Вариант для нормальной версии Qt
//    quint32 x = QRandomGenerator::global()->bounded(quint32(scene()->width()  * 0.8)) + quint32(scene()->width()  * 0.1);
//    quint32 y = QRandomGenerator::global()->bounded(quint32(scene()->height() * 0.8)) + quint32(scene()->height() * 0.1);

//    qsrand(quint32(QDateTime::currentMSecsSinceEpoch()));
    quint32 high = quint32(scene()->width()  * 0.8);
    quint32 low  = quint32(scene()->width()  * 0.1);
    quint32 x = quint32(qrand()) % ((high + 1) - low) + low;
    quint32 y = quint32(qrand()) % ((high + 1) - low) + low;
    return QPointF(x, y);
}


void GameView::createNewPlayer(QTcpSocket *pClientSocket)
{
    qint16 id = qint16(players.length());
    QPointF pos = getRandomPos();
    PlayerInfo newPlayer(id, pos, 45, PlayerInfo::PLANE, PLANE_SPEED, 0, 5);
    PlayerInfo_SERVER newPlayer_SERVER;
    newPlayer_SERVER.id = id;
    newPlayer_SERVER.socket = pClientSocket;
    newPlayer_SERVER.isEnabled = true;

    players << newPlayer;
    players_SERVER << newPlayer_SERVER;

    Plane* plane = new Plane(id, newPlayer.getType(), newPlayer.getAngle(), newPlayer.getSpeed(), newPlayer.getAngleSpeed(), newPlayer.getHealth());
    this->scene()->addItem(plane);
    planes << plane;
    plane->setPos(pos);
    connect(plane, SIGNAL(planeMoved(Plane*)), this, SLOT(updatePlanePos(Plane*)));
    connect(plane, SIGNAL(planeAndBulletCollided(Plane*, Bullet*)),
            this, SLOT(planeAndBulletCollided(Plane*, Bullet*)), Qt::QueuedConnection);
    connect(plane, SIGNAL(planeAndPlaneCollided(Plane*, Plane*)),
            this, SLOT(planeAndPlaneCollided(Plane*, Plane*)), Qt::QueuedConnection);

    idAndMap id_map;
    id_map.id  = id;
    id_map.map = map;
    emit sendIdAndMapToClient(pClientSocket, id_map);
    sendParamsForAllPlayers();
}

void GameView::updatePlayerParams(PlayerInfo player)
{
    qint32 number = player.getId();
    players[number].setSpeed(player.getSpeed());
    players[number].setAngleSpeed(player.getAngleSpeed());
    planes [number]->setSpeed(player.getSpeed());
    planes [number]->setAngleSpeed(player.getAngleSpeed());

    sendParamsForAllPlayers();
}

void GameView::sendParamsForAllPlayers()
{
    for (int i = 0; i < players_SERVER.length(); i++) {
        if (players_SERVER.at(i).isEnabled) {           // ПРОВЕРИТЬ УСЛОВИЕ ПРИ УДАЛЕНИИ ИГРОКОВ +++++++++++++++++++++
            emit sendCoordsToClient(players_SERVER.at(i).socket, players);
        }
    }
}

void GameView::planeAndBulletCollided(Plane *plane, Bullet *bullet)
{
    qDebug() << "Collision - BULLET!";
    players[(plane->getId())].setHealth(players[(plane->getId())].getHealth() - 1);
    qDebug() << "Health = " << players[(plane->getId())].getHealth();
    if (players[(plane->getId())].getHealth() <= 0)
        respawn(plane);



    this->scene()->removeItem(bullet);
    delete bullet;
    sendParamsForAllPlayers();
}

void GameView::planeAndPlaneCollided(Plane *plane1, Plane *plane2)
{
    qDebug() << "Collision - PLANES!";
    players[(plane1->getId())].setHealth(0);
    respawn(plane1);
    players[(plane2->getId())].setHealth(0);
    respawn(plane2, 7000);

    sendParamsForAllPlayers();
}

void GameView::updatePlanePos(Plane *plane)
{
    players[plane->getId()]. setPos(plane->scenePos());
    players[plane->getId()].setAngle(plane->getAngle());

    QPointF planePos = plane->scenePos();
    if (planePos.x() < 0 || planePos.x() > scene()->width() ||
            planePos.y() < 0 || planePos.y() > scene()->height()) {
        players[(plane->getId())].setHealth(0);
        respawn(plane);
    }
}


//void GameView::checkCollisions(Plane *plane)
//{
//    QList<QGraphicsItem *> items = scene()->collidingItems(plane);
//    if (items.isEmpty()) {
//        return;
//    }

//    for (int i = 0; i < items.length(); i++) {
//        Bullet* bullet = qgraphicsitem_cast<Bullet*>(items.at(i));
//        if (bullet) {
//            players[(plane->getId())].setHealth(players[(plane->getId())].getHealth() - 1);
//            qDebug() << "Health = " << players[(plane->getId())].getHealth();
//            if (players[(plane->getId())].getHealth() <= 0)
//                respawn(plane);
//            this->scene()->removeItem(bullet);
//            delete bullet;
//            sendParamsForAllPlayers();
//            qDebug() << "Collision - BULLET!";
//            return;
//        }
//        else {
//            Plane* enemyPlane = qgraphicsitem_cast<Plane*>(items.at(i));
//            if (enemyPlane) {
//                players[(plane->getId())].setHealth(0);
//                players[(enemyPlane->getId())].setHealth(0);
//                sendParamsForAllPlayers();
//                qDebug() << "Collision - PLANE!";
//                respawn(plane);
//                respawn(enemyPlane);
//                return;
//            }
//        }
//    }

//}

void GameView::respawn(Plane *plane, qint32 time)
{
    qDebug() << "Respawn!";
    QPointF newPos = getRandomPos();
    qDebug() << "New pos:" << newPos;
//    plane->hide();
    planes[plane->getId()]->setPos(newPos);
    planes[plane->getId()]->setSpeed(0);
    planes[plane->getId()]->setAngleSpeed(0);
    players[(plane->getId())].setPos(newPos);
    players[(plane->getId())].setSpeed(0);
    players[(plane->getId())].setAngleSpeed(0);
//    sendParamsForAllPlayers();
    //QTimer::singleShot(2000, plane, SLOT(makePlaneAlive(plane)));
    QTimer::singleShot(time, [=]() {
        qDebug() << "Plane alive again!!";
        players[(plane->getId())].setHealth(5);
        planes[plane->getId()]->setSpeed(PLANE_SPEED);
        players[(plane->getId())].setSpeed(PLANE_SPEED);
        sendParamsForAllPlayers();
    } );

//    QTimer *timer = new QTimer(this);
//    timer->setSingleShot(true);

//    connect(timer, &QTimer::timeout, [=]() {
//        plane->setSpeed(PLANE_SPEED);
//        players[(plane->getId())].setSpeed(PLANE_SPEED);
//        sendParamsForAllPlayers();
//        timer->deleteLater();
    //    } );
}

void GameView::connectGameViewAndServer()
{
    connect(server, SIGNAL(newPlayerConnected(QTcpSocket*)),
            this, SLOT(createNewPlayer(QTcpSocket*)), Qt::QueuedConnection);
    connect(server, SIGNAL(playerParamsChanged(PlayerInfo)),
            this, SLOT(updatePlayerParams(PlayerInfo)), Qt::BlockingQueuedConnection);
    connect(server, SIGNAL(bulletReceived(PlayerInfo)), this, SLOT(createBullet(PlayerInfo)), Qt::QueuedConnection);
    connect(server, SIGNAL(clientDisconneted(QTcpSocket* )), this, SLOT(disconnectClient(QTcpSocket*)), Qt::QueuedConnection);

    connect(this, SIGNAL(sendIdAndMapToClient(QTcpSocket *, idAndMap)),
            server, SLOT(sendIdAndMapToClient(QTcpSocket *, idAndMap)), Qt::QueuedConnection);
    connect(this, SIGNAL(sendCoordsToClient(QTcpSocket *, const QVector<PlayerInfo> )),
            server, SLOT(sendCoordsToClient(QTcpSocket *, const QVector<PlayerInfo> )), Qt::QueuedConnection);
    connect(this, SIGNAL(sendBulletToClient(QTcpSocket *, BulletInfo)),
            server, SLOT(sendBulletToClient(QTcpSocket *, BulletInfo)), Qt::QueuedConnection);
}


void GameView::createBullet(PlayerInfo player)
{
    QPointF pos = planes.at(player.getId())->scenePos() + planes.at(player.getId())->getGunPos();
    Bullet* bullet = new Bullet(pos, players.at(player.getId()).getAngle());
    this->scene()->addItem(bullet);
    bullet->setPos(pos);

    for (int i = 0; i < players_SERVER.length(); i++) {
        if (players_SERVER.at(i).isEnabled) {           // ПРОВЕРИТЬ УСЛОВИЕ ПРИ УДАЛЕНИИ ИГРОКОВ +++++++++++++++++++++
            emit sendBulletToClient(players_SERVER.at(i).socket, bullet->getBulletInfo());
        }
    }
}

//void GameView::makePlaneAlive(Plane *plane)
//{
//    plane->setSpeed(PLANE_SPEED);
//    players[(plane->getId())].setSpeed(PLANE_SPEED);
//    sendParamsForAllPlayers();
//}

void GameView::disconnectClient(QTcpSocket *pClientSocket)
{
    for (int i = 0; i < players_SERVER.length(); i++) {
        if (players_SERVER.at(i).socket == pClientSocket) {
            players_SERVER[i].isEnabled = false;
            players[i].setHealth(0);

            sendParamsForAllPlayers();
            this->scene()->removeItem(planes[i]);
            delete planes[i];
            return;
        }
    }
}

