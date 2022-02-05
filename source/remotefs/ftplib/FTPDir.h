#ifndef NXMP_FTPDIR_H
#define NXMP_FTPDIR_H

#include "platforms.h"

#ifdef NXMP_NETWORKSUPPORT

#include <string>
#include <vector>

#include "localfiles.h"
#include "utils.h"
#include "ftplib.h"
#include "playlist.h"

class FTPDir{
public:
	FTPDir(std::string _url,Playlist *_playlist);
	~FTPDir();
	void DirList(std::string path,const std::vector<std::string> &extensions);
	std::vector<FS::FileEntry> getCurrList();
	
	void backDir();
	std::string getUrl();
	std::string getCurrPath();
	std::string getBasePath();
	
	bool *checked(int pos);
	void clearChecked();
	
private:
	std::string url;
	std::string currpath;
	std::string basepath;
	std::vector<FS::FileEntry> currentlist;
	netbuf *ftp_con = nullptr;
	Playlist *playlist;

};

#endif
#endif