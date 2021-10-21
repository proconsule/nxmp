#ifndef NXMP_MPV_H
#define NXMP_MPV_H

#include <cstdio>
#include <string>

#include <mpv/client.h>
#include <mpv/render_gl.h>

class Mpv {

public:

    enum class LoadType {
        Replace,
        Append,
        AppendPlay
    };

    explicit Mpv(const std::string &configPath, bool initRender);

    ~Mpv();

    int load(const std::string &file, LoadType loadType, const std::string &options);

    int pause();

    int resume();

    int stop();

    int seek(double position);
	
	int seekOSD(double position);

    int setSpeed(double speed);

    double getSpeed();

    int setVid(int id);

    int setAid(int id);

    int setSid(int id);

    int getVideoBitrate();

    int getAudioBitrate();

    long getDuration();

    long getPosition();
	
	int getPercentPos();

    bool isStopped();

    bool isPaused();

    bool isAvailable();

    mpv_event *getEvent();

    mpv_handle *getHandle();

    mpv_render_context *getContext();

//    MediaInfo getMediaInfo(const c2d::Io::File &file);

private:

    mpv_handle *handle = nullptr;
    mpv_render_context *context = nullptr;

};

#endif //PPLAY_MPV_H