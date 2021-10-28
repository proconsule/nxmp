#include <algorithm>
#include <iostream>

#include "gui.h"
#include "localfiles.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

MenuItem item;

namespace GUI {
	enum SDL_KEYS {
		SDL_KEY_A, SDL_KEY_B, SDL_KEY_X, SDL_KEY_Y,
		SDL_KEY_LSTICK, SDL_KEY_RSTICK,
		SDL_KEY_L, SDL_KEY_R,
		SDL_KEY_ZL, SDL_KEY_ZR,
		SDL_KEY_PLUS, SDL_KEY_MINUS,
		SDL_KEY_DLEFT, SDL_KEY_DUP, SDL_KEY_DRIGHT, SDL_KEY_DDOWN,
		SDL_KEY_LSTICK_LEFT, SDL_KEY_LSTICK_UP, SDL_KEY_LSTICK_RIGHT, SDL_KEY_LSTICK_DOWN,
		SDL_KEY_RSTICK_LEFT, SDL_KEY_RSTICK_UP, SDL_KEY_RSTICK_RIGHT, SDL_KEY_RSTICK_DOWN,
		SDL_KEY_SL_LEFT, SDL_KEY_SR_LEFT, SDL_KEY_SL_RIGHT, SDL_KEY_SR_RIGHT
	};
	
	static void on_mpv_events(void *ctx)
	{
		SDL_Event event = {.type = wakeup_on_mpv_events};
		SDL_PushEvent(&event);
	}
	
	void initMpv(){
		libmpv = new libMpv("mpv");
		wakeup_on_mpv_render_update = SDL_RegisterEvents(1);
		wakeup_on_mpv_events        = SDL_RegisterEvents(1);
		//mpv_set_wakeup_callback(libmpv->getHandle(), [](void *) -> void {SDL_Event event = {.type = wakeup_on_mpv_events}; SDL_PushEvent(&event);}, NULL);
		mpv_set_wakeup_callback(libmpv->getHandle(), on_mpv_events, NULL);

		mpv_render_context_set_update_callback(libmpv->getContext(), [](void *) -> void { SDL_Event event = {.type = wakeup_on_mpv_render_update}; SDL_PushEvent(&event); }, NULL);
	
	}
	
