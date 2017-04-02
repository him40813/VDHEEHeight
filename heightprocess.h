#ifndef HEIGHTPROCESS_H
#define HEIGHTPROCESS_H

#include "groundplane.h"
#include "tools.h"
#include <vector>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <exLib/package_bgs/sjn/SJN_MultiCueBGS.h>
#include <QThread>
#include <QtCore>
#include <QMutex>

using namespace cv;
using namespace std;

class HeightProcess : public QThread
{
public:
    QMutex mutex;
    HeightProcess();
    HeightProcess(cv::Ptr<GroundPlane> gp);
    double heightBase(Mat input,int fnumber,cv::Ptr<GroundPlane> gp);
    cv::Ptr<GroundPlane> gp;
    IBGS *bgs;
    int calPeakBin(vector<int> heightHis,double percentage);
    int HDPAV;
    Mat edgeBg;
    Mat roiChecker,roiColor;
    int contCounter;
    double contTemp;
    int xAreaSize,yAreaSize;
    vector<int> heightHis;
    cv::Mat input,output, output_bkgmodel;

    void run();
    bool finished;
    Mat inputT;
    double resultT;
    int fnumberT,numfT;
    cv::Point tpT;
};

#endif // HEIGHTPROCESS_H
