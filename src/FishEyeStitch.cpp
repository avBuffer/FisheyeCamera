#include "include/FishEyeStitch.h"
#include "include/FishEyeImgProcess.h"
#include "include/FishEyeUtils.h"


FishEyeStitch::FishEyeStitch()
{
}


FishEyeStitch::~FishEyeStitch()
{
}


map<string, vector<cv::Mat> >* FishEyeStitch::splitImgs(
    map<string, vector<cv::Mat> >* imgsMap, string& outPath)
{
    map<string, vector<cv::Mat> >* retImgsMap = new map<string, vector<cv::Mat> >();

    FishEyeImgProcess fishEyeImgProcess;
    int idx = 0;
    map<string, vector<cv::Mat> >::iterator it;
    for (it = imgsMap->begin(); it != imgsMap->end(); ++it)
    {
        string fileName = it->first;       
        vector<cv::Mat> imgs = it->second;

        vector<cv::Mat> retImgs = fishEyeImgProcess.splitAllImgs(imgs, true);
        if (retImgs.size() < 1)
        {
            cout << "splitImgs fileName=" << fileName << " retImgs is empty!" << endl;
            continue;
        }

        retImgsMap->insert(make_pair(fileName, retImgs));
        idx++;

        int jdx = 0;
        FishEyeUtils fishEyeUtils;
        vector<cv::Mat>::iterator itr;
        for (itr = retImgs.begin(); itr != retImgs.end(); ++itr)
        {
            cv::Mat retImg = *itr;
            if (retImg.empty())
            {
                cout << "splitImgs retImg is empty!" << endl;
                continue;
            }
            
            string jdxStr = fishEyeUtils.int2str(jdx);
            cv::imwrite(outPath + "\\" + fileName + "_stitch_" + jdxStr + ".jpg", retImg);
            cout << "splitImgs no1#" << idx << "no2#" << jdx << " file:" << 
                outPath + "\\" + fileName + "_stitch_" << jdxStr << ".jpg" << endl;          
            jdx++;
        }           
    }

    cout << "splitImgs retImgsMap size=" << retImgsMap->size() << endl;
    return retImgsMap;
}


void FishEyeStitch::stitchImage(map<string, vector<cv::Mat> >* imgsMap, 
    string& outPath)
{
    FishEyeImgProcess fishEyeImgProcess;
    int idx = 0;
    map<string, vector<cv::Mat> >::iterator it;
    for (it = imgsMap->begin(); it != imgsMap->end(); ++it)
    {
        string fileName = it->first;       
        vector<cv::Mat> imgs = it->second;

        cv::Mat retMat = fishEyeImgProcess.stitchBatchImgs(imgs);
        if (retMat.empty())
        {
            cout << "fileName=" << fileName << " retMat is empty!" << endl;
        }
        else 
        {
            cv::imwrite(outPath + "\\" + fileName + "_pano.jpg", retMat);
            cout << "stitchImage no#" << idx << " file:" << 
                outPath + "\\" + fileName + "_pano.jpg" << endl;          
            idx++;
        }
    }
}


void FishEyeStitch::stitchLRImage(map<string, cv::Mat>* leftImgs, 
    map<string, cv::Mat>* rightImgs, string& outPath)
{
    FishEyeImgProcess fishEyeImgProcess;

    int idx = 0;
    map<string, cv::Mat>::iterator it;
    for (it = leftImgs->begin(); it != leftImgs->end(); ++it)
    {
        string leftFileName = it->first;       
        cv::Mat leftImg = it->second;

        if (leftImg.empty())
        {
            cout << "stitchLRImage leftImg is empty!" << endl;
            continue;
        }

        map<string, cv::Mat>::iterator itr;
        for (itr = rightImgs->begin(); itr != rightImgs->end(); ++itr)
        {
            string rightFileName = itr->first; 
            
            cout << "stitchLRImage leftFileName=" << leftFileName << 
                " rightFileName=" << rightFileName << endl;
            if (strcmp(leftFileName.c_str(), rightFileName.c_str()) == 0) 
            {
                cv::Mat rightImg = itr->second;
                if (rightImg.empty())
                {
                    cout << "surfStitchLRImage rightImg is empty!" << endl;
                    break;
                }
                
                vector<cv::Mat> left_right_imgs; 
                left_right_imgs.push_back(leftImg);                
                left_right_imgs.push_back(rightImg);
                
                cv::Mat left_right_img = fishEyeImgProcess.stitchBatchImgs(left_right_imgs);
                if (left_right_img.empty())
                {
                    cout << "fileName=" << leftFileName << " left_right_img is empty!" << endl;
                }
                else 
                {
                    cv::imwrite(outPath + "\\" + leftFileName + "_pano_left_right.jpg", 
                        left_right_img);
                    cout << "stitchImage no#" << idx << " file:" << 
                        outPath + "\\" + leftFileName + "_pano_left_right.jpg" << endl;          
                }

                vector<cv::Mat> right_left_imgs; 
                right_left_imgs.push_back(rightImg);                
                right_left_imgs.push_back(leftImg);

                cv::Mat right_left_img = fishEyeImgProcess.stitchBatchImgs(right_left_imgs);
                if (right_left_img.empty())
                {
                    cout << "fileName=" << rightFileName << " right_left_img is empty!" << endl;
                }
                else 
                {
                    cv::imwrite(outPath + "\\" + rightFileName + "_pano_right_left.jpg", 
                        right_left_img);
                    cout << "stitchImage no#" << idx << " file:" << 
                        outPath + "\\" + leftFileName + "_pano_right_left.jpg" << endl;          
                }       
    
                idx++;
                break;                
            }         
        }
    }
}


