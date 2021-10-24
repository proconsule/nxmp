#include <switch.h>

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

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "SimpleIni.h"

static bool init();

SDL_Window *window;
SDL_GLContext context;
Mpv *mpv;
HTTPDir *httpdir;
Config *configini;
Enigma2 *enigma2;
uint32_t wakeup_on_mpv_render_update;
uint32_t wakeup_on_mpv_events;

mpv_opengl_fbo fbo;
mpv_render_param params[3];
int __fbo_one = 1;
bool renderloopdone = false;

Tex SdCardTexture;
Tex NetworkTexture;
Tex Enigma2Texture;
Tex InfoTexture;
Tex SettingsTexture;


Tex FolderTexture;
Tex FileTexture;

Tex FTPTexture;
Tex HTTPTexture;

Tex FFMPEGTexture;
Tex MPVTexture;

Tex NXMPBannerTexture;
Tex ExitTexture;

const GLuint WIDTH = 1280, HEIGHT = 720;

std::string nxmpTitle = std::string("NXMP v") + std::to_string(VERSION_MAJOR) + std::string(".") + std::to_string(VERSION_MINOR) + std::string(".") + std::to_string(VERSION_MICRO);


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


int main() {
	
	socketInitializeDefault();
#ifdef NDEBUG
	nxlinkStdio();
#endif	
	printf("Loading Config\n");
	
	configini = new Config("config.ini");
	
	/*
	if(rc<0){
		consoleInit(NULL);
		padConfigureInput(1, HidNpadStyleSet_NpadStandard);
		PadState pad;
		padInitializeDefault(&pad);

		printf("Failed to load config.ini file, press + to exit\n");

		while(appletMainLoop())
		{
			padUpdate(&pad);
			u64 kDown = padGetButtonsDown(&pad);

			if (kDown & HidNpadButton_Plus) break;
			consoleUpdate(NULL);
		}

			consoleExit(NULL);
			return 0;
	};
	*/
	//std::string pippo = (char *)configini.GetValue("Enigma2", "e2address", "default");
	//urlschema testschema = Utility::parseUrl(pippo);
	
	Result ret;
	if (R_FAILED(ret = romfsInit())) {
		printf("romfsInit() failed: 0x%x\n", ret);
		return ret;
	}
	printf("Init GUI\n");
	if ( init() ) {
		
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
		io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		io.IniFilename = nullptr;
		io.MouseDrawCursor = false;
        
        ImGui::StyleColorsDark();
		printf("Init MPV\n");
		GUI::initMpv();
		printf("Init SDL\n");
		ImGui_ImplSDL2_InitForOpenGL(window, context);
        printf("Init OPENGL\n");
		ImGui_ImplOpenGL3_Init("#version 330 core");
		
		
		
		plInitialize(PlServiceType_System);
		if (R_FAILED(ret = nifmInitialize(NifmServiceType_User))) {
			printf("nifmInitialize(NifmServiceType_User) failed: 0x%x\n", ret);
			return ret;
		}
		printf("Init Fonts\n");
        Result ret = 0;
		//PlFontData standard, fonts_ext;
		//if (R_FAILED(ret = plGetSharedFontByType(&standard, PlSharedFontType_Standard))) {
		//	printf("plGetSharedFontByType(PlSharedFontType_Standard) failed: 0x%x\n", ret);
		//	return ret;
		//}
		
		 //plGetSharedFontByType(&fonts_ext, PlSharedFontType_NintendoExt);

		
		unsigned char *pixels = nullptr;
		int width = 0, height = 0, bpp = 0;
		ImFontConfig font_cfg;
		
		font_cfg.FontDataOwnedByAtlas = false;
		printf("Loading TTF\n");
		io.Fonts->AddFontFromFileTTF("romfs:/DejaVuSans.ttf", 24.0f,&font_cfg, io.Fonts->GetGlyphRangesDefault());
		font_cfg.MergeMode = true;
		//io.Fonts->AddFontFromMemoryTTF(standard.address, standard.size, 28.0f, &font_cfg, io.Fonts->GetGlyphRangesJapanese());
		
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
				
			static const ImWchar tmranges[] =
                {
					0x2019, 0x2019,
                    0x2122, 0x2122,
					0x2713, 0x2713,
					0x2714, 0x2714,
					0x2716, 0x2716,
					0,
                };
	
	printf("Loading Extended Chars\n");
    //io.Fonts->AddFontFromMemoryTTF (fonts_ext.address, fonts_ext.size, 24.0f, &font_cfg, ranges);
	io.Fonts->AddFontFromFileTTF("romfs:/DejaVuSans.ttf", 24.0f,&font_cfg, tmranges);
		
		
	io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height, &bpp);
	io.Fonts->Flags |= ImFontAtlasFlags_NoPowerOfTwoHeight;
	io.Fonts->Build();
	
	printf("Opening SD Card\n");
	devices[0] = *fsdevGetDeviceFileSystem("sdmc");
	fs = &devices[0];
	
	printf("Loading Textures\n");
	Utility::TxtLoadPNGFromFile("romfs:/sdcard.png",&SdCardTexture.id,&SdCardTexture.width,&SdCardTexture.height);
	Utility::TxtLoadPNGFromFile("romfs:/network.png",&NetworkTexture.id,&NetworkTexture.width,&NetworkTexture.height);
	Utility::TxtLoadPNGFromFile("romfs:/enigma2.png",&Enigma2Texture.id,&Enigma2Texture.width,&Enigma2Texture.height);
	Utility::TxtLoadPNGFromFile("romfs:/folder.png",&FolderTexture.id,&FolderTexture.width,&FolderTexture.height);
	Utility::TxtLoadPNGFromFile("romfs:/file.png",&FileTexture.id,&FileTexture.width,&FileTexture.height);
	Utility::TxtLoadPNGFromFile("romfs:/info.png",&InfoTexture.id,&InfoTexture.width,&InfoTexture.height);
	Utility::TxtLoadPNGFromFile("romfs:/settings.png",&SettingsTexture.id,&SettingsTexture.width,&SettingsTexture.height);
	
	Utility::TxtLoadPNGFromFile("romfs:/ffmpeg.png",&FFMPEGTexture.id,&FFMPEGTexture.width,&FFMPEGTexture.height);
	Utility::TxtLoadPNGFromFile("romfs:/http.png",&HTTPTexture.id,&HTTPTexture.width,&HTTPTexture.height);
	Utility::TxtLoadPNGFromFile("romfs:/ftp.png",&FTPTexture.id,&FTPTexture.width,&FTPTexture.height);
	
	Utility::TxtLoadPNGFromFile("romfs:/mpv.png",&MPVTexture.id,&MPVTexture.width,&MPVTexture.height);
	Utility::TxtLoadPNGFromFile("romfs:/exit.png",&ExitTexture.id,&ExitTexture.width,&ExitTexture.height);
	
	
	Utility::TxtLoadJPGFromFile("romfs:/nxmp-banner.jpg",&NXMPBannerTexture.id,&NXMPBannerTexture.width,&NXMPBannerTexture.height);
	
	
	enigma2 = new Enigma2();
	httpdir = new HTTPDir();
	
	
	
	
	GUI::RenderLoop();
	printf("Ending Render Loop\n");
		//return ret;
	delete(mpv);
	delete(httpdir);
	delete(enigma2);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(context);
    }
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_Quit();
	
	
	
	//nsExit();
	printf("Exit Services\n");
	ncmExit();
	plExit();
	romfsExit();
    socketExit();
    return 0;	
}
