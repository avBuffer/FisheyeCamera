#include "include/FishEyeUnwrap.h"
#include "include/FishEyeUtils.h"
#include "include/FishEyeImgProcess.h"


FishEyeUnwrap::FishEyeUnwrap()
{
}


FishEyeUnwrap::~FishEyeUnwrap()
{
}


cv::Mat FishEyeUnwrap::unwrapImg(cv::Mat& imgMat, cv::Matx33d& intrinsic_matrix, 
    cv::Vec4d& distortion_coeffs, cv::Size& image_size, cv::Mat& mapx, cv::Mat& mapy, cv::Mat& R)
{
    cv::Mat retMat;    
    if (imgMat.empty())
    {
        cout << "unwrapImg Origin imgMat is empty!" << endl;
        return retMat;    
    }
    
    cv::fisheye::initUndistortRectifyMap(intrinsic_matrix, distortion_coeffs, 
        R, intrinsic_matrix, image_size, CV_32FC1, mapx, mapy);
    
    retMat = imgMat.clone();
    cv::remap(imgMat, retMat, mapx, mapy, cv::INTER_LINEAR);
    
    if (retMat.empty())
    {
        cout << "unwrapImg retMat is empty!" << endl;
    }
    
    return retMat;    
}


int FishEyeUnwrap::unwrapBatchImgs(map<string, vector<cv::Mat> >* imgsMap, 
    string& outPath, cv::Matx33d& intrinsic_matrix, cv::Vec4d& distortion_coeffs, 
    cv::Size& image_size, cv::Mat& mapx, cv::Mat& mapy, cv::Mat& R)
{
    FishEyeUtils fishEyeUtils;
  
    map<string, vector<cv::Mat> >::iterator it;
    for (it = imgsMap->begin(); it != imgsMap->end(); ++it)
    {
        string fileName = it->first;
      
        vector<cv::Mat> imgs = it->second;        
        vector<cv::Mat>::iterator itr;
        int idx = 0;
        for (itr = imgs.begin(); itr != imgs.end(); ++itr)
        {
            cv::Mat imgMat = *itr;
            cv::Mat unWrapMat = unwrapImg(imgMat, intrinsic_matrix, distortion_coeffs,
                image_size, mapx, mapy, R);

            if (unWrapMat.empty())
            {
                cout << "unwrapBatchImgs unWrapMat is empty!" << endl;
                return -1;
            }

            string idxStr = fishEyeUtils.int2str(idx);  
            cv::imwrite(outPath + "\\" + fileName + "_unwrap_" + idxStr + ".jpg", unWrapMat);
            
            cout << "unwrapBatchImgs no#" << idx << " file:" << 
                outPath + "\\" + fileName + "_unwrap_" + idxStr + ".jpg" << endl;
            
            idx++;
        }
    }

    return 0;    
}


cv::Mat FishEyeUnwrap::optUnwrapImg(cv::Mat& imgMat, cv::Matx33d& intrinsic_matrix, 
    cv::Vec4d& distortion_coeffs, bool isRight)
{
    cv::Mat retMat;    
    if (imgMat.empty())
    {
        cout << "optUnwrapImg Origin imgMat is empty!" << endl;
        return retMat;    
    }
   
    int unwrap_x = round((FOV_HORI / 90) * SAMPLE_BASE);
    int unwrap_y = round((FOV_VERT / 90) * SAMPLE_BASE);    

    cv::Mat map_x = cv::Mat(cv::Size(unwrap_x, unwrap_y), CV_32FC1);
    cv::Mat map_y = cv::Mat(cv::Size(unwrap_x, unwrap_y), CV_32FC1);

    // calibration params
    double fx = intrinsic_matrix(0, 0);
    double fy = intrinsic_matrix(1, 1); 
    double cx = intrinsic_matrix(0, 2); 
    double cy = intrinsic_matrix(1, 2); 

    // distortion params
    double k1 = distortion_coeffs(0);
    double k2 = distortion_coeffs(1);
    double k3 = distortion_coeffs(2);
    double k4 = distortion_coeffs(3);

    double theta = 0.0;
    double phi = 0.0;
    double r_theta = 0.0;
    double phi2, phi4, phi6, phi8;
    double xd, yd;
    
    float u, v;

    for (int xi = 0; xi < unwrap_x; xi++)
    {
        // -4*math.pi/4
        theta = xi * FOV_HORI * (CV_PI / 180 / (unwrap_x - 1));   

        if (isRight)
        {
            theta -= CV_PI;            
        }

        for (int yi = 0; yi < unwrap_y; yi++)
        {
            phi = yi * FOV_VERT * (CV_PI / 180 / (unwrap_y - 1));
            
            phi2 = phi * phi;
            phi4 = phi2 * phi2;
            phi6 = phi2 * phi4;
            phi8 = phi4 * phi4;
            
            r_theta = phi*(1 + k1*phi2 + k2*phi4 + k3*phi6 + k4*phi8);
            
            xd = r_theta * cos(theta);
            yd = r_theta * sin(theta);            
            
            u = fx * xd + cx;
            v = fy * yd + cy;
            
            if (u < 0 ) 
            {
                u = 0;
            }
            else if (u > 2047)
            {
                u = 2047;
            }
            
            if (v < 0 ) 
            {
                v = 0;
            }
            else if (v > 2047)
            {
                v = 2047;
            }            

            map_x.at<float>(yi, xi) = u;
            map_y.at<float>(yi, xi) = v;

        }
    }

    retMat = imgMat.clone();
    cv::remap(imgMat, retMat, map_x, map_y, cv::INTER_LINEAR);
    
    if (retMat.empty())
    {
        cout << "optUnwrapImg retMat is empty!" << endl;
    }
        
    return retMat;
}


