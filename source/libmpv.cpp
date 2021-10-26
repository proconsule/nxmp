#include <SDL2/SDL_video.h>
#include "libmpv.h"

libMpv::libMpv(const std::string &configDir) {

    handle = mpv_create();
    if (!handle) {
        printf("Error Create Handle\n");
        return;
    }
	printf("MPV Handle Created\n");
    mpv_set_option_string(handle, "config", "yes");
    mpv_set_option_string(handle, "config-dir", configDir.c_str());
    mpv_set_option_string(handle, "terminal", "yes");
    mpv_set_option_string(handle, "msg-level", "all=errors");
    mpv_set_option_string(handle, "vd-lavc-threads", "4");
    mpv_set_option_string(handle, "vd-lavc-skiploopfilter", "all");
    mpv_set_option_string(handle, "audio-channels", "stereo");
    mpv_set_option_string(handle, "video-timing-offset", "0");
	mpv_set_option_string(handle, "osd-bar-align-y", "0.9");
	mpv_set_option_string(handle, "fbo-format", "rgba8");
	
	
	printf("MPV Handle initialize\n");
    int res = mpv_initialize(handle);
    if (res) {
        printf("libmpv_initialize: %s\n", mpv_error_string(res));
        mpv_terminate_destroy(handle);
        return;
    }
	
	printf("MPV Init Renderer\n");
	
	

	
	mpv_opengl_init_params mpv_gl_init_params = {
	.get_proc_address = [] (void *, const char *name) -> void * { return SDL_GL_GetProcAddress(name); },
	};
	mpv_render_param params[]{
		{MPV_RENDER_PARAM_API_TYPE, (void *) MPV_RENDER_API_TYPE_OPENGL},
		{MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &mpv_gl_init_params},
		{(mpv_render_param_type)0}
	};
	printf("MPV Init Context\n");
	if (mpv_render_context_create(&context, handle, params) < 0) {
		printf("error: mpv_render_context_create: %s\n", mpv_error_string(res));
		mpv_terminate_destroy(handle);
		handle = nullptr;
	}
	
	mpv_version = mpv_get_property_string(handle, "mpv-version");
	ffmpeg_version = mpv_get_property_string(handle, "ffmpeg-version");
	printf("MPV Init Completed\n");
	
}

int64_t libMpv::getPosition() {
	int64_t position = 0;
	mpv_get_property(handle, "playback-time", MPV_FORMAT_INT64, &position);
	return position;
}

void libMpv::Pause() {
	mpv_command_string(handle, "set pause yes");
}

void libMpv::Resume() {
	mpv_command_string(handle, "set pause no");
}

void libMpv::Stop() {
	const char *cmd[] = {"stop",  NULL};
	mpv_command_async(handle, 0, cmd);
}
	
void libMpv::seekSilent(double position) {
	std::string cmd = "no-osd seek " + std::to_string(position) + " absolute";
	mpv_command_string(handle, cmd.c_str());
}

void libMpv::seekOSD(double position) {
	std::string cmd = "osd-bar seek " + std::to_string(position) + " absolute";
	mpv_command_string(handle, cmd.c_str());
}

bool libMpv::Stopped(){
	int ret = 1;
	mpv_get_property(handle, "playback-abort", MPV_FORMAT_FLAG, &ret);
	return ret == 1;
}

bool libMpv::Paused(){
	int ret = -1;
	mpv_get_property(handle, "pause", MPV_FORMAT_FLAG, &ret);
	return ret == 1;
}

mpv_handle *libMpv::getHandle() {
    return handle;
}

mpv_render_context *libMpv::getContext() {
    return context;
}

libMpv::~libMpv(){
	if (context) {
        mpv_render_context_free(context);
    }
    if (handle) {
        mpv_terminate_destroy(handle);
    }
}
