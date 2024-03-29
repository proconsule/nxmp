#ifndef NXMP_LOCALFS_H
#define NXMP_LOCALFS_H

#include "localfiles.h"
#include "playlist.h"


class localFs{
public:
	localFs(std::string _path,Playlist * _playlist);
	~localFs();
	std::string getCurrentPath();
	std::string backPath();
	std::vector<FS::FileEntry> getCurrList();
	std::vector<FS::FileEntry> getCurrImageList();
	std::vector<FS::FileEntry> getCurrListUnicode();
	void DirList(const std::string &path,bool showHidden,const std::vector<std::string> &extensions);
	void clearChecked();
	bool *checked(int pos);
	
	void SetFileDbStatus(int idx,int dbstatus);
	void ResetDbStatus();
	
	int sortOrder = 0;
	
	bool getfileContents(std::string filepath,unsigned char ** _filedata,int &_size);
	
	bool OpenFile(std::string filepath);
	int FileRead(unsigned char * data,unsigned int count,unsigned int offset);
	void FileClose();
	

private:
	Playlist * playlist;
	std::string basepath = "/";
	std::string currentpath = "";
	std::vector<FS::FileEntry> currentlist;
	std::vector<FS::FileEntry> currentimagelist;
	
	FILE * open_file;
	bool file_opened = false;
	

};


#endif