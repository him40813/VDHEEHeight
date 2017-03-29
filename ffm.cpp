#include "ffm.h"

FFM::FFM(cv::Ptr<display> d,int r,int dis,int coutz,cv::Ptr<GroundPlane> gp)
{
    this->gp=gp;
    this->d=d;
    mResist=coutz;
    cResist=2;
    R=r;
    D=dis;

    calDisType=1;
    matchType=1;
    surf = xfeatures2d::SURF::create(500); // note extra namespace
    maxMovingDistance=-1;

}

void FFM::process(cv::Mat ff,cv::Mat frame){
    //check if no any current position
    cv::findNonZero(ff,nz);
    this->frame=frame;
    if (curr.size()==0 && !nz.empty()){
        addNewSFF(nz);
    }else {
        nzDes=extractMatKeyPoint(nz);

//        for (int i=0;i<curr.size();i++){
//            //int nea=findNearest(curr.at(i).pt,nz);
//            int nea=findNearestMatch(i,nz);
//            updateForeground(i,nea);
//        }00
        dynamicMatchV2();

        addNewSFF(nz);
        deleteFF();


    }


}

bool FFM::updateForeground(int i,int nea){
    if (nea!=-1){
        curr.at(i).pt=nz.at<cv::Point>(nea);
        curr.at(i).size=cResist;
        ffUsed.push_back(nea);
        int tempDistance=calDis(curr.at(i).pt,start.at(i).pt);
        int temp4MaxDis=calDis2d(curr.at(i).pt,start.at(i).pt);
        if (tempDistance>D || start.at(i).size>0){
            start.at(i).size=mResist;
            mf.at(i)=1;
            if (temp4MaxDis>maxMovingDistance){
                maxMovingDistance=temp4MaxDis;
            }
        }
        tempCurr.at(i).push_back(curr.at(i));
    }else{
        curr.at(i).size-=1;
        start.at(i).size-=1;
        if (curr.at(i).size<0 && start.at(i).size<0){
            del.push_back(i);
        }
        if (start.at(i).size>=0){
            mf.at(i)=1;
        }

    }


}

int FFM::getMFCount(){
    return sum(mf)[0];
}

int FFM::getSFCount(){
    return curr.size()-sum(mf)[0];
}

void FFM::addNewSFF(cv::Mat nz){
    for (int ffi = 0; ffi < nz.total(); ffi++ ) {
        if (std::find(ffUsed.begin(),ffUsed.end(),ffi)==ffUsed.end()){
            cv::Point ffPos=nz.at<cv::Point>(ffi);
            start.push_back(cv::KeyPoint(ffPos,0));
            curr.push_back(cv::KeyPoint(ffPos,cResist));
            currDes.push_back(Mat::zeros(1,64,currDes.type()));
            mf.push_back(0);
            //tempcurr
            vector<KeyPoint> temptempCurr;
            temptempCurr.push_back(cv::KeyPoint(ffPos,0));
            tempCurr.push_back(temptempCurr);
        }
    }


    if (matchType){

          currDes=extractCurrentKeyPointDynamic(curr);

    }
}

void FFM::deleteFF(){
    std::vector<cv::KeyPoint> start2;
    std::vector<cv::KeyPoint> curr2;
    std::vector<int> mf2;
    Mat currDesTemp;
    vector<vector<KeyPoint> > tempCurr2;
    for (int i=0;i<start.size();i++){
        if (std::find(del.begin(),del.end(),i)==del.end()){
            start2.push_back(start.at(i));
            curr2.push_back(curr.at(i));
            mf2.push_back(mf.at(i));
            currDesTemp.push_back(currDes.row(i));
            tempCurr2.push_back(tempCurr.at(i));
        }
    }

    currDes=currDesTemp;
    start=start2;
    curr=curr2;
    mf=mf2;
    tempCurr=tempCurr2;
}

int FFM::findNearest(cv::Point x,cv::Mat nz){

    int index=-1;
    double min=10000;
    for (int i=0;i<nz.total();i++){
        double dis=calDis(nz.at<cv::Point>(i),x);
        if (dis<min && dis<R && std::find(nzUsed.begin(),nzUsed.end(),i)==nzUsed.end()){
            index=i;
            min=dis;

        }
    }
    nzUsed.push_back(index);
    return index;
}

