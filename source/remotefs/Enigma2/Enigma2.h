#ifndef NXMP_ENIGMA2_H
#define NXMP_ENIGMA2_H

#include <cstdlib>
#include <cstring>
#include <curl/curl.h>
#include <iostream>
#include <vector>
#include <regex>
#include <tinyxml2.h>

using namespace std;
using namespace tinyxml2;

struct MemoryStruct {
  char *memory;
  size_t size;
};

struct ChannelStruct{
	string name;
	string url;
	string srvref;
	string epgtitle;
};


struct EnigmaServices{
	std::string name;
	std::string bouquetref;
	
};

class Enigma2{
public:
	string enigma2ip;
	vector<EnigmaServices> parseBouquet(char * data);
	void m3uParser(char * url);
	void curlDownload(char * url ,MemoryStruct * chunk);
	
	bool getServices();
	
	vector<ChannelStruct> e2currbouqet;
	vector<EnigmaServices> e2services;
	
	
};

#endif
