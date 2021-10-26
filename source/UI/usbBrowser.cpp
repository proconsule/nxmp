#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "windows.h"
#include "utils.h"
#include "localfiles.h"


namespace Windows {
    void USBBrowserWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		
        if (ImGui::Begin("USB Browser", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
            
            ImGui::SetNextWindowFocus();
            if(item.usbpath == ""){
				
				if (ImGui::BeginListBox("USB Browser Menu",ImVec2(1280.0f, 720.0f))){
					int total_w = ImGui::GetContentRegionAvail().x;
					static int selected = -1;	
					for (unsigned int n = 0; n < usbmounter->mounted_devs.size(); n++){
						std::string devname = usbmounter->mounted_devs[n].name  + std::string(" -> ") + usbmounter->mounted_devs[n].mount_point;
						if (ImGui::Selectable(devname.c_str(), selected == n)){
							item.usbpath = usbmounter->mounted_devs[n].mount_point + std::string("/");
							item.usbbasepath = item.usbpath;
							item.usbfileentries = FS::getDirList(item.usbpath.c_str(),true,Utility::getMediaExtensions());
							item.first_item = true;
						}
					
						if (selected)
							ImGui::SetItemDefaultFocus();
					}
					if (*first_item) {
						std::string firstname = usbmounter->mounted_devs[0].mount_point + std::string("/");
						ImGui::SetFocusID(ImGui::GetID(firstname.c_str()), ImGui::GetCurrentWindow());
						*first_item = false;
					}
				}	
				ImGui::EndListBox();	
			}else{
				if (ImGui::BeginListBox("USB Browser Menu",ImVec2(1280.0f, 720.0f))){
					int total_w = ImGui::GetContentRegionAvail().x;
					static int selected = -1;	
					for (unsigned int n = 0; n < item.usbfileentries.size(); n++){
						if(item.usbfileentries[n].type == FS::FileEntryType::Directory){
							ImGui::Image((void*)(intptr_t)FolderTexture.id, ImVec2(40,40));
						}else{
							ImGui::Image((void*)(intptr_t)FileTexture.id, ImVec2(40,40));
						}
						ImGui::SameLine();
						ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize) / 2});
						
						if (ImGui::Selectable(item.usbfileentries[n].name.c_str(), selected == n)){
							
							if(item.usbfileentries[n].type == FS::FileEntryType::Directory){
								item.usbpath = item.usbfileentries[n].path;
								item.usbfileentries = FS::getDirList(item.usbpath.c_str(),true,Utility::getMediaExtensions());
								item.first_item = true;
							}else if(item.usbfileentries[n].type == FS::FileEntryType::File){
								
								const char *cmd[] = {"loadfile", item.usbfileentries[n].path.c_str(), NULL};
								mpv_command_async(libmpv->getHandle(), 0, cmd);
							}
						}
						
						if(item.usbfileentries[n].type == FS::FileEntryType::File){
							ImGui::SameLine(total_w-150);
							ImGui::Text("%s",Utility::humanSize(item.usbfileentries[n].size).c_str());
						}
					
					
						if (selected)
							ImGui::SetItemDefaultFocus();
					}
					if (*first_item) {
						ImGui::SetFocusID(ImGui::GetID(item.usbfileentries[0].name.c_str()), ImGui::GetCurrentWindow());
						*first_item = false;
					}
				}	
				ImGui::EndListBox();	
			}
        }
        Windows::ExitWindow();
    }
}