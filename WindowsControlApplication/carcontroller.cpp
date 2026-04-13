#include "carcontroller.h"


CarController::CarController() {
    setFixedSize(400, 300);
    setFocusPolicy(Qt::StrongFocus);
    // Initialize networking
    dataManager = new QNetworkAccessManager(this);
    engineManager = new QNetworkAccessManager(this);

    // Setup timers
    connect(&updateTimer, &QTimer::timeout, this, &CarController::updateCar);
    connect(&requestTimer, &QTimer::timeout, this, &CarController::getData);
    requestTimer.start(1000);
    updateTimer.start(100);  // Update every 100 ms
}

void CarController::getData() {
    QUrl url(IP_ADRESS + QString("/distance"));  // Replace with your IP
    QNetworkRequest request(url);

    // Make asynchronous GET request
    QNetworkReply *reply = dataManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleDistanceReply(reply);
    });
}

void CarController::handleDistanceReply(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    // Parse JSON response
    QByteArray responseData = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

    if (jsonDoc.isObject()) {
        QJsonObject jsonObj = jsonDoc.object();
        if (jsonObj.contains("value") && jsonObj["value"].isDouble()) {
            distance = jsonObj["value"].toDouble();
            qDebug() << "Distance in cm:" << distance;
        } else {
            qDebug() << "Invalid JSON format.";
        }
    } else {
        qDebug() << "Failed to parse JSON.";
    }

    reply->deleteLater();
}

void CarController::updateCar() {
    // Update engine value
    if (statusMap.at(forward)) engineValue = qMin(engineValue + 2, 100);
    else if (statusMap.at(backward)) engineValue = qMax(engineValue - 2, 0);
    else engineValue = 50;  // Neutral

    // Update steering
    if (statusMap.at(right)) servoValue = qMax(servoValue - 5, 0);
    else if (statusMap.at(left)) servoValue = qMin(servoValue + 5, 100);
    else servoValue = 50;  // Neutral

    sendPostRequest("/engine", engineValue);
    sendPostRequest("/turn", servoValue);
}

void CarController::sendPostRequest(const QString &endpoint, int value) {
    QUrl url(IP_ADRESS + endpoint);  // Replace with the correct IP
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["value"] = value;

    engineManager->post(request, QJsonDocument(json).toJson());
}
