#ifndef NXMP_CONFIG_H
#define NXMP_CONFIG_H

#include <string>
#include <vector>
#include "SimpleIni.h"

struct networkSource{
	std::string name;
	std::string url;
};

class Config{
public:
	Config(std::string inifile);
	std::vector<networkSource> getNetworks();
	std::string getEnigma();
	std::string getStartPath();
	
	CSimpleIniA *ini;
};


#endif