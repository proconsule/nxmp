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
	
	
	showhidden = ini->GetBoolValue("Main", "showhidden");
	tmpshowhidden = showhidden;
	
	touchenable = ini->GetBoolValue("Main", "touchenable");
	tmptouchenable = touchenable;
	
	playeswipeseek = ini->GetLongValue("Main", "playeswipeseek");
	tmpplayeswipeseek = playeswipeseek;
	
	
	longseek = ini->GetLongValue("Main", "longseek");
	shortseek = ini->GetLongValue("Main", "shortseek");
	if(longseek == 0)longseek = 60;
	if(shortseek == 0)shortseek = 10;
	
	tmplongseek = longseek;
	tmpshortseek = shortseek;
	

	usealang = ini->GetBoolValue("Main", "usealang");
	tmpusealang = usealang;
	const char* alangpv;
	alangpv = ini->GetValue("Main", "alang");
	std::string alangstring = "eng";
	if(alangpv!= nullptr){
		alangstring = alangpv;
	}
	if(alangstring == ""){
		alangstring = "eng";
	}
	
	alang = Utility::getLanguagesIdx(alangstring);
	tmpalang = alang;

	//Slang
	useslang = ini->GetBoolValue("Main", "useslang");
	tmpuseslang = useslang;
	const char* slangpv;
	slangpv = ini->GetValue("Main", "slang");
	std::string slangstring = "eng";
	if(slangpv!= nullptr){
		slangstring = slangpv;
	}
	if(slangstring == ""){
		slangstring = "eng";
	}
	
	slang = Utility::getLanguagesIdx(slangstring);
	tmpslang = slang;
	//endSlang
	
	const char* themenamepv;
	themenamepv = ini->GetValue("Main", "theme");
	std::string themenamestring = "Default";
	if(themenamepv!= nullptr){
		themenamestring = themenamepv;
	}
	themename = themenamestring;
	tmpthemename = themenamestring;
	
	subfontsize = ini->GetLongValue("Main", "subfontsize");
	if(subfontsize == 0)subfontsize = 55;
	tmpsubfontsize = subfontsize;
	
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
	tmpsubfontcolor[0] = subfontcolor[0] = (float)tmpcolors[0]/255.0f;
	tmpsubfontcolor[1] = subfontcolor[1] = (float)tmpcolors[1]/255.0f;
	tmpsubfontcolor[2] = subfontcolor[2] = (float)tmpcolors[2]/255.0f;
	tmpsubfontcolor[3] = subfontcolor[3] = 1.0f;

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
	tmpsubbordercolor[0] = subbordercolor[0] = (float)tmpcolors2[0]/255.0f;
	tmpsubbordercolor[1] = subbordercolor[1] = (float)tmpcolors2[1]/255.0f;
	tmpsubbordercolor[2] = subbordercolor[2] = (float)tmpcolors2[2]/255.0f;
	tmpsubbordercolor[3] = subbordercolor[3] = 1.0f;
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
	
	if(deintstring == "no")tmpdeint = 0;
	if(deintstring == "yes")tmpdeint = 1;
	if(deintstring == "auto")tmpdeint = 2;
	deint = tmpdeint;
	
	
	const char* usedbpv;
	usedbpv = ini->GetValue("Main", "usedb");
	dbactive = false;
	if(usedbpv!= nullptr){
		std::string usebdstring = usedbpv;
		if(usebdstring == "yes")dbactive=true;
	}
	tmpdbactive = dbactive;
	
	startresumeperc = ini->GetLongValue("Main", "startresumeperc");
	stopresumeperc = ini->GetLongValue("Main", "stopresumeperc");
	if(startresumeperc == 0)startresumeperc = 5;
	if(stopresumeperc == 0)stopresumeperc = 5;
	tmpstartresumeperc = startresumeperc;
	tmpstopresumeperc = stopresumeperc;
	
	
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
		return tmpshowhidden;
	}
	return showhidden; 
}

void Config::setshowHidden(bool val){
	tmpshowhidden = val;
}

bool Config::getTouchEnable(bool tmpvalue){
	if(tmpvalue){
		return tmptouchenable;
	}
	return touchenable; 
}
void Config::setTouchEnable(bool val){
	tmptouchenable = val;
}

