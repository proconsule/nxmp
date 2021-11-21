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
	
	int getLongSeek(bool tmpvalue);
	int getShortSeek(bool tmpvalue);
	
	void setLongSeek(int seektime);
	void setShortSeek(int seektime);
	
	bool getUseAlang(bool tmpvalue);
	void setUseAlang(bool _val);
	int getAlang(bool tmpvalue);
	void setAlang(int lang);
	
	int getSubFontSize(bool tmpvalue);
	void setSubFontSize(int val);
	
	int getDeinterlace(bool tmpvalue);
	void setDeinterlace(int value);
	
	bool getDbActive(bool tmpvalue);
	void setDbActive(bool value);
	
	int getResumeStartPerc(bool tmpvalue);
	void setResumeStartPerc(int value);
	int getResumeStopPerc(bool tmpvalue);
	void setResumeStopPerc(int value);
	
	void saveSettings();
	
	std::vector<std::string> topmenu;
	CSimpleIniA *ini;
	
private:
	int shortseek;
	int longseek;
	
	int tmpshortseek;
	int tmplongseek;
	
	bool tmpusealang;
	bool usealang;
	
	int tmpalang;
	int alang;
	
	int tmpsubfontsize;
	int subfontsize;
	
	int tmpdeint;
	int deint;
	
	
	
	
	int tmpstartresumeperc;
	int tmpstopresumeperc;
	int startresumeperc;
	int stopresumeperc;
	
	
	bool tmpdbactive;
	bool dbactive;
	
	std::string inifilePath;
};


#endif