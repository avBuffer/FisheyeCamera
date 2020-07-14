#include "include/FishEyeUtils.h"


FishEyeUtils::FishEyeUtils()
{
}


FishEyeUtils::~FishEyeUtils()
{
}


bool FishEyeUtils::makeDir(string& dir)
{
    if (access(dir.c_str(), 0) == -1)  
    {  
        cout << "dir=" << dir << " not exist and make it!" << endl;  
       
        int flag=mkdir(dir.c_str()); 
        if (flag != 0)  
        {  
            cout << "make dir=" << dir << " error!" << endl; 
            return false;
        } 
    }  

    return true;  
}


bool FishEyeUtils::delDir(string& dir)
{
    if (access(dir.c_str(), 0) == 0)  
    {  
        cout << "dir=" << dir << " exist and delete it!" << endl;  

        int flag = rmdir(dir.c_str());
        if (flag != 0)  
        {  
            cout << "delete dir=" << dir << " error!" << endl; 
            return false;
        } 
    }  

    return true;  
}


bool FishEyeUtils::isSpecDir(char *path)
{
    return strcmp(path, "..") == 0 || strcmp(path, ".") == 0;
}


bool FishEyeUtils::isDir(int attrib)
{
    return attrib == 16 || attrib == 18 || attrib == 20;
}


void FishEyeUtils::getFilePath(const char *path, const char *file_name, 
    char *file_path)
{
    strcpy_s(file_path, sizeof(char) * _MAX_PATH, path);
    file_path[strlen(file_path) - 1] = '\0';
    strcat_s(file_path, sizeof(char) * _MAX_PATH, file_name);
    strcat_s(file_path, sizeof(char) * _MAX_PATH, "\\*");
}


void FishEyeUtils::delDirFile(string& path)
{
    _finddata_t dir_info;
    _finddata_t file_info;
    intptr_t f_handle;
    char tmp_path[_MAX_PATH];
    if((f_handle = _findfirst(path.c_str(), &dir_info)) != -1)
    {
        while(_findnext(f_handle, &file_info) == 0)
        {
            if(isSpecDir(file_info.name))
                continue;
            
			if(isDir(file_info.attrib))
            {   
                getFilePath(path.c_str(), file_info.name, tmp_path);
                delDirFile(string(tmp_path));
                tmp_path[strlen(tmp_path) - 2] = '\0';
                
                if(file_info.attrib == 20)
                {
                    cout << "delDirFile file_info name=" << file_info.name << 
                        " is system file, can't delete!" << endl;
                }
                else
                {
                    if(_rmdir(tmp_path) == -1)
                    {
                        cout << "delDirFile tmp_path=" << tmp_path << " isn't empty!" << endl;
                    }
                }
            }
            else
            {
                strcpy_s(tmp_path, path.c_str());
                tmp_path[strlen(tmp_path) - 1] = '\0';
                strcat_s(tmp_path, file_info.name);
               
                if(remove(tmp_path) == -1)
                {
                    cout << "delDirFile tmp_path=" << tmp_path << 
                        " file_info.name=" << file_info.name << " remove error!" << endl;
                }
            }
        }
        _findclose(f_handle);
    }

    delDir(path);
}


void FishEyeUtils::getFiles(string& path, vector<string>& files, bool hasDir)
{
    long hFile = 0;      
    struct _finddata_t fileinfo; 
    
    string p;  
    if((hFile = _findfirst(p.assign(path).append("\\*").c_str(), 
        &fileinfo)) != -1)  
    {  
        do  
        {  
            if((fileinfo.attrib &  _A_SUBDIR))  
            {  
                if(strcmp(fileinfo.name,".") != 0 && 
                    strcmp(fileinfo.name,"..") != 0)  
                {
                    getFiles( p.assign(path).append("\\").append(fileinfo.name), 
                        files, hasDir); 
                }
            }  
            else  
            {  
                if (hasDir)
                {
                    files.push_back(p.assign(path).append("\\").append(fileinfo.name));
                }
                else
                {
                    files.push_back(fileinfo.name);
                }
            }  
        }
        while(_findnext(hFile, &fileinfo) == 0); 
        
        _findclose(hFile);  
    }
}


vector<string> FishEyeUtils::split(string& str, string& delim)
{
    vector<string> elems; 
    
    int pos = 0;  
    int len = str.length();  
    int delim_len = delim.length();  
    
    if (delim_len == 0)
    { 
        return elems; 
    }
    
    while (pos < len)  
    {  
        int find_pos = str.find(delim, pos);
        
        if (find_pos < 0)  
        {  
            elems.push_back(str.substr(pos, len - pos));  
            break;  
        }  
        elems.push_back(str.substr(pos, find_pos - pos));  
        pos = find_pos + delim_len;  
    }  
    
    return elems; 
}


string FishEyeUtils::int2str(int idx)
{
     stringstream ss;
     ss<<idx; 
     
     string str;
     ss>>str;

     return str;
}


int FishEyeUtils::str2int(string& str)
{   
     stringstream ss;
     ss<<str;
    
     int idx;
     ss>>idx;

     return idx;
}


map<string, string>* FishEyeUtils::readCfgFile(string& cfgFile)
{
    map<string, string> *cfgParams = new map<string, string>();   
        
    fstream fCfgFile; 
    fCfgFile.open(cfgFile.c_str());
    
    if(!fCfgFile.is_open())
    {
        cout<<"readCfgFile File:" << cfgFile << " can not open!" << endl;
        return cfgParams;
    }
    
    char tmp[1024];    
    while(!fCfgFile.eof())
    {
        fCfgFile.getline(tmp, 1024);
        string line(tmp);
        
        size_t pos = line.find('=');        
        if(pos == string::npos)
        {
            continue;
        } 
        
		std::string key  = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        cfgParams->insert(make_pair(key, value));
    }

    return cfgParams;
}
