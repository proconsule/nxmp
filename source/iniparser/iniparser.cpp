#include "iniparser.h"
#include "utils.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <iterator>

#include "nxmp-i18n.h"


bool string_to_bool(std::string boolstring){
	if(boolstring == "true"){
		return true;
	}
	if(boolstring == "false"){
		return false;
	}
	return false;
}

int string_to_int(std::string intstring){
	return stoi(intstring);
}

std::string bool_to_string(bool value){
	return value ? std::string("true"):std::string("false");
}

std::string int_to_string(int value){
	return std::to_string(value);
}

std::string float_to_string(float value){
	std::stringstream ss1;
	ss1 << value;
	std::string str1 = ss1.str();
	return str1;
}

float string_to_float(std::string floatstring){
	return std::stof(floatstring);
}

std::string vector_to_string(std::vector<std::string> vectorval){
	std::stringstream ss;
 
    for (size_t i = 0; i < vectorval.size(); i++) {
        if (i != 0) {
            ss << ",";
        }
        ss << vectorval[i];
    }
	return ss.str();
}


std::vector<std::string> split_extension (const std::string &s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss (s);
    std::string item;

    while (getline (ss, item, delim)) {
        result.push_back (item);
    }

    return result;
}

void CIniParser::PrintConfig(){
	printf("Config:\n");
	printf("hwdec: %s\n",bool_to_string(nxmpconfig.hwdec).c_str());
	printf("vsync: %s\n",bool_to_string(nxmpconfig.vsync).c_str());
	printf("dbactive: %s\n",bool_to_string(nxmpconfig.dbactive).c_str());
	printf("useoc: %s\n",bool_to_string(nxmpconfig.useoc).c_str());
	printf("showhidden: %s\n",bool_to_string(nxmpconfig.showhidden).c_str());
	printf("touchenable: %s\n",bool_to_string(nxmpconfig.touchenable).c_str());
	printf("deint: %s\n",int_to_string(nxmpconfig.deint).c_str());
	printf("demuxcachesec %s\n",int_to_string(nxmpconfig.demuxcachesec).c_str());
	printf("longseek %s\n",int_to_string(nxmpconfig.longseek).c_str());
	printf("shortseek %s\n",int_to_string(nxmpconfig.shortseek).c_str());
	printf("usealang %s\n",bool_to_string(nxmpconfig.usealang).c_str());
	printf("alang %s\n",int_to_string(nxmpconfig.alang).c_str());
	printf("useslang %s\n",bool_to_string(nxmpconfig.useslang).c_str());
	printf("slang %s\n",int_to_string(nxmpconfig.slang).c_str());
	printf("subfontsize %s\n",int_to_string(nxmpconfig.subfontsize).c_str());
	printf("subfontscale %s\n",float_to_string(nxmpconfig.subfontscale).c_str());
	
	printf("themename %s\n",nxmpconfig.themename.c_str());
	printf("emuoverrides %s\n",bool_to_string(nxmpconfig.emuoverrides).c_str());
	printf("loglevel %s\n",int_to_string(nxmpconfig.loglevel).c_str());
	printf("consolewindow %s\n",bool_to_string(nxmpconfig.consolewindow).c_str());
	
	printf("Enabled Extension num: %lu\n",nxmpconfig.config_enabled_extensions.size());
	
	
}

CIniParser::CIniParser(std::string inifile){

	fileref = new mINI::INIFile(inifile);
	ReadConfig();
	
	
}

CIniParser::~CIniParser(){
	
	
}

bool CIniParser::getshowHidden(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.showhidden;
	}
	return nxmpconfig.showhidden; 
}

void CIniParser::setshowHidden(bool val){
	isModified = true;
	nxmptmpconfig.showhidden = val;
}

int CIniParser::getsortOrder(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.sortorder;
	}
	return nxmpconfig.sortorder;
}
void CIniParser::setsortOrder(int val){
	isModified = true;
	nxmptmpconfig.sortorder = val;
}

bool CIniParser::getTouchEnable(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.touchenable;
	}
	return nxmpconfig.touchenable; 
}
void CIniParser::setTouchEnable(bool val){
	isModified = true;
	nxmptmpconfig.touchenable = val;
}

