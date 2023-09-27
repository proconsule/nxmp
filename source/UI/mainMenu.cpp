#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"
#include "curldownloader.h"

namespace Windows {
	
	static std::vector<std::string> topmenu {"Local Files","USB","Stream Url","Network","UPNP","Enigma2","Playlist","Settings","Info","Exit"};
	
	
    void MainMenuWindow(bool *focus, bool *first_item) {
        Windows::SetupMainWindow();
		//std::vector<std::string> topmenu = configini->topmenu;
		
		
		
		GUI::cloktimeText(ImVec2((1180.0f*multiplyRes)-ImGui::CalcTextSize(nxmpstats->currentTime).x-(10.0*multiplyRes),5.0f*multiplyRes),true,nxmpstats->currentTime);
		GUI::newbatteryIcon(ImVec2(1180.0f*multiplyRes,5.0f*multiplyRes),true,batteryPercent,40*multiplyRes,20*multiplyRes,true);
		
		
		if (ImGui::Begin(nxmpTitle.c_str(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
			if (ImGui::BeginTable("##table1", 2)){
				ImGui::TableSetupColumn("icon", ImGuiTableColumnFlags_WidthFixed, ((80.0f+ImGui::GetStyle().FramePadding.x) *multiplyRes -2 * ImGui::GetStyle().ItemSpacing.x)); // Default to 100.0f
				ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthStretch); // Default to 200.0f
				
				static int selected = -1;					
				for (unsigned int n = 0; n < topmenu.size(); n++){
					ImGui::TableNextRow();
					std::string itemid = "##" + std::to_string(n);
					ImGui::TableSetColumnIndex(0);
					if(topmenu[n] == "Local Files"){
						GUI::NXMPImage((void*)(intptr_t)imgloader->icons.SdCardTexture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "USB"){
						GUI::NXMPImage((void*)(intptr_t)imgloader->icons.UsbTexture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "Network"){
						GUI::NXMPImage((void*)(intptr_t)imgloader->icons.NetworkTexture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "UPNP"){
						GUI::NXMPImage((void*)(intptr_t)imgloader->icons.UPNPTexture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "Enigma2"){
						GUI::NXMPImage((void*)(intptr_t)imgloader->icons.Enigma2Texture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "Playlist"){
						GUI::NXMPImage((void*)(intptr_t)imgloader->icons.PlaylistTexture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "Stream Url"){
						GUI::NXMPImage((void*)(intptr_t)imgloader->icons.NetworkTexture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "MTP"){
						GUI::NXMPImage((void*)(intptr_t)imgloader->icons.PlaylistTexture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "Info"){
						GUI::NXMPImage((void*)(intptr_t)imgloader->icons.InfoTexture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "Settings"){
						GUI::NXMPImage((void*)(intptr_t)imgloader->icons.SettingsTexture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "Exit"){
						GUI::NXMPImage((void*)(intptr_t)imgloader->icons.ExitTexture.id, ImVec2(50,50));
					}
							
															
					ImGui::TableSetColumnIndex(1);
					
					ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + ((50*multiplyRes) - ImGui::GetFont()->FontSize) / 2});
									
					if (ImGui::Selectable(itemid.c_str(), selected == n)){
						if(topmenu[n] == "Local Files"){
							item.state = MENU_STATE_FILEBROWSER;
							filebrowser = new CFileBrowser(configini->getStartPath(),playlist);
							filebrowser->DirList(configini->getStartPath(),true,Utility::getMediaExtensions());
							item.first_item = true;
						}
						if(topmenu[n] == "USB"){
							usbInit();
							if(MyUSBMount==nullptr){
								MyUSBMount=new USBMounter(playlist);
							}
							item.state = MENU_STATE_USB_MOUNT;
							filebrowser = new CFileBrowser("",playlist,MyUSBMount);
							
						}
						if(topmenu[n] == "Network"){
							item.networksources.clear();
							item.networksources = configini->getNetworks();
							item.state = MENU_STATE_NETWORKBROWSER;
							item.first_item = true;								
						}
						if(topmenu[n] == "UPNP"){
							nxupnp = new NXUPnP();
							nxupnp->Discovery();
							item.state = MENU_STATE_UPNPBROWSER;
							item.first_item = true;
						}
						if(topmenu[n] == "Enigma2"){
							enigma2 = new Enigma2(configini->getEnigma());
							item.first_item = true;
							if(configini->getEnigma() == ""){
								item.state = MENU_STATE_ENIGMABROWSER;
							}else{
								enigma2->getServices();
								item.state = MENU_STATE_ENIGMABROWSER;
							}
						}
						if(topmenu[n] == "Settings"){
							//configini->setLongSeek(configini->getLongSeek(false));
							//configini->setShortSeek(configini->getShortSeek(false));
							if(sqlitedb != nullptr)sqlitedb->UpdateDbStats();
							item.state = MENU_STATE_SETTINGS;
						}
						if(topmenu[n] == "Playlist"){
							item.state = MENU_STATE_PLAYLISTBROWSER;
							if(MediaProbe == nullptr){
								MediaProbe = new CMediaProbe(playlist);
								MediaProbe->StartMediaProbeThread();
							}
							
						}
						if(topmenu[n] == "MTP"){
							item.state = MENU_STATE_MTPSERVER;
							mtp = new CMTP();
							mtp->StartServer();
						}
						if(topmenu[n] == "Info"){
							item.state = MENU_STATE_INFO;
						}
						if(topmenu[n] == "Exit"){
							renderloopdone = true;
						}
						if(topmenu[n] == "Stream Url"){
					//i need move this, work in progress.
					    
						std::string namefile = "Streaming from Url...";
						std::string received = Utility::KeyboardCall ("Write the Url to start the streaming\n(Fembed, JKAnime, Bitly or Direct URL...)", tempKbUrl);
						
						tempKbUrl = received;
						curlDownloader Scraper;
                        if (!received.compare("debug"))
						{
						}

						if (received.find("https://jk/") != std::string::npos)
						{
						Utility::replace(received,"https://jk/","https://jkanime.net/");
						
						}
						if (received.find("https://bit.ly/") != std::string::npos)
						{//if the response from the address does not arrive within 5 seconds, it sends the direct link.
						received = Scraper.getRedirection(received,"",false,"",false);
						}

						if (received.find("https://jkanime.net/") != std::string::npos)
						{
						namefile = "Streaming from JKAnime...";
						received = Utility::Nozomi_Link(received);
						}

						if(received.find("http") != std::string::npos)
						{
						std::cout << "This: "<< received.c_str() << std::endl;
						libmpv->loadFileLive(received,namefile.c_str());
						}
					//i need move this, work in progress.
						}


					}
					ImGui::SameLine();
					//ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + ((50*multiplyRes) - ImGui::GetFont()->FontSize) / 2});
					
					ImGui::Text("%s",topmenu[n].c_str());
					
					
					
				}
				
				if (*first_item) {
					std::string itemid = "##" + std::to_string(0);
					ImGui::SetFocusID(ImGui::GetID(itemid.c_str()), ImGui::GetCurrentWindow());
					ImGuiContext& g = *ImGui::GetCurrentContext();
					g.NavDisableHighlight = false;
					g.NavDisableMouseHover = true;
					*first_item = false;
				}
				ImGui::EndTable();
			}	
		}
		
		Windows::ExitMainWindow();
		
	}
}
		