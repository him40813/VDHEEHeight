#ifndef PBAS_H
#define PBAS_H
//Implementation of the PBAS from:
//
//M. Hofmann, P. Tiefenbacher, G. Rigoll
//"Background Segmentation with Feedback: The Pixel-Based Adaptive Segmenter",
//in proc of IEEE Workshop on Change Detection, 2012
//
//Note: some changes, to improve the speed and memory requirements, were achieved in comparison to the
//described PBAS algorithm in the paper above.
//
//Example usage:
// //Somewhere during initalization:
// #include "PBAS.h"
// #include <opencv2/opencv.hpp>
// PBAS pbas;
//
// //you might want to change some parameters of the PBAS here...
// ....
//
// //repeat for each frame
// //make gaussian blur for reducing image noise
//cv::Mat bluredImage;
//cv::Mat pbastResult;
//cv::GaussianBlur(singleFrame, bluredImage, cv::Size(5,5), 1.5);
//
// //process image and receive segmentation in pbasResult
//pbas.process(&bluredImage, &pbasResult);
//
// //make medianBlur on the result to reduce "salt and pepper noise"
// //of the per pixel wise segmentation
//cv::medianBlur(pbasResult, pbasResult, 5);
//
//
//
//Author: P.Tiefenbacher, https://sites.google.com/site/pbassegmenter/
//Technische Universit�t M�nchen, Germany
//Date: 22-May-2012, Version:0.1
///////////

#pragma once
#include <iostream>
#include <opencv/opencv.hpp>
#include "IBGS.h"

using namespace std;
using namespace cv;

class PBAS : public IBGS
{
public:
        //// MOD
        void showResults(bool r);

        PBAS(void);
        ~PBAS(void);
        //bool process(cv::Mat* input, cv::Mat* output, cv::Mat &img_bgmodel);

        void process(const cv::Mat &input, cv::Mat &output, cv::Mat &img_bgmodel);

        //void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);
        //N - Number: Defining the size of the background-history-model
        // number of samples per pixel
        //size of background history B(x_i)
        void setN(int);
        //counter for minimal distance to background
        // Defining the number of background-model-images, which have
        // a lowerDIstance to the current Image than defined by the R-Thresholds, that are necessary
        // to decide that this pixel is background
        void setRaute_min(int);
        //R-Threshhold - Variables
        //minimal Threshold for foreground and initial value of R(x_i)
        // radius of the sphere -> lower border boundary
        void setR_lower(double);
        //decreasing / increasing factor of R(x_i)
        // increasing/decreasing factor for the r-Threshold based on
        // the result of rTreshScale * meanMinDistBackground
        void setR_incdec(double);
        //factor which defines new threshold of R(x_i) together with meanMinDist(x_i)
        // scale for the sphere threshhold to define pixel-based Thresholds
        void setR_scale(double);
        //initial value of inverse update factor T(x_i)
        // Initialize the background-model update rate
        void setT_init(double);
        //lower boundary of T(x_i)
        // defining a minimum value for nrSubsampling --> base value 2.0
        void setT_lower(double);
        //upper boundary of T(x_i)
        // defining an upper value, that nrSubsampling can achieve, thus it doesn't
        // reach to an infinite value, where actually no update is possible
        // at all
        void setT_upper(double);
        //decreasing factor of the update rate 1/T(x_i)
        // opposite scale to increasingRateScale, for decreasing the update rate of
        // the background model, if the current pixel is foreground
        //--> Thesis: Our Foreground is a real moving object -> thus the background-model
        // is good, so don't update it
        void setT_dec(double);
        //increasing Factor of the update rate 1/T(x_i)
        // scale that defines the increasing of the update rate of the background model,
        // if the current pixel is background
        //--> more frequently updates if pixel is background because, there shouln't be
        // any change
        void setT_inc(double);
        void setAlpha(double);   //balance of feature pixel value to conture value
        void setBeta(double);    //balance of feature pixel value to conture value

