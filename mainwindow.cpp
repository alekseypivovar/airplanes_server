#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Show server IP adress
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
             ui->label_IP->setText(address.toString());
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

QVector<QString> MainWindow::getMap()
{
    QFile file(":/map.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error while opening map file!";
        return QVector<QString>();
    }

    QVector<QString> map;

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        map << QString(line);
    }
    return map;
}

void MainWindow::on_pushButton_clicked()
{
    if (startServer()) {
        statusBar()->showMessage("Server started successfully");
        ui->pushButton->setDisabled(true);

        QVector<QString> map = getMap();
        if (map.isEmpty()) {
            qDebug() << "No map!";
            exit (1);
        }

        GameView* gameView = new GameView(server, map, this);
    }
    else {
        statusBar()->showMessage("Server start failed!");
        ui->pushButton->setEnabled(true);
    }
}

bool MainWindow::startServer()
{
    server = new Server(2323);
    return true;
}
