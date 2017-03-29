#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <QThread>
#include "videoProcess.h"
#include "bfm.h"
#include "math.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    Ui::MainWindow *ui;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setFrame(int n);

private slots:
    void on_cam_clicked();

    void on_choose_clicked();
    void vidUIChange(bool run);

    void on_stop_clicked();

    void on_pp_clicked();

    void on_s_clicked();

    void on_dis_textChanged(const QString &arg1);

private:

    videoProcess *vp;
    int h,framerate;


};

#endif // MAINWINDOW_H
