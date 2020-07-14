#ifndef _FISHEYECALIBRATION_H_
#define _FISHEYECALIBRATION_H_
#pragma once

#include <stdio.h>  
#include <io.h>  
#include <string>
#include <opencv2\opencv.hpp>
#include <fstream>
using namespace std;


class FishEyeCalibration
{
    public:
        FishEyeCalibration();
        ~FishEyeCalibration();

    public:
        cv::Matx33d mIntrinsicMatrix;
        cv::Vec4d mDistortionCoeffs;
        cv::Size mImageSize;
        cv::Mat mMapx;
        cv::Mat mMapy;
        cv::Mat mR;
        
        int calibrateImg(string& inPath, string& outPath);    
};
#endif
