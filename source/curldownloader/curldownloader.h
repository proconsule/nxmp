#ifndef NXMP_CURLDOWNLOADER_H
#define NXMP_CURLDOWNLOADER_H

#include <curl/curl.h>
#include <cstdlib>
#include <cstring>


struct MemoryStruct {
  char *memory;
  size_t size;
};


class curlDownloader{
public:
	curlDownloader();
	void Download(char * url ,MemoryStruct * chunk);
	
};


class SOAPcurlDownloader{
public:
	SOAPcurlDownloader();
	void Download(char * url ,MemoryStruct * chunk, char * OID);
	
};




#endif
