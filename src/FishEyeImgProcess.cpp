#include "include/FishEyeImgProcess.h"
#include "include/FishEyeUtils.h"


FishEyeImgProcess::FishEyeImgProcess()
{
}


FishEyeImgProcess::~FishEyeImgProcess()
{
}


vector<cv::Mat> FishEyeImgProcess::splitImg(cv::Mat& src_img, int m, int n)
{
    vector<cv::Mat> ceil_img;
    
    int height = src_img.rows;
    int width  = src_img.cols;

    int ceil_height = height/m;
    int ceil_width  = width/n; 

    for(int i = 0; i<m; i++)
    {
        for(int j = 0; j<n; j++)
        {
            //Rect rect(i+j*ceil_width, j+i*ceil_height, ceil_width, ceil_height);
            cv::Rect rect(j*ceil_width, i*ceil_height, ceil_width, ceil_height);
            
            cv::Mat roi_img;
            src_img(rect).copyTo(roi_img);
            ceil_img.push_back(roi_img);             
        }
    }

    return ceil_img;
}


int FishEyeImgProcess::splitLRImgs(cv::Mat& img, cv::Mat& leftImg, cv::Mat& rightImg)
{
    int height = img.rows;
    int width  = img.cols;

    int ceil_height = height;
    int ceil_width  = width / 2; 
    
    cv::Rect rect;
    rect = cv::Rect(0, 0, ceil_width, ceil_height);    
    img(rect).copyTo(leftImg);
    
    rect = cv::Rect((width - ceil_width), (height - ceil_height), 
        ceil_width, ceil_height);    
    img(rect).copyTo(rightImg);
    
    if (leftImg.empty() || rightImg.empty()) 
    {
        cout << "splitLRImgs leftImg or rightImg is empty!" << endl;
        return -1;
    }

    return 0;
}


map<string, vector<cv::Mat> >* FishEyeImgProcess::splitBatchImgs(string& inPath, 
    int m, int n, string& outPath)
{
    map<string, vector<cv::Mat> >* splitImgsMap = 
        new map<string, vector<cv::Mat> >();
        
    FishEyeUtils fishEyeUtils;
    vector<string> files;
    fishEyeUtils.getFiles(inPath, files, false);

    vector<string>::iterator it;
    for (it = files.begin(); it < files.end(); ++it)
    {
        string file = *it;
        cv::Mat image = cv::imread(inPath + "\\" + file);
        if (image.empty())
        {
            cout << "splitBatchImgs Current file: " << file << " is empty!" << endl;
            continue;
        }
        
        vector<cv::Mat> ceilImgs;
        ceilImgs = splitImg(image, m, n);        

        vector<string> fileNames;
        string pattern = ".";
        fileNames = fishEyeUtils.split(file, pattern);
        string tmpFileName = fileNames[0];
        
        splitImgsMap->insert(make_pair(tmpFileName, ceilImgs));

        int idx = 0;
        vector<cv::Mat>::iterator itr;
        int angle = 0;
        for (itr = ceilImgs.begin(); itr < ceilImgs.end(); ++itr)
        {   
            string idxStr = fishEyeUtils.int2str(idx);            
            cv::Mat img = *itr;
            
            if (img.empty())
            {
                cout << "splitBatchImgs origin image is empty! " << endl;
                continue;
            }

            cv::Mat newImg = rotateImg(img, angle);
            cv::imwrite(outPath + "\\" + tmpFileName + "_split_" + idxStr + ".jpg", newImg); 
            cout << "splitBatchImgs no#" << idx << " file:" << 
                outPath + "\\" + tmpFileName + "_split_" + idxStr + ".jpg" << endl;
            
            idx++; 
            angle = -angle;
        }
        
    }

    return splitImgsMap;
}


