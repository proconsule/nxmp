#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"


namespace Windows {
    void FileBrowserWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		std::vector<std::string> topmenu = {"Local Files","Network","Enigma2"};
		
        if (ImGui::Begin("File Browser", nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
            
            ImGui::SetNextWindowFocus();
            
			if (ImGui::BeginMenuBar()) {
				ImGui::Text("current path: %s",localdir->getCurrentPath().c_str());
				ImGui::EndMenuBar();
			}
			float total_w = ImGui::GetContentRegionAvail().x;
			float total_h = ImGui::GetContentRegionAvail().y;
			if (ImGui::BeginListBox("File Browser Menu",ImVec2(total_w, total_h))){
				
				std::vector<FS::FileEntry> thislist = localdir->getCurrList();
				bool triggerselect = false;
				for (unsigned int n = 0; n < thislist.size(); n++){
					
					static int selected = -1;
					
						if(thislist[n].type == FS::FileEntryType::Directory){
							ImGui::Image((void*)(intptr_t)FolderTexture.id, ImVec2(40,40));
						}else{
							ImGui::Image((void*)(intptr_t)FileTexture.id, ImVec2(40,40));
						}
						ImGui::SameLine();
						ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize) / 2});
						std::string itemid = "##" + std::to_string(n);
						if (ImGui::Selectable(itemid.c_str(), selected == n)){
							if(localdir->getCurrList()[n].type == FS::FileEntryType::Directory){
								triggerselect = true;
								localdir->DirList(thislist[n].path,true,Utility::getMediaExtensions());
							}
							else{
								libmpv->loadFile(thislist[n].path);
								if(configini->getDbActive(true)){
									libmpv->getFileInfo()->resume = sqlitedb->getResume(thislist[n].path);
									if(libmpv->getFileInfo()->resume>0){
										item.popupstate = POPUP_STATE_RESUME;
									}
								}
							}
						}
						ImGui::SameLine();
						ImGui::Text("%s",thislist[n].name.c_str());
						if(thislist[n].type == FS::FileEntryType::File){
							ImGui::SameLine(total_w-150);
							ImGui::Text("%s",Utility::humanSize(thislist[n].size).c_str());
						}
					
					if (selected)
						ImGui::SetItemDefaultFocus();
				}
				if (*first_item && thislist.size() >0) {
					std::string itemid = "##" + std::to_string(0);
					ImGui::SetFocusID(ImGui::GetID(itemid.c_str()), ImGui::GetCurrentWindow());
					ImGui::SetScrollY(0.0f);
					*first_item = false;
				}
				if(triggerselect == true){
					*first_item = true;
				}
			}	
			ImGui::EndListBox();		
        }
        Windows::ExitWindow();
    }
}