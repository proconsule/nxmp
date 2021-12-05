#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"


namespace Windows {
    void MainMenuWindow(bool *focus, bool *first_item) {
        Windows::SetupMainWindow();
		std::vector<std::string> topmenu = configini->topmenu;
		if (ImGui::Begin(nxmpTitle.c_str(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
				static int selected = -1;					
				for (unsigned int n = 0; n < topmenu.size(); n++){
					std::string itemid = "##" + std::to_string(n);
					if(topmenu[n] == "Local Files"){
						ImGui::Image((void*)(intptr_t)SdCardTexture.id, ImVec2(40,40));
					}
					else if(topmenu[n] == "USB"){
						ImGui::Image((void*)(intptr_t)UsbTexture.id, ImVec2(40,40));
					}
					else if(topmenu[n] == "Network"){
						ImGui::Image((void*)(intptr_t)NetworkTexture.id, ImVec2(40,40));
					}
					else if(topmenu[n] == "Enigma2"){
						ImGui::Image((void*)(intptr_t)Enigma2Texture.id, ImVec2(40,40));
					}
					else if(topmenu[n] == "Playlist"){
						ImGui::Image((void*)(intptr_t)PlaylistTexture.id, ImVec2(40,40));
					}
					else if(topmenu[n] == "Info"){
						ImGui::Image((void*)(intptr_t)InfoTexture.id, ImVec2(40,40));
					}
					else if(topmenu[n] == "Settings"){
						ImGui::Image((void*)(intptr_t)SettingsTexture.id, ImVec2(40,40));
					}
					else if(topmenu[n] == "Exit"){
						ImGui::Image((void*)(intptr_t)ExitTexture.id, ImVec2(40,40));
					}
							
							
					ImGui::SameLine();
					ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize) / 2});
								
					if (ImGui::Selectable(itemid.c_str(), selected == n)){
						if(topmenu[n] == "Local Files"){
							item.state = MENU_STATE_FILEBROWSER;
							localdir = new localFs(configini->getStartPath(),playlist);
							localdir->DirList(configini->getStartPath(),true,Utility::getMediaExtensions());
							item.first_item = true;
						}
						if(topmenu[n] == "USB"){
#ifdef __SWITCH__
							usbInit();
							item.state = MENU_STATE_USB;
							if(usbmounter == nullptr){
								usbmounter = new USBMounter(playlist);
							}
							
#endif
						}
						if(topmenu[n] == "Network"){
							item.networksources.clear();
							item.networksources = configini->getNetworks();
							item.state = MENU_STATE_NETWORKBROWSER;
							item.first_item = true;
									
						}
						if(topmenu[n] == "Enigma2"){
							enigma2 = new Enigma2(configini->getEnigma());
							item.first_item = true;
							if(configini->getEnigma() == ""){
								item.state = MENU_STATE_ENIGMABROWSER;
							}else{
								enigma2->getServices();
								item.state = MENU_STATE_ENIGMABROWSER;
							}
						}
						if(topmenu[n] == "Settings"){
							configini->setLongSeek(configini->getLongSeek(false));
							configini->setShortSeek(configini->getShortSeek(false));
							item.state = MENU_STATE_SETTINGS;
						}
						if(topmenu[n] == "Playlist"){
							item.state = MENU_STATE_PLAYLISTBROWSER;
						}
						if(topmenu[n] == "Info"){
							item.state = MENU_STATE_INFO;
						}
						if(topmenu[n] == "Exit"){
							renderloopdone = true;
						}
					}
					ImGui::SameLine();
					ImGui::Text("%s",topmenu[n].c_str());
				}
				if (*first_item) {
					printf("Set Top\n");
					fflush(stdout);
					std::string itemid = "##" + std::to_string(0);
					ImGui::SetFocusID(ImGui::GetID(itemid.c_str()), ImGui::GetCurrentWindow());
					ImGuiContext& g = *ImGui::GetCurrentContext();
					g.NavDisableHighlight = false;
					g.NavDisableMouseHover = true;
					*first_item = false;
				}
		}
		Windows::ExitMainWindow();
	}
}
		