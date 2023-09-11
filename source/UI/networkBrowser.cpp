#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"

namespace Windows {
	
	int netwinselected = -1;

	void ShareAddWindow(bool *focus, bool *first_item){
		Windows::SetupWindow();
		if (ImGui::Begin("Add Share", nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_MenuBar)) {
           
            if(item.popupstate == POPUP_STATE_NONE){
				//ImGui::SetNextWindowFocus();
			}
           
			if (ImGui::BeginMenuBar()) {
				ImGui::Text("Add Share Menu");
				ImGui::EndMenuBar();
			}
			NewNetworkShare->name = InputSwitchKeyboard("##sharename","Share Name:",NewNetworkShare->name);
			
			ImGui::Text("Protocol: ");
			ImGui::SameLine();
			//ImGui::Combo("##protocombo", (int *)&NewNetworkShare->type, NewNetworkShare->protonames, 5);
			static const char* item_current = NULL;
			if (ImGui::BeginCombo("##protocombo", item_current, 0)) // The second parameter is the label previewed before opening the combo.
			{
				for (int n = 0; n < IM_ARRAYSIZE(NewNetworkShare->protonames); n++)
				{
					bool is_selected = (item_current == NewNetworkShare->protonames[n]);
					
					if(std::string(NewNetworkShare->protonames[n]) == "FTP"){
						GUI::NXMPImage((void*)(intptr_t)nxmpicons.FTPTexture.id, ImVec2(30,30));
					}
					if(std::string(NewNetworkShare->protonames[n]) == "HTTP"){
						GUI::NXMPImage((void*)(intptr_t)nxmpicons.HTTPTexture.id, ImVec2(30,30));
					}
					if(std::string(NewNetworkShare->protonames[n]) == "SMB"){
						GUI::NXMPImage((void*)(intptr_t)nxmpicons.NFSTexture.id, ImVec2(30,30));
					}
					if(std::string(NewNetworkShare->protonames[n]) == "NFS"){
						GUI::NXMPImage((void*)(intptr_t)nxmpicons.FTPTexture.id, ImVec2(30,30));
					}
					if(std::string(NewNetworkShare->protonames[n]) == "SSH"){
						GUI::NXMPImage((void*)(intptr_t)nxmpicons.SFTPTexture.id, ImVec2(30,30));
					}
					
					ImGui::SameLine();
								
					if (ImGui::Selectable(NewNetworkShare->protonames[n], is_selected)){
						item_current = NewNetworkShare->protonames[n];
						NewNetworkShare->type = n;
						NewNetworkShare->port = NewNetworkShare->protodefports[n];
					}
						
					if (is_selected)
						ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
				}
				ImGui::EndCombo();
			}
			
			
			if(NewNetworkShare->type == 0){
				NewNetworkShare->httpstring = InputSwitchKeyboard("##httpstring","HTTP String",NewNetworkShare->httpstring);
			
			}
			
			if(NewNetworkShare->type >= 1){
				NewNetworkShare->address = InputSwitchKeyboard("##address","IP Address:",NewNetworkShare->address);
				if(NewNetworkShare->type ==3 || NewNetworkShare->type ==4){
					
				}else{
					NewNetworkShare->port = std::stoi(InputSwitchKeyboard("##port","Port:",std::to_string(NewNetworkShare->port)));
				}
				ImGui::Checkbox("Anonymous", &NewNetworkShare->anon);
				if(!NewNetworkShare->anon){
					NewNetworkShare->username = InputSwitchKeyboard("##username","Username:",NewNetworkShare->username);
					NewNetworkShare->password = InputSwitchKeyboard("##password","Password:",NewNetworkShare->password);
			
				}
				NewNetworkShare->remoteshare = InputSwitchKeyboard("##remoteshare","Remote Path:",NewNetworkShare->remoteshare);
				if(NewNetworkShare->type == 3){
					ImGui::Text("Remote share on server (share not path)");
				}else if(NewNetworkShare->type == 4){
					ImGui::Text("Remote export on server");
				}else{
					ImGui::Text("Remote path on server example: /movie/");
				}
				
			}
			
			
			ImGui::SetCursorPosX(ImGui::GetWindowSize().x/2);
		ImGui::SetCursorPosY(ImGui::GetWindowSize().y -50);
			
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));
		
