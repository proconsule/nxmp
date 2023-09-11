#include "mediaprobe.h"



void ProbeThread(void*arg) {
	
	CMediaProbe *ctx = (CMediaProbe *)arg;
	
	
	while(!ctx->exitThread){
		for(int i=0;i<ctx->playlist->currplaylist.size();i++){
			
			
			
			if(!ctx->playlist->currplaylist[i].is_valid){
				
				std::string mycheckfile = ctx->playlist->currplaylist[i].fulluri;
				if(Utility::startWith(mycheckfile,"ums",false)){
					mycheckfile = std::string("file:") + mycheckfile;
				}
				
				AVFormatContext* pFormatCtx = avformat_alloc_context();
				int rc = avformat_open_input(&pFormatCtx, mycheckfile.c_str(), NULL, NULL);
				if(rc != 0){
					char err[1024] = { 0 };
					av_strerror(rc, err, 1024);
					//printf("RC %s\r\n",err);
					ctx->playlist->currplaylist[i].is_valid = true;
					continue;
				}
				if(avformat_find_stream_info(pFormatCtx, NULL)<0){
					ctx->playlist->currplaylist[i].is_valid = true;
					continue;
					
				}
				ctx->playlist->currplaylist[i].duration = pFormatCtx->duration/AV_TIME_BASE;
				ctx->playlist->currplaylist[i].is_valid = true;
				ctx->playlist->CalcPlaylist();
				avformat_close_input(&pFormatCtx);
				avformat_free_context(pFormatCtx);
				svcSleepThread(100'000'000);
				
			}
			
		}
		svcSleepThread(1'000'000'000);
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

CMediaProbe::~CMediaProbe(){
	
	exitThread=true;
	threadWaitForExit(&t0);
	threadClose(&t0);
}
	


