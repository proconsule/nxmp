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
	
	
	topmenu.push_back("Local Files");
	topmenu.push_back("USB");
	
	ini->GetAllValues("Network", "source", values);
	if(getNetworks().size() != 0){
		topmenu.push_back("Network");
	}
	
	if(getEnigma() != ""){
		topmenu.push_back("Enigma2");
	}
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

void Config::setDeinterlace(int value){
	tmpdeint = value;
}

int Config::getDeinterlace(bool tmpvalue){
	if(tmpvalue){
		return tmpdeint;
	}
	return deint; 
}

void Config::saveSettings(){
	longseek = tmplongseek;
	shortseek = tmpshortseek;
	usealang = tmpusealang;
	deint = tmpdeint;
	
	ini->Delete("Main", "shortseek");
	ini->SetLongValue("Main", "shortseek", shortseek, NULL, false);
	ini->Delete("Main", "longseek");
	ini->SetLongValue("Main", "longseek", longseek, NULL, false);
	
	ini->Delete("Main", "usealang");
	ini->SetBoolValue("Main", "usealang", usealang, NULL, false);
	
	ini->Delete("Main", "alang");
	ini->SetValue("Main", "alang", Utility::getLanguages()[tmpalang].lang3.c_str());
	std::vector<std::string> deintopts = {"no","yes","auto"};
	ini->Delete("Main", "deinterlace");
	ini->SetValue("Main", "deinterlace", deintopts[deint].c_str());
	std::string data;
	ini->Save(data);
	ini->SaveFile(inifilePath.c_str());
}

