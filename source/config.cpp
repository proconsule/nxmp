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
	ini = new CSimpleIniA(true,true);
	ini->SetUnicode();
	ini->LoadFile(inifile.c_str());
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

std::string Config::getEnigma(){
	const char* pv;
	pv = ini->GetValue("Enigma2", "e2address");
	if(pv==nullptr){
		return "";
	}
	return pv;
	
}

