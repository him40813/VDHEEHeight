#ifndef VIDEOPROCESS_H
#define VIDEOPROCESS_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <exLib/package_bgs/sjn/SJN_MultiCueBGS.h>
#include <iostream>
#include "bfm.h"
#include "tools.h"
#include "ui_mainwindow.h"
#include <QtConcurrent/QtConcurrent>
#include <QThread>
#include <time.h>
#include <display.h>
#include "ffm.h"
#include "dialogrange.h"
#include "groundplane.h"
#include "heightprocess.h"
#include <QFileDialog>
#include <fstream>
#include <QObject>
#include <QStringList>


#include <vector>
#include <algorithm>
#include <QThread>


using namespace cv;
using namespace std;

class videoProcess
{
public:


    videoProcess(Ui::MainWindow *m,cv::VideoCapture v);
    void setVid(cv::VideoCapture v);
    int base();
    void process();
    void stopProcess();
    void pause();
    void step();
    DialogRange dr;
    bool readCheckingFile(const string x);
    void saveMatToCsv(const Mat &matrix,const string filename);
    void saveMatToCsvDouble(const Mat &matrix,const string filename);
    bool endOfVid;



protected:
    cv::VideoCapture vid;


private:
    int cIn,countIn,cOut;
    bool stop;
    bool p;
    bool s;
    bool human;
    bool setupRange;
    bool endOfTest;

    cv::Mat frame,realFrame,FF;
    cv::Mat checkFile,exel1,exel2,exel3,exelStat,exel4,exel5,exel6;
    std::vector<cv::KeyPoint> curr;
    Ui::MainWindow *m;
    cv::Ptr<BFM> b;
    cv::Ptr<FFM> ffm;
    cv::Ptr<HeightProcess> hp;
    cv::Ptr<GroundPlane> gp;
    cv::Ptr<display> d;
    std::vector<int> checkList,testCase;
    int h;
    int w;
    int n;//number of frame
    int cin,cout;
    int testType;
    bool hfinished;
    QFuture<double> future;
    HeightProcess hhhp;
    double heightCal();


    // height trans

};

#endif // VIDEOPROCESS_H
