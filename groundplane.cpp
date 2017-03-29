#include "groundplane.h"


GroundPlane::GroundPlane()
{
    readCameraMatrix();

    eachDis=100;//centimetre
    size1=2;//number in 1 line
    size2=3;//number of lines
}

void GroundPlane::readCameraMatrix()
{

    cout << endl << "Reading: " << endl;
    FileStorage fs;
    if (fs.open("out_camera_data.xml", FileStorage::READ)){
        fs["Camera_Matrix"]>>cm;
        fs["distortion_coefficients"] >> dc;
        fs.release();
    }else{
        cout<<"cannot read file camera matrix"<<endl;
    }
}

bool GroundPlane::readCameraMatrix(const string filename)
{

    cout << endl << "Reading: "<<filename << endl;
    FileStorage fs;
    if (fs.open(filename, FileStorage::READ)){
        fs["Camera_Matrix"]>>cm;
        fs["distortion_coefficients"] >> dc;

        fs.release();
        cout<<"dc"<<dc<<endl;
        return true;
    }else{
        cout<<"cannot read file camera matrix"<<endl;
        return false;
    }
}


vector<Point2f> GroundPlane::findCircle(Mat im)
{

    Size patternsize(size1,size2); //interior number of corners
    SimpleBlobDetector::Params params;
    params.minArea = 50;
    params.minDistBetweenBlobs = 5;
    Ptr<SimpleBlobDetector> blobDetector = SimpleBlobDetector::create(params);
    Mat gray;
    cv::cvtColor(im,gray, CV_BGR2GRAY);
    bool patternfound = cv::findCirclesGrid(gray, patternsize, corners,CALIB_CB_SYMMETRIC_GRID + CALIB_CB_CLUSTERING);//, blobDetector);
    cout<<corners<<endl;
    drawChessboardCorners(im, patternsize, Mat(corners), patternfound);
    return corners;
}


void GroundPlane::calPlaneEquation()
{
//--prepare data
    //read camera parameter
    readCameraMatrix();
    //prepare object cordinate
    vector<Point3f> corner3d;
    for (int i=0;i<size2;i++){
        for(int j=size1-1;j>=0;j--){
            corner3d.push_back(Point3f(i*eachDis,j*eachDis,0));

        }
    }
    //solve equation to cameracordinate
    solvePnP(corner3d,corners,cm,dc,rvec,tvec,false);
    //prepare equation of T(rvec+tvec)
    cv::Mat R;
    cv::Rodrigues(rvec, R);
    cv::Mat T(4, 4, R.type()); // T is 4x4
    T( cv::Range(0,3), cv::Range(0,3) ) = R * 1; // copies R into T
    T.row(3)=0;
    T( cv::Range(0,3), cv::Range(3,4) )=tvec*1;
    T.at<double>(3,3)=1;
    cout<<"R"<<R<<endl;
    cout<<"TV"<<tvec<<endl;
    cout<<"T"<<T<<endl;

//--prepare to build groundplane
    //prepare 3 point on ground (object point to camera point [calculate by rvec,tvec])
    //3point angle of L
    Mat testP1 = (Mat_<double>(4,1) << 0, 0, 0,1);
    Mat testP2 = (Mat_<double>(4,1) << 0, eachDis, 0,1);
    Mat testP3 = (Mat_<double>(4,1) << eachDis, eachDis,0,1);
    Mat resu1=T*testP1;
    Mat resu2=T*testP2;
    Mat resu3=T*testP3;
    //store RTvec
    RTvec=T.clone();
    cout<<"result1="<<resu1<<endl;
    cout<<"result2="<<resu2<<endl;
    cout<<"result3="<<resu3<<endl;
    //cout<<"p4Reverse="<<testP3<<endl;
    //cout<<"resultReverse="<<T.inv()*resu3<<endl;
//--calculate groundplane
    vector<double> temp12,temp23,tempN;
    //temp12 vector 1 to 2
    temp12.push_back(resu2.at<double>(0)-resu1.at<double>(0));
    temp12.push_back(resu2.at<double>(1)-resu1.at<double>(1));
    temp12.push_back(resu2.at<double>(2)-resu1.at<double>(2));
    //temp23 vector 2 to 3
    temp23.push_back(resu3.at<double>(0)-resu2.at<double>(0));
    temp23.push_back(resu3.at<double>(1)-resu2.at<double>(1));
    temp23.push_back(resu3.at<double>(2)-resu2.at<double>(2));
    //det find norm vec
    tempN.push_back(temp12[1]*temp23[2]-temp23[1]*temp12[2]);
    tempN.push_back(temp12[2]*temp23[0]-temp23[2]*temp12[0]);
    tempN.push_back(temp12[0]*temp23[1]-temp23[0]*temp12[1]);
//--result show as ax+by+cz+d=0
    a=tempN[0];
    b=tempN[1];
    c=tempN[2];
    d=tempN[0]*(resu2.at<double>(0))+tempN[1]*(resu2.at<double>(1))+tempN[2]*(resu2.at<double>(2));

    cout<<"a="<<a<<endl;
    cout<<"b="<<b<<endl;
    cout<<"c="<<c<<endl;
    cout<<"d="<<d<<endl;

}

