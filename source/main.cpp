#include <switch.h>

#include <stdio.h>
#include <ctype.h>
#include <utility>

#include <SDL.h>
#include <glad/glad.h>

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


static bool init();

SDL_Window *window;

SDL_GLContext context;
libMpv *libmpv = nullptr;

CFileBrowser *filebrowser = nullptr;
CTextScroller * BrowserTextScroller = nullptr;
CTextScroller * FilePopupTextScroller = nullptr;


CNetworkShare *NewNetworkShare = nullptr;
NXUPnP *nxupnp = nullptr;
USBMounter *usbmounter = nullptr;
Enigma2 *enigma2 = nullptr;
CMediaProbe *MediaProbe = nullptr;


USBMounter *MyUSBMount = nullptr;

Themes *themes = nullptr;

Config *configini = nullptr;
EQPreset *eqpreset = nullptr;
SQLiteDB *sqlitedb = nullptr;
CMTP *mtp = nullptr;
bool dbUpdated = false;

Playlist *playlist = nullptr;

CStats *nxmpstats = nullptr;

uint32_t wakeup_on_mpv_render_update;
uint32_t wakeup_on_mpv_events;


bool renderloopdone = false;

SysIcons nxmpicons;


bool dochangethemefont = false;
std::string themefontpath = "";
float themefontsize;
float themefontsmall;

ImFont* fontSmall;

bool isHandheld = true;
bool clockoc = false;
int newResW = 1280;
int newResH = 720;
float multiplyRes = 1.0f;
float initScale = 1.0f;
int initSize = 55;
int batteryPercent = 0;
std::string tempKbUrl = "https://";
shaderMania* shadermania = nullptr;

float currFontsize = 20.0f; 

GLuint WIDTH = handheldWidth, HEIGHT = handheldWidth;

std::string nxmpTitle = std::string("NXMP v") + std::to_string(VERSION_MAJOR) + std::string(".") + std::to_string(VERSION_MINOR) + std::string(".") + std::to_string(VERSION_MICRO);


int64_t playercachesec =  0;
int64_t playercachesize = 0;
CVOUT *videoout = nullptr;



void deinitTextures(){

	glDeleteTextures(1, &nxmpicons.SdCardTexture.id);
	glDeleteTextures(1, &nxmpicons.UsbTexture.id);
	glDeleteTextures(1, &nxmpicons.NetworkTexture.id);
	glDeleteTextures(1, &nxmpicons.Enigma2Texture.id);
	glDeleteTextures(1, &nxmpicons.FileTexture.id);
	glDeleteTextures(1, &nxmpicons.PlaylistTexture.id);
	glDeleteTextures(1, &nxmpicons.InfoTexture.id);
	glDeleteTextures(1, &nxmpicons.FFMPEGTexture.id);
	glDeleteTextures(1, &nxmpicons.HTTPTexture.id);
	glDeleteTextures(1, &nxmpicons.FTPTexture.id);
	glDeleteTextures(1, &nxmpicons.SFTPTexture.id);
	glDeleteTextures(1, &nxmpicons.SMBTexture.id);
	glDeleteTextures(1, &nxmpicons.NFSTexture.id);
	glDeleteTextures(1, &nxmpicons.SettingsTexture.id);
	glDeleteTextures(1, &nxmpicons.UPNPTexture.id);
	glDeleteTextures(1, &nxmpicons.MPVTexture.id);
	glDeleteTextures(1, &nxmpicons.ExitTexture.id);
	glDeleteTextures(1, &nxmpicons.FolderTexture.id);
	glDeleteTextures(1, &nxmpicons.NXMPBannerTexture.id);
	glDeleteTextures(1, &nxmpicons.PlayIcon.id);
	glDeleteTextures(1, &nxmpicons.StopIcon.id);
	glDeleteTextures(1, &nxmpicons.PauseIcon.id);
	glDeleteTextures(1, &nxmpicons.MuteIcon.id);
	glDeleteTextures(1, &nxmpicons.VolumeIcon.id);
	glDeleteTextures(1, &nxmpicons.LoopIcon.id);
	glDeleteTextures(1, &nxmpicons.NoLoopIcon.id);
	glDeleteTextures(1, &nxmpicons.ShareAddTexture.id);
	
	glDeleteTextures(1, &nxmpicons.GUI_D_UP.id);
	glDeleteTextures(1, &nxmpicons.GUI_D_DOWN.id);
	glDeleteTextures(1, &nxmpicons.GUI_D_LEFT.id);
	glDeleteTextures(1, &nxmpicons.GUI_D_RIGHT.id);
	
	glDeleteTextures(1, &nxmpicons.GUI_A_BUT.id);
	glDeleteTextures(1, &nxmpicons.GUI_B_BUT.id);
	glDeleteTextures(1, &nxmpicons.GUI_X_BUT.id);
	glDeleteTextures(1, &nxmpicons.GUI_Y_BUT.id);
	
}


