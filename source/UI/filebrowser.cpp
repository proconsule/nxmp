#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"


namespace Windows {
	
	float playertextscrollpos = 0.0f;
	int waitpos = 0;
	
	
	bool playertextforwardscroll = true;
	bool playertextlaststate = true;
	
	void BrowserScrollText(float w,float h,const char* fmt, ...){
		
		ImGui::BeginChild("###scrollable", ImVec2(w, h), false);
		va_list args;
		va_start(args, fmt);
		ImGui::TextV(fmt, args);
		va_end(args);
		
		float scroll_max_x = ImGui::GetScrollMaxX();
		if(playertextscrollpos>=scroll_max_x){
			playertextforwardscroll=false;
			if(!playertextlaststate==playertextforwardscroll){
				waitpos = 0;
			}
		}else if(playertextscrollpos<=0) {
			playertextforwardscroll=true;	
			if(!playertextlaststate==playertextforwardscroll){
				waitpos = 0;
			}
		}
		if(waitpos <= 60){
			waitpos++;
		}else{
			if(playertextforwardscroll){
				playertextscrollpos+=0.5;
			}else{
				playertextscrollpos-=0.5;
			}
		}
		
		playertextlaststate = playertextforwardscroll;
		ImGui::SetScrollX(playertextscrollpos);
		ImGui::EndChild();
	}
	
	
	void UniBrowserWindow(bool *focus, bool *first_item) {
		 Windows::SetupWindow();
		 if (ImGui::Begin(filebrowser->getTitle().c_str(), nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
            //if(item.popupstate == POPUP_STATE_NONE){
			//	ImGui::SetNextWindowFocus();
			//}
            
			if (ImGui::BeginMenuBar()) {
				ImGui::Text("current path: %s",filebrowser->getCurrentPath().c_str());
				ImGui::EndMenuBar();
			}
			float total_w = ImGui::GetContentRegionAvail().x;
			float total_h = ImGui::GetContentRegionAvail().y;
			std::vector<FS::FileEntry> thislist = filebrowser->getCurrList();
			bool triggerselect = false;
			
			
			if (ImGui::BeginTable("table1", 3)){
				ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthFixed, 850.0f*multiplyRes); // Default to 100.0f
				ImGui::TableSetupColumn("size", ImGuiTableColumnFlags_WidthFixed, 130.0f*multiplyRes); // Default to 200.0f
				ImGui::TableSetupColumn("date", ImGuiTableColumnFlags_WidthFixed,200.f*multiplyRes);       // Default to auto
           
				for (unsigned int n = 0; n < thislist.size(); n++){
					static int selected = -1;
					
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					if(thislist[n].type == FS::FileEntryType::Directory){
						GUI::NXMPImage((void*)(intptr_t)nxmpicons.FolderTexture.id, ImVec2(30,30));
					}else{
						GUI::NXMPImage((void*)(intptr_t)nxmpicons.FileTexture.id, ImVec2(30,30));
					}
					ImGui::SameLine();
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() +5.0f);
					std::string itemid = "##" + std::to_string(n);
					ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns;
					if (ImGui::Selectable(itemid.c_str(), selected == n,selectable_flags)){
							if(filebrowser->getCurrList()[n].type == FS::FileEntryType::Directory){
							triggerselect = true;
							filebrowser->DirList(thislist[n].path,configini->getshowHidden(false),Utility::getMediaExtensions());
						} else{
							item.laststate = item.state;
							playlist->clearPlaylist();
							filebrowser->clearChecked();
							libmpv->loadFile(filebrowser->getOpenUrlPart()+thislist[n].path);
							if(configini->getDbActive(true)){
								libmpv->getFileInfo()->resume = sqlitedb->getResume(filebrowser->getOpenUrlPart()+thislist[n].path);
								if(libmpv->getFileInfo()->resume>0){
									item.popupstate = POPUP_STATE_RESUME;
								}
							}
						}
					}
					if (ImGui::IsItemHovered()){
						if(item.fileHoveredidx != n){
							playertextscrollpos=0.0f;
							waitpos = 0;
						}
						item.fileHoveredidx = n;
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
					if(item.fileHoveredidx == n){
						BrowserScrollText(850.0f*multiplyRes,30.0f*multiplyRes,"%s",thislist[n].name.c_str());
					}else{
						ImGui::TextColored(textcolor,"%s",thislist[n].name.c_str());
					}
					ImGui::TableSetColumnIndex(1);
					if(thislist[n].type == FS::FileEntryType::File){
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() +5.0f*multiplyRes);
						std::string strsize = Utility::humanSize(thislist[n].size);
						ImVec2 textSize = ImGui::CalcTextSize(strsize.c_str());
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() -textSize.x- ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x);
						ImGui::TextColored(textcolor,"%s",strsize.c_str());
					}
					
					ImGui::TableSetColumnIndex(2);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() +5.0f*multiplyRes);
					
					if(thislist[n].timestamp.is_valid){
						std::string strdate = Utility::formatTimeStamp(thislist[n].timestamp.modified);
						ImVec2 textSize = ImGui::CalcTextSize(strdate.c_str());
						ImGui::TextColored(textcolor,"%s",strdate.c_str());
					}
					
				}
				ImGui::EndTable();
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
        Windows::ExitWindow();
		 
	}
	
	
	
	void aUniBrowserWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		
        if (ImGui::Begin(filebrowser->getTitle().c_str(), nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
            if(item.popupstate == POPUP_STATE_NONE){
				ImGui::SetNextWindowFocus();
			}
            
			if (ImGui::BeginMenuBar()) {
				ImGui::Text("current path: %s",filebrowser->getCurrentPath().c_str());
				ImGui::EndMenuBar();
			}
			float total_w = ImGui::GetContentRegionAvail().x;
			float total_h = ImGui::GetContentRegionAvail().y;
			if (ImGui::BeginListBox(std::string(filebrowser->getTitle() + " Menu").c_str(),ImVec2(total_w, total_h))){
				std::vector<FS::FileEntry> thislist = filebrowser->getCurrList();
				bool triggerselect = false;
				for (unsigned int n = 0; n < thislist.size(); n++){
					
					static int selected = -1;
					
						std::string itemid = "##" + std::to_string(n);
						if(item.selectionstate == FILE_SELECTION_CHECKBOX){
							if(thislist[n].type == FS::FileEntryType::Directory){
								ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40*multiplyRes - ImGui::GetFont()->FontSize) / 2});
								if (ImGui::Selectable(itemid.c_str(), selected == n)){
									triggerselect = true;
									filebrowser->DirList(thislist[n].path,configini->getshowHidden(false),Utility::getMediaExtensions());
								}
							}else{
								ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40*multiplyRes - ImGui::GetFont()->FontSize - ImGui::GetStyle().FramePadding.y * 2) / 2});
								std::string checkitemid = "##check" + std::to_string(n);
								if(thislist[n].type != FS::FileEntryType::Directory){
									if(ImGui::Checkbox(checkitemid.c_str(), filebrowser->checked(n))){
										if(*filebrowser->checked(n)){
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
							ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + 10+(40*multiplyRes - ImGui::GetFont()->FontSize) / 2});
							if (ImGui::Selectable(itemid.c_str(), selected == n)){
								if(filebrowser->getCurrList()[n].type == FS::FileEntryType::Directory){
									triggerselect = true;
									filebrowser->DirList(thislist[n].path,configini->getshowHidden(false),Utility::getMediaExtensions());
								}
								else{
									item.laststate = item.state;
									playlist->clearPlaylist();
									filebrowser->clearChecked();
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
						ImGui::Columns(3,"testcol",false);
						ImGui::SetColumnWidth(0,880.0f*multiplyRes);
						ImGui::SetColumnWidth(1,200.0f*multiplyRes);
						ImGui::SetColumnWidth(2,200.0f*multiplyRes);
						float currpos = ImGui::GetCursorPosY();
						ImGui::SetCursorPosY(currpos -10.f*multiplyRes);
						if(thislist[n].type == FS::FileEntryType::Directory){
							GUI::NXMPImage((void*)(intptr_t)nxmpicons.FolderTexture.id, ImVec2(40,40));
						}else{
							GUI::NXMPImage((void*)(intptr_t)nxmpicons.FileTexture.id, ImVec2(40,40));
						}
						
						ImGui::SameLine();
						ImGui::SetCursorPosY(currpos);
						
						ImGui::TextColored(textcolor,"%s",thislist[n].name.c_str());
						if(thislist[n].type == FS::FileEntryType::File){
							
							ImGui::NextColumn();
							if(thislist[n].timestamp.is_valid){
								std::string strdate = Utility::formatTimeStamp(thislist[n].timestamp.modified);
								ImVec2 textSize = ImGui::CalcTextSize(strdate.c_str());
								//ImGui::SameLine(total_w-150-textSize.x);
								ImGui::TextColored(textcolor,"%s",strdate.c_str());
							}
							ImGui::NextColumn();
							std::string strsize = Utility::humanSize(thislist[n].size);
							ImVec2 textSize = ImGui::CalcTextSize(strsize.c_str());
								
							//ImGui::SameLine(total_w-150);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() -textSize.x- ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x);
							ImGui::TextColored(textcolor,"%s",strsize.c_str());
						}else{
							ImGui::Columns(1);
						}
						
						
						if(item.selectionstate == FILE_SELECTION_CHECKBOX && thislist[n].type == FS::FileEntryType::File){
							ImGui::Dummy(ImVec2(0.0,0.0));
							ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y-1.0f});
							
						}
					ImGui::Columns(1);	
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
	
	
    void FileBrowserWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		std::vector<std::string> topmenu = {"Local Files","Network","Enigma2"};
		
        if (ImGui::Begin("File Browser", nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
            if(item.popupstate == POPUP_STATE_NONE){
				ImGui::SetNextWindowFocus();
			}
            
			if (ImGui::BeginMenuBar()) {
				ImGui::Text("current path: %s",filebrowser->getCurrentPath().c_str());
				ImGui::EndMenuBar();
			}
			float total_w = ImGui::GetContentRegionAvail().x;
			float total_h = ImGui::GetContentRegionAvail().y;
			if (ImGui::BeginListBox("File Browser Menu",ImVec2(total_w, total_h))){
				
				std::vector<FS::FileEntry> thislist = filebrowser->getCurrList();
				bool triggerselect = false;
				for (unsigned int n = 0; n < thislist.size(); n++){
					
					static int selected = -1;
					
						if(thislist[n].type == FS::FileEntryType::Directory){
							GUI::NXMPImage((void*)(intptr_t)nxmpicons.FolderTexture.id, ImVec2(40,40));
						}else{
							GUI::NXMPImage((void*)(intptr_t)nxmpicons.FileTexture.id, ImVec2(40,40));
						}
						ImGui::SameLine();
						std::string itemid = "##" + std::to_string(n);
						if(item.selectionstate == FILE_SELECTION_CHECKBOX){
							if(thislist[n].type == FS::FileEntryType::Directory){
								ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40*multiplyRes - ImGui::GetFont()->FontSize) / 2});
								if (ImGui::Selectable(itemid.c_str(), selected == n)){
									triggerselect = true;
									filebrowser->DirList(thislist[n].path,configini->getshowHidden(false),Utility::getMediaExtensions());
								}
							}else{
								ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40*multiplyRes - ImGui::GetFont()->FontSize - ImGui::GetStyle().FramePadding.y * 2) / 2});
								std::string checkitemid = "##check" + std::to_string(n);
								if(thislist[n].type != FS::FileEntryType::Directory){
									if(ImGui::Checkbox(checkitemid.c_str(), filebrowser->checked(n))){
										if(*filebrowser->checked(n)){
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
							ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40*multiplyRes - ImGui::GetFont()->FontSize) / 2});
							if (ImGui::Selectable(itemid.c_str(), selected == n)){
								if(filebrowser->getCurrList()[n].type == FS::FileEntryType::Directory){
									triggerselect = true;
									filebrowser->DirList(thislist[n].path,configini->getshowHidden(false),Utility::getMediaExtensions());
								}
								else{
									item.laststate = item.state;
									playlist->clearPlaylist();
									filebrowser->clearChecked();
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