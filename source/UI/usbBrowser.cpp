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
            if(usbmounter->getBasePath() == ""){
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
							//item.usbpath = thislist[n].mount_point + std::string("/");
							usbmounter->setBasePath(thislist[n].mount_point + std::string("/"));
							usbmounter->DirList(usbmounter->getBasePath(),configini->getshowHidden(false),Utility::getMediaExtensions());
							triggerselect = true;
						}
						ImGui::SameLine();
						float currstartYpos = ImGui::GetCursorPosY();
						ImGui::SetCursorPosY(ImGui::GetCursorPosY()+10.0f);
						ImGui::Image((void*)(intptr_t)nxmpicons.UsbTexture.id, ImVec2(40,40));
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
					std::vector<FS::FileEntry> thislist = usbmounter->getCurrList();
					for (unsigned int n = 0; n < thislist.size(); n++){
						if(thislist[n].type == FS::FileEntryType::Directory){
							ImGui::Image((void*)(intptr_t)nxmpicons.FolderTexture.id, ImVec2(40,40));
						}else{
							ImGui::Image((void*)(intptr_t)nxmpicons.FileTexture.id, ImVec2(40,40));
						}
						ImGui::SameLine();
						std::string itemid = "##" + std::to_string(n);
						if(item.selectionstate == FILE_SELECTION_CHECKBOX){
							if(thislist[n].type == FS::FileEntryType::Directory){
								ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize) / 2});
								if (ImGui::Selectable(itemid.c_str(), selected == n)){
									triggerselect = true;
									usbmounter->DirList(thislist[n].path,configini->getshowHidden(false),Utility::getMediaExtensions());
								}
							}else{
								ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize - ImGui::GetStyle().FramePadding.y * 2) / 2});
								std::string checkitemid = "##check" + std::to_string(n);
								if(thislist[n].type != FS::FileEntryType::Directory){
									if(ImGui::Checkbox(checkitemid.c_str(), usbmounter->checked(n))){
										if(*usbmounter->checked(n)){
											playlist->appendFile(thislist[n].name,thislist[n].path);
										} else {
											playlist->removeFile(thislist[n].name,thislist[n].path);
										}
									}
								}
							}
							
						}else {
							ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize) / 2});
							
							if (ImGui::Selectable(itemid.c_str(), selected == n)){
								
								if(thislist[n].type == FS::FileEntryType::Directory){
									usbmounter->DirList(thislist[n].path,configini->getshowHidden(false),Utility::getMediaExtensions());
									triggerselect = true;
								}else if(thislist[n].type == FS::FileEntryType::File){
									item.laststate = item.state;
									libmpv->loadFile(thislist[n].path);
									if(configini->getDbActive(true)){
										libmpv->getFileInfo()->resume = sqlitedb->getResume(thislist[n].path);
										if(libmpv->getFileInfo()->resume>0){
											item.popupstate = POPUP_STATE_RESUME;
										}
									}
								}
							}
						}
						ImGui::SameLine();
						ImVec4* colors = ImGui::GetStyle().Colors;
						ImVec4 textcolor = colors[ImGuiCol_Text];
						
						if(sqlitedb != nullptr){
							int dbfilestatus = sqlitedb->getFileDbStatus(thislist[n].path);
							if(dbfilestatus == 2){
								textcolor = ImVec4(0.0f,1.0f,0.0f,1.0f);
							}
							if(dbfilestatus == 1){
								textcolor = ImVec4(0.0f,1.0f,1.0f,1.0f);
							}
							
						}
						ImGui::TextColored(textcolor,"%s",thislist[n].name.c_str());
						
						if(thislist[n].type == FS::FileEntryType::File){
							ImGui::SameLine(total_w-150);
							ImGui::TextColored(textcolor,"%s",Utility::humanSize(thislist[n].size).c_str());
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
				//ImGuiContext& g = *GImGui;
				//ImGui::NavMoveRequestTryWrapping(g.CurrentWindow, ImGuiNavMoveFlags_LoopY);
				ImGui::EndListBox();	
			}
#endif
        }
        Windows::ExitWindow();
    }
}