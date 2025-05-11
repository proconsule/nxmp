#include "NX-UPNP.h"

#ifdef __SWITCH__

#include <sys/socket.h>
#include <arpa/inet.h>

#include <chrono>
#include <sstream>

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#endif

#include "tinyxml2.h"


using namespace tinyxml2;



#define MAX_DGRAM_SIZE 1024

const char *searchMediaServer = "M-SEARCH * HTTP/1.1\r\n"    \
                                           "HOST: 239.255.255.250:1900\r\n"     \
                                           "MAN: \"ssdp:discover\"\r\n"     \
                                           "MX: 5\r\n"          \
                                           "ST: urn:schemas-upnp-org:device:MediaServer:1\r\n" \
										   "USER-AGENT: NXMP\r\n\r\n";
										
const char *searchContentServer = "M-SEARCH * HTTP/1.1\r\n"    \
                                           "HOST: 239.255.255.250:1900\r\n"     \
                                           "MAN: \"ssdp:discover\"\r\n"     \
                                           "MX: 5\r\n"          \
                                           "ST: urn:schemas-upnp-org:service:ContentDirectory:1\r\n" \
										   "USER-AGENT: NXMP\r\n\r\n";

extern "C" size_t decode_html_entities_utf8(char *dest, const char *src);

void *DiscoverThreadFunc(void* arg)
{
    NXUPnP* nxupnp = (NXUPnP *)arg;
    nxupnp->ListenSSDPResponse();
	return (void *)0;
}

std::string Device::getIP(){
	return serversddress;
}

Device::Device(std::string _location){
	urlschema thisurl = Utility::parseUrl(_location);
	serversddress = thisurl.server;
	MemoryStruct *chunk = (MemoryStruct *)malloc(sizeof(MemoryStruct));
	curlDownloader * curldown = new curlDownloader();
	curldown->Download(_location.c_str(),chunk);
	NXLOG::DEBUGLOG("%s\n",chunk->memory);
	fflush(stdout);
	tinyxml2::XMLDocument doc;
	doc.Parse( chunk->memory );
	XMLElement * pRootElement = doc.RootElement();
	if (NULL != pRootElement) {
		XMLElement * devicenode = pRootElement->FirstChildElement("device");
		XMLElement * UDNNameTag = devicenode->FirstChildElement("UDN");
		XMLElement * friendlyNameTag = devicenode->FirstChildElement("friendlyName");
		XMLElement * manufacturerTag = devicenode->FirstChildElement("manufacturer");
		XMLElement * modelDescriptionTag = devicenode->FirstChildElement("modelDescription");
		XMLElement * modelNameTag = devicenode->FirstChildElement("modelName");
		
		XMLElement * iconListTag = devicenode->FirstChildElement("iconList");
		XMLElement * iconnode = iconListTag->FirstChildElement("icon");
		while(iconnode){
			XMLElement * mymetag = iconnode->FirstChildElement("mimetype");
			XMLElement * urltag = iconnode->FirstChildElement("url");
			std::string mimetype = mymetag->GetText();
			std::string tmpiconurl = urltag->GetText();
			if(mimetype == "image/png"){
				iconUrl = thisurl.scheme + std::string("://") + thisurl.server + ":" +thisurl.port + tmpiconurl;
				
				break;
			}
			
			
			iconnode = iconnode->NextSiblingElement("icon");
		}
		XMLElement * serviceListTag = devicenode->FirstChildElement("serviceList");
		XMLElement * servicenode = serviceListTag->FirstChildElement("service");
		while(servicenode){
			XMLElement * serviceTypetag = servicenode->FirstChildElement("serviceType");
			std::string serviceTypestring = serviceTypetag->GetText();
			if(serviceTypestring == "urn:schemas-upnp-org:service:ContentDirectory:1"){
				XMLElement * controlURLtag = servicenode->FirstChildElement("controlURL");
				controlUrl = thisurl.scheme + std::string("://") + thisurl.server + ":" +thisurl.port + std::string(controlURLtag->GetText());
				
				fflush(stdout);
			}
			servicenode = servicenode->NextSiblingElement("service");
		}
		
		
		friendlyName = friendlyNameTag->GetText();
		manufacturer = manufacturerTag->GetText();
		modelDescription = modelDescriptionTag->GetText();
		modelName = modelNameTag->GetText();
		
		devUDN = UDNNameTag->GetText();
		
		fflush(stdout);
	}
	
	free(chunk->memory);
	free(chunk);
	
	delete curldown;
	
}

