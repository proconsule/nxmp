#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "windows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"


namespace Windows {
    void NetworkWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		static unsigned int item_current_idx = 0;
        std::vector<std::string> topmenu = {"Local Files","Network","Enigma2"};
		
        if (ImGui::Begin("Network Browser", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_MenuBar)) {
           
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
								item.networklastpath = "";
								item_current_idx = 0;
								httpdir->setUrl(item.networkurl = item.networksources[n].url);
								item.networkentries = httpdir->dirList("");
								std::sort(item.networkentries.begin(),item.networkentries.end(),Utility::compare);
							}
							if(thisurl.scheme == "ftp"){
								item.networklastpath = thisurl.path;
								netbuf *ftp_con = nullptr;
								printf("FTP CONNECT %s\n",thisurl.server.c_str());
								if(thisurl.port == "")thisurl.port = "21";
								std::string ftphost = thisurl.server+std::string(":")+thisurl.port;
								item_current_idx = 0;
								if (!FtpConnect(ftphost.c_str(), &ftp_con)) {
									printf("could not connect to ftp server\n");
								}else{
									
									if (!FtpLogin(thisurl.user.c_str(), thisurl.pass.c_str(), ftp_con)) {
									printf("could not connect to ftp server\n");
										FtpQuit(ftp_con);
									}else{
										item.networkentries = FtpDirList(thisurl.path.c_str(), ftp_con);
										std::sort(item.networkentries.begin(),item.networkentries.end(),Utility::compare);
									}
								}
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
			else{
				if (ImGui::BeginMenuBar()) {
					ImGui::Text("current path: %s",item.networklastpath.c_str());
					ImGui::EndMenuBar();
				}
				if (ImGui::BeginListBox("Network Browser Menu",ImVec2(1280.0f, 720.0f))){
					//ImGui::Text("Current Dir: %s\n",item.networklastpath.c_str());
					int total_w = ImGui::GetContentRegionAvail().x;
					for(unsigned int n=0;n<item.networkentries.size();n++){
						if(item.networkentries[n].isDir || FS::GetFileType(item.networkentries[n].name) != FileTypeNone){
							if(item.networkentries[n].isDir){
								ImGui::Image((void*)(intptr_t)FolderTexture.id, ImVec2(40,40));
							}else{
								ImGui::Image((void*)(intptr_t)FileTexture.id, ImVec2(40,40));
							}
							ImGui::SameLine();
							ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40 - ImGui::GetFont()->FontSize) / 2});
						
							urlschema thisurl = Utility::parseUrl(item.networkurl);
							static int selected = -1;
							if (ImGui::Selectable(item.networkentries[n].name.c_str(), selected == n)){
										
								if(item.networkentries[n].isDir){
									item.first_item = true;
									item_current_idx = 0;
									if(thisurl.scheme == "http" || thisurl.scheme == "https"){
										item.networkentries = httpdir->dirList(item.networkentries[n].path);
										item.networklastpath = httpdir->getCurrentRelPath();
										std::sort(item.networkentries.begin(),item.networkentries.end(),Utility::compare);
									}
											
									if(thisurl.scheme == "ftp"){
										netbuf *ftp_con = nullptr;
										if(thisurl.port == "")thisurl.port = "21";
										std::string ftphost = thisurl.server+std::string(":")+thisurl.port;
										if (!FtpConnect(ftphost.c_str(), &ftp_con)) {
											printf("could not connect to ftp server\n");
										}else{
											
											if (!FtpLogin(thisurl.user.c_str(), thisurl.pass.c_str(), ftp_con)) {
												printf("could not connect to ftp server\n");
												FtpQuit(ftp_con);
											}else{
												std::string dirpath = item.networkentries[n].path + item.networkentries[n].name;
												item.networklastpath = dirpath;
												item.networkentries = FtpDirList(dirpath.c_str(), ftp_con);
												std::sort(item.networkentries.begin(),item.networkentries.end(),Utility::compare);
											}
										}
									}
								}else{
											
									urlschema thisurl = Utility::parseUrl(item.networkurl);
									if(thisurl.scheme == "http" || thisurl.scheme == "https"){
										std::string openurl = httpdir->getUrl() + httpdir->getCurrentRelPath() + item.networkentries[n].path;
										const char *cmd[] = {"loadfile", openurl.c_str(), NULL};
										mpv_command_async(libmpv->getHandle(), 0, cmd);
									}
									if(thisurl.scheme == "ftp"){
										std::string openurl = thisurl.scheme + std::string("://") + thisurl.user + std::string(":") + thisurl.pass + std::string("@") + thisurl.server + std::string("/") + item.networkentries[n].path + item.networkentries[n].name;
										const char *cmd[] = {"loadfile", openurl.c_str(), NULL};
										mpv_command_async(libmpv->getHandle(), 0, cmd);
									}
								}
							}
							
							if(!item.networkentries[n].isDir && thisurl.scheme != "http"){
								ImGui::SameLine(total_w-150);
								ImGui::Text("%s",Utility::humanSize(item.networkentries[n].size).c_str());
							}
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