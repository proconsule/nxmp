#ifndef NXMP_MEDIAPROBE_H
#define NXMP_MEDIAPROBE_H

#include <string>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include "logger.h"
#include "playlist.h"


class CMediaProbe{
public:
		CMediaProbe(Playlist *_playlist);
		~CMediaProbe();
		bool needUpdate();
		void SetProbeList(std::vector<Playlist::playlist_struct> *_playlist);
		
		Playlist *playlist;
		
		bool exitThread = false;
		void StartMediaProbeThread();
		Thread t0;
		
		AVFormatContext *pFormatContext;
	
private:
	

};


#endif