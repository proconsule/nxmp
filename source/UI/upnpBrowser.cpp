#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"


namespace Windows {
    void UPNPBrowserWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		if (ImGui::Begin("UPNP Browser", nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
			ImGui::SetNextWindowFocus();
			float total_w = ImGui::GetContentRegionAvail().x;
			float total_h = ImGui::GetContentRegionAvail().y;
			if(nxupnp->getSelDevice() == -1){
				if (ImGui::BeginListBox("UPNP Browser Menu",ImVec2(total_w, total_h))){
					static int selected = -1;	
					std::vector<Device *> thislist = nxupnp->getDevicesList();
					for (unsigned int n = 0; n < thislist.size(); n++){
						std::string itemid = "##" + std::to_string(n);
						
						if(thislist[n]->devIcon.id == 0){
							
							MemoryStruct *chunk = (MemoryStruct *)malloc(sizeof(MemoryStruct));
							curlDownloader * curldownload = new curlDownloader();
							curldownload->Download((char *)thislist[n]->iconUrl.c_str(),chunk);
							Utility::TxtLoadFromMemory(chunk->memory,chunk->size,&thislist[n]->devIcon.id,&thislist[n]->devIcon.width,&thislist[n]->devIcon.height);
							free(chunk->memory);
							free(chunk);
							delete curldownload;
							
						}
						ImGui::Image((void*)(intptr_t)thislist[n]->devIcon.id, ImVec2(80,80));
						ImGui::SameLine();
						if (ImGui::Selectable(itemid.c_str(), selected == n,0,ImVec2(0,80))){
							thislist[n]->parentList.push_back("0");
							thislist[n]->browseOID();
							nxupnp->setSelDevice(n);
							
						}
						ImGui::SameLine();
						float currstartpos = ImGui::GetCursorPosX();
						
						ImGui::Text("%s",thislist[n]->getfriendlyName().c_str());
						ImGui::SetCursorPosX(currstartpos);
						ImGui::SetCursorPosY(ImGui::GetCursorPosY()-40.0f);
						ImGui::Text("%s",thislist[n]->getIP().c_str());
						ImGui::Dummy(ImVec2(0,20));
					}
				}
				ImGui::EndListBox();	
			
			} else {
				if (ImGui::BeginListBox("UPNP Browser Menu",ImVec2(total_w, total_h))){
					bool triggerselect = false;
					int total_w = ImGui::GetContentRegionAvail().x;
					static int selected = -1;	
					std::vector<upnpres_struct> thislist = nxupnp->getDevice(nxupnp->getSelDevice())->currentlist;
					for (unsigned int n = 0; n < thislist.size(); n++){
						if(thislist[n].type == UPNPTYPE::UPNPContainer){
							ImGui::Image((void*)(intptr_t)nxmpicons.FolderTexture.id, ImVec2(40,40));
						}else{
							if(thislist[n].albumart.id ==0){
								ImGui::Image((void*)(intptr_t)nxmpicons.FileTexture.id, ImVec2(40,40));
							}else{
								ImGui::Image((void*)(intptr_t)thislist[n].albumart.id, ImVec2(thislist[n].albumart.width/2,thislist[n].albumart.height/2));
							}
						}
						ImGui::SameLine();
						std::string itemid = "##" + std::to_string(n);
						
							ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize) / 2});
							
							if (ImGui::Selectable(itemid.c_str(), selected == n)){
								
								if(thislist[n].type == UPNPTYPE::UPNPContainer){
									nxupnp->getDevice(nxupnp->getSelDevice())->parentList.push_back(thislist[n].uri);
									nxupnp->getDevice(nxupnp->getSelDevice())->browseOID();
									triggerselect = true;
								}else if(thislist[n].type == UPNPTYPE::UPNPItem){
									item.laststate = item.state;
									libmpv->loadFileLive(thislist[n].uri,thislist[n].name);
									//libmpv->loadFile(thislist[n].path);
									/*
									if(configini->getDbActive(true)){
										libmpv->getFileInfo()->resume = sqlitedb->getResume(thislist[n].path);
										if(libmpv->getFileInfo()->resume>0){
											item.popupstate = POPUP_STATE_RESUME;
										}
									}
									*/
								}
							}
							/*
							if(ImGui::IsItemVisible()){
								if(thislist[n].albumart.id ==0 && thislist[n].type == UPNPTYPE::UPNPItem){
									MemoryStruct *chunk = (MemoryStruct *)malloc(sizeof(MemoryStruct));
									curlDownloader * curldownload = new curlDownloader();
									curldownload->Download(thislist[n].albumarturi.c_str(),chunk);
									Utility::TxtLoadFromMemory(chunk->memory,chunk->size,&thislist[n].albumart.id,&thislist[n].albumart.width,&thislist[n].albumart.height);
									free(chunk->memory);
									free(chunk);
									delete curldownload;
								}
							
							}
							*/
						
						ImGui::SameLine();
						ImVec4* colors = ImGui::GetStyle().Colors;
						ImVec4 textcolor = colors[ImGuiCol_Text];
						
						/*
						if(sqlitedb != nullptr){
							int dbfilestatus = sqlitedb->getFileDbStatus(thislist[n].uri);
							if(dbfilestatus == 2){
								textcolor = ImVec4(0.0f,1.0f,0.0f,1.0f);
							}
							if(dbfilestatus == 1){
								textcolor = ImVec4(0.0f,1.0f,1.0f,1.0f);
							}
							
						}
						*/
						std::string nametext = Utility::truncateLen(thislist[n].name,87);
						ImGui::TextColored(textcolor,"%s",nametext.c_str());
						
						if(thislist[n].type == UPNPTYPE::UPNPItem){
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
				ImGui::EndListBox();
			}
			
		}
	
	Windows::ExitWindow();
    }
}
