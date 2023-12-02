#ifndef NXMP_FILEBROWSER_H
#define NXMP_FILEBROWSER_H

#include "localFs.h"
#include "sambaDir.h"
#include "sshDir.h"
#include "FTPDir.h"
#include "HTTPDir.h"
#include "nfsDir.h"
#include "usbfs.h"

#include "compressedfs.h"

#include "utils.h"

class CFileBrowser{
public:
	CFileBrowser(std::string _path,Playlist * _playlist,USBMounter * _myusb = nullptr);
	~CFileBrowser();
	
	localFs *mylocal= nullptr;
	sambaDir *mysamba = nullptr;
	sshDir *myssh = nullptr;
	nfsDir *mynfs = nullptr;
	FTPDir *myftp = nullptr;
	HTTPDir *myhttp = nullptr;
	USBMounter *myusb = nullptr;
	compressedFS * compressedArchive = nullptr;
	
	void OpenArchive(std::string _path);
	void CloseArchive();
	
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
	std::string title;
	
	unsigned char *archive_data = NULL;
	int archive_datasize = 0;
	
	std::string oldtitle = "";
	
};




#endif