#include "display.h"

display::display(Ui::MainWindow *m,std::string axisName)
{
    this->m=m;
    this->axisName=axisName;
    cv::namedWindow(axisName,CV_WINDOW_NORMAL);
    cv::resizeWindow(axisName, 960, 720);
    nonShow=!m->cbDis->isChecked();
}



void display::setImage(cv::Mat im)
{
    this->im=im;
    circleSize=std::ceil(0.0025*im.size().width);
    //draw console


}

void display::drawConsole(int totalEntry,int nowEntry,int fNumber,cv::Ptr<GroundPlane> gp){
    //-----------------------------------Vid 41
        if (fNumber==933){
        cv::Point p(300,263);
        cv::Vec3b red = cv::Vec3b(0,0,255);
        cv::Vec3b yellow =cv::Vec3b(0,255,255);
        for (int i=0;i<im.rows;++i){
            for (int j=0;j<im.cols;++j){
                //Point3d temp=gp->getPointAtGround(Point(j,i));
                double dis=gp->calDis3DFrom2D(Point(p.x,p.y),cv::Point(j,i));


                if (dis<36 ){//dis 36
                    //im.at<cv::Vec3b>(cv::Point(j,i)) = yellow;
                    cv::Mat roi = im(cv::Rect(j,i,1,1));
                    cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(0, 255, 255));
                    double alpha = 0.7;
                    cv::addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi);
                }

            }
        }
        p=cv::Point(453,397);
        for (int i=0;i<im.rows;++i){
            for (int j=0;j<im.cols;++j){
                //Point3d temp=gp->getPointAtGround(Point(j,i));
                double dis=gp->calDis3DFrom2D(Point(p.x,p.y),cv::Point(j,i));


                if (dis<36 ){//dis 36
                //im.at<cv::Vec3b>(cv::Point(j,i)) = yellow;
                cv::Mat roi = im(cv::Rect(j,i,1,1));
                cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(0, 255, 255));
                double alpha = 0.7;
                cv::addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi);
                }
            }
        }


    }
    //--------------------------------------First Vid
//    if (fNumber==1609){
//        cv::Point showPt;
//        showPt.x=376;
//        showPt.y=244;

//        cv::Mat roi = im(cv::Rect(showPt.x>=10?showPt.x-10:0,showPt.y>=20?showPt.y-20:0, showPt.x+125<im.cols?125:im.cols-showPt.x-1, showPt.y+30<im.rows?30:im.rows-showPt.y-1));
//        cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(90, 90, 90));
//        double alpha = 0.7;
//        cv::addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi);
//        putText(im, "Entry ! #4", showPt,
//            CV_FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0,255,255), 1, CV_AA);

//        showPt.x=753;
//        showPt.y=352;

//        roi = im(cv::Rect(showPt.x>=10?showPt.x-10:0,showPt.y>=20?showPt.y-20:0, showPt.x+125<im.cols?125:im.cols-showPt.x-1, showPt.y+30<im.rows?30:im.rows-showPt.y-1));
//        //cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(90, 90, 90));
//        //double alpha = 0.7;
//        cv::addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi);
//        putText(im, "Entry ! #5", showPt,
//            CV_FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0,255,255), 1, CV_AA);

//        nowEntry=2;
//        totalEntry+=1;
//    }

    int consoleX=im.cols-200;
    int consoleY=0;

    cv::Mat roi = im(cv::Rect(consoleX,consoleY, 200, 80));
    cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(90, 90, 90));
    double alpha = 0.85;
    cv::addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi);
    putText(im, "Total Count: "+tools::int2str(totalEntry), cv::Point(consoleX+10,consoleY+25),
        CV_FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0,255,255), 1, CV_AA);

    putText(im, nowEntry>0?"Human Alert! ":"No human", cv::Point(consoleX+10,consoleY+65),
        CV_FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0,nowEntry>0?0:255,nowEntry>0?255:0), 1, CV_AA);



}

void display::drawMask(cv::Mat mask,cv::Scalar color,bool check)
{
    if (check){
        cv::Mat nz;
        cv::findNonZero(mask, nz);
        for (int i = 0; i < nz.total(); i++ ) {
            cv::circle(im,nz.at<cv::Point>(i),circleSize,color,-1);
        }
    }
}

void display::drawTrueMask(cv::Mat mask,cv::Scalar color,bool check){
    if (check){
        for(int j = 0;j < mask.rows;j++){
            for(int i = 0;i < mask.cols;i++){
                if (mask.at<uchar>(i,j)>0)
                    cv::circle(im,cv::Point(j,i),circleSize,color,-1);
            }
        }
    }
}

