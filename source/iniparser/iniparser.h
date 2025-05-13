#ifndef NXMP_INIPARSER_H
#define NXMP_INIPARSER_H

#include <string>
#include <vector>

#define MINI_CASE_SENSITIVE
#include "ini.h"


typedef struct{
	std::string startpath = "/switch/nxmp/";
	std::string dbpath = "/switch/nxmp/nxmp.db";
	int exitmode = 0;
	bool showhidden = false;
	int sortorder = 0;
	bool touchenable = false;
	int themecolor = 2;
	int playeswipeseek ;
	int shortseek = 10;
	int longseek = 60;
	bool usealang = false;
	int alang = 0;
	bool useslang = false;
	int slang = 0;
	//int intlang = 0;
	std::string intlangstr = "0";
	bool onlylatinrange = false;
    bool useoc = false;
	bool hwdec = true;
	bool vsync = true;
	int subfontsize = 55;
	float subfontscale = 1.0f;
	int deint = 0;
	float subfontcolor[4] = {1.0f,1.0f,1.0f,1.0f};
	float subbordercolor[4] = {0.0f,0.0f,0.0f,0.0f};
	int startresumeperc = 5;
	int stopresumeperc = 5;
	int demuxcachesec = 20;
	bool dbactive = false;
	std::string themename = "Default";
	bool emuoverrides = false;
	int loglevel = 0;
	bool consolewindow = false;
	int aout = 0;
	std::string e2addr = "";
	std::vector<std::string> config_enabled_extensions;
}playerconf_struct;


struct oldnetworkSource{
	std::string name;
	std::string url;
};

struct networkstruct_v2{
	std::string name = "";
	std::string server = "";
	std::string type = "";
	std::string username = "";
	std::string password = "";
	std::string path = "";
	std::string pubkeypath = "";
	std::string privkeypath = "";
	std::string passphrase = "";
	int port = 0;
};


class CIniParser{
public:
	CIniParser(std::string inifile);
	~CIniParser();
	std::vector<oldnetworkSource> getNetworks();
	std::string getEnigma();
	std::string getStartPath();
	
	
	std::vector<networkstruct_v2> networks_v2;
	
	
	void PrintConfig();
	void ResetTemp();
	
	bool getshowHidden(bool tmpvalue);
	void setshowHidden(bool val);
	
	int getsortOrder(bool tmpvalue);
	void setsortOrder(int val);
	
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
	
	bool getVSYNC(bool tmpvalue);
	void setVSYNC(bool _val);

	int getSubFontSize(bool tmpvalue);
	void setSubFontSize(int val);
	
	//int getInterfaceLang(bool tmpvalue);
	//void setInterfaceLang(int lang);
	std::string getInterfaceLang(bool tmpvalue);
	void setInterfaceLang(std::string langpath);
	int getLangId(std::string _langstr);
	int getConfigLangId(bool tmpvalue);
	
	
	bool getOnlyLatinRange(bool tmpvalue);
	void setOnlyLatinRange(bool latinrange);
	

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
	
	int getDemuxCache(bool tmpvalue);
	void setDemuxCache(int mycachesec);
	
	bool getDbActive(bool tmpvalue);
	void setDbActive(bool value);
	
	int getExitMode(bool tmpvalue);
	void setExitMode(int value);
	
	int getThemeColor(bool tmpvalue);
	void setThemeColor(int value);
	
	int getResumeStartPerc(bool tmpvalue);
	void setResumeStartPerc(int value);
	int getResumeStopPerc(bool tmpvalue);
	void setResumeStopPerc(int value);
	std::string getThemeName(bool tmpvalue);
	void setThemeName(std::string value);
	void addNetworkShare(oldnetworkSource netshare);
	void RefreshNetworkShare(std::vector<oldnetworkSource> newnetsources);
	int getAout(bool tmpvalue);
	void setAout(int value);
	bool getEmuOverrides();
	int getLogLevel();
	bool getConsoleWindow();
	void toggleConsoleWindow();
	std::vector<std::string> getConfigExtensions();
	playerconf_struct getConfig(bool tmpvalue);
	
	std::string getDbPath();
	
	bool Modified();
	
	void ReadConfig();
	void saveSettings();
	
	mINI::INIFile *fileref;
	mINI::INIStructure inidata;
	
private:
	
	playerconf_struct nxmpconfig;
	playerconf_struct nxmptmpconfig;
	
	bool isModified = false;
	bool opened;
	
};



#endif