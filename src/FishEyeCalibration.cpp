#include "include/FishEyeCalibration.h"
#include "include/FishEyeUtils.h"


FishEyeCalibration::FishEyeCalibration()
{
}


FishEyeCalibration::~FishEyeCalibration()
{
}


int FishEyeCalibration::calibrateImg(string& inPath, string& outPath)
{
    ofstream fout = ofstream(outPath + "\\caliberation_result.txt");  
    cv::Size board_size = cv::Size(9, 6);    

    vector<cv::Point2f> corners;
    vector<vector<cv::Point2f>> corners_Seq;    
    map<string, cv::Mat>* image_Seq = new map<string, cv::Mat>();
    int successImageNum = 0; 
    
    FishEyeUtils fishEyeUtils;    
    vector<string> files;
    fishEyeUtils.getFiles(inPath, files, false);

    cv::Size firstSize;
    bool setFirstSize = false;

    int count = 0;
    int idx = 0;
    vector<string>::iterator it;
    for (it = files.begin(); it < files.end(); ++it)
    {
        string imageFileName = *it;

        cv::Mat image = cv::imread(inPath + "\\" + imageFileName);
        if (image.empty())
        {
            cout << "Current file: " << imageFileName << " is empty!" << endl;
            continue;
        }

        cv::Mat imageGray;
        cv::cvtColor(image, imageGray, cv::COLOR_RGB2GRAY);

        bool patternfound = cv::findChessboardCorners(image, board_size, corners, 
             + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK);
        if (!patternfound)
        {
            cout << "Current file: " << imageFileName 
                << " can not find chessboard corners!" << endl;
            continue;
        }
        else
        {
            cv::cornerSubPix(imageGray, corners, cv::Size(11, 11), cv::Size(-1, -1), 
                cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 100, 1e-6));
            cv::Mat imageTemp = image.clone();
            
			for (int j = 0; j < corners.size(); j++)
            {
                cv::circle(imageTemp, corners[j], 10, cv::Scalar(0, 0, 255), 2, 8, 0);
            }

            if (imageTemp.empty()) 
            {
                cout << "imageTemp is empty!" << endl;
                continue;
            }

            vector<string> fileNames;
            string pattern = ".";
            fileNames = fishEyeUtils.split(imageFileName, pattern);
            string tmpFileName = fileNames[0];
            
            string cornerFileName = tmpFileName + "_corner.jpg";            
            cv::imwrite(outPath + "\\" + cornerFileName, imageTemp);

            cout << "Frame corner#" << idx << " imageFileName:" << imageFileName << 
                " cornerFileName:" << cornerFileName << " end" << endl;

            count = count + corners.size();
            successImageNum = successImageNum + 1;
            corners_Seq.push_back(corners);
            
        }
        
        image_Seq->insert(make_pair(imageFileName, image));

        if (!setFirstSize)
        {
            firstSize = image.size();
            setFirstSize = true;
        }

        idx++;
    }

    cv::Size square_size = cv::Size(20, 20);    
    vector< vector<cv::Point3f> >  object_Points;
    cv::Mat image_points = cv::Mat(1, count, CV_32FC2, cv::Scalar::all(0));
    vector<int>  point_counts;
    
    for (int t = 0; t < successImageNum; t++)
    {
        vector<cv::Point3f> tempPointSet;
        for (int i = 0; i < board_size.height; i++)
        {
            for (int j = 0; j < board_size.width; j++)
            {
                cv::Point3f tempPoint;
                tempPoint.x = i*square_size.width;
                tempPoint.y = j*square_size.height;
                tempPoint.z = 0;
                tempPointSet.push_back(tempPoint);
            }
        }
        
        object_Points.push_back(tempPointSet);
    }
    
    for (int i = 0; i < successImageNum; i++)
    {
        point_counts.push_back(board_size.width * board_size.height);
    }

    
    cv::Size image_size = firstSize;
    cv::Matx33d intrinsic_matrix;
    cv::Vec4d distortion_coeffs;
    
	std::vector<cv::Vec3d> rotation_vectors;
    std::vector<cv::Vec3d> translation_vectors; 
    
    int flags = 0;
    flags |= cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC;
    flags |= cv::fisheye::CALIB_CHECK_COND;
    flags |= cv::fisheye::CALIB_FIX_SKEW;
    
    cv::fisheye::calibrate(object_Points, corners_Seq, image_size, intrinsic_matrix, 
        distortion_coeffs, rotation_vectors, translation_vectors, flags, 
        cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 100, 1e-6));

    double total_err = 0.0;  
    double err = 0.0; 
    vector<cv::Point2f>  image_points2;

    for (int i = 0; i < successImageNum; i++)
    {
        vector<cv::Point3f> tempPointSet = object_Points[i];        
        cv::fisheye::projectPoints(tempPointSet, image_points2, rotation_vectors[i], 
            translation_vectors[i], intrinsic_matrix, distortion_coeffs);
        vector<cv::Point2f> tempImagePoint = corners_Seq[i];
        
        cv::Mat tempImagePointMat = cv::Mat(1, tempImagePoint.size(), CV_32FC2);
        cv::Mat image_points2Mat = cv::Mat(1, image_points2.size(), CV_32FC2);
        
        for (size_t i = 0; i != tempImagePoint.size(); i++)
        {
            image_points2Mat.at<cv::Vec2f>(0, i) = 
                cv::Vec2f(image_points2[i].x, image_points2[i].y);
            
            tempImagePointMat.at<cv::Vec2f>(0, i) = 
                cv::Vec2f(tempImagePoint[i].x, tempImagePoint[i].y);
        }
        
        err = cv::norm(image_points2Mat, tempImagePointMat, cv::NORM_L2);
        total_err += err /= point_counts[i];
        
        fout << "第" << i << "幅图像的平均误差：" << err << "像素" << endl;
    }

    fout << "总体平均误差：" << total_err / successImageNum << "像素" << endl << endl;
    cout << "Total err=" << total_err / successImageNum << "pix" << endl << endl;

    cv::Mat rotation_matrix = cv::Mat(3, 3, CV_32FC1, cv::Scalar::all(0)); 

    fout << "相机内参数矩阵：" << endl;
    fout << intrinsic_matrix << endl;
    
    fout << "畸变系数：" << endl;
    fout << distortion_coeffs << endl;

    for ( int i=0; i < intrinsic_matrix.rows; i++ )
    {
        for ( int j=0; j < intrinsic_matrix.cols; j++ )
        {
            cout << "intrinsic_matrix(" << i << ", " << j << ")=" <<
                intrinsic_matrix(i, j) << endl;
        }
    }
    
    for ( int i=0; i < distortion_coeffs.channels; i++)
    {
        cout << "distortion_coeffs(" << i << ")=" <<
            distortion_coeffs(i) << endl;        
    }
    
        
    for (int i = 0; i < successImageNum; i++)
    {
        fout << "第" << i << "幅图像的旋转向量：" << endl;
        fout << rotation_vectors[i] << endl;
        cv::Rodrigues(rotation_vectors[i], rotation_matrix);
        
        fout << "第" << i << "幅图像的旋转矩阵：" << endl;
        fout << rotation_matrix << endl;
        
        fout << "第" << i << "幅图像的平移向量：" << endl;
        fout << translation_vectors[i] << endl;
    }
    fout << endl;    


    cv::Mat mapx = cv::Mat(image_size, CV_32FC1);
    cv::Mat mapy = cv::Mat(image_size, CV_32FC1);
    cv::Mat R = cv::Mat::eye(3, 3, CV_32F);

    int jdx = 0;
    map<string, cv::Mat>::iterator itr;
    for (itr = image_Seq->begin(); itr != image_Seq->end(); ++itr)
    {
        cv::fisheye::initUndistortRectifyMap(intrinsic_matrix, distortion_coeffs, 
            R, intrinsic_matrix, image_size, CV_32FC1, mapx, mapy);

        string imgFileName = itr->first;        
        
        cv::Mat img_seq = itr->second;
        cv::Mat t = img_seq.clone();
        cv::remap(img_seq, t, mapx, mapy, cv::INTER_LINEAR);

        if (t.empty()) 
        {
            cout << "wrapImage is empty!" << endl;
            continue;
        }
        
        vector<string> fileNames;
        string pattern = ".";
        fileNames = fishEyeUtils.split(imgFileName, pattern);
        string tmpFileName = fileNames[0];
        
        string wrapImageFileName = tmpFileName + "_unwrap.jpg";         
        cv::imwrite(outPath + "\\" + wrapImageFileName, t);
        
        cout << "Frame no#" << jdx << " WrapImageFileName:" << 
            outPath + "\\" + wrapImageFileName << endl;

        jdx++;
    }

    mIntrinsicMatrix = intrinsic_matrix;
    mDistortionCoeffs = distortion_coeffs; 
    mImageSize = image_size;
    mMapx = mapx;
    mMapy = mapy;
    mR = R;

    //delete[] image_Seq;    
    return 0;
}