int CIniParser::getPlayerSwipeSeek(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.playeswipeseek;
	}
	return nxmpconfig.playeswipeseek; 
}

void CIniParser::setPlayerSwipeSeek(int val){
	isModified = true;
	nxmptmpconfig.playeswipeseek = val;
}

int CIniParser::getLongSeek(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.longseek;
	}
	return nxmpconfig.longseek; 
}

int CIniParser::getShortSeek(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.shortseek;
	}
	return nxmpconfig.shortseek;
}

void CIniParser::setLongSeek(int seektime){
	isModified = true;
	nxmptmpconfig.longseek = seektime;
}
void CIniParser::setShortSeek(int seektime){
	isModified = true;
	nxmptmpconfig.shortseek = seektime;
}

bool CIniParser::getUseAlang(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.usealang;
	}
	return nxmpconfig.usealang;
}
void CIniParser::setUseAlang(bool _val){
	isModified = true;
	nxmptmpconfig.usealang = _val;
}

int CIniParser::getAlang(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.alang;
	}
	return nxmpconfig.alang; 
}

void CIniParser::setAlang(int lang){
	isModified = true;
	nxmptmpconfig.alang = lang;
}

//Slang
bool CIniParser::getUseSlang(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.useslang;
	}
	return nxmpconfig.useslang;
}
void CIniParser::setUseSlang(bool _val){
	isModified = true;
	nxmptmpconfig.useslang = _val;
}

int CIniParser::getSlang(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.slang;
	}
	return nxmpconfig.slang; 
}

void CIniParser::setSlang(int lang){
	isModified = true;
	nxmptmpconfig.slang = lang;
}
//end Slang

int CIniParser::getInterfaceLang(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.intlang;
	}
	return nxmpconfig.intlang; 
}
	
void CIniParser::setInterfaceLang(int lang){
	isModified = true;
	nxmptmpconfig.intlang = lang;
}

bool CIniParser::getOnlyLatinRange(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.onlylatinrange;
	}
	return nxmpconfig.onlylatinrange; 
}
	
void CIniParser::setOnlyLatinRange(bool latinrange){
	isModified = true;
	nxmptmpconfig.onlylatinrange = latinrange;
}


bool CIniParser::getUseOc(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.useoc;
	}
	return nxmpconfig.useoc;
}
void CIniParser::setUseOc(bool _val){
	isModified = true;
	nxmptmpconfig.useoc = _val;
}

bool CIniParser::getHWDec(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.hwdec;
	}
	return nxmpconfig.hwdec;
}
void CIniParser::setHWDec(bool _val){
	isModified = true;
	nxmptmpconfig.hwdec = _val;
}

bool CIniParser::getVSYNC(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.vsync;
	}
	return nxmpconfig.vsync;
}
void CIniParser::setVSYNC(bool _val){
	isModified = true;
	nxmptmpconfig.vsync = _val;
}

int CIniParser::getSubFontSize(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.subfontsize;
	}
	return nxmpconfig.subfontsize; 
}

void CIniParser::setSubFontSize(int val){
	isModified = true;
	nxmptmpconfig.subfontsize = val;
}

//subscale
float CIniParser::getSubFontScale(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.subfontscale;
	}
	return nxmpconfig.subfontscale; 
}

void CIniParser::setSubFontScale(float val){
	isModified = true;
	nxmptmpconfig.subfontscale = val;
}
//subscale

float * CIniParser::getSubFontColor(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.subfontcolor;
	}
	return nxmpconfig.subfontcolor; 
}
std::string CIniParser::getSubFontColorHex(bool tmpvalue){
	char subfontcstr[32];
	if(tmpvalue){
		sprintf(subfontcstr,"#%02X%02X%02X",(unsigned int)(nxmptmpconfig.subfontcolor[0]*255.0f),(unsigned int)(nxmptmpconfig.subfontcolor[1]*255.0f),(unsigned int)(nxmptmpconfig.subfontcolor[2]*255.0f));
		return std::string(subfontcstr);
	}
	sprintf(subfontcstr,"#%02X%02X%02X",(unsigned int)(nxmpconfig.subfontcolor[0]*255.0f),(unsigned int)(nxmpconfig.subfontcolor[1]*255.0f),(unsigned int)(nxmpconfig.subfontcolor[2]*255.0f));
	return std::string(subfontcstr);	
}
void CIniParser::setSubFontColor(float  *_color){
	isModified = true;
	nxmptmpconfig.subfontcolor[0] = _color[0];
	nxmptmpconfig.subfontcolor[1] = _color[1];
	nxmptmpconfig.subfontcolor[2] = _color[2];
}

