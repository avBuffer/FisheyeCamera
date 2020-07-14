#ifndef _FISHEYEIMGPROCESS_H_
#define _FISHEYEIMGPROCESS_H_
#pragma once

#include <opencv2/opencv.hpp> 
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/stitching.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;


class FishEyeImgProcess
{
    public:
        FishEyeImgProcess();
        ~FishEyeImgProcess();

    public:    
        vector<cv::Mat> splitImg(cv::Mat& src_img, int m, int n);    
        int splitLRImgs(cv::Mat& img, cv::Mat& leftImg, cv::Mat& rightImg);
        
        map<string, vector<cv::Mat> >* splitBatchImgs(string& path, int m, 
            int n, string& outPath);
        
        int splitBatchLRImgs(string& inPath, string& outPath, 
            map<string, cv::Mat>* leftImgs, map<string, cv::Mat>* rightImgs);
        
        cv::Mat rotateImg(cv::Mat& src, int angle);
        cv::Mat transposeImg(cv::Mat& img, int flag);
            
        cv::Mat stitchBatchImgs(std::vector<cv::Mat>& imgs);
        cv::Mat surfStitch2Imgs(cv::Mat& srcImage1, cv::Mat& srcImage2);
        vector<cv::Mat> getImgs(string& path);
        
        void stitchImage(map<string, vector<cv::Mat> >* imgsMap, string& outPath);
        vector<cv::Mat> splitAllImgs(vector<cv::Mat>& imgs, bool isTranspose);        
};
#endif
