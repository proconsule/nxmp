#ifndef NXMP_CONFIG_H
#define NXMP_CONFIG_H

#include <string>
#include <vector>

#include "platforms.h" 

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
	
	bool getshowHidden(bool tmpvalue);
	void setshowHidden(bool val);
	
	bool getTouchEnable(bool tmpvalue);
	void setTouchEnable(bool val);
	
	int getPlayerSwipeSeek(bool tmpvalue);
	void setPlayerSwipeSeek(int val);
	
	int getLongSeek(bool tmpvalue);
	int getShortSeek(bool tmpvalue);
	
	void setLongSeek(int seektime);
	void setShortSeek(int seektime);
	
	bool getUseAlang(bool tmpvalue);
	void setUseAlang(bool _val);
	int getAlang(bool tmpvalue);
	void setAlang(int lang);

	//Slang
	bool getUseSlang(bool tmpvalue);
	void setUseSlang(bool _val);
	int getSlang(bool tmpvalue);
	void setSlang(int lang);
    //End Slang
	
	int getSubFontSize(bool tmpvalue);
	void setSubFontSize(int val);
	
	float * getSubFontColor(bool tmpvalue);
	std::string getSubFontColorHex(bool tmpvalue);
	void setSubFontColor(float *_color);

	//bordercolor
	float * getSubBorderColor(bool tmpvalue);
	std::string getSubBorderColorHex(bool tmpvalue);
	void setSubBorderColor(float *_color);
	//endbordercolor
	
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
	
	std::string getThemeName(bool tmpvalue);
	void setThemeName(std::string value);
	
	
private:

	bool showhidden;
	bool tmpshowhidden;

	bool touchenable;
	bool tmptouchenable;
	
	int playeswipeseek;
	int tmpplayeswipeseek;

	int shortseek;
	int longseek;
	
	int tmpshortseek;
	int tmplongseek;
	
	bool tmpusealang;
	bool usealang;
	
	int tmpalang;
	int alang;
	
	//Slang
	bool tmpuseslang;
	bool useslang;
	
	int tmpslang;
	int slang;
    //End Slang
	
	int tmpsubfontsize;
	int subfontsize;
	
	int tmpdeint;
	int deint;
	
	float subfontcolor[4];
	float tmpsubfontcolor[4];
	
	//bordercolor
	float subbordercolor[4];
	float tmpsubbordercolor[4];
	//end bordercolor
	
	int tmpstartresumeperc;
	int tmpstopresumeperc;
	int startresumeperc;
	int stopresumeperc;
	
	
	bool tmpdbactive;
	bool dbactive;
	
	std::string themename;
	std::string tmpthemename;
	
	std::string inifilePath;
};


#endif