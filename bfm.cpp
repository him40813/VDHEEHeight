 #include "bfm.h"

BFM::BFM(int w,int h,int bg, int bf,cv::Ptr<display> d)
{
    this->bgn=bg;
    this->bfn=bf;
    this->d=d;
    this->h=h;
    this->w=w;
    this->result=cv::Mat(h,w,CV_16UC1,cv::Scalar(0));

}

void BFM::setNumber(int bg, int bf){
    this->bgn=bg;
    this->bfn=bf;


}



void BFM::add(std::vector<cv::KeyPoint> f,int n,bool human){
    if (human)
        return;
    this->n=n;
    if (n<bgn+bfn){//Install Background Model
        if (n<bgn)
            BG.push_back(f);
        else
            BF.push_back(f);

    }else{//Common loop
        BF.push_back(f);
        BG.push_back(BF.at(0));
        BF.erase(BF.begin());
        BG.erase(BG.begin());
        calBGM(BG);

    }


}

void BFM::calBGM(std::vector<std::vector<cv::KeyPoint> > BG){
    result=0;
    for(int i=0;i<BG.size();i++){
        std::vector<cv::KeyPoint> fk=BG.at(i);
        for(int k=0;k<fk.size();k++){
            result.at<uchar>(fk.at(k).pt)+=1;
        }
    }


}

 cv::Mat BFM::getForeground(cv::Mat currMat){
    Foreground=currMat.clone();
    cv::Mat nz;
    //display Label currF
    cff=cv::countNonZero(currMat);
    if (n>=bgn+bfn){
        //Find nonzero pixel on currimage

        cv::findNonZero(currMat,nz);
        for (int i = 0; i < nz.total(); i++ ) {
            if (result.at<uchar>(nz.at<cv::Point>(i))>=1)
                Foreground.at<uchar>(nz.at<cv::Point>(i))=0;
        }

        fff=cv::countNonZero(currMat);
        return Foreground;
    }
    return cv::Mat();
 }

 void BFM::releaseGap(){

        std::copy(BG.begin(),BG.begin()+bfn,BF.begin());
 }