int FFM::findNearestMatch(int rowNum,const Mat nz){
    int index=-1;

    Mat cdkp=currDes.row(rowNum);
    Mat nzdkp;
    std::vector<int> nzTemp;
    for (int i=0;i<nz.total();i++){
        double dis=calDis(nz.at<cv::Point>(i),curr.at(rowNum).pt);

        if (dis<R && std::find(nzUsed.begin(),nzUsed.end(),i)==nzUsed.end()){
            nzdkp.push_back(nzDes.row(i));
            nzTemp.push_back(i);
        }
        else{
            //nzdkp.row(i)=Mat::zeros(1,64,nzDes.type());
        }
    }

    FlannBasedMatcher matcher;

    if (!nzdkp.empty() && cv::sum(cdkp)[0]>0.0){
        matcher.match(cdkp,nzdkp,matches);

    }else{
        return findNearest(curr.at(rowNum).pt,nz);
    }

    index=nzTemp.at(matches[0].trainIdx);

    if (matches[0].distance>=1){

        return findNearest(curr.at(rowNum).pt,nz);

    }

    nzUsed.push_back(index);
    cout<<"in"<<matches[0].trainIdx<<endl;
    return index;

}

vector<int> FFM::dynamicMatch(){
    vector<int> result;
    vector<vector<int> > indexs;
    vector<vector<double> > values;
    for (int i=0;i<curr.size();i++){
        vector<int> tempIndex;
        vector<double> tempValue;
        for (int j=0;j<nz.total();j++){
            Point p1=curr.at(i).pt;
            Point p2=nz.at<cv::Point>(j);
            double distance;
            if (matchType){
                distance=calculateMatchDis(currDes.row(i),nzDes.row(j));
            }
            else{
                distance=calDis(p1,p2);
            }
            tempValue.push_back(distance);
            tempIndex.push_back(j);
//            for (int k=0;k<tempValue.size();k++){
//                if (distance<=tempValue.at(k)){
//                    std::swap(tempValue.at(k),tempValue.at(j));
//                    std::swap(tempIndex.at(k),tempIndex.at(j));
//                    break;
//                }
//            }

        }
        indexs.push_back(tempIndex);
        values.push_back(tempValue);

    }

    vector<int> alreadyUse;
    for (int i=0;i<indexs.size();i++){
        double min=10000;
        int tempRealIndex=-1;
        for (int j=0;j<indexs.at(i).size();j++){
            double val=values.at(i).at(j);
            double far=val;
            if (matchType){
                far=calDis(curr.at(i).pt,nz.at<Point>(indexs.at(i).at(j)));
            }
            if (val<=min && far<R && std::find(alreadyUse.begin(),alreadyUse.end(),indexs.at(i).at(j))==alreadyUse.end()){
                min=val;
                tempRealIndex=indexs.at(i).at(j);
            }
        }
        updateForeground(i,tempRealIndex);
        alreadyUse.push_back(tempRealIndex);
        result.push_back(tempRealIndex);
    }

    return result;

}

double FFM::calDis(cv::Point xx1,cv::Point xx2)
{

    if (calDisType){
        return calDis3D(xx1,xx2);
    }
    //old type
//    R=11.8;
//    D=23.6;
    //new type(x2)
//    R=23.6;
//    D=47.2;
    //return gp->calDis3DFrom2D(xx1,xx2);
    double x = xx1.x - xx2.x;
    double y = xx1.y - xx2.y;
    double dist;

    dist = pow(x,2)+pow(y,2);           //calculating distance by euclidean formula
    dist = sqrt(dist);                  //sqrt is function in math.h

    return dist;
}

double FFM::calDis2d(cv::Point xx1,cv::Point xx2)
{
    //old type
//    R=11.8;
//    D=23.6;
    //new type(x2)
//    R=23.6;
//    D=47.2;
    //return gp->calDis3DFrom2D(xx1,xx2);
    double x = xx1.x - xx2.x;
    double y = xx1.y - xx2.y;
    double dist;

    dist = pow(x,2)+pow(y,2);           //calculating distance by euclidean formula
    dist = sqrt(dist);                  //sqrt is function in math.h

    return dist;
}

double FFM::calDis3D(cv::Point xx1,cv::Point xx2){

    return gp->calDis3DFrom2D(xx1,xx2);
}

Mat FFM::extractKeyPoint(std::vector<cv::KeyPoint> kp){
    Mat result;
    std::vector<KeyPoint> keypoints_object;
    std::vector<Point2f> tttemp;

    KeyPoint::convert(kp,tttemp);
    KeyPoint::convert(tttemp,keypoints_object);
    //Mat descriptors_object, descriptors_scene;

    surf->compute( frame, keypoints_object, result );
    return result;

    //extractor.compute( frame, keypoints_scene, descriptors_scene );
}

Mat FFM::extractMatKeyPoint(Mat kp){
    Mat result;
    std::vector<KeyPoint> keypoints_object;
    std::vector<Point2f> tttemp;
    for (int i=0;i<kp.rows;i++){
        cv::Point ffPos=kp.at<cv::Point>(i);
        tttemp.push_back(ffPos);

    }
    KeyPoint::convert(tttemp,keypoints_object);


    surf->compute( frame, keypoints_object, result );
    return result;
}

