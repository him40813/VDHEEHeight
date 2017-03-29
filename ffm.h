#ifndef FFM_H
#define FFM_H

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "ui_mainwindow.h"
#include "tools.h"
#include <opencv2/features2d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>

#include <display.h>
#include "groundplane.h"

class FFM
{
public:
    int matchType,calDisType,maxMovingDistance;
    FFM(cv::Ptr<display> d,int r,int dis,int coutz,cv::Ptr<GroundPlane> gp);
    void process(cv::Mat ff,cv::Mat frame);
    int getMFCount();
    int getSFCount();
    int findNearest(cv::Point x,cv::Mat nz);
    int findNearestMatch(int rowNum,Mat nz);
    bool updateForeground(int i,int nea);
    double calDis(cv::Point xx1,cv::Point xx2);
    double calDis2d(cv::Point xx1,cv::Point xx2);
    void addNewSFF(cv::Mat nz);
    Mat extractKeyPoint(std::vector<cv::KeyPoint> kp);
    Mat extractMatKeyPoint(Mat kp);
    Mat extractCurrentKeyPointDynamic(std::vector<cv::KeyPoint> kp);
    double calculateMatchDis(Mat pointDes1,Mat pointDes2);
    void deleteFF();
    void clearNSort();
    std::vector<int> ffUsed,mf,del,nzUsed;
    std::vector<cv::KeyPoint> start,curr;
    vector<int> dynamicMatch();
    vector<int> dynamicMatchV2();
    cv::Mat frame,currDes,nzDes;
    std::vector< DMatch > matches;
    std::vector<std::vector< DMatch > > mmm;
    int cResist,mResist,numMF=0;
    double R,D;
    cv::Ptr<display> d;
    Mat nz;
    cv::Ptr<GroundPlane> gp;
    double calDis3D(cv::Point xx1,cv::Point xx2);
    vector<vector<KeyPoint> > tempCurr;

    Ptr<xfeatures2d::SURF> surf;

};

#endif // FFM_H