int FishEyeImgProcess::splitBatchLRImgs(string& inPath, string& outPath, 
    map<string, cv::Mat>* leftImgs, map<string, cv::Mat>* rightImgs)
{
    FishEyeUtils fishEyeUtils;
    vector<string> files;
    fishEyeUtils.getFiles(inPath, files, false);

    int idx = 0;
    vector<string>::iterator it;
    for (it = files.begin(); it < files.end(); ++it)
    {
        string file = *it;
        cv::Mat image = cv::imread(inPath + "\\" + file);
        if (image.empty())
        {
            cout << "splitBatchImgs Current file: " << file << " is empty!" << endl;
            continue;
        }

        cv::Mat leftImg, rightImg;
        int ret = splitLRImgs(image, leftImg, rightImg);
        if (ret != 0)
        {
            continue;
        }

        vector<string> fileNames;
        string pattern = ".";
        fileNames = fishEyeUtils.split(file, pattern);
        string tmpFileName = fileNames[0];
        
        leftImgs->insert(make_pair(tmpFileName, leftImg));        
        rightImgs->insert(make_pair(tmpFileName, rightImg));

        string leftPath = outPath + "\\left\\";
        fishEyeUtils.makeDir(leftPath);
        
        cv::imwrite(leftPath + tmpFileName + "_split_left.jpg", leftImg); 
        cout << "splitBatchImgs no#" << idx << " file:" << 
            leftPath + tmpFileName + "_split_left.jpg" << endl;


        string rightPath = outPath + "\\right\\";
        fishEyeUtils.makeDir(rightPath);
        
        cv::imwrite(rightPath + tmpFileName + "_split_right.jpg", rightImg); 
        cout << "splitBatchImgs no#" << idx << " file:" << 
            rightPath + tmpFileName + "_split_right.jpg" << endl;
            
        idx++; 
    }

    return 0;
}


cv::Mat FishEyeImgProcess::rotateImg(cv::Mat& src, int angle)
{
    float alpha = angle * CV_PI / 180;
    float rotateMat[3][3] = {{cos(alpha), -sin(alpha), 0}, {sin(alpha), cos(alpha), 0}, {0, 0, 1} };
        
    int nSrcRows = src.rows;
    int nSrcCols = src.cols;
    
    float a1 = nSrcCols * rotateMat[0][0] ;
    float b1 = nSrcCols * rotateMat[1][0] ;
    float a2 = nSrcCols * rotateMat[0][0] + nSrcRows * rotateMat[0][1];
    float b2 = nSrcCols * rotateMat[1][0] + nSrcRows * rotateMat[1][1];
    float a3 = nSrcRows * rotateMat[0][1];
    float b3 = nSrcRows * rotateMat[1][1];
    
    float kxMin =  min( min( min(0.0f,a1), a2 ), a3);
    float kxMax =  max( max( max(0.0f,a1), a2 ), a3);
    float kyMin =  min( min( min(0.0f,b1), b2 ), b3);
    float kyMax =  max( max( max(0.0f,b1), b2 ), b3);
    
    int nRows = abs(kxMax - kxMin);
    int nCols = abs(kyMax - kyMin);
    cv::Mat dst(nRows, nCols, src.type(), cv::Scalar::all(0)); 
    
    for( int i = 0; i < nRows; ++i)
    {       
        for (int  j = 0; j < nCols; ++j)
        {
            int x =  (j + kxMin) * rotateMat[0][0] -
                (i + kyMin) * rotateMat[0][1] ;
            int y = -(j + kxMin) * rotateMat[1][0] + 
                (i + kyMin) * rotateMat[1][1] ;
            
            if( (x >= 0) && (x < nSrcCols) && 
                (y >= 0) && (y < nSrcRows) ) 
            {  
                dst.at<cv::Vec3b>(i,j) = 
                    src.at<cv::Vec3b>(y,x);
            }
        }
    }
    
    return dst;    
}


cv::Mat FishEyeImgProcess::transposeImg(cv::Mat& img, int flag)
{
    cv::Mat retImg, tmpImg;     
    cv::transpose(img, tmpImg);
    cv::flip(tmpImg, retImg, flag);

    if (retImg.empty())
    {
        cout << "transposeImg retImg is empty!" << endl;
    }
   
    return retImg;
}


cv::Mat FishEyeImgProcess::stitchBatchImgs(std::vector<cv::Mat>& imgs)
{
    cv::Mat resultMat;    
    bool Flag = true;
    cv::Stitcher stitcher = cv::Stitcher::createDefault(Flag);
    
	cv::Stitcher::Status status = stitcher.stitch(imgs, resultMat);    
	if (status != cv::Stitcher::OK)
    {
        cout << "stitchBatchImgs error=" << status << endl;
    }

    if (resultMat.empty())
    {
        cout << "stitchBatchImgs resultMat is empty!" << endl;
    }

    return resultMat;   
}


