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
	
	void initMpv(){
		mpv = new Mpv("mpv", true);
		wakeup_on_mpv_render_update = SDL_RegisterEvents(1);
		wakeup_on_mpv_events        = SDL_RegisterEvents(1);
		mpv_set_wakeup_callback(mpv->getHandle(), [](void *) -> void {SDL_Event event = {.type = wakeup_on_mpv_events}; SDL_PushEvent(&event);}, NULL);
		mpv_render_context_set_update_callback(mpv->getContext(), [](void *) -> void { SDL_Event event = {.type = wakeup_on_mpv_render_update}; SDL_PushEvent(&event); }, NULL);


		
	}
	
	void HandleEvents(){
		SDL_Event event;
			while (SDL_PollEvent(&event)) {
				ImGui_ImplSDL2_ProcessEvent(&event);
				if (event.type == SDL_JOYBUTTONDOWN) {
					Uint8 button = event.jbutton.button;
					if (button == SDL_KEY_PLUS)
						renderloopdone = true;
				}
				if (event.type == SDL_JOYBUTTONDOWN) {
					Uint8 button = event.jbutton.button;
					if (button == SDL_KEY_ZR){
						if(item.state == MENU_STATE_PLAYER){
							if(item.playershowcontrols){
								mpv->seekOSD(mpv->getPosition() + 60.0);
							}else{
								mpv->seek(mpv->getPosition() + 60.0);
							}
						}
						
					}
					if (button == SDL_KEY_ZL){
						if(item.state == MENU_STATE_PLAYER){
							if(item.playershowcontrols){
								mpv->seekOSD(mpv->getPosition() - 60.0);
							}else{
								mpv->seek(mpv->getPosition() - 60.0);
							}
						}
						
					}
					if (button == SDL_KEY_R){
						if(item.state == MENU_STATE_PLAYER){
							if(item.playershowcontrols){
								mpv->seekOSD(mpv->getPosition() + 10.0);
							}else{
								mpv->seek(mpv->getPosition() + 10.0);
							}
						}
						
					}
					if (button == SDL_KEY_L){
						if(item.state == MENU_STATE_PLAYER){
							if(item.playershowcontrols){
								mpv->seekOSD(mpv->getPosition() - 10.0);
							}else{
								mpv->seek(mpv->getPosition() - 10.0);
							}
						}
						
					}
					if (button == SDL_KEY_X){
						if(item.state == MENU_STATE_PLAYER){
							if(item.playershowcontrols){
								item.playershowcontrols=false;
								mpv_set_option_string(mpv->getHandle(), "osd-level", "0");
								
							}else{
								item.playershowcontrols=true;
								mpv_set_option_string(mpv->getHandle(), "osd-level", "3");
							}
						}
					}
					if (button == SDL_KEY_A){
						if(item.state == MENU_STATE_FILEBROWSER){
							if(item.browserstate == BROWSER_STATE_LOCAL){
								FS::GetDirList(item.localpath.c_str(),item.localfileentries);
							}
						}
						if(item.state == MENU_STATE_PLAYER){
							if(mpv->isPaused()){
								mpv->resume();
							}else{
								mpv->pause();
							}
						}
					}
					if (button == SDL_KEY_Y){
						if(item.state == MENU_STATE_FILEBROWSER){
							item.networkselect = true;
							item.browserstate = BROWSER_STATE_HOME;
						}
						
					}
					if (button == SDL_KEY_B){
						if(item.state == MENU_STATE_FILEBROWSER && mpv->isStopped()){
							if(item.browserstate == BROWSER_STATE_ENIGMA2){
								if(item.enigma2bouquet != ""){
									item.enigma2bouquet = "";
								}
							}
							if(item.browserstate == BROWSER_STATE_LOCAL){
								if(item.localpath != "/"){
									item.localpath = item.localpath.substr(0, item.localpath.find_last_of("\\/"));
									if(item.localpath == "")item.localpath="/";
								}
								FS::GetDirList(item.localpath.c_str(),item.localfileentries);
							}
							if(item.browserstate == BROWSER_STATE_NETWORK){
								if(item.networklastpath != "/"){
									item.networklastpath = item.networklastpath.substr(0, item.networklastpath.find_last_of("\\/"));
									if(item.networklastpath == "")item.networklastpath="/";
								}
								urlschema thisurl = Utility::parseUrl(item.networkurl);
								if(thisurl.scheme == "ftp"){
											netbuf *ftp_con = nullptr;
											std::string ftphost = thisurl.server+std::string(":21");
											if (!FtpConnect(ftphost.c_str(), &ftp_con)) {
												printf("could not connect to ftp server\n");
											}else{
											
												if (!FtpLogin(thisurl.user.c_str(), thisurl.pass.c_str(), ftp_con)) {
												printf("could not connect to ftp server\n");
													FtpQuit(ftp_con);
												}else{
												
													item.networkentries = FtpDirList(item.networklastpath.c_str(), ftp_con);
												}
											}
								}
							
							}
						}
						
						if(!mpv->isStopped()){
							mpv->stop();
						}
					}
				}
				if (event.type == wakeup_on_mpv_render_update)
				{
					mpv_render_context_update(mpv->getContext());
				}
				if (event.type == wakeup_on_mpv_events)
				{
					int count = 10;
					while (count) {
					mpv_event *mp_event = (mpv_event*) mpv_wait_event(mpv->getHandle(), 0);
					if (mp_event->event_id == MPV_EVENT_NONE)
						break;
					if (mp_event->event_id == MPV_EVENT_LOG_MESSAGE) {
						mpv_event_log_message *msg = (mpv_event_log_message *) mp_event->data;
						if (strstr(msg->text, "DR image"))
							printf("log: %s", msg->text);
						continue;
					}if (mp_event->event_id == MPV_EVENT_START_FILE) {
						printf("START FILE\n");
						item.state = MENU_STATE_PLAYER;
					}
					if (mp_event->event_id == MPV_EVENT_END_FILE) {
						printf("END FILE\n");
						item.state = MENU_STATE_FILEBROWSER;
					}
					
					count --;
					printf("event: %s\n", mpv_event_name(mp_event->event_id));
					}


				}
				

			}
		
	}
	
	void HandleLayers(){
		bool focus = false, first_item = true;
		ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();
		//if(mpv->isStopped()){
			//GUI
			switch (item.state) {
				case MENU_STATE_FILEBROWSER:
					Windows::FileBrowserWindow(&focus, &first_item);
					break;
				case MENU_STATE_PLAYER:
					if(item.playershowcontrols){
						//PlayerWindows::PlayerControls();
					}
					break;
				
				
			}
			
			
		//}
		
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
			mpv_render_context_render(mpv->getContext(), params);
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
					