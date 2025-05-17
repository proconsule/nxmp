#include <stdio.h>
#include <ctype.h>
#include <utility>

#include "nxmp-render.h"
#include "imgui_nx.h"
#include "iniparser.h"
#include "imgloader.h"
#include "gui.h"
#include "utils.h"
#include "localfiles.h"
#include "networkShareClass.h"
#include "HTTPDir.h"
#include "NX-UPNP.h"

#include "nxmp-i18n.h"


#include "imgui.h"

#include "SimpleIni.h"
#include "SQLiteDB.h"
#include "eqpreset.h"
#include "mtpclass.h"

#include "playlist.h"

#include "SwitchSys.h"

#include "stats.h"
#include "logger.h"

#include <switch.h>

#include "globals.h"

#define NDEBUG 0


extern "C" u32 __nx_applet_exit_mode, __nx_nv_service_type, __nx_nv_transfermem_size;
bool                g_application_mode;

std::string nxmpTitle = std::string("NXMP v") + std::to_string(VERSION_MAJOR) + std::string(".") + std::to_string(VERSION_MINOR) + std::string(".") + std::to_string(VERSION_MICRO);


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

CIniParser *configini = nullptr;
CImgLoader *imgloader = nullptr;

EQPreset *eqpreset = nullptr;
SQLiteDB *sqlitedb = nullptr;
CMTP *mtp = nullptr;

Playlist *playlist = nullptr;
CStats *nxmpstats = nullptr;
CConsoleWindow *ConsoleWindow = nullptr;

NXMPRenderer *Renderer = nullptr;


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



int64_t playercachesec =  0;
int64_t playercachesize = 0;


bool slaveplayer = false;
std::string slaveplayer_file = "";


ImVec4 Text_color = ImVec4(255, 255, 255, 1.00f);
ImVec4 Active_color = ImVec4(0, 255, 203, 1.00f);
ImVec4 Disabled_color = ImVec4(0.41f, 0.40f, 0.40f, 1.00f);
ImVec4 Window_Bg_color = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
ImVec4 OptsTab_Bg_color = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
ImVec4 Popup_Bg_color = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
ImVec4 NavHover_color = ImVec4(0.0f, 0.0f, 0.0f, 0.20f);
ImVec4 HeaderHover_color = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
ImVec4 Button_color = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);
ImVec4 ButtonActive_color = ImVec4(0, 255, 203, 0.30f);

float CurrentVolume = 0.0f;


void SetDarkTheme(){
		Text_color = ImVec4(255, 255, 255, 1.00f);
		Active_color = ImVec4(0, 255, 203, 1.00f);
		Disabled_color = ImVec4(0.41f, 0.40f, 0.40f, 1.00f);
		Window_Bg_color = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
		OptsTab_Bg_color = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
		Popup_Bg_color = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
		NavHover_color = ImVec4(0.0f, 0.0f, 0.0f, 0.20f);
		HeaderHover_color = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
		Button_color = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);
		ButtonActive_color = ImVec4(0, 255, 203, 0.30f);
		
		ImGuiStyle* style = &ImGui::GetStyle();
		style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.62f);
		style->Colors[ImGuiCol_Text] = Text_color;
		
	}
	void SetLightTheme(){
		Text_color = ImVec4(0, 0, 0, 1.00f);
		Active_color = ImVec4(0, 0, 255, 1.00f);
		Disabled_color = ImVec4(0.41f, 0.40f, 0.40f, 1.00f);
		Window_Bg_color = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
		OptsTab_Bg_color = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
		Popup_Bg_color = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
		NavHover_color = ImVec4(1.0f, 1.0f, 1.0f, 0.20f);
		HeaderHover_color = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
		Button_color = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);
		ButtonActive_color = ImVec4(0, 0, 255, 0.30f);
		
		ImGuiStyle* style = &ImGui::GetStyle();
		style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.62f);
		style->Colors[ImGuiCol_Text] = Text_color;
		
	}

void SetColorTheme(int themecolor){
		
		ColorSetId currentTheme;
		Result rc = setsysInitialize();
		if (R_SUCCEEDED(rc)) {
		setsysGetColorSetId(&currentTheme);
		}
		
		if(themecolor == 0){
			SetDarkTheme();
		}else if(themecolor == 1){
			SetLightTheme();
		} else{
			if(currentTheme == 1){
				SetDarkTheme();
			}else{
				SetLightTheme();
			}
		}
		setsysExit();
	}


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
		FilePopupTextScroller = nullptr;
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
	
	if(configini != nullptr){
		delete configini;
		configini = nullptr;
	}
	
}



