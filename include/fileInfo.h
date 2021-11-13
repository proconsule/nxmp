#ifndef NXMP_FILEINFO_H
#define NXMP_FILEINFO_H

#include <string>
#include <vector>

class fileInfo {

public:

    class Track {
    public:
        int id;
        std::string type;
        std::string title = "Unknown";
        std::string language = "N/A";
        std::string codec;
        int channels;
        int bit_rate;
        int sample_rate;
        int width;
        int height;
		bool albumart = false;
		bool selected = false;
    };
	
	class Chapter{
	public:
		std::string title;
		double time;
	};

    class Playback {
    public:
        int vid_id = -1;
        int aud_id = -1;
        int sub_id = -1;
        int position = 0;
		int duration = 0;
		std::string title;
		std::string artist;
		double getplayPerc(){
			return (double)position/(double)duration;
		}
		bool islive = false;
    };

    
    std::string title = "Unknown";
    std::string path;
    long duration = 0;
	long resume = 0;
    int bit_rate = 0;
    std::vector<Track> videos;
    std::vector<Track> audios;
    std::vector<Track> subtitles;
	
	std::vector<Chapter> chapters;
	
	Playback playbackInfo;
    

};

#endif