#include "NX-UPNP.h"
#ifdef NXMP_UPNPSUPPORT

#ifdef __SWITCH__

#include <sys/socket.h>
#include <arpa/inet.h>

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#endif

#include "tinyxml2.h"

#ifdef _WIN32
#ifndef __MSXML_LIBRARY_DEFINED__
#define __MSXML_LIBRARY_DEFINED__
#endif
#endif

using namespace tinyxml2;

#ifdef _WIN32
#include <iptypes.h>
#include <iphlpapi.h>
#endif


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
	printf("%s\n",chunk->memory);
	fflush(stdout);
	tinyxml2::XMLDocument doc;
	doc.Parse( chunk->memory );
	XMLElement * pRootElement = doc.RootElement();
	if (NULL != pRootElement) {
		XMLElement * devicenode = pRootElement->FirstChildElement("device");
		XMLElement * UDNNameTag = devicenode->FirstChildElement("UDN");
		XMLElement * friendlyNameTag = devicenode->FirstChildElement("friendlyName");
		XMLElement * iconListTag = devicenode->FirstChildElement("iconList");
		XMLElement * iconnode = iconListTag->FirstChildElement("icon");
		while(iconnode){
			XMLElement * mymetag = iconnode->FirstChildElement("mimetype");
			XMLElement * urltag = iconnode->FirstChildElement("url");
			std::string mimetype = mymetag->GetText();
			std::string tmpiconurl = urltag->GetText();
			if(mimetype == "image/png"){
				iconUrl = thisurl.scheme + std::string("://") + thisurl.server + ":" +thisurl.port + tmpiconurl;
				printf("Icon Url: %s\n",iconUrl.c_str());
				
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
				printf("Control URL: %s\n",controlUrl.c_str());
				fflush(stdout);
			}
			servicenode = servicenode->NextSiblingElement("service");
		}
		
		
		friendlyName = friendlyNameTag->GetText();
		devUDN = UDNNameTag->GetText();
		printf("UDN: %s\n",friendlyName.c_str());
		printf("friendlyName: %s\n",devUDN.c_str());
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
	printf("CHUNK MEM\n");
	printf("RET: %s\n",chunk->memory);
	fflush(stdout);
	char * testret = NULL;
	decode_html_entities_utf8(chunk->memory,0);
	printf("RET2: %s\n",chunk->memory);
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
#ifdef __SWITCH__
		tmpentry.size = atoi(itemres->Attribute("size"));
#endif
#ifdef __WIN32
		tmpentry.size = _atoi64(itemres->Attribute("size"));
#endif
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
	}

std::vector<Device *> NXUPnP::getDevicesList(){
	return deviceslist;
}

void NXUPnP::InitDiscoverThread(){
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, 0x10000000);
	printf("InitDiscoverThread pthread_create\n");
	pthread_create(&DiscoverThreadID, &attr, DiscoverThreadFunc, this);
      
}

NXUPnP::NXUPnP(){
	
#ifdef _WIN32
	WSADATA wsadata;
	int err;
 
	err = WSAStartup(MAKEWORD(2, 0), &wsadata);
	if(err != 0) {
		printf("WSAStartup failed with error: %d\n", err);

    }else{
		printf("WSAStartup ok\n");
	}
#endif

    struct sockaddr_in upnpControl, broadcast_addr;
    discoverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (discoverSocket == INVALID_SOCKET)
    {
        printf("socked failed INVALID_SOCKET\n");
        return;
    }

	int reuse = 1;
	setsockopt(discoverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));

#ifdef _WIN32
	char ttl = 4;
#else
	int ret = -1;
    socklen_t ttl = 4;