std::string Device::getUDN(){
	return devUDN;
}


int NXUPnP::getSelDevice(){
	return seldevice;
	
}

Device * NXUPnP::getDevice(int idx){
	return deviceslist[idx];
	
}

void NXUPnP::Discovery(){
	
	
}

NXUPnP::~NXUPnP(){
	for(int i=0;i<deviceslist.size();i++){
		if(deviceslist[i]->devIcon.id != -1 && imgloader->Renderer != nullptr){
			imgloader->Renderer->unregister_texture(deviceslist[i]->devIcon);
		}
	}
	if(searchthreadexit == 0){
		searchthreadexit = 1;
		close(discoverSocket);
	}
	pthread_join(DiscoverThreadID,NULL);
	
}

void NXUPnP::addDevice(Device * _dev){
	bool found = false;
	for(int i=0;i<deviceslist.size();i++){
		if(_dev->getUDN() == deviceslist[i]->getUDN()){
			found = true;
		}
	}
	if(!found){
		deviceslist.push_back(_dev);
	}
}


void Device::browseOID(){
	currentlist.clear();
	SOAPcurlDownloader *testcurl = new SOAPcurlDownloader();
	MemoryStruct *chunk = (MemoryStruct *)malloc(sizeof(MemoryStruct));
	testcurl->Download(controlUrl.c_str(),chunk,parentList.back().c_str());
	fflush(stdout);
	char * testret = NULL;
	decode_html_entities_utf8(chunk->memory,0);
	fflush(stdout);
	tinyxml2::XMLDocument doc;
	doc.Parse( chunk->memory ,strlen(chunk->memory));
	
	XMLElement * BrowseResponseEle =  doc.FirstChildElement( "s:Envelope" )->FirstChildElement( "s:Body" )->FirstChildElement( "u:BrowseResponse" );
	
	
	XMLElement * DIDEle =  BrowseResponseEle->FirstChildElement( "Result" )->FirstChildElement( "DIDL-Lite" );
	
	XMLElement * containernode = DIDEle->FirstChildElement("container");
	while(containernode){
		XMLElement * conttitleele = containernode->FirstChildElement("dc:title");
		
		upnpres_struct tmpentry;
		tmpentry.name = conttitleele->GetText();
		tmpentry.uri = containernode->Attribute("id");
		tmpentry.type = UPNPTYPE::UPNPContainer;
		currentlist.push_back(tmpentry);
		containernode = containernode->NextSiblingElement("container");
		
	}
	XMLElement * itemnode = DIDEle->FirstChildElement("item");
	while(itemnode){
		XMLElement * itemele = itemnode->FirstChildElement("dc:title");
		upnpres_struct tmpentry;
		tmpentry.name = itemele->GetText();
		XMLElement * itemres = itemnode->FirstChildElement("res");
		tmpentry.uri = itemres->GetText();
		tmpentry.size = atoi(itemres->Attribute("size"));

		
		
		tmpentry.type = UPNPTYPE::UPNPItem;
		currentlist.push_back(tmpentry);
		
		
		itemnode = itemnode->NextSiblingElement("item");
	}
	
	fflush(stdout);
	free(chunk->memory);
	free(chunk);
	
	delete testcurl;
	
}

std::string Device::getfriendlyName(){
	return friendlyName;
}

std::string Device::getmanufacturer(){
	return manufacturer;
}
std::string Device::getmodelDescription(){
	return modelDescription;
}
std::string Device::getmodelName(){
	return modelName;
}

void NXUPnP::setSelDevice(int idx){
	if(searchthreadexit == 0){
		searchthreadexit = 1;
		close(discoverSocket);
	}
	seldevice = idx;
		
}


void Device::back(){
		if(parentList.size()>1){
			parentList.pop_back();
		}
		if(friendlyparentList.size()>1){
			friendlyparentList.pop_back();
		}
	}

std::vector<Device *> NXUPnP::getDevicesList(){
	return deviceslist;
}

void NXUPnP::InitDiscoverThread(){
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, 0x10000000);
	NXLOG::DEBUGLOG("InitDiscoverThread pthread_create\n");
	pthread_create(&DiscoverThreadID, &attr, DiscoverThreadFunc, this);
      
}

