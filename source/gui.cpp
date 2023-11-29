#include <algorithm>
#include <iostream>

#include "gui.h"
#include "localfiles.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "SwitchSys.h"

#include "GLFW/glfw3.h"


MenuItem item;


bool B_A_PRESS = false;
bool B_B_PRESS = false;
bool B_X_PRESS = false;
bool B_Y_PRESS = false;
bool B_DU_PRESS = false;
bool B_DD_PRESS = false;
bool B_DL_PRESS = false;
bool B_DR_PRESS = false;

bool B_TL_PRESS = false;
bool B_TR_PRESS = false;


bool B_R_PRESS = false;
bool B_L_PRESS = false;
bool B_ZR_PRESS = false;
bool B_ZL_PRESS = false;

bool B_PLUS_PRESS = false;
bool B_MINUS_PRESS = false;

bool mpvevent = false;

#define is_bit_set(val, bit_no) (((val) >> (bit_no)) & 1)


namespace GUI {
	
	const int JOYSTICK_DEAD_ZONE = 8000;
	const int JOYSTICK_EXTENDED_DEAD_ZONE = 32726;
	
	int wakeup = 0;
	
	bool GUI_REINIT = false;
	
	void MpvEvents(void *ctx);
	void MpvUpdate();
	
	static void on_mpv_events(void *ctx)
	{
		mpvevent = true;
	}
	
	static void on_mpv_update(void *ctx)
	{
		
		GUI::wakeup = 1;
	}
	
	
	
