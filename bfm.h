#ifndef BFM_H
#define BFM_H
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "ui_mainwindow.h"
#include "tools.h"
#include <opencv2/features2d.hpp>
#include <display.h>

class BFM
{
public:
    BFM(int w,int h,int bg,int bf,cv::Ptr<display> d);
    std::vector<cv::KeyPoint> BGM;
    std::vector<std::vector<cv::KeyPoint> > BG;
    std::vector<std::vector<cv::KeyPoint> > BF;
    cv::Mat result,Foreground;

    void calBGM(std::vector<std::vector<cv::KeyPoint> > BG);
    void setNumber(int bg,int bf);
    void add(std::vector<cv::KeyPoint> f,int n,bool human);
    void releaseGap();
    cv::Mat getForeground(cv::Mat currMat);
    int cff,fff;


private:
    int bgn;
    int bfn;
    int n;
    int h,w;

    cv::Ptr<display> d;
    Ui::MainWindow *m;
};

#endif // BFM_H
