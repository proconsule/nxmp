#include "config.h"
#include <regex>
#include <string>
#include <vector>
#include "utils.h"

std::vector<std::string> char_split (const std::string &s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss (s);
    std::string item;

    while (getline (ss, item, delim)) {
        result.push_back (item);
    }

    return result;
}

Config::Config(std::string inifile){
	inifilePath = inifile;
	ini = new CSimpleIniA(true,true);
	ini->SetUnicode();
	ini->LoadFile(inifile.c_str());
	
	CSimpleIniA::TNamesDepend values;
	

	
	nxmpconfig.showhidden = ini->GetBoolValue("Main", "showhidden");
	
	nxmpconfig.touchenable = ini->GetBoolValue("Main", "touchenable");
	
	nxmpconfig.playeswipeseek = ini->GetLongValue("Main", "playeswipeseek");
	
	
	nxmpconfig.longseek = ini->GetLongValue("Main", "longseek");
	nxmpconfig.shortseek = ini->GetLongValue("Main", "shortseek");
	if(nxmpconfig.longseek == 0)nxmpconfig.longseek = 60;
	if(nxmpconfig.shortseek == 0)nxmpconfig.shortseek = 10;
	
	
	nxmpconfig.usealang = ini->GetBoolValue("Main", "usealang");
	const char* alangpv;
	alangpv = ini->GetValue("Main", "alang");
	std::string alangstring = "eng";
	if(alangpv!= nullptr){
		alangstring = alangpv;
	}
	if(alangstring == ""){
		alangstring = "eng";
	}
	
	nxmpconfig.alang = Utility::getLanguagesIdx(alangstring);
	
	//Slang
	nxmpconfig.useslang = ini->GetBoolValue("Main", "useslang");
	
	const char* slangpv;
	slangpv = ini->GetValue("Main", "slang");
	std::string slangstring = "eng";
	if(slangpv!= nullptr){
		slangstring = slangpv;
	}
	if(slangstring == ""){
		slangstring = "eng";
	}
	
	nxmpconfig.slang = Utility::getLanguagesIdx(slangstring);
	//endSlang
	
	nxmpconfig.useoc = ini->GetBoolValue("Main", "useoc");
	
	const char* themenamepv;
	themenamepv = ini->GetValue("Main", "theme");
	std::string themenamestring = "Default";
	if(themenamepv!= nullptr){
		themenamestring = themenamepv;
	}
	nxmpconfig.themename = themenamestring;
	
	nxmpconfig.subfontsize = ini->GetLongValue("Main", "subfontsize");
	if(nxmpconfig.subfontsize == 0)nxmpconfig.subfontsize = 55;
	
	
	std::string fontcolorstring = "#FFFFFF";
	const char* fontcolorpv;
	fontcolorpv = ini->GetValue("Main", "subfontcolor");
	if(fontcolorpv!= nullptr){
		fontcolorstring = fontcolorpv;
	}
	if(fontcolorstring == ""){
		fontcolorstring = "#FFFFFF";
	}
	int tmpcolors[3];
	sscanf(fontcolorstring.c_str(),"#%02X%02X%02X",&tmpcolors[0],&tmpcolors[1],&tmpcolors[2]);
	nxmpconfig.subfontcolor[0] = (float)tmpcolors[0]/255.0f;
	nxmpconfig.subfontcolor[1] = (float)tmpcolors[1]/255.0f;
	nxmpconfig.subfontcolor[2] = (float)tmpcolors[2]/255.0f;
	nxmpconfig.subfontcolor[3] = 1.0f;

	//bordercolor
	
	std::string fontcolorstring2 = "#000000";
	const char* fontcolorpv2;
	fontcolorpv2 = ini->GetValue("Main", "subbordercolor");
	if(fontcolorpv2!= nullptr){
		fontcolorstring2 = fontcolorpv2;
	}
	if(fontcolorstring2 == ""){
		fontcolorstring2 = "#000000";
	}
	int tmpcolors2[3];
	sscanf(fontcolorstring2.c_str(),"#%02X%02X%02X",&tmpcolors2[0],&tmpcolors2[1],&tmpcolors2[2]);
	nxmpconfig.subbordercolor[0] = (float)tmpcolors2[0]/255.0f;
	nxmpconfig.subbordercolor[1] = (float)tmpcolors2[1]/255.0f;
	nxmpconfig.subbordercolor[2] = (float)tmpcolors2[2]/255.0f;
	nxmpconfig.subbordercolor[3] = 1.0f;
	//endbordercolor
	
	const char* deintpv;
	deintpv = ini->GetValue("Main", "deinterlace");
	std::string deintstring;
	if(deintpv!= nullptr){
		deintstring = deintpv;
	}
	if(deintstring == ""){
		deintstring = "no";
	}
	
	if(deintstring == "no")nxmpconfig.deint = 0;
	if(deintstring == "yes")nxmpconfig.deint = 1;
	if(deintstring == "auto")nxmpconfig.deint = 2;
	
	
	const char* usedbpv;
	usedbpv = ini->GetValue("Main", "usedb");
	nxmpconfig.dbactive = false;
	if(usedbpv!= nullptr){
		std::string usebdstring = usedbpv;
		if(usebdstring == "yes")nxmpconfig.dbactive=true;
	}
	
	nxmpconfig.startresumeperc = ini->GetLongValue("Main", "startresumeperc");
	nxmpconfig.stopresumeperc = ini->GetLongValue("Main", "stopresumeperc");
	if(nxmpconfig.startresumeperc == 0)nxmpconfig.startresumeperc = 5;
	if(nxmpconfig.stopresumeperc == 0)nxmpconfig.stopresumeperc = 5;
	
	nxmptmpconfig = nxmpconfig;
	
	
	topmenu.push_back("Local Files");
#ifdef NXMP_USBSUPPORT
	topmenu.push_back("USB");
#endif
#ifdef NXMP_NETWORKSUPPORT
	ini->GetAllValues("Network", "source", values);
	if(getNetworks().size() != 0){
		topmenu.push_back("Network");
	}
#endif
#ifdef NXMP_UPNPSUPPORT
	topmenu.push_back("UPNP");
#endif
#ifdef NXMP_ENIGMASUPPORT
	if(getEnigma() != ""){
		topmenu.push_back("Enigma2");
	}
#endif
	topmenu.push_back("Playlist");
	topmenu.push_back("Settings");
	topmenu.push_back("Info");
	topmenu.push_back("Exit");
	
}

