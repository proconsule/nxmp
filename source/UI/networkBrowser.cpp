#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"


namespace Windows {
	
	void FtpWindow(bool *focus, bool *first_item){
		Windows::SetupWindow();
		if (ImGui::Begin("Ftp Browser", nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_MenuBar)) {
           
            ImGui::SetNextWindowFocus();
           
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
							ImGui::Image((void*)(intptr_t)FolderTexture.id, ImVec2(40,40));
						}else{
							ImGui::Image((void*)(intptr_t)FileTexture.id, ImVec2(40,40));
						}
						ImGui::SameLine();
						ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize) / 2});
					
						urlschema thisurl = Utility::parseUrl(ftpdir->getUrl());
						static int selected = -1;
						std::string itemid = "##" + std::to_string(n);
						if (ImGui::Selectable(itemid.c_str(), selected == n)){
										
							if(thislist[n].type == FS::FileEntryType::Directory){
								triggerselect = true;
								ftpdir->DirList(thislist[n].path + thislist[n].name,Utility::getMediaExtensions());
									
							}else if (thislist[n].type == FS::FileEntryType::File){
											
									std::string openurl = thisurl.scheme + std::string("://") + thisurl.user + std::string(":") + thisurl.pass + std::string("@") + thisurl.server + std::string("/") + thislist[n].path + thislist[n].name;
									libmpv->loadFile(openurl);
									if(configini->getDbActive(true)){
										libmpv->getFileInfo()->resume = sqlitedb->getResume(openurl);
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
			ImGui::EndListBox();
		}
		Windows::ExitWindow();
	}
	
	void HttpWindow(bool *focus, bool *first_item){
		Windows::SetupWindow();
		if (ImGui::Begin("Http Browser", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_MenuBar)) {
           
            ImGui::SetNextWindowFocus();
           
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
							ImGui::Image((void*)(intptr_t)FolderTexture.id, ImVec2(40,40));
						}else{
							ImGui::Image((void*)(intptr_t)FileTexture.id, ImVec2(40,40));
						}
						ImGui::SameLine();
						ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize) / 2});
					
						static int selected = -1;
						if (ImGui::Selectable(thislist[n].name.c_str(), selected == n)){
										
							if(thislist[n].type == FS::FileEntryType::Directory){
								item.first_item = true;
								httpdir->DirList(httpdir->getCurrPath() + thislist[n].path,Utility::getMediaExtensions());
									
							}else if (thislist[n].type == FS::FileEntryType::File){
									
									urlschema thisurl = Utility::parseUrl(httpdir->getUrl());
									std::string openurl = thisurl.scheme + std::string("://") + thisurl.server + std::string("/") + httpdir->getCurrPath() + thislist[n].name;
									libmpv->loadFile(openurl);
									libmpv->getFileInfo()->resume = sqlitedb->getResume(openurl);
									if(libmpv->getFileInfo()->resume>0){
										item.popupstate = POPUP_STATE_RESUME;
									}
									
							}
						}
						
				}
				if (*first_item && thislist.size() >0) {
					ImGui::SetFocusID(ImGui::GetID((thislist[0].name.c_str())), ImGui::GetCurrentWindow());
					*first_item = false;
				}	
			}
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
							
							if(thisurl.scheme == "http" || thisurl.scheme == "https"){
								httpdir = new HTTPDir(item.networksources[n].url);
								httpdir->DirList(httpdir->getBasePath(),Utility::getMediaExtensions());
								item.state = MENU_STATE_HTTPBROWSER;
							}
							if(thisurl.scheme == "ftp"){
								ftpdir = new FTPDir(item.networksources[n].url);
								ftpdir->DirList(ftpdir->getBasePath(),Utility::getMediaExtensions());	
								item.state = MENU_STATE_FTPBROWSER;
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
}