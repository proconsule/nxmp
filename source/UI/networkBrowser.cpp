#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"

#include "nxmp-i18n.h"


#include "smb2fs.h"
#include "sshfs.h"
#include "nfsfs.h"
#include "ftpfs.h"

namespace Windows {
	
	int netwinselected = -1;
	
	CFileBrowser * sharecheck = nullptr;
	
	bool urlcheck_isconnected = false;
	
	std::string newnetaddress = "";
	std::string newentryname = "New Share";
	
	void NewShareWindow(){
		Windows::SetupWindow();
		
		if (ImGui::Begin("Add Network Source", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
			/*
			if (!*focus) {
                ImGui::SetNextWindowFocus();
                *focus = true;
            }
			*/
			
		}
		
		float total_w = ImGui::GetContentRegionAvail().x;
		float total_h = ImGui::GetContentRegionAvail().y;
		float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
		ImGui::BeginChild("##shareaddmainview",ImVec2(total_w,total_h-45*multiplyRes),false,ImGuiWindowFlags_NoScrollbar);
		
		ImGui::PushStyleColor(ImGuiCol_ChildBg,OptsTab_Bg_color);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, HeaderHover_color);
		ImGui::PushStyleColor(ImGuiCol_NavHighlight, Active_color);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(40.0f,0));
		
