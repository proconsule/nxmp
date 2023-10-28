#include <switch.h>

#include <stdio.h>
#include <ctype.h>
#include <utility>


#include <glad/glad.h>

#include "nxmp-gfx.h"
#include "iniparser.h"
#include "imgloader.h"
#include "gui.h"
#include "utils.h"
#include "localfiles.h"
#include "networkShareClass.h"
#include "ftplib.h"
#include "HTTPDir.h"
#include "FTPDir.h"
#include "sshDir.h"
#include "sambaDir.h"
#include "NX-UPNP.h"
#include "nfsDir.h"

#include "nxmp-i18n.h"


#include "imgui.h"

#include "SimpleIni.h"
#include "SQLiteDB.h"
#include "eqpreset.h"
#include "mtpclass.h"

#include "playlist.h"

#include "shaderMania.h"
#include "SwitchSys.h"

#include "stats.h"
#include "logger.h"

#include <switch.h>

#define NDEBUG 1

//#define APPLETMODEENA 1


extern u32 __nx_applet_exit_mode;

static bool init();

/* Classes */

libMpv *libmpv = nullptr;

CFileBrowser *filebrowser = nullptr;
CTextScroller * BrowserTextScroller = nullptr;
CTextScroller * FilePopupTextScroller = nullptr;
CTextScroller * playerTextScroller = nullptr;

CNetworkShare *NewNetworkShare = nullptr;
NXUPnP *nxupnp = nullptr;
USBMounter *usbmounter = nullptr;
Enigma2 *enigma2 = nullptr;
CMediaProbe *MediaProbe = nullptr;

USBMounter *MyUSBMount = nullptr;

shaderMania* shadermania = nullptr;

Themes *themes = nullptr;

CIniParser *configini = nullptr;
CImgLoader *imgloader = nullptr;

EQPreset *eqpreset = nullptr;
SQLiteDB *sqlitedb = nullptr;
CMTP *mtp = nullptr;

Playlist *playlist = nullptr;
CStats *nxmpstats = nullptr;
CConsoleWindow *ConsoleWindow = nullptr;

CVOUT *videoout = nullptr;

/* VARS */


bool dbUpdated = false;
bool renderloopdone = false;


uint32_t wakeup_on_mpv_render_update;
uint32_t wakeup_on_mpv_events;

bool dochangethemefont = false;
std::string themefontpath = "";
float themefontsize;

bool isHandheld = true;
bool clockoc = false;
int newResW = 1280;
int newResH = 720;
float multiplyRes = 1.0f;
float initScale = 1.0f;
int initSize = 55;
int batteryPercent = 0;
std::string tempKbUrl = "https://";


float currFontsize = 20.0f; 

GLuint WIDTH = handheldWidth, HEIGHT = handheldWidth;

std::string nxmpTitle = std::string("NXMP v") + std::to_string(VERSION_MAJOR) + std::string(".") + std::to_string(VERSION_MINOR) + std::string(".") + std::to_string(VERSION_MICRO)  + std::string(" ") + (RELEASE_TYPE == 0 ? std::string("Stable"): RELEASE_TYPE == 1 ? std::string("Beta"): std::string("R.C."));

int64_t playercachesec =  0;
int64_t playercachesize = 0;


void DeallocateExtern(){
	
	if(playerTextScroller != nullptr){
		delete playerTextScroller;
		playerTextScroller=nullptr;
	}
	if(ConsoleWindow!= nullptr){
		delete ConsoleWindow;
		ConsoleWindow=nullptr;
	}
	if(NewNetworkShare != nullptr){
		delete NewNetworkShare;
		NewNetworkShare=nullptr;
	}
	if(filebrowser == nullptr){
		delete filebrowser;
		filebrowser = nullptr;
	}
	if(FilePopupTextScroller == nullptr){
		delete FilePopupTextScroller;
		BrowserTextScroller = nullptr;
	}
	if(BrowserTextScroller == nullptr){
		delete BrowserTextScroller;
		BrowserTextScroller = nullptr;
	}
	
	if(eqpreset != nullptr){
		delete eqpreset;
		eqpreset = nullptr;
	}
	
	if(usbmounter != nullptr){
		delete usbmounter;
		usbmounter = nullptr;
	}
	if(MediaProbe != nullptr){
		delete MediaProbe;
		MediaProbe = nullptr;
	}
	if(enigma2 != nullptr){
		delete enigma2;
		enigma2 = nullptr;
	}
	if(nxupnp != nullptr){
		delete nxupnp;
		nxupnp = nullptr;
	}
	if(sqlitedb != nullptr){
		delete sqlitedb;
		sqlitedb = nullptr;
	}
	if(MyUSBMount != nullptr){
		delete MyUSBMount;
		MyUSBMount=nullptr;
	}
	if(playlist != nullptr){
		delete playlist;
		playlist=nullptr;
	}
	if(themes != nullptr){
		delete themes;
		themes=nullptr;
	}
	if(shadermania != nullptr){
		delete shadermania;
		shadermania=nullptr;
	}
	
	if(configini != nullptr){
		delete configini;
		configini = nullptr;
	}
	
	
}


