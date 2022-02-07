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
						ImGui::Image((void*)(intptr_t)nxmpicons.SdCardTexture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "USB"){
						ImGui::Image((void*)(intptr_t)nxmpicons.UsbTexture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "Network"){
						ImGui::Image((void*)(intptr_t)nxmpicons.NetworkTexture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "UPNP"){
						ImGui::Image((void*)(intptr_t)nxmpicons.UPNPTexture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "Enigma2"){
						ImGui::Image((void*)(intptr_t)nxmpicons.Enigma2Texture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "Playlist"){
						ImGui::Image((void*)(intptr_t)nxmpicons.PlaylistTexture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "Info"){
						ImGui::Image((void*)(intptr_t)nxmpicons.InfoTexture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "Settings"){
						ImGui::Image((void*)(intptr_t)nxmpicons.SettingsTexture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "Exit"){
						ImGui::Image((void*)(intptr_t)nxmpicons.ExitTexture.id, ImVec2(50,50));
					}
							
							
					ImGui::SameLine();
					ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (50 - ImGui::GetFont()->FontSize) / 2});
								
					if (ImGui::Selectable(itemid.c_str(), selected == n)){
						if(topmenu[n] == "Local Files"){
							item.state = MENU_STATE_FILEBROWSER;
							localdir = new localFs(configini->getStartPath(),playlist);
							localdir->DirList(configini->getStartPath(),true,Utility::getMediaExtensions());
							item.first_item = true;
						}
						if(topmenu[n] == "USB"){
#ifdef NXMP_USBSUPPORT
							usbInit();
							item.state = MENU_STATE_USB;
							if(usbmounter == nullptr){
								usbmounter = new USBMounter(playlist);
							}
							
#endif
						}
						if(topmenu[n] == "Network"){
#ifdef NXMP_NETWORKSUPPORT
							item.networksources.clear();
							item.networksources = configini->getNetworks();
							item.state = MENU_STATE_NETWORKBROWSER;
							item.first_item = true;
#endif
									
						}
						if(topmenu[n] == "UPNP"){
#ifdef NXMP_UPNPSUPPORT
							printf("upnp selected\n");
							fflush(stdout);
							nxupnp = new NXUPnP();
							nxupnp->Discovery();
							item.state = MENU_STATE_UPNPBROWSER;
							item.first_item = true;
#endif
						}
						if(topmenu[n] == "Enigma2"){
#ifdef NXMP_ENIGMASUPPORT
							enigma2 = new Enigma2(configini->getEnigma());
							item.first_item = true;
							if(configini->getEnigma() == ""){
								item.state = MENU_STATE_ENIGMABROWSER;
							}else{
								enigma2->getServices();
								item.state = MENU_STATE_ENIGMABROWSER;
							}
#endif
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
		