	void initMpv(){

		libmpv = new libMpv("mpv");
		mpv_set_wakeup_callback(libmpv->getHandle(), on_mpv_events, NULL);
		mpv_render_context_set_update_callback(libmpv->getContext(),  on_mpv_update, nullptr);
		if(slaveplayer){
			libmpv->loadFile(slaveplayer_file.c_str());
			item.state = MENU_STATE_GUILESS;
		}

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

	void MpvUpdate(){
		//mpv_render_context_update(libmpv->getContext());
		//GUI::wakeup = 1;
		 
	}

	
	
	void HandleEvents(){
		
		while (1) {
			if(libmpv == nullptr)break;
			mpv_event *mp_event = (mpv_event*) mpv_wait_event(libmpv->getHandle(), 0);
			if (mp_event->event_id == MPV_EVENT_NONE){
				break;
			}
			if (mp_event->event_id == MPV_EVENT_SHUTDOWN){
				break;
			}
			if (mp_event->event_id == MPV_EVENT_LOG_MESSAGE) {
				mpv_event_log_message *msg = (mpv_event_log_message *) mp_event->data;
				continue;
			}
			if (mp_event->event_id == MPV_EVENT_FILE_LOADED) {
				
				libmpv->getfileInfo();
				item.state = MENU_STATE_PLAYER;
				appletSetMediaPlaybackState(true);
				
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
					mpv_observe_property(libmpv->getHandle(), 0, "paused-for-cache", MPV_FORMAT_FLAG);
					mpv_observe_property(libmpv->getHandle(), 0, "demuxer-cache-duration", MPV_FORMAT_INT64);
					mpv_observe_property(libmpv->getHandle(), 0, "demuxer-cache-state", MPV_FORMAT_INT64);
	
					item.playerstate = PLAYER_STATE_VIDEO;
				}
						
						
				if(libmpv->getFileInfo()->videos.size() > 0){
					nxmpstats->decodingstats.videodecstats.codec = libmpv->getVideoCodec();
				}
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
#ifdef DEKO3D_BACKEND
				nxmpgfx::setBGAlpha(1.0f);
#endif
				if(slaveplayer){
					renderloopdone = true;
				}
				
				if(item.playerstate == PLAYER_STATE_VIDEO  && libmpv->getFileInfo()->playbackInfo.islive == false && sqlitedb != nullptr){
					if(libmpv->getFileInfoPerc() >= configini->getResumeStartPerc(false) && 100-libmpv->getFileInfoPerc() >= configini->getResumeStopPerc(false)){
						sqlitedb->writeResume(libmpv->getFileInfo()->path,libmpv->getFileInfo()->playbackInfo.position);
						if(filebrowser!=nullptr)filebrowser->ResetDbStatus();
					}else{
						if(eof->reason != MPV_END_FILE_REASON_EOF){
							if(100-libmpv->getFileInfoPerc() < configini->getResumeStopPerc(false))
							sqlitedb->markCompleted(libmpv->getFileInfo()->path);
							if(filebrowser!=nullptr)filebrowser->ResetDbStatus();
						}
					}
					
					if(eof->reason == MPV_END_FILE_REASON_EOF){
						sqlitedb->markCompleted(libmpv->getFileInfo()->path);
						if(filebrowser!=nullptr)filebrowser->ResetDbStatus();
					}
							
				}
				if(eof->reason == MPV_END_FILE_REASON_EOF){
					if(playlist != nullptr){
						if(playlist->getPlaylist().size() >0){
							if(playlist->getCurrIdx() < playlist->getPlaylist().size()-1){
								Playlist::playlist_struct nextfile = playlist->getNext();
								libmpv->loadFile(nextfile.fulluri);
								if(configini->getDbActive(true) && sqlitedb != nullptr){
									libmpv->getFileInfo()->resume = sqlitedb->getResume(nextfile.fulluri);
									if(libmpv->getFileInfo()->resume>0){
										item.popupstate = POPUP_STATE_RESUME;
									}
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
				if(prop == NULL)continue;
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
						
				if(std::string(prop->name) == "paused-for-cache") 
				{
					if(prop->format == MPV_FORMAT_FLAG)
					{
						bool playcache = *(bool *)prop->data;
						
						if(playcache){
							item.laststate = item.state;
							item.state = MENU_STATE_PLAYERCACHING;
						}else{
							item.state = MENU_STATE_PLAYER;
						}
						
								
					}
				}
						
				if(std::string(prop->name) == "demuxer-cache-duration") 
				{
					if(prop->format == MPV_FORMAT_INT64)
					{
						int64_t timepos = *(int64_t *)prop->data;
						playercachesec = timepos;
					}
				}
						
				if(std::string(prop->name) == "demuxer-cache-state") 
				{
					if(prop->format == MPV_FORMAT_INT64)
					{
						int64_t timepos = *(int64_t *)prop->data;
						playercachesize = timepos;
					}
				}
						
						
			}
			
			if(mp_event->event_id!=22){
				//printf("event: %s %d\n", mpv_event_name(mp_event->event_id),mp_event->event_id);
			}
		}
		mpvevent=false;
		
		
		uint64_t event_ret = nxmpgfx::Process_UI_Events(std::chrono::system_clock::now());
		
		if(event_ret>0){
			
			if(is_bit_set(event_ret,nxmpgfx::B_AX_R_UP)){
				if(item.state == MENU_STATE_IMGVIEWER){
					Windows::setImageZoom(Windows::getImageZoom()+0.1);
				}else{
					ImGui::GetIO().AddKeyEvent(ImGuiKey_GamepadLStickUp,true);
				}
			}
			
			if(is_bit_set(event_ret,nxmpgfx::B_AX_R_DOWN)){
				if(item.state == MENU_STATE_IMGVIEWER){
					if(Windows::getImageZoom()>=1.1f){
						Windows::setImageZoom(Windows::getImageZoom()-0.1);
					}
				}else{
					ImGui::GetIO().AddKeyEvent(ImGuiKey_GamepadLStickDown,true);
				}
			}
			
			if(is_bit_set(event_ret,nxmpgfx::B_AX_L_UP)){
				if(item.state == MENU_STATE_IMGVIEWER){
					
				}
			}
			
			if(is_bit_set(event_ret,nxmpgfx::B_AX_L_DOWN)){
				if(item.state == MENU_STATE_IMGVIEWER){
					
				}
			}
			
			if(is_bit_set(event_ret,nxmpgfx::B_AX_L_LEFT)){
				if(item.state == MENU_STATE_IMGVIEWER){
					
				}
				ImGui::GetIO().AddKeyEvent(ImGuiKey_GamepadDpadLeft,true);
			}
			
			if(is_bit_set(event_ret,nxmpgfx::B_AX_L_RIGHT)){
				if(item.state == MENU_STATE_IMGVIEWER){
					
				}
				ImGui::GetIO().AddKeyEvent(ImGuiKey_GamepadDpadRight,true);
			}
			
			if(is_bit_set(event_ret,nxmpgfx::BUT_Y)){
				if(item.state != MENU_STATE_HOME && item.state != MENU_STATE_PLAYER && item.popupstate == POPUP_STATE_NONE){
			
					if(item.selectionstate == FILE_SELECTION_CHECKBOX){
						item.selectionstate =FILE_SELECTION_NONE;
					}
					if(filebrowser != nullptr){
						delete filebrowser;
						filebrowser = nullptr;
					}
							
					if(MyUSBMount != nullptr && libmpv->Stopped() && !playlist->HaveUSBEntrys()){
						delete MyUSBMount;
						MyUSBMount = nullptr;
					}
					if(NewNetworkShare != nullptr){
						delete NewNetworkShare;
						NewNetworkShare = nullptr;
					}
							
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
					if(item.state == MENU_STATE_SETTINGS){
						if(Windows::settingsview_combopopup==-1){
							if(configini->Modified()){
								item.popupstate = POPUP_STATE_SAVE_SETTINGS;
							}else{
								item.state = MENU_STATE_HOME;
							}
						}
					}else{
						item.state = MENU_STATE_HOME;
					}
							
				}
			}
			
			if (is_bit_set(event_ret,nxmpgfx::BUT_DUP) || is_bit_set(event_ret,nxmpgfx::B_AX_L_UP)){
				if(item.state == MENU_STATE_PLAYER && !item.masterlock && item.rightmenustate == PLAYER_RIGHT_MENU_PLAYER && item.popupstate == POPUP_STATE_NONE){
					
				}
			}
			
			if (is_bit_set(event_ret,nxmpgfx::BUT_DDOWN) || is_bit_set(event_ret,nxmpgfx::B_AX_L_DOWN)){
				if(item.state == MENU_STATE_PLAYER && !item.masterlock && item.rightmenustate == PLAYER_RIGHT_MENU_PLAYER && item.popupstate == POPUP_STATE_NONE){
					if(item.playercontrolstate == PLAYER_CONTROL_STATE_NONE){
						item.playercontrolstate = PLAYER_CONTROL_STATE_CONTROLS;
								
					}else{
						item.playercontrolstate = PLAYER_CONTROL_STATE_NONE;
					}
				}
			}
			
			if (is_bit_set(event_ret,nxmpgfx::BUT_X)){
				if(item.state == MENU_STATE_FILEBROWSER || item.state == MENU_STATE_FTPBROWSER || item.state == MENU_STATE_HTTPBROWSER || item.state == MENU_STATE_USB_BROWSER || item.state == MENU_STATE_SSHBROWSER || item.state == MENU_STATE_SAMBABROWSER || item.state == MENU_STATE_NFSBROWSER){
					if(FilePopupTextScroller == nullptr){
							FilePopupTextScroller = new CTextScroller("##filepopuptextscroll");
					}
					FilePopupTextScroller->ResetPosition();
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
			
			if (is_bit_set(event_ret,nxmpgfx::BUT_A)){
				if(item.state == MENU_STATE_PLAYER){
					if(item.rightmenustate == PLAYER_RIGHT_MENU_PLAYER && item.playercontrolstate == PLAYER_CONTROL_STATE_NONE && item.popupstate == POPUP_STATE_NONE){
						if(libmpv->Paused()){
							libmpv->Resume();
						}else{
							libmpv->Pause();
						}
					}
				}else if(item.state == MENU_STATE_IMGVIEWER){
					Windows::setImageZoom(1.0f);
				}
				
			}
			if (is_bit_set(event_ret,nxmpgfx::BUT_B)){
				
					if(item.state == MENU_STATE_IMGVIEWER){
						item.state = item.laststate;
					}else if(item.state == MENU_STATE_APPEXIT){
						item.state = MENU_STATE_HOME;
					}else if(item.state == MENU_STATE_SETTINGS && Windows::settingsview_combopopup != -1){
						Windows::settingsview_combopopup = -1;
					}else if(item.selectionstate == FILE_SELECTION_CHECKBOX){
						item.selectionstate =FILE_SELECTION_NONE;
					}else if(item.state == MENU_STATE_FILEBROWSER || item.state == MENU_STATE_FTPBROWSER || item.state == MENU_STATE_HTTPBROWSER || item.state == MENU_STATE_SSHBROWSER || item.state == MENU_STATE_SAMBABROWSER || item.state == MENU_STATE_NFSBROWSER|| item.state == MENU_STATE_USB_BROWSER){
						if(item.popupstate != POPUP_STATE_NONE){
							item.popupstate = POPUP_STATE_NONE;
						}else{
							item.first_item = true;
							std::string mybackdir = filebrowser->backDir();
							Windows::SetBrowserNav(mybackdir);
							filebrowser->DirList(filebrowser->getCurrentPath(),configini->getshowHidden(false),Utility::getMediaExtensions());
							
						}
							
					}else if(item.state == MENU_STATE_ENIGMABROWSER){
						item.first_item = true;
						enigma2->backToTop();
					}else if(item.state == MENU_STATE_UPNPBROWSER){
						item.first_item = true;
						if(nxupnp->getSelDevice()>-1){
							nxupnp->getDevice(nxupnp->getSelDevice())->back();
							
							nxupnp->getDevice(nxupnp->getSelDevice())->browseOID();
						}
					}else if(item.state == MENU_STATE_USB_MOUNT){
						
						
					}else if(item.state == MENU_STATE_PLAYER || item.state == MENU_STATE_PLAYERCACHING){
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
						else if(item.rightmenustate == PLAYER_RIGHT_MENU_ANIME4K){
							item.rightmenustate = PLAYER_RIGHT_MENU_HOME;
						}
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
			
			if (is_bit_set(event_ret,nxmpgfx::BUT_DRIGHT)){
				
				if(item.state == MENU_STATE_SETTINGS && Windows::settingsview_combopopup == -1 && item.popupstate == POPUP_STATE_NONE){
					Windows::settingsview_page = true;
				}
				
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
			
			if (is_bit_set(event_ret,nxmpgfx::BUT_DLEFT)){
				
				if(item.state == MENU_STATE_SETTINGS && Windows::settingsview_combopopup == -1 && item.popupstate == POPUP_STATE_NONE){
					Windows::settingsview_page = false;
				}
				
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
			
			if (is_bit_set(event_ret,nxmpgfx::BUT_ZR)){
				if(item.state == MENU_STATE_PLAYER && !item.masterlock){
					libmpv->seek(libmpv->getPosition() + configini->getLongSeek(false),item.playershowcontrols);
				}
						
			}
			if (is_bit_set(event_ret,nxmpgfx::BUT_ZL)){
				if(item.state == MENU_STATE_PLAYER && !item.masterlock){
					libmpv->seek(libmpv->getPosition() - configini->getLongSeek(false),item.playershowcontrols);
						
				}
						
			}
			
			if (is_bit_set(event_ret,nxmpgfx::BUT_R)){
				
				
				
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
			
			if (is_bit_set(event_ret,nxmpgfx::BUT_L)){
				
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
			
			if (is_bit_set(event_ret,nxmpgfx::BUT_TL) && is_bit_set(event_ret,nxmpgfx::BUT_TR)){
				if(item.state == MENU_STATE_HOME){
					configini->toggleConsoleWindow();
				}
			}
			
			if (is_bit_set(event_ret,nxmpgfx::BUT_TL)){
				if(item.state == MENU_STATE_PLAYER && !item.masterlock){
					toggleOC();
				}
						
			}
			if (is_bit_set(event_ret,nxmpgfx::BUT_TR)){
				if(item.state == MENU_STATE_PLAYER){
					toggleMasterLock();
				}
						
			}
				
			if (is_bit_set(event_ret,nxmpgfx::BUT_PLUS)){
				if(item.state == MENU_STATE_PLAYER && !item.masterlock && item.rightmenustate == PLAYER_RIGHT_MENU_PLAYER){
					item.savestate = item.laststate;
					item.state = item.laststate;	
					nxmpgfx::SetFullScreen(false);
#ifdef DEKO3D_BACKEND
					nxmpgfx::setBGAlpha(0.3f);
#endif
				}else if(item.state != MENU_STATE_PLAYER && !libmpv->Stopped()){
					std::cout << std::endl <<" State is?: " << item.state << std::endl;
					//fix crash
					if (item.state != item.savestate)
					{
								
						item.state = MENU_STATE_FILEBROWSER;
						if(filebrowser==NULL){
							filebrowser = new CFileBrowser(configini->getStartPath(),playlist);
						}
						filebrowser->DirList(configini->getStartPath(),true,Utility::getMediaExtensions());
						item.first_item = true;
					}
					nxmpgfx::SetFullScreen(true);
					item.state = MENU_STATE_PLAYER;
#ifdef DEKO3D_BACKEND
					nxmpgfx::setBGAlpha(1.0f);
#endif
							
				}
			}
			
			if(is_bit_set(event_ret,nxmpgfx::B_AX_R_DOWN)){
				if(item.state == MENU_STATE_PLAYER && !item.masterlock && item.rightmenustate == PLAYER_RIGHT_MENU_PLAYER){
					item.showVolume = true;
					ImGuiContext& g = *GImGui;
					item.VolumeHide = g.Time;
					if(libmpv->getVolume()-1>=0){
						libmpv->setVolume(libmpv->getVolume()-1,false);
					}
				}
			}
			if(is_bit_set(event_ret,nxmpgfx::B_AX_R_UP)){
				if(item.state == MENU_STATE_PLAYER && !item.masterlock && item.rightmenustate == PLAYER_RIGHT_MENU_PLAYER){
					item.showVolume = true;
					ImGuiContext& g = *GImGui;
					item.VolumeHide = g.Time;
					if(libmpv->getVolume()+1<=100){
						libmpv->setVolume(libmpv->getVolume()+1,false);
					}
				}
			}
		}else{
			ImGui::GetIO().AddKeyEvent(ImGuiKey_GamepadLStickUp,false);
			ImGui::GetIO().AddKeyEvent(ImGuiKey_GamepadLStickDown,false);
		}
	
	}
	
	void HandleLayers(){
		nxmpgfx::NewFrame();
        ImGui::NewFrame();
			if(item.state == MENU_STATE_PLAYER){
				nxmpgfx::Draw_VO();
			}
		
			switch (item.state) {
				case MENU_STATE_HOME:
					Windows::MainMenuWindow(&item.focus, &item.first_item);
					if(item.popupstate == POPUP_STATE_DBUPDATED){
						Popups::DBUpdatedPopup();
					}
					break;
				case MENU_STATE_FILEBROWSER:
				case MENU_STATE_FTPBROWSER:
				case MENU_STATE_HTTPBROWSER:
				case MENU_STATE_SSHBROWSER:
				case MENU_STATE_SAMBABROWSER:
				case MENU_STATE_NFSBROWSER:
				case MENU_STATE_USB_BROWSER:
					Windows::UniBrowserWindow(/*&item.focus, &item.first_item*/);
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
					Windows::SettingsUIWindow(&item.focus, &item.first_item);
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
				case MENU_STATE_IMGVIEWER:
					Windows::imageViewer();
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
				
				case MENU_STATE_APPEXIT:
					Windows::MainMenuWindow(&item.focus, &item.first_item);
					Popups::AppExitPopup();
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
			
			if(ConsoleWindow != nullptr){
				if(configini->getConsoleWindow()){
					ConsoleWindow->Draw();
				}
			}
			
	}
	
	void HandleRender(){
		
		
		nxmpgfx::Render_PreMPV();
		
		if(GUI::wakeup == 1){


			GUI::wakeup = 0;
		}
#ifdef OPENGL_BACKEND
			mpv_render_context_render(libmpv->getContext(), nxmpgfx::getMPV_Params()); // this "renders" to the video_framebuffer "linked by ID" in the params_fbo - BLOCKING
			mpv_render_context_report_swap(libmpv->getContext());
#endif	
#ifdef DEKO3D_BACKEND
			
			nxmpgfx::UpdateFBO();
			mpv_render_context_render(libmpv->getContext(), nxmpgfx::getMPV_Params());
			nxmpgfx::queueWaitDoneFence();
			mpv_render_context_report_swap(libmpv->getContext());
			
			
			
#endif	
		
		nxmpgfx::Render_PostMPV();

	}
	

	int RenderLoop(void) {
		if(dbUpdated){
			item.popupstate = POPUP_STATE_DBUPDATED;
		}
		item.first_item = true;
		while (!renderloopdone && appletMainLoop() && !nxmpgfx::WindowShouldClose())
		{
			
			AppletOperationMode stus=appletGetOperationMode();
			if (stus == AppletOperationMode_Handheld) {
				if (isHandheld == false) {
					isHandheld=true;
					NXLOG::DEBUGLOG("changed to Handheld Mode.\n");
					newResW = handheldWidth;
					newResH = handheldHeight;
					multiplyRes = 1.0f;
					currFontsize = 20.0f;
					
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
					
					reinit();
					
				}
			}
			
			
			nxmpstats->starttime = std::chrono::system_clock::now();
			HandleEvents();
			nxmpstats->eventtime = std::chrono::system_clock::now();
			HandleLayers();
			nxmpstats->layertime = std::chrono::system_clock::now();
			HandleRender();
			nxmpstats->rendertime = std::chrono::system_clock::now();
			
			
			if(GUI::wakeup == 0){
				batteryPercent = nxmpstats->batpercentage;
			}
			

    		//rewrite switch state
			
			
			
			
			
			if(GUI_REINIT){
				reinit();
				GUI_REINIT=false;
			}
			
			
			
			
			nxmpstats->endtime = std::chrono::system_clock::now();
			nxmpstats->CalcDelay();
		}
		
		return 0;	
	}
    void reinit()
	{
		
		
		nxmpgfx::Resize(newResW,newResH);
		nxmpgfx::Resize_VO(newResW,newResH);

		
		
		
	}
	
}	
