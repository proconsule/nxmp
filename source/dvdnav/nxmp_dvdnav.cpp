
#include "nxmp_dvdnav.h"
#include <dvdnav/dvdnav.h>





CDVDNav::CDVDNav(std::string _path){

	

}


int CDVDNav::ParseDVDISO(std::string _isofilepath){
	
	const char* dvd_path = _isofilepath.c_str();

    dvdnav_t* dvdnav;
    

    if (dvdnav_open(&dvdnav, dvd_path) < 0) {
        std::cerr << "Errore nell'apertura del DVD con libdvdnav." << std::endl;
        return 1;
    }

    std::vector<TitleInfo> titles_info;

    int total_titles = 0;
    if (dvdnav_get_number_of_titles(dvdnav, &total_titles) == DVDNAV_STATUS_OK) {
        std::cout << "Trovati " << total_titles << " titoli.\n";

        for (int i = 1; i <= total_titles; ++i) {
            TitleInfo current_title;
            current_title.title_number = i;
            current_title.title_name = "Nome non disponibile (libdvdnav)"; 
			
			 for (int j = 0; j < 8; ++j) {
				audio_attr_t audioAttr;
				if (dvdnav_get_audio_attr(dvdnav, j, &audioAttr) == DVDNAV_STATUS_OK) {
					std::cout << "  Language Code: 0x" << std::hex << std::setw(4) << std::setfill('0')
						  << audioAttr.lang_code << std::dec << std::endl;
				}
			}
            for (int j = 0; j < 8; ++j) {
				
                uint16_t lang_code = dvdnav_audio_stream_to_lang(dvdnav, j);
				if (lang_code != 0xffff) {
                    current_title.audio_languages.push_back(lang_code);
                }
            }

            for (int j = 0; j < 32; ++j) {
                uint16_t lang_code = dvdnav_spu_stream_to_lang(dvdnav, j);
                if (lang_code != 0xffff) {
                    current_title.subtitle_languages.push_back(lang_code);
                }
            }

            uint64_t* chapter_times = nullptr;
            uint64_t duration = 0;
            if (dvdnav_describe_title_chapters(dvdnav, i, &chapter_times, &duration) == 0) {
                current_title.duration_in_seconds = duration / 90000;
                if (chapter_times) {
                    free(chapter_times);
                }
            } else {
                current_title.duration_in_seconds = 0;
            }

            titles_info.push_back(current_title);
        }

       
	    for (const auto& title : titles_info) {
            std::cout << "Titolo " << std::setw(2) << title.title_number << ":\n";
            std::cout << "  Nome:   " << title.title_name << "\n";
            std::cout << "  Durata: " << title.duration_in_seconds << " secondi\n";

            std::cout << "  Audio:  ";
            if (title.audio_languages.empty()) {
                std::cout << "Nessuna\n";
            } else {
                for (uint16_t lang_code : title.audio_languages) {
                    std::cout << languageCodeToString(lang_code) << " ";
                }
                std::cout << "\n";
            }

            std::cout << "  Sottotitoli: ";
            if (title.subtitle_languages.empty()) {
                std::cout << "Nessuno\n";
            } else {
                for (uint16_t lang_code : title.subtitle_languages) {
                    std::cout << languageCodeToString(lang_code) << " ";
                }
                std::cout << "\n";
            }
            std::cout << "---\n";
        }

    } else {
        std::cerr << "Errore nell'ottenere il numero di titoli." << std::endl;
    }

    dvdnav_close(dvdnav);
   
    return 0;

}
