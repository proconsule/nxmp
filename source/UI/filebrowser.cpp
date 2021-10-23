#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "windows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"


namespace Windows {
    void FileBrowserWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		static unsigned int item_current_idx = 0;
        std::vector<std::string> topmenu = {"Local Files","Network","Enigma2"};
		
        if (ImGui::Begin(nxmpTitle.c_str(), nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
            if (!*focus) {
                ImGui::SetNextWindowFocus();
                *focus = true;
            }
			if (ImGui::BeginMenuBar()) {
				ImGui::TextUnformatted("current path: %s",item.localpath.c_str());
				ImGui::EndMenuBar();
			}
			if (ImGui::BeginListBox("File Browser Menu",ImVec2(1280.0f, 720.0f))){
				int total_w = ImGui::GetContentRegionAvail().x;
						//ImGui::Text("current path: %s",item.localpath.c_str());
				for (unsigned int n = 0; n < item.localfileentries.size(); n++){
					const bool is_selected = (item_current_idx == n);
					if(item.localfileentries[n].type == FsDirEntryType_Dir || FS::GetFileType(item.localfileentries[n].name) != FileTypeNone){
						if(item.localfileentries[n].type == FsDirEntryType_Dir){
							ImGui::Image((void*)(intptr_t)FolderTexture.id, ImVec2(40,40));
						}else{
							ImGui::Image((void*)(intptr_t)FileTexture.id, ImVec2(40,40));
						}
						ImGui::SameLine();
						ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize) / 2});
						if (ImGui::Selectable(item.localfileentries[n].name, is_selected)){
							if(item.localfileentries[n].type == FsDirEntryType_Dir){
								item_current_idx = 0;
								if(item.localpath!="/"){
									item.localpath.append("/");
								}
								item.localpath.append(item.localfileentries[n].name);
								FS::GetDirList(item.localpath.c_str(),item.localfileentries);
							}
							else{
								std::string playfilepath = item.localpath;
								playfilepath.append("/");
								playfilepath.append(item.localfileentries[n].name);
								const char *cmd[] = {"loadfile", playfilepath.c_str(), NULL};
								mpv_command_async(mpv->getHandle(), 0, cmd);
							}
						}
						if(item.localfileentries[n].type != FsDirEntryType_Dir){
							ImGui::SameLine(total_w-150);
							ImGui::Text("%s",Utility::humanSize(item.localfileentries[n].file_size).c_str());
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