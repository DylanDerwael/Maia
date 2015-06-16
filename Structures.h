#ifndef STRUCTURS
#define STRUCTURS

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <tbb/flow_graph.h>
#include <opencv2/core/mat.hpp>
#endif // STRUCTURS

typedef tbb::flow::tuple<cv::Mat, cv::Mat> imagesJoin2;
typedef tbb::flow::tuple<cv::Mat, cv::Mat, cv::Mat> imagesJoin3;

typedef tbb::flow::tuple<bool, bool, bool> joinForSigma;
typedef tbb::flow::tuple<bool, bool> joinForDisplay;

////low brighness config
const int redLowerBound = 10;
const int redUpperBound = 225;
const int blueUpperBound = 20;
const int greenUpperBound = 20;
const double greenSaturationLevel = 0.4;
const double blueSaturationLevel = 0.4;


////high brightness config
//const int redLowerBound = 10;
//const int redUpperBound = 255;
//const int blueUpperBound = 70;
//const int greenUpperBound = 70;
//const double greenSaturationLevel = 0.2;
//const double blueSaturationLevel = 0.2;

/**
 * @brief The _gray struct
 * Returns a grayscale of the image
 */
struct _gray {
  cv::Mat operator()(cv::Mat in) {
      cv::Mat out;
      cv::cvtColor(in, out, CV_BGR2GRAY);
      return out;}
};

/**
 * @brief The _red struct
 * Returns only the red channel of the image
 */
struct _red {
    cv::Mat operator()(cv::Mat in){
        cv::Mat channels[3];
        cv::Mat out;
        cv::split(in,channels);
        out = channels[2];
        return out;
    }
};

/**
 * @brief The _green struct
 * Returns only the green channel of the image
 */
struct _green {
    cv::Mat operator()(cv::Mat in){
        cv::Mat channels[3];
        cv::Mat out;
        cv::split(in,channels);
        out = channels[1];
        return out;
    }
};

/**
 * @brief The _blue struct
 * Returns only the blue channel of the image
 */
struct _blue {
    cv::Mat operator()(cv::Mat in){
        cv::Mat channels[3];
        cv::Mat out;
        cv::split(in,channels);
        out = channels[0];
        return out;
    }
};

/**
 * @brief The _toHSV struct
 * Converts the image to the HSV color spectrum and returns it
 */
struct _toHSV{
    cv::Mat operator()(cv::Mat in){
        cv::Mat out;
        cv::cvtColor(in,out,CV_BGR2HSV);
        return out;
    }
};

/**
 * @brief The _blood struct
 * looks for the pixels that are considerd to be blood and returns a black and white where white is the pixels found with blood
 */
struct _blood {
    cv::Mat operator() (cv::Mat in) {

        cv::Mat channel[3];
        cv::Mat out, red, green, blue;
        cv::split(in,channel);

        //variables to make the code easier to read
        red = channel[2];
        green = channel[1];
        blue = channel[0];

        // Filter on RGB range
        cv::Mat redRange = (red > redLowerBound) & (red < redUpperBound);
        cv::Mat greenRange = (green < greenUpperBound);
        cv::Mat blueRange =  (blue < blueUpperBound);

        // Filter on ratio between colors channels
        cv::Mat saturationGreen = ((green/red) < greenSaturationLevel);
        cv::Mat saturationBlue = ((blue/red) < blueSaturationLevel);

        // If all conditions are met set pixel to 1 if not 0
        out = redRange & greenRange & blueRange & saturationGreen & saturationBlue;

        // Filter out noise
        cv::erode(out,out,cv::Mat());
        cv::erode(out,out,cv::Mat());

        return out;
    }
};

struct _standardDeviation{
    cv::Mat operator()(cv::Mat in){
        cv::Mat out, gray;

        cv::cvtColor(in,gray, CV_BGR2GRAY);
        cv::Mat image32f;
        gray.convertTo(image32f, CV_32F);

        cv::Mat mu;
        cv::blur(image32f, mu, cv::Size(3, 3));

        cv::Mat mu2;
        cv::blur(image32f.mul(image32f), mu2, cv::Size(3, 3));

        cv::Mat sigma;
        cv::sqrt(mu2 - mu.mul(mu), sigma);

        sigma.convertTo(out, CV_8UC1);

        threshold(out,out,30,170,CV_THRESH_BINARY);

        cv::morphologyEx(out,out,CV_MOP_CLOSE,cv::Mat(), cv::Point(-1,-1),3);

        return out;
    }
};

struct _sigmaCalc{
    cv::Mat operator()(cv::Mat mu, cv::Mat mu2){
        cv::Mat sigma, out;
        cv::sqrt(mu2 - mu.mul(mu), sigma);

        sigma.convertTo(out, CV_8UC1);

        threshold(out,out,30,170,CV_THRESH_BINARY);

        cv::morphologyEx(out,out,CV_MOP_CLOSE,cv::Mat(), cv::Point(-1,-1),3);

        return out;
    }
};

struct _muCalc{
    cv::Mat operator()(cv::Mat in){
        cv::Mat mu;
        cv::blur(in, mu, Size(3, 3));
        return mu;
    }
};

struct _mu2Calc{
    cv::Mat operator()(cv::Mat in){
        cv::Mat mu2;
        cv::blur(in.mul(in), mu2, Size(3, 3));
        return mu2;
    }
};

struct _convertToGray{
    cv::Mat operator()(cv::Mat in){
        cv::Mat out, gray;

        cv::cvtColor(in,gray, CV_BGR2GRAY);
        gray.convertTo(out, CV_32F);

        return out;
    }
};


struct _whiteElements{
    cv::Mat operator()(cv::Mat in){
        cv::Mat out, red, green, blue;

        cv::Mat channel[3];
        cv::split(in,channel);

        red = channel[2];
        green = channel[1];
        blue = channel[0];

        cv::Mat rangeBlueMinGreen = ((blue - green) < 15) & ((blue - green) > -15) & (green>50);
        cv::Mat rangeMinBlue = (blue>50);
        cv::Mat rangeRedMinBlueAndGreen = ((red-blue)<100)&((red- green)<100);

        out = rangeBlueMinGreen & rangeMinBlue & rangeRedMinBlueAndGreen;

        cv::morphologyEx(out,out,CV_MOP_CLOSE,cv::Mat(), cv::Point(-1,-1),1);
        cv::morphologyEx(out,out,CV_MOP_OPEN,cv::Mat(), cv::Point(-1,-1),1);

        return out;
    }
};

struct _combineTextureAndColor{
    cv::Mat operator()(cv::Mat texture, cv::Mat color){
        cv::Mat out;

        cv::bitwise_and(texture, color, out);

        return out;
    }
};