void FishEyeStitch::surfStitchImage(map<string, vector<cv::Mat> >* imgsMap, 
    string& outPath)
{
    FishEyeImgProcess fishEyeImgProcess;
    
    int idx = 0;
    map<string, vector<cv::Mat> >::iterator it;
    for (it = imgsMap->begin(); it != imgsMap->end(); ++it)
    {
        string fileName = it->first;       
        vector<cv::Mat> imgs = it->second;

        cout << "fileName=" << fileName << " imgs size=" << imgs.size() << endl;
        if (imgs.size() < 4)
        {
            continue;
        }

        cv::Mat mat0 = imgs[0];
        cv::Mat mat1 = imgs[1];
        cv::Mat mat2 = imgs[2];
        cv::Mat mat3 = imgs[3];

        cv::Mat panoMat02 = fishEyeImgProcess.transposeImg(
            fishEyeImgProcess.surfStitch2Imgs(mat0, mat2), 1);
        if (panoMat02.empty())
        {
            cout << "fileName=" << fileName << " panoMat02 is empty!" << endl;
        }
        else 
        {
            cv::imwrite(outPath + "\\" + fileName + "_surf_pano_02.jpg", panoMat02);
            cout << "stitchImage no#" << idx << " file:" << 
                outPath + "\\" + fileName + "_surf_pano_02.jpg" << endl;
        }

        cv::Mat panoMat20 = fishEyeImgProcess.transposeImg(
            fishEyeImgProcess.surfStitch2Imgs(mat2, mat0), 1);
        if (panoMat20.empty())
        {
            cout << "fileName=" << fileName << " panoMat20 is empty!" << endl;
        }
        else 
        {
            cv::imwrite(outPath + "\\" + fileName + "_surf_pano_20.jpg", panoMat20);
            cout << "stitchImage no#" << idx << " file:" << 
                outPath + "\\" + fileName + "_surf_pano_20.jpg" << endl;
        }


        cv::Mat panoMat03 = fishEyeImgProcess.transposeImg(
            fishEyeImgProcess.surfStitch2Imgs(mat0, mat3), 1);
        if (panoMat03.empty())
        {
            cout << "fileName=" << fileName << " panoMat03 is empty!" << endl;
        }
        else 
        {
            cv::imwrite(outPath + "\\" + fileName + "_surf_pano_03.jpg", panoMat03);
            cout << "stitchImage no#" << idx << " file:" << 
                outPath + "\\" + fileName + "_surf_pano_03.jpg" << endl;
        }

        cv::Mat panoMat30 = fishEyeImgProcess.transposeImg(
            fishEyeImgProcess.surfStitch2Imgs(mat3, mat0), 1);
        if (panoMat30.empty())
        {
            cout << "fileName=" << fileName << " panoMat30 is empty!" << endl;
        }
        else 
        {
            cv::imwrite(outPath + "\\" + fileName + "_surf_pano_30.jpg", panoMat30);
            cout << "stitchImage no#" << idx << " file:" << 
                outPath + "\\" + fileName + "_surf_pano_30.jpg" << endl;
        }

        
        cv::Mat panoMat12 = fishEyeImgProcess.transposeImg(
            fishEyeImgProcess.surfStitch2Imgs(mat1, mat2), 1);
        if (panoMat12.empty())
        {
            cout << "fileName=" << fileName << " panoMat12 is empty!" << endl;
        }
        else 
        {
            cv::imwrite(outPath + "\\" + fileName + "_surf_pano_12.jpg", panoMat12);
            cout << "stitchImage no#" << idx << " file:" << 
                outPath + "\\" + fileName + "_surf_pano_12.jpg" << endl;
        }

        cv::Mat panoMat21 = fishEyeImgProcess.transposeImg(
            fishEyeImgProcess.surfStitch2Imgs(mat2, mat1), 1);
        if (panoMat21.empty())
        {
            cout << "fileName=" << fileName << " panoMat21 is empty!" << endl;
        }
        else 
        {
            cv::imwrite(outPath + "\\" + fileName + "_surf_pano_21.jpg", panoMat21);
            cout << "stitchImage no#" << idx << " file:" << 
                outPath + "\\" + fileName + "_surf_pano_21.jpg" << endl;
        }

        cv::Mat panoMat13 = fishEyeImgProcess.transposeImg(
            fishEyeImgProcess.surfStitch2Imgs(mat1, mat3), 1);
        if (panoMat13.empty())
        {
            cout << "fileName=" << fileName << " panoMat13 is empty!" << endl;
        }
        else 
        {
            cv::imwrite(outPath + "\\" + fileName + "_surf_pano_13.jpg", panoMat13);
            cout << "stitchImage no#" << idx << " file:" << 
                outPath + "\\" + fileName + "_surf_pano_13.jpg" << endl;
        }
        
        cv::Mat panoMat31 = fishEyeImgProcess.transposeImg(
            fishEyeImgProcess.surfStitch2Imgs(mat3, mat1), 1);
        if (panoMat31.empty())
        {
            cout << "fileName=" << fileName << " panoMat13 is empty!" << endl;
        }
        else 
        {
            cv::imwrite(outPath + "\\" + fileName + "_surf_pano_31.jpg", panoMat31);
            cout << "stitchImage no#" << idx << " file:" << 
                outPath + "\\" + fileName + "_surf_pano_31.jpg" << endl;
        }

        idx++;            
    }
}


