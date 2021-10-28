#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"


namespace Windows {
    void MainMenuWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		//std::vector<std::string> topmenu = {"Local Files","USB","Network","Enigma2","Settings","Info","Exit"};
		std::vector<std::string> topmenu = configini->topmenu;
		
        if (ImGui::Begin(nxmpTitle.c_str(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
			ImGui::SetNextWindowFocus();
            
			if (ImGui::BeginListBox("Browser Menu",ImVec2(1280.0f, 720.0f))){
				static int selected = -1;					
				for (unsigned int n = 0; n < topmenu.size(); n++){
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
								
					if (ImGui::Selectable(topmenu[n].c_str(), selected == n)){
						if(topmenu[n] == "Local Files"){
							item.state = MENU_STATE_FILEBROWSER;
							item.localfileentries = FS::getDirList(item.localpath,true,Utility::getMediaExtensions());
							item.first_item = true;
						}
						if(topmenu[n] == "USB"){
#ifdef __SWITCH__
							usbInit();
							item.state = MENU_STATE_USB;
							usbmounter = new USBMounter();
							item.first_item = true;
#endif
						}
						if(topmenu[n] == "Network"){
							item.networksources.clear();
							item.networksources = configini->getNetworks();
							item.state = MENU_STATE_NETWORKBROWSER;
							item.networkurl = "";
							item.networklastpath = "";
							item.first_item = true;
									
						}
						if(topmenu[n] == "Enigma2"){
							enigma2 = new Enigma2();
							item.first_item = true;
							std::string e2uri = configini->getEnigma();
							if(e2uri == ""){
								item.state = MENU_STATE_ENIGMABROWSER;
							}else{
								urlschema e2schema = Utility::parseUrl(e2uri);
								enigma2->enigma2ip = e2schema.server;
								enigma2->getServices();
								item.state = MENU_STATE_ENIGMABROWSER;
							}
						}
						if(topmenu[n] == "Settings"){
							item.state = MENU_STATE_SETTINGS;
						}
						if(topmenu[n] == "Info"){
							item.state = MENU_STATE_INFO;
						}
						if(topmenu[n] == "Exit"){
							renderloopdone = true;
						}
					}
					if (selected)
					ImGui::SetItemDefaultFocus();
				}
				if (*first_item) {
					ImGui::SetFocusID(ImGui::GetID((topmenu[0].c_str())), ImGui::GetCurrentWindow());
					*first_item = false;
                }
			}				
			ImGui::EndListBox();
		}	
		Windows::ExitWindow();
	}
}
		