#ifndef NXMP_GUI_H
#define NXMP_GUI_H

#include <vector>
#include <string>
#include <SDL.h>
#include "platforms.h"
#ifdef __SWITCH__
#include <switch.h>
#endif
#include <glad/glad.h>
#include "appwindows.h"
#include "apppopups.h"
#include "playerwindows.h"
#include "libmpv.h"
#include "config.h"
#include "playlist.h"
#include "remotefs.h"
#include "localfiles.h"
#include "SimpleIni.h"
#include "eqpreset.h"
#include "SQLiteDB.h"
#include "remotefs.h"
#include "localFs.h"
#include "utils.h"
#ifdef NXMP_USBSUPPORT
#include "usbfs.h"
#endif
#ifdef NXMP_ENIGMASUPPORT
#include "Enigma2.h"
#endif
#ifdef NXMP_NETWORKSUPPORT
#include "HTTPDir.h"
#include "FTPDir.h"
#include "sshDir.h"
#include "sambaDir.h"
#include "nfsDir.h"
#endif
#ifdef NXMP_UPNPSUPPORT
#include "NX-UPNP.h"
#endif

#include "imgui_impl_sdl.h"

#include "touchcontrols.h"
#include "shaderMania.h"
#include "themes.h"

#define handheldWidth 1280
#define handheldHeight 720
#define dockedWidth 1920
#define dockedHeight 1080

enum MENU_STATES {
	MENU_STATE_HOME,
    MENU_STATE_FILEBROWSER,
	MENU_STATE_USB,
	MENU_STATE_NETWORKBROWSER,
	MENU_STATE_FTPBROWSER,
	MENU_STATE_HTTPBROWSER,
	MENU_STATE_SSHBROWSER,
	MENU_STATE_SAMBABROWSER,
	MENU_STATE_NFSBROWSER,
	MENU_STATE_UPNPBROWSER,
	MENU_STATE_ENIGMABROWSER,
	MENU_STATE_PLAYLISTBROWSER,
	MENU_STATE_INFO,
	MENU_STATE_SETTINGS,
	MENU_STATE_PLAYER,
	MENU_STATE_PLAYERCACHING
};

enum FILE_SELECTION_STATES {
	FILE_SELECTION_NONE,
	FILE_SELECTION_CHECKBOX
};

enum PLAYER_STATES {
	PLAYER_STATE_VIDEO,
	PLAYER_STATE_AUDIO
};

enum APP_POPUP_STATES {
	POPUP_STATE_NONE,
	POPUP_STATE_SAVE_SETTINGS,
	POPUP_STATE_RESUME,
	POPUP_STATE_ADDQUEUE,
	POPUP_STATE_STARTPLAYLIST,
	POPUP_STATE_SUBFONTCOLOR,
	POPUP_STATE_SUBBORDERCOLOR,
	POPUP_STATE_DBUPDATED
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
	PLAYER_RIGHT_MENU_SHADERMANIA,
	PLAYER_RIGHT_MENU_ANIME4K,
	PLAYER_AUDIOEQ,
	PLAYER_SUPERAUDIOEQ
};

enum PLAYER_CONTROL_STATES {
	PLAYER_CONTROL_STATE_NONE,
	PLAYER_CONTROL_STATE_CONTROLS
};


typedef struct {
    MENU_STATES state = MENU_STATE_HOME;
	MENU_STATES laststate = MENU_STATE_FILEBROWSER;
	FILE_SELECTION_STATES selectionstate = FILE_SELECTION_NONE;
	PLAYER_RIGHT_MENU_STATES rightmenustate = PLAYER_RIGHT_MENU_PLAYER;
	PLAYER_STATES playerstate = PLAYER_STATE_VIDEO;
	PLAYER_CONTROL_STATES playercontrolstate = PLAYER_CONTROL_STATE_NONE;
	APP_POPUP_STATES popupstate = POPUP_STATE_NONE;
	int selected = 0;
	//std::string usbpath = "";
	//std::string usbbasepath = "";
	std::vector<FS::FileEntry> usbfileentries;
	
	std::vector<networkSource> networksources;
	
	float rightmenu_startpos = 1280.0;
	
	bool networkselect = true;
	int playershowcontrols = false;
	int playershowstats = false;
	bool masterlock = false;
	bool clockoc = false;
	bool first_item;
	bool focus;
	int fileHoveredidx = 0;
	
	
	bool rightmenu_first_item;
	bool rightmenu_focus;
	
	bool showVolume = false;
	float VolumeHide = 0.0;
	
	int playlistitemHighlighted = 0;
	bool playlistUpdateHovered = false;
	int playlistnewHoverIdx = 0;
    
} MenuItem;

