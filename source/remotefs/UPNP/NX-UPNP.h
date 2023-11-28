#ifndef NXMP_UPNP_H
#define NXMP_UPNP_H





#include <stdio.h>
#include <cstring>
#include <vector>
#include <string>
#include <stdarg.h>
#include <pthread.h>
#include <unistd.h>



#include "curldownloader.h"
#include "localfiles.h"

#include <sstream>
#include <tinyxml2.h>

#ifdef __SWITCH__
#include <switch.h>

#include "nxmp-gfx.h"

using namespace tinyxml2;


#define _GNU_SOURCE


typedef enum UPNPTYPE {
    UPNPItem,
    UPNPContainer
} UPNPTYPE;


struct upnpres_struct{
	std::string name;
	std::string uri;
	int64_t size;
	UPNPTYPE type;
#ifdef OPENGL_BACKEND
	Tex albumart;
#endif


	int duration;
	std::string albumarturi;
};

class Device{
public:
	Device(std::string _location);
	~Device();
	std::string getIP();
	std::string getfriendlyName();
	std::string getUDN();
#ifdef OPENGL_BACKEND
	Tex devIcon;
#endif
#ifdef DEKO3D_BACKEND
	Texture devIcon;
#endif
	std::string iconUrl;
	void browseOID();
	void back();
	std::vector<upnpres_struct> currentlist;
	std::vector<std::string> parentList;
	
private:
	std::string devUDN;
	std::string friendlyName;
	std::string currentID = "0";
	std::string parentID = "0";
	
	char * baseUrl = NULL;
	std::string serversddress;
	std::string controlUrl;
	
	

};

class NXUPnP{
public:
	NXUPnP();
	~NXUPnP();
	void InitDiscoverThread();
	void ListenSSDPResponse();
	void Discovery();
	void addDevice(Device * _dev);
	Device * getDevice(int idx);
	std::vector<Device *> getDevicesList();
	//UpnpClient_Handle  CtrlPnt;
	int getSelDevice();
	void setSelDevice(int idx);
	
private:
	pthread_t DiscoverThreadID;
	int discoverSocket = -1;
	//UpnpLib * libpointer = NULL;
	std::vector<Device *> deviceslist;
	int seldevice = -1;
	
	int searchthreadexit = 0;
	
};

#endif
#endif