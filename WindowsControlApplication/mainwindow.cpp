#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground);
    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &MainWindow::refreshLCD);
    timer->start(200);
    carController = new CarController();
    ui->setupUi(this);
}

void MainWindow::refreshLCD(){
    double currentSpeed = ((double)carController->GetEngineValue()-50)*2;
    double currentAngle = ((double)carController->GetServoValue()-50)*2/5;
    int distance = carController->GetDistance();
    ui->lcdDistance->display(distance);
    ui->lcdSpeed->display(currentSpeed);
    ui->lcdAngle->display(currentAngle);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    qDebug() << "Key Pressed:" << event->key();
    switch (event->key()) {
        case Qt::Key_W: carController->setCurrentStatus(forward,true); break;
        case Qt::Key_S: carController->setCurrentStatus(backward,true); break;
        case Qt::Key_A: carController->setCurrentStatus(left,true); break;
        case Qt::Key_D: carController->setCurrentStatus(right,true); break;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    qDebug() << "Key Released:" << event->key();
    switch (event->key()) {
        case Qt::Key_W: carController->setCurrentStatus(forward,false); break;
        case Qt::Key_S: carController->setCurrentStatus(backward,false); break;
        case Qt::Key_A: carController->setCurrentStatus(left,false); break;
        case Qt::Key_D: carController->setCurrentStatus(right,false); break;
    }
}