		if(!NewNetworkShare->isValidShare()){
			 ImGui::BeginDisabled();
		}
		
		if (ImGui::Button("Save")){
			item.state = MENU_STATE_NETWORKBROWSER;
			configini->addNetworkShare(NewNetworkShare->GenConfigLine());
		}
		if(!NewNetworkShare->isValidShare()){
			 ImGui::EndDisabled();
		}
		ImGui::PopStyleColor(3);
		ImGui::SameLine(0,50);
		if (ImGui::Button("Cancel")){
				
			item.state = MENU_STATE_NETWORKBROWSER;
		}
			
			
		}
		Windows::ExitWindow();
	}

	void FtpWindow(bool *focus, bool *first_item){
		Windows::SetupWindow();
		if (ImGui::Begin("Ftp Browser", nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_MenuBar)) {
           
            if(item.popupstate == POPUP_STATE_NONE){
				ImGui::SetNextWindowFocus();
			}
           
			if (ImGui::BeginMenuBar()) {
				ImGui::Text("current path: %s",filebrowser->getCurrentPath().c_str());
				ImGui::EndMenuBar();
			}
			float total_w = ImGui::GetContentRegionAvail().x;
			float total_h = ImGui::GetContentRegionAvail().y;
			if (ImGui::BeginListBox("Ftp Browser Menu",ImVec2(total_w, total_h))){
				//ImGui::Text("Current Dir: %s\n",item.networklastpath.c_str());
				int total_w = ImGui::GetContentRegionAvail().x;
				std::vector<FS::FileEntry> thislist = filebrowser->getCurrList();
				bool triggerselect = false;
				for(unsigned int n=0;n<thislist.size();n++){
						
						if(thislist[n].type == FS::FileEntryType::Directory){
							GUI::NXMPImage((void*)(intptr_t)nxmpicons.FolderTexture.id, ImVec2(40,40));
						}else{
							GUI::NXMPImage((void*)(intptr_t)nxmpicons.FileTexture.id, ImVec2(40,40));
						}
						ImGui::SameLine();
						
						urlschema thisurl = Utility::parseUrl(filebrowser->getUrl());
						static int selected = -1;
						std::string itemid = "##" + std::to_string(n);
						if(item.selectionstate == FILE_SELECTION_CHECKBOX){
							if(thislist[n].type == FS::FileEntryType::Directory){
								ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40*multiplyRes - ImGui::GetFont()->FontSize) / 2});
								if (ImGui::Selectable(itemid.c_str(), selected == n)){
									triggerselect = true;
									filebrowser->DirList(thislist[n].path + thislist[n].name,false,Utility::getMediaExtensions());
								}
							}else{
								ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40*multiplyRes - ImGui::GetFont()->FontSize - ImGui::GetStyle().FramePadding.y * 2) / 2});
								std::string checkitemid = "##check" + std::to_string(n);
								if(thislist[n].type != FS::FileEntryType::Directory){
									std::string openurl = thisurl.scheme + std::string("://") + thisurl.user + std::string(":") + thisurl.pass + std::string("@") + thisurl.server + std::string("/") + thislist[n].path + thislist[n].name;
									if(ImGui::Checkbox(checkitemid.c_str(), filebrowser->checked(n))){
										if(*filebrowser->checked(n)){
											playlist->appendFile(thislist[n],openurl);
										} else {
											playlist->removeFile(thislist[n],openurl);
										}
									}
								}
							}
							if (ImGui::IsItemHovered()){
								item.fileHoveredidx = n;
							}
							
						}else if(item.selectionstate == FILE_SELECTION_NONE){
							ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40*multiplyRes - ImGui::GetFont()->FontSize) / 2});
							if (ImGui::Selectable(itemid.c_str(), selected == n)){
										
								if(thislist[n].type == FS::FileEntryType::Directory){
									triggerselect = true;
									filebrowser->DirList(thislist[n].path + thislist[n].name,false,Utility::getMediaExtensions());
									
								}else if (thislist[n].type == FS::FileEntryType::File){
											
									std::string openurl = thisurl.scheme + std::string("://") + thisurl.user + std::string(":") + thisurl.pass + std::string("@") + thisurl.server + std::string("/") + thislist[n].path + thislist[n].name;
									item.laststate = item.state;
									playlist->clearPlaylist();
									filebrowser->clearChecked();
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
				ImGui::Text("current path: %s",filebrowser->getCurrentPath().c_str());
				ImGui::EndMenuBar();
			}
			float total_w = ImGui::GetContentRegionAvail().x;
			float total_h = ImGui::GetContentRegionAvail().y;
			
			if (ImGui::BeginListBox("Http Browser Menu",ImVec2(total_w, total_h))){
				std::vector<FS::FileEntry> thislist = filebrowser->getCurrList();
				for(unsigned int n=0;n<thislist.size();n++){
						
						if(thislist[n].type == FS::FileEntryType::Directory){
							GUI::NXMPImage((void*)(intptr_t)nxmpicons.FolderTexture.id, ImVec2(40,40));
						}else{
							GUI::NXMPImage((void*)(intptr_t)nxmpicons.FileTexture.id, ImVec2(40,40));
						}
						ImGui::SameLine();
						std::string itemid = "##" + std::to_string(n);
						static int selected = -1;
						urlschema thisurl = Utility::parseUrl(filebrowser->getUrl());
						if(item.selectionstate == FILE_SELECTION_CHECKBOX){
							
							if(thislist[n].type == FS::FileEntryType::Directory){
								ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40*multiplyRes - ImGui::GetFont()->FontSize) / 2});
								if (ImGui::Selectable(itemid.c_str(), selected == n)){
									item.first_item = true;
									filebrowser->DirList(filebrowser->getCurrentPath() + thislist[n].path,false,Utility::getMediaExtensions());
								}
								
							}else if (thislist[n].type == FS::FileEntryType::File){
								
								std::string openurl = thisurl.scheme + std::string("://") + thisurl.server + (thisurl.port.empty() ? std::string() : ':' + thisurl.port) + std::string("/") + filebrowser->getCurrentPath() + thislist[n].name;
								std::string checkitemid = "##check" + std::to_string(n);
								ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40*multiplyRes - ImGui::GetFont()->FontSize - ImGui::GetStyle().FramePadding.y * 2) / 2});
								if(ImGui::Checkbox(checkitemid.c_str(), filebrowser->checked(n))){
									if(*filebrowser->checked(n)){
										playlist->appendFile(thislist[n],openurl);
									} else {
										playlist->removeFile(thislist[n],openurl);
									}
								}		
							}
							if (ImGui::IsItemHovered()){
								item.fileHoveredidx = n;
							}
							
						}else if(item.selectionstate == FILE_SELECTION_NONE){
							ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40*multiplyRes - ImGui::GetFont()->FontSize) / 2});
					
							static int selected = -1;
							if (ImGui::Selectable(itemid.c_str(), selected == n)){
										
								if(thislist[n].type == FS::FileEntryType::Directory){
									item.first_item = true;
									filebrowser->DirList(filebrowser->getCurrentPath() + thislist[n].path,false,Utility::getMediaExtensions());
										
								}else if (thislist[n].type == FS::FileEntryType::File){
									
										urlschema thisurl = Utility::parseUrl(filebrowser->getUrl());
										std::string openurl = thisurl.scheme + std::string("://") + thisurl.server + (thisurl.port.empty() ? std::string() : ':' + thisurl.port) + std::string("/") + filebrowser->getCurrentPath() + thislist[n].name;
										item.laststate = item.state;
										playlist->clearPlaylist();
										filebrowser->clearChecked();
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
							std::string openurl = thisurl.scheme + std::string("://") + thisurl.server + (thisurl.port.empty() ? std::string() : ':' + thisurl.port) + std::string("/") + filebrowser->getCurrentPath() + thislist[n].name;
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
		
        if (ImGui::Begin("SSH Browser", nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
            if(item.popupstate == POPUP_STATE_NONE){
				ImGui::SetNextWindowFocus();
			}
            
			if (ImGui::BeginMenuBar()) {
				ImGui::Text("current path: %s",filebrowser->getCurrentPath().c_str());
				ImGui::EndMenuBar();
			}
			float total_w = ImGui::GetContentRegionAvail().x;
			float total_h = ImGui::GetContentRegionAvail().y;
			if (ImGui::BeginListBox("SSH Browser Menu",ImVec2(total_w, total_h))){
				
				urlschema thisurl = Utility::parseUrl(filebrowser->getUrl());
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
										std::string openurl = thisurl.scheme + std::string("://") + thisurl.user + std::string(":") + thisurl.pass + std::string("@") + thisurl.server + thislist[n].path;
										if(*filebrowser->checked(n)){
											playlist->appendFile(thislist[n],openurl);
										} else {
											playlist->removeFile(thislist[n],openurl);
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
		
        if (ImGui::Begin("NFS Browser", nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
            if(item.popupstate == POPUP_STATE_NONE){
				ImGui::SetNextWindowFocus();
			}
            
			if (ImGui::BeginMenuBar()) {
				ImGui::Text("current path: %s",filebrowser->getCurrentPath().c_str());
				ImGui::EndMenuBar();
			}
			float total_w = ImGui::GetContentRegionAvail().x;
			float total_h = ImGui::GetContentRegionAvail().y;
			if (ImGui::BeginListBox("NFS Browser Menu",ImVec2(total_w, total_h))){
				
				urlschema thisurl = Utility::parseUrl(filebrowser->getUrl());
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
										std::string openurl = thisurl.scheme + std::string("://") + thisurl.server + thislist[n].path;
										if(*filebrowser->checked(n)){
											playlist->appendFile(thislist[n],openurl);
										} else {
											playlist->removeFile(thislist[n],openurl);
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
		
        if (ImGui::Begin("SMB Browser", nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
            if(item.popupstate == POPUP_STATE_NONE){
				ImGui::SetNextWindowFocus();
			}
            
			if (ImGui::BeginMenuBar()) {
				ImGui::Text("current path: %s",filebrowser->getCurrentPath().c_str());
				ImGui::EndMenuBar();
			}
			float total_w = ImGui::GetContentRegionAvail().x;
			float total_h = ImGui::GetContentRegionAvail().y;
			if (ImGui::BeginListBox("Samba Browser Menu",ImVec2(total_w, total_h))){
				
				urlschema thisurl = Utility::parseUrl(filebrowser->getUrl());
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
										std::string openurl = thisurl.scheme + std::string("://") + thisurl.user + std::string(":") + thisurl.pass + std::string("@") + thisurl.server + std::string("/") + filebrowser->getShare() + std::string("/") + thislist[n].path;
										if(*filebrowser->checked(n)){
											playlist->appendFile(thislist[n],openurl);
										} else {
											playlist->removeFile(thislist[n],openurl);
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
									std::string openurl = thisurl.scheme + std::string("://") + thisurl.user + std::string(":") + thisurl.pass + std::string("@") + thisurl.server + std::string("/") + filebrowser->getShare() + std::string("/") + thislist[n].path;
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
							std::string openurl = thisurl.scheme + std::string("://") + thisurl.user + std::string(":") + thisurl.pass + std::string("@") + thisurl.server + std::string("/") + filebrowser->getShare() + std::string("/") + thislist[n].path;
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
           
            if(item.popupstate == POPUP_STATE_NONE){
				ImGui::SetNextWindowFocus();
			}
           
			if(item.networkselect){
				if (ImGui::BeginListBox("Network Source Menu",ImVec2(1280.0f*multiplyRes, 720.0f*multiplyRes))){
					GUI::NXMPImage((void*)(intptr_t)nxmpicons.ShareAddTexture.id, ImVec2(50,50));
					ImGui::SameLine();
					ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (50 - ImGui::GetFont()->FontSize) / 2});
					static int selected = -1;
					if (ImGui::Selectable("Add Share", selected == 0)){
						NewNetworkShare = new CNetworkShare();
						item.state = MENU_STATE_ADDSHARE;
					}	
					if(ImGui::IsItemHovered()){
						netwinselected = -1;
					}
					
					for(unsigned int n=0;n<item.networksources.size();n++){
						
						urlschema thisurl = Utility::parseUrl(item.networksources[n].url);
						if(thisurl.scheme == "ftp"){
							GUI::NXMPImage((void*)(intptr_t)nxmpicons.FTPTexture.id, ImVec2(50,50));
						}
						if(thisurl.scheme == "http"){
							GUI::NXMPImage((void*)(intptr_t)nxmpicons.HTTPTexture.id, ImVec2(50,50));
						}
						if(thisurl.scheme == "sftp"){
							GUI::NXMPImage((void*)(intptr_t)nxmpicons.SFTPTexture.id, ImVec2(50,50));
						}
						if(thisurl.scheme == "smb"){
							GUI::NXMPImage((void*)(intptr_t)nxmpicons.SMBTexture.id, ImVec2(50,50));
						}
						if(thisurl.scheme == "nfs"){
							GUI::NXMPImage((void*)(intptr_t)nxmpicons.NFSTexture.id, ImVec2(50,50));
						}
						ImGui::SameLine();
						ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (50*multiplyRes - ImGui::GetFont()->FontSize) / 2});
						
						if (ImGui::Selectable(item.networksources[n].name.c_str(), selected == n+1)){
							item.first_item = true;		
							item.networkselect = false;
							
							if(thisurl.scheme == "http" || thisurl.scheme == "https"){
								filebrowser = new CFileBrowser(item.networksources[n].url,playlist);
								filebrowser->DirList(filebrowser->getBasePath(),false,Utility::getMediaExtensions());
								item.state = MENU_STATE_HTTPBROWSER;
							}
							if(thisurl.scheme == "ftp"){
								filebrowser = new CFileBrowser(item.networksources[n].url,playlist);
								filebrowser->DirList(filebrowser->getBasePath(),false,Utility::getMediaExtensions());	
								item.state = MENU_STATE_FTPBROWSER;
							}
							if(thisurl.scheme == "sftp"){
								filebrowser = new CFileBrowser(item.networksources[n].url,playlist);
								filebrowser->DirList(filebrowser->getBasePath(),configini->getshowHidden(false),Utility::getMediaExtensions());	
								item.state = MENU_STATE_SSHBROWSER;
							}
							if(thisurl.scheme == "smb"){
								filebrowser = new CFileBrowser(item.networksources[n].url,playlist);
								filebrowser->DirList(filebrowser->getBasePath(),configini->getshowHidden(false),Utility::getMediaExtensions());	
								item.state = MENU_STATE_SAMBABROWSER;
							}
							if(thisurl.scheme == "nfs"){
								filebrowser = new CFileBrowser(item.networksources[n].url,playlist);
								filebrowser->DirList(filebrowser->getBasePath(),configini->getshowHidden(false),Utility::getMediaExtensions());	
								item.state = MENU_STATE_NFSBROWSER;
							}
							
							if (selected){
								
								ImGui::SetItemDefaultFocus();
							}
						}
						if(ImGui::IsItemHovered()){
								netwinselected = n;
						}
								
					}
					
				
					//netwinselected = selected;
								
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
}
