#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "windows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"


namespace Windows {
    void FileBrowserWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		static unsigned int item_current_idx = 0;
        std::vector<std::string> topmenu = {"Local Files","Network","Enigma2"};
		
        if (ImGui::Begin(nxmpTitle.c_str(), nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
            if (!*focus) {
                ImGui::SetNextWindowFocus();
                *focus = true;
            }
			switch (item.browserstate){
				case BROWSER_STATE_HOME:
					if (ImGui::BeginListBox("Browser Menu",ImVec2(1280.0f, 720.0f))){
						
						for (unsigned int n = 0; n < topmenu.size(); n++){
							const bool is_selected = (item_current_idx == n);
							if (ImGui::Selectable(topmenu[n].c_str(), is_selected)){
								if(n==0){
									item.browserstate = BROWSER_STATE_LOCAL;
									FS::GetDirList(item.localpath.c_str(),item.localfileentries);
								}
								if(n==1){
									item.networksources.clear();
									item.networksources = configini->getNetworks();
									item.browserstate = BROWSER_STATE_NETWORK;
									item.networkurl = "";
									item.networklastpath = "";
									
									
								}
								if(n==2){
									std::string e2uri = configini->getEnigma();
									urlschema e2schema = Utility::parseUrl(e2uri);
									enigma2->enigma2ip = e2schema.server;
									enigma2->getServices();
									item.browserstate = BROWSER_STATE_ENIGMA2;
								}
							}
							
						}
					
					}				
					ImGui::EndListBox();
					break;
				case BROWSER_STATE_LOCAL:
					if (ImGui::BeginListBox("File Browser Menu",ImVec2(1280.0f, 720.0f))){
						int total_w = ImGui::GetContentRegionAvail().x;
						ImGui::Text("current path: %s",item.localpath.c_str());
						for (unsigned int n = 0; n < item.localfileentries.size(); n++){
							const bool is_selected = (item_current_idx == n);
							if(item.localfileentries[n].type == FsDirEntryType_Dir || FS::GetFileType(item.localfileentries[n].name) != FileTypeNone){
								if(item.localfileentries[n].type == FsDirEntryType_Dir){
									ImGui::Image((void*)(intptr_t)FolderTexture.id, ImVec2(50,50));
								}else{
									ImGui::Image((void*)(intptr_t)FileTexture.id, ImVec2(FileTexture.width,FileTexture.height));
								}
								ImGui::SameLine();
								if (ImGui::Selectable(item.localfileentries[n].name, is_selected)){
									if(item.localfileentries[n].type == FsDirEntryType_Dir){
										item_current_idx = 0;
										if(item.localpath!="/"){
											item.localpath.append("/");
										}
										item.localpath.append(item.localfileentries[n].name);
										FS::GetDirList(item.localpath.c_str(),item.localfileentries);
									}
									else{
										std::string playfilepath = item.localpath;
										playfilepath.append("/");
										playfilepath.append(item.localfileentries[n].name);
										const char *cmd[] = {"loadfile", playfilepath.c_str(), NULL};
										mpv_command_async(mpv->getHandle(), 0, cmd);
									}
								}
								ImGui::SameLine(total_w-100);
								ImGui::Text("AAAA");
							}
						}
					}
					
					ImGui::EndListBox();
					//ImGui::Text("FILESYSTEM");
					
					break;
				case BROWSER_STATE_NETWORK:
					if(item.networkselect){
						if (ImGui::BeginListBox("Network Source Menu",ImVec2(1280.0f, 720.0f))){
							for(unsigned int n=0;n<item.networksources.size();n++){
								const bool is_selected = (item_current_idx == n);
								if (ImGui::Selectable(item.networksources[n].name.c_str(), is_selected)){
									item.networkselect = false;
									item.networkurl = item.networksources[n].url;
									urlschema thisurl = Utility::parseUrl(item.networksources[n].url);
									item.networklastpath = thisurl.path;
									if(thisurl.scheme == "ftp"){
										netbuf *ftp_con = nullptr;
										printf("FTP CONNECT %s\n",thisurl.server.c_str());
										std::string ftphost = thisurl.server+std::string(":21");
										if (!FtpConnect(ftphost.c_str(), &ftp_con)) {
											printf("could not connect to ftp server\n");
										}else{
											
											if (!FtpLogin(thisurl.user.c_str(), thisurl.pass.c_str(), ftp_con)) {
											printf("could not connect to ftp server\n");
												FtpQuit(ftp_con);
											}else{
												item.networkentries = FtpDirList(thisurl.path.c_str(), ftp_con);
											}
										}
									}
								}
								
							}
						}
						ImGui::EndListBox();
					}
					else{
						if (ImGui::BeginListBox("Network Browser Menu",ImVec2(1280.0f, 720.0f))){
							ImGui::Text("Current Dir: %s\n",item.networklastpath.c_str());
							for(unsigned int n=0;n<item.networkentries.size();n++){
								const bool is_selected = (item_current_idx == n);
								if (ImGui::Selectable(item.networkentries[n].name.c_str(), is_selected)){
									if(item.networkentries[n].isDir){
										urlschema thisurl = Utility::parseUrl(item.networkurl);
										if(thisurl.scheme == "ftp"){
											netbuf *ftp_con = nullptr;
											std::string ftphost = thisurl.server+std::string(":21");
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
												}
											}
										}
									}else{

										urlschema thisurl = Utility::parseUrl(item.networkurl);
										std::string openurl = thisurl.scheme + std::string("://") + thisurl.user + std::string(":") + thisurl.pass + std::string("@") + thisurl.server + std::string("/") + item.networkentries[n].path + item.networkentries[n].name;
										const char *cmd[] = {"loadfile", openurl.c_str(), NULL};
										mpv_command_async(mpv->getHandle(), 0, cmd);
									}
								}
							}
						
						}
						ImGui::EndListBox();
					}
					ImGui::Text("NETWORK");
					break;				
				case BROWSER_STATE_ENIGMA2:
					if (ImGui::BeginListBox("Enigma2 Browser Menu",ImVec2(1280.0f, 720.0f))){
						if(item.enigma2bouquet == ""){
							for (unsigned int n = 0; n < enigma2->e2services.size(); n++){
								const bool is_selected = (item_current_idx == n);
								if (ImGui::Selectable(enigma2->e2services[n].name.c_str(), is_selected)){
									item_current_idx = 0;
									enigma2->m3uParser((char *)enigma2->e2services[n].bouquetref.c_str());
									item.enigma2bouquet =  enigma2->e2services[n].bouquetref;
								}
							
							}
						}else if(item.enigma2bouquet != ""){
							for (unsigned int n = 0; n < enigma2->e2currbouqet.size(); n++){
								const bool is_selected = (item_current_idx == n);
								if (ImGui::Selectable(enigma2->e2currbouqet[n].name.c_str(), is_selected)){
									item_current_idx = 0;
									const char *cmd[] = {"loadfile", enigma2->e2currbouqet[n].url.c_str(), NULL};
									mpv_command_async(mpv->getHandle(), 0, cmd);
								}
							
							}
						}
					}
					ImGui::EndListBox();
					break;				
        
			}
            
        }
        
        Windows::ExitWindow();
    }
}