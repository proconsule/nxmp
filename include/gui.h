#ifndef NXMP_GUI_H
#define NXMP_GUI_H

#include <vector>
#include <string>
#include <SDL.h>
#ifdef __SWITCH__
#include <switch.h>
#endif
#include <glad/glad.h>
#include "appwindows.h"
#include "apppopups.h"
#include "playerwindows.h"
#include "libmpv.h"
#include "config.h"
#include "remotefs.h"
#include "localfiles.h"
#include "SimpleIni.h"
#include "remotefs.h"
#include "localFs.h"
#include "utils.h"
#include "usbfs.h"
#include "Enigma2.h"
#include "HTTPDir.h"
#include "FTPDir.h"


enum MENU_STATES {
	MENU_STATE_HOME,
    MENU_STATE_FILEBROWSER,
	MENU_STATE_USB,
	MENU_STATE_NETWORKBROWSER,
	MENU_STATE_FTPBROWSER,
	MENU_STATE_HTTPBROWSER,
	MENU_STATE_ENIGMABROWSER,
	MENU_STATE_INFO,
//    MENU_STATE_OPTIONS,
	MENU_STATE_SETTINGS,
	MENU_STATE_PLAYER,
	MENU_STATE_PLAYERCACHING
};

enum PLAYER_STATES {
	PLAYER_STATE_VIDEO,
	PLAYER_STATE_AUDIO
};

enum APP_POPUP_STATES {
	POPUP_STATE_NONE,
	POPUP_STATE_SAVE_SETTINGS
};

enum PLAYER_RIGHT_MENU_STATES {
	PLAYER_RIGHT_MENU_PLAYER,
	PLAYER_RIGHT_MENU_HOME,
	PLAYER_RIGHT_MENU_TRACKS,
	PLAYER_RIGHT_MENU_TRACKS_VIDEO,
	PLAYER_RIGHT_MENU_TRACKS_AUDIO,
	PLAYER_RIGHT_MENU_TRACKS_SUB,
	PLAYER_RIGHT_MENU_CHAPTERS,
	PLAYER_RIGHT_MENU_ARATIO,
	PLAYER_RIGHT_MENU_CUSTOMARATIO,
	PLAYER_RIGHT_MENU_IMAGE,
	PLAYER_RIGHT_MENU_AUDIO,
	PLAYER_RIGHT_MENU_SUB,
	PLAYER_AUDIOEQ,
	PLAYER_SUPERAUDIOEQ
};


typedef struct {
    MENU_STATES state = MENU_STATE_HOME;
	MENU_STATES laststate = MENU_STATE_FILEBROWSER;
	PLAYER_RIGHT_MENU_STATES rightmenustate = PLAYER_RIGHT_MENU_PLAYER;
	PLAYER_STATES playerstate = PLAYER_STATE_VIDEO;
	APP_POPUP_STATES popupstate = POPUP_STATE_NONE;
	int selected = 0;
	std::string usbpath = "";
	std::string usbbasepath = "";
	std::vector<FS::FileEntry> usbfileentries;
	
	std::vector<networkSource> networksources;
	
	
	bool networkselect = true;
	int playershowcontrols = false;
	int playershowstats = false;
	bool masterlock = false;
	
	bool first_item;
	bool focus;
	
	bool rightmenu_first_item;
	bool rightmenu_focus;
	
    
} MenuItem;


extern SDL_Window *window;
extern MenuItem item;

extern libMpv *libmpv;
extern localFs *localdir;
extern HTTPDir *httpdir;
extern FTPDir *ftpdir;
#ifdef __SWITCH__
extern USBMounter *usbmounter;
#endif
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
extern Tex UsbTexture;
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

extern ImFont* fontSmall;


namespace GUI {
	
	void initMpv();
	void HandleEvents();
	void HandleLayers();
	void HandleRender();
    int RenderLoop(void);
	
}



#endif