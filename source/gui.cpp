#include <algorithm>
#include <iostream>

#include "platforms.h"

#include "gui.h"
#include "localfiles.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "SwitchSys.h"


#ifdef NXMP_SWITCH
using namespace c2d;
#endif

MenuItem item;

namespace GUI {
	
	const int JOYSTICK_DEAD_ZONE = 8000;
	const int JOYSTICK_EXTENDED_DEAD_ZONE = 32726;
	
	
	
	
	static void on_mpv_events(void *ctx)
	{
		SDL_Event event = {.type = wakeup_on_mpv_events};
		SDL_PushEvent(&event);
	}
	
	void initMpv(){
		libmpv = new libMpv("mpv");
		wakeup_on_mpv_render_update = SDL_RegisterEvents(1);
		wakeup_on_mpv_events        = SDL_RegisterEvents(1);
		mpv_set_wakeup_callback(libmpv->getHandle(), on_mpv_events, NULL);
		mpv_render_context_set_update_callback(libmpv->getContext(), [](void *) -> void { SDL_Event event = {.type = wakeup_on_mpv_render_update}; SDL_PushEvent(&event); }, NULL);
	
	}
	
	void toggleMasterLock(){
		item.masterlock = !item.masterlock;
		if(item.masterlock){
			const char *cmd[] = {"show-text", "Masterlock Enabled","2000", NULL};
			mpv_command_async(libmpv->getHandle(), 0, cmd);
		}else{
			const char *cmd[] = {"show-text", "Masterlock Disabled","2000", NULL};
			mpv_command_async(libmpv->getHandle(), 0, cmd);
		}
		
	}

