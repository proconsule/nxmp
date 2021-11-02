#include <SDL2/SDL_video.h>
#include <iomanip>
#include <sstream>
#include "libmpv.h"
#include "utils.h"


bool codecSort(const decoderlist_struct &a, const decoderlist_struct &b) {

		return Utility::str_tolower(a.codecname) < Utility::str_tolower(b.codecname);
}

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
	mpv_set_option_string(handle, "volume-max", "200");
	
	if(configini->getUseAlang(false)){
		std::string alangstring = Utility::getLanguages()[configini->getAlang(false)].lang3 + std::string(",") + Utility::getLanguages()[configini->getAlang(false)].lang2 + std::string(",eng,en");
		mpv_set_option_string(handle, "alang", alangstring.c_str());
	}
#ifdef _WIN32
	mpv_set_option_string(handle, "hwdec", "auto-copy");
#endif
	
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
	
	mpv_node node;
    mpv_get_property(handle, "decoder-list", MPV_FORMAT_NODE, &node);
    if (node.format == MPV_FORMAT_NODE_ARRAY) {
        for (int i = 0; i < node.u.list->num; i++) {
            if (node.u.list->values[i].format == MPV_FORMAT_NODE_MAP) {
				decoderlist_struct decoderentry{};
				for (int n = 0; n < node.u.list->values[i].u.list->num; n++) {
					std::string key = node.u.list->values[i].u.list->keys[n];
					if (key == "codec") {
						decoderentry.codecname = node.u.list->values[i].u.list->values[n].u.string;
					}
					if (key == "description") {
						decoderentry.codecdesc = node.u.list->values[i].u.list->values[n].u.string;
					}
				}
				decoderlist.push_back(decoderentry);
				
			}
			
		}
		
	}
	
	std::sort(decoderlist.begin(),decoderlist.end(),codecSort);
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
	