map<string, vector<cv::Mat> >* FishEyeUnwrap::optUnwrapBatchImgs(
    map<string, vector<cv::Mat> >* imgsMap, string& outPath, 
    cv::Matx33d& intrinsic_matrix, cv::Vec4d& distortion_coeffs, bool isRight)
{
    map<string, vector<cv::Mat> >* unwrapImgsMap = 
        new map<string, vector<cv::Mat> >();

    FishEyeUtils fishEyeUtils;
  
    map<string, vector<cv::Mat> >::iterator it;
    for (it = imgsMap->begin(); it != imgsMap->end(); ++it)
    {
        string fileName = it->first;

        int idx = 0;
        vector<cv::Mat> unwrapImgs;      
      
        vector<cv::Mat> imgs = it->second;        
        vector<cv::Mat>::iterator itr;
        for (itr = imgs.begin(); itr != imgs.end(); ++itr)
        {
            cv::Mat imgMat = *itr;
            cv::Mat unWrapMat = optUnwrapImg(imgMat, intrinsic_matrix, 
                distortion_coeffs, isRight);

            if (unWrapMat.empty())
            {
                cout << "optUnwrapBatchImgs unWrapMat is empty!" << endl;
                continue;
            }

            if ((idx % 2) != 0)
            {
                cv::Mat tmpMat_vh = (cv::Mat_<double>(2, 3) << -1, 0, 
                    unWrapMat.cols, 0, -1, unWrapMat.rows);
                
                cv::Mat tmpMat;
                cv::warpAffine(unWrapMat, tmpMat, tmpMat_vh, unWrapMat.size());                
                unWrapMat = tmpMat;
                
                cout << "idx:" << idx << " unWrapMat.empty=" << unWrapMat.empty() << endl;
            }
    
            unwrapImgs.push_back(unWrapMat);
            cout << "fileName=" << fileName << " unwrapImgs size=" << unwrapImgs.size() << endl;
            
            string idxStr = fishEyeUtils.int2str(idx);  
            cv::imwrite(outPath + "\\" + fileName + "_opt_unwrap_" + idxStr + ".jpg", unWrapMat);
            
            cout << "unwrapBatchImgs no#" << idx << " file:" << 
                outPath + "\\" + fileName + "_opt_unwrap_" + idxStr + ".jpg" << endl;
            
            idx++;
        }

        unwrapImgsMap->insert(make_pair(fileName, unwrapImgs));
    }

    return unwrapImgsMap;    
}


map<string, cv::Mat>* FishEyeUnwrap::optUnwrapBatchLRImgs(
    map<string, cv::Mat>* imgsMap, string& outPath, 
    cv::Matx33d& intrinsic_matrix, cv::Vec4d& distortion_coeffs, 
    ofstream& optUnwrapFile, bool isRight)
{
    map<string, cv::Mat>* unwrapImgsMap = new map<string, cv::Mat>();

    FishEyeUtils fishEyeUtils;  
    int idx = 0;

    FishEyeImgProcess fishEyeImgProcess;
  
    map<string, cv::Mat>::iterator it;
    for (it = imgsMap->begin(); it != imgsMap->end(); ++it)
    {
        string fileName = it->first;
        cv::Mat imgMat = it->second;
        
        cv::Mat unWrapImg = optUnwrapImg(imgMat, intrinsic_matrix, 
            distortion_coeffs, isRight);

        if (unWrapImg.empty())
        {
            cout << "optUnwrapBatchImgs unWrapMat is empty!" << endl;
            continue;
        }

        if (isRight)
        {
            cv::Mat tmpMat_vh = (cv::Mat_<double>(2, 3) << -1, 0, 
                unWrapImg.cols, 0, -1, unWrapImg.rows);
            
            cv::Mat tmpMat;
            cv::warpAffine(unWrapImg, tmpMat, tmpMat_vh, unWrapImg.size());                
            unWrapImg = tmpMat;        

            string rightPath = outPath + "\\right\\";
            fishEyeUtils.makeDir(rightPath);
            
            string rightFile = rightPath + fileName + "_opt_unwrap_left.jpg";            
            cv::imwrite(rightFile, fishEyeImgProcess.transposeImg(unWrapImg, 0));
            
            cout << "unwrapBatchImgs no#" << idx << " file:" << rightFile << endl;            
            optUnwrapFile << fileName << "=" << rightFile << endl;
        }
        else
        {            
            string leftPath = outPath + "\\left\\";
            fishEyeUtils.makeDir(leftPath);

            string leftFile = leftPath + fileName + "_opt_unwrap_left.jpg";            
            cv::imwrite(leftFile, fishEyeImgProcess.transposeImg(unWrapImg, 0));
            
            cout << "unwrapBatchImgs no#" << idx << " file:" << leftFile << endl;            
            optUnwrapFile << fileName << "=" << leftFile << endl;
        }
            
        unwrapImgsMap->insert(make_pair(fileName, 
            fishEyeImgProcess.transposeImg(unWrapImg, 0)));
        
        idx++;
    }

    return unwrapImgsMap;    
}
