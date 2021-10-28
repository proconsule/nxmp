#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"


namespace Windows {
    void NetworkWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		std::vector<std::string> topmenu = {"Local Files","Network","Enigma2"};
		
        if (ImGui::Begin("Network Source Selection", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_MenuBar)) {
           
            ImGui::SetNextWindowFocus();
           
			if(item.networkselect){
				if (ImGui::BeginListBox("Network Source Menu",ImVec2(1280.0f, 720.0f))){
					for(unsigned int n=0;n<item.networksources.size();n++){
						static int selected = -1;
						urlschema thisurl = Utility::parseUrl(item.networksources[n].url);
						if(thisurl.scheme == "ftp"){
							ImGui::Image((void*)(intptr_t)FTPTexture.id, ImVec2(40,40));
						}
						if(thisurl.scheme == "http"){
							ImGui::Image((void*)(intptr_t)HTTPTexture.id, ImVec2(40,40));
						}
						ImGui::SameLine();
						ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize) / 2});
						
						if (ImGui::Selectable(item.networksources[n].name.c_str(), selected == n)){
							item.first_item = true;		
							item.networkselect = false;
							item.networkurl = item.networksources[n].url;
							
							if(thisurl.scheme == "http" || thisurl.scheme == "https"){
								httpdir = new HTTPDir(item.networkurl);
								item.networklastpath = httpdir->getBasePath();
								item.networkentries = httpdir->getDir(httpdir->getBasePath(),Utility::getMediaExtensions());
							}
							if(thisurl.scheme == "ftp"){
								ftpdir = new FTPDir(item.networkurl);
								item.networklastpath = ftpdir->getBasePath();
								item.networkentries = ftpdir->getDir(ftpdir->getBasePath(),Utility::getMediaExtensions());	
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
			} else {
				if (ImGui::BeginMenuBar()) {
					ImGui::Text("current path: %s",item.networklastpath.c_str());
					ImGui::EndMenuBar();
				}
				if (ImGui::BeginListBox("Network Browser Menu",ImVec2(1280.0f, 720.0f))){
					//ImGui::Text("Current Dir: %s\n",item.networklastpath.c_str());
					int total_w = ImGui::GetContentRegionAvail().x;
					for(unsigned int n=0;n<item.networkentries.size();n++){
						
							if(item.networkentries[n].type == FS::FileEntryType::Directory){
								ImGui::Image((void*)(intptr_t)FolderTexture.id, ImVec2(40,40));
							}else{
								ImGui::Image((void*)(intptr_t)FileTexture.id, ImVec2(40,40));
							}
							ImGui::SameLine();
							ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize) / 2});
						
							urlschema thisurl = Utility::parseUrl(item.networkurl);
							static int selected = -1;
							if (ImGui::Selectable(item.networkentries[n].name.c_str(), selected == n)){
										
								if(item.networkentries[n].type == FS::FileEntryType::Directory){
									item.first_item = true;
				
									if(thisurl.scheme == "http" || thisurl.scheme == "https"){
										item.networklastpath = httpdir->getCurrPath()+item.networkentries[n].path;
										item.networkentries = httpdir->getDir(httpdir->getCurrPath()+item.networkentries[n].path,Utility::getMediaExtensions());
									}
											
									if(thisurl.scheme == "ftp"){
										item.networklastpath =  item.networkentries[n].path + item.networkentries[n].name;
										item.networkentries = ftpdir->getDir(item.networklastpath,Utility::getMediaExtensions());
									
										
									}
								}else if (item.networkentries[n].type == FS::FileEntryType::File){
											
									urlschema thisurl = Utility::parseUrl(item.networkurl);
									if(thisurl.scheme == "http" || thisurl.scheme == "https"){
										std::string openurl = thisurl.scheme + std::string("://") + thisurl.server + std::string("/") + httpdir->getCurrPath() + item.networkentries[n].name;
										const char *cmd[] = {"loadfile", openurl.c_str() , NULL};
										mpv_command_async(libmpv->getHandle(), 0, cmd);
									}
									if(thisurl.scheme == "ftp"){
										std::string openurl = thisurl.scheme + std::string("://") + thisurl.user + std::string(":") + thisurl.pass + std::string("@") + thisurl.server + std::string("/") + item.networkentries[n].path + item.networkentries[n].name;
										const char *cmd[] = {"loadfile", openurl.c_str(), NULL};
										mpv_command_async(libmpv->getHandle(), 0, cmd);
									}
								}
							}
							
							if(item.networkentries[n].type == FS::FileEntryType::File && thisurl.scheme != "http"){
								ImGui::SameLine(total_w-150);
								ImGui::Text("%s",Utility::humanSize(item.networkentries[n].size).c_str());
							}
						
								
					}
					if (*first_item) {
                        ImGui::SetFocusID(ImGui::GetID((item.networkentries[0].name.c_str())), ImGui::GetCurrentWindow());
                        *first_item = false;
                    }	
				}
				ImGui::EndListBox();
			}
		}
		Windows::ExitWindow();
	}
}