#ifndef NXMP_LOCALFS_H
#define NXMP_LOCALFS_H

#include "localfiles.h"


class localFs{
public:
	localFs(std::string _path);
	~localFs();
	std::string getCurrentPath();
	void backPath();
	std::vector<FS::FileEntry> getCurrList();
	void DirList(const std::string &path,bool showHidden,const std::vector<std::string> &extensions);

private:
	std::string basepath = "/";
	std::string currentpath = "";
	std::vector<FS::FileEntry> currentlist;

};


#endif