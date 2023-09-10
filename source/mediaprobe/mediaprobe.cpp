#include "mediaprobe.h"



void ProbeThread(void*arg) {
	
	CMediaProbe *ctx = (CMediaProbe *)arg;
	
	
	while(!ctx->exitThread){
		for(int i=0;i<ctx->playlist->currplaylist.size();i++){
			
			
			if(!ctx->playlist->currplaylist[i].is_valid){
				AVFormatContext* pFormatCtx = avformat_alloc_context();
				int rc = avformat_open_input(&pFormatCtx, ctx->playlist->currplaylist[i].fulluri.c_str(), NULL, NULL);
				if(rc != 0){
					printf("RC %d\r\n",rc);
					ctx->playlist->currplaylist[i].is_valid = true;
					continue;
				}
				if(avformat_find_stream_info(pFormatCtx, NULL)<0){
					ctx->playlist->currplaylist[i].is_valid = true;
					continue;
					
				}
				int64_t duration = pFormatCtx->duration;
				ctx->playlist->currplaylist[i].duration = pFormatCtx->duration/AV_TIME_BASE;
				ctx->playlist->currplaylist[i].is_valid = true;
				avformat_close_input(&pFormatCtx);
				avformat_free_context(pFormatCtx);
				
				
			}
			
		}
		usleep(1000000);
	}
}


CMediaProbe::CMediaProbe(Playlist *_playlist){
	
	playlist = _playlist;
	
	
	
}


void CMediaProbe::StartMediaProbeThread(){
	threadCreate(&t0, ProbeThread, this, NULL, 0x10000, 0x3B, -2);
	threadStart(&t0);
}

bool CMediaProbe::needUpdate(){
	bool refresh = false;
	for(int i=0;i<playlist->currplaylist.size();i++){
		if(!playlist->currplaylist[i].is_valid)refresh = true;
	}
	
	return refresh;
	
}


int64_t CMediaProbe::ProbeFileDuration(std::string _path){
	
}


CMediaProbe::~CMediaProbe(){
	
	exitThread=true;
	threadWaitForExit(&t0);
	threadClose(&t0);
}


