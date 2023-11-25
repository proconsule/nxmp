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


CIniParser *configini = nullptr;
CImgLoader *imgloader = nullptr;

EQPreset *eqpreset = nullptr;
SQLiteDB *sqlitedb = nullptr;
CMTP *mtp = nullptr;

Playlist *playlist = nullptr;
CStats *nxmpstats = nullptr;
CConsoleWindow *ConsoleWindow = nullptr;

//CVOUT *videoout = nullptr;

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

unsigned int app_exit_mode = 0;

float currFontsize = 20.0f; 

GLuint WIDTH = handheldWidth, HEIGHT = handheldWidth;

#ifdef OPENGL_BACKEND
std::string nxmpTitle = std::string("NXMP v") + std::to_string(VERSION_MAJOR) + std::string(".") + std::to_string(VERSION_MINOR) + std::string(".") + std::to_string(VERSION_MICRO)  + std::string(" ") + (RELEASE_TYPE == 0 ? std::string("Stable"): RELEASE_TYPE == 1 ? std::string("Beta"): std::string("R.C.")+ (" (OpenGL)"));
#endif
#ifdef DEKO3D_BACKEND
std::string nxmpTitle = std::string("NXMP v") + std::to_string(VERSION_MAJOR) + std::string(".") + std::to_string(VERSION_MINOR) + std::string(".") + std::to_string(VERSION_MICRO)  + std::string(" ") + (RELEASE_TYPE == 0 ? std::string("Stable"): RELEASE_TYPE == 1 ? std::string("Beta"): std::string("R.C."))+ (" (deko3d)");
#endif


int64_t playercachesec =  0;
int64_t playercachesize = 0;


bool slaveplayer = false;
std::string slaveplayer_file = "";


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
	
	plExit();
	romfsExit();
	socketExit();
	appletUnlockExit();

}




int main(int argc, char* argv[]) {
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
		currFontsize = 18.0f;
	}
	if (stus == AppletOperationMode_Console) {
		NXLOG::DEBUGLOG("Docked Mode\n");
		isHandheld=false;
		newResW = dockedWidth;
		newResH = dockedHeight;
		multiplyRes = 1.5f;
		currFontsize = 27.0f;
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
	if(argc>1){
		slaveplayer=true;
		printf("File: %s\n",argv[1]);
		slaveplayer_file = argv[1];
	}
	
	if (R_FAILED(ret = nifmInitialize(NifmServiceType_User))) {
		NXLOG::ERRORLOG("nifmInitialize(NifmServiceType_User) failed: 0x%x\n", ret);
		return ret;
	}
	
	if (R_FAILED(ret = plInitialize(PlServiceType_User))) {
		NXLOG::ERRORLOG("plInitialize(PlServiceType_User) failed: 0x%x\n", ret);
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
	
	if(configini->getExitMode(false) == 1){
		app_exit_mode = 0;
	}else if(configini->getExitMode(false) == 2){
		app_exit_mode = 0;
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
	
	if(configini->getDbActive(false) && !slaveplayer){
		sqlitedb = new SQLiteDB(configini->getDbPath());
		dbUpdated = sqlitedb->dbwasUpdated();
	}
	
	emuoverrides = configini->getEmuOverrides();
	if(!emuoverrides){
		audctlInitialize(); // Master Volume Test
		audctlGetSystemOutputMasterVolume(&nxmpgfx::CurrentVolume); 
	}
	
	shadermania = new shaderMania();

	
	nxmpgfx::updateSplash(25);
	
	
	nxmpgfx::Init_ImGui(!isHandheld);
	
	nxmpgfx::SetColorTheme(configini->getThemeColor(false));
	nxmpgfx::setEnableTouch(configini->getTouchEnable(false));


	nxmpgfx::updateSplash(50);
	//if(configini->getConsoleWindow()){
		ConsoleWindow = new CConsoleWindow();
	//}
		
	std::vector<std::string> extensionlist = configini->getConfigExtensions();
		
	Utility::setMediaExtensions(extensionlist);
	
	
	
	imgloader = new CImgLoader("romfs:");
#ifdef OPENGL_BACKEND
		std::vector<nxmpgfx::fonttype_struct> nullfonts;
		nxmpgfx::UniFontLoader(configini->getOnlyLatinRange(false));
#endif
		
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
	nxmpgfx::updateSplash(100);
	NXLOG::DEBUGLOG("SWITCHRenderer(): clocks: cpu=%i, gpu=%i, emc=%i\n",
	SwitchSys::stock_cpu_clock, SwitchSys::stock_gpu_clock, SwitchSys::stock_emc_clock);


	nxmpgfx::Create_VO_FrameBuffer(nxmpgfx::getWidth(),nxmpgfx::getHeight());
	
	GUI::initMpv();
	
	
	if(nxmpstats == nullptr){
		nxmpstats = new CStats();
		nxmpstats->emuoverrides = emuoverrides;
		nxmpstats->StartThreads();
	}
	
	
	GUI::RenderLoop();
	NXLOG::DEBUGLOG("Ending Render Loop\n");

	SwitchSys::defaultClock(SwitchSys::stock_cpu_clock, SwitchSys::stock_gpu_clock, SwitchSys::stock_emc_clock);                

	//__nx_applet_exit_mode = configini->getExitMode(true);

	__nx_applet_exit_mode = app_exit_mode;
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
	
	if(!emuoverrides){
		audctlExit();
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
