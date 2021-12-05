#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"


namespace Windows {
	
    void SettingsMenuWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		if (ImGui::Begin("Settings Window", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
            if (!*focus) {
                ImGui::SetNextWindowFocus();
                *focus = true;
            }
			
			float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
			
			if (ImGui::BeginTabBar("Settings Tab bar", 0))
            {
				if (ImGui::BeginTabItem("Generals")) {
					ImGui::Text("File Browser");
					ImGui::Separator();
					bool showhiddenbool = configini->getshowHidden(true);
					if(ImGui::Checkbox("Show Hidden Files", &showhiddenbool)){
						configini->setshowHidden(showhiddenbool);
						
					}
					ImGui::Dummy(ImVec2(0.0f,30.0f));
					ImGui::Text("Navigation");
					ImGui::Separator();
					bool touchbool = configini->getTouchEnable(true);
					if(ImGui::Checkbox("Enable Touch Controls", &touchbool)){
						configini->setTouchEnable(touchbool);
						if(touchbool){
							ImGuiIO &io = ImGui::GetIO();
							io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
						}else{
							ImGuiIO &io = ImGui::GetIO();
							io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
						}
					}
					
					
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Player Settings"))
                {
				ImGui::Text("Seek");
				ImGui::Separator();
				ImGui::Text("Short Seek Time");
				ImGui::SameLine(220,spacing);
				ImGui::PushButtonRepeat(true);
				if (ImGui::ArrowButton("##shortleft", ImGuiDir_Left)) {
					if(configini->getShortSeek(true)-1 >0){
						configini->setShortSeek(configini->getShortSeek(true)-1);
					}
				}
				ImGui::SameLine(0.0f, spacing);
				if (ImGui::ArrowButton("##shortright", ImGuiDir_Right)) { 
					configini->setShortSeek(configini->getShortSeek(true)+1);
				}
				ImGui::PopButtonRepeat();
				ImGui::SameLine();
				ImGui::Text("%d sec", configini->getShortSeek(true));
			
				ImGui::Text("Long Seek Time");
				ImGui::SameLine(220,spacing);
				ImGui::PushButtonRepeat(true);
				if (ImGui::ArrowButton("##longleft", ImGuiDir_Left)) {
					if(configini->getLongSeek(true)-1 >0){
						configini->setLongSeek(configini->getLongSeek(true)-1);
					}
				}
				ImGui::SameLine(0.0f, spacing);
				if (ImGui::ArrowButton("##longright", ImGuiDir_Right)) { 
					configini->setLongSeek(configini->getLongSeek(true)+1);
				}
				ImGui::PopButtonRepeat();
				ImGui::SameLine();
				ImGui::Text("%d sec", configini->getLongSeek(true));
				ImGui::Dummy(ImVec2(0.0f,30.0f));
				
				ImGui::Text("Language");
				ImGui::Separator();
				bool alangbool = configini->getUseAlang(true);
				if(ImGui::Checkbox("Use Auto Audio Language", &alangbool)){
					configini->setUseAlang(alangbool);
				}
				
				if(!alangbool){
					ImGui::BeginDisabled();
				}

				const char* combo_preview_value = Utility::getLanguages()[configini->getAlang(true)].lang3.c_str();  // Pass in the preview value visible before opening the combo (it could be anything)
				ImGui::PushItemWidth(300);
				if (ImGui::BeginCombo("Audio Language", combo_preview_value, 0))
				{	
					for (int n = 0; n < Utility::getLanguages().size(); n++)
					{
						const bool is_selected = (configini->getAlang(true) == n);
						if (ImGui::Selectable(Utility::getLanguages()[n].lang3.c_str(), is_selected))
							configini->setAlang(n);

						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				if(!alangbool){
					ImGui::EndDisabled();
				}
				ImGui::Dummy(ImVec2(0.0f,30.0f));
				ImGui::Text("Subtitle");
				ImGui::Separator();
				ImGui::Text("Sub Font Size");
				ImGui::SameLine(220,spacing);
				ImGui::PushButtonRepeat(true);
				if (ImGui::ArrowButton("##subsizeleft", ImGuiDir_Left)) {
					if(configini->getSubFontSize(true)-1 >0){
						configini->setSubFontSize(configini->getSubFontSize(true)-1);
					}
				}
				ImGui::SameLine(0.0f, spacing);
				if (ImGui::ArrowButton("##subsizeright", ImGuiDir_Right)) { 
					if(configini->getSubFontSize(true)+1 <120){
						configini->setSubFontSize(configini->getSubFontSize(true)+1);
					}
				}
				ImGui::PopButtonRepeat();
				ImGui::SameLine();
				ImGui::Text("%d", configini->getSubFontSize(true));
				ImGui::Dummy(ImVec2(0.0f,30.0f));
				ImGui::Text("Video");
				ImGui::Separator();
				std::vector<std::string> deintmenu = {"No","Yes","Auto"};
				const char* combo_deintpreview_value = deintmenu[configini->getDeinterlace(true)].c_str();
				ImGui::PushItemWidth(300);
				if (ImGui::BeginCombo("Deinterlace", combo_deintpreview_value, 0))
				{	
					for (int n = 0; n < deintmenu.size(); n++)
					{
						const bool is_selected = (configini->getDeinterlace(true) == n);
						if (ImGui::Selectable(deintmenu[n].c_str(), is_selected))
							configini->setDeinterlace(n);

						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				
				
				ImGui::PopItemWidth();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Database")) {
				bool usedbbool = configini->getDbActive(true);
				if(ImGui::Checkbox("Use Database", &usedbbool)){
					if(sqlitedb != nullptr){
						delete sqlitedb;
						sqlitedb = nullptr;
					}
					if(usedbbool){
						sqlitedb = new SQLiteDB("nxmp.db");
					}
					configini->setDbActive(usedbbool);	
					
				}
				if(sqlitedb != nullptr){
					if(sqlitedb->getCorrupted()){
						ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),"Database File Corrupted");
					}else{
						ImGui::Text("Database File Version: ");
						ImGui::SameLine();
						ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f),sqlitedb->getDbVersion().c_str());
					}
						ImGui::Text("SQLite Version: ");
						ImGui::SameLine();
						ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f),sqlitedb->getSQLiteVersion().c_str());
						
						
						ImGui::SetCursorPosY(ImGui::GetCursorPosY()+50);
						ImGui::Text("Start saving resume info @ ");
						ImGui::SameLine(300,spacing);
						ImGui::PushButtonRepeat(true);
						if (ImGui::ArrowButton("##resumestartleft", ImGuiDir_Left)) {
							if(configini->getResumeStartPerc(true)-1 >0){
								configini->setResumeStartPerc(configini->getResumeStartPerc(true)-1);
							}
						}
						ImGui::SameLine(0.0f, spacing);
						if (ImGui::ArrowButton("##resumestartright", ImGuiDir_Right)) { 
							configini->setResumeStartPerc(configini->getResumeStartPerc(true)+1);
						}
						ImGui::PopButtonRepeat();
						ImGui::SameLine();
						ImGui::Text("%d%%", configini->getResumeStartPerc(true));
						
						
						ImGui::Text("Stop saving resume info @ ");
						ImGui::SameLine(300,spacing);
						ImGui::PushButtonRepeat(true);
						if (ImGui::ArrowButton("##resumestopleft", ImGuiDir_Left)) {
							if(configini->getResumeStopPerc(true)-1 >0){
								configini->setResumeStopPerc(configini->getResumeStopPerc(true)-1);
							}
						}
						ImGui::SameLine(0.0f, spacing);
						if (ImGui::ArrowButton("##resumestopright", ImGuiDir_Right)) { 
							if(configini->getResumeStopPerc(true)+1 <=100){
								configini->setResumeStopPerc(configini->getResumeStopPerc(true)+1);
							}
						}
						ImGui::PopButtonRepeat();
						ImGui::SameLine();
						ImGui::Text("%d%%", configini->getResumeStopPerc(true));
						
						
						
				}else{
					ImGui::Text("Database not active");
				}
				
				
					
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Touch Settings")) {
				
				ImGui::Text("One Finger Swipe during playback");
				static int touchseekradio = configini->getPlayerSwipeSeek(true);
				if(ImGui::RadioButton("Short Seek", &touchseekradio, 0)){
					configini->setPlayerSwipeSeek(touchseekradio);
				}
				ImGui::SameLine();
				if(ImGui::RadioButton("Long Seek", &touchseekradio, 1)){
					configini->setPlayerSwipeSeek(touchseekradio);
				}
				ImGui::SameLine();
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
			
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x/2);
		ImGui::SetCursorPosY(ImGui::GetWindowSize().y -50);
			
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));
		if (ImGui::Button("Save")){
			item.popupstate = POPUP_STATE_SAVE_SETTINGS;
		}
		ImGui::PopStyleColor(3);
		ImGui::SameLine(0,50);
		if (ImGui::Button("Cancel")){
				configini->setLongSeek(configini->getLongSeek(false));
				configini->setShortSeek(configini->getShortSeek(false));
				configini->setAlang(configini->getAlang(false));
				configini->setSubFontSize(configini->getSubFontSize(false));
				configini->setSubFontColor(configini->getSubFontColor(false));
				configini->setDbActive(configini->getDeinterlace(false));
				
				
				configini->setshowHidden(configini->getshowHidden(false));
				configini->setTouchEnable(configini->getTouchEnable(false));
				configini->setPlayerSwipeSeek(configini->getPlayerSwipeSeek(false));
				if(configini->getTouchEnable(false)){
					ImGuiIO &io = ImGui::GetIO();
					io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
				}else{
					ImGuiIO &io = ImGui::GetIO();
					io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
				}
				
				
				item.state = MENU_STATE_HOME;
			}
		}	
		Windows::ExitWindow();
	}
}