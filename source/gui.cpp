#include <algorithm>
#include <iostream>

#include "gui.h"
#include "localfiles.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "SwitchSys.h"



MenuItem item;

namespace GUI {
	
	const int JOYSTICK_DEAD_ZONE = 8000;
	const int JOYSTICK_EXTENDED_DEAD_ZONE = 32726;
	
	int wakeup = 0;
	
	
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

		clockoc = !clockoc;
		if(clockoc){
			const char *cmd[] = {"show-text", "Overclock Enabled","2000", NULL};
			mpv_command_async(libmpv->getHandle(), 0, cmd);
			SwitchSys::maxClock();
		}else{
			const char *cmd[] = {"show-text", "Overclock Disabled","2000", NULL};
			mpv_command_async(libmpv->getHandle(), 0, cmd);
			SwitchSys::defaultClock(SwitchSys::stock_cpu_clock, SwitchSys::stock_gpu_clock, SwitchSys::stock_emc_clock); 
		}

	}

	
	void HandleEvents(){
		SDL_Event event;
			while (SDL_PollEvent(&event)) {
				ImGui_ImplSDL2_ProcessEvent(&event);

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
				{	
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
					//I changed the close to the minus button, because the Keyboard applet uses the + by default to confirm, 
					//and if you press + to confirm it conflicts with this, and as a result the app crashes
					if (button == SDL_KEY_MINUS && !item.masterlock)
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
						if((item.state == MENU_STATE_FILEBROWSER || item.state == MENU_STATE_USB_BROWSER || item.state == MENU_STATE_FTPBROWSER || item.state == MENU_STATE_HTTPBROWSER || item.state == MENU_STATE_SSHBROWSER || item.state == MENU_STATE_SAMBABROWSER || item.state == MENU_STATE_NFSBROWSER) && item.popupstate == POPUP_STATE_NONE){
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
					//exchanged the minus for the plus.
					//to avoid the crash of the integrated keyboard.
					if (button == SDL_KEY_PLUS){
						if(item.state == MENU_STATE_PLAYER && !item.masterlock && item.rightmenustate == PLAYER_RIGHT_MENU_PLAYER){
							item.savestate = item.laststate;
							item.state = item.laststate;	
						}else if(item.state != MENU_STATE_PLAYER && !libmpv->Stopped()){
							std::cout << std::endl <<" State is?: " << item.state << std::endl;
							//fix crash
							if (item.state != item.savestate)
							{item.state = MENU_STATE_FILEBROWSER;
							filebrowser = new CFileBrowser(configini->getStartPath(),playlist);
							filebrowser->DirList(configini->getStartPath(),true,Utility::getMediaExtensions());
							item.first_item = true;}
							
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
						
						if(item.state == MENU_STATE_FILEBROWSER || item.state == MENU_STATE_FTPBROWSER || item.state == MENU_STATE_HTTPBROWSER || item.state == MENU_STATE_USB_BROWSER || item.state == MENU_STATE_SSHBROWSER || item.state == MENU_STATE_SAMBABROWSER || item.state == MENU_STATE_NFSBROWSER){
							item.popupstate = POPUP_STATE_FILECONTEXTMENU;
						}
						
						if(item.state == MENU_STATE_PLAYLISTBROWSER){
							if(playlist->getPlaylist().size()>0){
								item.popupstate = POPUP_STATE_PLAYLISTCONTEXTMENU;
							}
						}
						
						if(item.state == MENU_STATE_NETWORKBROWSER){
							if(Windows::netwinselected != -1){
								item.popupstate = POPUP_STATE_NETWORKMENU;
							}
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
							if(filebrowser != nullptr){
								delete filebrowser;
								filebrowser = nullptr;
							}
							/*
							if(localdir != nullptr){
								delete localdir;
								localdir = nullptr;
							}
							*/
							if(MyUSBMount != nullptr && libmpv->Stopped() && !playlist->HaveUSBEntrys()){
								delete MyUSBMount;
								MyUSBMount = nullptr;
							}
							if(NewNetworkShare != nullptr){
								delete NewNetworkShare;
								NewNetworkShare = nullptr;
							}
							/*
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
							*/
							if(nxupnp != nullptr){
								delete nxupnp;
								nxupnp = nullptr;
							}
							if(enigma2 != nullptr){
								delete enigma2;
								enigma2 = nullptr;
							}
							
							if(mtp != nullptr){
								delete mtp;
								mtp = nullptr;
							}
							item.networkselect = true;
							item.first_item = true;
							item.state = MENU_STATE_HOME;
							//if(usbmounter != nullptr && usbmounter->haveIteminPlaylist()){
							//	usbmounter->setBasePath("");
							//}
							
						}
										
					}
					if (button == SDL_KEY_B){
						
						if(item.state == MENU_STATE_FILEBROWSER || item.state == MENU_STATE_FTPBROWSER || item.state == MENU_STATE_HTTPBROWSER || item.state == MENU_STATE_SSHBROWSER || item.state == MENU_STATE_SAMBABROWSER || item.state == MENU_STATE_NFSBROWSER|| item.state == MENU_STATE_USB_BROWSER){
							if(item.popupstate != POPUP_STATE_NONE){
								item.popupstate = POPUP_STATE_NONE;
							}else{
								item.first_item = true;
								filebrowser->backDir();
								filebrowser->DirList(filebrowser->getCurrentPath(),configini->getshowHidden(false),Utility::getMediaExtensions());
							
							}
							
						}
						if(item.state == MENU_STATE_ENIGMABROWSER){
							item.first_item = true;
							enigma2->backToTop();
						}
						/*
						if(item.state == MENU_STATE_FTPBROWSER){
							item.first_item = true;
							filebrowser->backDir();
							filebrowser->DirList(filebrowser->getCurrentPath(),false,Utility::getMediaExtensions());
						}
						
						if(item.state == MENU_STATE_HTTPBROWSER){
							item.first_item = true;
							filebrowser->backDir();
							filebrowser->DirList(filebrowser->getCurrentPath(),false,Utility::getMediaExtensions());
							
						}
						if(item.state == MENU_STATE_SSHBROWSER){
							item.first_item = true;
							filebrowser->backDir();
							filebrowser->DirList(filebrowser->getCurrentPath(),configini->getshowHidden(false),Utility::getMediaExtensions());
							
						}
						if(item.state == MENU_STATE_SAMBABROWSER){
							item.first_item = true;
							filebrowser->backDir();
							filebrowser->DirList(filebrowser->getCurrentPath(),configini->getshowHidden(false),Utility::getMediaExtensions());
							
						}
						if(item.state == MENU_STATE_NFSBROWSER){
							item.first_item = true;
							filebrowser->backDir();
							filebrowser->DirList(filebrowser->getCurrentPath(),configini->getshowHidden(false),Utility::getMediaExtensions());
							
						}
						*/
						if(item.state == MENU_STATE_UPNPBROWSER){
							item.first_item = true;
							if(nxupnp->getSelDevice()>-1){
								nxupnp->getDevice(nxupnp->getSelDevice())->back();
							
								nxupnp->getDevice(nxupnp->getSelDevice())->browseOID();
							}
						}
						/*
						if(item.state == MENU_STATE_USB_BROWSER){	
							item.first_item = true;
							filebrowser->backDir();
							filebrowser->DirList(filebrowser->getCurrentPath(),configini->getshowHidden(false),Utility::getMediaExtensions());
								
						}
						*/
						if(item.state == MENU_STATE_USB_MOUNT){
/*							
							if(usbmounter->getBasePath() != ""){
								item.first_item = true;
								usbmounter->backDir();
								usbmounter->DirList(usbmounter->getCurrentPath(),configini->getshowHidden(false),Utility::getMediaExtensions());
						
							}
							*/		
						}
						/*
						if(item.state == MENU_STATE_FILEBROWSER){
							item.first_item = true;
							filebrowser->backDir();
							filebrowser->DirList(filebrowser->getCurrentPath(),configini->getshowHidden(false),Utility::getMediaExtensions());
							
						}
						*/
						
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
					GUI::wakeup = 1;
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
							mpv_observe_property(libmpv->getHandle(), 0, "hwdec-current", MPV_FORMAT_STRING);
							mpv_observe_property(libmpv->getHandle(), 0, "video-format", MPV_FORMAT_STRING);
							mpv_observe_property(libmpv->getHandle(), 0, "playback-time", MPV_FORMAT_DOUBLE);
							mpv_observe_property(libmpv->getHandle(), 0, "duration", MPV_FORMAT_DOUBLE);
							mpv_observe_property(libmpv->getHandle(), 0, "metadata", MPV_FORMAT_NODE);
							mpv_observe_property(libmpv->getHandle(), 0, "video-bitrate", MPV_FORMAT_DOUBLE);
							mpv_observe_property(libmpv->getHandle(), 0, "audio-bitrate", MPV_FORMAT_DOUBLE);
							mpv_observe_property(libmpv->getHandle(), 0, "estimated-vf-fps", MPV_FORMAT_DOUBLE);
							mpv_observe_property(libmpv->getHandle(), 0, "vo-drop-frame-count", MPV_FORMAT_INT64);
							
							mpv_observe_property(libmpv->getHandle(), 0, "colormatrix", MPV_FORMAT_STRING);
							
							
							mpv_observe_property(libmpv->getHandle(), 0, "video-params", MPV_FORMAT_NODE);
							mpv_observe_property(libmpv->getHandle(), 0, "audio-params", MPV_FORMAT_NODE);
							item.playerstate = PLAYER_STATE_VIDEO;
						}
						
						
							
						nxmpstats->decodingstats.videodecstats.codec = libmpv->getVideoCodec();
						
						if(libmpv->getFileInfo()->audios.size() > 0){
							nxmpstats->decodingstats.audiodecstats.codec = libmpv->getAudioCodec();
							
						}
						
					}
					
					if (mp_event->event_id == MPV_EVENT_START_FILE) {

						appletSetMediaPlaybackState(true);
						item.laststate = item.state;
						item.state = MENU_STATE_PLAYERCACHING;
						
					}
					if (mp_event->event_id == MPV_EVENT_END_FILE) {
						appletSetMediaPlaybackState(false);
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
								sqlitedb->markCompleted(libmpv->getFileInfo()->path);
							}
							
						}
						if(eof->reason == MPV_END_FILE_REASON_EOF){
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
						item.state = item.laststate;
						item.rightmenustate = PLAYER_RIGHT_MENU_PLAYER;
						item.playercontrolstate = PLAYER_CONTROL_STATE_NONE;
						item.masterlock = false;
						
					}
					if(mp_event->event_id == MPV_EVENT_PROPERTY_CHANGE){
						mpv_event_property *prop = (mpv_event_property*)mp_event->data;
						if(std::string(prop->name) == "metadata" && !libmpv->Stopped()) 
						{
							if(prop->data == nullptr)continue;
							mpv_node node = *(mpv_node *)prop->data;
							if (node.format == MPV_FORMAT_NODE_MAP) {
								for (int n = 0; n < node.u.list->num; n++) {
									std::string key = node.u.list->keys[n];
									if(key == "title"){
										libmpv->getFileInfo()->playbackInfo.title = node.u.list->values[n].u.string;
									}
									if(key == "artist"){
										libmpv->getFileInfo()->playbackInfo.artist = node.u.list->values[n].u.string;
									}
								}
							}
						}
						
						if(std::string(prop->name) == "video-params") 
						{
							if(prop->format == MPV_FORMAT_NODE)
							{
								mpv_node node = *(mpv_node *)prop->data;
								if(node.format == MPV_FORMAT_NODE_MAP)
								{
									for(int i = 0; i < node.u.list->num; i++)
									{
										if(std::string(node.u.list->keys[i]) == "pixelformat"){
										 if(node.u.list->values[i].format == MPV_FORMAT_STRING){
											 nxmpstats->decodingstats.videodecstats.pixelformat = node.u.list->values[i].u.string;
										 }
										}
										if(std::string(node.u.list->keys[i]) == "dw"){
										 if(node.u.list->values[i].format == MPV_FORMAT_INT64){
											 nxmpstats->decodingstats.videodecstats.width = node.u.list->values[i].u.int64;
										 }
										}
										if(std::string(node.u.list->keys[i]) == "dh"){
										 if(node.u.list->values[i].format == MPV_FORMAT_INT64){
											 nxmpstats->decodingstats.videodecstats.height = node.u.list->values[i].u.int64;
										 }
										}
										else if(std::string(node.u.list->keys[i]) == "hw-pixelformat"){
										 if(node.u.list->values[i].format == MPV_FORMAT_STRING){
											 nxmpstats->decodingstats.videodecstats.hwpixelformat = node.u.list->values[i].u.string;
										 }
										}
										
										else if(std::string(node.u.list->keys[i]) == "colormatrix"){
										 if(node.u.list->values[i].format == MPV_FORMAT_STRING){
											 nxmpstats->decodingstats.videodecstats.colormatrix = node.u.list->values[i].u.string;
										 }
										}
										
										
										
									}
								}
								
								//libmpv->getFileInfo()->playbackInfo.position = (int)timepos;
							}
						}
						
						if(std::string(prop->name) == "audio-params") 
						{
							if(prop->format == MPV_FORMAT_NODE)
							{
								mpv_node node = *(mpv_node *)prop->data;
								if(node.format == MPV_FORMAT_NODE_MAP)
								{
									for(int i = 0; i < node.u.list->num; i++)
									{
										if(std::string(node.u.list->keys[i]) == "samplerate"){
										 if(node.u.list->values[i].format == MPV_FORMAT_INT64){
											 nxmpstats->decodingstats.audiodecstats.samplerate = node.u.list->values[i].u.int64;
										 }
										}
										else if(std::string(node.u.list->keys[i]) == "channel-count"){
										 if(node.u.list->values[i].format == MPV_FORMAT_INT64){
											 nxmpstats->decodingstats.audiodecstats.channels = node.u.list->values[i].u.int64;
										 }
										}
										else if(std::string(node.u.list->keys[i]) == "samplerate"){
										 if(node.u.list->values[i].format == MPV_FORMAT_INT64){
											 nxmpstats->decodingstats.audiodecstats.samplerate = node.u.list->values[i].u.int64;
										 }
										}
										else if(std::string(node.u.list->keys[i]) == "hr-channels"){
										 if(node.u.list->values[i].format == MPV_FORMAT_STRING){
											 nxmpstats->decodingstats.audiodecstats.hrchannels = node.u.list->values[i].u.string;
										 }
										}

									}
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
						if(std::string(prop->name) == "video-bitrate") 
						{
							if(prop->format == MPV_FORMAT_DOUBLE)
							{
								double bitrateval = *(double *)prop->data;
								nxmpstats->decodingstats.videodecstats.bitrate = bitrateval;
							}
						}
						if(std::string(prop->name) == "audio-bitrate") 
						{
							if(prop->format == MPV_FORMAT_DOUBLE)
							{
								double bitrateval = *(double *)prop->data;
								nxmpstats->decodingstats.audiodecstats.bitrate = bitrateval;
							}
						}
						if(std::string(prop->name) == "estimated-vf-fps") 
						{
							if(prop->format == MPV_FORMAT_DOUBLE)
							{
								int64_t fpsval = *(double *)prop->data;
								nxmpstats->decodingstats.videodecstats.fps = fpsval;
							}
						}
						
						if(std::string(prop->name) == "vo-drop-frame-count") 
						{
							if(prop->format == MPV_FORMAT_INT64)
							{
								int64_t dropval = *(int64_t *)prop->data;
								nxmpstats->decodingstats.videodecstats.framedropcount = dropval;
							}
						}
						
						if(std::string(prop->name) == "video-format") 
						{
							if(prop->format == MPV_FORMAT_STRING)
							{
								nxmpstats->decodingstats.videodecstats.videoformat = *(char**)prop->data;
								//nxmpstats->decodingstats.videodecstats.hwdec = hwdecval;
							}
						}
						if(std::string(prop->name) == "hwdec-current") 
						{
							if(prop->format == MPV_FORMAT_STRING)
							{
								nxmpstats->decodingstats.videodecstats.hwdec = *(char**)prop->data;
								//nxmpstats->decodingstats.videodecstats.hwdec = hwdecval;
							}
						}
						
					}
					if(mp_event->event_id!=22){
						//printf("event: %s %d\n", mpv_event_name(mp_event->event_id),mp_event->event_id);
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
					Windows::UniBrowserWindow(&item.focus, &item.first_item);
					if(item.popupstate == POPUP_STATE_FILECONTEXTMENU){
						Popups::FileContextPopup();
					}
					break;
				case MENU_STATE_USB_BROWSER:
					Windows::UniBrowserWindow(&item.focus, &item.first_item);
					if(item.popupstate == POPUP_STATE_FILECONTEXTMENU){
						Popups::FileContextPopup();
					}
					break;
				case MENU_STATE_USB_MOUNT:
					Windows::USBMountWindow(&item.focus, &item.first_item);
					break;
				case MENU_STATE_NETWORKBROWSER:
					Windows::NetworkWindow(&item.focus, &item.first_item);
					if(item.popupstate == POPUP_STATE_NETWORKMENU){
						Popups::NetMenuPopup();
					}
					break;
	            case MENU_STATE_ADDSHARE:
					Windows::ShareAddWindow(&item.focus, &item.first_item);
					break;
				case MENU_STATE_FTPBROWSER:
					Windows::UniBrowserWindow(&item.focus, &item.first_item);
					if(item.popupstate == POPUP_STATE_STARTPLAYLIST){
						Popups::PlaylistStartPlaylist();
					}
					break;
				case MENU_STATE_HTTPBROWSER:
					Windows::UniBrowserWindow(&item.focus, &item.first_item);
					if(item.popupstate == POPUP_STATE_FILECONTEXTMENU){
						Popups::FileContextPopup();
					}
					break;
				case MENU_STATE_SSHBROWSER:
					Windows::UniBrowserWindow(&item.focus, &item.first_item);
					if(item.popupstate == POPUP_STATE_FILECONTEXTMENU){
						Popups::FileContextPopup();
					}
					break;
				case MENU_STATE_SAMBABROWSER:
					Windows::UniBrowserWindow(&item.focus, &item.first_item);
					if(item.popupstate == POPUP_STATE_FILECONTEXTMENU){
						Popups::FileContextPopup();
					}
					break;
				case MENU_STATE_NFSBROWSER:
					Windows::UniBrowserWindow(&item.focus, &item.first_item);
					if(item.popupstate == POPUP_STATE_FILECONTEXTMENU){
						Popups::FileContextPopup();
					}
					break;
				case MENU_STATE_UPNPBROWSER:
					Windows::UPNPBrowserWindow(&item.focus, &item.first_item);
					//if(item.popupstate == POPUP_STATE_STARTPLAYLIST){
					//	Popups::PlaylistStartPlaylist();
					//}
					break;
				case MENU_STATE_ENIGMABROWSER:
					Windows::EnigmaWindow(&item.focus, &item.first_item);
					break;
				case MENU_STATE_SETTINGS:
					Windows::SettingsMenuWindow(&item.focus, &item.first_item);
					if(item.popupstate == POPUP_STATE_SAVE_SETTINGS){
						Popups::SaveSettingsPopup();
					}
					break;
				case MENU_STATE_PLAYLISTBROWSER:
					Windows::PlaylistWindow(&item.focus, &item.first_item);
					if(item.popupstate == POPUP_STATE_PLAYLISTCONTEXTMENU){
						Popups::PlaylistContextPopup();
					}
					break;
				case MENU_STATE_INFO:
					Windows::InfoMenuWindow(&item.focus, &item.first_item);
					break;
				case MENU_STATE_MTPSERVER:
					Windows::MTPServerWindow(&item.focus, &item.first_item);
					break;
				case MENU_STATE_PLAYERCACHING:
					playerWindows::CacheWindow();
					break;
				case MENU_STATE_PLAYER:
				
					if(item.showstats){
						playerWindows::StatsWindow();	
					}
					if(item.showdecstats){
						playerWindows::DecodingStatsWindow();	
					}
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
				case PLAYER_RIGHT_MENU_ANIME4K:
					//playerWindows::RightHomeAnime4K(&item.rightmenu_focus,&item.rightmenu_first_item);
					break;
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
		//mpv_render_context_render(libmpv->getContext(), params); // this "renders" to the video_framebuffer "linked by ID" in the params_fbo - BLOCKING
         
		mpv_render_context_render(libmpv->getContext(), params); // this "renders" to the video_framebuffer "linked by ID" in the params_fbo - BLOCKING
         
		 /*
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
		*/
		
    	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(window);

	}

	int RenderLoop(void) {
		if(dbUpdated){
			item.popupstate = POPUP_STATE_DBUPDATED;
		}
		item.first_item = true;
		while (!renderloopdone && appletMainLoop())
		{
			HandleEvents();
			HandleLayers();
			HandleRender();
			//if(dochangethemefont){
			//	changeFontTheme();
			//	dochangethemefont = false;
			//}
			
			nxmpstats->UpdateStats();
			if(GUI::wakeup == 0){
				nxmpstats->UpdateStats();
				batteryPercent = nxmpstats->batpercentage;
			}
			GUI::wakeup = 0;

    		//rewrite switch state
			AppletOperationMode stus=appletGetOperationMode();
			if (stus == AppletOperationMode_Handheld) {
				if (isHandheld == false) {
					isHandheld=true;
					NXLOG::DEBUGLOG("changed to Handheld Mode.\n");
					newResW = handheldWidth;
					newResH = handheldHeight;
					multiplyRes = 1.0f;
					currFontsize = 20.0f;
					SDL_SetWindowSize(window, newResW, newResH);
					fbo = {
						.fbo = 0,
						.w = newResW,
						.h = newResH,
					};
					reinit();
				}
			}
			if (stus == AppletOperationMode_Console) {
				if (isHandheld == true) {
					isHandheld=false;
					NXLOG::DEBUGLOG("changed to Docked Mode.\n");
					newResW = dockedWidth;
					newResH = dockedHeight;
					multiplyRes = 1.5f;
					currFontsize = 30.0f;
					SDL_SetWindowSize(window, newResW, newResH);
					fbo = {
						.fbo = 0,
						.w = newResW,
						.h = newResH,
					};
					reinit();
				}
			}
			
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
		NXLOG::DEBUGLOG("Init MPV\n");
		
		NXLOG::DEBUGLOG("Init SDL\n");
		ImGui_ImplSDL2_InitForOpenGL(window, context);
        NXLOG::DEBUGLOG("Init OPENGL\n");
		const char* glsl_version = "#version 430 core";
		ImGui_ImplOpenGL3_Init(glsl_version);
		
		Utility::FontLoader("romfs:/DejaVuSans.ttf",currFontsize,io,fontSmall);
		
		
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
	
}	

/*
static void on_mpv_render_update(void *ctx)
{
    // we set the wakeup flag here to enable the mpv_render_context_render path in the main loop.
    GUI::wakeup = 1;
}
*/