void GroundPlane::calImagePlaneEquation(){

    Mat testP1 = (Mat_<double>(3,1) << 0, 0,1);
    Mat testP2 = (Mat_<double>(3,1) << 0, eachDis,1);
    Mat testP3 = (Mat_<double>(3,1) << eachDis, eachDis,1);

    Mat resu1=cm.inv()*testP1;
    Mat resu2=cm.inv()*testP2;
    Mat resu3=cm.inv()*testP3;

    cout<<"resu1"<<resu1<<endl;
    cout<<"resu2"<<resu2<<endl;
    cout<<"resu3"<<resu3<<endl;

    vector<double> temp12,temp23,tempN;
    //temp12 vector 1 to 2
    temp12.push_back(resu2.at<double>(0)-resu1.at<double>(0));
    temp12.push_back(resu2.at<double>(1)-resu1.at<double>(1));
    temp12.push_back(resu2.at<double>(2)-resu1.at<double>(2));
    //temp23 vector 2 to 3
    temp23.push_back(resu3.at<double>(0)-resu2.at<double>(0));
    temp23.push_back(resu3.at<double>(1)-resu2.at<double>(1));
    temp23.push_back(resu3.at<double>(2)-resu2.at<double>(2));
    //det find norm vec
    tempN.push_back(temp12[1]*temp23[2]-temp23[1]*temp12[2]);
    tempN.push_back(temp12[2]*temp23[0]-temp23[2]*temp12[0]);
    tempN.push_back(temp12[0]*temp23[1]-temp23[0]*temp12[1]);
//--result show as ax+by+cz+d=0
    ia=tempN[0];
    ib=tempN[1];
    ic=tempN[2];
    id=tempN[0]*(resu2.at<double>(0))+tempN[1]*(resu2.at<double>(1))+tempN[2]*(resu2.at<double>(2));

    cout<<"ia="<<ia<<endl;
    cout<<"ib="<<ib<<endl;
    cout<<"ic="<<ic<<endl;
    cout<<"id="<<id<<endl;
}



Point3d GroundPlane::imgCord2CameraCord(Point2d imgCord)
{
    //former calculation(uncompounded undistort)
//    Mat ttt1 = (Mat_<double>(3,1) << imgCord.x, imgCord.y,1);
//    ttt1=cm.inv()*ttt1;
//    cout<<"ttt1"<<ttt1<<endl;
    //current calculation(increase undistort)
    Mat ttt2 = (Mat_<Point2f>(1,1) << imgCord);
    Mat distorted_points2d;
    undistortPoints(ttt2,distorted_points2d,cm,dc);
    //cout<<"distorted2d"<<distorted_points2d<<endl;
    distorted_points2d.convertTo(distorted_points2d, CV_64F);
    //return Point3d(ttt1.at<double>(0),ttt1.at<double>(1),ttt1.at<double>(2));
    return Point3d(distorted_points2d.at<double>(0),distorted_points2d.at<double>(1),1);
}