NXUPnP::NXUPnP(){
	
#ifdef _WIN32
	WSADATA wsadata;
	int err;
 
	err = WSAStartup(MAKEWORD(2, 0), &wsadata);
	if(err != 0) {
		NXLOG::DEBUGLOG("WSAStartup failed with error: %d\n", err);

    }else{
		NXLOG::DEBUGLOG("WSAStartup ok\n");
	}
#endif

    struct sockaddr_in upnpControl, broadcast_addr;
    discoverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (discoverSocket == INVALID_SOCKET)
    {
        NXLOG::DEBUGLOG("socked failed INVALID_SOCKET\n");
        return;
    }

	int reuse = 1;
	setsockopt(discoverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));


	int ret = -1;
    socklen_t ttl = 4;

char ipv4address[INET_ADDRSTRLEN];


	struct ip_mreq ssdpMcastAddr;

	memset((char *) &upnpControl, 0, sizeof(upnpControl));
	upnpControl.sin_family = AF_INET;
    upnpControl.sin_port = htons(0);
    upnpControl.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(discoverSocket, (sockaddr*)&upnpControl, sizeof(upnpControl)) == SOCKET_ERROR)
    {
        NXLOG::DEBUGLOG("bind failed\n");
		fflush(stdout);
        close(discoverSocket);
        return;
    }


	
	nifmInitialize(NifmServiceType_Admin);
	uint32_t currentIp;
	nifmGetCurrentIpAddress(&currentIp);
	inet_ntop(AF_INET, &currentIp, ipv4address, INET_ADDRSTRLEN);




	ssdpMcastAddr.imr_interface.s_addr =  currentIp;

	
	ssdpMcastAddr.imr_multiaddr.s_addr = inet_addr("239.255.255.250");
	
	memset((void *)&ssdpMcastAddr, 0, sizeof ssdpMcastAddr);
	ret = setsockopt(discoverSocket,
		IPPROTO_IP,
		IP_ADD_MEMBERSHIP,
		(char *)&ssdpMcastAddr,
		sizeof(struct ip_mreq));
		
	if (ret == -1) {
		NXLOG::DEBUGLOG("Failed to set Multicast Group\n");
	}

    


	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	setsockopt(discoverSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);


	int onOff = 1;
	ret = setsockopt(discoverSocket,
		SOL_SOCKET,
		SO_BROADCAST,
		(char *)&onOff,
		sizeof(onOff));

	InitDiscoverThread();

}

void NXUPnP::ListenSSDPResponse()
{
	char buf[MAX_DGRAM_SIZE];
	memset(buf, 0, MAX_DGRAM_SIZE);

	struct sockaddr_in broadcast_addr;
	memset((char *) &broadcast_addr, 0, sizeof(broadcast_addr));
	broadcast_addr.sin_family = AF_INET;
	broadcast_addr.sin_port = htons(1900);
	broadcast_addr.sin_addr.s_addr = inet_addr("239.255.255.250");
	int bcLen = sizeof(broadcast_addr);
	
	if(sendto(discoverSocket, searchMediaServer, strlen(searchMediaServer), 0, (sockaddr *)&broadcast_addr, sizeof(broadcast_addr)) == SOCKET_ERROR)
	{
		NXLOG::DEBUGLOG("Error Sending SSDP\n");
	}else{
		NXLOG::DEBUGLOG("Sent SSDP\n");
	}
	
    while(searchthreadexit == 0){
        
		
        while (recvfrom(discoverSocket, buf, MAX_DGRAM_SIZE, 0,  (struct sockaddr*)&broadcast_addr, (socklen_t*)&bcLen) >0)
		{
		    
			//NXLOG::DEBUGLOG("buf:%s\n",buf);
			if(strstr(buf, "LOCATION:")){
				std::ostringstream ss;
				int i = 0;
				char *deviceIp = strstr(buf, "LOCATION:") + 9; 
				while(deviceIp[i] == ' ') i++; 
				while(!isspace(deviceIp[i]))
				{
					ss << deviceIp[i];
					++i;
				}
				std::string location = ss.str().c_str();
				
				Device *tmpdev = new Device(location);
				
				MemoryStruct *chunk = (MemoryStruct *)malloc(sizeof(MemoryStruct));
				curlDownloader * curldownload = new curlDownloader();
				curldownload->Download((char *)tmpdev->iconUrl.c_str(),chunk);
				tmpdev->devIcon = imgloader->OpenImageMemory(chunk->memory,chunk->size);
				
				free(chunk->memory);
				free(chunk);
				delete curldownload;
				
				addDevice(tmpdev);
			}
            memset(buf, 0, MAX_DGRAM_SIZE);

        }
		
	}
	NXLOG::DEBUGLOG("Exit SSDP Read Thread\n");
	fflush(stdout);
}
