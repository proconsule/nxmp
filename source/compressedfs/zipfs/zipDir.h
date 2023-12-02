#ifndef NXMP_ZIPDIR_H
#define NXMP_ZIPDIR_H


#include <string>
#include <vector>
#include <stdio.h>
#include <cstring>
#include "localfiles.h"
#include "playlist.h"

#include <archive.h>
#include <archive_entry.h>


typedef struct{
	std::string parent;
	FS::FileEntry fileentry;
	int level;
	
	
}archfileentry_struct;


class zipDir{
public:
	zipDir(unsigned char * _archive_data,int _archive_size);
	void ReadList();
	
	void DirList(std::string path,bool showHidden,const std::vector<std::string> &extensions);
	std::vector<FS::FileEntry> getCurrList();
	std::vector<FS::FileEntry> getCurrImageList();
	
	std::string getCurrentPath();
	std::string backDir();

	bool *checked(int pos);
	void clearChecked();
	
	int sortOrder=0;
	
	//void SetFileDbStatus(int idx,int dbstatus);
	//void ResetDbStatus();
	
	bool getfileContents(std::string filepath,unsigned char ** _filedata,int &_size);
	
	std::string errormsg = "";
	
	

	
private:

	unsigned char * archive_data;
	int archive_size;
	
	std::vector<archfileentry_struct> totalfilelist;

	std::vector<FS::FileEntry> currentlist;
	std::vector<FS::FileEntry> currentimagelist;
	std::string path;
	std::string share = "";
	std::string basepath = "";
	std::string currentpath = "";
	Playlist *playlist;
};












#endif