	void toggleStats(){
		const char *cmd[] = {"script-binding","stats/display-stats-toggle" ,NULL};
		mpv_command_async(libmpv->getHandle(), 0, cmd);
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
	
	
	void HandleEvents(){
		SDL_Event event;
			while (SDL_PollEvent(&event)) {
				ImGui_ImplSDL2_ProcessEvent(&event);
#ifndef __SWITCH__
				if (event.type == SDL_KEYDOWN) {
					Uint8 keycode = event.key.keysym.sym;
					if(keycode == SDLK_ESCAPE){
						SDL_Event sdlevent;
						sdlevent.type = SDL_JOYBUTTONDOWN;
						sdlevent.jbutton.button = SDL_KEY_PLUS;
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
					
				}
#endif
				if (event.type == SDL_JOYBUTTONDOWN) {
					
					Uint8 button = event.jbutton.button;
					if (button == SDL_KEY_PLUS)
						renderloopdone = true;
					
					if (button == SDL_KEY_RSTICK){
						if(item.state == MENU_STATE_PLAYER){
							toggleMasterLock();
						}
					}
					if (button == SDL_KEY_MINUS){
						if(item.state == MENU_STATE_PLAYER && !item.masterlock){
							toggleStats();
						}
					}
					
					if (button == SDL_KEY_ZR){
						if(item.state == MENU_STATE_PLAYER && !item.masterlock){
							if(item.playershowcontrols){
								libmpv->seekOSD(libmpv->getPosition() + 60.0);
							}else{
								libmpv->seekSilent(libmpv->getPosition() + 60.0);
							}
						}
						
					}
					if (button == SDL_KEY_ZL){
						if(item.state == MENU_STATE_PLAYER && !item.masterlock){
							if(item.playershowcontrols){
								libmpv->seekOSD(libmpv->getPosition() - 60.0);
							}else{
								libmpv->seekSilent(libmpv->getPosition() - 60.0);
							}
						}
						
					}
					if (button == SDL_KEY_R){
						if(item.state == MENU_STATE_PLAYER && !item.masterlock){
							if(item.playershowcontrols){
								libmpv->seekOSD(libmpv->getPosition() + 10.0);
							}else{
								libmpv->seekSilent(libmpv->getPosition() + 10.0);
							}
						}
						
					}
					if (button == SDL_KEY_L){
						if(item.state == MENU_STATE_PLAYER && !item.masterlock){
							if(item.playershowcontrols){
								libmpv->seekOSD(libmpv->getPosition() - 10.0);
							}else{
								libmpv->seekSilent(libmpv->getPosition() - 10.0);
							}
						}
						
					}
					if (button == SDL_KEY_X){
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
							if(libmpv->Paused()){
								libmpv->Resume();
							}else{
								libmpv->Pause();
							}
						}
					}
					if (button == SDL_KEY_Y && !item.masterlock){
						if(item.state != MENU_STATE_HOME && item.state != MENU_STATE_PLAYER){
#ifdef __SWITCH__
							if(usbmounter != nullptr){
								delete usbmounter;
								usbmounter = nullptr;
							}
#endif
							if(ftpdir != nullptr){
								delete ftpdir;
								ftpdir = nullptr;
							}
							if(httpdir != nullptr){
								delete httpdir;
								httpdir = nullptr;
							}
							if(enigma2 != nullptr){
								delete enigma2;
								enigma2 = nullptr;
							}
							item.localpath = configini->getStartPath();
							item.networkselect = true;
							item.first_item = true;
							item.state = MENU_STATE_HOME;
							item.usbpath = "";
							
						}
										
					}
					if (button == SDL_KEY_B){
						if(item.state == MENU_STATE_ENIGMABROWSER && libmpv->Stopped()){
							item.first_item = true;
							enigma2->backToTop();
						}
						
						if(item.state == MENU_STATE_NETWORKBROWSER && libmpv->Stopped()){
							item.first_item = true;
							urlschema thisurl = Utility::parseUrl(item.networkurl);
							if(thisurl.scheme == "http" || thisurl.scheme == "https"){
								item.networkentries = httpdir->getDir(httpdir->backDir(),Utility::getMediaExtensions());
								item.networklastpath = httpdir->getCurrPath();
							}
							if(thisurl.scheme == "ftp"){
								item.networkentries = ftpdir->getDir(ftpdir->backDir(),Utility::getMediaExtensions());
								item.networklastpath = ftpdir->getCurrPath();
							}
							
						}
						
						if(item.state == MENU_STATE_USB && libmpv->Stopped()){
							if(item.usbbasepath != ""){
								item.first_item = true;
								if(item.usbpath != item.usbbasepath){
									item.usbpath = item.usbpath.substr(0, item.usbpath.find_last_of("\\/"));
									if(item.usbpath == "")item.usbpath=item.usbbasepath;
								}
								
								item.usbfileentries = FS::getDirList(item.usbpath.c_str(),true,Utility::getMediaExtensions());
							}
						}
						
						if(item.state == MENU_STATE_FILEBROWSER && libmpv->Stopped()){
							item.first_item = true;
							item.localpath = FS::backPath(item.localpath);
							item.localfileentries = FS::getDirList(item.localpath,true,Utility::getMediaExtensions());
							
						}
						
						if(!libmpv->Stopped()  && !item.masterlock){
							item.state = item.laststate;
							libmpv->Stop();
						}
					}
				}
				if (event.type == wakeup_on_mpv_render_update)
				{
					mpv_render_context_update(libmpv->getContext());
				}
				if (event.type == wakeup_on_mpv_events)
				{
					int count = 20;
					while (count) {
					mpv_event *mp_event = (mpv_event*) mpv_wait_event(libmpv->getHandle(), 0);
					if (mp_event->event_id == MPV_EVENT_NONE)
						break;
					if (mp_event->event_id == MPV_EVENT_LOG_MESSAGE) {
						mpv_event_log_message *msg = (mpv_event_log_message *) mp_event->data;
						if (strstr(msg->text, "DR image"))
							printf("log: %s", msg->text);
						continue;
					}if (mp_event->event_id == MPV_EVENT_START_FILE) {
						printf("START FILE\n");
						item.laststate = item.state;
						item.state = MENU_STATE_PLAYER;
					}
					printf("Event id: %d\n",mp_event->event_id);
					if (mp_event->event_id == MPV_EVENT_END_FILE) {
						printf("END FILE\n");
						item.state = item.laststate;
						item.masterlock = false;
						printf("MENU STATE: %d\n",item.state );
					}
					
					count --;
					printf("event: %s\n", mpv_event_name(mp_event->event_id));
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
					break;
				case MENU_STATE_FILEBROWSER:
					Windows::FileBrowserWindow(&item.focus, &item.first_item);
					break;
				case MENU_STATE_USB:
					Windows::USBBrowserWindow(&item.focus, &item.first_item);
					break;
				case MENU_STATE_NETWORKBROWSER:
					Windows::NetworkWindow(&item.focus, &item.first_item);
					break;
				case MENU_STATE_ENIGMABROWSER:
					Windows::EnigmaWindow(&item.focus, &item.first_item);
					break;
				case MENU_STATE_SETTINGS:
					Windows::SettingsMenuWindow(&item.focus, &item.first_item);
					break;
				case MENU_STATE_INFO:
					Windows::InfoMenuWindow(&item.focus, &item.first_item);
					break;
				case MENU_STATE_PLAYER:
					if(item.playershowcontrols){
						//PlayerWindows::PlayerControls();
					}
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
	
		while (!renderloopdone)
		{
			HandleEvents();
			HandleLayers();
			HandleRender();
		}
		
		return 0;	
	}

}	
					