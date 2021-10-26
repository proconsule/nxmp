#ifndef NXMP_FTPDIR_H
#define NXMP_FTPDIR_H

#include <string>
#include <vector>

#include "localfiles.h"
#include "utils.h"
#include "ftplib.h"

class FTPDir{
public:
	FTPDir(std::string _url);
	~FTPDir();
	std::vector<FS::FileEntry> getDir(std::string path,const std::vector<std::string> &extensions);
	
	std::string backDir();
	std::string getUrl();
	std::string getCurrPath();
	std::string getBasePath();
	
	
private:
	std::string url;
	std::string currpath;
	std::string basepath;
	netbuf *ftp_con = nullptr;

};

#endif