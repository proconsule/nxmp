#ifndef NXMP_SAMBADIR_H
#define NXMP_SAMBADIR_H

#include <stdint.h>
#include <sys/_types.h>
#include <smb2/smb2.h>
#include <smb2/libsmb2.h>
#include <smb2/libsmb2-raw.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <cstring>
#include "localfiles.h"
#include "playlist.h"

class sambaDir{
public:
	sambaDir(std::string _url,Playlist * _playlist);
	
	void DirList(std::string path,bool showHidden,const std::vector<std::string> &extensions);
	std::vector<FS::FileEntry> getCurrList();
	
	void backDir();
	std::string getUrl();
	std::string getCurrPath();
	std::string getBasePath();
	std::string getShare();
	
	bool *checked(int pos);
	void clearChecked();
	
	int sortOrder=0;
	
private:
	std::vector<FS::FileEntry> currentlist;
	std::string url;
	std::string share = "";
	std::string basepath = "";
	std::string currentpath = "";
	Playlist *playlist;
};

#endif