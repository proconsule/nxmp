#ifndef NXMP_SSHDIR_H
#define NXMP_SSHDIR_H

#include <libssh2.h>
#include <libssh2_sftp.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <cstring>
#include "localfiles.h"
#include "playlist.h"

class sshDir{
public:
	sshDir(std::string _url,Playlist * _playlist);
	
	void DirList(std::string path,bool showHidden,const std::vector<std::string> &extensions);
	std::vector<FS::FileEntry> getCurrList();
	
	void backDir();
	std::string getUrl();
	std::string getCurrPath();
	std::string getBasePath();
	
	bool *checked(int pos);
	void clearChecked();
	
private:
	std::vector<FS::FileEntry> currentlist;
	std::string url;
	std::string basepath = "";
	std::string currentpath = "";
	Playlist *playlist;
};


#endif