Point GroundPlane::camCord2ImgCord(Point3d camCord){
    Mat ttt2=(Mat_<double>(3,1) << camCord.x, camCord.y,camCord.z);
    ttt2=cm*ttt2;
    return Point(round(ttt2.at<double>(0)),round(ttt2.at<double>(1)));
}

Point3d GroundPlane::getPointAtGround(Point2d imgCord)
{
    Mat ttt1 = (Mat_<double>(3,1) << imgCord.x, imgCord.y,1);
    ttt1=cm.inv()*ttt1;
    double x=d/(a*ttt1.at<double>(0)+b*ttt1.at<double>(1)+c*ttt1.at<double>(2))*ttt1.at<double>(0);
    double y=d/(a*ttt1.at<double>(0)+b*ttt1.at<double>(1)+c*ttt1.at<double>(2))*ttt1.at<double>(1);
    double z=d/(a*ttt1.at<double>(0)+b*ttt1.at<double>(1)+c*ttt1.at<double>(2));
    return Point3d(x,y,z);
}

Point GroundPlane::getPointAtImageFrom3D(Point3d camCord){
    Mat ttt1 = (Mat_<double>(3,1) << camCord.x, camCord.y,camCord.z);
    double x=id/(ia*ttt1.at<double>(0)+ib*ttt1.at<double>(1)+ic*ttt1.at<double>(2))*ttt1.at<double>(0);
    double y=id/(ia*ttt1.at<double>(0)+ib*ttt1.at<double>(1)+ic*ttt1.at<double>(2))*ttt1.at<double>(1);
    double z=id/(ia*ttt1.at<double>(0)+ib*ttt1.at<double>(1)+ic*ttt1.at<double>(2))*ttt1.at<double>(2);

    return camCord2ImgCord(Point3d(x,y,z));
}

double GroundPlane::calDis3D(Point3d a,Point3d b)
{

    return norm(a-b);

}

double GroundPlane::calDis3DFrom2D(Point2d a,Point2d b)
{
    return norm(getPointAtGround(a)-getPointAtGround(b));
}

bool GroundPlane::saveGroundParam()
{
    FileStorage fs("ground.xml", FileStorage::WRITE );
    fs<<"a"<<a;
    fs<<"b"<<b;
    fs<<"c"<<c;
    fs<<"d"<<d;
    fs<<"RTvec"<<RTvec;
    fs<<"ia"<<ia;
    fs<<"ib"<<ib;
    fs<<"ic"<<ic;
    fs<<"id"<<id;
    fs.release();
    return true;
}

bool GroundPlane::saveGroundParam(const string x)
{
    FileStorage fs(x, FileStorage::WRITE );
    fs<<"a"<<a;
    fs<<"b"<<b;
    fs<<"c"<<c;
    fs<<"d"<<d;
    fs<<"RTvec"<<RTvec;
    fs<<"ia"<<ia;
    fs<<"ib"<<ib;
    fs<<"ic"<<ic;
    fs<<"id"<<id;
    fs.release();
    return true;
}

bool GroundPlane::readGroundParam()
{
    FileStorage fs;
    if (fs.open("ground.xml", FileStorage::READ)){
        fs["a"]>>a;
        fs["b"]>>b;
        fs["c"]>>c;
        fs["d"]>>d;
        fs["RTvec"]>>RTvec;
        fs["ia"]>>ia;
        fs["ib"]>>ib;
        fs["ic"]>>ic;
        fs["id"]>>id;
        cout<<a<<endl;
        cout<<b<<endl;
        cout<<c<<endl;
        cout<<d<<endl;
        fs.release();
        return true;
    }else{
        cout<<"cannot read ground plane"<<endl;
        return false;
    }
}

