
#include "platforms.h"

#ifdef NXMP_SWITCH
#include <switch.h>
#endif


#include <stdio.h>
#include <ctype.h>

#include <SDL.h>
#include <glad/glad.h>


#include "config.h"
#include "gui.h"
#include "utils.h"
#include "localfiles.h"
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

#include "playlist.h"

#include "shaderMania.h"
#include "SwitchSys.h"

#define NDEBUG 1

#ifdef NXMP_SWITCH
using namespace c2d;
#endif

static bool init();

SDL_Window *window;
#ifdef NXMP_WIN32
bool fullscreen = false;
#endif

SDL_GLContext context;
libMpv *libmpv = nullptr;
localFs *localdir = nullptr;
#ifdef NXMP_NETWORKSUPPORT
FTPDir *ftpdir = nullptr;
HTTPDir *httpdir = nullptr;
sshDir *sshdir = nullptr;
sambaDir *sambadir = nullptr;
nfsDir *nfsdir = nullptr;
#endif

#ifdef NXMP_UPNPSUPPORT
NXUPnP *nxupnp = nullptr;
#endif

#ifdef NXMP_USBSUPPORT
USBMounter *usbmounter = nullptr;
#endif

#ifdef NXMP_ENIGMASUPPORT
Enigma2 *enigma2 = nullptr;
#endif

Themes *themes = nullptr;

Config *configini = nullptr;
EQPreset *eqpreset = nullptr;
SQLiteDB *sqlitedb = nullptr;
bool dbUpdated = false;

Playlist *playlist = nullptr;

uint32_t wakeup_on_mpv_render_update;
uint32_t wakeup_on_mpv_events;

mpv_opengl_fbo fbo;
mpv_render_param params[3];
int __fbo_one = 1;
bool renderloopdone = false;

SysIcons nxmpicons;


bool dochangethemefont = false;
std::string themefontpath = "";
float themefontsize;
float themefontsmall;

ImFont* fontSmall;

bool isHandheld=true;
int newResW = 1280;
int newResH = 720;
float multiplyRes = 1.0f;
shaderMania* shadermania = nullptr;

GLuint WIDTH = handheldWidth, HEIGHT = handheldWidth;

std::string nxmpTitle = std::string("NXMP v") + std::to_string(VERSION_MAJOR) + std::string(".") + std::to_string(VERSION_MINOR) + std::string(".") + std::to_string(VERSION_MICRO);

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
	
}


static bool init() {
	#ifdef NXMP_SWITCH
    //get if console is docked
	AppletOperationMode stus=appletGetOperationMode();
	if (stus == AppletOperationMode_Handheld) 
	{printf("Handheld Mode\n");
	isHandheld=true;
	newResW = handheldWidth;
	newResH = handheldHeight;
	multiplyRes = 1.0f;
	}
	if (stus == AppletOperationMode_Console) 
	{printf("Docked Mode\n");
	isHandheld=false;
	newResW = dockedWidth;
	newResH = dockedHeight;
	multiplyRes = 1.5f;
	}
    #endif
	
	
	bool success = true;

	SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "no");
    if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0 ){
        printf("%s: SDL could not initialize! SDL Error: %s", __func__, SDL_GetError());
        success =  false;
    }
    else {
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
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
            printf("%s: Window could not be created! SDL Error: %s", __func__, SDL_GetError());
            success =  false;
        }
        else {
            context = SDL_GL_CreateContext(window);
            if( context == NULL )
            {
                printf( "%s: OpenGL context could not be created! SDL Error: %s", __func__, SDL_GetError());
                success =  false;
            }
            else {
                gladLoadGL();
            }
        }
    }
    return success;
}

