#ifndef NXMP_GUI_H
#define NXMP_GUI_H

#include <vector>
#include <string>
#include <SDL.h>
#include <switch.h>
#include <glad/glad.h>
#include "windows.h"
#include "libmpv.h"
#include "config.h"
#include "remotefs.h"
#include "SimpleIni.h"
#include "ftplib.h"
#include "remotefs.h"
#include "utils.h"
#include "Enigma2.h"
#include "HTTPDir.h"

enum MENU_STATES {
	MENU_STATE_HOME,
    MENU_STATE_FILEBROWSER,
	MENU_STATE_NETWORKBROWSER,
	MENU_STATE_ENIGMABROWSER,
	MENU_STATE_INFO,
    MENU_STATE_OPTIONS,
	MENU_STATE_SETTINGS,
	MENU_STATE_PLAYER
};


typedef struct {
    MENU_STATES state = MENU_STATE_HOME;
	MENU_STATES laststate = MENU_STATE_FILEBROWSER;
	int selected = 0;
	std::string localpath = "/switch/nxmp";
	std::string enigma2bouquet = "";
    std::vector<FsDirectoryEntry> localfileentries;
	std::vector<networkSource> networksources;
	std::string networkurl;
	std::string networklastpath;
	std::vector<remotefs_entry> networkentries;
	
	bool networkselect = true;
	int playershowcontrols = false;
	int playershowstats = false;
	bool masterlock = false;
	
	bool first_item;
	bool focus;
	
    
} MenuItem;


extern SDL_Window *window;
extern MenuItem item;

extern libMpv *libmpv;
extern HTTPDir *httpdir;
extern uint32_t wakeup_on_mpv_render_update;
extern uint32_t wakeup_on_mpv_events;

extern mpv_opengl_fbo fbo;
extern mpv_render_param params[3];
extern int __fbo_one;

extern bool renderloopdone;

extern std::string nxmpTitle;

extern Enigma2 *enigma2;

extern Config *configini;

extern Tex SdCardTexture;
extern Tex NetworkTexture;
extern Tex Enigma2Texture;
extern Tex InfoTexture;
extern Tex SettingsTexture;

extern Tex FolderTexture;
extern Tex FileTexture;

extern Tex FTPTexture;
extern Tex HTTPTexture;

extern Tex FFMPEGTexture;
extern Tex MPVTexture;

extern Tex NXMPBannerTexture;
extern Tex ExitTexture;



namespace GUI {
	
	void initMpv();
	void HandleEvents();
	void HandleLayers();
	void HandleRender();
    int RenderLoop(void);
	
}



#endif