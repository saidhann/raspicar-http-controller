#ifndef CARCONTROLLER_H
#define CARCONTROLLER_H

#define IP_ADRESS "http://192.168.137.25:18080"
#include <QApplication>
#include <QWidget>
#include <QKeyEvent>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

enum modeStatus{
    forward,
    backward,
    left,
    right
};

class CarController : public QWidget {
    Q_OBJECT

    public:
        CarController();
        int GetEngineValue()    {return engineValue;}
        int GetServoValue()     {return servoValue;}
        int GetDistance()       {return distance;}
        void setCurrentStatus(modeStatus status,bool mode) {statusMap.at(status) = mode;}

    private slots:
        void updateCar();
        void getData();
        void handleDistanceReply(QNetworkReply *reply);

    private:
        void sendPostRequest(const QString &endpoint, int value);
        QNetworkAccessManager *engineManager;
        QNetworkAccessManager *dataManager;
        QTimer updateTimer;
        QTimer requestTimer;

        int distance = 0;
        int engineValue = 50;  // 50 = stop
        int servoValue = 50;   // 50 = straight

        std::map<modeStatus,bool> statusMap {
            {forward,false},
            {backward,false},
            {left,false},
            {right,false}
        };
};
#endif // CARCONTROLLER_H