void display::drawLine(std::vector<cv::KeyPoint> start,std::vector<cv::KeyPoint> curr,cv::Ptr<GroundPlane> gp,bool check){
    if (check){
        int lowestY=-1;
        cv::Point p;
        //loop draw line
        for (int i=0;i<curr.size();i++){
            if (curr.at(i).pt.y>=lowestY){
                p.x=curr.at(i).pt.x;
                p.y=curr.at(i).pt.y;
                lowestY=curr.at(i).pt.y;
            }
            //cv::line(im,curr.at(i).pt,start.at(i).pt,cv::Scalar(0,(start.at(i).size<=0?0:255),255));
        }
        if (lowestY==-1)
            return;
        //draw 3d rad
        cv::Vec3b red = cv::Vec3b(0,0,255);
        cv::Vec3b yellow =cv::Vec3b(0,255,255);
        std::cout<<im.type()<<endl;
        for (int i=0;i<im.rows;++i){
            for (int j=0;j<im.cols;++j){
                //Point3d temp=gp->getPointAtGround(Point(j,i));
                double dis=gp->calDis3DFrom2D(Point(p.x,p.y),cv::Point(j,i));


                if (dis<36 ){//dis 36
                    im.at<cv::Vec3b>(cv::Point(j,i)) = yellow;
                }
    //            if (dis<19 ){//dis 18
    //                im.at<Vec3b>(Point(j,i)) = red;
    //            }
            }
        }
    }
}

void display::drawLineTempCurr(std::vector<std::vector<cv::KeyPoint> > tempCurr,std::vector<cv::KeyPoint> start,bool check){
    if (check){
        for (int i=0;i<tempCurr.size();i++){
            cv::Point lastest=tempCurr.at(i).at(0).pt;

            for (int j=1;j<tempCurr.at(i).size();j+=10){

                cv::line(im,lastest,tempCurr.at(i).at(j).pt,cv::Scalar(0,(start.at(i).size<=0?0:255),255));
                lastest=tempCurr.at(i).at(j).pt;
            }
            cv::line(im,lastest,tempCurr.at(i).at(tempCurr.at(i).size()-1).pt,cv::Scalar(0,(start.at(i).size<=0?0:255),255));
        }


    }
}

void display::drawEntry(int hNum,std::vector<cv::KeyPoint> start,std::vector<cv::KeyPoint> curr,bool human,bool check){
    if (!start.empty() && human){
        int top=480;
        cv::Point showPt;
        for (int i=0;i<start.size();++i){
            if (start.at(i).size>0){
                if (top>curr.at(i).pt.y){
                    top=curr.at(i).pt.y;
                    showPt=curr.at(i).pt;
                }


            }
        }


        showPt.x-=showPt.x>=30?30:0;
        showPt.y-=showPt.y>=10?10:0;

        cv::Mat roi = im(cv::Rect(showPt.x>=10?showPt.x-10:0,showPt.y>=20?showPt.y-20:0, showPt.x+125<im.cols?125:im.cols-showPt.x-1, showPt.y+30<im.rows?30:im.rows-showPt.y-1));
        cv::Mat color(roi.size(), CV_8UC3, cv::Scalar(90, 90, 90));
        double alpha = 0.7;
        cv::addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi);
        putText(im, "Entry ! #"+tools::int2str(hNum), showPt,
            CV_FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0,255,255), 1, CV_AA);




    }
}

void display::drawHeight(cv::Point point,double h){
    putText(im, tools::int2str(h)+" cm", point,
        CV_FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0,255,255), 1, CV_AA);
}

void display::showIm(){
    if (!im.empty() && !nonShow)
    {
        imshow(axisName,im);
        m->frameNum->setText(frameN);
        m->FPSLabel->setText(fps);
        m->BGLabel->setText(bg);
        m->BFLabel->setText(bf);
        m->CurrFLabel->setText(cff);
        m->FFLabel->setText(ff);
        m->SFLabel->setText(sfm);
        m->MFLabel->setText(mfm);
    }
}







//Setter Param number Label
void display::setFrameN(int value)
{
    frameN = QString::fromStdString("Frame# ")+tools::num2str(value);
}
void display::setFps(int value)
{
    fps = QString::fromStdString("FPS: "+tools::int2str(value));
}
void display::setBg(int value)
{
    bg = QString::fromStdString("BGN "+tools::int2str(value));
}
void display::setBf(int value)
{
    bf = QString::fromStdString("BFN "+tools::int2str(value));
}
void display::setCff(int value)
{
    cff = QString::fromStdString("Current Feature: "+tools::int2str(value));
}
void display::setFf(int value)
{
    ff = QString::fromStdString("Foreground Feature: "+tools::int2str(value));
}
void display::setSfm(int value)
{
    sfm = QString::fromStdString("SFM: "+tools::int2str(value));
}
void display::setMfm(int value)
{
    mfm = QString::fromStdString("MFM: "+tools::int2str(value));
}









