#ifndef NXMP_GUI_H
#define NXMP_GUI_H

#include <vector>
#include <string>


#include <switch.h>


//#include <glad/glad.h>
#include "appwindows.h"
#include "apppopups.h"
#include "playerwindows.h"
#include "libmpv.h"

//#include "config.h"
#include "iniparser.h"
#include "imgloader.h"

#include "playlist.h"
#include "remotefs.h"
#include "networkShareClass.h"
#include "localfiles.h"
//#include "SimpleIni.h"
#include "eqpreset.h"
#include "SQLiteDB.h"
#include "remotefs.h"
#include "localFs.h"
#include "utils.h"
#include "mtpclass.h"
#include "stats.h"

#include "usbfs.h"




#include "Enigma2.h"
#include "mediaprobe.h"

#include "fileBrowserClass.h"


#include "HTTPDir.h"

#include "NX-UPNP.h"

#include "nxmp-render.h"

#include "touchcontrols.h"

#include "logger.h"

#define handheldWidth 1280
#define handheldHeight 720
#define dockedWidth 1920
#define dockedHeight 1080


#define FONT_A_BUTTON "\xEE\x80\x80"
#define FONT_B_BUTTON "\xEE\x80\x81"
#define FONT_X_BUTTON "\xEE\x80\x82"
#define FONT_Y_BUTTON "\xEE\x80\x83"

#define FONT_A_BUTTON_FILLED "\xEE\x83\xA0"
#define FONT_B_BUTTON_FILLED "\xEE\x83\xA1"
#define FONT_X_BUTTON_FILLED "\xEE\x83\xA2"
#define FONT_Y_BUTTON_FILLED "\xEE\x83\xA3"

#define FONT_ZL_BUTTON_FILLED "\xEE\x83\xA6"
#define FONT_ZR_BUTTON_FILLED "\xEE\x83\xA7"


#define FONT_CHECKED_ICON "\xEE\x85\x8B"

#define FONT_FILLED_CROSS "\xEE\x81\xB1"


#define FONT_DPADUP_BUTTON "\xEE\x82\xAF"
#define FONT_DPADDOWN_BUTTON "\xEE\x82\xB0"

#define FONT_DPADUP_BUTTON_FILLED "\xEE\x83\xAB"
#define FONT_DPADDOWN_BUTTON_FILLED "\xEE\x83\xAC"

#define FONT_DPADLEFT_BUTTON_FILLED "\xEE\x83\xAD"
#define FONT_DPADRIGHT_BUTTON_FILLED "\xEE\x83\xAE"

#define FONT_LOADING_ICON0 "\xEE\x80\xA0"
#define FONT_LOADING_ICON1 "\xEE\x80\xA1"
#define FONT_LOADING_ICON2 "\xEE\x80\xA2"
#define FONT_LOADING_ICON3 "\xEE\x80\xA3"
#define FONT_LOADING_ICON4 "\xEE\x80\xA4"
#define FONT_LOADING_ICON5 "\xEE\x80\xA5"
#define FONT_LOADING_ICON6 "\xEE\x80\xA6"
#define FONT_LOADING_ICON7 "\xEE\x80\xA7"

#define FONT_SPEAKER_ICON "\xEE\x84\xBC"

#define FONT_LEFTSTICK "\xEE\x83\x81"
#define FONT_RIGHTSTICK "\xEE\x83\x82"

