#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"
#include "curldownloader.h"

#include "custom_widgets.h"

namespace Windows {
    void MainMenuWindow(bool *focus, bool *first_item) {
        Windows::SetupMainWindow();
		std::vector<std::string> topmenu = configini->topmenu;
		/*
		if(isHandheld == true){
			batteryIcon(ImVec2(1230.0f,5.0f),true,batteryPercent,40,20);
		}
        else{
			batteryIcon(ImVec2(1230.0f*multiplyRes + 20,5.0f),true,batteryPercent,40,20);
		}
		*/
		
		GUI::newbatteryIcon(ImVec2(1180.0f*multiplyRes,5.0f),true,batteryPercent,40,20,true);
				
		
		if (ImGui::Begin(nxmpTitle.c_str(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
				static int selected = -1;					
				for (unsigned int n = 0; n < topmenu.size(); n++){
					std::string itemid = "##" + std::to_string(n);
					if(topmenu[n] == "Local Files"){
						GUI::NXMPImage((void*)(intptr_t)nxmpicons.SdCardTexture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "USB"){
						GUI::NXMPImage((void*)(intptr_t)nxmpicons.UsbTexture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "Network"){
						GUI::NXMPImage((void*)(intptr_t)nxmpicons.NetworkTexture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "UPNP"){
						GUI::NXMPImage((void*)(intptr_t)nxmpicons.UPNPTexture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "Enigma2"){
						GUI::NXMPImage((void*)(intptr_t)nxmpicons.Enigma2Texture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "Playlist"){
						GUI::NXMPImage((void*)(intptr_t)nxmpicons.PlaylistTexture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "Stream Url"){
						GUI::NXMPImage((void*)(intptr_t)nxmpicons.NetworkTexture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "MTP"){
						GUI::NXMPImage((void*)(intptr_t)nxmpicons.PlaylistTexture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "Info"){
						GUI::NXMPImage((void*)(intptr_t)nxmpicons.InfoTexture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "Settings"){
						GUI::NXMPImage((void*)(intptr_t)nxmpicons.SettingsTexture.id, ImVec2(50,50));
					}
					else if(topmenu[n] == "Exit"){
						GUI::NXMPImage((void*)(intptr_t)nxmpicons.ExitTexture.id, ImVec2(50,50));
					}
							
							
					ImGui::SameLine();
					ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + ((50*multiplyRes) - ImGui::GetFont()->FontSize) / 2});
								
					if (ImGui::Selectable(itemid.c_str(), selected == n)){
						if(topmenu[n] == "Local Files"){
							item.state = MENU_STATE_FILEBROWSER;
							localdir = new localFs(configini->getStartPath(),playlist);
							localdir->DirList(configini->getStartPath(),true,Utility::getMediaExtensions());
							item.first_item = true;
						}
						if(topmenu[n] == "USB"){
							usbInit();
							item.state = MENU_STATE_USB;
							if(usbmounter == nullptr){
								usbmounter = new USBMounter(playlist);
							}
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
							configini->setLongSeek(configini->getLongSeek(false));
							configini->setShortSeek(configini->getShortSeek(false));
							item.state = MENU_STATE_SETTINGS;
						}
						if(topmenu[n] == "Playlist"){
							item.state = MENU_STATE_PLAYLISTBROWSER;
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
                        
						if (received.find("https://bit.ly/") != std::string::npos)
						{//if the response from the address does not arrive within 5 seconds, it sends the direct link.
						received = Scraper.getRedirection(received,"",false,"",false);}

						if (received.find("https://jkanime.net/") != std::string::npos)
						{
						namefile = "Streaming from JKAnime...";
						received = Utility::Nozomi_Link(received);
						}

						if (received.find("https://www.fembed.com/v/") != std::string::npos || received.find("https://embedsito.com/v/") != std::string::npos || received.find("https://suzihaza.com/v/") != std::string::npos)
						{
						namefile = "Streaming from Fembed...";

						Utility::replace(received,"https://suzihaza.com/v/","https://www.fembed.com/api/source/");
						Utility::replace(received,"https://www.fembed.com/v/","https://www.fembed.com/api/source/");
						Utility::replace(received,"https://embedsito.com/v/","https://www.fembed.com/api/source/");
						std::string htmlcode = Scraper.scrapeHtml(received, "", true, "", false);
                       
						int tempValue1 = htmlcode.rfind("https:");
						int tempValue2 = htmlcode.find("\"",tempValue1);
						received = htmlcode.substr(tempValue1,tempValue2 - tempValue1);
						Utility::replace(received,"\\/","/");
						std::cout << received << std::endl;
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
				
		}
		
		Windows::ExitMainWindow();
		
	}
}
		