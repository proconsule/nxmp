
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


#define NDEBUG 1

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

Tex SdCardTexture;
Tex NetworkTexture;
Tex Enigma2Texture;
Tex PlaylistTexture;
Tex UsbTexture;
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

ImFont* fontSmall;

shaderMania* shadermania = nullptr;

const GLuint WIDTH = 1280, HEIGHT = 720;

std::string nxmpTitle = std::string("NXMP v") + std::to_string(VERSION_MAJOR) + std::string(".") + std::to_string(VERSION_MINOR) + std::string(".") + std::to_string(VERSION_MICRO);

void deinitTextures(){

	glDeleteTextures(1, &SdCardTexture.id);
	glDeleteTextures(1, &UsbTexture.id);
	glDeleteTextures(1, &NetworkTexture.id);
	glDeleteTextures(1, &Enigma2Texture.id);
	glDeleteTextures(1, &FileTexture.id);
	glDeleteTextures(1, &PlaylistTexture.id);
	glDeleteTextures(1, &InfoTexture.id);
	glDeleteTextures(1, &FFMPEGTexture.id);
	glDeleteTextures(1, &HTTPTexture.id);
	glDeleteTextures(1, &FTPTexture.id);
	glDeleteTextures(1, &SFTPTexture.id);
	glDeleteTextures(1, &SMBTexture.id);
	glDeleteTextures(1, &MPVTexture.id);
	glDeleteTextures(1, &ExitTexture.id);
	glDeleteTextures(1, &NXMPBannerTexture.id);
	glDeleteTextures(1, &PlayIcon.id);
	glDeleteTextures(1, &StopIcon.id);
	glDeleteTextures(1, &PauseIcon.id);
	glDeleteTextures(1, &MuteIcon.id);
	glDeleteTextures(1, &VolumeIcon.id);
	glDeleteTextures(1, &LoopIcon.id);
	glDeleteTextures(1, &NoLoopIcon.id);
	
}


