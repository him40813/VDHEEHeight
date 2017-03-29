#ifndef GROUNDPLANE_H
#define GROUNDPLANE_H

#include <opencv2/features2d.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

using namespace cv;
using namespace std;

class GroundPlane
{
public:


    Mat cm,dc,RTvec;

    double a,b,c,d;

    double ia,ib,ic,id;

    GroundPlane();

    void readCameraMatrix();

    bool readCameraMatrix(const string filename);

    vector<Point2f> findCircle(Mat im);

    void calPlaneEquation();

    void calImagePlaneEquation();

    Point3d imgCord2CameraCord(Point2d imgCord);

    Point camCord2ImgCord(Point3d camCord);

    Point3d getPointAtGround(Point2d imgCord);

    Point getPointAtImageFrom3D(Point3d camCord);

    //New-------
    Point calPerpenTopPoint(Point lowest);

    Point calTopPoint(Point lowest,Point Heighest,double &high);

    Point calTopPointNonPerpendicular(Point lowest,Point Heighest,double &highV2,double &distanecRN,double &farCV2);

    double lm,lm2,lb,lb2;

    //2D on Image Line (create line and find perpendicular from point)

    void findMB4Line(Point a,Point b);

    void calPerpendicular(Point xxx);

    Point intersectionMB();

    //3D line for finding intersection of norm and top point

    Point3d pointIntersecNorm3D(Point topP,Point3d l,Point3d h);
    Point3d pointIntersecNorm3DRay(Point topP,Point3d l,Point3d h,double &distanceRN);

    //---------

    double calDis3D(Point3d a,Point3d b);

    double calDis3DFrom2D(Point2d a,Point2d b);

    bool saveGroundParam();

    bool saveGroundParam(const string x);

    bool readGroundParam();

    bool readGroundParam(const string x);

    bool writeSetting(const string x);

    vector<Point2f> corners;

    Point calTestHigh(Point l);

    Point3d calTestHighV3d(Point lowest);

    double dot(Point3d a,Point3d b);

    void simulateGroundPlane(double alpha,double dist,Mat im);

    void undistortCM(Mat src);

private:
    int eachDis,size1,size2;


    Mat rvec,tvec;
};

#endif // GROUNDPLANE_H
