#include "cameradevice.h"
#include <QTimer>
#include <QImage>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

/**
 * @brief CameraDevice::CameraDevice
 * @param parent
 * Constructor
 */
CameraDevice::CameraDevice(QObject *parent) :
    QObject(parent)
{
    m_capture = new cv::VideoCapture;
    m_timer = new QTimer(this);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

/**
 * @brief CameraDevice::~CameraDevice
 * Destructor
 */
CameraDevice::~CameraDevice()
{
    delete m_capture;
    m_capture = NULL;
}

/**
 * @brief CameraDevice::start
 * @return bool
 * Opens the capturing device
 */
bool CameraDevice::start()
{
    if (m_capture->isOpened())
        return true;

    if(src.length() <=1){
        m_capture->open(CV_CAP_ANY);
    } else {
        m_capture->open(src.toStdString().c_str());
    }

    if (m_capture->isOpened())
        m_timer->start(33);

    return m_capture->isOpened();
}

/**
 * @brief CameraDevice::stop
 * @return bool
 * closes the capturing device
 */
bool CameraDevice::stop()
{
    if (m_capture->isOpened())
        m_capture->release();

    return true;
}

/**
 * @brief CameraDevice::onTimeout
 * Captures a frame every time the timer has a timeout
 */
void CameraDevice::onTimeout()
{
    if (!m_capture->isOpened())
        return;
    cv::Mat out;
    static cv::Mat frame;
    *m_capture >> frame;

    if (frame.cols)
        emit imageReady(frame);
}

/**
 * @brief CameraDevice::setSource
 * @param fileName
 * Sets the source file for a video or sets "0" to use the camera
 */
void CameraDevice::setSource(QString fileName){
    src = fileName;
}