std::vector<networkSource> Config::getNetworks(){
	std::vector<networkSource> tmpret;
	CSimpleIniA::TNamesDepend values;
	ini->GetAllValues("Network", "source", values);
	if(values.size() == 0)return tmpret;
	CSimpleIniA::TNamesDepend::const_iterator it;
	
	for (it = values.begin(); it != values.end(); ++it) {
		networkSource tmpsource;
		std::string test = it->pItem;
		std::vector<std::string> splitted = char_split(test,'|');
		tmpsource.name = splitted[0];
		tmpsource.url = splitted[1];
		tmpret.push_back(tmpsource);
	}
	return tmpret;
}

std::string Config::getStartPath(){
	const char* pv;
	pv = ini->GetValue("Main", "startpath");
	if(pv==nullptr){
#ifdef __SWITCH__
		return "/switch/nxmp";
#else
#ifdef _WIN32
		return "c:\\";
#endif
#endif
	}
	return pv;
}

std::string Config::getEnigma(){
	const char* pv;
	pv = ini->GetValue("Enigma2", "e2address");
	if(pv==nullptr){
		return "";
	}
	return pv;
	
}

bool Config::getshowHidden(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.showhidden;
	}
	return nxmpconfig.showhidden; 
}

void Config::setshowHidden(bool val){
	nxmptmpconfig.showhidden = val;
}

bool Config::getTouchEnable(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.touchenable;
	}
	return nxmpconfig.touchenable; 
}
void Config::setTouchEnable(bool val){
	nxmptmpconfig.touchenable = val;
}

int Config::getPlayerSwipeSeek(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.playeswipeseek;
	}
	return nxmpconfig.playeswipeseek; 
}

void Config::setPlayerSwipeSeek(int val){
	nxmptmpconfig.playeswipeseek = val;
}

