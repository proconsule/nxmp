#include "config.h"
#include <regex>
#include <string>
#include <vector>

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
	tmplongseek = longseek;
	tmpshortseek = shortseek;
	
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

void Config::saveSettings(){
	longseek = tmplongseek;
	shortseek = tmpshortseek;
	ini->Delete("Main", "shortseek");
	ini->SetLongValue("Main", "shortseek", shortseek, NULL, false);
	ini->Delete("Main", "longseek");
	ini->SetLongValue("Main", "longseek", longseek, NULL, false);
	std::string data;
	int rc = ini->Save(data);
	rc = ini->SaveFile(inifilePath.c_str());
}

