
#ifndef DIALOGRANGE_H
#define DIALOGRANGE_H

#include <QDialog>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <QFileDialog>
#include "tools.h"
#include "calibrater.h"
#include <opencv2/features2d.hpp>
#include "groundplane.h"


namespace Ui {
class DialogRange;
}

class DialogRange : public QDialog
{
    Q_OBJECT

public:
    void setImageMat(cv::Mat im,VideoCapture vid,Ptr<GroundPlane> gp,bool *p);



    Calibrater ccc;
    VideoCapture vid;

    explicit DialogRange(QWidget *parent = 0);
    ~DialogRange();


    cv::Mat A,B,xx,cm,dc,cf;
    void ReadCameraMatrix();
    //plane Equation

    Ptr<GroundPlane> gp;

    void draw3DRadius(QPoint p);
    void showPointRadius(QPoint p);

private slots:
    void on_cdButton_clicked();

    void on_capButton_clicked();

    void on_lcpBut_clicked();

    void on_ccBut_clicked();

    void on_scpBut_clicked();

    void on_fmBut_clicked();

    void on_fcBut_clicked();

    void on_lgpBut_clicked();

    void on_sgpBut_clicked();

    void on_lspBut_clicked();

    void on_sspBut_clicked();

    void on_buttonBox_accepted();



private:
    Ui::DialogRange *ui;

    void setStatusLb(const string x,bool success);

    bool *p;
    Mat im;
};

#endif // DIALOGRANGE_H