bool GroundPlane::readGroundParam(const string x)
{
    FileStorage fs;
    if (fs.open(x, FileStorage::READ)){
        fs["a"]>>a;
        fs["b"]>>b;
        fs["c"]>>c;
        fs["d"]>>d;
        fs["RTvec"]>>RTvec;
        fs["ia"]>>ia;
        fs["ib"]>>ib;
        fs["ic"]>>ic;
        fs["id"]>>id;
        cout<<a<<endl;
        cout<<b<<endl;
        cout<<c<<endl;
        cout<<d<<endl;
        fs.release();
        return true;
    }else{
        cout<<"cannot read ground plane"<<endl;
        return false;
    }
}

bool GroundPlane::writeSetting(const string x)
{
    FileStorage fs(x, FileStorage::WRITE );
    fs<<"Camera_Matrix"<<cm;
    fs<<"distortion_coefficients"<<dc;
    fs<<"a"<<a;
    fs<<"b"<<b;
    fs<<"c"<<c;
    fs<<"d"<<d;
    fs<<"RTvec"<<RTvec;
    fs<<"ia"<<ia;
    fs<<"ib"<<ib;
    fs<<"ic"<<ic;
    fs<<"id"<<id;
    fs.release();

}

//temp of calculation for get norm from ground
Point GroundPlane::calPerpenTopPoint(Point lowest){
    Point3d l =getPointAtGround(lowest);
    double normalizeValue=norm(Point3d(a,b,c));
    //x3D=RTvec*x3D;
    l.x+=a/normalizeValue*300;
    l.y+=b/normalizeValue*300;
    l.z+=c/normalizeValue*300;
//    cout<<"ia="<<ia<<endl;
//    cout<<"ib="<<ib<<endl;
//    cout<<"ic="<<ic<<endl;
//    cout<<"id="<<id<<endl;

//    cout<<"x"<<l.x<<endl;
//    cout<<"y"<<l.y<<endl;
//    cout<<"z"<<l.z<<endl;
    cout<<getPointAtImageFrom3D(l)<<endl;


    return getPointAtImageFrom3D(l);
}

//main for calculate top point
Point GroundPlane::calTopPoint(Point lowest,Point Heighest,double &high){
    //Mat x3D = (Mat_<double>(4,1) << lowest.x, lowest.y, 0,1);
    Point3d l =getPointAtGround(lowest);
    Point3d h =Point3d(l.x,l.y,l.z);
    double normalizeValue=norm(Point3d(a,b,c));
    //x3D=RTvec*x3D;
    h.x+=a/normalizeValue*300;
    h.y+=b/normalizeValue*300;
    h.z+=c/normalizeValue*300;



    findMB4Line(getPointAtImageFrom3D(l),getPointAtImageFrom3D(h));

    calPerpendicular(Heighest);

    Point resultTop=intersectionMB();


    Point3d hhh=pointIntersecNorm3D(resultTop,l,h);

    high=calDis3D(l,hhh);

    //cout<<"high of Contour "<<high<<endl;



    return getPointAtImageFrom3D(hhh);

}

Point GroundPlane::calTopPointNonPerpendicular(Point lowest,Point Heighest,double &highV2,double &distanecRN,double &farCV2){
    //Mat x3D = (Mat_<double>(4,1) << lowest.x, lowest.y, 0,1);
    Point3d l =getPointAtGround(lowest);
    Point3d h =Point3d(l.x,l.y,l.z);
    double normalizeValue=norm(Point3d(a,b,c));
    //x3D=RTvec*x3D;
    h.x+=a/normalizeValue*300;
    h.y+=b/normalizeValue*300;
    h.z+=c/normalizeValue*300;



    findMB4Line(getPointAtImageFrom3D(l),getPointAtImageFrom3D(h));

    calPerpendicular(Heighest);

    Point resultTop=intersectionMB();

    //first approach
    //Point3d hhh=pointIntersecNorm3D(resultTop,l,h);

    //second approach

    Point3d hhh=pointIntersecNorm3DRay(Heighest,l,h,distanecRN);

    highV2=calDis3D(l,hhh);

    farCV2=calDis3D(Point3d(0,0,0),hhh);

    //cout<<"high of Contour "<<highV2<<endl;



    return getPointAtImageFrom3D(hhh);

}