void FishEyeStitch::surfStitchLRImage(map<string, cv::Mat>* leftImgs, 
    map<string, cv::Mat>* rightImgs, string& outPath)
{
    FishEyeImgProcess fishEyeImgProcess;
    
    int idx = 0;
    map<string, cv::Mat>::iterator it;
    for (it = leftImgs->begin(); it != leftImgs->end(); ++it)
    {
        string leftFileName = it->first;       
        cv::Mat leftImg = it->second;

        if (leftImg.empty())
        {
            cout << "surfStitchLRImage leftImg is empty!" << endl;
            continue;
        }

        map<string, cv::Mat>::iterator itr;
        for (itr = rightImgs->begin(); itr != rightImgs->end(); ++itr)
        {
            string rightFileName = itr->first; 
            if (strcmp(leftFileName.c_str(), rightFileName.c_str()) == 0) 
            {
                cout << "surfStitchLRImage leftFileName=" << leftFileName << 
                    " rightFileName=" << rightFileName << endl;
                
                cv::Mat rightImg = itr->second;
                if (rightImg.empty())
                {
                    cout << "surfStitchLRImage rightImg is empty!" << endl;
                    continue;
                }

                cout << "leftImg:cols=" << leftImg.cols << " rows=" << leftImg.rows <<
                    " rightImg:cols=" << rightImg.cols << " rows=" << rightImg.rows <<endl;
                
                cv::Mat left_right_img = 
                    fishEyeImgProcess.surfStitch2Imgs(leftImg, rightImg);                 
                if (left_right_img.empty())
                {
                    cout << "fileName=" << leftFileName << 
                        " left_right_img is empty!" << endl;
                }
                else 
                {
                    //cv::Mat left_right_pano =
                    //    imageProcess.transposeImg(left_right_img, 1);
                    
                    cv::imwrite(outPath + "\\" + leftFileName + "_surf_left_right_pano.jpg", 
                        left_right_img);
                    
                    cout << "stitchImage no#" << idx << " file:" << 
                        outPath + "\\" + leftFileName + "_surf_left_right_pano.jpg" << endl;
                }
                
                cv::Mat right_left_img = 
                      fishEyeImgProcess.surfStitch2Imgs(rightImg, leftImg);
                if (right_left_img.empty())
                {
                    cout << "fileName=" << rightFileName << 
                        " right_left_img is empty!" << endl;
                }
                else 
                {
                    //cv::Mat right_left_pano =
                    //    imageProcess.transposeImg(right_left_img, 1);
                    
                    cv::imwrite(outPath + "\\" + rightFileName + "_surf_right_left_pano.jpg", 
                        right_left_img);
                    
                    cout << "stitchImage no#" << idx << " file:" << 
                        outPath + "\\" + rightFileName + "_surf_right_left_pano.jpg" << endl;
                }

                idx++;
                break;                
            }         
        }
    }    
}