//bordercolor
float * CIniParser::getSubBorderColor(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.subbordercolor;
	}
	return nxmpconfig.subbordercolor; 
}
std::string CIniParser::getSubBorderColorHex(bool tmpvalue){
	char subfontcstr2[32];
	if(tmpvalue){
		sprintf(subfontcstr2,"#%02X%02X%02X",(unsigned int)(nxmptmpconfig.subbordercolor[0]*255.0f),(unsigned int)(nxmptmpconfig.subbordercolor[1]*255.0f),(unsigned int)(nxmptmpconfig.subbordercolor[2]*255.0f));
		return std::string(subfontcstr2);
	}
	sprintf(subfontcstr2,"#%02X%02X%02X",(unsigned int)(nxmpconfig.subbordercolor[0]*255.0f),(unsigned int)(nxmpconfig.subbordercolor[1]*255.0f),(unsigned int)(nxmpconfig.subbordercolor[2]*255.0f));
	return std::string(subfontcstr2);	
}
void CIniParser::setSubBorderColor(float  *_color){
	isModified = true;
	nxmptmpconfig.subbordercolor[0] = _color[0];
	nxmptmpconfig.subbordercolor[1] = _color[1];
	nxmptmpconfig.subbordercolor[2] = _color[2];
}
//endbordercolor

void CIniParser::setDeinterlace(int value){
	isModified = true;
	nxmptmpconfig.deint = value;
}

int CIniParser::getDeinterlace(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.deint;
	}
	return nxmpconfig.deint; 
}


void CIniParser::setDemuxCache(int mycachesec){
	nxmptmpconfig.demuxcachesec = mycachesec;
}

int CIniParser::getDemuxCache(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.demuxcachesec;
	}
	return nxmpconfig.demuxcachesec; 
}


bool CIniParser::getDbActive(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.dbactive;
	}
	return nxmpconfig.dbactive; 
}

void CIniParser::setDbActive(bool value){
	isModified = true;
	nxmptmpconfig.dbactive = value;
}

int CIniParser::getResumeStartPerc(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.startresumeperc;
	}
	return nxmpconfig.startresumeperc;
}
void CIniParser::setResumeStartPerc(int value){
	isModified = true;
	nxmptmpconfig.startresumeperc = value;
}

int CIniParser::getResumeStopPerc(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.stopresumeperc;
	}
	return nxmpconfig.stopresumeperc;
}
void CIniParser::setResumeStopPerc(int value){
	isModified = true;
	nxmptmpconfig.stopresumeperc = value;
}

int CIniParser::getThemeColor(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.themecolor;
	}
	return nxmpconfig.themecolor;
}
void CIniParser::setThemeColor(int value){
	isModified = true;
	nxmptmpconfig.themecolor = value;
}


std::string CIniParser::getThemeName(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.themename;
	}
	return nxmpconfig.themename;
}

playerconf_struct CIniParser::getConfig(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig;
	}
	return nxmpconfig;
}

int CIniParser::getAout(bool tmpvalue){
#ifdef OPENGL_BACKEND
	if(tmpvalue){
		return nxmptmpconfig.aout;
	}
	return nxmpconfig.aout;
#endif
#ifdef DEKO3D_BACKEND
	return 1;
#endif
}

void CIniParser::setAout(int value){
	isModified = true;
#ifdef OPENGL_BACKEND	
	nxmptmpconfig.aout = value;
#endif
#ifdef DEKO3D_BACKEND
	nxmptmpconfig.aout = 1;
#endif
}

bool CIniParser::getEmuOverrides(){
	return nxmpconfig.emuoverrides;
}

int CIniParser::getLogLevel(){
	return nxmpconfig.loglevel;
}

