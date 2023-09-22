#include <switch.h>

#include <stdio.h>
#include <ctype.h>
#include <utility>

#include <SDL.h>
#include <glad/glad.h>


#include "iniparser.h"
#include "imgloader.h"
#include "config.h"
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




#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
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


/*	Exit from HB Menu instead fo returning  
	__nx_applet_exit_mode = 1;
	
*/

extern u32 __nx_applet_exit_mode;

static bool init();

/* Contexts */
SDL_Window *window;
SDL_GLContext context;

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

//SysIcons nxmpicons;

bool dochangethemefont = false;
std::string themefontpath = "";
float themefontsize;
//float themefontsmall;

//ImFont* fontSmall;

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



static bool init() {
	//get if console is docked
	printf("Started recording\n");
	fflush(stdout);
	appletInitializeGamePlayRecording();
    appletSetWirelessPriorityMode(AppletWirelessPriorityMode_OptimizedForWlan);
	
	extern u32 __nx_applet_type;
    auto saved_applet_type = std::exchange(__nx_applet_type, AppletType_SystemApplet );

    nvInitialize();
    __nx_applet_type = saved_applet_type;
	
	printf("Applet type\n");
	fflush(stdout);
	
	
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
	
	bool success = true;
	NXLOG::DEBUGLOG("INIT SDL");
    SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "no");
    if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0 ){
        NXLOG::ERRORLOG("%s: SDL could not initialize! SDL Error: %s", __func__, SDL_GetError());
        success =  false;
    }
    else {
		
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
		
		
	

		WIDTH = newResW; HEIGHT = newResH;
        window = SDL_CreateWindow(
                "[glad] GL with SDL",
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                WIDTH, HEIGHT,
                SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
        );
        if( window == NULL ){
            NXLOG::ERRORLOG("%s: Window could not be created! SDL Error: %s", __func__, SDL_GetError());
            success =  false;
        }
        else {
            context = SDL_GL_CreateContext(window);
			
            if( context == NULL )
            {
                NXLOG::ERRORLOG( "%s: OpenGL context could not be created! SDL Error: %s", __func__, SDL_GetError());
                success =  false;
            }
            else {
                gladLoadGL();
            }
			if(configini->getVSYNC(false)){
				SDL_GL_SetSwapInterval(1);
			}else{
				SDL_GL_SetSwapInterval(0);
			}
        }
    }
    return success;
}


