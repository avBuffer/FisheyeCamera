#ifndef _FISHEYESTITCH_H_
#define _FISHEYESTITCH_H_
#pragma once

#include <stdio.h>  
#include <io.h>  
#include <string>
#include <opencv2\opencv.hpp>
#include <fstream>
#include <cmath>

using namespace std;


class FishEyeStitch
{
    public:
        FishEyeStitch();
        ~FishEyeStitch();
        
    public:
        void stitchImage(map<string, vector<cv::Mat> >* imgsMap, string& outPath);
        void stitchLRImage(map<string, cv::Mat>* leftImgs, 
            map<string, cv::Mat>* rightImgs, string& outPath);
        
        map<string, vector<cv::Mat> >* splitImgs(map<string, vector<cv::Mat> >* imgsMap, 
            string& outPath);

        void surfStitchImage(map<string, vector<cv::Mat> >* imgsMap, string& outPath);
        void surfStitchLRImage(map<string, cv::Mat>* leftImgs, map<string, cv::Mat>* rightImgs, 
            string& outPath);        
};
#endif
