// wideostreamwidget.h
#ifndef VIDEOSTREAMWIDGET_H
#define VIDEOSTREAMWIDGET_H

#include <QWidget>
#include <opencv2/opencv.hpp>
#include <QTimer>

class VideoStreamWidget : public QWidget {
    Q_OBJECT

public:
    VideoStreamWidget(QWidget *parent = nullptr);
    ~VideoStreamWidget();

private slots:
    void updateFrame();

private:
    std::string ipAddress;
    cv::VideoCapture cap;
    QTimer *timer;
};

#endif // VIDEOSTREAMWIDGET_H