Mat FFM::extractCurrentKeyPointDynamic(std::vector<cv::KeyPoint> kp){
    Mat result;

    std::vector<KeyPoint> keypoints_object;
    std::vector<Point2f> tttemp;

    KeyPoint::convert(kp,tttemp);
    KeyPoint::convert(tttemp,keypoints_object);
    //Mat descriptors_object, descriptors_scene;

    surf->compute( frame, keypoints_object, result );

    for(int i=0;i<curr.size();i++){
        if (curr.at(i).size<cResist){
            result.row(i)=currDes.row(i);
        }
    }
    return result;

}

double FFM::calculateMatchDis(Mat pointDes1,Mat pointDes2){
    FlannBasedMatcher matcher;
    std::vector<DMatch> tempMatch;
    if (!pointDes1.empty()){
        matcher.match(pointDes1,pointDes2,tempMatch);
        return tempMatch[0].distance;
    }
    return -1;
}


void FFM::clearNSort(){
    int si=0;
    for (int i=0;i<mf.size();i++){
        if (mf.at(i)==1){
            std::swap(curr.at(si),curr.at(i));
            std::swap(start.at(si),start.at(i));
            std::swap(tempCurr.at(si),tempCurr.at(i));
            if (matchType){
                Mat temp=currDes.row(si);
                currDes.row(si)=currDes.row(i);
                currDes.row(i)=temp;
            }

            si++;
        }
    }
    for (int j=si;j<curr.size();j++){
       if (curr.at(j).size==cResist-1){
                    std::swap(curr.at(si),curr.at(j));
                    std::swap(start.at(si),start.at(j));
                    std::swap(tempCurr.at(si),tempCurr.at(j));
                    if (matchType){
                        Mat temp=currDes.row(si);
                        currDes.row(si)=currDes.row(j);
                        currDes.row(j)=temp;
                    }
                    si++;
       }

    }

    numMF=si-1;

    std::fill(mf.begin(),mf.end(),0);
    ffUsed.clear();
    nzUsed.clear();
    del.clear();
}

bool response_comparator(const cv::KeyPoint& p1, const cv::KeyPoint& p2) {
    return p1.size > p2.size;
}


vector<int> FFM::dynamicMatchV2(){
    vector<int> result;
    vector<vector<int> > indexs;
    vector<vector<double> > values;
    for (int i=0;i<curr.size();i++){
        vector<int> tempIndex;
        vector<double> tempValue;
        for (int j=0;j<nz.total();j++){
            Point p1=curr.at(i).pt;
            Point p2=nz.at<cv::Point>(j);
            double distance;
            if (matchType){
                distance=calculateMatchDis(currDes.row(i),nzDes.row(j));
            }
            else{
                distance=calDis(p1,p2);
            }
            tempValue.push_back(distance);
            tempIndex.push_back(j);
//            for (int k=0;k<tempValue.size();k++){
//                if (distance<=tempValue.at(k)){
//                    std::swap(tempValue.at(k),tempValue.at(j));
//                    std::swap(tempIndex.at(k),tempIndex.at(j));
//                    break;
//                }
//            }

        }
        indexs.push_back(tempIndex);
        values.push_back(tempValue);
        result.push_back(-1);
    }

    vector<int> alreadyUseCurr,alreadyUse,notFound;

    for (int i=0;i<numMF;i++){
        double min=10000;
        int tempRealIndex=-1;
        for (int j=0;j<nz.total();j++){
            double val=values.at(i).at(j);
            double far=val;
            if (matchType){
                far=calDis(curr.at(i).pt,nz.at<Point>(j));
            }
            if (val<=min && far<R && std::find(alreadyUse.begin(),alreadyUse.end(),j)==alreadyUse.end()){
                min=val;
                tempRealIndex=j;
            }
        }
        if (tempRealIndex>=0){
            //updateForeground(i,tempRealIndex);
            alreadyUse.push_back(tempRealIndex);
            result.at(i)=tempRealIndex;
            alreadyUseCurr.push_back(i);
        }

    }


    for (int i=0;i<nz.total();i++){
        double min=10000;
        int tempRealIndex=-1;
        if (std::find(alreadyUse.begin(),alreadyUse.end(),i)!=alreadyUse.end()){
            continue;
        }
        for (int j=0;j<curr.size();j++){
            double val=values.at(j).at(i);
            double far=val;
            if (matchType){
                far=calDis(curr.at(j).pt,nz.at<Point>(i));
            }

            if (val<=min && far<R && std::find(alreadyUseCurr.begin(),alreadyUseCurr.end(),j)==alreadyUseCurr.end()){

                min=val;
                tempRealIndex=j;
            }
        }
        if (tempRealIndex>=0){

            alreadyUseCurr.push_back(tempRealIndex);
            result.at(tempRealIndex)=i;
        }
    }

    for (int i=0;i<curr.size();i++){
        updateForeground(i,result.at(i));
    }


    return result;

}
