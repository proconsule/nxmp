#ifndef NXMP_COMPRESSEDFS_H
#define NXMP_COMPRESSEDFS_H

#include <string>

#include "usbfs.h"
#include "playlist.h"


#include "zipDir.h"


class compressedFS{
public:
	compressedFS(unsigned char * _archive_data,int _archive_size);
	void ReadList();

	void DirList(std::string path,bool showHidden,const std::vector<std::string> &extensions);
	std::vector<FS::FileEntry> getCurrList();
	std::vector<FS::FileEntry> getCurrImageList();
	std::vector<FS::FileEntry> getChecked();
	
	std::string backDir();
	std::string getUrl();
	std::string getCurrentPath();
	std::string getBasePath();
	std::string getShare();
	std::string getOpenUrlPart();
	
	void setBasePath(std::string _basepath);
	
	void clearChecked();
	bool *checked(int pos);
	
	bool getfileContents(std::string filepath,unsigned char ** _filedata,int &_size);
	
	void setSordOrder(FS::FILESORTORDER myorder);
	void SetFileDbStatus(int idx,int dbstatus);
	void ResetDbStatus();
	
	
	std::string getTitle(); 
	std::string path;
	
	std::string currentFileinUse;
	
	FS::FILESORTORDER sortOrder = FS::FS_NAME_ASCENDINGORDER;
	bool currshowHidden = false;
	
	bool timelessFS = false;
	bool connected = true;
	std::string errormsg = "";
	std::vector<usb_devices> getUsbDev(bool dummy = false);
	
	int getNextImg();
	int getPrevImg();



private:
	zipDir *zipFs;
	
	unsigned char * archive_data;
	int archive_size;

};


#endif
