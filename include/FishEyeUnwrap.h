#ifndef _FISHEYEUNWRAP_H_
#define _FISHEYEUNWRAP_H_
#pragma once

#include <stdio.h>  
#include <io.h>  
#include <string> 
#include <opencv2\opencv.hpp>
#include <fstream>
#include <cmath>

using namespace std;

// samples per 90 degree
#define SAMPLE_BASE 900
#define FOV_HORI 360
#define FOV_VERT 100


class FishEyeUnwrap
{
    public:
        FishEyeUnwrap();
        ~FishEyeUnwrap();

    public:        
        cv::Mat unwrapImg(cv::Mat& imgMat, cv::Matx33d& intrinsic_matrix, 
            cv::Vec4d& distortion_coeffs, cv::Size& image_size, cv::Mat& mapx, 
            cv::Mat& mapy, cv::Mat& R);
        
        int unwrapBatchImgs(map<string, vector<cv::Mat> >* imgsMap, 
            string& outPath, cv::Matx33d& intrinsic_matrix, cv::Vec4d& distortion_coeffs, 
            cv::Size& image_size, cv::Mat& mapx, cv::Mat& mapy, cv::Mat& R);

        cv::Mat optUnwrapImg(cv::Mat& imgMat, cv::Matx33d& intrinsic_matrix, 
            cv::Vec4d& distortion_coeffs, bool isRight);
        
        map<string, vector<cv::Mat> >* optUnwrapBatchImgs(
            map<string, vector<cv::Mat> >* imgsMap, string& outPath, 
            cv::Matx33d& intrinsic_matrix, cv::Vec4d& distortion_coeffs, bool isRight); 

        map<string, cv::Mat>* optUnwrapBatchLRImgs(map<string, cv::Mat>* imgsMap, 
            string& outPath, cv::Matx33d& intrinsic_matrix, cv::Vec4d& distortion_coeffs, 
            ofstream& optUnwrapFile, bool isRight);        
};
#endif
