#ifndef NXMP_LIBMPVMPV_H
#define NXMP_LIBMPVMPV_H

#include <string>

#include <mpv/client.h>
#include <mpv/render_gl.h>

class libMpv{
public:

	libMpv(const std::string &configDir);
	
	void Pause();

	void Resume();

	void Stop();
	
	void seekSilent(double position);
	
	void seekOSD(double position);
	
	bool Stopped();
	
	bool Paused();
	
	int64_t getPosition();
	
	mpv_handle *getHandle();

	mpv_render_context *getContext();
	
	std::string mpv_version;
	std::string ffmpeg_version;

private:

    mpv_handle *handle = nullptr;
    mpv_render_context *context = nullptr;
	
};

#endif