static bool init() {
	//get if console is docked
	AppletOperationMode stus=appletGetOperationMode();
	appletInitializeGamePlayRecording();
    appletSetWirelessPriorityMode(AppletWirelessPriorityMode_OptimizedForWlan);
	
	extern u32 __nx_applet_type;
    auto saved_applet_type = std::exchange(__nx_applet_type, AppletType_LibraryApplet);

    nvInitialize();
    __nx_applet_type = saved_applet_type;
	
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
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		
		
	

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
	
	if(videoout!= nullptr){
		delete videoout;
		videoout = nullptr;
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
	
	 
	
	
	
}

const char *failtext =  "          ::::    :::       :::    :::         :::   :::       :::::::::\n" 
"         :+:+:   :+:       :+:    :+:        :+:+: :+:+:      :+:    :+:\n" 
"        :+:+:+  +:+        +:+  +:+        +:+ +:+:+ +:+     +:+    +:+\n"  
"       +#+ +:+ +#+         +#++:+         +#+  +:+  +#+     +#++:++#+\n"    
"      +#+  +#+#+#        +#+  +#+        +#+       +#+     +#+\n"          
"     #+#   #+#+#       #+#    #+#       #+#       #+#     #+#\n"            
"    ###    ####       ###    ###       ###       ###     ###\n";                  



int main() {
	
	AppletType at = appletGetAppletType();
    if (at != AppletType_Application && at != AppletType_SystemApplication) {
		
		
		consoleInit(NULL);
		
		printf(failtext);
		
		printf("\x1b[19;20HCannot run in Applet mode!\n");
		printf("\x1b[20;19HStart a Game Holding R Button\n");
		printf("\x1b[21;27HPress + to exit\n");
		
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
		
		return 0;
	}
	
	appletLockExit();
	//socketInitializeDefault();
	SocketInitConfig cfg = *(socketGetDefaultInitConfig());
	cfg.bsd_service_type = BsdServiceType_System;
	socketInitialize(&cfg);


#ifdef NDEBUG
	nxlinkStdio();
#endif	
	
	NXLOG::loglevel = 0;
	bool emuoverrides = true;
	

	NXLOG::DEBUGLOG("Loading Config\n");
	
	configini = new Config("config.ini");
	
	eqpreset = new EQPreset("eqpresets.ini");
	
	playlist = new Playlist();
	
	if(configini->getDbActive(false)){
		sqlitedb = new SQLiteDB("nxmp.db");
		dbUpdated = sqlitedb->dbwasUpdated();
	}
	
	emuoverrides = configini->getEmuOverrides();
	NXLOG::loglevel = configini->getLogLevel();
	
	shadermania = new shaderMania();
	NXLOG::loglevel = 2;
	

	Result ret;
	if (R_FAILED(ret = romfsInit())) {
		NXLOG::ERRORLOG("romfsInit() failed: 0x%x\n", ret);
		return ret;
	}
	
	//if (R_FAILED(ret = psmInitialize()))
	//	printf("psmInitialize() failed: 0x%x.\n\n", ret);

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
			NXLOG::ConsoleWindow = new CConsoleWindow();
		}
		
		std::vector<std::string> extensionlist = configini->getConfigExtensions();
		
		Utility::setMediaExtensions(extensionlist);
		
		//plInitialize(PlServiceType_System);
		if (R_FAILED(ret = nifmInitialize(NifmServiceType_User))) {
			NXLOG::ERRORLOG("nifmInitialize(NifmServiceType_User) failed: 0x%x\n", ret);
			return ret;
		}
		
		
		Utility::FontLoader("romfs:/DejaVuSans.ttf",currFontsize,io);
	
		NXLOG::DEBUGLOG("Loading Textures\n");

		Utility::TxtLoadFromFile("romfs:/sdcard.png",&nxmpicons.SdCardTexture.id,&nxmpicons.SdCardTexture.width,&nxmpicons.SdCardTexture.height);
		Utility::TxtLoadFromFile("romfs:/usb.png",&nxmpicons.UsbTexture.id,&nxmpicons.UsbTexture.width,&nxmpicons.UsbTexture.height);
		Utility::TxtLoadFromFile("romfs:/network.png",&nxmpicons.NetworkTexture.id,&nxmpicons.NetworkTexture.width,&nxmpicons.NetworkTexture.height);
		Utility::TxtLoadFromFile("romfs:/shareadd.png",&nxmpicons.ShareAddTexture.id,&nxmpicons.ShareAddTexture.width,&nxmpicons.ShareAddTexture.height);
		Utility::TxtLoadFromFile("romfs:/enigma2.png",&nxmpicons.Enigma2Texture.id,&nxmpicons.Enigma2Texture.width,&nxmpicons.Enigma2Texture.height);
		Utility::TxtLoadFromFile("romfs:/folder.png",&nxmpicons.FolderTexture.id,&nxmpicons.FolderTexture.width,&nxmpicons.FolderTexture.height);
		Utility::TxtLoadFromFile("romfs:/file.png",&nxmpicons.FileTexture.id,&nxmpicons.FileTexture.width,&nxmpicons.FileTexture.height);
		Utility::TxtLoadFromFile("romfs:/playlist.png",&nxmpicons.PlaylistTexture.id,&nxmpicons.PlaylistTexture.width,&nxmpicons.PlaylistTexture.height);
		Utility::TxtLoadFromFile("romfs:/info.png",&nxmpicons.InfoTexture.id,&nxmpicons.InfoTexture.width,&nxmpicons.InfoTexture.height);
		Utility::TxtLoadFromFile("romfs:/settings.png",&nxmpicons.SettingsTexture.id,&nxmpicons.SettingsTexture.width,&nxmpicons.SettingsTexture.height);
		Utility::TxtLoadFromFile("romfs:/ffmpeg.png",&nxmpicons.FFMPEGTexture.id,&nxmpicons.FFMPEGTexture.width,&nxmpicons.FFMPEGTexture.height);
		Utility::TxtLoadFromFile("romfs:/http.png",&nxmpicons.HTTPTexture.id,&nxmpicons.HTTPTexture.width,&nxmpicons.HTTPTexture.height);
		Utility::TxtLoadFromFile("romfs:/ftp.png",&nxmpicons.FTPTexture.id,&nxmpicons.FTPTexture.width,&nxmpicons.FTPTexture.height);
		Utility::TxtLoadFromFile("romfs:/sftp.png",&nxmpicons.SFTPTexture.id,&nxmpicons.SFTPTexture.width,&nxmpicons.SFTPTexture.height);
		Utility::TxtLoadFromFile("romfs:/smb.png",&nxmpicons.SMBTexture.id,&nxmpicons.SMBTexture.width,&nxmpicons.SMBTexture.height);
		Utility::TxtLoadFromFile("romfs:/nfs.png",&nxmpicons.NFSTexture.id,&nxmpicons.NFSTexture.width,&nxmpicons.NFSTexture.height);
		Utility::TxtLoadFromFile("romfs:/upnp.png",&nxmpicons.UPNPTexture.id,&nxmpicons.UPNPTexture.width,&nxmpicons.UPNPTexture.height);
		Utility::TxtLoadFromFile("romfs:/mpv.png",&nxmpicons.MPVTexture.id,&nxmpicons.MPVTexture.width,&nxmpicons.MPVTexture.height);
		Utility::TxtLoadFromFile("romfs:/exit.png",&nxmpicons.ExitTexture.id,&nxmpicons.ExitTexture.width,&nxmpicons.ExitTexture.height);
		Utility::TxtLoadFromFile("romfs:/nxmp-banner.jpg",&nxmpicons.NXMPBannerTexture.id,&nxmpicons.NXMPBannerTexture.width,&nxmpicons.NXMPBannerTexture.height);
		Utility::TxtLoadFromFile("romfs:/player/play.png",&nxmpicons.PlayIcon.id,&nxmpicons.PlayIcon.width,&nxmpicons.PlayIcon.height);
		Utility::TxtLoadFromFile("romfs:/player/stop.png",&nxmpicons.StopIcon.id,&nxmpicons.StopIcon.width,&nxmpicons.StopIcon.height);
		Utility::TxtLoadFromFile("romfs:/player/pause.png",&nxmpicons.PauseIcon.id,&nxmpicons.PauseIcon.width,&nxmpicons.PauseIcon.height);
		Utility::TxtLoadFromFile("romfs:/player/mute.png",&nxmpicons.MuteIcon.id,&nxmpicons.MuteIcon.width,&nxmpicons.MuteIcon.height);
		Utility::TxtLoadFromFile("romfs:/player/volume.png",&nxmpicons.VolumeIcon.id,&nxmpicons.VolumeIcon.width,&nxmpicons.VolumeIcon.height);
		Utility::TxtLoadFromFile("romfs:/player/loop.png",&nxmpicons.LoopIcon.id,&nxmpicons.LoopIcon.width,&nxmpicons.LoopIcon.height);
		Utility::TxtLoadFromFile("romfs:/player/noloop.png",&nxmpicons.NoLoopIcon.id,&nxmpicons.NoLoopIcon.width,&nxmpicons.NoLoopIcon.height);
		
		Utility::TxtLoadFromFile("romfs:/gui/dpad-up.png",&nxmpicons.GUI_D_UP.id,&nxmpicons.GUI_D_UP.width,&nxmpicons.GUI_D_UP.height);
		Utility::TxtLoadFromFile("romfs:/gui/dpad-down.png",&nxmpicons.GUI_D_DOWN.id,&nxmpicons.GUI_D_DOWN.width,&nxmpicons.GUI_D_DOWN.height);
		Utility::TxtLoadFromFile("romfs:/gui/dpad-left.png",&nxmpicons.GUI_D_LEFT.id,&nxmpicons.GUI_D_LEFT.width,&nxmpicons.GUI_D_LEFT.height);
		Utility::TxtLoadFromFile("romfs:/gui/dpad-right.png",&nxmpicons.GUI_D_RIGHT.id,&nxmpicons.GUI_D_RIGHT.width,&nxmpicons.GUI_D_RIGHT.height);
		
		Utility::TxtLoadFromFile("romfs:/gui/a-but.png",&nxmpicons.GUI_A_BUT.id,&nxmpicons.GUI_A_BUT.width,&nxmpicons.GUI_A_BUT.height);
		Utility::TxtLoadFromFile("romfs:/gui/b-but.png",&nxmpicons.GUI_B_BUT.id,&nxmpicons.GUI_B_BUT.width,&nxmpicons.GUI_B_BUT.height);
		Utility::TxtLoadFromFile("romfs:/gui/x-but.png",&nxmpicons.GUI_X_BUT.id,&nxmpicons.GUI_X_BUT.width,&nxmpicons.GUI_X_BUT.height);
		Utility::TxtLoadFromFile("romfs:/gui/y-but.png",&nxmpicons.GUI_Y_BUT.id,&nxmpicons.GUI_Y_BUT.width,&nxmpicons.GUI_Y_BUT.height);
		

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
		
		
		nxmpstats = new CStats();
		nxmpstats->emuoverrides = emuoverrides;
		nxmpstats->StartThreads();
		
		
		GUI::RenderLoop();
		NXLOG::DEBUGLOG("Ending Render Loop\n");



		SwitchSys::defaultClock(SwitchSys::stock_cpu_clock, SwitchSys::stock_gpu_clock, SwitchSys::stock_emc_clock);                



		delete libmpv;
		libmpv = nullptr;
		NXLOG::DEBUGLOG("Ending MPV\n");
		
		
		
	
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
		SDL_GL_DeleteContext(context);
		deinitTextures();
		
	}
	SDL_DestroyWindow(window);
	window = NULL;
	SDL_Quit();
	
	DeallocateExtern();
	
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

	//ncmExit();
	romfsExit();
    socketExit();
	
	appletUnlockExit();

    return 0;	
}
