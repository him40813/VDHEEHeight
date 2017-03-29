#include "heightprocess.h"

HeightProcess::HeightProcess(cv::Ptr<GroundPlane> gp)
{
    this->gp=gp;
    bgs = new SJN_MultiCueBGS;
    heightHis=vector<int>();
    roiChecker=imread("106-109_ROI.png",CV_LOAD_IMAGE_GRAYSCALE);
    cv::cvtColor(roiChecker, roiColor, cv::COLOR_GRAY2BGR);
}

double HeightProcess::heightBase(Mat frame,int fnumber){



    if(fnumber==46){
        int a= 2;
    }
    int yExtra=0;
    int yExtraV2=0;
    cv::cvtColor(frame,input,CV_BGR2GRAY);

    bgs->process(frame,output,output_bkgmodel);
    vector<vector<Point> > contours;
    vector<vector<Point> > contoursDes;
    vector<Vec4i> hierarchy;
    Mat canny_output;
    int thresh=100;
    RNG rng(12345);
    Canny( output, canny_output, thresh, thresh*2, 3 );



    findContours(canny_output,contours,hierarchy,CV_RETR_EXTERNAL , CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
    //new
    Mat mask_image( canny_output.size(), CV_8U, Scalar(0));
    // draw your contour in mask
    drawContours(mask_image, contours, 0, Scalar(255), CV_FILLED);

     //edge of real frame
    Mat canny_output_real;
    int thresh_real=100;
    Canny( frame, canny_output_real, thresh_real, thresh_real*2, 3 );

    //edge background subtraction
    if (fnumber==0){
        edgeBg=canny_output_real.clone();
        cv::morphologyEx(edgeBg, edgeBg, cv::MORPH_DILATE, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3)));
        xAreaSize=(frame.cols/3);
        yAreaSize=(frame.rows/3);
    }else{
        for (int r=0;r<edgeBg.rows;++r){
            for(int c=0;c<edgeBg.cols;++c){
                if (edgeBg.at<uchar>(r,c)>0){
                    canny_output_real.at<uchar>(r,c)=0;
                }
            }
        }
    }

    Mat drawing2;
    frame.copyTo(drawing2,output);

    Mat drawing2V2=roiColor.clone();
    //convert cannyReal output to bgr
    Mat colorCanny;
    cv::cvtColor(canny_output_real, colorCanny, cv::COLOR_GRAY2BGR);
    //copy with mask to drawing2v2
    colorCanny.copyTo(drawing2V2,output);
//                namedWindow("ttt");
//                imshow("ttt",canny_output_real);
    bool conCheck=false;

    for( int i = 0; i< contours.size() ; i++ )
    {

       bool outOfRoi=false;


       Mat tempContour=Mat::zeros(output.size(), CV_8UC1);;
       drawContours(tempContour, contours, i, Scalar(255), CV_FILLED);
       Point top,low;
       vector<Point> vtemp;
       cv::Mat nz;
       cv::findNonZero(canny_output_real, nz);
       int yyT=1000,yyL=-1;

       for (int j = 0; j < nz.total(); j++ ) {
           Point temp=nz.at<cv::Point>(j);

           if (tempContour.type()==16){

               if (tempContour.at<Vec3b>(nz.at<cv::Point>(j))[0]>0){

                  if (temp.y>yyL){
                      yyL=temp.y;
                      low=temp;
                  }
                  if (temp.y<yyT){
                      yyT=temp.y;
                      top=temp;
                  }
               }
           }else if (tempContour.type()==0){

               if (tempContour.at<uchar>(nz.at<cv::Point>(j))>0){


                   if (roiChecker.at<uchar>(nz.at<cv::Point>(j))>0){


                       //cout<<roiChecker<<endl;

                       top=Point(50,50);
                       low=Point(50,50);
                       outOfRoi=true;
                       break;

                   }
                  if (temp.y>yyL){
                      yyL=temp.y;
                      low=temp;
                  }
                  if (temp.y<yyT){
                      yyT=temp.y;
                      top=temp;
                  }
               }
           }


       }
       vtemp.push_back(top);
       vtemp.push_back(low);
       contoursDes.push_back(vtemp);
       //cv::line(drawing,top,low
            //    ,cv::Scalar(0,255,255));
       //draw
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       if (!outOfRoi)
            drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );

       if (fnumber!=0){
           //cout<<gp->calNormVec2D(Point(400,400))<<endl;
           //line(drawing,Point(400,400),gp->calNormVec2D(Point(400,400)),cv::Scalar(0,0,255));
           //test
           //Point3d s=Point3d(0,0,0);
           //Point3d e=Point3d(600,450,0);
           double highC=0;
           Point nTop=gp->calTopPoint(low,top,highC);

           double highCV2=0;
           double farCV2=0;
           double distanceRN=1000;
           Point nTopV2=gp->calTopPointNonPerpendicular(low,top,highCV2,distanceRN,farCV2);

           //cv::line(drawing,low,gp->calPerpenTopPoint(low),cv::Scalar(0,0,255));


            cout<<highCV2<<endl;
           if (highCV2>90 && highCV2<250 && !outOfRoi ){//highCV2>140 && highCV2<200 && distanceRN<=0.5){

               double iR=0.05;
               double growingPersent=abs(highCV2-contTemp)*100/contTemp;

               if(!conCheck && distanceRN<1 && highCV2>=contTemp*(1-iR) && highCV2<=contTemp*(1+iR)){//high increase +- 10 percent
                    ++contCounter;
                    contTemp=highCV2;
                    conCheck=true;
               }else {
                    if (!conCheck && i==contours.size()-1)
                        contCounter=0;
                        contTemp=highCV2;
               }

               //heightHis push to vector
                if (round(highCV2)!=104 && round(highCV2)!=495)
                    heightHis.push_back(round(highCV2));

               if (heightHis.size()>10){
                   HDPAV=calPeakBin(heightHis,30);
               }

            if(heightHis.size()>10) {
                cout<<HDPAV<<endl;
                return HDPAV;
            }
            else{
                return -1;
            }


           }

        }

    }

    return -1;
}

