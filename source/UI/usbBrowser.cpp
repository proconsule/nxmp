#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"


namespace Windows {
    void USBBrowserWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		
        if (ImGui::Begin("USB Browser", nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
#ifdef __SWITCH__
            ImGui::SetNextWindowFocus();
            if(item.usbpath == ""){
				float total_w = ImGui::GetContentRegionAvail().x;
				float total_h = ImGui::GetContentRegionAvail().y;
				if (ImGui::BeginListBox("USB Browser Menu",ImVec2(total_w, total_h))){
					bool triggerselect = false;
					static int selected = -1;	
					std::vector<usb_devices> thislist = usbmounter->mounted_devs;
					for (unsigned int n = 0; n < thislist.size(); n++){
						std::string itemid = "##" + std::to_string(n);
						ImGui::SameLine();
						if (ImGui::Selectable(itemid.c_str(), selected == n,0, ImVec2(0, 60))){
							item.usbpath = thislist[n].mount_point + std::string("/");
							item.usbbasepath = item.usbpath;
							item.usbfileentries = FS::getDirList(item.usbpath.c_str(),true,Utility::getMediaExtensions());
							triggerselect = true;
						}
						ImGui::SameLine();
						float currstartYpos = ImGui::GetCursorPosY();
						ImGui::SetCursorPosY(ImGui::GetCursorPosY()+10.0f);
						ImGui::Image((void*)(intptr_t)UsbTexture.id, ImVec2(40,40));
						ImGui::SameLine();
						ImGui::SetCursorPosY(currstartYpos);
						float currstartXpos = ImGui::GetCursorPosX();
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
					if(triggerselect == true){
						*first_item = true;
					}
				}	
				ImGui::EndListBox();	
			}else{
				float total_w = ImGui::GetContentRegionAvail().x;
				float total_h = ImGui::GetContentRegionAvail().y;
			
				if (ImGui::BeginListBox("USB Browser Menu",ImVec2(total_w, total_h))){
					bool triggerselect = false;
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
						std::string itemid = "##" + std::to_string(n);
						if (ImGui::Selectable(itemid.c_str(), selected == n)){
							
							if(thislist[n].type == FS::FileEntryType::Directory){
								item.usbpath = item.usbfileentries[n].path;
								item.usbfileentries = FS::getDirList(item.usbpath.c_str(),true,Utility::getMediaExtensions());
								triggerselect = true;
							}else if(thislist[n].type == FS::FileEntryType::File){
								
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
#endif
        }
        Windows::ExitWindow();
    }
}