#ifndef NXMP_UTILITY_H
#define NXMP_UTILITY_H

#include <string>
#include <iostream>    
#include <sstream>    
#include <iomanip>
#include <regex>
#include <remotefs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include <switch.h>

#include "curldownloader.h"


#include "imgui.h"


#include "logger.h"



extern std::string tempKbUrl;


struct urlschema{
	std::string scheme;
	std::string server;
	std::string user;
	std::string pass;
	std::string port;
	std::string path;
};



namespace Utility{
	
	struct lang_code_struct {
		std::string lang2;
		std::string lang3;
	};
	
	extern std::vector<std::string> supportedExtensions;
	
	std::string formatTimeStamp(time_t timestamp);
	std::string formatTimeShort(double seconds);
	urlschema parseUrl(std::string url); 
	bool compare(const remotefs_entry &a, const remotefs_entry &b);
#ifdef OPENGGL_BACKEND
	bool TxtLoadFromFile(std::string filename, GLuint* out_texture, int* out_width, int* out_height);
	bool TxtLoadFromMemory(unsigned char* data,size_t image_size, GLuint* out_texture, int* out_width, int* out_height);
#endif
	std::string toLower(const std::string &str);
	std::string toUpper(const std::string &str);
	
	bool endsWith(const std::string &value, const std::string &ending, bool sensitive);
	bool startWith(const std::string &str, const std::string &value, bool sensitive);
	
	std::vector<std::string> getMediaExtensions();
	void setMediaExtensions(std::vector<std::string> myext);
	std::string humanSize(size_t bytes);
	std::string str_tolower(std::string s);
	std::vector<lang_code_struct> getLanguages();
	int getLanguagesIdx(std::string lang);

	void replace(std::string& subject, const std::string& search,const std::string& replace);
	bool GetChargeStatue(void);
	uint32_t GetBatteryPercentage(void);
	std::string scrapElement(std::string content, std::string get,std::string delim);
	std::string KeyboardCall (std::string hint="", std::string text="");
	std::string Nozomi_Link(std::string Link);
	std::string ltrim(const std::string &s);
	std::string rtrim(const std::string &s);
	std::string trim(const std::string &s);
	std::string truncateLen(std::string path,int len);
	std::string pathfromarray(std::vector<std::string> vec,int startpos);
	
	
	bool isImageExtension(std::string path);
	bool isArchiveExtension(std::string path);
	bool isM3UExtension(std::string path);
	

}

#endif