int Config::getLongSeek(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.longseek;
	}
	return nxmpconfig.longseek; 
}

int Config::getShortSeek(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.shortseek;
	}
	return nxmpconfig.shortseek;
}

void Config::setLongSeek(int seektime){
	nxmptmpconfig.longseek = seektime;
}
void Config::setShortSeek(int seektime){
	nxmptmpconfig.shortseek = seektime;
}

bool Config::getUseAlang(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.usealang;
	}
	return nxmpconfig.usealang;
}
void Config::setUseAlang(bool _val){
	nxmptmpconfig.usealang = _val;
}

int Config::getAlang(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.alang;
	}
	return nxmpconfig.alang; 
}

void Config::setAlang(int lang){
	nxmptmpconfig.alang = lang;
}

//Slang
bool Config::getUseSlang(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.useslang;
	}
	return nxmpconfig.useslang;
}
void Config::setUseSlang(bool _val){
	nxmptmpconfig.useslang = _val;
}

int Config::getSlang(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.slang;
	}
	return nxmpconfig.slang; 
}

void Config::setSlang(int lang){
	nxmptmpconfig.slang = lang;
}
//end Slang

bool Config::getUseOc(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.useoc;
	}
	return nxmpconfig.useoc;
}
void Config::setUseOc(bool _val){
	nxmptmpconfig.useoc = _val;
}

int Config::getSubFontSize(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.subfontsize;
	}
	return nxmpconfig.subfontsize; 
}

void Config::setSubFontSize(int val){
	nxmptmpconfig.subfontsize = val;
}

float * Config::getSubFontColor(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.subfontcolor;
	}
	return nxmpconfig.subfontcolor; 
}
std::string Config::getSubFontColorHex(bool tmpvalue){
	char subfontcstr[32];
	if(tmpvalue){
		sprintf(subfontcstr,"#%02X%02X%02X",(unsigned int)(nxmptmpconfig.subfontcolor[0]*255.0f),(unsigned int)(nxmptmpconfig.subfontcolor[1]*255.0f),(unsigned int)(nxmptmpconfig.subfontcolor[2]*255.0f));
		return std::string(subfontcstr);
	}
	sprintf(subfontcstr,"#%02X%02X%02X",(unsigned int)(nxmpconfig.subfontcolor[0]*255.0f),(unsigned int)(nxmpconfig.subfontcolor[1]*255.0f),(unsigned int)(nxmpconfig.subfontcolor[2]*255.0f));
	return std::string(subfontcstr);	
}
void Config::setSubFontColor(float  *_color){
	nxmptmpconfig.subfontcolor[0] = _color[0];
	nxmptmpconfig.subfontcolor[1] = _color[1];
	nxmptmpconfig.subfontcolor[2] = _color[2];
}

//bordercolor
float * Config::getSubBorderColor(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.subbordercolor;
	}
	return nxmpconfig.subbordercolor; 
}
std::string Config::getSubBorderColorHex(bool tmpvalue){
	char subfontcstr2[32];
	if(tmpvalue){
		sprintf(subfontcstr2,"#%02X%02X%02X",(unsigned int)(nxmptmpconfig.subbordercolor[0]*255.0f),(unsigned int)(nxmptmpconfig.subbordercolor[1]*255.0f),(unsigned int)(nxmptmpconfig.subbordercolor[2]*255.0f));
		return std::string(subfontcstr2);
	}
	sprintf(subfontcstr2,"#%02X%02X%02X",(unsigned int)(nxmpconfig.subbordercolor[0]*255.0f),(unsigned int)(nxmpconfig.subbordercolor[1]*255.0f),(unsigned int)(nxmpconfig.subbordercolor[2]*255.0f));
	return std::string(subfontcstr2);	
}
void Config::setSubBorderColor(float  *_color){
	nxmptmpconfig.subbordercolor[0] = _color[0];
	nxmptmpconfig.subbordercolor[1] = _color[1];
	nxmptmpconfig.subbordercolor[2] = _color[2];
}
//endbordercolor

void Config::setDeinterlace(int value){
	nxmptmpconfig.deint = value;
}

int Config::getDeinterlace(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.deint;
	}
	return nxmpconfig.deint; 
}


