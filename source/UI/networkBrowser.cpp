#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"


namespace Windows {
#ifdef NXMP_NETWORKSUPPORT
	void FtpWindow(bool *focus, bool *first_item){
		Windows::SetupWindow();
		if (ImGui::Begin("Ftp Browser", nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_MenuBar)) {
           
            if(item.popupstate == POPUP_STATE_NONE){
				ImGui::SetNextWindowFocus();
			}
           
			if (ImGui::BeginMenuBar()) {
				ImGui::Text("current path: %s",ftpdir->getCurrPath().c_str());
				ImGui::EndMenuBar();
			}
			float total_w = ImGui::GetContentRegionAvail().x;
			float total_h = ImGui::GetContentRegionAvail().y;
			if (ImGui::BeginListBox("Ftp Browser Menu",ImVec2(total_w, total_h))){
				//ImGui::Text("Current Dir: %s\n",item.networklastpath.c_str());
				int total_w = ImGui::GetContentRegionAvail().x;
				std::vector<FS::FileEntry> thislist = ftpdir->getCurrList();
				bool triggerselect = false;
				for(unsigned int n=0;n<thislist.size();n++){
						
						if(thislist[n].type == FS::FileEntryType::Directory){
							ImGui::Image((void*)(intptr_t)nxmpicons.FolderTexture.id, ImVec2(40,40));
						}else{
							ImGui::Image((void*)(intptr_t)nxmpicons.FileTexture.id, ImVec2(40,40));
						}
						ImGui::SameLine();
						
						urlschema thisurl = Utility::parseUrl(ftpdir->getUrl());
						static int selected = -1;
						std::string itemid = "##" + std::to_string(n);
						if(item.selectionstate == FILE_SELECTION_CHECKBOX){
							if(thislist[n].type == FS::FileEntryType::Directory){
								ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize) / 2});
								if (ImGui::Selectable(itemid.c_str(), selected == n)){
									triggerselect = true;
									ftpdir->DirList(thislist[n].path + thislist[n].name,Utility::getMediaExtensions());
								}
							}else{
								ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize - ImGui::GetStyle().FramePadding.y * 2) / 2});
								std::string checkitemid = "##check" + std::to_string(n);
								if(thislist[n].type != FS::FileEntryType::Directory){
									std::string openurl = thisurl.scheme + std::string("://") + thisurl.user + std::string(":") + thisurl.pass + std::string("@") + thisurl.server + std::string("/") + thislist[n].path + thislist[n].name;
									if(ImGui::Checkbox(checkitemid.c_str(), ftpdir->checked(n))){
										if(*ftpdir->checked(n)){
											playlist->appendFile(thislist[n].name,openurl);
										} else {
											playlist->removeFile(thislist[n].name,openurl);
										}
									}
								}
							}
							if (ImGui::IsItemHovered()){
								item.fileHoveredidx = n;
							}
							
						}else if(item.selectionstate == FILE_SELECTION_NONE){
							ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize) / 2});
							if (ImGui::Selectable(itemid.c_str(), selected == n)){
										
								if(thislist[n].type == FS::FileEntryType::Directory){
									triggerselect = true;
									ftpdir->DirList(thislist[n].path + thislist[n].name,Utility::getMediaExtensions());
									
								}else if (thislist[n].type == FS::FileEntryType::File){
											
									std::string openurl = thisurl.scheme + std::string("://") + thisurl.user + std::string(":") + thisurl.pass + std::string("@") + thisurl.server + std::string("/") + thislist[n].path + thislist[n].name;
									item.laststate = item.state;
									playlist->clearPlaylist();
									ftpdir->clearChecked();
									libmpv->loadFile(openurl);
									if(configini->getDbActive(true)){
										libmpv->getFileInfo()->resume = sqlitedb->getResume(openurl);
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
						ImGui::SameLine();
						ImVec4* colors = ImGui::GetStyle().Colors;
						ImVec4 textcolor = colors[ImGuiCol_Text];
						
						if(sqlitedb != nullptr){
							std::string openurl = thisurl.scheme + std::string("://") + thisurl.user + std::string(":") + thisurl.pass + std::string("@") + thisurl.server + std::string("/") + thislist[n].path + thislist[n].name;
							int dbfilestatus = sqlitedb->getFileDbStatus(openurl);
							if(dbfilestatus == 2){
								textcolor = ImVec4(0.0f,1.0f,0.0f,1.0f);
							}
							if(dbfilestatus == 1){
								textcolor = ImVec4(0.0f,1.0f,1.0f,1.0f);
							}
							
						}
						std::string nametext = Utility::truncateLen(thislist[n].name,87);
						ImGui::TextColored(textcolor,"%s",nametext.c_str());
						if(thislist[n].type == FS::FileEntryType::File){
							ImGui::SameLine(total_w-150);
							ImGui::TextColored(textcolor,"%s",Utility::humanSize(thislist[n].size).c_str());
						}
						if(item.selectionstate == FILE_SELECTION_CHECKBOX && thislist[n].type == FS::FileEntryType::File){
							ImGui::Dummy(ImVec2(0.0,0.0));
							ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y-1.0f});
							
						}
						
						
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
				if (*first_item && thislist.size() >0) {
					std::string itemid = "##" + std::to_string(0);
					ImGui::SetFocusID(ImGui::GetID((itemid.c_str())), ImGui::GetCurrentWindow());
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
		Windows::ExitWindow();
	}
	
	void HttpWindow(bool *focus, bool *first_item){
		Windows::SetupWindow();
		if (ImGui::Begin("Http Browser", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_MenuBar)) {
           
            if(item.popupstate == POPUP_STATE_NONE){
				ImGui::SetNextWindowFocus();
			}
           
			if (ImGui::BeginMenuBar()) {
				ImGui::Text("current path: %s",httpdir->getCurrPath().c_str());
				ImGui::EndMenuBar();
			}
			float total_w = ImGui::GetContentRegionAvail().x;
			float total_h = ImGui::GetContentRegionAvail().y;
			
			if (ImGui::BeginListBox("Http Browser Menu",ImVec2(total_w, total_h))){
				std::vector<FS::FileEntry> thislist = httpdir->getCurrList();
				for(unsigned int n=0;n<thislist.size();n++){
						
						if(thislist[n].type == FS::FileEntryType::Directory){
							ImGui::Image((void*)(intptr_t)nxmpicons.FolderTexture.id, ImVec2(40,40));
						}else{
							ImGui::Image((void*)(intptr_t)nxmpicons.FileTexture.id, ImVec2(40,40));
						}
						ImGui::SameLine();
						std::string itemid = "##" + std::to_string(n);
						static int selected = -1;
						urlschema thisurl = Utility::parseUrl(httpdir->getUrl());
						if(item.selectionstate == FILE_SELECTION_CHECKBOX){
							
							if(thislist[n].type == FS::FileEntryType::Directory){
								ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize) / 2});
								if (ImGui::Selectable(itemid.c_str(), selected == n)){
									item.first_item = true;
									httpdir->DirList(httpdir->getCurrPath() + thislist[n].path,Utility::getMediaExtensions());
								}
								
							}else if (thislist[n].type == FS::FileEntryType::File){
								
								std::string openurl = thisurl.scheme + std::string("://") + thisurl.server + std::string("/") + httpdir->getCurrPath() + thislist[n].name;
								std::string checkitemid = "##check" + std::to_string(n);
								ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize - ImGui::GetStyle().FramePadding.y * 2) / 2});
								if(ImGui::Checkbox(checkitemid.c_str(), httpdir->checked(n))){
									if(*httpdir->checked(n)){
										playlist->appendFile(thislist[n].name,openurl);
									} else {
										playlist->removeFile(thislist[n].name,openurl);
									}
								}		
							}
							if (ImGui::IsItemHovered()){
								item.fileHoveredidx = n;
							}
							
						}else if(item.selectionstate == FILE_SELECTION_NONE){
							ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize) / 2});
					
							static int selected = -1;
							if (ImGui::Selectable(itemid.c_str(), selected == n)){
										
								if(thislist[n].type == FS::FileEntryType::Directory){
									item.first_item = true;
									httpdir->DirList(httpdir->getCurrPath() + thislist[n].path,Utility::getMediaExtensions());
										
								}else if (thislist[n].type == FS::FileEntryType::File){
									
										urlschema thisurl = Utility::parseUrl(httpdir->getUrl());
										std::string openurl = thisurl.scheme + std::string("://") + thisurl.server + std::string("/") + httpdir->getCurrPath() + thislist[n].name;
										item.laststate = item.state;
										playlist->clearPlaylist();
										httpdir->clearChecked();
										libmpv->loadFile(openurl);
										libmpv->getFileInfo()->resume = sqlitedb->getResume(openurl);
										if(libmpv->getFileInfo()->resume>0){
											item.popupstate = POPUP_STATE_RESUME;
										}
									
								}
							}
							if (ImGui::IsItemHovered()){
								item.fileHoveredidx = n;
							}
						}
						ImGui::SameLine();
						ImVec4* colors = ImGui::GetStyle().Colors;
						ImVec4 textcolor = colors[ImGuiCol_Text];
						
						if(sqlitedb != nullptr){
							std::string openurl = thisurl.scheme + std::string("://") + thisurl.server + std::string("/") + httpdir->getCurrPath() + thislist[n].name;
							int dbfilestatus = sqlitedb->getFileDbStatus(openurl);
							if(dbfilestatus == 2){
								textcolor = ImVec4(0.0f,1.0f,0.0f,1.0f);
							}
							if(dbfilestatus == 1){
								textcolor = ImVec4(0.0f,1.0f,1.0f,1.0f);
							}
							
						}
						std::string nametext = Utility::truncateLen(thislist[n].name,87);
						ImGui::TextColored(textcolor,"%s",nametext.c_str());
						
						if(item.selectionstate == FILE_SELECTION_CHECKBOX && thislist[n].type == FS::FileEntryType::File){
							ImGui::Dummy(ImVec2(0.0,0.0));
							ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y-1.0f});
							
						}
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
				if (*first_item && thislist.size() >0) {
					ImGui::SetFocusID(ImGui::GetID((thislist[0].name.c_str())), ImGui::GetCurrentWindow());
					*first_item = false;
				}	
			}
			//ImGuiContext& g = *GImGui;
			//ImGui::NavMoveRequestTryWrapping(g.CurrentWindow, ImGuiNavMoveFlags_LoopY);
		
			ImGui::EndListBox();
		}
		Windows::ExitWindow();
	}
	
	void SSHWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		std::vector<std::string> topmenu = {"Local Files","Network","Enigma2"};
		static unsigned int lastfocus = 0;
		
        if (ImGui::Begin("SSH Browser", nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
            if(item.popupstate == POPUP_STATE_NONE){
				ImGui::SetNextWindowFocus();
			}
            
			if (ImGui::BeginMenuBar()) {
				ImGui::Text("current path: %s",sshdir->getCurrPath().c_str());
				ImGui::EndMenuBar();
			}
			float total_w = ImGui::GetContentRegionAvail().x;
			float total_h = ImGui::GetContentRegionAvail().y;
			if (ImGui::BeginListBox("SSH Browser Menu",ImVec2(total_w, total_h))){
				
				urlschema thisurl = Utility::parseUrl(sshdir->getUrl());
				std::vector<FS::FileEntry> thislist = sshdir->getCurrList();
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
									sshdir->DirList(thislist[n].path,configini->getshowHidden(false),Utility::getMediaExtensions());
								}
							}else{
								ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize - ImGui::GetStyle().FramePadding.y * 2) / 2});
								std::string checkitemid = "##check" + std::to_string(n);
								if(thislist[n].type != FS::FileEntryType::Directory){
									if(ImGui::Checkbox(checkitemid.c_str(), sshdir->checked(n))){
										std::string openurl = thisurl.scheme + std::string("://") + thisurl.user + std::string(":") + thisurl.pass + std::string("@") + thisurl.server + thislist[n].path;
										if(*sshdir->checked(n)){
											playlist->appendFile(thislist[n].name,openurl);
										} else {
											playlist->removeFile(thislist[n].name,openurl);
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
								if(sshdir->getCurrList()[n].type == FS::FileEntryType::Directory){
									triggerselect = true;
									sshdir->DirList(thislist[n].path,configini->getshowHidden(false),Utility::getMediaExtensions());
								}
								else{
									item.laststate = item.state;
									playlist->clearPlaylist();
									sshdir->clearChecked();
									std::string openurl = thisurl.scheme + std::string("://") + thisurl.user + std::string(":") + thisurl.pass + std::string("@") + thisurl.server + thislist[n].path;
									libmpv->loadFile(openurl);
									if(configini->getDbActive(true)){
										libmpv->getFileInfo()->resume = sqlitedb->getResume(openurl);
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
							std::string openurl = thisurl.scheme + std::string("://") + thisurl.user + std::string(":") + thisurl.pass + std::string("@") + thisurl.server + thislist[n].path;
							int dbfilestatus = sqlitedb->getFileDbStatus(openurl);
							if(dbfilestatus == 2){
								textcolor = ImVec4(0.0f,1.0f,0.0f,1.0f);
							}
							if(dbfilestatus == 1){
								textcolor = ImVec4(0.0f,1.0f,1.0f,1.0f);
							}
							
						}
						ImGui::SameLine();
						std::string nametext = Utility::truncateLen(thislist[n].name,87);
						ImGui::TextColored(textcolor,"%s",nametext.c_str());
						
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
			//ImGuiContext& g = *GImGui;
			//ImGui::NavMoveRequestTryWrapping(g.CurrentWindow, ImGuiNavMoveFlags_LoopY);
			ImGui::EndListBox();		
			
        }
        Windows::ExitWindow();
    }
	
	void NFSWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		std::vector<std::string> topmenu = {"Local Files","Network","Enigma2"};
		static unsigned int lastfocus = 0;
		
        if (ImGui::Begin("SSH Browser", nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
            if(item.popupstate == POPUP_STATE_NONE){
				ImGui::SetNextWindowFocus();
			}
            
			if (ImGui::BeginMenuBar()) {
				ImGui::Text("current path: %s",nfsdir->getCurrPath().c_str());
				ImGui::EndMenuBar();
			}
			float total_w = ImGui::GetContentRegionAvail().x;
			float total_h = ImGui::GetContentRegionAvail().y;
			if (ImGui::BeginListBox("SSH Browser Menu",ImVec2(total_w, total_h))){
				
				urlschema thisurl = Utility::parseUrl(nfsdir->getUrl());
				std::vector<FS::FileEntry> thislist = nfsdir->getCurrList();
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
									nfsdir->DirList(thislist[n].path,configini->getshowHidden(false),Utility::getMediaExtensions());
								}
							}else{
								ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize - ImGui::GetStyle().FramePadding.y * 2) / 2});
								std::string checkitemid = "##check" + std::to_string(n);
								if(thislist[n].type != FS::FileEntryType::Directory){
									if(ImGui::Checkbox(checkitemid.c_str(), nfsdir->checked(n))){
										std::string openurl = thisurl.scheme + std::string("://") + thisurl.server + thislist[n].path;
										if(*nfsdir->checked(n)){
											playlist->appendFile(thislist[n].name,openurl);
										} else {
											playlist->removeFile(thislist[n].name,openurl);
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
								if(nfsdir->getCurrList()[n].type == FS::FileEntryType::Directory){
									triggerselect = true;
									nfsdir->DirList(thislist[n].path,configini->getshowHidden(false),Utility::getMediaExtensions());
								}
								else{
									item.laststate = item.state;
									playlist->clearPlaylist();
									nfsdir->clearChecked();
									std::string openurl = thisurl.scheme + std::string("://") + thisurl.server + thislist[n].path;
									libmpv->loadFile(openurl);
									if(configini->getDbActive(true)){
										libmpv->getFileInfo()->resume = sqlitedb->getResume(openurl);
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
							std::string openurl = thisurl.scheme + std::string("://") + thisurl.server + thislist[n].path;
							int dbfilestatus = sqlitedb->getFileDbStatus(openurl);
							if(dbfilestatus == 2){
								textcolor = ImVec4(0.0f,1.0f,0.0f,1.0f);
							}
							if(dbfilestatus == 1){
								textcolor = ImVec4(0.0f,1.0f,1.0f,1.0f);
							}
							
						}
						ImGui::SameLine();
						
						std::string nametext = Utility::truncateLen(thislist[n].name,87);
						ImGui::TextColored(textcolor,"%s",nametext.c_str());
						
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
			//ImGuiContext& g = *GImGui;
			//ImGui::NavMoveRequestTryWrapping(g.CurrentWindow, ImGuiNavMoveFlags_LoopY);
			ImGui::EndListBox();		
			
        }
        Windows::ExitWindow();
    }
	
	void SambaWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		std::vector<std::string> topmenu = {"Local Files","Network","Enigma2"};
		static unsigned int lastfocus = 0;
		
        if (ImGui::Begin("SSH Browser", nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
            if(item.popupstate == POPUP_STATE_NONE){
				ImGui::SetNextWindowFocus();
			}
            
			if (ImGui::BeginMenuBar()) {
				ImGui::Text("current path: %s",sambadir->getCurrPath().c_str());
				ImGui::EndMenuBar();
			}
			float total_w = ImGui::GetContentRegionAvail().x;
			float total_h = ImGui::GetContentRegionAvail().y;
			if (ImGui::BeginListBox("Samba Browser Menu",ImVec2(total_w, total_h))){
				
				urlschema thisurl = Utility::parseUrl(sambadir->getUrl());
				std::vector<FS::FileEntry> thislist = sambadir->getCurrList();
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
									sambadir->DirList(thislist[n].path,configini->getshowHidden(false),Utility::getMediaExtensions());
								}
							}else{
								ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize - ImGui::GetStyle().FramePadding.y * 2) / 2});
								std::string checkitemid = "##check" + std::to_string(n);
								if(thislist[n].type != FS::FileEntryType::Directory){
									if(ImGui::Checkbox(checkitemid.c_str(), sambadir->checked(n))){
										std::string openurl = thisurl.scheme + std::string("://") + thisurl.user + std::string(":") + thisurl.pass + std::string("@") + thisurl.server + std::string("/") + sambadir->getShare() + std::string("/") + thislist[n].path;
										if(*sambadir->checked(n)){
											playlist->appendFile(thislist[n].name,openurl);
										} else {
											playlist->removeFile(thislist[n].name,openurl);
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
								if(sambadir->getCurrList()[n].type == FS::FileEntryType::Directory){
									triggerselect = true;
									sambadir->DirList(thislist[n].path,configini->getshowHidden(false),Utility::getMediaExtensions());
								}
								else{
									item.laststate = item.state;
									playlist->clearPlaylist();
									sambadir->clearChecked();
									std::string openurl = thisurl.scheme + std::string("://") + thisurl.user + std::string(":") + thisurl.pass + std::string("@") + thisurl.server + std::string("/") + sambadir->getShare() + std::string("/") + thislist[n].path;
									libmpv->loadFile(openurl);
									if(configini->getDbActive(true)){
										libmpv->getFileInfo()->resume = sqlitedb->getResume(openurl);
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
							std::string openurl = thisurl.scheme + std::string("://") + thisurl.user + std::string(":") + thisurl.pass + std::string("@") + thisurl.server + std::string("/") + sambadir->getShare() + std::string("/") + thislist[n].path;
							int dbfilestatus = sqlitedb->getFileDbStatus(openurl);
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
			//ImGuiContext& g = *GImGui;
			//ImGui::NavMoveRequestTryWrapping(g.CurrentWindow, ImGuiNavMoveFlags_LoopY);
			ImGui::EndListBox();		
			
        }
        Windows::ExitWindow();
    }
	
    void NetworkWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		std::vector<std::string> topmenu = {"Local Files","Network","Enigma2"};
		
        if (ImGui::Begin("Network Source Selection", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_MenuBar)) {
           
            ImGui::SetNextWindowFocus();
           
			if(item.networkselect){
				if (ImGui::BeginListBox("Network Source Menu",ImVec2(1280.0f*multiplyRes, 720.0f*multiplyRes))){
					for(unsigned int n=0;n<item.networksources.size();n++){
						static int selected = -1;
						urlschema thisurl = Utility::parseUrl(item.networksources[n].url);
						if(thisurl.scheme == "ftp"){
							ImGui::Image((void*)(intptr_t)nxmpicons.FTPTexture.id, ImVec2(50,50));
						}
						if(thisurl.scheme == "http"){
							ImGui::Image((void*)(intptr_t)nxmpicons.HTTPTexture.id, ImVec2(50,50));
						}
						if(thisurl.scheme == "sftp"){
							ImGui::Image((void*)(intptr_t)nxmpicons.SFTPTexture.id, ImVec2(50,50));
						}
						if(thisurl.scheme == "smb"){
							ImGui::Image((void*)(intptr_t)nxmpicons.SMBTexture.id, ImVec2(50,50));
						}
						if(thisurl.scheme == "nfs"){
							ImGui::Image((void*)(intptr_t)nxmpicons.NFSTexture.id, ImVec2(50,50));
						}
						ImGui::SameLine();
						ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (50 - ImGui::GetFont()->FontSize) / 2});
						
						if (ImGui::Selectable(item.networksources[n].name.c_str(), selected == n)){
							item.first_item = true;		
							item.networkselect = false;
							
							if(thisurl.scheme == "http" || thisurl.scheme == "https"){
								httpdir = new HTTPDir(item.networksources[n].url);
								httpdir->DirList(httpdir->getBasePath(),Utility::getMediaExtensions());
								item.state = MENU_STATE_HTTPBROWSER;
							}
							if(thisurl.scheme == "ftp"){
								ftpdir = new FTPDir(item.networksources[n].url,playlist);
								ftpdir->DirList(ftpdir->getBasePath(),Utility::getMediaExtensions());	
								item.state = MENU_STATE_FTPBROWSER;
							}
							if(thisurl.scheme == "sftp"){
								sshdir = new sshDir(item.networksources[n].url,playlist);
								sshdir->DirList(sshdir->getBasePath(),configini->getshowHidden(false),Utility::getMediaExtensions());	
								item.state = MENU_STATE_SSHBROWSER;
							}
							if(thisurl.scheme == "smb"){
								sambadir = new sambaDir(item.networksources[n].url,playlist);
								sambadir->DirList(sambadir->getBasePath(),configini->getshowHidden(false),Utility::getMediaExtensions());	
								item.state = MENU_STATE_SAMBABROWSER;
							}
							if(thisurl.scheme == "nfs"){
								nfsdir = new nfsDir(item.networksources[n].url,playlist);
								nfsdir->DirList(nfsdir->getBasePath(),configini->getshowHidden(false),Utility::getMediaExtensions());	
								item.state = MENU_STATE_NFSBROWSER;
							}
							if (selected)
								ImGui::SetItemDefaultFocus();
						}
								
					}
					if (*first_item) {
                        ImGui::SetFocusID(ImGui::GetID((item.networksources[0].name.c_str())), ImGui::GetCurrentWindow());
                        *first_item = false;
                    }
				}
				ImGui::EndListBox();
			}
		}
		Windows::ExitWindow();
	}
#endif
}