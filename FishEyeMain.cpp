#include "include/FishEyeUtils.h"
#include "include/FishEyeImgProcess.h"
#include "include/FishEyeCalibration.h"
#include "include/FishEyeUnwrap.h"
#include "include/FishEyeStitch.h"


int main(int argc, char *argv[])
{
    string paramCfg = "cfg/parameters.cfg"; 
    string mOrgFilePath, mSplitFilePath, mCaliPath, mCaliOutPath, mOptUnwrapPath, mResultPath;

    int ret = 0;    
    FishEyeUtils fishEyeUtils;
    map<string, string>* paramsCfg = new map<string, string>();
    paramsCfg = fishEyeUtils.readCfgFile(paramCfg);
    
    map<string, string>::iterator it;
    for (it = paramsCfg->begin(); it != paramsCfg->end(); ++it)
    {
        string key = it->first;
        
        if (strcmp(key.c_str(), "mOrgFilePath") == 0)
        {
            mOrgFilePath = it->second;
        }

        if (strcmp(key.c_str(), "mSplitFilePath") == 0)
        {
            mSplitFilePath = it->second;  
            string mSplitFiles = mSplitFilePath + "\\*";
            fishEyeUtils.delDirFile(mSplitFiles);
            fishEyeUtils.makeDir(mSplitFilePath);
        }
        
        if (strcmp(key.c_str(), "mCaliPath") == 0)
        {
            mCaliPath = it->second;
        }

        if (strcmp(key.c_str(), "mCaliOutPath") == 0)
        {
            mCaliOutPath = it->second;            
            string mCaliOutFiles = mCaliOutPath + "\\*";
            fishEyeUtils.delDirFile(mCaliOutFiles);
            fishEyeUtils.makeDir(mCaliOutPath);
        }

        if (strcmp(key.c_str(), "mOptUnwrapPath") == 0)
        {
            mOptUnwrapPath = it->second;            
            string mOptUnwrapFiles = mOptUnwrapPath + "\\*";
            fishEyeUtils.delDirFile(mOptUnwrapFiles);
            fishEyeUtils.makeDir(mOptUnwrapPath);
        }

        if (strcmp(key.c_str(), "mResultPath") == 0)
        {
            mResultPath = it->second;       
            string mResultFiles = mResultPath + "\\*";
            fishEyeUtils.delDirFile(mResultFiles);
            fishEyeUtils.makeDir(mResultPath);
        }
    }


    // Split origin 
    FishEyeImgProcess fishEyeImgProcess;    
    map<string, cv::Mat>* mLeftSplitImgs = new map<string, cv::Mat>();
    map<string, cv::Mat>* mRightSplitImgs = new map<string, cv::Mat>();

    ret = fishEyeImgProcess.splitBatchLRImgs(mOrgFilePath, mSplitFilePath, 
        mLeftSplitImgs, mRightSplitImgs);

    cout << "main mLeftSplitImgs size=" << mLeftSplitImgs->size() <<
        " mRightSplitImgs size=" << mRightSplitImgs->size() << endl;

    FishEyeCalibration fishEyeCalibration; 
    FishEyeUnwrap fishEyeUnwrap;

    ofstream mOptUnwrapFile = ofstream(mOptUnwrapPath + "\\opt_unwarp_result.txt");  
    
    string mLeftCaliPath = mCaliPath + "\\left";
    string mLeftCaliOutPath = mCaliOutPath + "\\left";
    fishEyeUtils.makeDir(mLeftCaliOutPath);

    fishEyeCalibration.calibrateImg(mLeftCaliPath, mLeftCaliOutPath);
    
    map<string, cv::Mat>* mLeftUnwrapImgs = new map<string, cv::Mat>();    
    mLeftUnwrapImgs = fishEyeUnwrap.optUnwrapBatchLRImgs(mLeftSplitImgs, 
        mOptUnwrapPath, fishEyeCalibration.mIntrinsicMatrix, 
        fishEyeCalibration.mDistortionCoeffs, mOptUnwrapFile, false);


    string mRightCaliPath = mCaliPath + "\\right";
    string mRightCaliOutPath = mCaliOutPath + "\\right";
    fishEyeUtils.makeDir(mRightCaliOutPath);

    fishEyeCalibration.calibrateImg(mRightCaliPath, mRightCaliOutPath);
    
    map<string, cv::Mat>* mRightUnwrapImgs = new map<string, cv::Mat>();
    mRightUnwrapImgs = fishEyeUnwrap.optUnwrapBatchLRImgs(mRightSplitImgs, 
        mOptUnwrapPath, fishEyeCalibration.mIntrinsicMatrix, 
        fishEyeCalibration.mDistortionCoeffs, mOptUnwrapFile, true);

    //delete[] paramsCfg, mLeftSplitImgs, mRightSplitImgs, mRightUnwrapImgs;
    return 0;    
}
