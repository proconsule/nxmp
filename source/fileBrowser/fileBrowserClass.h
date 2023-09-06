#ifndef NXMP_FILEBROWSER_H
#define NXMP_FILEBROWSER_H

#include "localFs.h"
#include "sambaDir.h"
#include "sshDir.h"
#include "ftpDir.h"
#include "httpDir.h"
#include "nfsDir.h"

#include "utils.h"

class CFileBrowser{
public:
	CFileBrowser(std::string _path,Playlist * _playlist);
	~CFileBrowser();
	
	localFs *mylocal= nullptr;
	sambaDir *mysamba = nullptr;
	sshDir *myssh = nullptr;
	nfsDir *mynfs = nullptr;
	FTPDir *myftp = nullptr;
	HTTPDir *myhttp = nullptr;
	
	void DirList(std::string path,bool showHidden,const std::vector<std::string> &extensions);
	std::vector<FS::FileEntry> getCurrList();
	
	void backDir();
	std::string getUrl();
	std::string getCurrentPath();
	std::string getBasePath();
	std::string getShare();
	
	void clearChecked();
	bool *checked(int pos);
	
	std::string path;
	
};




#endif