enum MENU_STATES {
	MENU_STATE_HOME,
    MENU_STATE_FILEBROWSER,
	MENU_STATE_USB_MOUNT,
	MENU_STATE_USB_BROWSER,
	MENU_STATE_NETWORKBROWSER,
	MENU_STATE_ADDSHARE,
	MENU_STATE_FTPBROWSER,
	MENU_STATE_HTTPBROWSER,
	MENU_STATE_SSHBROWSER,
	MENU_STATE_SAMBABROWSER,
	MENU_STATE_NFSBROWSER,
	MENU_STATE_UPNPBROWSER,
	MENU_STATE_ENIGMABROWSER,
	MENU_STATE_PLAYLISTBROWSER,
	MENU_STATE_MTPSERVER,
	MENU_STATE_INFO,
	MENU_STATE_SETTINGS,
	MENU_STATE_PLAYER,
	MENU_STATE_PLAYERCACHING,
	MENU_STATE_APPEXIT,
	MENU_STATE_IMGVIEWER,
	MENU_STATE_PDFVIEWER,
	MENU_STATE_GUILESS
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
	POPUP_STATE_DBUPDATED,
	POPUP_STATE_NETWORKMENU,
	POPUP_STATE_FILECONTEXTMENU,
	POPUP_STATE_PLAYLISTCONTEXTMENU
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
	MENU_STATES savestate = MENU_STATE_FILEBROWSER;
	FILE_SELECTION_STATES selectionstate = FILE_SELECTION_NONE;
	PLAYER_RIGHT_MENU_STATES rightmenustate = PLAYER_RIGHT_MENU_PLAYER;
	PLAYER_STATES playerstate = PLAYER_STATE_VIDEO;
	PLAYER_CONTROL_STATES playercontrolstate = PLAYER_CONTROL_STATE_NONE;
	APP_POPUP_STATES popupstate = POPUP_STATE_NONE;
	bool showstats = false;
	bool showdecstats = false;
	int selected = 0;
	//std::string usbpath = "";
	//std::string usbbasepath = "";
	std::vector<FS::FileEntry> usbfileentries;
	
	std::vector<oldnetworkSource> networksources;
	
	float rightmenu_startpos = 1280.0;
	
	bool networkselect = true;
	int playershowcontrols = false;
	int playershowstats = false;
	bool masterlock = false;
	
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

#ifdef OPENGL_BACKEND
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

	Tex ShareAddTexture;
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
	
	Tex GUI_D_UP;
	Tex GUI_D_DOWN;
	Tex GUI_D_LEFT;
	Tex GUI_D_RIGHT;
	
	Tex GUI_A_BUT;
	Tex GUI_B_BUT;
	Tex GUI_X_BUT;
	Tex GUI_Y_BUT;
	
	
		
} SysIcons;

#endif

extern MenuItem item;

extern libMpv *libmpv;

extern CFileBrowser *filebrowser;


extern NXUPnP *nxupnp;

extern USBMounter *MyUSBMount;

extern Enigma2 *enigma2;

extern uint32_t wakeup_on_mpv_render_update;
extern uint32_t wakeup_on_mpv_events;


extern CMediaProbe *MediaProbe;


extern bool renderloopdone;

extern std::string nxmpTitle;

extern CIniParser *configini;
extern CImgLoader *imgloader;

extern EQPreset *eqpreset;
extern SQLiteDB *sqlitedb;
extern CMTP *mtp;

extern bool dbUpdated;

extern Playlist *playlist;


extern CStats *nxmpstats;


extern int newResW;
extern int newResH;
extern float multiplyRes;
extern int initSize;
extern int batteryPercent;
extern float initScale;
extern bool isHandheld;
extern bool clockoc;
extern std::string tempKbUrl;


extern CNetworkShare *NewNetworkShare;

extern NXMPRenderer *Renderer;

extern float currFontsize;

extern int64_t playercachesec;
extern int64_t playercachesize;



/* theme porp */
extern bool dochangethemefont;
extern std::string themefontpath;
extern float themefontsize;
extern float themefontsmall;


extern bool slaveplayer;
extern std::string slaveplayer_file;

extern unsigned int app_exit_mode;


/*
extern float swipex;
extern float swipey;
extern float fingersum;
*/


constexpr static float MasterVolumeMin     = 0.0f;
constexpr static float MasterVolumeMax     = 5.0f;
constexpr static float MasterVolumeDefault = 1.0f;
constexpr static float MasterVolumeExp     = 2.0f;
extern float CurrentVolume;


namespace GUI {
	
	extern bool GUI_REINIT;
	
	void GUILESS();
	void HandleEvents();
	void HandleLayers();
	void HandleRender();
    int RenderLoop(void);
	
	void changeFontTheme();
	
	void reinit();
	
	void NXMPImage(ImTextureID user_texture_id, const ImVec2& size);
	
	uint64_t Process_UI_Events(std::chrono::time_point<std::chrono::system_clock> myeventtime);
	
	
	
	
}



#endif