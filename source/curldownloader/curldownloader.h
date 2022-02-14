#ifndef NXMP_CURLDOWNLOADER_H
#define NXMP_CURLDOWNLOADER_H

#include <curl/curl.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

struct MemoryStruct {
  char *memory;
  size_t size;
};


class curlDownloader{
public:
	curlDownloader();
	void Download(char * url ,MemoryStruct * chunk);
	std::string scrapeHtml (std::string myurl, std::string postcode, bool needpost);
	
};


class SOAPcurlDownloader{
public:
	SOAPcurlDownloader();
	void Download(char * url ,MemoryStruct * chunk, char * OID);
	
};




#endif
