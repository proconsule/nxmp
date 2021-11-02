#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"


namespace Windows {
    void FileBrowserWindow(bool *focus, bool first_item) {
        Windows::SetupWindow();
		std::vector<std::string> topmenu = {"Local Files","Network","Enigma2"};
		
        if (ImGui::Begin("File Browser", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
            
            ImGui::SetNextWindowFocus();
            
			if (ImGui::BeginMenuBar()) {
				ImGui::Text("current path: %s",localdir->getCurrentPath().c_str());
				ImGui::EndMenuBar();
			}
			if (ImGui::BeginListBox("File Browser Menu",ImVec2(1280.0f, 720.0f))){
				int total_w = ImGui::GetContentRegionAvail().x;
				std::vector<FS::FileEntry> thislist = localdir->getCurrList();
				for (unsigned int n = 0; n < thislist.size(); n++){
					static int selected = -1;
					
						if(thislist[n].type == FS::FileEntryType::Directory){
							ImGui::Image((void*)(intptr_t)FolderTexture.id, ImVec2(40,40));
						}else{
							ImGui::Image((void*)(intptr_t)FileTexture.id, ImVec2(40,40));
						}
						ImGui::SameLine();
						ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize) / 2});
						if (ImGui::Selectable(thislist[n].name.c_str(), selected == n)){
							if(localdir->getCurrList()[n].type == FS::FileEntryType::Directory){
								item.first_item = true;
								
								localdir->DirList(thislist[n].path,true,Utility::getMediaExtensions());
							}
							else{
								const char *cmd[] = {"loadfile",  thislist[n].path.c_str(), NULL};
								mpv_command_async(libmpv->getHandle(), 0, cmd);
							}
						}
						if(thislist[n].type == FS::FileEntryType::File){
							ImGui::SameLine(total_w-150);
							ImGui::Text("%s",Utility::humanSize(thislist[n].size).c_str());
						}
					
					if (selected)
						ImGui::SetItemDefaultFocus();
				}
				if (first_item && thislist.size() >0) {
					ImGui::SetFocusID(ImGui::GetID(thislist[0].name.c_str()), ImGui::GetCurrentWindow());
					ImGui::ScrollToItem();
					first_item = false;
					item.first_item = false;
				}
			}	
			ImGui::EndListBox();		
        }
        Windows::ExitWindow();
    }
}