void libMpv::seek(double position,bool osd) {
	if(osd){
		std::string cmd = "seek " + std::to_string(position) + " absolute";
		mpv_command_string(handle, cmd.c_str());
	}else{
		std::string cmd = "no-osd seek " + std::to_string(position) + " absolute";
		mpv_command_string(handle, cmd.c_str());
	}
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

std::vector<decoderlist_struct> libMpv::getDecoderList(){
	return decoderlist;
}

mpv_handle *libMpv::getHandle() {
    return handle;
}

mpv_render_context *libMpv::getContext() {
    return context;
}

fileInfo *libMpv::getFileInfo(){
	return fileinfo;
}

int64_t libMpv::getVideoWidth(){
	int64_t ret = 0;
	mpv_get_property(handle, "width", MPV_FORMAT_INT64, &ret);
	return ret;
}

int64_t libMpv::getVideoHeight(){
	int64_t ret = 0;
	mpv_get_property(handle, "height", MPV_FORMAT_INT64, &ret);
	return ret;
}

void libMpv::getfileInfo() {
	
	
	std::vector<fileInfo::Track> streams;
	mpv_node node;
    mpv_get_property(handle, "track-list", MPV_FORMAT_NODE, &node);
    if (node.format == MPV_FORMAT_NODE_ARRAY) {
        for (int i = 0; i < node.u.list->num; i++) {
            if (node.u.list->values[i].format == MPV_FORMAT_NODE_MAP) {
                fileInfo::Track stream{};
				for (int n = 0; n < node.u.list->values[i].u.list->num; n++) {
                    std::string key = node.u.list->values[i].u.list->keys[n];
                    if (key == "type") {
                        if (node.u.list->values[i].u.list->values[n].format == MPV_FORMAT_STRING) {
                            stream.type = node.u.list->values[i].u.list->values[n].u.string;
                        }
                    } else if (key == "id") {
                        if (node.u.list->values[i].u.list->values[n].format == MPV_FORMAT_INT64) {
                            stream.id = (int) node.u.list->values[i].u.list->values[n].u.int64;
                        }
                    } else if (key == "title") {
                        if (node.u.list->values[i].u.list->values[n].format == MPV_FORMAT_STRING) {
                            stream.title = node.u.list->values[i].u.list->values[n].u.string;
                        }
                    } else if (key == "lang") {
                        if (node.u.list->values[i].u.list->values[n].format == MPV_FORMAT_STRING) {
                            stream.language = node.u.list->values[i].u.list->values[n].u.string;
                        }
                    } else if (key == "codec") {
                        if (node.u.list->values[i].u.list->values[n].format == MPV_FORMAT_STRING) {
                            stream.codec = node.u.list->values[i].u.list->values[n].u.string;
                        }
                    } else if (key == "demux-w") {
                        if (node.u.list->values[i].u.list->values[n].format == MPV_FORMAT_INT64) {
                            stream.width = (int) node.u.list->values[i].u.list->values[n].u.int64;
                        }
                    } else if (key == "demux-h") {
                        if (node.u.list->values[i].u.list->values[n].format == MPV_FORMAT_INT64) {
                            stream.height = (int) node.u.list->values[i].u.list->values[n].u.int64;
                        }
                    } else if (key == "demux-samplerate") {
                        if (node.u.list->values[i].u.list->values[n].format == MPV_FORMAT_INT64) {
                            stream.sample_rate = (int) node.u.list->values[i].u.list->values[n].u.int64;
                        }
                    } else if (key == "demux-channel-count") {
                        if (node.u.list->values[i].u.list->values[n].format == MPV_FORMAT_INT64) {
                            stream.channels = (int) node.u.list->values[i].u.list->values[n].u.int64;
                        }
                    } else if (key == "selected") {
						stream.selected = (bool)node.u.list->values[i].u.list->values[n].u.flag;
					}
                }
                streams.push_back(stream);
            }
        }
    }
	
	std::vector<fileInfo::Chapter> chapters;
	mpv_node chapternode;
    mpv_get_property(handle, "chapter-list", MPV_FORMAT_NODE, &chapternode);
	if (chapternode.format == MPV_FORMAT_NODE_ARRAY) {
        for (int i = 0; i < chapternode.u.list->num; i++) {
            if (chapternode.u.list->values[i].format == MPV_FORMAT_NODE_MAP) {
				fileInfo::Chapter chapter{};
				for (int n = 0; n < chapternode.u.list->values[i].u.list->num; n++) {
					std::string key = chapternode.u.list->values[i].u.list->keys[n];
					if (key == "title") {
						if (chapternode.u.list->values[i].u.list->values[n].format == MPV_FORMAT_STRING) {
							chapter.title = chapternode.u.list->values[i].u.list->values[n].u.string;
						}
						
					}
					else if (key == "time") {
						if (chapternode.u.list->values[i].u.list->values[n].format == MPV_FORMAT_DOUBLE) {
							chapter.time = (double) chapternode.u.list->values[i].u.list->values[n].u.double_;
						}
						
					}
					
				}
				chapters.push_back(chapter);
				
			}
			
		}
		
	}
	
	
	
	if(fileinfo != nullptr){
		delete fileinfo;
	}
	fileinfo = new fileInfo();
	
	fileInfo::Track dummysubstream{};
	dummysubstream.id = -1;
	dummysubstream.title = "None";
	fileinfo->subtitles.push_back(dummysubstream);
	
	
	for (auto &stream: streams) {
        if (stream.type == "video") {
            fileinfo->videos.push_back(stream);
			if(stream.selected){
				fileinfo->playbackInfo.vid_id = stream.id;
			}
        } else if (stream.type == "audio") {
            fileinfo->audios.push_back(stream);
			if(stream.selected){
				fileinfo->playbackInfo.aud_id = stream.id;
			}
        } else if (stream.type == "sub") {
            fileinfo->subtitles.push_back(stream);
			if(stream.selected){
				fileinfo->playbackInfo.sub_id = stream.id;
			}
        }
    }
	
	for (auto &chapter: chapters) {
		fileinfo->chapters.push_back(chapter);
	}
	
	
}

void libMpv::setVid(int id, bool osd) {
    if (id > -1) {
        for(int i=0;i<fileinfo->videos.size();i++){
			if(fileinfo->videos[i].selected)fileinfo->videos[i].selected=false;
			if(fileinfo->videos[i].id == id)fileinfo->videos[i].selected=true;
		}
		if(osd){
			std::string cmd = "set vid " + std::to_string(id);
			mpv_command_string(handle, cmd.c_str());
		}else{
			std::string cmd = "no-osd set vid " + std::to_string(id);
			mpv_command_string(handle, cmd.c_str());
		}
    }
}

void libMpv::setAid(int id, bool osd) {
    if (id > -1) {
		for(int i=0;i<fileinfo->audios.size();i++){
			if(fileinfo->audios[i].selected)fileinfo->audios[i].selected=false;
			if(fileinfo->audios[i].id == id)fileinfo->audios[i].selected=true;
		}
		if(osd){
			std::string cmd = "set aid " + std::to_string(id);
			mpv_command_string(handle, cmd.c_str());
		}else{
			std::string cmd = "no-osd set aid " + std::to_string(id);
			mpv_command_string(handle, cmd.c_str());
		}
    }
}

void libMpv::setSid(int id, bool osd) {
    for(int i=0;i<fileinfo->subtitles.size();i++){
		if(fileinfo->subtitles[i].selected)fileinfo->subtitles[i].selected=false;
		if(fileinfo->subtitles[i].id == id)fileinfo->subtitles[i].selected=true;
	}
	if(id == -1){
		if(osd){
			std::string cmd = "set sid no";
			mpv_command_string(handle, cmd.c_str());
		}else{
			std::string cmd = "no-osd set sid no";
			mpv_command_string(handle, cmd.c_str());
		}
	}else{
		if(osd){
			std::string cmd = "set sid " + std::to_string(id);
			mpv_command_string(handle, cmd.c_str());
		}else{
			std::string cmd = "no-osd set sid " + std::to_string(id);
			mpv_command_string(handle, cmd.c_str());
		}
	}
}

void libMpv::setAspectRatio(double ratio,bool osd){
	mpv_set_property_async(handle, 0,"video-aspect-override", MPV_FORMAT_DOUBLE, &ratio);
}

void libMpv::setBrightness(int value,bool osd){
	if(osd){
		std::string cmd = "set brightness " + std::to_string(value);
		mpv_command_string(handle, cmd.c_str());
	}else{
		std::string cmd = "no-osd set brightness " + std::to_string(value);
		mpv_command_string(handle, cmd.c_str());
	}
}

void libMpv::setContrast(int value,bool osd){
	if(osd){
		std::string cmd = "set contrast " + std::to_string(value);
		mpv_command_string(handle, cmd.c_str());
	}else{
		std::string cmd = "no-osd set contrast " + std::to_string(value);
		mpv_command_string(handle, cmd.c_str());
	}
}

void libMpv::setSaturation(int value,bool osd){
	if(osd){
		std::string cmd = "set saturation " + std::to_string(value);
		mpv_command_string(handle, cmd.c_str());
	}else{
		std::string cmd = "no-osd set saturation " + std::to_string(value);
		mpv_command_string(handle, cmd.c_str());
	}
}

void libMpv::setGamma(int value,bool osd){
	if(osd){
		std::string cmd = "set gamma " + std::to_string(value);
		mpv_command_string(handle, cmd.c_str());
	}else{
		std::string cmd = "no-osd set gamma " + std::to_string(value);
		mpv_command_string(handle, cmd.c_str());
	}
}

void libMpv::setHue(int value,bool osd){
	if(osd){
		std::string cmd = "set hue " + std::to_string(value);
		mpv_command_string(handle, cmd.c_str());
	}else{
		std::string cmd = "no-osd set hue " + std::to_string(value);
		mpv_command_string(handle, cmd.c_str());
	}
}


void libMpv::setRotate(int value,bool osd){
	if(value == -1){
		std::string cmd = "set video-rotate no";
		mpv_command_string(handle, cmd.c_str());
	}else{
		std::string cmd = "no-osd set video-rotate " + std::to_string(value);
		mpv_command_string(handle, cmd.c_str());
	}
}


void libMpv::setVolume(int value,bool osd){
	if(osd){
		std::string cmd = "set volume " + std::to_string(value);
		mpv_command_string(handle, cmd.c_str());
	}else{
		std::string cmd = "no-osd set volume " + std::to_string(value);
		mpv_command_string(handle, cmd.c_str());
	}
}

void libMpv::setAudioDelay(double value, bool osd){
	mpv_set_property_async(handle, 0,"audio-delay", MPV_FORMAT_DOUBLE, &value);
}

libMpv::~libMpv(){
	if (context) {
        mpv_render_context_free(context);
    }
    if (handle) {
        mpv_terminate_destroy(handle);
    }
}
