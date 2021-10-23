#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "windows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"


namespace Windows {
    void MainMenuWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		static unsigned int item_current_idx = 0;
        std::vector<std::string> topmenu = {"Local Files","Network","Enigma2"};
		
        if (ImGui::Begin(nxmpTitle.c_str(), nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
            if (!*focus) {
                ImGui::SetNextWindowFocus();
                *focus = true;
            }
			if (ImGui::BeginListBox("Browser Menu",ImVec2(1280.0f, 720.0f))){
						
				for (unsigned int n = 0; n < topmenu.size(); n++){
					const bool is_selected = (item_current_idx == n);
					if(topmenu[n] == "Local Files"){
						ImGui::Image((void*)(intptr_t)SdCardTexture.id, ImVec2(40,40));
					}
					else if(topmenu[n] == "Network"){
						ImGui::Image((void*)(intptr_t)NetworkTexture.id, ImVec2(40,40));
					}
					else if(topmenu[n] == "Enigma2"){
						ImGui::Image((void*)(intptr_t)Enigma2Texture.id, ImVec2(40,40));
					}
							
							
					ImGui::SameLine();
					ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize) / 2});
								
					if (ImGui::Selectable(topmenu[n].c_str(), is_selected)){
						item_current_idx = 0;
						if(n==0){
							item.state = MENU_STATE_FILEBROWSER;
							FS::GetDirList(item.localpath.c_str(),item.localfileentries);
						}
						if(n==1){
							item.networksources.clear();
							item.networksources = configini->getNetworks();
							item.state = MENU_STATE_NETWORKBROWSER;
							item.networkurl = "";
							item.networklastpath = "";
									
						}
						if(n==2){
							std::string e2uri = configini->getEnigma();
							urlschema e2schema = Utility::parseUrl(e2uri);
							enigma2->enigma2ip = e2schema.server;
							enigma2->getServices();
							item.state = MENU_STATE_ENIGMABROWSER;
						}
					}
					if (is_selected)
					ImGui::SetItemDefaultFocus();
				}
				
			}				
			ImGui::EndListBox();
		}	
		Windows::ExitWindow();
	}
}
		