bool Config::getDbActive(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.dbactive;
	}
	return nxmpconfig.dbactive; 
}

void Config::setDbActive(bool value){
	nxmptmpconfig.dbactive = value;
}

int Config::getResumeStartPerc(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.startresumeperc;
	}
	return nxmpconfig.startresumeperc;
}
void Config::setResumeStartPerc(int value){
	nxmptmpconfig.startresumeperc = value;
}

int Config::getResumeStopPerc(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.stopresumeperc;
	}
	return nxmpconfig.stopresumeperc;
}
void Config::setResumeStopPerc(int value){
	nxmptmpconfig.stopresumeperc = value;
}

std::string Config::getThemeName(bool tmpvalue){
	if(tmpvalue){
		return nxmptmpconfig.themename;
	}
	return nxmpconfig.themename;
}

void Config::setThemeName(std::string value){
	nxmptmpconfig.themename = value;
}

void Config::saveSettings(){
	
	
	nxmpconfig = nxmptmpconfig;
	
	
	ini->Delete("Main", "showhidden");
	ini->SetBoolValue("Main", "showhidden",nxmpconfig.showhidden,NULL,false);
	
	
	ini->Delete("Main", "touchenable");
	ini->SetBoolValue("Main", "touchenable",nxmpconfig.touchenable,NULL,false);
	
	ini->Delete("Main", "playeswipeseek");
	ini->SetLongValue("Main", "playeswipeseek",nxmpconfig.playeswipeseek,NULL,false);
	
	ini->Delete("Main", "shortseek");
	ini->SetLongValue("Main", "shortseek", nxmpconfig.shortseek, NULL, false);
	ini->Delete("Main", "longseek");
	ini->SetLongValue("Main", "longseek", nxmpconfig.longseek, NULL, false);
	
	ini->Delete("Main", "usealang");
	ini->SetBoolValue("Main", "usealang", nxmpconfig.usealang, NULL, false);
	
	ini->Delete("Main", "alang");
	ini->SetValue("Main", "alang", Utility::getLanguages()[nxmpconfig.alang].lang3.c_str());
	
	//slang
	ini->Delete("Main", "useslang");
	ini->SetBoolValue("Main", "useslang", nxmpconfig.useslang, NULL, false);
	
	ini->Delete("Main", "slang");
	ini->SetValue("Main", "slang", Utility::getLanguages()[nxmpconfig.slang].lang3.c_str());
	//end slang

	ini->Delete("Main", "useoc");
	ini->SetBoolValue("Main", "useoc", nxmpconfig.useoc, NULL, false);

	ini->Delete("Main", "subfontsize");
	ini->SetLongValue("Main", "subfontsize", nxmpconfig.subfontsize, NULL, false);
	
	//Fix Save color.
	ini->Delete("Main", "subfontcolor");
	ini->SetValue("Main", "subfontcolor", getSubFontColorHex(true).c_str());
	
	//bordercolor
	ini->Delete("Main", "subbordercolor");
	ini->SetValue("Main", "subbordercolor", getSubBorderColorHex(true).c_str());
	//endbordercolor
    //end Fix Save Color
	std::vector<std::string> deintopts = {"no","yes","auto"};
	ini->Delete("Main", "deinterlace");
	ini->SetValue("Main", "deinterlace", deintopts[nxmpconfig.deint].c_str());
	std::vector<std::string> usedbintopts = {"no","yes"};
	ini->Delete("Main", "usedb");
	ini->SetValue("Main", "usedb", usedbintopts[nxmpconfig.dbactive].c_str());
	
	ini->Delete("Main", "startresumeperc");
	ini->SetLongValue("Main", "startresumeperc", nxmpconfig.startresumeperc, NULL, false);
	ini->Delete("Main", "stopresumeperc");
	ini->SetLongValue("Main", "stopresumeperc", nxmpconfig.stopresumeperc, NULL, false);
	
	ini->Delete("Main", "theme");
	ini->SetValue("Main", "theme", nxmpconfig.themename.c_str());
	
	std::string data;
	ini->Save(data);
	ini->SaveFile(inifilePath.c_str());
}
