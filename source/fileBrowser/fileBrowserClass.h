#ifndef NXMP_FILEBROWSER_H
#define NXMP_FILEBROWSER_H

#include "localFs.h"
#include "sambaDir.h"
#include "sshDir.h"
#include "ftpDir.h"
#include "httpDir.h"
#include "nfsDir.h"
#include "usbfs.h"

#include "utils.h"

class CFileBrowser{
public:
	CFileBrowser(std::string _path,Playlist * _playlist,bool isUSB = false);
	~CFileBrowser();
	
	localFs *mylocal= nullptr;
	sambaDir *mysamba = nullptr;
	sshDir *myssh = nullptr;
	nfsDir *mynfs = nullptr;
	FTPDir *myftp = nullptr;
	HTTPDir *myhttp = nullptr;
	USBMounter *myusb = nullptr;
	
	void DirList(std::string path,bool showHidden,const std::vector<std::string> &extensions);
	std::vector<FS::FileEntry> getCurrList();
	std::vector<FS::FileEntry> getChecked();
	
	void backDir();
	std::string getUrl();
	std::string getCurrentPath();
	std::string getBasePath();
	std::string getShare();
	std::string getOpenUrlPart();
	
	void setBasePath(std::string _basepath);
	
	void clearChecked();
	bool *checked(int pos);
	
	void setSordOrder(FS::FILESORTORDER myorder);
	
	std::string getTitle(); 
	std::string path;
	
	FS::FILESORTORDER sortOrder = FS::FS_NAME_ASCENDINGORDER;
	bool currshowHidden = false;
	
	std::vector<usb_devices> getUsbDev();

private:
	std::string title;
	
};




#endif