int Config::getPlayerSwipeSeek(bool tmpvalue){
	if(tmpvalue){
		return tmpplayeswipeseek;
	}
	return playeswipeseek; 
}

void Config::setPlayerSwipeSeek(int val){
	tmpplayeswipeseek = val;
}

int Config::getLongSeek(bool tmpvalue){
	if(tmpvalue){
		return tmplongseek;
	}
	return longseek; 
}

int Config::getShortSeek(bool tmpvalue){
	if(tmpvalue){
		return tmpshortseek;
	}
	return shortseek;
}

void Config::setLongSeek(int seektime){
	tmplongseek = seektime;
}
void Config::setShortSeek(int seektime){
	tmpshortseek = seektime;
}

bool Config::getUseAlang(bool tmpvalue){
	if(tmpvalue){
		return tmpusealang;
	}
	return usealang;
}
void Config::setUseAlang(bool _val){
	tmpusealang = _val;
}

int Config::getAlang(bool tmpvalue){
	if(tmpvalue){
		return tmpalang;
	}
	return alang; 
}

void Config::setAlang(int lang){
	tmpalang = lang;
}

//Slang
bool Config::getUseSlang(bool tmpvalue){
	if(tmpvalue){
		return tmpuseslang;
	}
	return useslang;
}
void Config::setUseSlang(bool _val){
	tmpuseslang = _val;
}

int Config::getSlang(bool tmpvalue){
	if(tmpvalue){
		return tmpslang;
	}
	return slang; 
}

void Config::setSlang(int lang){
	tmpslang = lang;
}
//end Slang


int Config::getSubFontSize(bool tmpvalue){
	if(tmpvalue){
		return tmpsubfontsize;
	}
	return subfontsize; 
}

void Config::setSubFontSize(int val){
	tmpsubfontsize = val;
}

float * Config::getSubFontColor(bool tmpvalue){
	if(tmpvalue){
		return tmpsubfontcolor;
	}
	return subfontcolor; 
}
std::string Config::getSubFontColorHex(bool tmpvalue){
	char subfontcstr[32];
	if(tmpvalue){
		sprintf(subfontcstr,"#%02X%02X%02X",(unsigned int)(tmpsubfontcolor[0]*255.0f),(unsigned int)(tmpsubfontcolor[1]*255.0f),(unsigned int)(tmpsubfontcolor[2]*255.0f));
		return std::string(subfontcstr);
	}
	sprintf(subfontcstr,"#%02X%02X%02X",(unsigned int)(subfontcolor[0]*255.0f),(unsigned int)(subfontcolor[1]*255.0f),(unsigned int)(subfontcolor[2]*255.0f));
	return std::string(subfontcstr);	
}
void Config::setSubFontColor(float  *_color){
	tmpsubfontcolor[0] = _color[0];
	tmpsubfontcolor[1] = _color[1];
	tmpsubfontcolor[2] = _color[2];
}

//bordercolor
float * Config::getSubBorderColor(bool tmpvalue){
	if(tmpvalue){
		return tmpsubbordercolor;
	}
	return subbordercolor; 
}
std::string Config::getSubBorderColorHex(bool tmpvalue){
	char subfontcstr2[32];
	if(tmpvalue){
		sprintf(subfontcstr2,"#%02X%02X%02X",(unsigned int)(tmpsubbordercolor[0]*255.0f),(unsigned int)(tmpsubbordercolor[1]*255.0f),(unsigned int)(tmpsubbordercolor[2]*255.0f));
		return std::string(subfontcstr2);
	}
	sprintf(subfontcstr2,"#%02X%02X%02X",(unsigned int)(subbordercolor[0]*255.0f),(unsigned int)(subbordercolor[1]*255.0f),(unsigned int)(subbordercolor[2]*255.0f));
	return std::string(subfontcstr2);	
}
void Config::setSubBorderColor(float  *_color){
	tmpsubbordercolor[0] = _color[0];
	tmpsubbordercolor[1] = _color[1];
	tmpsubbordercolor[2] = _color[2];
}
//endbordercolor

void Config::setDeinterlace(int value){
	tmpdeint = value;
}

