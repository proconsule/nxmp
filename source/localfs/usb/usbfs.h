#ifndef NXMP_USBFS_H
#define NXMP_USBFS_H

#include "platforms.h"

#ifdef NXMP_USBSUPPORT

#include <cstdlib>
#include <string>
#include <vector>
#include <threads.h>
#include <usbhsfs.h>
#include "localfiles.h"
#include "playlist.h"



struct usb_devices{
	std::string mount_point;
	std::string name;
	std::string fstype;
	size_t capacity;
};

int usbThread(void *arg);

class USBMounter{
public:
	USBMounter(Playlist *_playlist);
	~USBMounter();
	std::vector<usb_devices> mounted_devs;
	
	void setBasePath(std::string _basepath);
	std::string getBasePath();
	std::string getCurrentPath();
	void backPath();
	std::vector<FS::FileEntry> getCurrList();
	void DirList(const std::string &path,bool showHidden,const std::vector<std::string> &extensions);
	void clearChecked();
	bool *checked(int pos);
	
	bool haveIteminPlaylist();
	
private:
	Playlist * playlist;
	std::string basepath = "";
	std::string currentpath = "";
	std::vector<FS::FileEntry> currentlist;
	
};


void usbInit();

#endif
#endif
