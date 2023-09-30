#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"

#include "nxmp-i18n.h"

namespace Windows {
	
	int netwinselected = -1;
	
	CFileBrowser * sharecheck = nullptr;
	

	void ShareAddWindow(bool *focus, bool *first_item){
		Windows::SetupWindow();
		if (ImGui::Begin(Network_STR[NXNET_ADDSHARE], nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_MenuBar)) {
           
		   
			if (ImGui::BeginMenuBar()) {
				ImGui::Text(Network_STR[NXNET_ADDSHAREMENU]);
				ImGui::EndMenuBar();
			}
			ImGui::SeparatorText(Network_STR[NXNET_SHARECONFIG]);
			NewNetworkShare->name = InputSwitchKeyboard("##sharename","Share Name:",NewNetworkShare->name);
			
			ImGui::Text(Network_STR[NXNET_PROTOCOL]);
			ImGui::SameLine();
			static const char* item_current = NULL;
			if (ImGui::BeginCombo("##protocombo", item_current, 0)) // The second parameter is the label previewed before opening the combo.
			{
				for (int n = 0; n < IM_ARRAYSIZE(NewNetworkShare->protonames); n++)
				{
					bool is_selected = (item_current == NewNetworkShare->protonames[n]);
					
					if(std::string(NewNetworkShare->protonames[n]) == "FTP"){
						GUI::NXMPImage((void*)(intptr_t)imgloader->icons.FTPTexture.id, ImVec2(30,30));
					}
					if(std::string(NewNetworkShare->protonames[n]) == "HTTP"){
						GUI::NXMPImage((void*)(intptr_t)imgloader->icons.HTTPTexture.id, ImVec2(30,30));
					}
					if(std::string(NewNetworkShare->protonames[n]) == "SMB"){
						GUI::NXMPImage((void*)(intptr_t)imgloader->icons.NFSTexture.id, ImVec2(30,30));
					}
					if(std::string(NewNetworkShare->protonames[n]) == "NFS"){
						GUI::NXMPImage((void*)(intptr_t)imgloader->icons.FTPTexture.id, ImVec2(30,30));
					}
					if(std::string(NewNetworkShare->protonames[n]) == "SSH"){
						GUI::NXMPImage((void*)(intptr_t)imgloader->icons.SFTPTexture.id, ImVec2(30,30));
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
				ImGui::SeparatorText(Network_STR[NXNET_CREDENTIALS]);
				NewNetworkShare->address = InputSwitchKeyboard("##address",Network_STR[NXNET_ADDRESS],NewNetworkShare->address);
				if(NewNetworkShare->type ==3 || NewNetworkShare->type ==4){
					
				}else{
					NewNetworkShare->port = std::stoi(InputSwitchKeyboard("##port",Network_STR[NXNET_PORT],std::to_string(NewNetworkShare->port)));
				}
				ImGui::Checkbox("Anonymous", &NewNetworkShare->anon);
				if(!NewNetworkShare->anon){
					NewNetworkShare->username = InputSwitchKeyboard("##username",Network_STR[NXNET_USERNAME],NewNetworkShare->username);
					NewNetworkShare->password = InputSwitchKeyboard("##password",Network_STR[NXNET_PASSWORD],NewNetworkShare->password);
			
				}
				ImGui::SeparatorText("Path");
				NewNetworkShare->remoteshare = InputSwitchKeyboard("##remoteshare",Network_STR[NXNET_REMOTEPATH],NewNetworkShare->remoteshare);
				if(NewNetworkShare->type == 3){
					ImGui::Text(Network_STR[NXNET_REMOTESHAREDESC]);
				}else if(NewNetworkShare->type == 4){
					ImGui::Text("Remote export on server");
				}else{
					ImGui::Text(Network_STR[NXNET_REMOTEPATHDESC]);
				}
				
			}
			
			if(!NewNetworkShare->isValidShare()){
				 ImGui::BeginDisabled();
			}
			if (ImGui::Button(Network_STR[NXNET_TESTCONNECTION])){
				if(sharecheck == nullptr){
					sharecheck = new CFileBrowser(NewNetworkShare->GenConfigLine().url,NULL,NULL);
					sharecheck->DirList(sharecheck->getBasePath(),configini->getshowHidden(false),Utility::getMediaExtensions());
				}else{
					delete sharecheck;
					sharecheck = new CFileBrowser(NewNetworkShare->GenConfigLine().url,NULL,NULL);
					sharecheck->DirList(sharecheck->getBasePath(),configini->getshowHidden(false),Utility::getMediaExtensions());
				}
			}
			if(!NewNetworkShare->isValidShare()){
				 ImGui::EndDisabled();
			}
			if(sharecheck != nullptr){
				if(sharecheck->connected){
					ImGui::TextColored(ImVec4(0.0f,1.0f,0.0f,1.0f),Network_STR[NXNET_TESTOK]);
				}else{
					ImGui::TextColored(ImVec4(1.0f,0.0f,0.0f,1.0f),Network_STR[NXNET_TESTERROR]);
					ImGui::Text(sharecheck->errormsg.c_str());
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
		
		if (ImGui::Button(Common_STR[NXCOMMON_SAVE])){
			configini->addNetworkShare(NewNetworkShare->GenConfigLine());
			item.state = MENU_STATE_NETWORKBROWSER;
		}
		if(!NewNetworkShare->isValidShare()){
			 ImGui::EndDisabled();
		}
		ImGui::PopStyleColor(3);
		ImGui::SameLine(0,50);
		if (ImGui::Button(Common_STR[NXCOMMON_CANCEL])){
				
			item.state = MENU_STATE_NETWORKBROWSER;
		}
			
			
		}
		Windows::ExitWindow();
	}

	
    void NetworkWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		
        if (ImGui::Begin(Network_STR[NXNET_NETWORKSOURCESEL], nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_MenuBar)) {
           
			GUI::cloktimeText(ImVec2((1180.0f*multiplyRes)-ImGui::CalcTextSize(nxmpstats->currentTime).x-(10.0*multiplyRes),2.0f*multiplyRes),true,nxmpstats->currentTime);
			GUI::newbatteryIcon(ImVec2(1180.0f*multiplyRes,2.0f*multiplyRes),true,batteryPercent,40,20,true);
			
			float total_w = ImGui::GetContentRegionAvail().x;
			float total_h = ImGui::GetContentRegionAvail().y;
			
			if(item.networkselect){
				ImGui::BeginChild("##tablecontainer",ImVec2(total_w,total_h-30*multiplyRes));
				if (ImGui::BeginTable(Network_STR[NXNET_NETWORKSOURCEMENU],1)){
					ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthFixed, (1280.0f*multiplyRes-2 * ImGui::GetStyle().ItemSpacing.x)*multiplyRes); // Default to 100.0f
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					GUI::NXMPImage((void*)(intptr_t)imgloader->icons.ShareAddTexture.id, ImVec2(50,50));
					ImGui::SameLine();
					ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (50 - ImGui::GetFont()->FontSize) / 2});
					static int selected = -1;
					if (ImGui::Selectable(Network_STR[NXNET_ADDSHARE], selected == 0)){
						NewNetworkShare = new CNetworkShare();
						item.state = MENU_STATE_ADDSHARE;
					}	
					if(ImGui::IsItemHovered()){
						netwinselected = -1;
					}
					
					for(unsigned int n=0;n<item.networksources.size();n++){
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						urlschema thisurl = Utility::parseUrl(item.networksources[n].url);
						if(thisurl.scheme == "ftp"){
							GUI::NXMPImage((void*)(intptr_t)imgloader->icons.FTPTexture.id, ImVec2(50,50));
						}
						if(thisurl.scheme == "http"){
							GUI::NXMPImage((void*)(intptr_t)imgloader->icons.HTTPTexture.id, ImVec2(50,50));
						}
						if(thisurl.scheme == "sftp"){
							GUI::NXMPImage((void*)(intptr_t)imgloader->icons.SFTPTexture.id, ImVec2(50,50));
						}
						if(thisurl.scheme == "smb"){
							GUI::NXMPImage((void*)(intptr_t)imgloader->icons.SMBTexture.id, ImVec2(50,50));
						}
						if(thisurl.scheme == "nfs"){
							GUI::NXMPImage((void*)(intptr_t)imgloader->icons.NFSTexture.id, ImVec2(50,50));
						}
						ImGui::SameLine();
						ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (50*multiplyRes - ImGui::GetFont()->FontSize) / 2});
						
						if (ImGui::Selectable(std::string(item.networksources[n].name + " @ " +thisurl.server).c_str(), selected == n+1)){
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
								
					if (*first_item) {
                        ImGui::SetFocusID(ImGui::GetID((item.networksources[0].name.c_str())), ImGui::GetCurrentWindow());
                        *first_item = false;
                    }
					ImGui::EndTable();
					ImGui::SetWindowFocus();
				}
				ImGui::EndChild();
				ImGui::BeginChild("##helpchild",ImVec2(total_w,total_h-30-2*ImGui::GetStyle().FramePadding.y*multiplyRes));
				GUI::NXMPImage((void*)(intptr_t)imgloader->icons.GUI_D_UP.id, ImVec2(30,30));
				ImGui::SameLine();
				GUI::NXMPImage((void*)(intptr_t)imgloader->icons.GUI_D_DOWN.id, ImVec2(30,30));
				ImGui::SameLine();
				ImGui::AlignTextToFramePadding();
				ImGui::Text(Common_STR[NXCOMMON_NAVIGATION]);
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
				GUI::NXMPImage((void*)(intptr_t)imgloader->icons.GUI_A_BUT.id, ImVec2(30,30));
				ImGui::SameLine();
				ImGui::AlignTextToFramePadding();
				ImGui::Text(Common_STR[NXCOMMON_SELECT]);
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
				GUI::NXMPImage((void*)(intptr_t)imgloader->icons.GUI_Y_BUT.id, ImVec2(30,30));
				ImGui::SameLine();
				ImGui::AlignTextToFramePadding();
				ImGui::Text(Common_STR[NXCOMMON_HOME]);
				
				
				ImGui::EndChild();
			}
		}
		Windows::ExitWindow();
	}
}
