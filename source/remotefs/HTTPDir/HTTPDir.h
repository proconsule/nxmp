#ifndef NXMP_HTTPDIR_H
#define NXMP_HTTPDIR_H

#include <cstdio>
#include <string>
#include <vector>
#include <stdio.h>
#include <cstring>
#include <regex>
#include <curl/curl.h>
#include "localfiles.h"



struct HTTPMemoryStruct {
  char *memory;
  size_t size;
};

class HTTPDir{
public:
	HTTPDir(std::string _url);
	~HTTPDir();
	void curlDownload(char * url ,HTTPMemoryStruct * chunk);
	
	std::vector<FS::FileEntry> getDir(std::string path,const std::vector<std::string> &extensions);
	std::string backDir();
	std::string getUrl();
	std::string getBasePath();
	std::string getCurrPath();
	
	
private:
	std::string url;
	std::string basepath = "";
	std::string currentpath = "";
};

#endif