#include "tools.h"



tools::tools(Ui::MainWindow *m)
{
    this->m=m;
}

std::string tools::int2str(int x){
    std::stringstream ss;
    ss << x;
    return ss.str();
}

QString tools::num2str(int x){
    return QString::number(x);
}

QString tools::num2str(double x){
    return QString::number(x);
}

std::string tools::catStr(std::string x,std::string y){
    std::stringstream ss;
    ss << x << y;
    return ss.str();
}

void tools::drawText(cv::Mat frame,std::string s,int x,int y){
    putText(frame,s, cv::Point(x,y),
        CV_FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255,255,255), 1, CV_AA);
}

cv::Mat tools::key2Mat(std::vector<cv::KeyPoint> fk,int h,int w){
    cv::Mat result=cv::Mat(h,w,CV_8UC1,cv::Scalar(0));
    for(int k=0;k<fk.size();k++){
        cv::KeyPoint key=fk.at(k);
        result.at<uchar>(cv::Point(key.pt.x,key.pt.y))=1;
    }
    return result;
}

std::vector<cv::KeyPoint> tools::mat2Key(cv::Mat image){
    std::vector<cv::KeyPoint> rs;
    cv::Mat nz;
    cv::findNonZero(image, nz);
    for (int i = 0; i < nz.total(); i++ ) {
        rs.push_back(cv::KeyPoint(nz.at<cv::Point>(i).x,nz.at<cv::Point>(i).y,1));
    }
    return rs;
}

QImage tools::Mat2QImage(const cv::Mat frame) {
    QImage img;
    cv::Mat RGBframe;
    if (frame.channels()== 3){
        cv::cvtColor(frame, RGBframe,CV_BGR2RGB);
        img = QImage((const unsigned char*)(RGBframe.data),
                          RGBframe.cols,RGBframe.rows,QImage::Format_RGB888);
        img.bits();
    }
    else
    {
        img = QImage((const unsigned char*)(frame.data),
                             frame.cols,frame.rows,QImage::Format_Indexed8);
    }
    return img;
}

QPixmap tools::Mat2QPixmap(const cv::Mat frame){
    return QPixmap::fromImage(Mat2QImage(frame));
}

double tools::Determinant3D(int a,int b,int c,int d,int e,int f,int g,int h,int i){
    return a*(e*i-f*h)-b*(d*i-f*g)+c*(d*h-e*g);
}

QString tools::getPath(){
    QString path="C:\\Users\\HHM-MSI\\Videos\\TC3+FW";
    return path;
}