bool CIniParser::getConsoleWindow(){
	return nxmpconfig.consolewindow;
}

void CIniParser::toggleConsoleWindow(){
	nxmpconfig.consolewindow = !nxmpconfig.consolewindow;
	nxmptmpconfig.consolewindow = nxmpconfig.consolewindow;
}

int CIniParser::getExitMode(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.exitmode;
	}
	return nxmpconfig.exitmode;
}

void CIniParser::setExitMode(int value){
	isModified = true;
	nxmptmpconfig.exitmode = value;
}

std::vector<std::string> CIniParser::getConfigExtensions(){
	
	//const char* pv;
	//pv = ini->GetValue("Main", "enabled_extensions",".8svx,.aac,.ac3,.aif,.asf,.avi,.dv,.flv,.m2ts,.m2v,.m4a,.mkv,.mov,.mp3,.mp4,.mpeg,.mpg,.mts,.ogg,.rmvb,.swf,.ts,.vob,.wav,.wma,.wmv,.flac,.m3u,.m3u8,.webm,.jpg,.gif,.png,.iso");
	//nxmpconfig.config_enabled_extensions = char_split(std::string(pv),',');
	
	return nxmpconfig.config_enabled_extensions;
}

std::string CIniParser::getStartPath(){
	return nxmpconfig.startpath;
}

std::string CIniParser::getEnigma(){
	return nxmpconfig.e2addr;
	
}

void CIniParser::RefreshNetworkShare(std::vector<oldnetworkSource> newnetsources){
	
	inidata["Network"].clear();
	for(int i=0;i<newnetsources.size();i++){
		inidata["Network"][newnetsources[i].name] = newnetsources[i].url;
	}
	fileref->write(inidata);
	ReadConfig();
}

std::vector<oldnetworkSource> CIniParser::getNetworks(){
	std::vector<oldnetworkSource> retNet;
	auto& collection = inidata["Network"];
	for (auto const& it2 : collection){
		oldnetworkSource tmpsrc;
		auto const& key = it2.first;
		auto const& value = it2.second;
		tmpsrc.name = key;
		tmpsrc.url = value;
		retNet.push_back(tmpsrc);
	}
	return retNet;
}

void CIniParser::addNetworkShare(oldnetworkSource netshare){
	std::vector<oldnetworkSource> actuallist = getNetworks();
	
	actuallist.push_back(netshare);
	RefreshNetworkShare(actuallist);
}

