#ifndef NXMP_LOCALFS_H
#define NXMP_LOCALFS_H

#include "localfiles.h"
#include "playlist.h"


class localFs{
public:
	localFs(std::string _path,Playlist * _playlist);
	~localFs();
	std::string getCurrentPath();
	void backPath();
	std::vector<FS::FileEntry> getCurrList();
	std::vector<FS::FileEntry> getCurrListUnicode();
	void DirList(const std::string &path,bool showHidden,const std::vector<std::string> &extensions);
	void clearChecked();
	bool *checked(int pos);
	
	int sortOrder = 0;

private:
	Playlist * playlist;
	std::string basepath = "/";
	std::string currentpath = "";
	std::vector<FS::FileEntry> currentlist;

};


#endif