#ifdef NXMP_SWITCH
int main() {
	 appletLockExit();
#else
int main(int argc,char *argv[]){
	if(argc>1){
		if(strcmp(argv[1],"-d") == 0){
			isHandheld=false;
			newResW = dockedWidth;
			newResH = dockedHeight;
			multiplyRes = 1.5f;
		}
	}
#endif
	

#ifdef NXMP_SWITCH
	socketInitializeDefault();
#ifdef NDEBUG
	nxlinkStdio();
#endif	
#endif

	printf("Loading Config\n");
	
	configini = new Config("config.ini");
	
	eqpreset = new EQPreset("eqpresets.ini");
	
	playlist = new Playlist();
	
	if(configini->getDbActive(false)){
		sqlitedb = new SQLiteDB("nxmp.db");
		dbUpdated = sqlitedb->dbwasUpdated();
	}
	
	shadermania = new shaderMania();
	
	
#ifdef NXMP_SWITCH
	Result ret;
	if (R_FAILED(ret = romfsInit())) {
		printf("romfsInit() failed: 0x%x\n", ret);
		return ret;
	}
#endif
	printf("Init GUI\n");
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
		printf("Init MPV\n");
		
		printf("Init SDL\n");
		ImGui_ImplSDL2_InitForOpenGL(window, context);
        printf("Init OPENGL\n");
		ImGui_ImplOpenGL3_Init("#version 430 core");
		
		
#ifdef NXMP_SWITCH
		plInitialize(PlServiceType_System);
		if (R_FAILED(ret = nifmInitialize(NifmServiceType_User))) {
			printf("nifmInitialize(NifmServiceType_User) failed: 0x%x\n", ret);
			return ret;
		}
#endif
		printf("Init Fonts\n");
      
		unsigned char *pixels = nullptr;
		int width = 0, height = 0, bpp = 0;
		ImFontConfig font_cfg;

		printf("Loading TTF\n");
		
		
		
#ifdef NXMP_SWITCH
		io.Fonts->AddFontFromFileTTF("romfs:/DejaVuSans.ttf", 24.0f,&font_cfg);
		fontSmall = io.Fonts->AddFontFromFileTTF("romfs:/DejaVuSans.ttf", 16.0f,&font_cfg);
#else
		io.Fonts->AddFontFromFileTTF("./romfs/DejaVuSans.ttf", 24.0f,&font_cfg);
		fontSmall = io.Fonts->AddFontFromFileTTF("./romfs/DejaVuSans.ttf", 16.0f,&font_cfg);
#endif
		
			static const ImWchar tmranges[] =
                {
					0x2019, 0x2019,
                    0x2122, 0x2122,
					0x2713, 0x2713,
					0x2714, 0x2714,
					0x2716, 0x2716,
					0,
                };
	
	
	static ImWchar ranges[] = { 0x1, 0x1FFFF, 0 };
	font_cfg.OversampleH = font_cfg.OversampleV = 1;
	font_cfg.MergeMode = true;

#ifdef NXMP_SWITCH
	io.Fonts->AddFontFromFileTTF("romfs:/DejaVuSans.ttf", 24.0f,&font_cfg, tmranges);
	fontSmall = io.Fonts->AddFontFromFileTTF("romfs:/DejaVuSans.ttf", 16.0f,&font_cfg, tmranges);
#else
	io.Fonts->AddFontFromFileTTF("./romfs/DejaVuSans.ttf", 24.0f,&font_cfg, tmranges);
	fontSmall = io.Fonts->AddFontFromFileTTF("./romfs/DejaVuSans.ttf", 16.0f,&font_cfg, tmranges);
#endif
		
		
	io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height, &bpp);
	io.Fonts->Flags |= ImFontAtlasFlags_NoPowerOfTwoHeight;
	io.Fonts->Build();
	
	
	printf("Loading Textures\n");
#ifdef NXMP_SWITCH
	Utility::TxtLoadFromFile("romfs:/sdcard.png",&nxmpicons.SdCardTexture.id,&nxmpicons.SdCardTexture.width,&nxmpicons.SdCardTexture.height);
	Utility::TxtLoadFromFile("romfs:/usb.png",&nxmpicons.UsbTexture.id,&nxmpicons.UsbTexture.width,&nxmpicons.UsbTexture.height);
	Utility::TxtLoadFromFile("romfs:/network.png",&nxmpicons.NetworkTexture.id,&nxmpicons.NetworkTexture.width,&nxmpicons.NetworkTexture.height);
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
	
#else
	Utility::TxtLoadFromFile("./romfs/sdcard.png",&nxmpicons.SdCardTexture.id,&nxmpicons.SdCardTexture.width,&nxmpicons.SdCardTexture.height);
	Utility::TxtLoadFromFile("./romfs/usb.png",&nxmpicons.UsbTexture.id,&nxmpicons.UsbTexture.width,&nxmpicons.UsbTexture.height);
	Utility::TxtLoadFromFile("./romfs/network.png",&nxmpicons.NetworkTexture.id,&nxmpicons.NetworkTexture.width,&nxmpicons.NetworkTexture.height);
	Utility::TxtLoadFromFile("./romfs/enigma2.png",&nxmpicons.Enigma2Texture.id,&nxmpicons.Enigma2Texture.width,&nxmpicons.Enigma2Texture.height);
	Utility::TxtLoadFromFile("./romfs/folder.png",&nxmpicons.FolderTexture.id,&nxmpicons.FolderTexture.width,&nxmpicons.FolderTexture.height);
	Utility::TxtLoadFromFile("./romfs/file.png",&nxmpicons.FileTexture.id,&nxmpicons.FileTexture.width,&nxmpicons.FileTexture.height);
	Utility::TxtLoadFromFile("./romfs/info.png",&nxmpicons.InfoTexture.id,&nxmpicons.InfoTexture.width,&nxmpicons.InfoTexture.height);
	Utility::TxtLoadFromFile("./romfs/playlist.png",&nxmpicons.PlaylistTexture.id,&nxmpicons.PlaylistTexture.width,&nxmpicons.PlaylistTexture.height);
	Utility::TxtLoadFromFile("./romfs/settings.png",&nxmpicons.SettingsTexture.id,&nxmpicons.SettingsTexture.width,&nxmpicons.SettingsTexture.height);
	Utility::TxtLoadFromFile("./romfs/ffmpeg.png",&nxmpicons.FFMPEGTexture.id,&nxmpicons.FFMPEGTexture.width,&nxmpicons.FFMPEGTexture.height);
	Utility::TxtLoadFromFile("./romfs/http.png",&nxmpicons.HTTPTexture.id,&nxmpicons.HTTPTexture.width,&nxmpicons.HTTPTexture.height);
	Utility::TxtLoadFromFile("./romfs/ftp.png",&nxmpicons.FTPTexture.id,&nxmpicons.FTPTexture.width,&nxmpicons.FTPTexture.height);
	Utility::TxtLoadFromFile("./romfs/sftp.png",&nxmpicons.SFTPTexture.id,&nxmpicons.SFTPTexture.width,&nxmpicons.SFTPTexture.height);
	Utility::TxtLoadFromFile("./romfs/smb.png",&nxmpicons.SMBTexture.id,&nxmpicons.SMBTexture.width,&nxmpicons.SMBTexture.height);
	Utility::TxtLoadFromFile("./romfs/nfs.png",&nxmpicons.NFSTexture.id,&nxmpicons.NFSTexture.width,&nxmpicons.NFSTexture.height);
	Utility::TxtLoadFromFile("./romfs/upnp.png",&nxmpicons.UPNPTexture.id,&nxmpicons.UPNPTexture.width,&nxmpicons.UPNPTexture.height);
	Utility::TxtLoadFromFile("./romfs/mpv.png",&nxmpicons.MPVTexture.id,&nxmpicons.MPVTexture.width,&nxmpicons.MPVTexture.height);
	Utility::TxtLoadFromFile("./romfs/exit.png",&nxmpicons.ExitTexture.id,&nxmpicons.ExitTexture.width,&nxmpicons.ExitTexture.height);
	Utility::TxtLoadFromFile("./romfs/nxmp-banner.jpg",&nxmpicons.NXMPBannerTexture.id,&nxmpicons.NXMPBannerTexture.width,&nxmpicons.NXMPBannerTexture.height);
	Utility::TxtLoadFromFile("./romfs/player/play.png",&nxmpicons.PlayIcon.id,&nxmpicons.PlayIcon.width,&nxmpicons.PlayIcon.height);
	Utility::TxtLoadFromFile("./romfs/player/stop.png",&nxmpicons.StopIcon.id,&nxmpicons.StopIcon.width,&nxmpicons.StopIcon.height);
	Utility::TxtLoadFromFile("./romfs/player/pause.png",&nxmpicons.PauseIcon.id,&nxmpicons.PauseIcon.width,&nxmpicons.PauseIcon.height);
	Utility::TxtLoadFromFile("./romfs/player/mute.png",&nxmpicons.MuteIcon.id,&nxmpicons.MuteIcon.width,&nxmpicons.MuteIcon.height);
	Utility::TxtLoadFromFile("./romfs/player/volume.png",&nxmpicons.VolumeIcon.id,&nxmpicons.VolumeIcon.width,&nxmpicons.VolumeIcon.height);
	Utility::TxtLoadFromFile("./romfs/player/loop.png",&nxmpicons.LoopIcon.id,&nxmpicons.LoopIcon.width,&nxmpicons.LoopIcon.height);
	Utility::TxtLoadFromFile("./romfs/player/noloop.png",&nxmpicons.NoLoopIcon.id,&nxmpicons.NoLoopIcon.width,&nxmpicons.NoLoopIcon.height);
	
	
#endif	

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
	
#ifdef __SWITCH__

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

    printf("SWITCHRenderer(): clocks: cpu=%i, gpu=%i, emc=%i\n",
    SwitchSys::stock_cpu_clock, SwitchSys::stock_gpu_clock, SwitchSys::stock_emc_clock);
#endif

	GUI::initMpv();
	
	GUI::RenderLoop();
	printf("Ending Render Loop\n");

#ifdef __SWITCH__
	SwitchSys::defaultClock(SwitchSys::stock_cpu_clock, SwitchSys::stock_gpu_clock, SwitchSys::stock_emc_clock);                
#endif

	delete libmpv;
	libmpv = nullptr;
	printf("Ending MPV\n");
	
	
	if(localdir != nullptr){
		delete localdir;
		localdir = nullptr;
	}
#ifdef NXMP_USBSUPPORT
	if(usbmounter != nullptr){
		delete usbmounter;
		usbmounter = nullptr;
	}
#endif
#ifdef NXMP_NETWORKSUPPORT
	if(ftpdir != nullptr){
		delete ftpdir;
		ftpdir = nullptr;
	}
	if(httpdir != nullptr){
		delete httpdir;
		httpdir = nullptr;
	}
#endif
#ifdef NXMP_ENIGMASUPPORT
	if(enigma2 != nullptr){
		delete enigma2;
		enigma2 = nullptr;
	}
#endif
#ifdef NXMP_UPNPSUPPORT
	if(nxupnp != nullptr){
		delete nxupnp;
		nxupnp = nullptr;
	}
#endif
	
	if(sqlitedb != nullptr){
		delete sqlitedb;
		sqlitedb = nullptr;
	}
	
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(context);
	deinitTextures();
    }
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_Quit();
	
	
	
	printf("Exit Services\n");
	
#ifdef NXMP_SWITCH
	if (hosversionBefore(8, 0, 0)) {
    pcvExit();
    } else {
    clkrstExit();
    }
	ncmExit();
	plExit();
	romfsExit();
    socketExit();
	appletUnlockExit();
#endif
    return 0;	
}
