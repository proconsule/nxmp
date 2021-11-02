#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"


namespace Windows {
    void USBBrowserWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		
        if (ImGui::Begin("USB Browser", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
#ifdef __SWITCH__
            ImGui::SetNextWindowFocus();
            if(item.usbpath == ""){
				
				if (ImGui::BeginListBox("USB Browser Menu",ImVec2(1280.0f, 720.0f))){
					int total_w = ImGui::GetContentRegionAvail().x;
					static int selected = -1;	
					std::vector<usb_devices> thislist = usbmounter->mounted_devs;
					for (unsigned int n = 0; n < thislist.size(); n++){
						std::string itemid = "##" + std::to_string(n);
						ImGui::SameLine();
						//float currstartpos = ImGui::GetCursorPosX();
						if (ImGui::Selectable(itemid.c_str(), selected == n,0, ImVec2(0, 60))){
							item.usbpath = thislist[n].mount_point + std::string("/");
							item.usbbasepath = item.usbpath;
							item.usbfileentries = FS::getDirList(item.usbpath.c_str(),true,Utility::getMediaExtensions());
							item.first_item = true;
						}
						ImGui::SameLine();
						float currstartYpos = ImGui::GetCursorPosY();
						ImGui::SetCursorPosY(ImGui::GetCursorPosY()+10.0f);
						ImGui::Image((void*)(intptr_t)UsbTexture.id, ImVec2(40,40));
						ImGui::SameLine();
						ImGui::SetCursorPosY(currstartYpos);
						float currstartXpos = ImGui::GetCursorPosX();
						//ImGui::SetCursorPosX(currstartpos);
						std::string devname = Utility::trim(usbmounter->mounted_devs[n].name)  + std::string(" -> ") + usbmounter->mounted_devs[n].mount_point;
						ImGui::Text("%s",devname.c_str());
						ImGui::SetCursorPosY(ImGui::GetCursorPosY()-30.0f);
						ImGui::SetCursorPosX(currstartXpos);
						ImGui::Text("%s %s",usbmounter->mounted_devs[n].fstype.c_str(),Utility::humanSize(usbmounter->mounted_devs[n].capacity).c_str());
						
					
						if (selected)
							ImGui::SetItemDefaultFocus();
					}
					if (*first_item) {
						std::string itemid = "##" + std::to_string(0);
						ImGui::SetFocusID(ImGui::GetID(itemid.c_str()), ImGui::GetCurrentWindow());
						*first_item = false;
					}
				}	
				ImGui::EndListBox();	
			}else{
				if (ImGui::BeginListBox("USB Browser Menu",ImVec2(1280.0f, 720.0f))){
					int total_w = ImGui::GetContentRegionAvail().x;
					static int selected = -1;	
					std::vector<FS::FileEntry> thislist = item.usbfileentries;
					for (unsigned int n = 0; n < thislist.size(); n++){
						if(thislist[n].type == FS::FileEntryType::Directory){
							ImGui::Image((void*)(intptr_t)FolderTexture.id, ImVec2(40,40));
						}else{
							ImGui::Image((void*)(intptr_t)FileTexture.id, ImVec2(40,40));
						}
						ImGui::SameLine();
						ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize) / 2});
						
						if (ImGui::Selectable(thislist[n].name.c_str(), selected == n)){
							
							if(thislist[n].type == FS::FileEntryType::Directory){
								item.usbpath = item.usbfileentries[n].path;
								item.usbfileentries = FS::getDirList(item.usbpath.c_str(),true,Utility::getMediaExtensions());
								item.first_item = true;
							}else if(thislist[n].type == FS::FileEntryType::File){
								
								const char *cmd[] = {"loadfile", thislist[n].path.c_str(), NULL};
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
					if (*first_item && thislist.size() >0) {
						ImGui::SetFocusID(ImGui::GetID(thislist[0].name.c_str()), ImGui::GetCurrentWindow());
						*first_item = false;
					}
				}	
				ImGui::EndListBox();	
			}
#endif
        }
        Windows::ExitWindow();
    }
}