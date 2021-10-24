#ifndef NXMP_HTTPDIR_H
#define NXMP_HTTPDIR_H

#include <cstdio>
#include <string>
#include <vector>
#include <stdio.h>
#include <cstring>
#include <regex>
#include <curl/curl.h>
#include "remotefs.h"

struct HTTPMemoryStruct {
  char *memory;
  size_t size;
};

class HTTPDir{
public:
	HTTPDir();
	void curlDownload(char * url ,HTTPMemoryStruct * chunk);
	void setCurrentRelPath(std::string _path);
	std::string getCurrentRelPath();
	void setUrl(std::string _path);
	std::string getUrl();
	std::vector<remotefs_entry> dirList(std::string path);
	void backDir();
private:
	
	std::string currentrelpath = "";
	std::string url;
	
};

#endif