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

using namespace cv;
using namespace std;

class HeightProcess
{
public:
    HeightProcess(cv::Ptr<GroundPlane> gp);
    double heightBase(Mat input,int fnumber);
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
};

#endif // HEIGHTPROCESS_H
