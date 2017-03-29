#ifndef TOOLS_H
#define TOOLS_H
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "ui_mainwindow.h"
#include <QString>

class tools
{
public:
    tools(Ui::MainWindow *m);

    static void drawText(cv::Mat frame,std::string s,int x,int y);

    //mat
    static cv::Mat key2Mat(std::vector<cv::KeyPoint> k,int h,int w);
    static std::vector<cv::KeyPoint> mat2Key(cv::Mat image);

    //string
    static QString num2str(int x);
    static QString num2str(double x);
    static std::string int2str(int x);
    static std::string catStr(std::string x,std::string y);
    static QImage Mat2QImage(const cv::Mat frame);
    static QPixmap Mat2QPixmap(const cv::Mat frame);
    static double Determinant3D(int a,int b,int c,int d,int e,int f,int g,int h,int i);
    static QString getPath();



    Ui::MainWindow *m;
};

#endif // TOOLS_H