//find Y=mX+b
void GroundPlane::findMB4Line(Point a,Point b){
    //double semiX,semiY;
    double mX=(a.x-b.x);
    double mY=(a.y-b.y);
    lm=mY/mX;

    lb=a.y-(a.x*lm);

}

//find m2,b2 of perpendicular
void GroundPlane::calPerpendicular(Point xxx){
    lm2=(-1/lm);
    lb2=xxx.y-(lm2*xxx.x);

}

//find intersect point of Y=mX+b , Y=m2X+b2
Point GroundPlane::intersectionMB(){
    double x=(lb2-lb)/(lm-lm2);
    double y=lm*x+lb;

    return Point(x,y);

}

//find intersection of highest point with norm vector(from ground)
Point3d GroundPlane::pointIntersecNorm3D(Point topP,Point3d l,Point3d h){
    Point3d lowImP=Point3d(0,0,0);
    Point3d topImP=imgCord2CameraCord(topP);

    Point3d vecIm=Point3d(topImP.x-lowImP.x,topImP.y-lowImP.y,topImP.z-lowImP.z);
    Point3d vecLH=Point3d(h.x-l.x,h.y-l.y,h.z-l.z);

    Point3d   u = Point3d(topImP.x-lowImP.x,topImP.y-lowImP.y,topImP.z-lowImP.z);
    Point3d   v = Point3d(h.x-l.x,h.y-l.y,h.z-l.z);
    Point3d   w = Point3d(lowImP.x-l.x,lowImP.y-l.y,lowImP.z-l.z);

    double    a = dot(u,u);         // always >= 0
    double    b = dot(u,v);
    double    c = dot(v,v);         // always >= 0
    double    d = dot(u,w);
    double    e = dot(v,w);
    double    D = a*c - b*b;        // always >= 0
    double    sc, tc;

    if (D < 0.00000001) {          // the lines are almost parallel
        sc = 0.0;
        tc = (b>c ? d/b : e/c);    // use the largest denominator
    }
    else {
        sc = (b*e - c*d) / D;
        tc = (a*e - b*d) / D;
    }

    return Point3d(l.x+tc*v.x,l.y+tc*v.y,l.z+tc*v.z);


}

//find intersection of highest point with norm vector(from ground)
Point3d GroundPlane::pointIntersecNorm3DRay(Point topP,Point3d l,Point3d h,double &distanceRN){
    Point3d lowImP=Point3d(0,0,0);
    Point3d topImP=imgCord2CameraCord(topP);

    Point3d vecIm=Point3d(topImP.x-lowImP.x,topImP.y-lowImP.y,topImP.z-lowImP.z);
    Point3d vecLH=Point3d(h.x-l.x,h.y-l.y,h.z-l.z);

    Point3d   u = Point3d(topImP.x-lowImP.x,topImP.y-lowImP.y,topImP.z-lowImP.z);
    Point3d   v = Point3d(h.x-l.x,h.y-l.y,h.z-l.z);
    Point3d   w = Point3d(lowImP.x-l.x,lowImP.y-l.y,lowImP.z-l.z);

    double    a = dot(u,u);         // always >= 0
    double    b = dot(u,v);
    double    c = dot(v,v);         // always >= 0
    double    d = dot(u,w);
    double    e = dot(v,w);
    double    D = a*c - b*b;        // always >= 0
    double    sc, tc;

    if (D < 0.00000001) {          // the lines are almost parallel
        sc = 0.0;
        tc = (b>c ? d/b : e/c);    // use the largest denominator
    }
    else {
        sc = (b*e - c*d) / D;
        tc = (a*e - b*d) / D;
    }

    Point3d pointAtRay=Point3d(lowImP.x+sc*u.x,lowImP.y+sc*u.y,lowImP.z+sc*u.z);
    Point3d pointAtNorm=Point3d(l.x+tc*v.x,l.y+tc*v.y,l.z+tc*v.z);

    distanceRN=calDis3D(pointAtRay,pointAtNorm);

    //return Point3d(lowImP.x+sc*u.x,lowImP.y+sc*u.y,lowImP.z+sc*u.z);
return Point3d(l.x+tc*v.x,l.y+tc*v.y,l.z+tc*v.z);

}



