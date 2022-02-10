#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"
#include "themes.h"
#include "updater.h"


namespace Windows {
	
	Themes *themes = nullptr;
	Updater *updater = nullptr;
	Tex thempreview;
	
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

				//Slang
				ImGui::Dummy(ImVec2(0.0f,10.0f));
				bool slangbool = configini->getUseSlang(true);
				if(ImGui::Checkbox("Use Auto Subtitles Language", &slangbool)){
					configini->setUseSlang(slangbool);
				}
				
				if(!slangbool){
					ImGui::BeginDisabled();
				}


				const char* combo_preview_value_2 = Utility::getLanguages()[configini->getSlang(true)].lang3.c_str();  // Pass in the preview value visible before opening the combo (it could be anything)
				ImGui::PushItemWidth(300);
				if (ImGui::BeginCombo("Subtitles Language", combo_preview_value_2, 0))
				{	
					for (int n = 0; n < Utility::getLanguages().size(); n++)
					{
						const bool is_selected = (configini->getSlang(true) == n);
						if (ImGui::Selectable(Utility::getLanguages()[n].lang3.c_str(), is_selected))
							configini->setSlang(n);

						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				if(!slangbool){
					ImGui::EndDisabled();
				}
				//End Slang
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
				if(ImGui::RadioButton("Swipe Off", &touchseekradio, 2)){
					configini->setPlayerSwipeSeek(touchseekradio);
				}
				ImGui::SameLine();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Themes")) {
				if(themes == nullptr){
					themes = new Themes();
					themes->getThemes();
				}
				std::vector<std::string> themescombomenu;
				themescombomenu.push_back("Default");
				for(int i=0;i<themes->themeslist.size();i++){
					themescombomenu.push_back(themes->themeslist[i].name);
				}
				int themeprevidx = themes->getThemeIDX(configini->getThemeName(true))+1;
				const char* combo_themepreview_value = themescombomenu[themeprevidx].c_str();
				
				if (ImGui::BeginCombo("Themes", combo_themepreview_value, 0)){
					for (int n = 0; n < themescombomenu.size(); n++)
					{
						const bool is_selected = false;
						if (ImGui::Selectable(themescombomenu[n].c_str(), is_selected)){
							if(n ==0){
								configini->setThemeName("Default");
								//themes->setDefault();
							}else{
								//themes->setTheme(themes->themeslist[n-1].path);
								configini->setThemeName(themes->themeslist[n-1].name);
							}
						}
					}
					ImGui::EndCombo();
					
				}
				//ImGui::SameLine(100);
				if (ImGui::Button("Apply Theme")){
					if(themeprevidx == 0){
						themes->setDefault();
					}else{
						themes->setTheme(themes->themeslist[themeprevidx-1].path);
					}
				}
				
				if(themeprevidx == 0){
					ImGui::Text("Name: %s","Default");
					ImGui::Text("Author: %s","proconsule/bodyXY");
					Utility::TxtLoadFromFile("./romfs/preview.jpg",&thempreview.id,&thempreview.width,&thempreview.height);
								
				}else{
					Utility::TxtLoadFromFile(themes->themeslist[themeprevidx-1].path+"preview.jpg",&thempreview.id,&thempreview.width,&thempreview.height);
								
					ImGui::Text("Name: %s",themes->themeslist[themeprevidx-1].name.c_str());
					ImGui::Text("Author: %s",themes->themeslist[themeprevidx-1].author.c_str());
					
				}
				
				ImGui::Image((void*)(intptr_t)thempreview.id, ImVec2(thempreview.width/2,thempreview.height/2));
				
					
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Updates")) {
				if(updater == nullptr){
					updater = new Updater();
					updater->fetchReleases();
				}
				
				bool lastprerelease = false;
				bool laststablerelease = false;
				gitrelease_struct gitprerelease;
				gitrelease_struct gitstablerelease;
				
				
				for(int i=0;i<updater->getReleases().size();i++){
					if(lastprerelease && laststablerelease)break;
					if(updater->getReleases()[i].prerelease && !lastprerelease){
						lastprerelease = true;
						gitprerelease = updater->getReleases()[i];
					}
					if(!updater->getReleases()[i].prerelease && !laststablerelease){
						laststablerelease = true;
						gitstablerelease = updater->getReleases()[i];
					}
				}
				
				ImGui::Text("LAST Nightly: %s",gitprerelease.tagname.c_str());
				ImGui::Text("LAST Stable: %s",gitstablerelease.tagname.c_str());
				
				
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
				//Slang
				configini->setSlang(configini->getSlang(false));
				//end Slang
				
				configini->setThemeName(configini->getThemeName(false));
				
				configini->setSubFontSize(configini->getSubFontSize(false));
				configini->setSubFontColor(configini->getSubFontColor(false));
				//bordercolor
				configini->setSubBorderColor(configini->getSubBorderColor(false));
				//endbordercolor
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