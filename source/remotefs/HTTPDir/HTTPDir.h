#ifndef NXMP_HTTPDIR_H
#define NXMP_HTTPDIR_H

#include <platforms.h>

#ifdef NXMP_NETWORKSUPPORT

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
	
	void DirList(std::string path,const std::vector<std::string> &extensions);
	void backDir();
	std::vector<FS::FileEntry> getCurrList();
	std::string getUrl();
	std::string getBasePath();
	std::string getCurrPath();
	
	bool *checked(int pos);
	void clearChecked();
	
	
private:
	std::vector<FS::FileEntry> currentlist;
	std::string url;
	std::string basepath = "";
	std::string currentpath = "";
};

#endif
#endif