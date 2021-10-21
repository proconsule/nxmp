#ifndef NXMP_GUI_H
#define NXMP_GUI_H

#include <vector>
#include <string>
#include <SDL.h>
#include <switch.h>
#include <glad/glad.h>
#include "windows.h"
#include "mpv.h"
#include "config.h"
#include "remotefs.h"
#include "SimpleIni.h"
#include "ftplib.h"
#include "remotefs.h"
#include "utils.h"
#include "Enigma2.h"

enum MENU_STATES {
    MENU_STATE_FILEBROWSER,
    MENU_STATE_OPTIONS,
	MENU_STATE_SETTINGS,
	MENU_STATE_PLAYER
};

enum BROWSER_STATES {
    BROWSER_STATE_HOME,
	BROWSER_STATE_LOCAL,
	BROWSER_STATE_NETWORK,
	BROWSER_STATE_ENIGMA2,
};

typedef struct {
    MENU_STATES state = MENU_STATE_FILEBROWSER;
	BROWSER_STATES browserstate = BROWSER_STATE_HOME;
	int selected = 0;
	std::string localpath = "/";
	std::string enigma2bouquet = "";
    std::vector<FsDirectoryEntry> localfileentries;
	std::vector<networkSource> networksources;
	std::string networkurl;
	std::string networklastpath;
	std::vector<remotefs_entry> networkentries;
	
	bool networkselect = true;
	int playershowcontrols = false;
	
    
} MenuItem;


extern SDL_Window *window;
extern MenuItem item;

extern Mpv *mpv;
extern uint32_t wakeup_on_mpv_render_update;
extern uint32_t wakeup_on_mpv_events;

extern mpv_opengl_fbo fbo;
extern mpv_render_param params[3];
extern int __fbo_one;

extern bool renderloopdone;

extern std::string nxmpTitle;

extern Enigma2 *enigma2;

extern Config *configini;

extern Tex FolderTexture;
extern Tex FileTexture;

namespace GUI {
	
	void initMpv();
	void HandleEvents();
	void HandleLayers();
	void HandleRender();
    int RenderLoop(void);
	
}



#endif