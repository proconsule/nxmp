#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "windows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"


namespace Windows {
    void EnigmaWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		static unsigned int item_current_idx = 0;
        std::vector<std::string> topmenu = {"Local Files","Network","Enigma2"};
		
        if (ImGui::Begin("Enigma2", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
            if (!*focus) {
                ImGui::SetNextWindowFocus();
                *focus = true;
            }
			if (ImGui::BeginListBox("Enigma2 Browser Menu",ImVec2(1280.0f, 720.0f))){
				if(item.enigma2bouquet == ""){
					for (unsigned int n = 0; n < enigma2->e2services.size(); n++){
						const bool is_selected = (item_current_idx == n);
						if (ImGui::Selectable(enigma2->e2services[n].name.c_str(), is_selected)){
			
							enigma2->m3uParser((char *)enigma2->e2services[n].bouquetref.c_str());
							item.enigma2bouquet =  enigma2->e2services[n].bouquetref;
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();
							
					}
				}else if(item.enigma2bouquet != ""){
					for (unsigned int n = 0; n < enigma2->e2currbouqet.size(); n++){
						const bool is_selected = (item_current_idx == n);
						std::string channame = std::to_string(n) + std::string(". ") +enigma2->e2currbouqet[n].name;
						if (ImGui::Selectable(channame.c_str(), is_selected)){
									
							const char *cmd[] = {"loadfile", enigma2->e2currbouqet[n].url.c_str(), NULL};
							mpv_command_async(mpv->getHandle(), 0, cmd);
						}
						if (is_selected)
						ImGui::SetItemDefaultFocus();
					}
				}
			}
			ImGui::EndListBox();
		}
		Windows::ExitWindow();
	}
}