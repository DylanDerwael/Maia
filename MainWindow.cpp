#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <sys/time.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_camera(new CameraDevice(this))

{
    ui->setupUi(this);

    // sets the camera as default
    alternateSource(true);


    connect(m_camera, SIGNAL(imageReady(cv::Mat)), this, SLOT(onImageArrival(cv::Mat)));
    connect(ui->actionStart, SIGNAL(triggered()), m_camera, SLOT(start()));
    connect(ui->actionStop, SIGNAL(triggered()), m_camera, SLOT(stop()));
    connect(this, SIGNAL(videoSource(QString)), m_camera, SLOT(setSource(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::on_actionCamera_toggled
 * @param cameraOn
 * Selects the camera as Source in the capture object
 */
void MainWindow::on_actionCamera_toggled(bool cameraOn)
{
    alternateSource(cameraOn);
    if(cameraOn){
       emit videoSource("0");
    }
}

/**
 * @brief MainWindow::on_actionVideo_toggled
 * @param cameraOn
 * Opens a File chooser and gives the selected file to the capture object
 */
void MainWindow::on_actionVideo_toggled(bool cameraOn)
{
    alternateSource(!cameraOn);
    if(cameraOn){
        QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "../..",
        tr("Image Files (*.mp4)"));

        emit videoSource(fileName);
    }
}

/**
 * @brief MainWindow::alternateSource
 * @param cameraOn
 * Switches the checked mark between the two source options
 */
void MainWindow::alternateSource(bool cameraOn){
    ui->actionCamera->setChecked(cameraOn);
    ui->actionVideo->setChecked(!cameraOn);
}

/**
 * @brief MainWindow::onImageArrival
 * @param image
 * When an image arrives the Intell TBB graph will be created and executed
 */
void MainWindow::onImageArrival(const cv::Mat &image)
{

    frame = image;

    ui->lblStream->setPixmap(MatToQPixMap(image));
/////Bool switch

    tbb::flow::graph g;

    tbb::flow::broadcast_node<bool> broadcast(g);


    tbb::flow::function_node<bool, bool> splitColorChannels (g, 1, [this]( const bool x) {

            cv::split(frame,channel);
            red = channel[2];
            green = channel[1];
            blue = channel[0];

            cv::cvtColor(frame,gray, CV_BGR2GRAY);
            gray.convertTo(image32f, CV_32F);

            return x;
        });

    tbb::flow::function_node<bool, bool> detectBlood(g, 1, [this](const bool x){

            cv::Mat out;
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
            cv::erode(out,blood,cv::Mat());


            ui->lblDetectionBlood->setPixmap(MatToQPixMap(blood));

            return x;
        });

    tbb::flow::function_node<bool, bool> detectBandageColor(g, 1, [this](const bool x){
        cv::Mat out;
        cv::Mat rangeBlueMinGreen = ((blue - green) < 15) & ((blue - green) > -15) & (green>50);
        cv::Mat rangeMinBlue = (blue>50);
        cv::Mat rangeRedMinBlueAndGreen = ((red-blue)<100)&((red- green)<100);

        out = rangeBlueMinGreen & rangeMinBlue & rangeRedMinBlueAndGreen;

        cv::morphologyEx(out, out,CV_MOP_CLOSE,cv::Mat(), cv::Point(-1,-1),1);
        cv::morphologyEx(out, bandageColor,CV_MOP_OPEN,cv::Mat(), cv::Point(-1,-1),1);

            return x;
        });

    tbb::flow::function_node<bool, bool> calcMu (g, 4, [this](const bool x){

            cv::blur(image32f, mu, cv::Size(3, 3));

            return x;
        });

    tbb::flow::function_node<bool, bool> calcMu2(g, 4, [this](const bool x){

            cv::blur(image32f.mul(image32f), mu2, cv::Size(3, 3));

            return x;
        });

    tbb::flow::join_node<joinForSigma> sigmaJoin(g);

    tbb::flow::function_node<joinForSigma, bool> calcSigma(g, 2, [this](const joinForSigma x){
            const bool x1 = tbb::flow::get<0>(x);
            const bool x2 = tbb::flow::get<1>(x);
            const bool x3 = tbb::flow::get<2>(x);

            cv::Mat out, combineTextAndColor;

            if(x1 & x2 & x3){
                cv::sqrt(mu2 - mu.mul(mu), sigma);

                threshold(sigma,out,30,170,CV_THRESH_BINARY);

                cv::morphologyEx(out,out,CV_MOP_CLOSE,cv::Mat(), cv::Point(-1,-1),3);
                out.convertTo(sigma, CV_8UC1);

                cv::bitwise_and(sigma, bandageColor, combineTextAndColor);

                    int largest_area=0;
                        int largest_contour_index=0;

                        vector< vector<Point> > contours; // Vector for storing contour
                        vector<Vec4i> hierarchy;

                        findContours( combineTextAndColor, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE ); // Find the contours in the image

                        for( int i = 0; i< contours.size(); i++ ) {// iterate through each contour.
                            double a=contourArea( contours[i],false);  //  Find the area of contour
                            if(a>largest_area){
                                largest_area=a;
                                largest_contour_index=i;                //Store the index of largest contour
                                //bounding_rect=boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
                            }
                        }

                        drawContours( frame, contours, largest_contour_index, Scalar(255), CV_FILLED, 8, hierarchy ); // Draw the largest contour using previously stored index.

                        ui->lblDetectionBandage->setPixmap(MatToQPixMap(frame));

                return true;
            } else {
                return false;
            }

        });

    tbb::flow::join_node<joinForDisplay> displayJoin(g);


    tbb::flow::function_node<joinForDisplay, bool> display(g, 1, [this](const joinForDisplay x){
            ui->lblStream->setPixmap(MatToQPixMap(frame));
            ui->lblDetectionBlood->setPixmap(MatToQPixMap(blood));

            return true;
        });

    tbb::flow::make_edge(broadcast, splitColorChannels);
    tbb::flow::make_edge(splitColorChannels, detectBlood);
    tbb::flow::make_edge(splitColorChannels, calcMu);
    tbb::flow::make_edge(splitColorChannels, calcMu2);
    tbb::flow::make_edge(splitColorChannels, detectBandageColor);
    tbb::flow::make_edge(calcMu, tbb::flow::get<0>(sigmaJoin.input_ports()));
    tbb::flow::make_edge(calcMu2, tbb::flow::get<1>(sigmaJoin.input_ports()));
    tbb::flow::make_edge(detectBandageColor, tbb::flow::get<2>(sigmaJoin.input_ports()));
    tbb::flow::make_edge(sigmaJoin, calcSigma);

//    tbb::flow::make_edge(detectBlood, tbb::flow::get<0>(displayJoin.input_ports()));
//    tbb::flow::make_edge(calcSigma, tbb::flow::get<1>(displayJoin.input_ports()));

//    tbb::flow::make_edge(displayJoin, display);

    broadcast.try_put(true);
    g.wait_for_all();

}