	void toggleOC(){
#ifdef NXMP_SWITCH
		item.clockoc = !item.clockoc;
		if(item.clockoc){
			const char *cmd[] = {"show-text", "Overclock Enabled","2000", NULL};
			mpv_command_async(libmpv->getHandle(), 0, cmd);
			SwitchSys::maxClock();
		}else{
			const char *cmd[] = {"show-text", "Overclock Disabled","2000", NULL};
			mpv_command_async(libmpv->getHandle(), 0, cmd);
			SwitchSys::defaultClock(SwitchSys::stock_cpu_clock, SwitchSys::stock_gpu_clock, SwitchSys::stock_emc_clock); 
		}
#endif
	}
	
	
	void HandleEvents(){
		SDL_Event event;
			while (SDL_PollEvent(&event)) {
				ImGui_ImplSDL2_ProcessEvent(&event);
#ifdef NXMP_WIN32
				if (event.type == SDL_KEYDOWN) {
					Uint8 keycode = event.key.keysym.sym;
					if(keycode == SDLK_ESCAPE){
						SDL_Event sdlevent;
						sdlevent.type = SDL_JOYBUTTONDOWN;
						sdlevent.jbutton.button = SDL_KEY_PLUS;
						SDL_PushEvent(&sdlevent);
					}
					if(keycode == 79){
						SDL_Event sdlevent;
						sdlevent.type = SDL_JOYBUTTONDOWN;
						sdlevent.jbutton.button = SDL_KEY_DRIGHT;
						SDL_PushEvent(&sdlevent);
					}
					if(keycode == 80){
						SDL_Event sdlevent;
						sdlevent.type = SDL_JOYBUTTONDOWN;
						sdlevent.jbutton.button = SDL_KEY_DLEFT;
						SDL_PushEvent(&sdlevent);
					}
					if(keycode == 82){
						SDL_Event sdlevent;
						sdlevent.type = SDL_JOYBUTTONDOWN;
						sdlevent.jbutton.button = SDL_KEY_DUP;
						SDL_PushEvent(&sdlevent);
					}
					if(keycode == 81){
						SDL_Event sdlevent;
						sdlevent.type = SDL_JOYBUTTONDOWN;
						sdlevent.jbutton.button = SDL_KEY_DDOWN;
						SDL_PushEvent(&sdlevent);
					}
					if(keycode == 32){
						SDL_Event sdlevent;
						sdlevent.type = SDL_JOYBUTTONDOWN;
						sdlevent.jbutton.button = SDL_KEY_A;
						SDL_PushEvent(&sdlevent);
					}
					if(keycode == SDLK_y){
						SDL_Event sdlevent;
						sdlevent.type = SDL_JOYBUTTONDOWN;
						sdlevent.jbutton.button = SDL_KEY_Y;
						SDL_PushEvent(&sdlevent);
					}
					if(keycode == SDLK_b){
						SDL_Event sdlevent;
						sdlevent.type = SDL_JOYBUTTONDOWN;
						sdlevent.jbutton.button = SDL_KEY_B;
						SDL_PushEvent(&sdlevent);
					}
					if(keycode == SDLK_a){
						SDL_Event sdlevent;
						sdlevent.type = SDL_JOYBUTTONDOWN;
						sdlevent.jbutton.button = SDL_KEY_A;
						SDL_PushEvent(&sdlevent);
					}
					if(keycode == SDLK_x){
						SDL_Event sdlevent;
						sdlevent.type = SDL_JOYBUTTONDOWN;
						sdlevent.jbutton.button = SDL_KEY_X;
						SDL_PushEvent(&sdlevent);
					}
					if(keycode == SDLK_r){
						SDL_Event sdlevent;
						sdlevent.type = SDL_JOYBUTTONDOWN;
						sdlevent.jbutton.button = SDL_KEY_R;
						SDL_PushEvent(&sdlevent);
					}
					if(keycode == SDLK_l){
						SDL_Event sdlevent;
						sdlevent.type = SDL_JOYBUTTONDOWN;
						sdlevent.jbutton.button = SDL_KEY_L;
						SDL_PushEvent(&sdlevent);
					}
					if(keycode == SDLK_i){
						SDL_Event sdlevent;
						sdlevent.type = SDL_JOYBUTTONDOWN;
						sdlevent.jbutton.button = SDL_KEY_RSTICK_UP;
						SDL_PushEvent(&sdlevent);
					}
					if(keycode == SDLK_k){
						SDL_Event sdlevent;
						sdlevent.type = SDL_JOYBUTTONDOWN;
						sdlevent.jbutton.button = SDL_KEY_RSTICK_DOWN;
						SDL_PushEvent(&sdlevent);
					}
					
					if(keycode == SDLK_f){
						if(fullscreen){
							SDL_SetWindowFullscreen(window,0);
							fullscreen = false;
						}else{
							SDL_SetWindowFullscreen(window,SDL_WINDOW_FULLSCREEN);
							fullscreen = true;
						}
						
					}
					
					
					if(keycode == 45){
						SDL_Event sdlevent;
						sdlevent.type = SDL_JOYBUTTONDOWN;
						sdlevent.jbutton.button = SDL_KEY_MINUS;
						SDL_PushEvent(&sdlevent);
					}
					if(keycode == SDLK_t){
						Themes *themes = new Themes();
						themes->setTheme("./themes/test1/");
						delete themes;
					}
					if(keycode == SDLK_e){
						Themes *themes = new Themes();
						themes->getThemes();
						themes->setDefault();
						delete themes;
					}
					
					
				}
				else if( event.type == SDL_MOUSEBUTTONDOWN  )
                {
					if( event.button.button == SDL_BUTTON_RIGHT) {
						isMouseSelection = true;
						SDL_Event sdlevent;
						sdlevent.type = SDL_FINGERDOWN ;
						sdlevent.tfinger.x = event.motion.x/1280.0f;
						sdlevent.tfinger.y = event.motion.y/720.0f;
						startMousex = event.motion.x;
						startMousey = event.motion.y;
						SDL_PushEvent(&sdlevent);
					}
				}
				else if( event.type == SDL_MOUSEMOTION )
                {
					if( isMouseSelection) {
						SDL_Event sdlevent;
						sdlevent.type = SDL_FINGERMOTION ;
						sdlevent.tfinger.x = event.motion.x/1280.0f;
						sdlevent.tfinger.y = event.motion.y/720.0f;
						float startfloatMousex = startMousex/1280.0f;
						float startfloatMousey = startMousey/720.0f;
						sdlevent.tfinger.dx = sdlevent.tfinger.x-startfloatMousex;
						sdlevent.tfinger.dy = sdlevent.tfinger.y-startfloatMousey;
						
						SDL_PushEvent(&sdlevent);
					}
					
				}
				else if( event.type == SDL_MOUSEBUTTONUP  )
                {
					if( event.button.button == SDL_BUTTON_RIGHT) {
						isMouseSelection = false;
						SDL_Event sdlevent;
						sdlevent.type = SDL_FINGERUP ;
						sdlevent.tfinger.x = event.motion.x/1280.0f;
						sdlevent.tfinger.y = event.motion.y/720.0f;
						startMousex = 0;
						startMousey = 0;
						SDL_PushEvent(&sdlevent);
						
					}
				}
				
				
				
#endif
				if( event.type == SDL_FINGERDOWN && configini->getTouchEnable(false) )
				{
					TOUCHCONTROLS::fingerDown(&event);
				
				}
				
				else if( event.type == SDL_FINGERMOTION && configini->getTouchEnable(false))
				{
					TOUCHCONTROLS::fingerMotion(&event);
					
				}
				
				else if( event.type == SDL_FINGERUP && configini->getTouchEnable(false))
				{
					TOUCHCONTROLS::fingerUp(&event);
					
				}
				else if(event.type == SDL_FINGERUP && !configini->getTouchEnable(false))
				{	printf("New Focus with Touch Disabled.\n");
					ImGui::SetNextWindowFocus();
				}

				if(event.type == SDL_JOYAXISMOTION){
					if( event.jaxis.axis == 3 ) {
						if( event.jaxis.value > JOYSTICK_DEAD_ZONE ) {
							if(item.state == MENU_STATE_PLAYER && !item.masterlock && item.rightmenustate == PLAYER_RIGHT_MENU_PLAYER){
								item.showVolume = true;
								ImGuiContext& g = *GImGui;
								item.VolumeHide = g.Time;
								if(libmpv->getVolume()-1>=0){
									libmpv->setVolume(libmpv->getVolume()-1,false);
								}
							}
						}
						else if( event.jaxis.value < -JOYSTICK_DEAD_ZONE ) {
							if(item.state == MENU_STATE_PLAYER && !item.masterlock && item.rightmenustate == PLAYER_RIGHT_MENU_PLAYER){
								item.showVolume = true;
								ImGuiContext& g = *GImGui;
								item.VolumeHide = g.Time;
								if(libmpv->getVolume()+1<=100){
									libmpv->setVolume(libmpv->getVolume()+1,false);
								}
							}
						}
						
					}
				
				}
				
				if (event.type == SDL_JOYBUTTONDOWN) {
					
					Uint8 button = event.jbutton.button;
					if (button == SDL_KEY_PLUS && !item.masterlock)
						renderloopdone = true;
					
					
					
					
					if (button == SDL_KEY_RSTICK_UP){
						if(item.state == MENU_STATE_PLAYER && !item.masterlock && item.rightmenustate == PLAYER_RIGHT_MENU_PLAYER){
							item.showVolume = true;
							ImGuiContext& g = *GImGui;
							item.VolumeHide = g.Time;
							if(libmpv->getVolume()+1<=100){
								libmpv->setVolume(libmpv->getVolume()+1,false);
							}
						}
					}
					
					if (button == SDL_KEY_RSTICK_DOWN){
						if(item.state == MENU_STATE_PLAYER && !item.masterlock && item.rightmenustate == PLAYER_RIGHT_MENU_PLAYER){
							item.showVolume = true;
							ImGuiContext& g = *GImGui;
							item.VolumeHide = g.Time;
							if(libmpv->getVolume()-1>=0){
								libmpv->setVolume(libmpv->getVolume()-1,false);
							}
						}
					}
					
					
					if (button == SDL_KEY_DUP){
						if(item.state == MENU_STATE_PLAYER && !item.masterlock && item.rightmenustate == PLAYER_RIGHT_MENU_PLAYER && item.popupstate == POPUP_STATE_NONE){
							
						}
					}
					
					if (button == SDL_KEY_DDOWN){
						if(item.state == MENU_STATE_PLAYER && !item.masterlock && item.rightmenustate == PLAYER_RIGHT_MENU_PLAYER && item.popupstate == POPUP_STATE_NONE){
							if(item.playercontrolstate == PLAYER_CONTROL_STATE_NONE){
								item.playercontrolstate = PLAYER_CONTROL_STATE_CONTROLS;
								
							}else{
								item.playercontrolstate = PLAYER_CONTROL_STATE_NONE;
							}
						}
					}
					
					if (button == SDL_KEY_DRIGHT){
						if(item.state == MENU_STATE_PLAYER && !item.masterlock && item.playercontrolstate != PLAYER_CONTROL_STATE_CONTROLS && item.popupstate == POPUP_STATE_NONE){
							if(item.rightmenustate == PLAYER_RIGHT_MENU_PLAYER){
								if(item.rightmenustate != PLAYER_RIGHT_MENU_CUSTOMARATIO && item.rightmenustate != PLAYER_RIGHT_MENU_IMAGE && item.rightmenustate != PLAYER_RIGHT_MENU_AUDIO && item.rightmenustate != PLAYER_RIGHT_MENU_SUB && item.rightmenustate != PLAYER_AUDIOEQ && item.rightmenustate != PLAYER_SUPERAUDIOEQ){
									item.rightmenustate = PLAYER_RIGHT_MENU_HOME;
								}
							}else if(item.rightmenustate != PLAYER_RIGHT_MENU_PLAYER){
								if(item.rightmenustate != PLAYER_RIGHT_MENU_CUSTOMARATIO && item.rightmenustate != PLAYER_RIGHT_MENU_IMAGE && item.rightmenustate != PLAYER_RIGHT_MENU_AUDIO && item.rightmenustate != PLAYER_RIGHT_MENU_SUB && item.rightmenustate != PLAYER_AUDIOEQ && item.rightmenustate != PLAYER_SUPERAUDIOEQ){
									item.rightmenustate = PLAYER_RIGHT_MENU_PLAYER;
									item.rightmenu_startpos = 1280.0f;
								}
							}
						}
					}
					
					if (button == SDL_KEY_DLEFT){
						if((item.state == MENU_STATE_FILEBROWSER || item.state == MENU_STATE_USB || item.state == MENU_STATE_FTPBROWSER || item.state == MENU_STATE_HTTPBROWSER || item.state == MENU_STATE_SSHBROWSER || item.state == MENU_STATE_SAMBABROWSER || item.state == MENU_STATE_NFSBROWSER) && item.popupstate == POPUP_STATE_NONE){
							if(item.selectionstate == FILE_SELECTION_CHECKBOX){
								item.focus = true;
								item.selectionstate = FILE_SELECTION_NONE;
							}else if(item.selectionstate == FILE_SELECTION_NONE){
								item.selectionstate = FILE_SELECTION_CHECKBOX;
								item.focus = true;
							}
						}
						
					}
					
					if (button == SDL_KEY_RSTICK){
						if(item.state == MENU_STATE_PLAYER){
							toggleMasterLock();
						}
					}
					if (button == SDL_KEY_LSTICK){
						if(item.state == MENU_STATE_PLAYER && !item.masterlock){
							toggleOC();
						}
					}
					if (button == SDL_KEY_MINUS){
						if(item.state == MENU_STATE_PLAYER && !item.masterlock){
							item.state = item.laststate;
						}else if(item.state != MENU_STATE_PLAYER && !libmpv->Stopped()){
							item.state = MENU_STATE_PLAYER;
						}
					}
					
					if (button == SDL_KEY_ZR){
						if(item.state == MENU_STATE_PLAYER && !item.masterlock){
							libmpv->seek(libmpv->getPosition() + configini->getLongSeek(false),item.playershowcontrols);
						}
						
					}
					if (button == SDL_KEY_ZL){
						if(item.state == MENU_STATE_PLAYER && !item.masterlock){
							libmpv->seek(libmpv->getPosition() - configini->getLongSeek(false),item.playershowcontrols);
						
						}
						
					}
					if (button == SDL_KEY_R){
						if(item.state == MENU_STATE_PLAYER && !item.masterlock){
							libmpv->seek(libmpv->getPosition() + configini->getShortSeek(false),item.playershowcontrols);
						
						}
						if(item.state == MENU_STATE_PLAYLISTBROWSER){
							if(item.playlistitemHighlighted+1 < playlist->getPlaylist().size()){
								playlist->moveForw(item.playlistitemHighlighted);
								item.playlistUpdateHovered = true;
								item.playlistnewHoverIdx = item.playlistitemHighlighted+1;
							}
						}
						
					}
					if (button == SDL_KEY_L){
						if(item.state == MENU_STATE_PLAYER && !item.masterlock){
							libmpv->seek(libmpv->getPosition() - configini->getShortSeek(false),item.playershowcontrols);
						
						}
						if(item.state == MENU_STATE_PLAYLISTBROWSER){
							if(item.playlistitemHighlighted-1 >= 0){
								playlist->moveBack(item.playlistitemHighlighted);
								item.playlistUpdateHovered = true;
								item.playlistnewHoverIdx = item.playlistitemHighlighted-1;
							}
						}
						
					}
					if (button == SDL_KEY_X){
						
						if(item.state == MENU_STATE_FILEBROWSER || item.state == MENU_STATE_FTPBROWSER || item.state == MENU_STATE_HTTPBROWSER || item.state == MENU_STATE_USB || item.state == MENU_STATE_SSHBROWSER || item.state == MENU_STATE_SAMBABROWSER || item.state == MENU_STATE_NFSBROWSER){
							item.popupstate = POPUP_STATE_STARTPLAYLIST;
						}
						
						if(item.state == MENU_STATE_PLAYER && !item.masterlock){
							if(item.playershowcontrols){
								item.playershowcontrols=false;
								mpv_set_option_string(libmpv->getHandle(), "osd-level", "0");
								
							}else{
								item.playershowcontrols=true;
								mpv_set_option_string(libmpv->getHandle(), "osd-level", "3");
							}
						}
					}
					if (button == SDL_KEY_A){
						if(item.state == MENU_STATE_PLAYER){
							if(item.rightmenustate == PLAYER_RIGHT_MENU_PLAYER && item.playercontrolstate == PLAYER_CONTROL_STATE_NONE && item.popupstate == POPUP_STATE_NONE){
								if(libmpv->Paused()){
									libmpv->Resume();
								}else{
									libmpv->Pause();
								}
							}
						}
					}
					if (button == SDL_KEY_Y && !item.masterlock){
						if(item.state != MENU_STATE_HOME && item.state != MENU_STATE_PLAYER && item.popupstate == POPUP_STATE_NONE){
							if(localdir != nullptr){
								delete localdir;
								localdir = nullptr;
							}
#ifdef NXMP_USBSUPPORT
							if(usbmounter != nullptr && libmpv->Stopped() && !usbmounter->haveIteminPlaylist()){
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
							if(sshdir != nullptr){
								delete sshdir;
								sshdir = nullptr;
							}
							if(sambadir != nullptr){
								delete sambadir;
								sambadir = nullptr;
							}
#endif
#ifdef NXMP_UPNPSUPPORT
							if(nxupnp != nullptr){
								delete nxupnp;
								nxupnp = nullptr;
							}
#endif
#ifdef NXMP_ENIGMASUPPORT
							if(enigma2 != nullptr){
								delete enigma2;
								enigma2 = nullptr;
							}
#endif
							item.networkselect = true;
							item.first_item = true;
							item.state = MENU_STATE_HOME;
#ifdef NXMP_USBSUPPORT
							if(usbmounter != nullptr && usbmounter->haveIteminPlaylist()){
								usbmounter->setBasePath("");
							}
#endif
							
						}
										
					}
					if (button == SDL_KEY_B){
#ifdef NXMP_ENIGMASUPPORT
						if(item.state == MENU_STATE_ENIGMABROWSER){
							item.first_item = true;
							enigma2->backToTop();
						}
#endif
#ifdef NXMP_NETWORKSUPPORT
						if(item.state == MENU_STATE_FTPBROWSER){
							item.first_item = true;
							ftpdir->backDir();
							ftpdir->DirList(ftpdir->getCurrPath(),Utility::getMediaExtensions());
						}
						
						if(item.state == MENU_STATE_HTTPBROWSER){
							item.first_item = true;
							httpdir->backDir();
							httpdir->DirList(httpdir->getCurrPath(),Utility::getMediaExtensions());
							
						}
						if(item.state == MENU_STATE_SSHBROWSER){
							item.first_item = true;
							sshdir->backDir();
							sshdir->DirList(sshdir->getCurrPath(),configini->getshowHidden(false),Utility::getMediaExtensions());
							
						}
						if(item.state == MENU_STATE_SAMBABROWSER){
							item.first_item = true;
							sambadir->backDir();
							sambadir->DirList(sambadir->getCurrPath(),configini->getshowHidden(false),Utility::getMediaExtensions());
							
						}
						if(item.state == MENU_STATE_NFSBROWSER){
							item.first_item = true;
							nfsdir->backDir();
							nfsdir->DirList(nfsdir->getCurrPath(),configini->getshowHidden(false),Utility::getMediaExtensions());
							
						}
#endif
#ifdef NXMP_UPNPSUPPORT
						if(item.state == MENU_STATE_UPNPBROWSER){
							item.first_item = true;
							if(nxupnp->getSelDevice()>-1){
								nxupnp->getDevice(nxupnp->getSelDevice())->back();
							
								nxupnp->getDevice(nxupnp->getSelDevice())->browseOID();
							}
						}
#endif
						if(item.state == MENU_STATE_USB){
#ifdef NXMP_USBSUPPORT							
							if(usbmounter->getBasePath() != ""){
								item.first_item = true;
								usbmounter->backPath();
								usbmounter->DirList(usbmounter->getCurrentPath(),configini->getshowHidden(false),Utility::getMediaExtensions());
								
							}
#endif
						}
						
						if(item.state == MENU_STATE_FILEBROWSER){
							item.first_item = true;
							localdir->backPath();
							localdir->DirList(localdir->getCurrentPath(),configini->getshowHidden(false),Utility::getMediaExtensions());
							
						}
						
						if(item.state == MENU_STATE_PLAYER || item.state == MENU_STATE_PLAYERCACHING){
							if(item.rightmenustate == PLAYER_RIGHT_MENU_PLAYER && item.playercontrolstate == PLAYER_CONTROL_STATE_NONE){
								if(!libmpv->Stopped()  && !item.masterlock){
									item.state = item.laststate;
									libmpv->Stop();
								}
							}else if(item.rightmenustate == PLAYER_RIGHT_MENU_TRACKS){
								item.rightmenustate = PLAYER_RIGHT_MENU_HOME;
							}
							else if(item.rightmenustate == PLAYER_RIGHT_MENU_TRACKS_VIDEO||item.rightmenustate == PLAYER_RIGHT_MENU_TRACKS_AUDIO||item.rightmenustate == PLAYER_RIGHT_MENU_TRACKS_SUB){
								item.rightmenustate = PLAYER_RIGHT_MENU_TRACKS;
							}
							else if(item.rightmenustate == PLAYER_RIGHT_MENU_CHAPTERS){
								item.rightmenustate = PLAYER_RIGHT_MENU_HOME;
							}
							/*else if(item.rightmenustate == PLAYER_RIGHT_MENU_ANIME4K){
								item.rightmenustate = PLAYER_RIGHT_MENU_HOME;
							}*/
							else if(item.rightmenustate == PLAYER_RIGHT_MENU_ARATIO){
								item.rightmenustate = PLAYER_RIGHT_MENU_HOME;
							}
							else if(item.rightmenustate == PLAYER_RIGHT_MENU_CUSTOMARATIO){
								item.rightmenustate = PLAYER_RIGHT_MENU_ARATIO;
							}
							else if(item.rightmenustate == PLAYER_RIGHT_MENU_IMAGE){
								item.rightmenustate = PLAYER_RIGHT_MENU_HOME;
							}
							else if(item.rightmenustate == PLAYER_RIGHT_MENU_AUDIO){
								item.rightmenustate = PLAYER_RIGHT_MENU_HOME;
							}
							else if(item.rightmenustate == PLAYER_RIGHT_MENU_SUB){
								if(item.popupstate == POPUP_STATE_SUBFONTCOLOR){
									item.popupstate = POPUP_STATE_NONE;
									configini->setSubFontColor(configini->getSubFontColor(false));
								}else{
									item.rightmenustate = PLAYER_RIGHT_MENU_HOME;
								}
							//bordercolor
								if(item.popupstate == POPUP_STATE_SUBBORDERCOLOR){
									item.popupstate = POPUP_STATE_NONE;
									configini->setSubBorderColor(configini->getSubBorderColor(false));
								}else{
									item.rightmenustate = PLAYER_RIGHT_MENU_HOME;
								}
							}
							else if(item.rightmenustate == PLAYER_AUDIOEQ){
								item.rightmenustate = PLAYER_RIGHT_MENU_AUDIO;
							}
							else if(item.rightmenustate == PLAYER_SUPERAUDIOEQ){
								item.rightmenustate = PLAYER_RIGHT_MENU_AUDIO;
							}
							else if(item.rightmenustate == PLAYER_RIGHT_MENU_SHADERMANIA){
								item.rightmenustate = PLAYER_RIGHT_MENU_HOME;
							}
							
							
						}
					}
				}
				if (event.type == wakeup_on_mpv_render_update)
				{
					mpv_render_context_update(libmpv->getContext());
				}
				if (event.type == wakeup_on_mpv_events)
				{
					while (1) {
					mpv_event *mp_event = (mpv_event*) mpv_wait_event(libmpv->getHandle(), 0);
					if (mp_event->event_id == MPV_EVENT_NONE)
						break;
					if (mp_event->event_id == MPV_EVENT_LOG_MESSAGE) {
						//mpv_event_log_message *msg = (mpv_event_log_message *) mp_event->data;
						continue;
					}
					if (mp_event->event_id == MPV_EVENT_FILE_LOADED) {
						libmpv->getfileInfo();
						item.state = MENU_STATE_PLAYER;
						if(libmpv->getFileInfo()->videos.size() == 0 || (libmpv->getFileInfo()->videos.size() == 1 && libmpv->getFileInfo()->videos[0].albumart) ){
							item.playerstate = PLAYER_STATE_AUDIO;
							mpv_observe_property(libmpv->getHandle(), 0, "playback-time", MPV_FORMAT_DOUBLE);
							mpv_observe_property(libmpv->getHandle(), 0, "duration", MPV_FORMAT_DOUBLE);
							mpv_observe_property(libmpv->getHandle(), 0, "metadata", MPV_FORMAT_NODE);
							item.playercontrolstate = PLAYER_CONTROL_STATE_CONTROLS;
							
						}else{
							mpv_observe_property(libmpv->getHandle(), 0, "playback-time", MPV_FORMAT_DOUBLE);
							mpv_observe_property(libmpv->getHandle(), 0, "duration", MPV_FORMAT_DOUBLE);
							mpv_observe_property(libmpv->getHandle(), 0, "metadata", MPV_FORMAT_NODE);
							item.playerstate = PLAYER_STATE_VIDEO;
						}
					}
					
					if (mp_event->event_id == MPV_EVENT_START_FILE) {
#ifdef NXMP_SWITCH
						appletSetMediaPlaybackState(true);
#endif					
						printf("START FILE\n");
						item.laststate = item.state;
						item.state = MENU_STATE_PLAYERCACHING;
					}
					//printf("Event id: %d\n",mp_event->event_id);
					if (mp_event->event_id == MPV_EVENT_END_FILE) {
#ifdef NXMP_SWITCH
						appletSetMediaPlaybackState(false);
#endif					
						struct mpv_event_end_file *eof = (struct mpv_event_end_file *)mp_event->data;
						libmpv->setLoop(false);
						if(item.playerstate == PLAYER_STATE_VIDEO  && libmpv->getFileInfo()->playbackInfo.islive == false && sqlitedb != nullptr){
							if(libmpv->getFileInfoPerc() >= configini->getResumeStartPerc(false) && 100-libmpv->getFileInfoPerc() >= configini->getResumeStopPerc(false)){
								sqlitedb->writeResume(libmpv->getFileInfo()->path,libmpv->getFileInfo()->playbackInfo.position);
							}else{
								if(eof->reason != MPV_END_FILE_REASON_EOF){
									if(100-libmpv->getFileInfoPerc() < configini->getResumeStopPerc(false))
									sqlitedb->markCompleted(libmpv->getFileInfo()->path);
								}
							}
							
							if(eof->reason == MPV_END_FILE_REASON_EOF){
								printf("FILE EOF\n");
								sqlitedb->markCompleted(libmpv->getFileInfo()->path);
							}
							
						}
						if(eof->reason == MPV_END_FILE_REASON_EOF){
							printf("FILE EOF\n");
							if(playlist->getPlaylist().size() >0){
								if(playlist->getCurrIdx() < playlist->getPlaylist().size()-1){
									Playlist::playlist_struct nextfile = playlist->getNext();
									libmpv->loadFile(nextfile.fulluri);
									if(configini->getDbActive(true)){
										libmpv->getFileInfo()->resume = sqlitedb->getResume(nextfile.fulluri);
										if(libmpv->getFileInfo()->resume>0){
											item.popupstate = POPUP_STATE_RESUME;
										}
									}
								}
							}
						}
						printf("END PLAY FILE\n");
						item.state = item.laststate;
						item.rightmenustate = PLAYER_RIGHT_MENU_PLAYER;
						item.playercontrolstate = PLAYER_CONTROL_STATE_NONE;
						item.masterlock = false;
						printf("MENU STATE: %d\n",item.state );
					}
					if(mp_event->event_id == MPV_EVENT_PROPERTY_CHANGE){
						mpv_event_property *prop = (mpv_event_property*)mp_event->data;
						if(std::string(prop->name) == "metadata" && !libmpv->Stopped()) 
						{
							if(prop->data == nullptr)continue;
							mpv_node node = *(mpv_node *)prop->data;
							printf("Node format %d\n",node.format);
							if (node.format == MPV_FORMAT_NODE_MAP) {
								for (int n = 0; n < node.u.list->num; n++) {
									std::string key = node.u.list->keys[n];
									if(key == "title"){
										libmpv->getFileInfo()->playbackInfo.title = node.u.list->values[n].u.string;
									}
									if(key == "artist"){
										libmpv->getFileInfo()->playbackInfo.artist = node.u.list->values[n].u.string;
									}
									printf("META KEY %s\n",key.c_str());
								}
							}
						}
						if(std::string(prop->name) == "playback-time") 
						{
							if(prop->format == MPV_FORMAT_DOUBLE)
							{
								double timepos = *(double *)prop->data;
								libmpv->getFileInfo()->playbackInfo.position = (int)timepos;
							}
						}
						if(std::string(prop->name) == "duration") 
						{
							if(prop->format == MPV_FORMAT_INT64)
							{
								int64_t timepos = *(int64_t *)prop->data;
								libmpv->getFileInfo()->playbackInfo.duration = timepos;
							}
						}
					}
					if(mp_event->event_id!=22){
						printf("event: %s %d\n", mpv_event_name(mp_event->event_id),mp_event->event_id);
					}
					}
				}
			}
	}
	
	void HandleLayers(){
		ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();
			switch (item.state) {
				case MENU_STATE_HOME:
					Windows::MainMenuWindow(&item.focus, &item.first_item);
					if(item.popupstate == POPUP_STATE_DBUPDATED){
						Popups::DBUpdatedPopup();
					}
					break;
				case MENU_STATE_FILEBROWSER:
					Windows::FileBrowserWindow(&item.focus, &item.first_item);
					if(item.popupstate == POPUP_STATE_STARTPLAYLIST){
						Popups::PlaylistStartPlaylist();
					}
					break;
				case MENU_STATE_USB:
#ifdef NXMP_USBSUPPORT
					Windows::USBBrowserWindow(&item.focus, &item.first_item);
					if(item.popupstate == POPUP_STATE_STARTPLAYLIST){
						Popups::PlaylistStartPlaylist();
					}
#endif
					break;
				case MENU_STATE_NETWORKBROWSER:
#ifdef NXMP_NETWORKSUPPORT
					Windows::NetworkWindow(&item.focus, &item.first_item);
#endif
					break;
				case MENU_STATE_FTPBROWSER:
#ifdef NXMP_NETWORKSUPPORT
					Windows::FtpWindow(&item.focus, &item.first_item);
					if(item.popupstate == POPUP_STATE_STARTPLAYLIST){
						Popups::PlaylistStartPlaylist();
					}
#endif
					break;
				case MENU_STATE_HTTPBROWSER:
#ifdef NXMP_NETWORKSUPPORT
					Windows::HttpWindow(&item.focus, &item.first_item);
					if(item.popupstate == POPUP_STATE_STARTPLAYLIST){
						Popups::PlaylistStartPlaylist();
					}
#endif
					break;
				case MENU_STATE_SSHBROWSER:
#ifdef NXMP_NETWORKSUPPORT
					Windows::SSHWindow(&item.focus, &item.first_item);
					if(item.popupstate == POPUP_STATE_STARTPLAYLIST){
						Popups::PlaylistStartPlaylist();
					}
#endif
					break;
				case MENU_STATE_SAMBABROWSER:
#ifdef NXMP_NETWORKSUPPORT
					Windows::SambaWindow(&item.focus, &item.first_item);
					if(item.popupstate == POPUP_STATE_STARTPLAYLIST){
						Popups::PlaylistStartPlaylist();
					}
#endif
					break;
				case MENU_STATE_NFSBROWSER:
#ifdef NXMP_NETWORKSUPPORT
					Windows::NFSWindow(&item.focus, &item.first_item);
					if(item.popupstate == POPUP_STATE_STARTPLAYLIST){
						Popups::PlaylistStartPlaylist();
					}
#endif
					break;
				case MENU_STATE_UPNPBROWSER:
#ifdef NXMP_UPNPSUPPORT
					Windows::UPNPBrowserWindow(&item.focus, &item.first_item);
					//if(item.popupstate == POPUP_STATE_STARTPLAYLIST){
					//	Popups::PlaylistStartPlaylist();
					//}
#endif
					break;
				case MENU_STATE_ENIGMABROWSER:
#ifdef NXMP_ENIGMASUPPORT
					Windows::EnigmaWindow(&item.focus, &item.first_item);
#endif
					break;
				case MENU_STATE_SETTINGS:
					Windows::SettingsMenuWindow(&item.focus, &item.first_item);
					if(item.popupstate == POPUP_STATE_SAVE_SETTINGS){
						Popups::SaveSettingsPopup();
					}
					break;
				case MENU_STATE_PLAYLISTBROWSER:
					Windows::PlaylistWindow(&item.focus, &item.first_item);
					break;
				case MENU_STATE_INFO:
					Windows::InfoMenuWindow(&item.focus, &item.first_item);
					break;
				case MENU_STATE_PLAYERCACHING:
					playerWindows::CacheWindow();
					break;
				case MENU_STATE_PLAYER:
					if(item.showVolume){
						playerWindows::VolumeWindow();
					}
					if(item.popupstate == POPUP_STATE_RESUME){
						Popups::ResumePopup();
					}
				
					if(item.playerstate == PLAYER_STATE_AUDIO){
						
					}
					break;
				
			}
			
			switch (item.playercontrolstate) {
				case PLAYER_CONTROL_STATE_NONE:
					break;
				case PLAYER_CONTROL_STATE_CONTROLS:
					playerWindows::playerControls();
					break;
			}
			
			switch (item.rightmenustate) {
				case PLAYER_RIGHT_MENU_PLAYER:
					break;
				case PLAYER_RIGHT_MENU_HOME:
					playerWindows::RightHomeWindow(&item.rightmenu_focus,&item.rightmenu_first_item);
					break;
				case PLAYER_RIGHT_MENU_TRACKS:
					playerWindows::RightTrackWindow(&item.rightmenu_focus,&item.rightmenu_first_item);
					break;
				case PLAYER_RIGHT_MENU_TRACKS_VIDEO:
					playerWindows::RightTrackVideoWindow(&item.rightmenu_focus,&item.rightmenu_first_item);
					break;
				case PLAYER_RIGHT_MENU_TRACKS_AUDIO:
					playerWindows::RightTrackAudioWindow(&item.rightmenu_focus,&item.rightmenu_first_item);
					break;
				case PLAYER_RIGHT_MENU_TRACKS_SUB:
					playerWindows::RightTrackSubWindow(&item.rightmenu_focus,&item.rightmenu_first_item);
					break;
				case PLAYER_RIGHT_MENU_CHAPTERS:
					playerWindows::RightChapterWindow(&item.rightmenu_focus,&item.rightmenu_first_item);
					break;
				/*case PLAYER_RIGHT_MENU_ANIME4K:
					playerWindows::RightHomeAnime4K(&item.rightmenu_focus,&item.rightmenu_first_item);
					break;*/
				case PLAYER_RIGHT_MENU_ARATIO:
					playerWindows::RightHomeARatio(&item.rightmenu_focus,&item.rightmenu_first_item);
					break;
				case PLAYER_RIGHT_MENU_CUSTOMARATIO:
					playerWindows::RightHomeCustomARatio(&item.rightmenu_focus,&item.rightmenu_first_item);
					break;
				case PLAYER_RIGHT_MENU_IMAGE:
					playerWindows::RightHomeImage(&item.rightmenu_focus,&item.rightmenu_first_item);
					break;
				case PLAYER_RIGHT_MENU_AUDIO:
					playerWindows::RightHomeAudio(&item.rightmenu_focus,&item.rightmenu_first_item);
					break;
				case PLAYER_RIGHT_MENU_SUB:
					playerWindows::RightHomeSub(&item.rightmenu_focus,&item.rightmenu_first_item);
					if(item.popupstate == POPUP_STATE_SUBFONTCOLOR){
						Popups::SubFontColorPopup();
					}
					if(item.popupstate == POPUP_STATE_SUBBORDERCOLOR){
						Popups::SubBorderColorPopup();
					}
					break;
				case PLAYER_RIGHT_MENU_SHADERMANIA:
					playerWindows::RightHomeShaderMania();
					break;
				case PLAYER_AUDIOEQ:
					playerWindows::AudioEqualizer(&item.rightmenu_focus,&item.rightmenu_first_item);
					break;
				case PLAYER_SUPERAUDIOEQ:
					playerWindows::AudioSuperEqualizer(&item.rightmenu_focus,&item.rightmenu_first_item);
					break;	
					
			}
		
	}
	
	void HandleRender(){
		ImGui::Render();
		ImGuiIO &io = ImGui::GetIO();
		glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
		glClearColor(0.00f, 0.00f, 0.00f, 1.00f);
		glClear(GL_COLOR_BUFFER_BIT);
	
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);

		{
			int w, h;
			SDL_GetWindowSize(window, &w, &h);
			fbo = {
				.fbo = 0,
				.w = w,
				.h = h,
			};
			params[0] = {MPV_RENDER_PARAM_OPENGL_FBO, &fbo};
			params[1] = {MPV_RENDER_PARAM_FLIP_Y, &__fbo_one};
			params[2] = {(mpv_render_param_type)0};
			mpv_render_context_render(libmpv->getContext(), params);
		}
    	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);

	}

	int RenderLoop(void) {
		if(dbUpdated){
			item.popupstate = POPUP_STATE_DBUPDATED;
		}
		item.first_item = true;
#ifdef NXMP_SWITCH
		while (!renderloopdone && appletMainLoop())
#endif
#ifdef NXMP_WIN32
		while (!renderloopdone)
#endif
		{
			HandleEvents();
			HandleLayers();
			HandleRender();
			if(dochangethemefont){
				changeFontTheme();
				dochangethemefont = false;
			}

			#ifdef NXMP_SWITCH
    		//rewrite switch state
			AppletOperationMode stus=appletGetOperationMode();
			if (stus == AppletOperationMode_Handheld) 
			{
				if (isHandheld == false) 
				{isHandheld=true;
				printf("changed to Handheld Mode.\n");
				newResW = handheldWidth;
				newResH = handheldHeight;
				multiplyRes = 1.0f;
				SDL_SetWindowSize(window, newResW, newResH);
				reinit();
				}
			}
			if (stus == AppletOperationMode_Console) 
			{	if (isHandheld == true) 
				{isHandheld=false;
				printf("changed to Docked Mode.\n");
				newResW = dockedWidth;
				newResH = dockedHeight;
				multiplyRes = 1.5f;
				SDL_SetWindowSize(window, newResW, newResH);
				reinit();
				}
			}
    		#endif
			
		}
		
		return 0;	
	}
    void reinit()
	{
	ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();


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

	}
	void changeFontTheme(){
		ImGuiIO &io = ImGui::GetIO();
		
		unsigned char *pixels = nullptr;
		int width = 0, height = 0, bpp = 0;
		ImFontConfig font_cfg;
		
		io.Fonts->Clear();
		
		io.Fonts->AddFontFromFileTTF(themefontpath.c_str(), themefontsize,&font_cfg);
		fontSmall = io.Fonts->AddFontFromFileTTF(themefontpath.c_str(), themefontsmall,&font_cfg);
		
		io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height, &bpp);
		io.Fonts->Flags |= ImFontAtlasFlags_NoPowerOfTwoHeight;
		io.Fonts->Build();
		ImGui_ImplOpenGL3_CreateFontsTexture();
		
	}

}	

#ifdef _WIN32
bool isMouseSelection = false;
int startMousex = 0;
int startMousey = 0;
#endif
	