int main(int argc, const  char **argv) {
	

/**************** This init taken from SwitchWave https://github.com/averne/SwitchWave  make it applet mode friendly  ************************/	

	svcSetThreadPriority(CUR_THREAD_HANDLE, 0x20);
	
	appletLockExit();
	auto at = appletGetAppletType();
    g_application_mode = at == AppletType_Application || at == AppletType_SystemApplication;
	__nx_nv_service_type     = NvServiceType_Factory;
    __nx_nv_transfermem_size = (g_application_mode ? 16 : 3) * 0x100000;
	
	
/*************************************************************************************/
	
	romfsInit();
	auto socket_conf = *socketGetDefaultInitConfig();
    socket_conf.bsd_service_type = BsdServiceType_Auto;
    socketInitialize(&socket_conf);
	
	
	
	


#ifdef NDEBUG
	nxlinkStdio();
#endif	
	NXLOG::DEBUGLOG("Starting NXMP\n");
	


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
	
	
	if(argc>1){
		slaveplayer=true;
		NXLOG::DEBUGLOG("Opening File: %s\n",argv[1]);
		slaveplayer_file = argv[1];
	}
	
	
	NXLOG::loglevel = 0;
	
	nxlangs::Init_Langs();
	
	NXLOG::DEBUGLOG("Loading Config\n");
	
	configini = new CIniParser("config.ini");
	NXLOG::loglevel = configini->getLogLevel();
	if(NXLOG::loglevel >0){
		configini->PrintConfig();
	}
	NXLOG::loglevel = 2;
	
	nxlangs::selectedlang = configini->getConfigLangId(false);
	
	appletInitializeGamePlayRecording();
    appletSetWirelessPriorityMode(AppletWirelessPriorityMode_OptimizedForWlan);
	
	
	if(configini->getDbActive(false) && !slaveplayer){
		sqlitedb = new SQLiteDB(configini->getDbPath());
		dbUpdated = sqlitedb->dbwasUpdated();
	}
	
	Renderer = new NXMPRenderer();
	Renderer->Init_ImGui();
	if(!Renderer->initialize()){
		
	}
	Renderer->Init_ImGui_deko3d();
	NXLOG::DEBUGLOG("Renderer Init ok\n");
	

	imgui::nx::setEnableTouch(configini->getTouchEnable(false));
		
	
	eqpreset = new EQPreset("eqpresets.ini");
	
	playlist = new Playlist();
	
	
	
	audctlInitialize(); // Master Volume Test
	audctlGetSystemOutputMasterVolume(&CurrentVolume); 
	
	
	
	
	NXLOG::DEBUGLOG("Imgui Init OK\n");
	
	std::vector<std::string> extensionlist = configini->getConfigExtensions();
	extensionlist.push_back(".pdf");
		
	Utility::setMediaExtensions(extensionlist);
	
	imgloader = new CImgLoader();
	imgloader->Renderer = Renderer;
	imgloader->LoadBaseTextures("romfs:");
	
	
		
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

	
	SetColorTheme(configini->getThemeColor(false));
	
	
	libmpv = new libMpv("mpv");
	Renderer->create_mpv_render_context(libmpv);
	NXLOG::DEBUGLOG("MPV Context Done\n");
	GUI::GUILESS();
	
	
	
	if(nxmpstats == nullptr){
		nxmpstats = new CStats();
		nxmpstats->StartThreads();
	}	
	
	
	
	NXLOG::DEBUGLOG("STATS INIT OK\n");
	GUI::RenderLoop();
	
	
	if(configini->getExitMode(false) == 1){
		app_exit_mode = 0;
	}else if(configini->getExitMode(false) == 2){
		app_exit_mode = 1;
	}
	
	SwitchSys::defaultClock(SwitchSys::stock_cpu_clock, SwitchSys::stock_gpu_clock, SwitchSys::stock_emc_clock);                

	__nx_applet_exit_mode = app_exit_mode;
	
	if(imgloader != nullptr){
		delete imgloader;
		imgloader = nullptr;
	}

	
	NXLOG::DEBUGLOG("Destroy Renderer\n");
	Renderer->destroy_mpv_render_context();
	delete libmpv;
	libmpv = nullptr;
	NXLOG::DEBUGLOG("Ending MPV\n");	
	delete Renderer;	
	DeallocateExtern();	
	
	
	if(nxmpstats != nullptr){
		delete nxmpstats;
		nxmpstats = nullptr;
	}
	
	audctlExit();


	if (hosversionBefore(8, 0, 0)) {
		pcvExit();
    } else {
		clkrstExit();
    }

    nvExit();
	
	romfsExit();
	NXLOG::DEBUGLOG("NXMP End\n");
    socketExit();
	appletUnlockExit();
	
    return 0;
}

