#ifdef __SWITCH__
#ifndef NXMP_USBFS_H
#define NXMP_USBFS_H

#include <cstdlib>
#include <string>
#include <vector>
#include <threads.h>
#include <usbhsfs.h>



struct usb_devices{
	std::string mount_point;
	std::string name;
	std::string fstype;
	size_t capacity;
};

int usbThread(void *arg);

class USBMounter{
public:
	USBMounter();
	~USBMounter();
	std::vector<usb_devices> mounted_devs;
	
	
	
private:
	
	
};


void usbInit();

#endif
#endif