cv::Mat FishEyeImgProcess::surfStitch2Imgs(cv::Mat& srcImage1, cv::Mat& srcImage2)
{
    cv::Mat retPanorama;
    cv::Mat srcImage2Warped;
    cv::Ptr<cv::Feature2D> surf = cv::xfeatures2d::SURF::create();

    vector<cv::KeyPoint> kPointMat, kPointMat2;;
    cv::Mat desMat1, desMat2;

    surf->detectAndCompute(srcImage1, cv::Mat(), kPointMat, desMat1);
    surf->detectAndCompute(srcImage2, cv::Mat(), kPointMat2, desMat2);

    cv::FlannBasedMatcher matcher;
    std::vector<cv::DMatch> matches;
    matcher.match(desMat1, desMat2, matches);
    double max_dist = 0, min_dist = 100;

    for (int i = 0; i < desMat1.rows; i++)
    {
        double dist = matches[i].distance;
        if (dist < min_dist) min_dist = dist;
        if (dist > max_dist) max_dist = dist;
    }

    std::vector<cv::DMatch> good_matches;
    for (int i = 0; i < desMat1.rows; i++)
    {
        if (matches[i].distance < 3 * min_dist)
        {
            good_matches.push_back(matches[i]);
        }
    }

    std::vector<cv::Point2f> srcImage1_matchedKPs;
    std::vector<cv::Point2f> srcImage2_matchedKPs;
    for (size_t i = 0; i < good_matches.size(); i++)
    {
        srcImage1_matchedKPs.push_back(
            kPointMat[good_matches[i].queryIdx].pt);
        srcImage2_matchedKPs.push_back(
            kPointMat2[good_matches[i].trainIdx].pt);
    }

    cv::Mat H = cv::findHomography(cv::Mat(srcImage2_matchedKPs),
        cv::Mat(srcImage1_matchedKPs), CV_RANSAC);

    cv::warpPerspective(srcImage2, srcImage2Warped, H,
        cv::Size(srcImage2.cols * 2, srcImage2.rows), cv::INTER_CUBIC);
    
    retPanorama = srcImage2Warped.clone();

    cv::Mat roi(retPanorama, cv::Rect(0, 0, srcImage1.cols, srcImage1.rows));
    srcImage1.copyTo(roi);
    
    if (retPanorama.empty())
    {
        cout << "surfStitch2Imgs retPanorama is empty!" << endl;
    }

    return retPanorama;  
}


vector<cv::Mat> FishEyeImgProcess::getImgs(string& path)
{
    vector<cv::Mat> imgs;
    
    FishEyeUtils fishEyeUtils;
    vector<string> files;
    fishEyeUtils.getFiles(path, files, true);

    vector<string>::iterator it;
    for (it = files.begin(); it < files.end(); ++it)
    {
        string file = *it;
        cv::Mat image = cv::imread(file);
        if (image.empty())
        {
            cout << "getImgs Current file: " << file << " is empty!" << endl;
            continue;
        }

        imgs.push_back(image);            
    }
    
    return imgs; 
}


vector<cv::Mat> FishEyeImgProcess::splitAllImgs(vector<cv::Mat>& imgs, 
    bool isTranspose)
{
    vector<cv::Mat> ceil_img;
	FishEyeImgProcess fishEyeImgProcess;
   
    int idx = 0;
    vector<cv::Mat>::iterator it;
    for (it = imgs.begin(); it != imgs.end(); ++it)
    {
        cv::Mat img = *it;

        int ceil_height, ceil_width;  
        cv::Rect rect;
        cv::Mat roi_img, reRoi_img;
        
        ceil_height = img.rows;
        ceil_width = (img.cols * 2) / 3;

        cout << "splitAllImgs no#" << idx << " ceil_height=" << ceil_height <<
            " ceil_width=" << ceil_width << endl;
            
        rect = cv::Rect(0, 0, ceil_width, ceil_height);    
        img(rect).copyTo(roi_img);
        
        rect = cv::Rect((img.cols - ceil_width), (img.rows - ceil_height), 
            ceil_width, ceil_height);    
        img(rect).copyTo(reRoi_img);    

        cout << "splitAllImgs no#" << idx << 
            " img.cols - ceil_width=" << img.cols - ceil_width << 
            " img.rows - ceil_height=" << img.rows - ceil_height << endl;

        if (roi_img.empty() || reRoi_img.empty()) 
        {
            cout << "roi_img or reRoi_img is empty!" << endl;
            continue;
        }

        if (isTranspose)
        {
            //cv::Mat new_roi_img = imageProcess.transposeImg(roi_img);
            //cv::Mat new_reRoi_img = imageProcess.transposeImg(reRoi_img);
            
			ceil_img.push_back(fishEyeImgProcess.transposeImg(roi_img, 0));
            ceil_img.push_back(fishEyeImgProcess.transposeImg(reRoi_img, 0));
        }
        else
        {
            ceil_img.push_back(roi_img);
            ceil_img.push_back(reRoi_img);
        }

        idx++;
    }

    cout << "splitAllImgs ceil_img size=" << ceil_img.size() << endl;
    return ceil_img;
}