void CIniParser::ReadConfig(){
	
	inidata.clear();
	
	opened =  fileref->read(inidata);
	
	if(opened){
	
		if(inidata["Main"].has("startpath")){
			nxmpconfig.startpath = inidata.get("Main").get("startpath");
		}
		if(inidata["Main"].has("dbpath")){
			nxmpconfig.dbpath = inidata.get("Main").get("dbpath");
		}
		if(inidata["Main"].has("exitmode")){
			nxmpconfig.exitmode = string_to_int(inidata.get("Main").get("exitmode"));
		}
		if(inidata["Main"].has("hwdec")){
			nxmpconfig.hwdec = string_to_bool(inidata.get("Main").get("hwdec"));
		}
		if(inidata["Main"].has("vsync")){
			nxmpconfig.vsync = string_to_bool(inidata.get("Main").get("vsync"));
		}
		if(inidata["Main"].has("usedb")){
			nxmpconfig.dbactive = string_to_bool(inidata.get("Main").get("usedb"));
		}
		if(inidata["Main"].has("useoc")){
			nxmpconfig.useoc = string_to_bool(inidata.get("Main").get("useoc"));
		}
		if(inidata["Main"].has("showhidden")){
			nxmpconfig.showhidden = string_to_bool(inidata.get("Main").get("showhidden"));
		}
		if(inidata["Main"].has("sortorder")){
			nxmpconfig.sortorder = string_to_int(inidata.get("Main").get("sortorder"));
		}
		if(inidata["Main"].has("touchenable")){
			nxmpconfig.touchenable = string_to_bool(inidata.get("Main").get("touchenable"));
		}
		if(inidata["Main"].has("themecolor")){
			nxmpconfig.themecolor = string_to_int(inidata.get("Main").get("themecolor"));
		}
		if(inidata["Main"].has("deinterlace")){
			std::string deintstring = inidata.get("Main").get("deinterlace");
			if(deintstring == "no")nxmpconfig.deint = 0;
			if(deintstring == "yes")nxmpconfig.deint = 1;
			if(deintstring == "auto")nxmpconfig.deint = 2;
		}
		if(inidata["Main"].has("demuxcachesec")){
			nxmpconfig.demuxcachesec = string_to_int(inidata.get("Main").get("demuxcachesec"));
		}
		if(inidata["Main"].has("aout")){
			nxmpconfig.aout = string_to_int(inidata.get("Main").get("aout"));
		}
		if(inidata["Main"].has("playeswipeseek")){
			nxmpconfig.playeswipeseek = string_to_bool(inidata.get("Main").get("playeswipeseek"));
		}
		if(inidata["Main"].has("longseek")){
			nxmpconfig.longseek = string_to_int(inidata.get("Main").get("longseek"));
		}
		if(inidata["Main"].has("shortseek")){
			nxmpconfig.shortseek = string_to_int(inidata.get("Main").get("shortseek"));
		}
		if(inidata["Main"].has("usealang")){
			nxmpconfig.usealang = string_to_bool(inidata.get("Main").get("usealang"));
		}
		if(inidata["Main"].has("alang")){
			nxmpconfig.alang =  Utility::getLanguagesIdx(inidata.get("Main").get("alang"));
		}
		if(inidata["Main"].has("useslang")){
			nxmpconfig.useslang = string_to_bool(inidata.get("Main").get("useslang"));
		}
		if(inidata["Main"].has("slang")){
			nxmpconfig.slang =  Utility::getLanguagesIdx(inidata.get("Main").get("slang"));
		}
		if(inidata["Main"].has("intlang")){
			nxmpconfig.intlang =  string_to_int(inidata.get("Main").get("intlang"));
		}
		if(inidata["Main"].has("onlylatinrange")){
			nxmpconfig.onlylatinrange =  string_to_bool(inidata.get("Main").get("onlylatinrange"));
		}
		if(inidata["Main"].has("subfontsize")){
			nxmpconfig.subfontsize = string_to_int(inidata.get("Main").get("subfontsize"));
		}
		if(inidata["Main"].has("subfontscale")){
			nxmpconfig.subfontscale = string_to_int(inidata.get("Main").get("subfontscale"));
		}
		if(inidata["Main"].has("subfontcolor")){
			std::string fontcolorstr = inidata.get("Main").get("subfontcolor");
			int tmpcolors[3];
			sscanf(fontcolorstr.c_str(),"#%02X%02X%02X",&tmpcolors[0],&tmpcolors[1],&tmpcolors[2]);
			nxmpconfig.subfontcolor[0] = (float)tmpcolors[0]/255.0f;
			nxmpconfig.subfontcolor[1] = (float)tmpcolors[1]/255.0f;
			nxmpconfig.subfontcolor[2] = (float)tmpcolors[2]/255.0f;
			nxmpconfig.subfontcolor[3] = 1.0f;
		
		}
		if(inidata["Main"].has("subbordercolor")){
			std::string fontcolorstr = inidata.get("Main").get("subbordercolor");
			int tmpcolors[3];
			sscanf(fontcolorstr.c_str(),"#%02X%02X%02X",&tmpcolors[0],&tmpcolors[1],&tmpcolors[2]);
			nxmpconfig.subbordercolor[0] = (float)tmpcolors[0]/255.0f;
			nxmpconfig.subbordercolor[1] = (float)tmpcolors[1]/255.0f;
			nxmpconfig.subbordercolor[2] = (float)tmpcolors[2]/255.0f;
			nxmpconfig.subbordercolor[3] = 1.0f;
		
		}
		
		
		if(inidata["Main"].has("startresumeperc")){
			nxmpconfig.startresumeperc = string_to_int(inidata.get("Main").get("startresumeperc"));
		}
		if(inidata["Main"].has("stopresumeperc")){
			nxmpconfig.stopresumeperc = string_to_int(inidata.get("Main").get("stopresumeperc"));
		}
		if(inidata["Main"].has("enabled_extensions")){
			std::string enabled_extensions_string = inidata.get("Main").get("enabled_extensions");
		}
		if(inidata["Main"].has("enabled_extensions")){
			std::string enabled_extensions_string = inidata.get("Main").get("enabled_extensions");
			nxmpconfig.config_enabled_extensions = split_extension(enabled_extensions_string,',');
			Utility::setMediaExtensions(nxmpconfig.config_enabled_extensions);
		}
		if(inidata["Main"].has("theme")){
			nxmpconfig.themename = inidata.get("Main").get("theme");
		}
		if(inidata["Main"].has("emuoverrides")){
			nxmpconfig.emuoverrides = string_to_bool(inidata.get("Main").get("emuoverrides"));
		}
		if(inidata["Main"].has("loglevel")){
			nxmpconfig.loglevel = string_to_int(inidata.get("Main").get("loglevel"));
		}
		if(inidata["Main"].has("consolewindow")){
			nxmpconfig.consolewindow = string_to_bool(inidata.get("Main").get("consolewindow"));
		}
		if(inidata["Enigma2"].has("e2address")){
			nxmpconfig.e2addr = inidata.get("Enigma2").get("e2address");
		}
		
		InitLang((NX_LANGS)nxmpconfig.intlang);
	
	}
	
	
	nxmptmpconfig = nxmpconfig;
	isModified = false;

}
void CIniParser::saveSettings(){
	
	
	nxmpconfig = nxmptmpconfig;
	
	
	
	inidata["Main"].set({
		{"startpath", nxmpconfig.startpath},
		{"dbpath", nxmpconfig.dbpath},
		{"hwdec", bool_to_string(nxmpconfig.hwdec)},
		{"vsync", bool_to_string(nxmpconfig.vsync)},
		{"usedb", bool_to_string(nxmpconfig.dbactive)},
		{"useoc", bool_to_string(nxmpconfig.useoc)},
		{"showhidden", bool_to_string(nxmpconfig.showhidden)},
		{"sortorder", int_to_string(nxmpconfig.sortorder)},
		{"touchenable", bool_to_string(nxmpconfig.touchenable)},
		{"themecolor", int_to_string(nxmpconfig.themecolor)},
		{"demuxcachesec", int_to_string(nxmpconfig.demuxcachesec)},
		{"playeswipeseek", bool_to_string(nxmpconfig.playeswipeseek)},
		{"longseek", int_to_string(nxmpconfig.longseek)},
		{"shortseek", int_to_string(nxmpconfig.shortseek)},
		{"usealang", bool_to_string(nxmpconfig.usealang)},
		{"aout", int_to_string(nxmpconfig.aout)},
		{"alang", Utility::getLanguages()[nxmpconfig.alang].lang3},
		{"useslang", bool_to_string(nxmpconfig.useslang)},
		{"slang",Utility::getLanguages()[nxmpconfig.slang].lang3},
		{"intlang",int_to_string(nxmpconfig.intlang)},
		{"onlylatinrange",bool_to_string(nxmpconfig.onlylatinrange)},
		{"subfontsize", int_to_string(nxmpconfig.subfontsize)},
		{"subfontscale", float_to_string(nxmpconfig.subfontscale)},
		{"subfontcolor", getSubFontColorHex(true)},
		{"subbordercolor", getSubBorderColorHex(true)},
		{"playeswipeseek", bool_to_string(nxmpconfig.playeswipeseek)},
		{"startresumeperc", int_to_string(nxmpconfig.startresumeperc)},
		{"stopresumeperc", int_to_string(nxmpconfig.stopresumeperc)},
		{"theme", nxmpconfig.themename},
		{"consolewindow", bool_to_string(nxmpconfig.consolewindow)},
		{"enabled_extensions", vector_to_string(nxmpconfig.config_enabled_extensions)}
		
		
	});
	
	
	if(opened){
		fileref->write(inidata, true);
	}else{
		fileref->generate(inidata, true);
	}
	
}


std::string CIniParser::getDbPath(){
	return nxmpconfig.dbpath;
}

void CIniParser::setThemeName(std::string value){
	isModified = true;
	nxmptmpconfig.themename = value;
}

bool CIniParser::Modified(){
	return isModified;
}