        bool isMovement();
        cv::Mat *getR1_xi();
        cv::Mat *getT_xi();

private:
        //// MOD
        bool firstTime;
        bool showOutput;

        void saveConfig();
        void loadConfig();

        void calculateFeatures(std::vector<cv::Mat>* feature, cv::Mat* inputImage);
        void checkValid(int *x, int *y);
        void decisionThresholdRegulator(float* pt, float* meanDistArr);
        void learningRateRegulator(float* pt, float* meanDist,uchar* isFore);
        void init(cv::Mat*);
        void newInitialization();

        cv::Mat meanMinDist;
        float* meanMinDist_Pt;



        int width, height;
        int chans;

        //balance of feature pixel value to conture value
        double alpha, beta;
        //##################################################################################

        double formerMeanNorm;

        //define value of foreground/background pixels
        int foregroundValue, backgroundValue;

        //##################################################################################
        //random number parameters

        //random number generator
        cv::RNG randomGenerator;

        //length of random array initialization
        long countOfRandomNumb;

        //pre - initialize the randomNumbers for better performance
        std::vector<int> randomN, randomMinDist, randomX, randomY, randomT, randomTN;

        //###################################################################################

        //check if something is moving
        bool isMove;

        //for init, count number of runs
        int runs;


        cv::Mat* resultMap;
        std::vector<cv::Mat> currentFeatures;

        std::vector<float*> currentFeaturesM_Pt;
        std::vector<uchar*> currentFeaturesC_Pt;
        uchar* resultMap_Pt;

        std::vector<std::vector<float*>>B_Mag_Pts;
        std::vector<std::vector<uchar*>>B_Col_Pts;

        double sumMagnitude;
        double formerMeanMag;
        float formerDistanceBack;

        //####################################################################################
        //N - Number: Defining the size of the background-history-model
        // number of samples per pixel
        //size of background history B(x_i)
        int N;
        // background model
        std::vector<std::vector<cv::Mat>> backgroundModel;
        //####################################################################################
        //####################################################################################
        //R-Threshhold - Variables
        //minimal Threshold for foreground and initial value of R(x_i)
        // radius of the sphere -> lower border boundary
        double R_lower;

        //factor which defines new threshold of R(x_i) together with meanMinDist(x_i)
        // scale for the sphere threshhold to define pixel-based Thresholds
        double R_scale;

        //decreasing / increasing factor of R(x_i)
        // increasing/decreasing factor for the r-Threshold based on the result of rTreshScale * meanMinDistBackground
        double R_incdec;

        cv::Mat actualR;
        float* actualR_Pt; //new pixel-based r-threshhold -> pointer to arrays
        //#####################################################################################
        //####################################################################################
        //counter for minimal distance to background
        // Defining the number of background-model-images, which have a lowerDIstance to the current Image than defined by the R-Thresholds, that are necessary
        // to decide that this pixel is background
        int Raute_min;
        //#####################################################################################
        //####################################################################################
        //initial value of inverse update factor T(x_i)
        // Initialize the background-model update rate
        double T_init;

        //increasing Factor of the update rate 1/T(x_i)
        // scale that defines the increasing of the update rate of the background model, if the current pixel is background
        //--> more frequently updates if pixel is background because, there shouln't be any change
        double T_inc;

        //upper boundary of T(x_i)
        // defining an upper value, that nrSubsampling can achieve, thus it doesn't reach to an infinite value, where actually no update is possible
        // at all
        double T_upper;

        //lower boundary of T(x_i)
        // defining a minimum value for nrSubsampling --> base value 2.0
        double T_lower;

        //decreasing factor of the update rate 1/T(x_i)
        // opposite scale to increasingRateScale, for decreasing the update rate of the background model, if the current pixel is foreground
        //--> Thesis: Our Foreground is a real moving object -> thus the background-model is good, so don't update it
        double T_dec;

        //holds update rate of current pixel
        cv::Mat actualT;
        float* actualT_Pt;

        //#####################################################################################


        cv::Mat sobelX, sobelY;
};
Q_DECLARE_METATYPE(PBAS)

#endif // PBAS_H