void DeallocateExtern(){
	/*
	if(nxmpstats != nullptr){
		nxmpstats->CloseThreads();
		delete nxmpstats;
		nxmpstats = nullptr;
	}
	*/
	
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

const char *failtext =  "																			\n"
						"																			\n"
						"            ::::    :::       :::    :::         :::   :::       :::::::::\n" 
						"           :+:+:   :+:       :+:    :+:        :+:+: :+:+:      :+:    :+:\n" 
						"          :+:+:+  +:+        +:+  +:+        +:+ +:+:+ +:+     +:+    +:+\n"  
						"         +#+ +:+ +#+         +#++:+         +#+  +:+  +#+     +#++:++#+\n"    
						"        +#+  +#+#+#        +#+  +#+        +#+       +#+     +#+\n"          
						"       #+#   #+#+#       #+#    #+#       #+#       #+#     #+#\n"            
						"      ###    ####       ###    ###       ###       ###     ###\n";
						


int main() {
	appletLockExit();
	
#ifndef APPLETMODEENA
	AppletType at = appletGetAppletType();
    if ( at != AppletType_Application && at != AppletType_SystemApplication) {
		
		consoleInit(NULL);
		
		printf(failtext,VERSION_MAJOR,VERSION_MINOR,VERSION_MICRO);
		
		printf("\x1b[11;17H");
		printf("NXMP is coded by proconsule and darkxex\n");
		printf("\x1b[12;25H");
		printf("\x1b[32mversion: v%d.%d.%d\n",VERSION_MAJOR,VERSION_MINOR,VERSION_MICRO);
		
		
		printf("\x1b[19;23H");
		printf("\x1b[31mCannot run in Applet mode!\n");
		printf("\x1b[20;22");
		printf("\x1b[0mStart a Title Holding R Button\n");
		printf("\x1b[21;31HPress + to exit\n");
		
		 padConfigureInput(1, HidNpadStyleSet_NpadStandard);

		PadState pad;
		padInitializeDefault(&pad);
	
		while (appletMainLoop())
		{
			
			padUpdate(&pad);
			u64 kDown = padGetButtonsDown(&pad);

			if (kDown & HidNpadButton_Plus) break;
			
			consoleUpdate(NULL);
		}

		consoleExit(NULL);
		appletUnlockExit();
		return 0;
	}
#endif
	SocketInitConfig cfg = *(socketGetDefaultInitConfig());
	cfg.bsd_service_type = BsdServiceType_System;
	socketInitialize(&cfg);


#ifdef NDEBUG
	nxlinkStdio();
#endif	
	printf("Starting\n");
	
	Result ret = 0;
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
	
	eqpreset = new EQPreset("eqpresets.ini");
	
	playlist = new Playlist();
	
	if(configini->getDbActive(false)){
		sqlitedb = new SQLiteDB("nxmp.db");
		dbUpdated = sqlitedb->dbwasUpdated();
	}
	
	emuoverrides = configini->getEmuOverrides();
	
	
	shadermania = new shaderMania();


	if (R_FAILED(ret = romfsInit())) {
		NXLOG::ERRORLOG("romfsInit() failed: 0x%x\n", ret);
		return ret;
	}
	
	NXLOG::DEBUGLOG("Init GUI\n");
	if ( init() ) {
		
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
		io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		
		if(!configini->getTouchEnable(false)){
			io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
		}
		
		  
		io.IniFilename = nullptr;
		io.MouseDrawCursor = false;
        
        ImGui::StyleColorsDark();
		
		NXLOG::DEBUGLOG("Init IMGUI SDL BACKEND\n");
		ImGui_ImplSDL2_InitForOpenGL(window, context);
        NXLOG::DEBUGLOG("Init IMGUI OPENGL BACKEND\n");
		const char* glsl_version = "#version 430 core";
		
		ImGui_ImplOpenGL3_Init(glsl_version);
		
		if(configini->getConsoleWindow()){
			ConsoleWindow = new CConsoleWindow();
		}
		
		std::vector<std::string> extensionlist = configini->getConfigExtensions();
		
		Utility::setMediaExtensions(extensionlist);
		
		Utility::FontLoader("romfs:/DejaVuSans.ttf",currFontsize,io);
	
		NXLOG::DEBUGLOG("Loading Textures\n");
		
		imgloader = new CImgLoader("romfs:");
		
		

		/*
		if(configini->getThemeName(false) != "Default"){
			Themes  *themes = new Themes();
			themes->getThemes();
			int themeidx = themes->getThemeIDX(configini->getThemeName(false));
			if(themeidx >-1){
				themes->setTheme(themes->themeslist[themeidx].path);
			}
			delete themes;
		}else{
			Themes  *themes = new Themes();
			themes->setDefault();
			delete themes;
		}
		*/
		
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
		libmpv->setDemuxCache(configini->getDemuxCache(false));

		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		
		videoout = new CVOUT();
		videoout->Create_Framebuffer(w,h);
		
		if(nxmpstats == nullptr){
			nxmpstats = new CStats();
			nxmpstats->emuoverrides = emuoverrides;
			nxmpstats->StartThreads();
		}
		
		GUI::RenderLoop();
		NXLOG::DEBUGLOG("Ending Render Loop\n");


		SwitchSys::defaultClock(SwitchSys::stock_cpu_clock, SwitchSys::stock_gpu_clock, SwitchSys::stock_emc_clock);                


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
	
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
		SDL_GL_DeleteContext(context);
		
		
	}
	SDL_DestroyWindow(window);
	window = NULL;
	SDL_Quit();
	
	
	
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
	ncmExit();
	romfsExit();
    
	NXLOG::DEBUGLOG("Socket End\n");
	
	socketExit();
	__nx_applet_exit_mode = 1;
	appletUnlockExit();
    exit(1);
}
