#ifndef CONVERTMATTOQIMAGE_H
#define CONVERTMATTOQIMAGE_H


// Qt
#include <QtGui/QImage>
#include <QtGui/QPixmap>
// OpenCV
//#include <opencv/cv.h>
//#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;

QImage MatToQImage(const Mat&);

QPixmap MatToQPixMap(const Mat&);

cv::Mat QImageToMat(const QImage&);
#endif // CONVERTMATTOQIMAGE_H