extern "C" void userAppInit() {
	romfsInit();
	SocketInitConfig cfg = *(socketGetDefaultInitConfig());
	cfg.bsd_service_type = BsdServiceType_System;
	socketInitialize(&cfg);

}

extern "C" void userAppExit(void) {
	
	romfsExit();
	socketExit();
	appletUnlockExit();

}




int main() {
	appletLockExit();
	Result ret = 0;
	
	/*
	if (R_FAILED(ret = romfsInit())) {
		NXLOG::ERRORLOG("romfsInit() failed: 0x%x\n", ret);
		return ret;
	}
	*/
	
	AppletOperationMode stus=appletGetOperationMode();
	if (stus == AppletOperationMode_Handheld) {
		NXLOG::DEBUGLOG("Handheld Mode\n");
		isHandheld=true;
		newResW = handheldWidth;
		newResH = handheldHeight;
		multiplyRes = 1.0f;
		currFontsize = 20.0f;
	}
	if (stus == AppletOperationMode_Console) {
		NXLOG::DEBUGLOG("Docked Mode\n");
		isHandheld=false;
		newResW = dockedWidth;
		newResH = dockedHeight;
		multiplyRes = 1.5f;
		currFontsize = 30.0f;
	}
	
#ifndef APPLETMODEENA
	AppletType at = appletGetAppletType();
    if ( at != AppletType_Application && at != AppletType_SystemApplication) {
		
		
		nxmpgfx::Init_Backend(!isHandheld,true);
		nxmpgfx::Init_Backend_AppletMode(!isHandheld);
		nxmpgfx::loopAppletMode();


		
		nxmpgfx::Destroy_Backend();
		romfsExit();
		appletUnlockExit();
		return 0;
	}
#endif
	


#ifdef NDEBUG
	nxlinkStdio();
#endif	
	printf("Starting\n");
	
	if (R_FAILED(ret = nifmInitialize(NifmServiceType_User))) {
		NXLOG::ERRORLOG("nifmInitialize(NifmServiceType_User) failed: 0x%x\n", ret);
		return ret;
	}
	
	NXLOG::loglevel = 0;
	bool emuoverrides = true;
	
	printf("Loading Config\n");
	fflush(stdout);
	
	NXLOG::DEBUGLOG("Loading Config\n");
	
	configini = new CIniParser("config.ini");
	NXLOG::loglevel = configini->getLogLevel();
	if(NXLOG::loglevel >0){
		configini->PrintConfig();
	}
	
	
	appletInitializeGamePlayRecording();
    appletSetWirelessPriorityMode(AppletWirelessPriorityMode_OptimizedForWlan);
	
	extern u32 __nx_applet_type;
    auto saved_applet_type = std::exchange(__nx_applet_type, AppletType_SystemApplet );

    nvInitialize();
    __nx_applet_type = saved_applet_type;
	
	nxmpgfx::Init_Backend(!isHandheld,configini->getVSYNC(false));
	nxmpgfx::Init_Backend_Splash(!isHandheld);
	
	
	
	
	eqpreset = new EQPreset("eqpresets.ini");
	
	playlist = new Playlist();
	
	if(configini->getDbActive(false)){
		sqlitedb = new SQLiteDB(configini->getDbPath());
		dbUpdated = sqlitedb->dbwasUpdated();
	}
	
	emuoverrides = configini->getEmuOverrides();
	
	
	shadermania = new shaderMania();

	
	nxmpgfx::updateSplash(25);
	
	
	
	
	
	nxmpgfx::Init_ImGui(!isHandheld);


	nxmpgfx::updateSplash(50);
	//if(configini->getConsoleWindow()){
		ConsoleWindow = new CConsoleWindow();
	//}
		
	std::vector<std::string> extensionlist = configini->getConfigExtensions();
		
	Utility::setMediaExtensions(extensionlist);
	
		
	Themes  *themes = new Themes();
	themes->getThemes();
	int themeidx = themes->getThemeIDX(configini->getThemeName(true));
	if(themeidx >-1){
		themes->setTheme(themes->themeslist[themeidx].path);
	}

	NXLOG::DEBUGLOG("Loading Textures & Fonts\n");
	
	if(themeidx == -1){
		imgloader = new CImgLoader("romfs:");
		
		nxmpgfx::updateSplash(50);
		nxmpgfx::UniFontLoader(themes->getThemeFonts(-1,configini->getOnlyLatinRange(false)));
		nxmpgfx::updateSplash(100);
	}else{
		if(isHandheld){
			nxmpgfx::UniFontLoader(themes->getThemeFonts(themeidx,configini->getOnlyLatinRange(false)));
			nxmpgfx::updateSplash(100);
		}else{
			nxmpgfx::UniFontLoader(themes->getThemeFonts(themeidx,configini->getOnlyLatinRange(false)));
			nxmpgfx::updateSplash(100);
		}
		imgloader = new CImgLoader(themes->themeslist[themeidx].path);
		themes->setThemeColor(themes->themeslist[themeidx].path);
	}
	delete themes;
		
		
	if (hosversionBefore(8, 0, 0)) {
		if (R_SUCCEEDED(pcvInitialize())) {
			SwitchSys::stock_cpu_clock = SwitchSys::getClock(SwitchSys::Module::Cpu);
			SwitchSys::stock_gpu_clock = SwitchSys::getClock(SwitchSys::Module::Gpu);
			SwitchSys::stock_emc_clock = SwitchSys::getClock(SwitchSys::Module::Emc);
		}
	} else {
		if (R_SUCCEEDED(clkrstInitialize())) {
			SwitchSys::stock_cpu_clock = SwitchSys::getClock(SwitchSys::Module::Cpu);
			SwitchSys::stock_gpu_clock = SwitchSys::getClock(SwitchSys::Module::Gpu);
			SwitchSys::stock_emc_clock = SwitchSys::getClock(SwitchSys::Module::Emc);
		}
	}

	NXLOG::DEBUGLOG("SWITCHRenderer(): clocks: cpu=%i, gpu=%i, emc=%i\n",
	SwitchSys::stock_cpu_clock, SwitchSys::stock_gpu_clock, SwitchSys::stock_emc_clock);

	
	GUI::initMpv();
	
	
	videoout = new CVOUT();
	videoout->Create_Framebuffer(nxmpgfx::WIDTH,nxmpgfx::HEIGHT);
		
	if(nxmpstats == nullptr){
		nxmpstats = new CStats();
		nxmpstats->emuoverrides = emuoverrides;
		nxmpstats->StartThreads();
	}
	
	
	GUI::RenderLoop();
	NXLOG::DEBUGLOG("Ending Render Loop\n");

	SwitchSys::defaultClock(SwitchSys::stock_cpu_clock, SwitchSys::stock_gpu_clock, SwitchSys::stock_emc_clock);                

	__nx_applet_exit_mode = configini->getExitMode(true);

	if(videoout!= nullptr){
		delete videoout;
		videoout = nullptr;
	}
	delete libmpv;
	libmpv = nullptr;
	NXLOG::DEBUGLOG("Ending MPV\n");
		
	DeallocateExtern();	
	if(imgloader != nullptr){
		delete imgloader;
		imgloader = nullptr;
	}
	
	
	
	nxmpgfx::Destroy();
	nxmpgfx::Quit();
	
	
	if(nxmpstats != nullptr){
		nxmpstats->CloseThreads();
		delete nxmpstats;
		nxmpstats = nullptr;
	}
	
	
	NXLOG::DEBUGLOG("Exit Services\n");
	

	if (hosversionBefore(8, 0, 0)) {
		pcvExit();
    } else {
		clkrstExit();
    }

	nifmExit();
	
    
	
	NXLOG::DEBUGLOG("Socket End\n");
	nvExit();
	
	
	
    exit(0);
}