int Config::getDeinterlace(bool tmpvalue){
	if(tmpvalue){
		return tmpdeint;
	}
	return deint; 
}


bool Config::getDbActive(bool tmpvalue){
	if(tmpvalue){
		return tmpdbactive;
	}
	return dbactive; 
}

void Config::setDbActive(bool value){
	tmpdbactive = value;
}

int Config::getResumeStartPerc(bool tmpvalue){
	if(tmpvalue){
		return tmpstartresumeperc;
	}
	return startresumeperc;
}
void Config::setResumeStartPerc(int value){
	tmpstartresumeperc = value;
}

int Config::getResumeStopPerc(bool tmpvalue){
	if(tmpvalue){
		return tmpstopresumeperc;
	}
	return stopresumeperc;
}
void Config::setResumeStopPerc(int value){
	tmpstopresumeperc = value;
}

void Config::saveSettings(){
	
	
	showhidden = tmpshowhidden;
	touchenable = tmptouchenable;
	playeswipeseek = tmpplayeswipeseek;
	longseek = tmplongseek;
	shortseek = tmpshortseek;
	usealang = tmpusealang;
	//slang
	useslang = tmpuseslang;
	//end slang
	subfontsize = tmpsubfontsize;
	deint = tmpdeint;
	dbactive = tmpdbactive;
	
	startresumeperc = tmpstartresumeperc;
	stopresumeperc = tmpstopresumeperc;
	
	subfontcolor[0] = tmpsubfontcolor[0];
	subfontcolor[1] = tmpsubfontcolor[1];
	subfontcolor[2] = tmpsubfontcolor[2];
	
	//bordercolor
	subbordercolor[0] = tmpsubbordercolor[0];
	subbordercolor[1] = tmpsubbordercolor[1];
	subbordercolor[2] = tmpsubbordercolor[2];
	//endbordercolor

	themename = tmpthemename;
	
	
	ini->Delete("Main", "showhidden");
	ini->SetBoolValue("Main", "showhidden",showhidden,NULL,false);
	
	
	ini->Delete("Main", "touchenable");
	ini->SetBoolValue("Main", "touchenable",touchenable,NULL,false);
	
	ini->Delete("Main", "playeswipeseek");
	ini->SetLongValue("Main", "playeswipeseek",playeswipeseek,NULL,false);
	
	ini->Delete("Main", "shortseek");
	ini->SetLongValue("Main", "shortseek", shortseek, NULL, false);
	ini->Delete("Main", "longseek");
	ini->SetLongValue("Main", "longseek", longseek, NULL, false);
	
	ini->Delete("Main", "usealang");
	ini->SetBoolValue("Main", "usealang", usealang, NULL, false);
	
	ini->Delete("Main", "alang");
	ini->SetValue("Main", "alang", Utility::getLanguages()[tmpalang].lang3.c_str());
	
	//slang
	ini->Delete("Main", "useslang");
	ini->SetBoolValue("Main", "useslang", useslang, NULL, false);
	
	ini->Delete("Main", "slang");
	ini->SetValue("Main", "slang", Utility::getLanguages()[tmpslang].lang3.c_str());
	//end slang

	ini->Delete("Main", "subfontsize");
	ini->SetLongValue("Main", "subfontsize", subfontsize, NULL, false);
	
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
	ini->SetValue("Main", "deinterlace", deintopts[deint].c_str());
	std::vector<std::string> usedbintopts = {"no","yes"};
	ini->Delete("Main", "usedb");
	ini->SetValue("Main", "usedb", usedbintopts[dbactive].c_str());
	
	ini->Delete("Main", "startresumeperc");
	ini->SetLongValue("Main", "startresumeperc", startresumeperc, NULL, false);
	ini->Delete("Main", "stopresumeperc");
	ini->SetLongValue("Main", "stopresumeperc", stopresumeperc, NULL, false);
	
	ini->Delete("Main", "theme");
	ini->SetValue("Main", "theme", themename.c_str());
	
	std::string data;
	ini->Save(data);
	ini->SaveFile(inifilePath.c_str());
}

std::string Config::getThemeName(bool tmpvalue){
	if(tmpvalue){
		return tmpthemename;
	}
	return themename;
}

void Config::setThemeName(std::string value){
	tmpthemename = value;
}