//dot product
double GroundPlane::dot(Point3d a,Point3d b){
    return a.x*b.x+a.y*b.y+a.z*b.z;
}

//calculate point of norm to test on image
Point GroundPlane::calTestHigh(Point lowest){
    Point3d l =getPointAtGround(lowest);
    Point3d h =Point3d(l.x,l.y,l.z);
    double normalizeValue=norm(Point3d(a,b,c));
    //x3D=RTvec*x3D;
    h.x+=a/normalizeValue*175;
    h.y+=b/normalizeValue*175;
    h.z+=c/normalizeValue*175;

    return getPointAtImageFrom3D(h);
}

Point3d GroundPlane::calTestHighV3d(Point lowest){
    Point3d l =getPointAtGround(lowest);
    Point3d h =Point3d(l.x,l.y,l.z);
    double normalizeValue=norm(Point3d(a,b,c));
    //x3D=RTvec*x3D;
    h.x+=a/normalizeValue*175;
    h.y+=b/normalizeValue*175;
    h.z+=c/normalizeValue*175;

    return Point3d(h.x,h.y,h.z);
}

void GroundPlane::simulateGroundPlane(double alpha,double dist,Mat im){
    // Rotation matrices around the X axis
    alpha=alpha*(3.14159265)/180;
    Mat T = (Mat_<double>(4, 4) <<
             1,          0,           0, 0,
             0, cos(alpha), -sin(alpha), 0,
             0, sin(alpha),  cos(alpha), dist,
             0,          0,           0, 1);
    cout<<T<<endl;
    Point3d aa=imgCord2CameraCord(Point(cm.at<double>(0,2)-5000,cm.at<double>(1,2)-5000));
    Point3d bb=imgCord2CameraCord(Point(cm.at<double>(0,2)-5000,cm.at<double>(1,2)-4900));
    Point3d cc=imgCord2CameraCord(Point(cm.at<double>(0,2)-4900,cm.at<double>(1,2)-4900));
    Mat testP1 = (Mat_<double>(4,1) << -5000, -5000, 0,1);
    Mat testP2 = (Mat_<double>(4,1) << -5000, -4900, 0,1);
    Mat testP3 = (Mat_<double>(4,1) << -4900, -4900, 0,1);
    Mat resu1=T*testP1;
    Mat resu2=T*testP2;
    Mat resu3=T*testP3;
    //store RTvec
    RTvec=T.clone();
    cout<<"result1="<<resu1<<endl;
    cout<<"result2="<<resu2<<endl;
    cout<<"result3="<<resu3<<endl;

    vector<double> temp12,temp23,tempN;
    //temp12 vector 1 to 2
    temp12.push_back(resu2.at<double>(0)-resu1.at<double>(0));
    temp12.push_back(resu2.at<double>(1)-resu1.at<double>(1));
    temp12.push_back(resu2.at<double>(2)-resu1.at<double>(2));
    //temp23 vector 2 to 3
    temp23.push_back(resu3.at<double>(0)-resu2.at<double>(0));
    temp23.push_back(resu3.at<double>(1)-resu2.at<double>(1));
    temp23.push_back(resu3.at<double>(2)-resu2.at<double>(2));
    //det find norm vec
    tempN.push_back(temp12[1]*temp23[2]-temp23[1]*temp12[2]);
    tempN.push_back(temp12[2]*temp23[0]-temp23[2]*temp12[0]);
    tempN.push_back(temp12[0]*temp23[1]-temp23[0]*temp12[1]);
//--result show as ax+by+cz+d=0
    a=tempN[0];
    b=tempN[1];
    c=tempN[2];
    d=tempN[0]*(resu2.at<double>(0))+tempN[1]*(resu2.at<double>(1))+tempN[2]*(resu2.at<double>(2));

    cout<<"a="<<a<<endl;
    cout<<"b="<<b<<endl;
    cout<<"c="<<c<<endl;
    cout<<"d="<<d<<endl;

    // get start point onimage(0,0) to object coordinate
    Point3d beginP=getPointAtGround(Point(0,480));
    Point3d endP=getPointAtGround(Point(640,480));
    cout<<beginP<<endl;
    Mat testPPP = (Mat_<double>(4,1) << beginP.x, beginP.y, beginP.z,1);
    Mat testPPPRS=T.inv()*testPPP;
    Mat testPPPE = (Mat_<double>(4,1) << endP.x, endP.y, endP.z,1);
    Mat testPPPRSE=T.inv()*testPPPE;
    Point3d startObjectP=Point3d(testPPPRS.at<double>(0,0),testPPPRS.at<double>(0,1),0);
    Point3d endObjectP=Point3d(testPPPRSE.at<double>(0,0),testPPPRSE.at<double>(0,1),0);
    //cout<<round(testPPPRS.at<double>(0,2))<<endl;
    //cout<<round(testPPPRS.at<double>(0,2))<<endl;

    int redInten=255;
    for (int y=-1000;y<endObjectP.y;y+=100){
        Mat tempStart = (Mat_<double>(4,1) << startObjectP.x, y, 0,1);
        Mat tempEnd = (Mat_<double>(4,1) << endObjectP.x, y, 0,1);
        Mat tempStartRS=T*tempStart;
        Mat tempEndRS=T*tempEnd;
        Point tempStartLine=getPointAtImageFrom3D(Point3d(tempStartRS.at<double>(0,0),tempStartRS.at<double>(0,1),tempStartRS.at<double>(0,2)));
        Point tempEndLine=getPointAtImageFrom3D(Point3d(tempEndRS.at<double>(0,0),tempEndRS.at<double>(0,1),tempEndRS.at<double>(0,2)));
        cv::line(im,tempStartLine,tempEndLine,cv::Scalar(0,0,255));
        redInten-=1;
    }
    for (int x=startObjectP.x;x<endObjectP.x;x+=100){
        Mat tempStart = (Mat_<double>(4,1) << x, -1000, 0,1);
        Mat tempEnd = (Mat_<double>(4,1) << x, endObjectP.y, 0,1);
        Mat tempStartRS=T*tempStart;
        Mat tempEndRS=T*tempEnd;
        Point tempStartLine=getPointAtImageFrom3D(Point3d(tempStartRS.at<double>(0,0),tempStartRS.at<double>(0,1),tempStartRS.at<double>(0,2)));
        Point tempEndLine=getPointAtImageFrom3D(Point3d(tempEndRS.at<double>(0,0),tempEndRS.at<double>(0,1),tempEndRS.at<double>(0,2)));
        cv::line(im,tempStartLine,tempEndLine,cv::Scalar(0,0,255));
        redInten-=1;
    }




}

void GroundPlane::undistortCM(Mat src){
    Mat uselessMat;
    Mat tempNewCM;
    cout<<"src"<<src<<endl;
    undistort(src,uselessMat,cm,dc,tempNewCM);
    cout<<"initialize new CM from distort"<<endl;
    cout<<"cm"<<cm<<endl;
    cm=tempNewCM.clone();
    cout<<"cm"<<cm<<endl;
}
