#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

#include "cameradevice.h"
#include "ConvertMatToGui.h"
#include "Structures.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <tbb/tbb.h>
#include <tbb/flow_graph.h>
#include <tbb/task.h>


namespace Ui {
class MainWindow;
}

//class CameraDevice;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionCamera_toggled(bool arg1);
    void on_actionVideo_toggled(bool arg1);
    void onImageArrival(const cv::Mat & image);


signals:
    void videoSource(QString fileName);

private:
    Ui::MainWindow *ui;
    void alternateSource(bool);
    CameraDevice * m_camera;
    void startSending(QString x);
    int concurrency = tbb::flow::unlimited;
    int serial = tbb::flow::serial;
    double counter =0;
    double frameCounter =0;

    cv::Mat channel[3];
    cv::Mat red, green, blue, blood, frame, gray, bandageColor;
    cv::Mat mu, mu2, sigma;
    cv::Mat image32f;
    cv::Mat bandage;

};

#endif // MAINWINDOW_H
