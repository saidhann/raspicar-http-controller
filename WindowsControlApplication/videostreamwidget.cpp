#include "videostreamwidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QImage>
#include <QPainter>
#include <QDebug>  // Include for QDebug

VideoStreamWidget::VideoStreamWidget(QWidget *parent)
    : QWidget(parent), ipAddress("tcp://192.168.137.25:8554"), timer(new QTimer(this)) {
    // Setup layout

    setStyleSheet("VideoStreamWidget {"
                  "border-radius: 20px;"
                  "background-color: black;"
                  "}");

    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel("Video Stream", this);
    layout->addWidget(label);

    // Start video stream
    if (!cap.open(ipAddress)) {
        // Log error using QDebug
        qDebug() << "Error: Unable to open video stream at" << QString::fromStdString(ipAddress);
        label->setText("Error: Unable to open video stream at " + QString::fromStdString(ipAddress));
        return; // Exit the constructor if the stream cannot be opened
    }

    // Set up timer to update the frame
    connect(timer, &QTimer::timeout, this, &VideoStreamWidget::updateFrame);
    timer->start(30); // Update every 30 ms
}

VideoStreamWidget::~VideoStreamWidget() {
    cap.release();
}

void VideoStreamWidget::updateFrame() {
    cv::Mat frame;
    // Attempt to read a frame from the video stream
    if (cap.read(frame)) {
        // Convert OpenCV frame to RGB format (OpenCV default is BGR)
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

        // Create QImage from the frame
        QImage image = QImage(frame.data, frame.cols, frame.rows, frame.step[0], QImage::Format_RGB888);

        // Convert the QImage to QPixmap
        QPixmap pixmap = QPixmap::fromImage(image);

        // Specify maximum dimensions for scaling, instead of using widget size directly
        int maxWidth = 800;  // Or whatever size you want
        int maxHeight = 600;

        // Scale the image to fit within the widget's max dimensions
        pixmap = pixmap.scaled(maxWidth, maxHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // Find the QLabel (or another widget) to display the image
        QLabel *label = findChild<QLabel *>();
        if (label) {
            label->setPixmap(pixmap);
        }
    } else {
        // Log error if the frame cannot be read
        qDebug() << "Error: Stream ended unexpectedly or cannot read frame.";
    }
}




