#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    vidUIChange(false);


}

MainWindow::~MainWindow()
{
    delete ui;
}


//choose Button
void MainWindow::on_choose_clicked()
{

    //File Chooser
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Image"),tools::getPath() , tr("Image Files (*.mp4 *.avi *.wmv)"));
    std::string fileNameStr = fileName.toUtf8().constData();//cast to string

    if (fileNameStr.compare("")){
        //Get Video Source
        cv::VideoCapture vid(fileNameStr);

        vidUIChange(true);
        //reCalculate R and D for resolution
        this->h=vid.get(CV_CAP_PROP_FRAME_HEIGHT);//bug with wmv
        this->framerate=vid.get(CV_CAP_PROP_FPS);
        on_dis_textChanged(ui->dis->text());
        //create function process video

        vp=new videoProcess(ui,vid);
//        tools::setWH(vid.get(CV_CAP_PROP_FRAME_WIDTH),vid.get(CV_CAP_PROP_FRAME_HEIGHT));
        //process
        vp->process();

        vidUIChange(false);

    }
}

void MainWindow::on_stop_clicked()
{
    vidUIChange(false);
    vp->stopProcess();
    cv::destroyWindow("axis1");
}

void MainWindow::on_pp_clicked()
{
    vp->pause();
}

void MainWindow::on_s_clicked()
{
    vp->step();
}


//UI control
void MainWindow::vidUIChange(bool run){
    if (run){
        //set Enable/Disable Button
        ui->choose->setEnabled(false);
        ui->stop->setEnabled(true);
        ui->s->setEnabled(true);
        ui->pp->setEnabled(true);

        //Change UI size and location
        this->setFixedSize(400,600);
        move(1100,200);

    }else{
        //set Enable/Disable Button
        ui->choose->setEnabled(true);
        ui->stop->setEnabled(false);
        ui->s->setEnabled(false);
        ui->pp->setEnabled(false);

        //Change UI size and location
        this->setFixedSize(800,600);
        move(200,200);
    }

}


void MainWindow::on_dis_textChanged(const QString &arg1)
{
    if (!arg1.isEmpty()){
        int dis=arg1.toInt();
        if (framerate==0)
            framerate=30;

        float fSpeedH=10.2312;
        float R=(h/dis)*(fSpeedH*1000/(3600*framerate));
        float D=2*R;
        ui->r->setText(tools::num2str(round(R)));
        ui->d->setText(tools::num2str(round(D)));
    }
}


void MainWindow::on_cam_clicked()
{
    cv::VideoCapture vid(ui->tCID->text().toInt());

    vidUIChange(true);
    //reCalculate R and D for resolution
    this->h=vid.get(CV_CAP_PROP_FRAME_HEIGHT);
    this->framerate=vid.get(CV_CAP_PROP_FPS);
    on_dis_textChanged(ui->dis->text());
    //create function process video

    vp=new videoProcess(ui,vid);
//        tools::setWH(vid.get(CV_CAP_PROP_FRAME_WIDTH),vid.get(CV_CAP_PROP_FRAME_HEIGHT));
    //process
    vp->process();

    vidUIChange(false);
}



