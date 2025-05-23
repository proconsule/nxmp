#ifndef NXMP_DVDNAV_H
#define NXMP_DVDNAV_H

#include <iostream>
#include <dvdnav/dvdnav.h>

#include <unistd.h>
#include <string>
#include <cstdlib>
#include <fcntl.h>

#include "utils.h"




class CDVDNav{
public:

struct TitleInfo {
    int title_number;
    std::string title_name;
    std::vector<uint16_t> audio_languages; // DVDLangID_t
    std::vector<uint16_t> subtitle_languages; // DVDLangID_t
    uint64_t duration_in_seconds;
};

// Funzione di utilità per convertire i codici lingua in stringhe (semplificata)
std::string languageCodeToString(uint16_t lang_code) {
    char code[3];
    code[0] = (lang_code >> 8) & 0xFF;
    code[1] = lang_code & 0xFF;
    code[2] = '\0';
    return std::string(code);
}

	CDVDNav(std::string _path);
	int ParseDVDISO(std::string _isofilepath);
	
	

private:
	int iso_fd;
	
	std::vector<TitleInfo> titlesInfo;
	
	
	
};



#endif