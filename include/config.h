#ifndef NXMP_CONFIG_H
#define NXMP_CONFIG_H

#include <string>
#include <vector>


#include "SimpleIni.h"


typedef struct{
	bool showhidden;
	bool touchenable;
	int playeswipeseek;
	int shortseek;
	int longseek;
	bool usealang;
	int alang;
	bool useslang;
	int slang;
    bool useoc;
	bool hwdec = true;
	int subfontsize;
	float subfontscale;
	int deint;
	float subfontcolor[4];
	float subbordercolor[4];
	int startresumeperc;
	int stopresumeperc;
	bool dbactive;
	std::string themename;
}nxmpconfig_struct;


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
	
	bool getUseOc(bool tmpvalue);
	void setUseOc(bool _val);
	
	bool getHWDec(bool tmpvalue);
	void setHWDec(bool _val);

	int getSubFontSize(bool tmpvalue);
	void setSubFontSize(int val);

	//subscale
	float getSubFontScale(bool tmpvalue);
	void setSubFontScale(float val);
	//endsubscale

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
	
	void addNetworkShare(networkSource netshare);
	void RefreshNetworkShare(std::vector<networkSource> newnetsources);
	
	
private:
	
	nxmpconfig_struct nxmpconfig;
	nxmpconfig_struct nxmptmpconfig;
	
	std::string inifilePath;
};


#endif