		ImGui::PushStyleColor(ImGuiCol_Button,Button_color);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,ButtonActive_color);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered,NavHover_color);
		
		ImGui::Separator();
		newentryname = NativeInputSwitchKeyboard("##netaddentry","Name:",newentryname);
		ImGui::Separator();
		
		newnetaddress = NativeInputSwitchKeyboard("##netaddredd","Enter Net URL:",newnetaddress);
		
		
		if(!newnetaddress.empty()){
		
			urlschema thisurl = Utility::parseUrl(newnetaddress);
			
			ImGui::Separator();
			float currentposy = ImGui::GetCursorPosY();
			ImVec2 textsize = ImGui::CalcTextSize("A");
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()+(50.0f*multiplyRes-textsize.y)/2.0);
			ImGui::Text("Network Type:");
			//textsize = ImGui::CalcTextSize(thisurl);
			ImGui::SameLine(1280.0f*multiplyRes-50-2*ImGui::GetStyle().ItemSpacing.x);
			//ImGui::PushStyleColor(ImGuiCol_Text, nxmpgfx::Active_color);
			//ImGui::Text(curr_subfontsizestr.c_str());
			//ImGui::PopStyleColor();
			ImGui::SetCursorPosY(currentposy);
			if(Utility::startWith(newnetaddress,"smb",false)){
				GUI::NXMPImage((void*)(intptr_t)imgloader->icons.SMBTexture.id, ImVec2(50,50));
				
			}else if(Utility::startWith(newnetaddress,"sftp",false)){
				GUI::NXMPImage((void*)(intptr_t)imgloader->icons.SFTPTexture.id, ImVec2(50,50));
				
			}else if(Utility::startWith(newnetaddress,"ftp",false)){
				GUI::NXMPImage((void*)(intptr_t)imgloader->icons.FTPTexture.id, ImVec2(50,50));
						
			}else if(Utility::startWith(newnetaddress,"http",false)){
				GUI::NXMPImage((void*)(intptr_t)imgloader->icons.HTTPTexture.id, ImVec2(50,50));
				
			}else if(Utility::startWith(newnetaddress,"nfs",false)){
				GUI::NXMPImage((void*)(intptr_t)imgloader->icons.NFSTexture.id, ImVec2(50,50));
				
			}else{
				ImGui::Dummy(ImVec2(50.0f,50.0f));
			}
			
			ImGui::Separator();
			currentposy = ImGui::GetCursorPosY();
			textsize = ImGui::CalcTextSize("A");
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()+(50.0f*multiplyRes-textsize.y)/2.0);
			ImGui::Text("Host:");
			//textsize = ImGui::CalcTextSize(thisurl);
			textsize = ImGui::CalcTextSize(thisurl.server.c_str());
			ImGui::SameLine(1280.0f*multiplyRes-textsize.x-2*ImGui::GetStyle().ItemSpacing.x);
			//ImGui::PushStyleColor(ImGuiCol_Text, nxmpgfx::Active_color);
			//ImGui::Text(curr_subfontsizestr.c_str());
			//ImGui::PopStyleColor();
			ImGui::SetCursorPosY(currentposy+(50.0f*multiplyRes-textsize.y)/2.0);
			ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			ImGui::Text(thisurl.server.c_str());
			ImGui::PopStyleColor();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()+(50.0f*multiplyRes-textsize.y)/2.0);
			
			ImGui::Separator();
			currentposy = ImGui::GetCursorPosY();
			textsize = ImGui::CalcTextSize("A");
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()+(50.0f*multiplyRes-textsize.y)/2.0);
			ImGui::Text("Port:");
			//textsize = ImGui::CalcTextSize(thisurl);
			std::string portstr = thisurl.port.empty() ? "Protocol Default":thisurl.port;
			textsize = ImGui::CalcTextSize(portstr.c_str());
			ImGui::SameLine(1280.0f*multiplyRes-textsize.x-2*ImGui::GetStyle().ItemSpacing.x);
			//ImGui::PushStyleColor(ImGuiCol_Text, nxmpgfx::Active_color);
			//ImGui::Text(curr_subfontsizestr.c_str());
			//ImGui::PopStyleColor();
			ImGui::SetCursorPosY(currentposy+(50.0f*multiplyRes-textsize.y)/2.0);
			ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			ImGui::Text(portstr.c_str());
			ImGui::PopStyleColor();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()+(50.0f*multiplyRes-textsize.y)/2.0);
			
			
			ImGui::Separator();
			currentposy = ImGui::GetCursorPosY();
			textsize = ImGui::CalcTextSize("A");
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()+(50.0f*multiplyRes-textsize.y)/2.0);
			ImGui::Text("Username:");
			//textsize = ImGui::CalcTextSize(thisurl);
			textsize = ImGui::CalcTextSize(thisurl.user.c_str());
			ImGui::SameLine(1280.0f*multiplyRes-textsize.x-2*ImGui::GetStyle().ItemSpacing.x);
			//ImGui::PushStyleColor(ImGuiCol_Text, nxmpgfx::Active_color);
			//ImGui::Text(curr_subfontsizestr.c_str());
			//ImGui::PopStyleColor();
			ImGui::SetCursorPosY(currentposy+(50.0f*multiplyRes-textsize.y)/2.0);
			ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			ImGui::Text(thisurl.user.c_str());
			ImGui::PopStyleColor();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()+(50.0f*multiplyRes-textsize.y)/2.0);
			
			
			ImGui::Separator();
			currentposy = ImGui::GetCursorPosY();
			textsize = ImGui::CalcTextSize("A");
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()+(50.0f*multiplyRes-textsize.y)/2.0);
			ImGui::Text("Password:");
			//textsize = ImGui::CalcTextSize(thisurl);
			textsize = ImGui::CalcTextSize(thisurl.pass.c_str());
			ImGui::SameLine(1280.0f*multiplyRes-textsize.x-2*ImGui::GetStyle().ItemSpacing.x);
			//ImGui::PushStyleColor(ImGuiCol_Text, nxmpgfx::Active_color);
			//ImGui::Text(curr_subfontsizestr.c_str());
			//ImGui::PopStyleColor();
			ImGui::SetCursorPosY(currentposy+(50.0f*multiplyRes-textsize.y)/2.0);
			ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			ImGui::Text(thisurl.pass.c_str());
			ImGui::PopStyleColor();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()+(50.0f*multiplyRes-textsize.y)/2.0);
			
			ImGui::Separator();
			currentposy = ImGui::GetCursorPosY();
			textsize = ImGui::CalcTextSize("A");
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()+(50.0f*multiplyRes-textsize.y)/2.0);
			
			if(Utility::startWith(newnetaddress,"smb",false)){
				ImGui::Text("Share:");
			}else{
				ImGui::Text("Remote Path:");
			}
			//textsize = ImGui::CalcTextSize(thisurl);
			textsize = ImGui::CalcTextSize(thisurl.path.c_str());
			ImGui::SameLine(1280.0f*multiplyRes-textsize.x-2*ImGui::GetStyle().ItemSpacing.x);
			//ImGui::PushStyleColor(ImGuiCol_Text, nxmpgfx::Active_color);
			//ImGui::Text(curr_subfontsizestr.c_str());
			//ImGui::PopStyleColor();
			ImGui::SetCursorPosY(currentposy+(50.0f*multiplyRes-textsize.y)/2.0);
			ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			ImGui::Text(thisurl.path.c_str());
			ImGui::PopStyleColor();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()+(50.0f*multiplyRes-textsize.y)/2.0);
			
			
			
			ImGui::Separator();
			
			currentposy = ImGui::GetCursorPosY();
			
			//ImGui::SetCursorPosY(292.0f*multiplyRes-70.0f);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()+10.0f*multiplyRes);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX()+10.0f*multiplyRes);
			ImVec2 button_size(total_w-10.0*multiplyRes-2*ImGui::GetStyle().ItemSpacing.x, 60.0f);
				
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize,1.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, HeaderHover_color);	
				
			
			
			if (ImGui::Button("Test Connection", button_size))
			{
					if(Utility::startWith(newnetaddress,"smb",false)){
						CSMB2FS *testcon = new CSMB2FS(newnetaddress,"smb0","smb0:");
						urlcheck_isconnected = testcon->CheckConnection();
						delete testcon;
						
					}else if(Utility::startWith(newnetaddress,"sftp",false)){
						CSSHFS *testcon = new CSSHFS(newnetaddress,"ssh0","ssh0:");
						urlcheck_isconnected = testcon->CheckConnection();
						delete testcon;
						
					}else if(Utility::startWith(newnetaddress,"ftp",false)){
						CFTPFS *testcon = new CFTPFS(newnetaddress,"ftp0","ftp0:");
						urlcheck_isconnected = testcon->CheckConnection();
						delete testcon;		
					}else if(Utility::startWith(newnetaddress,"nfs",false)){
						CNFSFS *testcon = new CNFSFS(newnetaddress,"nfs0","nfs0:");
						urlcheck_isconnected = testcon->CheckConnection();
						delete testcon;
					}else{
						ImGui::Dummy(ImVec2(50.0f,50.0f));
					}
			}
			
			
			
			
			ImGui::PopStyleColor(2);
			ImGui::PopStyleVar();
			
			
			ImGui::Separator();
			std::string connectstr = urlcheck_isconnected ? "Connected" : "Not Connected";
			currentposy = ImGui::GetCursorPosY();
			textsize = ImGui::CalcTextSize("A");
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()+(50.0f*multiplyRes-textsize.y)/2.0);
			ImGui::Text("Connection Test Status:");
			//textsize = ImGui::CalcTextSize(thisurl);
			textsize = ImGui::CalcTextSize(connectstr.c_str());
			ImGui::SameLine(1280.0f*multiplyRes-textsize.x-2*ImGui::GetStyle().ItemSpacing.x);
			//ImGui::PushStyleColor(ImGuiCol_Text, nxmpgfx::Active_color);
			//ImGui::Text(curr_subfontsizestr.c_str());
			//ImGui::PopStyleColor();
			ImGui::SetCursorPosY(currentposy+(50.0f*multiplyRes-textsize.y)/2.0);
			if(urlcheck_isconnected){
				ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			}else{
				ImGui::PushStyleColor(ImGuiCol_Text, Disabled_color );
			}
			ImGui::Text(connectstr.c_str());
			ImGui::PopStyleColor();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()+(50.0f*multiplyRes-textsize.y)/2.0);
		
			ImGui::Separator();
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize,1.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, HeaderHover_color);	
			
			
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()+10.0f*multiplyRes);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX()+10.0f*multiplyRes);
			if (ImGui::Button("Save Connection", button_size))
			{
				oldnetworkSource tmpsource;
				tmpsource.name = newentryname;
				tmpsource.url = newnetaddress;
				configini->addNetworkShare(tmpsource);
				item.networksources = configini->getNetworks();
				item.state = MENU_STATE_NETWORKBROWSER;
				
			}
			ImGui::PopStyleColor(2);
			ImGui::PopStyleVar();
			ImGui::Separator();
		}
		
		
		
		ImGui::PopStyleColor(6);
		ImGui::PopStyleVar();
		ImGui::EndChild();
		
		
		ImGui::BeginChild("##helpchild",ImVec2(total_w,ImGui::GetContentRegionAvail().y));
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		ImGui::Dummy(ImVec2(0,5));
		ImVec2 startpos =  ImGui::GetCursorScreenPos();
		ImGui::Dummy(ImVec2(0,5));
		ImGui::Text(FONT_DPADUP_BUTTON_FILLED);
		ImGui::SameLine();
		ImGui::Text(FONT_DPADDOWN_BUTTON_FILLED);
		ImGui::SameLine();
		ImGui::Text(nxlangs::get_common_str(nxlangs::NXCOMMON_NAVIGATION).c_str());
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
		ImGui::Text(FONT_A_BUTTON_FILLED);
		ImGui::SameLine();
		ImGui::Text(nxlangs::get_common_str(nxlangs::NXCOMMON_SELECT).c_str());
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
		ImGui::Text(FONT_B_BUTTON_FILLED);
		ImGui::SameLine();
		ImGui::Text(nxlangs::get_common_str(nxlangs::NXCOMMON_CANCEL).c_str());
		
		window->DrawList->AddLine(startpos,ImVec2(startpos.x+1280*multiplyRes,startpos.y) , ImGui::GetColorU32(Text_color), 1.0f);
		
		ImGui::EndChild();
		
		Windows::ExitWindow();
		
	}

	void ShareAddWindow(bool *focus, bool *first_item){
		Windows::SetupWindow();
		if (ImGui::Begin(nxlangs::get_network_str(nxlangs::NXNET_ADDSHARE).c_str(), nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_MenuBar)) {
           
		   
			if (ImGui::BeginMenuBar()) {
				ImGui::Text(nxlangs::get_network_str(nxlangs::NXNET_ADDSHAREMENU).c_str());
				ImGui::EndMenuBar();
			}
			ImGui::SeparatorText(nxlangs::get_network_str(nxlangs::NXNET_SHARECONFIG).c_str());
			NewNetworkShare->name = InputSwitchKeyboard("##sharename","Share Name:",NewNetworkShare->name);
			
			ImGui::Text(nxlangs::get_network_str(nxlangs::NXNET_PROTOCOL).c_str());
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
				ImGui::SeparatorText(nxlangs::get_network_str(nxlangs::NXNET_CREDENTIALS).c_str());
				NewNetworkShare->address = InputSwitchKeyboard("##address",nxlangs::get_network_str(nxlangs::NXNET_ADDRESS).c_str(),NewNetworkShare->address);
				if(NewNetworkShare->type ==3 || NewNetworkShare->type ==4){
					
				}else{
					NewNetworkShare->port = std::stoi(InputSwitchKeyboard("##port",nxlangs::get_network_str(nxlangs::NXNET_PORT).c_str(),std::to_string(NewNetworkShare->port)));
				}
				ImGui::Checkbox("Anonymous", &NewNetworkShare->anon);
				if(!NewNetworkShare->anon){
					NewNetworkShare->username = InputSwitchKeyboard("##username",nxlangs::get_network_str(nxlangs::NXNET_USERNAME).c_str(),NewNetworkShare->username);
					NewNetworkShare->password = InputSwitchKeyboard("##password",nxlangs::get_network_str(nxlangs::NXNET_PASSWORD).c_str(),NewNetworkShare->password);
			
				}
				ImGui::SeparatorText("Path");
				NewNetworkShare->remoteshare = InputSwitchKeyboard("##remoteshare",nxlangs::get_network_str(nxlangs::NXNET_REMOTEPATH).c_str(),NewNetworkShare->remoteshare);
				if(NewNetworkShare->type == 3){
					ImGui::Text(nxlangs::get_network_str(nxlangs::NXNET_REMOTESHAREDESC).c_str());
				}else if(NewNetworkShare->type == 4){
					ImGui::Text("Remote export on server");
				}else{
					ImGui::Text(nxlangs::get_network_str(nxlangs::NXNET_REMOTEPATHDESC).c_str());
				}
				
			}
			
			if(!NewNetworkShare->isValidShare()){
				 ImGui::BeginDisabled();
			}
			if (ImGui::Button(nxlangs::get_network_str(nxlangs::NXNET_TESTCONNECTION).c_str())){
				if(sharecheck == nullptr){
					sharecheck = new CFileBrowser(NewNetworkShare->GenConfigLine().url,nullptr);
					sharecheck->DirList(sharecheck->getBasePath(),configini->getshowHidden(false),Utility::getMediaExtensions());
				}else{
					delete sharecheck;
					sharecheck = new CFileBrowser(NewNetworkShare->GenConfigLine().url,nullptr);
					sharecheck->DirList(sharecheck->getBasePath(),configini->getshowHidden(false),Utility::getMediaExtensions());
				}
			}
			if(!NewNetworkShare->isValidShare()){
				 ImGui::EndDisabled();
			}
			if(sharecheck != nullptr){
				if(sharecheck->connected){
					ImGui::TextColored(ImVec4(0.0f,1.0f,0.0f,1.0f),nxlangs::get_network_str(nxlangs::NXNET_TESTOK).c_str());
				}else{
					ImGui::TextColored(ImVec4(1.0f,0.0f,0.0f,1.0f),nxlangs::get_network_str(nxlangs::NXNET_TESTERROR).c_str());
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
		
		if (ImGui::Button(nxlangs::get_common_str(nxlangs::NXCOMMON_SAVE).c_str())){
			configini->addNetworkShare(NewNetworkShare->GenConfigLine());
			item.state = MENU_STATE_NETWORKBROWSER;
		}
		if(!NewNetworkShare->isValidShare()){
			 ImGui::EndDisabled();
		}
		ImGui::PopStyleColor(3);
		ImGui::SameLine(0,50);
		if (ImGui::Button(nxlangs::get_common_str(nxlangs::NXCOMMON_CANCEL).c_str())){
				
			item.state = MENU_STATE_NETWORKBROWSER;
		}
			
			
		}
		Windows::ExitWindow();
	}

	
    void NetworkWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		
        if (ImGui::Begin(nxlangs::get_network_str(nxlangs::NXNET_NETWORKSOURCESEL).c_str(), nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_MenuBar)) {
           
			GUI::cloktimeText(ImVec2((1180.0f*multiplyRes)-ImGui::CalcTextSize(nxmpstats->currentTime).x-(10.0*multiplyRes),2.0f*multiplyRes),true,nxmpstats->currentTime);
			GUI::newbatteryIcon(ImVec2(1180.0f*multiplyRes,2.0f*multiplyRes),true,batteryPercent,40,20,true);
			
			float total_w = ImGui::GetContentRegionAvail().x;
			float total_h = ImGui::GetContentRegionAvail().y;
			
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, NavHover_color);
			ImGui::PushStyleColor(ImGuiCol_NavHighlight, Active_color);
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {0, 5});
			
			
		
			if(item.networkselect){
				ImGui::BeginChild("##tablecontainer",ImVec2(total_w,total_h-45*multiplyRes));
				ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns;
				if (ImGui::BeginTable(nxlangs::get_network_str(nxlangs::NXNET_NETWORKSOURCEMENU).c_str(),2)){
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, ImGui::GetStyle().CellPadding.y * 2)); // Fix
    				
					ImGui::TableSetupColumn("icon", ImGuiTableColumnFlags_WidthFixed, ((80.0f+ImGui::GetStyle().FramePadding.x) *multiplyRes -2 * ImGui::GetStyle().ItemSpacing.x)); 
					ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthStretch); 
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					
					GUI::NXMPImage((void*)(intptr_t)imgloader->icons.ShareAddTexture.id, ImVec2(50,50));
					ImGui::SameLine();
					//ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (50 - ImGui::GetFont()->FontSize) / 2});
					static int selected = -1;
					if (ImGui::Selectable("##addsharesel", selected == 0,selectable_flags,ImVec2(1280*multiplyRes,50*multiplyRes))){
						NewNetworkShare = new CNetworkShare();
						item.state = MENU_STATE_ADDSHARE;
					}
					ImGui::TableSetColumnIndex(1);
					ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + ((50*multiplyRes) - ImGui::GetFont()->FontSize) / 2});
					ImGui::Text(nxlangs::get_network_str(nxlangs::NXNET_ADDSHARE).c_str());
					if(ImGui::IsItemHovered()){
						netwinselected = -1;
					}
					
					unsigned int n = 0;
					
					for(n=0;n<item.networksources.size();n++){
						
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
						//ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (50*multiplyRes - ImGui::GetFont()->FontSize) / 2});
						std::string itemid = "##itemid" + std::to_string(n);
						if (ImGui::Selectable(itemid.c_str(), selected == n+1,selectable_flags,ImVec2(1280*multiplyRes,50*multiplyRes))){
							item.first_item = true;		
							item.networkselect = false;
							
							if(thisurl.scheme == "http" || thisurl.scheme == "https"){
								filebrowser = new CFileBrowser(item.networksources[n].url,playlist);
								filebrowser->setSordOrder((FS::FILESORTORDER)configini->getsortOrder(false));
								filebrowser->DirList(filebrowser->getBasePath(),false,Utility::getMediaExtensions());
								item.state = MENU_STATE_HTTPBROWSER;
							}
							if(thisurl.scheme == "ftp"){
								filebrowser = new CFileBrowser(item.networksources[n].url,playlist);
								filebrowser->setSordOrder((FS::FILESORTORDER)configini->getsortOrder(false));
								filebrowser->DirList(filebrowser->getBasePath(),false,Utility::getMediaExtensions());	
								item.state = MENU_STATE_FTPBROWSER;
							}
							if(thisurl.scheme == "sftp"){
								filebrowser = new CFileBrowser(item.networksources[n].url,playlist);
								filebrowser->setSordOrder((FS::FILESORTORDER)configini->getsortOrder(false));
								filebrowser->DirList(filebrowser->getBasePath(),configini->getshowHidden(false),Utility::getMediaExtensions());	
								item.state = MENU_STATE_SSHBROWSER;
							}
							if(thisurl.scheme == "smb"){
								filebrowser = new CFileBrowser(item.networksources[n].url,playlist);
								filebrowser->setSordOrder((FS::FILESORTORDER)configini->getsortOrder(false));
								filebrowser->DirList(filebrowser->getBasePath(),configini->getshowHidden(false),Utility::getMediaExtensions());	
								item.state = MENU_STATE_SAMBABROWSER;
							}
							if(thisurl.scheme == "nfs"){
								filebrowser = new CFileBrowser(item.networksources[n].url,playlist);
								filebrowser->setSordOrder((FS::FILESORTORDER)configini->getsortOrder(false));
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
						ImGui::TableSetColumnIndex(1);
						ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + ((50*multiplyRes) - ImGui::GetFont()->FontSize) / 2});
						ImGui::Text(std::string(item.networksources[n].name + " @ " +thisurl.server).c_str());
						if(ImGui::IsItemHovered()){
								netwinselected = n;
						}
						
								
					}
					
					for(unsigned int z=0;z<configini->networks_v2.size();z++){
						
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						if(configini->networks_v2[z].type == "ftp"){
							GUI::NXMPImage((void*)(intptr_t)imgloader->icons.FTPTexture.id, ImVec2(50,50));
						}
						if(configini->networks_v2[z].type == "http"){
							GUI::NXMPImage((void*)(intptr_t)imgloader->icons.HTTPTexture.id, ImVec2(50,50));
						}
						if(configini->networks_v2[z].type == "sftp"){
							GUI::NXMPImage((void*)(intptr_t)imgloader->icons.SFTPTexture.id, ImVec2(50,50));
						}
						if(configini->networks_v2[z].type == "smb"){
							GUI::NXMPImage((void*)(intptr_t)imgloader->icons.SMBTexture.id, ImVec2(50,50));
						}
						if(configini->networks_v2[z].type == "nfs"){
							GUI::NXMPImage((void*)(intptr_t)imgloader->icons.NFSTexture.id, ImVec2(50,50));
						}
						ImGui::SameLine();
					
						std::string itemid = "##itemid" + std::to_string(z+n);
						if (ImGui::Selectable(itemid.c_str(), selected == z+n+1,selectable_flags,ImVec2(1280*multiplyRes,50*multiplyRes))){
							filebrowser = new CFileBrowser(configini->networks_v2[z],playlist);
							filebrowser->setSordOrder((FS::FILESORTORDER)configini->getsortOrder(false));
							filebrowser->DirList(filebrowser->getBasePath(),configini->getshowHidden(false),Utility::getMediaExtensions());	
							item.state = MENU_STATE_SAMBABROWSER;
						
						}
						
						if(ImGui::IsItemHovered()){
							netwinselected = z+n;
						}
						ImGui::TableSetColumnIndex(1);
						ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + ((50*multiplyRes) - ImGui::GetFont()->FontSize) / 2});
						ImGui::Text(std::string(configini->networks_v2[z].name + " @ " +configini->networks_v2[z].server).c_str());
						if(ImGui::IsItemHovered()){
								netwinselected = z+n;
						}
						
					}
					
					
					ImGui::PopStyleVar();			
					if (*first_item) {
						if(item.popupstate == POPUP_STATE_NONE){
							ImGui::SetFocusID(ImGui::GetID((item.networksources[0].name.c_str())), ImGui::GetCurrentWindow());
                        }
						*first_item = false;
                    }
					ImGui::EndTable();
					if(item.popupstate == POPUP_STATE_NONE){
						ImGui::SetWindowFocus();
					}
				}
				ImGui::EndChild();
				
				ImGui::BeginChild("##helpchild",ImVec2(total_w,ImGui::GetContentRegionAvail().y));
				ImGuiWindow* window = ImGui::GetCurrentWindow();
				
				ImGui::Dummy(ImVec2(0,5));
				ImVec2 startpos =  ImGui::GetCursorScreenPos();
				ImGui::Dummy(ImVec2(0,5));
				ImGui::Text(FONT_DPADUP_BUTTON_FILLED);
				ImGui::SameLine();
				ImGui::Text(FONT_DPADDOWN_BUTTON_FILLED);
				ImGui::SameLine();
				ImGui::Text(nxlangs::get_common_str(nxlangs::NXCOMMON_NAVIGATION).c_str());
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
				ImGui::Text(FONT_A_BUTTON_FILLED);
				ImGui::SameLine();
				ImGui::Text(nxlangs::get_common_str(nxlangs::NXCOMMON_SELECT).c_str());
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
				ImGui::Text(FONT_Y_BUTTON_FILLED);
				ImGui::SameLine();
				ImGui::Text(nxlangs::get_common_str(nxlangs::NXCOMMON_HOME).c_str());
				
				window->DrawList->AddLine(startpos,ImVec2(startpos.x+1280*multiplyRes,startpos.y) , ImGui::GetColorU32(ImVec4(1.0f,1.0f,1.0f,1.0f)), 1.0f);
				
				ImGui::EndChild();
			}
			ImGui::PopStyleColor(2);
			ImGui::PopStyleVar();
		}
		Windows::ExitWindow();
	}
}