#endif
char ipv4address[INET_ADDRSTRLEN];
#ifdef _WIN32
					/* CODE TAKEN FROM LIBUPNP*/
        /* ---------------------------------------------------- */
        /* WIN32 implementation will use the IpHlpAPI library.  */
        /* ---------------------------------------------------- */
        PIP_ADAPTER_ADDRESSES adapts = NULL;
        PIP_ADAPTER_ADDRESSES adapts_item;
        PIP_ADAPTER_UNICAST_ADDRESS uni_addr;
        SOCKADDR *ip_addr;
        struct in_addr v4_addr;
        struct in6_addr v6_addr;
        ULONG adapts_sz = 0;
        ULONG ret;
        int ifname_found = 0;
        int valid_addr_found = 0;
        char inet_addr4[INET_ADDRSTRLEN];
        char inet_addr6[INET6_ADDRSTRLEN];
		
		char ipv4_ifname[180];
		int ipv4_ifindex;

        /* Get Adapters addresses required size. */
        ret = GetAdaptersAddresses(AF_UNSPEC,
                GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_DNS_SERVER,
                NULL,
                adapts,
                &adapts_sz);
        if (ret != ERROR_BUFFER_OVERFLOW) {
                
        }
        /* Allocate enough memory. */
        adapts = (PIP_ADAPTER_ADDRESSES)malloc(adapts_sz);
        if (adapts == NULL) {
                
        }
        /* Do the call that will actually return the info. */
        ret = GetAdaptersAddresses(AF_UNSPEC,
                GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_DNS_SERVER,
                NULL,
                adapts,
                &adapts_sz);
        if (ret != ERROR_SUCCESS) {
                free(adapts);
                
        }
        /* Copy interface name, if it was provided. */
        
        for (adapts_item = adapts; adapts_item != NULL;
                adapts_item = adapts_item->Next) {
                if (adapts_item->Flags & IP_ADAPTER_NO_MULTICAST ||
                        adapts_item->OperStatus != IfOperStatusUp) {
                        continue;
                }
                /*
                 * Partial fix for Windows: Friendly name is wchar
                 * string, but currently p->gIF_NAME is char string. For
                 * now try to convert it, which will work with many (but
                 * not all) adapters. A full fix would require a lot of
                 * big changes (p->gIF_NAME to wchar string?).
                 */
                if (!ifname_found) {
                        /* We have found a valid interface name. Keep
                         * it. */
                        char tmpIfName[180];
                        wcstombs(tmpIfName,
                                adapts_item->FriendlyName,
                                180);
                        strcpy(ipv4_ifname, tmpIfName);
                        ifname_found = 1;
                } else {
                        char tmpIfName[180] = {0};
                        wcstombs(tmpIfName,
                                adapts_item->FriendlyName,
                                sizeof(tmpIfName));
                        if (strncmp(ipv4_ifname,
                                    tmpIfName,
                                    180) != 0) {
                                /* This is not the interface we're
                                 * looking for.
                                 */
                                continue;
                        }
                }
                /* Loop thru this adapter's unicast IP addresses. */
                uni_addr = adapts_item->FirstUnicastAddress;
                while (uni_addr) {
                        ip_addr = uni_addr->Address.lpSockaddr;
                        switch (ip_addr->sa_family) {
                        case AF_INET:
                                memcpy(&v4_addr,
                                        &((struct sockaddr_in *)ip_addr)
                                                 ->sin_addr,
                                        sizeof(v4_addr));
                                /* TODO: Retrieve IPv4 netmask */
                                valid_addr_found = 1;
                                break;
                        case AF_INET6:
                                /* TODO: Retrieve IPv6 ULA or GUA
                                 * address and its prefix */
                                /* Only keep IPv6 link-local addresses.
                                 */
                                if (IN6_IS_ADDR_LINKLOCAL(
                                            &((struct sockaddr_in6 *)ip_addr)
                                                     ->sin6_addr)) {
                                        memcpy(&v6_addr,
                                                &((struct sockaddr_in6 *)
                                                                ip_addr)
                                                         ->sin6_addr,
                                                sizeof(v6_addr));
                                        /* TODO: Retrieve IPv6 LLA
                                         * prefix */
                                        valid_addr_found = 1;
                                }
                                break;
                        default:
                                if (valid_addr_found == 0) {
                                        /* Address is not IPv4 or IPv6
                                         * and no valid address has  */
                                        /* yet been found for this
                                         * interface.
                                         * Discard interface name. */
                                        ifname_found = 0;
                                }
                                break;
                        }
                        /* Next address. */
                        uni_addr = uni_addr->Next;
                }
                if (valid_addr_found == 1) {
                        ipv4_ifindex = adapts_item->IfIndex;
                        break;
                }
        }
        free(adapts);
        /* Failed to find a valid interface, or valid address. */
        if (ifname_found == 0 || valid_addr_found == 0) {
                return;
        }
        inet_ntop(AF_INET, &v4_addr, inet_addr4, sizeof inet_addr4);
        strcpy(ipv4address,inet_addr4);
        

#endif

	memset((char *) &upnpControl, 0, sizeof(upnpControl));
	upnpControl.sin_family = AF_INET;
    upnpControl.sin_port = htons(0);
    upnpControl.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(discoverSocket, (sockaddr*)&upnpControl, sizeof(upnpControl)) == SOCKET_ERROR)
    {
        printf("bind failed\n");
		fflush(stdout);
        close(discoverSocket);
        return;
    }

#ifdef __SWITCH__
	
	nifmInitialize(NifmServiceType_User);
	uint32_t currentIp;
	nifmGetCurrentIpAddress(&currentIp);
	inet_ntop(AF_INET, &currentIp, ipv4address, INET_ADDRSTRLEN);
#endif

#ifdef _WIN32


	struct in_addr addr;
	memset((void *)&addr, 0, sizeof(struct in_addr));
	addr.s_addr = inet_addr(ipv4address);
	ret = setsockopt(discoverSocket,
                IPPROTO_IP,
                IP_MULTICAST_IF,
                (char *)&addr,
                sizeof addr);
    if (ret == -1) {
		printf("Failed to set Multicast IF\n");
	}

#endif

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
		printf("Error Sending SSDP\n");
	}else{
		printf("Sent SSDP\n");
	}
	
    while(searchthreadexit == 0){
        
		
        while (recvfrom(discoverSocket, buf, MAX_DGRAM_SIZE, 0,  (struct sockaddr*)&broadcast_addr, (socklen_t*)&bcLen) >0)
		{
		    
			printf("buf:%s\n",buf);
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
				printf("location:%s\n",location.c_str());
				Device *tmpdev = new Device(location);
				addDevice(tmpdev);
			}
            memset(buf, 0, MAX_DGRAM_SIZE);

        }
		
	}
	printf("Exit SSDP Read Thread\n");
	fflush(stdout);
}

#endif