#ifndef _UTILS_H_
#define _UTILS_H_
#pragma once

#include <opencv2/opencv.hpp>  
#include <stdio.h>  
#include <io.h>  
#include <string>  
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <direct.h>
#include <stdlib.h>
#include <memory>

using namespace std;


class FishEyeUtils
{
    public:
        FishEyeUtils();
        ~FishEyeUtils();

    public:
        bool makeDir(string& dir);
        bool delDir(string& dir);
        bool isSpecDir(char *path);
        bool isDir(int attrib);
        void getFilePath(const char *path, const char *file_name, char *file_path);
        void delDirFile(string& path);
            
        void getFiles(string& path, vector<string>& files, bool hasDir);
        vector<string> split(string& s, string& delim);

        string int2str(int idx);
        int str2int(string& str);
        
        bool readCfgFile(string& cfgfilepath, string& key, string& value);
        map<string, string>* readCfgFile(string& cfgfilepath);
};
#endif
