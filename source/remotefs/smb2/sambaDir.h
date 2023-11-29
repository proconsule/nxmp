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

#define SMB_MAX_PATH_LENGTH 1024

class sambaDir{
public:
	sambaDir(std::string _url,Playlist * _playlist);
	
	bool DirList(std::string path,bool showHidden,const std::vector<std::string> &extensions);
	std::vector<FS::FileEntry> getCurrList();
	
	std::string backDir();
	std::string getUrl();
	std::string getCurrPath();
	std::string getBasePath();
	std::string getShare();
	
	bool *checked(int pos);
	void clearChecked();
	
	int sortOrder=0;
	
	void SetFileDbStatus(int idx,int dbstatus);
	void ResetDbStatus();
	
	bool getfileContents(std::string filepath,unsigned char ** _filedata,int &_size);
	
	std::string errormsg = "";
	
private:
	std::vector<FS::FileEntry> currentlist;
	std::string url;
	std::string share = "";
	std::string basepath = "";
	std::string currentpath = "";
	Playlist *playlist;
};

#endif