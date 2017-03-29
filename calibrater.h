#ifndef CALIBRATER_H
#define CALIBRATER_H
#include "settings.h"
#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

#ifndef _CRT_SECURE_NO_WARNINGS
# define _CRT_SECURE_NO_WARNINGS
#endif

using namespace cv;
using namespace std;


class Calibrater
{
public:
    Calibrater();


    static void help();

//    static void read(const FileNode& node, Settings& x, const Settings& default_value);

    enum { DETECTION = 0, CAPTURING = 1, CALIBRATED = 2 };


//    bool runCalibrationAndSave(Settings& s, Size imageSize, Mat&  cameraMatrix, Mat& distCoeffs,
//                               vector<vector<Point2f> > imagePoints );

    int start();


    static double computeReprojectionErrors( const vector<vector<Point3f> >& objectPoints,
                                             const vector<vector<Point2f> >& imagePoints,
                                             const vector<Mat>& rvecs, const vector<Mat>& tvecs,
                                             const Mat& cameraMatrix , const Mat& distCoeffs,
                                             vector<float>& perViewErrors);

    static void calcBoardCornerPositions(Size boardSize, float squareSize, vector<Point3f>& corners,
                                         Settings::Pattern patternType /*= Settings::CHESSBOARD*/);

    static bool runCalibration( Settings& s, Size& imageSize, Mat& cameraMatrix, Mat& distCoeffs,
                                vector<vector<Point2f> > imagePoints, vector<Mat>& rvecs, vector<Mat>& tvecs,
                                vector<float>& reprojErrs,  double& totalAvgErr);

    // Print camera parameters to the output file
    static void saveCameraParams( Settings& s, Size& imageSize, Mat& cameraMatrix, Mat& distCoeffs,
                                  const vector<Mat>& rvecs, const vector<Mat>& tvecs,
                                  const vector<float>& reprojErrs, const vector<vector<Point2f> >& imagePoints,
                                  double totalAvgErr );
    bool runCalibrationAndSave(Settings& s, Size imageSize, Mat&  cameraMatrix, Mat& distCoeffs,vector<vector<Point2f> > imagePoints );

};

#endif // CALIBRATER_H
