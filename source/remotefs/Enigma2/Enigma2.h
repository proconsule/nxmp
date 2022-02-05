#ifndef NXMP_ENIGMA2_H
#define NXMP_ENIGMA2_H

#include "platforms.h"

#ifdef NXMP_ENIGMASUPPORT

#include <cstdlib>
#include <cstring>
#include "curldownloader.h"
#include <iostream>
#include <vector>
#include <regex>
#include <tinyxml2.h>

using namespace std;
using namespace tinyxml2;


struct epgStruct{
	string title;
	int startTime;
	int duration;
	int currTime;
};

struct ChannelStruct{
	string name;
	string url;
	string srvref;
	epgStruct epg;
};


struct EnigmaServices{
	std::string name = "";
	std::string bouquetref = "";
	
};

class Enigma2{
public:

	Enigma2(std::string url);
	string enigma2ip;
	vector<EnigmaServices> parseBouquet(char * data);
	void m3uParser(char * url);
	//void curlDownload(char * url ,MemoryStruct * chunk);
	
	void backToTop();
	EnigmaServices getCurrBouquet();
	void setCurrBouquet(EnigmaServices _bouquet);
	
	bool getServices();
	
	vector<ChannelStruct> e2currbouqet;
	vector<EnigmaServices> e2services;
	
	private:
	
	std::string enigmaurl;
	EnigmaServices currbouquet;
	
	
};

#endif
#endif