int HeightProcess::calPeakBin(vector<int> heightHis,double percentage)
{
    //sort first
    sort(heightHis.begin(),heightHis.end());
    //prepare parameter
    vector<int> tempHeight,tempNum;
    int remainNumber=heightHis.size()*percentage/100;

    //order height information into histrogram
    for (int i=0;i<heightHis.size();++i){
        int thisHeight=heightHis.at(i);

        std::vector<int>::iterator found=find(tempHeight.begin(),tempHeight.end(),thisHeight);
        if (found!=tempHeight.end()){
            int hIndex=distance(tempHeight.begin(),found);
            ++tempNum.at(hIndex);
            //cout<<thisHeight<<" : "<<tempHeight.at(hIndex)<<endl;
             cout<<"thisHeight1"<<tempNum.at(hIndex)<<endl;
        }else {
            tempHeight.push_back(thisHeight);
            tempNum.push_back(1);
            cout<<"thisHeight1"<<tempHeight.at(0)<<endl;
        }
    }
    cout<<"1"<<endl;

    //find peak bin
    std::vector<int>::iterator maxNum=max_element(tempNum.begin(),tempNum.end());
    //cout<<"maxNum: "<<*maxNum<<endl;
    int maxNumIndex=distance(tempNum.begin(),maxNum);
    int maxCount = count (tempNum.begin(), tempNum.end(),*maxNum);
    //cout<<"maxCount: "<<maxCount<<endl;
    //Multiple max value circumstance
    if (maxCount>1){
        int mostValue=0;
        int mostValInd=-1;
        for (int mi=0;mi<tempNum.size();++mi){
            if (tempNum.at(mi)==*maxNum){
                //initialize neighboor value
                cout<<"maxNum & ind"<<tempNum.at(mi)<<" : "<<mi<<endl;
                int thisVal=0;
                if (mi-1>=0)
                    thisVal+=tempNum.at(mi-1);
                if (mi+1<tempNum.size())
                    thisVal+=tempNum.at(mi+1);
                //compare with mostValue
                if (thisVal>mostValue){
                    mostValue=thisVal;
                    mostValInd=mi;
                }
            }
        }
        maxNumIndex=mostValInd;
        //cout<<"maxNumInd : "<<maxNumIndex<<" : "<<tempHeight.at(maxNumIndex)<<endl;
    }

    cout<<"2"<<endl;
    //main loop for opt
    int leftShift=1;
    int rightShift=1;
    vector<int> resultNum(tempNum.size(),0);
    cout<<tempNum.size()<<" : "<<resultNum.size()<<endl;
    //install max num
    resultNum.at(maxNumIndex)=tempNum.at(maxNumIndex);
    while(remainNumber>0){
        int lVal=-1;
        int rVal=-1;

        //initialize right and left val
        if (maxNumIndex-leftShift>=0)
            lVal=tempNum.at(maxNumIndex-leftShift);
        if (maxNumIndex+rightShift<tempNum.size())
            rVal=tempNum.at(maxNumIndex+rightShift);

        //compare
        if (lVal>=rVal && lVal!=-1){

            resultNum.at(maxNumIndex-leftShift)=lVal;
            remainNumber-=lVal;
            ++leftShift;
        }else if(rVal!=-1) {

            resultNum.at(maxNumIndex+rightShift)=rVal;
            remainNumber-=rVal;
            ++rightShift;
        }else{
            //cout<<remainNumber<<endl;
            break;
        }

    }
    cout<<"3"<<endl;
    int allVal=0;
    int allNum=0;
    for (int wi=0;wi<resultNum.size();++wi){
        cout<<wi<<","<<resultNum.size()<<endl;
        int tempNum=resultNum.at(wi);
        cout<<tempNum<<endl;
        if (tempNum>0){
            allVal+=tempNum*tempHeight.at(wi);
            allNum+=tempNum;
        }
        cout<<allVal<<","<<allNum<<endl;
    }

    cout<<"average : "<<allVal/allNum<<endl;

    return allVal/allNum;

}



