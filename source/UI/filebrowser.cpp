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
		static unsigned int lastfocus = 0;
		
        if (ImGui::Begin("File Browser", nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
            if(item.popupstate == POPUP_STATE_NONE){
				ImGui::SetNextWindowFocus();
			}
            
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
									localdir->DirList(thislist[n].path,configini->getshowHidden(false),Utility::getMediaExtensions());
								}
							}else{
								ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize - ImGui::GetStyle().FramePadding.y * 2) / 2});
								std::string checkitemid = "##check" + std::to_string(n);
								if(thislist[n].type != FS::FileEntryType::Directory){
									if(ImGui::Checkbox(checkitemid.c_str(), localdir->checked(n))){
										if(*localdir->checked(n)){
											playlist->appendFile(thislist[n].name,thislist[n].path);
										} else {
											playlist->removeFile(thislist[n].name,thislist[n].path);
										}
									}
								}
							}
							if (ImGui::IsItemHovered()){
								item.fileHoveredidx = n;
							}
						}
						if(item.selectionstate == FILE_SELECTION_NONE){
							ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize) / 2});
							if (ImGui::Selectable(itemid.c_str(), selected == n)){
								if(localdir->getCurrList()[n].type == FS::FileEntryType::Directory){
									triggerselect = true;
									localdir->DirList(thislist[n].path,configini->getshowHidden(false),Utility::getMediaExtensions());
								}
								else{
									item.laststate = item.state;
									playlist->clearPlaylist();
									localdir->clearChecked();
									libmpv->loadFile(thislist[n].path);
									if(configini->getDbActive(true)){
										libmpv->getFileInfo()->resume = sqlitedb->getResume(thislist[n].path);
										if(libmpv->getFileInfo()->resume>0){
											item.popupstate = POPUP_STATE_RESUME;
										}
									}
								}
							}
							if (ImGui::IsItemHovered()){
								item.fileHoveredidx = n;
							}
						}
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
						ImGui::SameLine();
						ImGui::TextColored(textcolor,"%s",thislist[n].name.c_str());
						if(thislist[n].type == FS::FileEntryType::File){
							ImGui::SameLine(total_w-150);
							ImGui::TextColored(textcolor,"%s",Utility::humanSize(thislist[n].size).c_str());
						}
						
						
						if(item.selectionstate == FILE_SELECTION_CHECKBOX && thislist[n].type == FS::FileEntryType::File){
							ImGui::Dummy(ImVec2(0.0,0.0));
							ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y-1.0f});
							
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
				if(*focus){
					if(item.selectionstate == FILE_SELECTION_NONE){
						std::string itemid = "##" + std::to_string(item.fileHoveredidx);
						ImGui::SetFocusID(ImGui::GetID((itemid.c_str())), ImGui::GetCurrentWindow());
					}
					if(item.selectionstate == FILE_SELECTION_CHECKBOX){
						std::string checkitemid = "##check" + std::to_string(item.fileHoveredidx);
						ImGui::SetFocusID(ImGui::GetID((checkitemid.c_str())), ImGui::GetCurrentWindow());
					}
					*focus = false;
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