typedef struct {
	Tex SdCardTexture;
	Tex UsbTexture;
	Tex NetworkTexture;
	Tex Enigma2Texture;
	Tex PlaylistTexture;
	Tex InfoTexture;
	Tex SettingsTexture;

	Tex FolderTexture;
	Tex FileTexture;


	Tex FTPTexture;
	Tex HTTPTexture;
	Tex SFTPTexture;
	Tex SMBTexture;
	Tex NFSTexture;
	Tex UPNPTexture;

	Tex FFMPEGTexture;
	Tex MPVTexture;

	Tex NXMPBannerTexture;
	Tex ExitTexture;


	Tex PlayIcon;
	Tex PauseIcon;
	Tex StopIcon;
	Tex MuteIcon;
	Tex VolumeIcon;
	Tex LoopIcon;
	Tex NoLoopIcon;
		
} SysIcons;



extern SDL_Window *window;
#ifdef _WIN32
extern bool fullscreen;
#endif
extern MenuItem item;

extern libMpv *libmpv;
extern localFs *localdir;
#ifdef NXMP_NETWORKSUPPORT
extern HTTPDir *httpdir;
extern FTPDir *ftpdir;
extern sshDir *sshdir;
extern sambaDir *sambadir;
extern nfsDir *nfsdir;
#endif
#ifdef NXMP_UPNPSUPPORT
extern NXUPnP *nxupnp;
#endif
#ifdef NXMP_USBSUPPORT
extern USBMounter *usbmounter;
#endif
#ifdef NXMP_ENIGMASUPPORT
extern Enigma2 *enigma2;
#endif

extern uint32_t wakeup_on_mpv_render_update;
extern uint32_t wakeup_on_mpv_events;

extern mpv_opengl_fbo fbo;
extern mpv_render_param params[3];
extern int __fbo_one;

extern bool renderloopdone;

extern std::string nxmpTitle;

extern Config *configini;
extern EQPreset *eqpreset;
extern SQLiteDB *sqlitedb;
extern bool dbUpdated;

extern Playlist *playlist;

extern SysIcons nxmpicons;

extern int newResW;
extern int newResH;
extern float multiplyRes;
extern bool isHandheld;
extern SDL_GLContext context;
/*
extern Tex SdCardTexture;
extern Tex UsbTexture;
extern Tex NetworkTexture;
extern Tex Enigma2Texture;
extern Tex PlaylistTexture;
extern Tex InfoTexture;
extern Tex SettingsTexture;

extern Tex FolderTexture;
extern Tex FileTexture;


extern Tex FTPTexture;
extern Tex HTTPTexture;
extern Tex SFTPTexture;
extern Tex SMBTexture;
extern Tex NFSTexture;
extern Tex UPNPTexture;

extern Tex FFMPEGTexture;
extern Tex MPVTexture;

extern Tex NXMPBannerTexture;
extern Tex ExitTexture;


extern Tex PlayIcon;
extern Tex PauseIcon;
extern Tex StopIcon;
extern Tex MuteIcon;
extern Tex VolumeIcon;
extern Tex LoopIcon;
extern Tex NoLoopIcon;
*/


extern ImFont* fontSmall;

extern shaderMania* shadermania;



/* theme porp */
extern bool dochangethemefont;
extern std::string themefontpath;
extern float themefontsize;
extern float themefontsmall;



#ifdef _WIN32
extern bool isMouseSelection;
extern int startMousex;
extern int startMousey;
#endif

/*
extern float swipex;
extern float swipey;
extern float fingersum;
*/

namespace GUI {
	
	enum SDL_KEYS {
		SDL_KEY_A, SDL_KEY_B, SDL_KEY_X, SDL_KEY_Y,
		SDL_KEY_LSTICK, SDL_KEY_RSTICK,
		SDL_KEY_L, SDL_KEY_R,
		SDL_KEY_ZL, SDL_KEY_ZR,
		SDL_KEY_PLUS, SDL_KEY_MINUS,
		SDL_KEY_DLEFT, SDL_KEY_DUP, SDL_KEY_DRIGHT, SDL_KEY_DDOWN,
		SDL_KEY_LSTICK_LEFT, SDL_KEY_LSTICK_UP, SDL_KEY_LSTICK_RIGHT, SDL_KEY_LSTICK_DOWN,
		SDL_KEY_RSTICK_LEFT, SDL_KEY_RSTICK_UP, SDL_KEY_RSTICK_RIGHT, SDL_KEY_RSTICK_DOWN,
		SDL_KEY_SL_LEFT, SDL_KEY_SR_LEFT, SDL_KEY_SL_RIGHT, SDL_KEY_SR_RIGHT
	};
	
	void initMpv();
	void HandleEvents();
	void HandleLayers();
	void HandleRender();
    int RenderLoop(void);
	
	void changeFontTheme();
	
	void reinit();
	
}



#endif