#ifndef NXMP_USBFS_H
#define NXMP_USBFS_H

#include <cstdlib>
#include <string>
#include <vector>
#include <threads.h>
#include <usbhsfs.h>
#include "logger.h"
#include "localfiles.h"
#include "playlist.h"


//#define USB_USE_EVENT_SYSTEM
#define USB_USE_CALLBACK_SYSTEM



struct usb_devices{
	std::string mount_point;
	std::string name;
	std::string fstype;
	size_t capacity;
};


#ifdef USB_USE_EVENT_SYSTEM
int usbThread(void *arg);
#endif

class USBMounter{
public:
	USBMounter();
	~USBMounter();
	std::vector<usb_devices> mounted_devs;
	
	void setBasePath(std::string _basepath);
	std::string getBasePath();
	std::string getCurrentPath();
	std::string backDir();
	std::vector<FS::FileEntry> getCurrList();
	std::vector<FS::FileEntry> getCurrImageList();
	void DirList(const std::string &path,bool showHidden,const std::vector<std::string> &extensions);
	void clearChecked();
	bool *checked(int pos);
	
	bool haveIteminPlaylist();
	
	void SetFileDbStatus(int idx,int dbstatus);
	void ResetDbStatus();
	
	int sortOrder=0;
	
	bool getfileContents(std::string filepath,unsigned char ** _filedata,int &_size);

	//UsbHsFsDevice *g_usbDevices;
	//u32 g_usbDeviceCount;

#ifdef USB_USE_CALLBACK_SYSTEM	
	
	void usbMscTestDevices(void);
#endif

private:
	
	std::string basepath = "";
	std::string currentpath = "";
	std::vector<FS::FileEntry> currentlist;
	std::vector<FS::FileEntry> currentimagelist;
	
};

#endif