static bool init() {
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
	
	
	//sambaDir * sambadir = new sambaDir("smb://ceco:1@10.34.0.236/Users/",playlist);
	//sambadir->DirList("Ceco/Games",configini->getshowHidden(false),Utility::getMediaExtensions());
	//return 0;
	
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
        //PlFontData standard, fonts_ext;
		//if (R_FAILED(ret = plGetSharedFontByType(&standard, PlSharedFontType_Standard))) {
		//	printf("plGetSharedFontByType(PlSharedFontType_Standard) failed: 0x%x\n", ret);
		//	return ret;
		//}
		
		 //plGetSharedFontByType(&fonts_ext, PlSharedFontType_NintendoExt);

		
		unsigned char *pixels = nullptr;
		int width = 0, height = 0, bpp = 0;
		ImFontConfig font_cfg;
		
		//font_cfg.FontDataOwnedByAtlas = false;
		printf("Loading TTF\n");
		
		
		
#ifdef NXMP_SWITCH
		io.Fonts->AddFontFromFileTTF("romfs:/DejaVuSans.ttf", 24.0f,&font_cfg);
		fontSmall = io.Fonts->AddFontFromFileTTF("romfs:/DejaVuSans.ttf", 16.0f,&font_cfg);
#else
		io.Fonts->AddFontFromFileTTF("./romfs/DejaVuSans.ttf", 24.0f,&font_cfg);
		fontSmall = io.Fonts->AddFontFromFileTTF("./romfs/DejaVuSans.ttf", 16.0f,&font_cfg);
#endif
		//font_cfg.MergeMode = true;
		//io.Fonts->AddFontFromFileTTF("./romfs/DejaVuSans.ttf", 24.0f,&font_cfg, io.Fonts->GetGlyphRangesJapanese());
		//io.Fonts->AddFontFromMemoryTTF(standard.address, standard.size, 28.0f, &font_cfg, io.Fonts->GetGlyphRangesJapanese());
		/*
		    static const ImWchar ranges[] =
                {
                    0xE000, 0xE06B,
                    0xE070, 0xE07E,
                    0xE080, 0xE099,
                    0xE0A0, 0xE0BA,
                    0xE0C0, 0xE0D6,
                    0xE0E0, 0xE0F5,
                    0xE100, 0xE105,
                    0xE110, 0xE116,
                    0xE121, 0xE12C,
                    0xE130, 0xE13C,
                    0xE140, 0xE14D,
                    0xE150, 0xE153,
                    0,
                };
		*/		
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
	//printf("Loading Extended Chars\n");
    //io.Fonts->AddFontFromMemoryTTF (fonts_ext.address, fonts_ext.size, 24.0f, &font_cfg, ranges);
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
	Utility::TxtLoadFromFile("romfs:/sdcard.png",&SdCardTexture.id,&SdCardTexture.width,&SdCardTexture.height);
	Utility::TxtLoadFromFile("romfs:/usb.png",&UsbTexture.id,&UsbTexture.width,&UsbTexture.height);
	Utility::TxtLoadFromFile("romfs:/network.png",&NetworkTexture.id,&NetworkTexture.width,&NetworkTexture.height);
	Utility::TxtLoadFromFile("romfs:/enigma2.png",&Enigma2Texture.id,&Enigma2Texture.width,&Enigma2Texture.height);
	Utility::TxtLoadFromFile("romfs:/folder.png",&FolderTexture.id,&FolderTexture.width,&FolderTexture.height);
	Utility::TxtLoadFromFile("romfs:/file.png",&FileTexture.id,&FileTexture.width,&FileTexture.height);
	Utility::TxtLoadFromFile("romfs:/playlist.png",&PlaylistTexture.id,&PlaylistTexture.width,&PlaylistTexture.height);
	Utility::TxtLoadFromFile("romfs:/info.png",&InfoTexture.id,&InfoTexture.width,&InfoTexture.height);
	Utility::TxtLoadFromFile("romfs:/settings.png",&SettingsTexture.id,&SettingsTexture.width,&SettingsTexture.height);
	Utility::TxtLoadFromFile("romfs:/ffmpeg.png",&FFMPEGTexture.id,&FFMPEGTexture.width,&FFMPEGTexture.height);
	Utility::TxtLoadFromFile("romfs:/http.png",&HTTPTexture.id,&HTTPTexture.width,&HTTPTexture.height);
	Utility::TxtLoadFromFile("romfs:/ftp.png",&FTPTexture.id,&FTPTexture.width,&FTPTexture.height);
	Utility::TxtLoadFromFile("romfs:/sftp.png",&SFTPTexture.id,&SFTPTexture.width,&SFTPTexture.height);
	Utility::TxtLoadFromFile("romfs:/smb.png",&SMBTexture.id,&SMBTexture.width,&SMBTexture.height);
	Utility::TxtLoadFromFile("romfs:/nfs.png",&NFSTexture.id,&NFSTexture.width,&NFSTexture.height);
	Utility::TxtLoadFromFile("romfs:/upnp.png",&UPNPTexture.id,&UPNPTexture.width,&UPNPTexture.height);
	Utility::TxtLoadFromFile("romfs:/mpv.png",&MPVTexture.id,&MPVTexture.width,&MPVTexture.height);
	Utility::TxtLoadFromFile("romfs:/exit.png",&ExitTexture.id,&ExitTexture.width,&ExitTexture.height);
	Utility::TxtLoadFromFile("romfs:/nxmp-banner.jpg",&NXMPBannerTexture.id,&NXMPBannerTexture.width,&NXMPBannerTexture.height);
	Utility::TxtLoadFromFile("romfs:/player/play.png",&PlayIcon.id,&PlayIcon.width,&PlayIcon.height);
	Utility::TxtLoadFromFile("romfs:/player/stop.png",&StopIcon.id,&StopIcon.width,&StopIcon.height);
	Utility::TxtLoadFromFile("romfs:/player/pause.png",&PauseIcon.id,&PauseIcon.width,&PauseIcon.height);
	Utility::TxtLoadFromFile("romfs:/player/mute.png",&MuteIcon.id,&MuteIcon.width,&MuteIcon.height);
	Utility::TxtLoadFromFile("romfs:/player/volume.png",&VolumeIcon.id,&VolumeIcon.width,&VolumeIcon.height);
	Utility::TxtLoadFromFile("romfs:/player/loop.png",&LoopIcon.id,&LoopIcon.width,&LoopIcon.height);
	Utility::TxtLoadFromFile("romfs:/player/noloop.png",&NoLoopIcon.id,&NoLoopIcon.width,&NoLoopIcon.height);
	
#else
	Utility::TxtLoadFromFile("./romfs/sdcard.png",&SdCardTexture.id,&SdCardTexture.width,&SdCardTexture.height);
	Utility::TxtLoadFromFile("./romfs/usb.png",&UsbTexture.id,&UsbTexture.width,&UsbTexture.height);
	Utility::TxtLoadFromFile("./romfs/network.png",&NetworkTexture.id,&NetworkTexture.width,&NetworkTexture.height);
	Utility::TxtLoadFromFile("./romfs/enigma2.png",&Enigma2Texture.id,&Enigma2Texture.width,&Enigma2Texture.height);
	Utility::TxtLoadFromFile("./romfs/folder.png",&FolderTexture.id,&FolderTexture.width,&FolderTexture.height);
	Utility::TxtLoadFromFile("./romfs/file.png",&FileTexture.id,&FileTexture.width,&FileTexture.height);
	Utility::TxtLoadFromFile("./romfs/info.png",&InfoTexture.id,&InfoTexture.width,&InfoTexture.height);
	Utility::TxtLoadFromFile("./romfs/playlist.png",&PlaylistTexture.id,&PlaylistTexture.width,&PlaylistTexture.height);
	Utility::TxtLoadFromFile("./romfs/settings.png",&SettingsTexture.id,&SettingsTexture.width,&SettingsTexture.height);
	Utility::TxtLoadFromFile("./romfs/ffmpeg.png",&FFMPEGTexture.id,&FFMPEGTexture.width,&FFMPEGTexture.height);
	Utility::TxtLoadFromFile("./romfs/http.png",&HTTPTexture.id,&HTTPTexture.width,&HTTPTexture.height);
	Utility::TxtLoadFromFile("./romfs/ftp.png",&FTPTexture.id,&FTPTexture.width,&FTPTexture.height);
	Utility::TxtLoadFromFile("./romfs/sftp.png",&SFTPTexture.id,&SFTPTexture.width,&SFTPTexture.height);
	Utility::TxtLoadFromFile("./romfs/smb.png",&SMBTexture.id,&SMBTexture.width,&SMBTexture.height);
	Utility::TxtLoadFromFile("./romfs/nfs.png",&NFSTexture.id,&NFSTexture.width,&NFSTexture.height);
	Utility::TxtLoadFromFile("./romfs/upnp.png",&UPNPTexture.id,&UPNPTexture.width,&UPNPTexture.height);
	Utility::TxtLoadFromFile("./romfs/mpv.png",&MPVTexture.id,&MPVTexture.width,&MPVTexture.height);
	Utility::TxtLoadFromFile("./romfs/exit.png",&ExitTexture.id,&ExitTexture.width,&ExitTexture.height);
	Utility::TxtLoadFromFile("./romfs/nxmp-banner.jpg",&NXMPBannerTexture.id,&NXMPBannerTexture.width,&NXMPBannerTexture.height);
	Utility::TxtLoadFromFile("./romfs/player/play.png",&PlayIcon.id,&PlayIcon.width,&PlayIcon.height);
	Utility::TxtLoadFromFile("./romfs/player/stop.png",&StopIcon.id,&StopIcon.width,&StopIcon.height);
	Utility::TxtLoadFromFile("./romfs/player/pause.png",&PauseIcon.id,&PauseIcon.width,&PauseIcon.height);
	Utility::TxtLoadFromFile("./romfs/player/mute.png",&MuteIcon.id,&MuteIcon.width,&MuteIcon.height);
	Utility::TxtLoadFromFile("./romfs/player/volume.png",&VolumeIcon.id,&VolumeIcon.width,&VolumeIcon.height);
	Utility::TxtLoadFromFile("./romfs/player/loop.png",&LoopIcon.id,&LoopIcon.width,&LoopIcon.height);
	Utility::TxtLoadFromFile("./romfs/player/noloop.png",&NoLoopIcon.id,&NoLoopIcon.width,&NoLoopIcon.height);
	
	
#endif	
	
	GUI::initMpv();
	
	GUI::RenderLoop();
	printf("Ending Render Loop\n");
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
	ncmExit();
	plExit();
	romfsExit();
    socketExit();
	appletUnlockExit();
#endif
    return 0;	
}
