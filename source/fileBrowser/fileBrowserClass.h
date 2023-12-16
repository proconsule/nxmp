#ifndef NXMP_FILEBROWSER_H
#define NXMP_FILEBROWSER_H

#include "HTTPDir.h"
#include "usbfs.h"


#include "smb2fs.h"
#include "sshfs.h"
#include "nfsfs.h"
#include "libarchivefs.h"
#include "ftpfs.h"


#include "utils.h"

class CFileBrowser{
public:
	CFileBrowser(std::string _path,Playlist * _playlist,USBMounter * _myusb = nullptr);
	~CFileBrowser();
	
	HTTPDir *myhttp = nullptr;
	USBMounter *myusb = nullptr;
	
	
	
	void OpenArchive(std::string _path);
	std::string CloseArchive();
	
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
	
	
protected:
	CSSHFS * sshfs = nullptr;
	CSMB2FS * smb2fs = nullptr;
	CNFSFS * nfsfs = nullptr;
	CARCHFS * archfs = nullptr;
	CFTPFS *ftpfs = nullptr;

private:
	
	

	std::string title;
	
	unsigned char *archive_data = NULL;
	int archive_datasize = 0;
	
	std::string oldtitle = "";
	std::string oldmount = "";
	
	
	std::string basepath = "/";
	std::string currentpath = "";
	std::vector<FS::FileEntry> currentlist;
	std::vector<FS::FileEntry> currentimagelist;

	uint32_t maxreadsize = 1024*1024;
	
};




#endif