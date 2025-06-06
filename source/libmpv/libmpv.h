#ifndef NXMP_LIBMPVMPV_H
#define NXMP_LIBMPVMPV_H


#include <string>
#include <mpv/client.h>
#include <deko3d.hpp>


#ifdef DEKO3D_BACKEND
#include <mpv/render_dk3d.h>
#endif


#include "fileInfo.h"

#include "iniparser.h"
#include "logger.h"


struct decoderlist_struct{
	std::string codecname;
	std::string codecdesc;
};

extern CIniParser *configini;
extern bool clockoc;
extern float initScale;
extern int initSize;

class libMpv{
public:

	libMpv(const std::string &configDir);
	
	~libMpv();
	
	void loadFile(std::string _path);
	
	void loadDVD(std::string _path);
	
	void loadFileLive(std::string _path,std::string _changename = "");
	
	void Pause();

	void Resume();

	void Stop();
	
	void seek(double position,bool osd);
	
	bool Stopped();
	
	bool Paused();
	
	int64_t getPosition();
	int64_t getDuration();
	
	int getFileInfoPerc();
	
	std::string getHWDec();
	int64_t getVideoWidth();
	int64_t getVideoHeight();
	double getFPS();
	int64_t getVideoBitrate();
	double getAudioBitrate();
	
	std::string getVideoCodec();
	
	
	std::string getAudioCodec();
	std::string getAudioSampleRate();
	std::string getAudioChannels();
	
	fileInfo *getFileInfo();
	
	void setVid(int id, bool osd);
	void setAid(int id, bool osd);
	void setSid(int id, bool osd);
	
	void setAspectRatio(double ratio,bool osd);
	
	void setBrightness(int value, bool osd);
	void setContrast(int value, bool osd);
	void setSaturation(int value, bool osd);
	void setGamma(int value, bool osd);
	void setHue(int value, bool osd);
	
	void setRotate(int value, bool osd);
	
	void setVolume(int value,bool osd);
	int getVolume();
	
	bool getMute();
	void toggleMute();

	void setOcState();
	
	void setAudioDelay(double value,bool osd);
	
	void setAudioEQ(int *eqval,bool osd);
	void setAudioSuperEQ(float *eqval,bool osd);
	void setAudioSuperEQband(float eqval,int band,bool osd);
	
	void setSubDelay(double value,bool osd);
	void setSubPos(int value,bool osd);
	void setSubFontSize(int value,bool osd);
	void setSubScaleSize(double value,bool osd);
	void setSubBorderSize(int value,bool osd);
	void setSubBorderBlur(double value,bool osd);
	void setShadowIntensity(double value,bool osd);
	void setShadowOffset(int value,bool osd);
	void setSubFontColor(std::string hexcolor);
	void setSubBorderColor(std::string hexcolor);
	void setDeinterlace(int value);
	
	void setLoop(bool val);
	bool getLoop();
	
	void setAudioNormalize(bool val);
	bool getAudioNormalize();
	
	void getfileInfo();
	
	void setShader(std::string _filename);
	void clearShader();

	void setDemuxCache(int mycachesec);
	
	mpv_handle *getHandle();

	mpv_render_context *getContext();
	std::vector<decoderlist_struct> getDecoderList();
	
	std::string mpv_version;
	std::string ffmpeg_version;
	
private:


	void resetFileInfo();
	mpv_handle *handle = nullptr;
	mpv_render_context *context = nullptr;
	std::vector<decoderlist_struct> decoderlist;
	fileInfo * fileinfo = nullptr;
	
	bool loop = false;
	bool audionorm = false;
	int volume = 100;
	int tmpvolume = 100;
	
	int demuxcachesec = 20;
	
};

#endif
