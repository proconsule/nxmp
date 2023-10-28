#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"
#include "themes.h"
#include "updater.h"

#include "nxmp-i18n.h"


namespace Windows {
	
	Themes *themes = nullptr;
	Updater *updater = nullptr;
	Tex thempreview;
	
	int lastthemeprevidx = -1;
	
    void SettingsMenuWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		if (ImGui::Begin("Settings Window", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
            if (!*focus) {
                ImGui::SetNextWindowFocus();
                *focus = true;
            }
			float total_w = ImGui::GetContentRegionAvail().x;
			float total_h = ImGui::GetContentRegionAvail().y;
			float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
			ImGui::BeginChild("##settingstablecontainer",ImVec2(total_w,total_h-30*multiplyRes));
				
			if (ImGui::BeginTabBar("Settings Tab bar", 0))
            {
				if (ImGui::BeginTabItem(SettingsMenu_STR[NXSET_GENERALS])) {
					ImGui::Text(SettingsMenu_STR[NXSET_FILEBROWSER]);
					ImGui::Separator();
					bool showhiddenbool = configini->getshowHidden(true);
					if(ImGui::Checkbox(SettingsMenu_STR[NXSET_HIDDENFILES], &showhiddenbool)){
						configini->setshowHidden(showhiddenbool);
						
					}
					ImGui::Dummy(ImVec2(0.0f,20.0f));
					char *orderlistcombo[] = {Popup_STR[NXPOPUP_SORTASC_NAME],Popup_STR[NXPOPUP_SORTDESC_NAME],Popup_STR[NXPOPUP_SORTASC_DATE],Popup_STR[NXPOPUP_SORTDESC_DATE],Popup_STR[NXPOPUP_SORTASC_SIZE],Popup_STR[NXPOPUP_SORTDESC_SIZE]};
					if (ImGui::BeginCombo("Default File Ordering", orderlistcombo[configini->getsortOrder(true)], 0))
					{	
						for (int n = 0; n < IM_ARRAYSIZE(orderlistcombo); n++)
						{
							const bool is_selected = (configini->getInterfaceLang(true) == n);
							
							if (ImGui::Selectable(orderlistcombo[n], is_selected)){
								configini->setsortOrder(n);
							}
							

							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
					ImGui::Dummy(ImVec2(0.0f,20.0f));
					ImGui::Text(SettingsMenu_STR[NXSET_NAVIGATION]);
					ImGui::Separator();
					bool touchbool = configini->getTouchEnable(true);
					if(ImGui::Checkbox(SettingsMenu_STR[NXSET_TOUCHCONTROL], &touchbool)){
						configini->setTouchEnable(touchbool);
						if(touchbool){
							ImGuiIO &io = ImGui::GetIO();
							io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
						}else{
							ImGuiIO &io = ImGui::GetIO();
							io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
						}
					}
					ImGui::Dummy(ImVec2(0.0f,20.0f));
					ImGui::Text(SettingsMenu_STR[NXSET_OVERCLOCK]);
					ImGui::Separator();
					bool ocdefault = configini->getUseOc(true);
					if(ImGui::Checkbox(SettingsMenu_STR[NXSET_OVERCLOCK_STARTUP], &ocdefault)){
						configini->setUseOc(ocdefault);	
					}
					ImGui::Dummy(ImVec2(0.0f,20.0f));
					ImGui::Text(SettingsMenu_STR[NXSET_MISC]);
					ImGui::Separator();
					bool vsyncdefault = configini->getVSYNC(true);
					if(ImGui::Checkbox(SettingsMenu_STR[NXSET_VSYNC], &vsyncdefault)){
						configini->setVSYNC(vsyncdefault);	
					}
					
					ImGui::Dummy(ImVec2(0.0f,20.0f));
					ImGui::SeparatorText("I18N");
					ImGui::PushItemWidth(300);
					if (ImGui::BeginCombo(SettingsMenu_STR[NXSET_INTLANG], NXLANGNAME_ENG[configini->getInterfaceLang(true)], 0))
					{	
						for (int n = 0; n < NX_LANGS_NR_ITEMS; n++)
						{
							const bool is_selected = (configini->getInterfaceLang(true) == n);
							if(!NXLANGACTIVE[n]){
								ImGui::BeginDisabled();
							}
							if (ImGui::Selectable(NXLANGNAME_ENG[n], is_selected)){
								configini->setInterfaceLang(n);
								InitLang((NX_LANGS)n);
							}
							if(!NXLANGACTIVE[n]){
								ImGui::EndDisabled();
							}

							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
					ImGui::Text("%s translation by %s",NXLANGNAME_ENG[configini->getInterfaceLang(true)],NXLANGAUTHORS[configini->getInterfaceLang(true)]);
					if(NXLANGNATIVE[configini->getInterfaceLang(true)]){
						ImGui::TextColored(ImVec4(0.0f,1.0f,0.0f,1.0f),"Translated into native language");
					}else{
						ImGui::TextColored(ImVec4(1.0f,0.7f,0.0f,1.0f),"Translated using online translator");
					}
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem(SettingsMenu_STR[NXSET_PLAYERSETTINGS]))
                {
					ImGui::Text(SettingsMenu_STR[NXSET_DECODER]);
					ImGui::Separator();
					bool hwdecdefault = configini->getHWDec(true);
					if(ImGui::Checkbox(SettingsMenu_STR[NXSET_HWDECODER], &hwdecdefault)){
						configini->setHWDec(hwdecdefault);	
					}
					
					ImGui::Text(SettingsMenu_STR[NXSET_DEMUXCACHE]);
					ImGui::Separator();
					ImGui::Text(SettingsMenu_STR[NXSET_DEMUXCACHESEC]);
					ImGui::SameLine(240,spacing);
					ImGui::PushButtonRepeat(true);
					if (ImGui::ArrowButton("##demuxleft", ImGuiDir_Left)) {
						if(configini->getShortSeek(true)-1 >0){
							configini->setDemuxCache(configini->getDemuxCache(true)-1);
						}
					}
					ImGui::SameLine(0.0f, spacing);
					if (ImGui::ArrowButton("##demuxright", ImGuiDir_Right)) { 
						configini->setDemuxCache(configini->getDemuxCache(true)+1);
					}
					ImGui::PopButtonRepeat();
					ImGui::SameLine();
					ImGui::Text("%d sec", configini->getDemuxCache(true));
					
					
					
					
					ImGui::Text(SettingsMenu_STR[NXSET_AUDIO]);
					ImGui::Separator();
					std::vector<std::string> aout_preview_value = {"SDL","Audren"};
					//ImGui::BeginDisabled();
					if (ImGui::BeginCombo(SettingsMenu_STR[NXSET_AUDIOOUTPUT], aout_preview_value[configini->getAout(true)].c_str(), 0))
					{	
						for (int n = 0; n < aout_preview_value.size(); n++)
						{
							const bool is_selected = (configini->getAout(true) == n);
							if (ImGui::Selectable(aout_preview_value[n].c_str(), is_selected))
								configini->setAout(n);

							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
					//ImGui::EndDisabled();
				ImGui::Text(SettingsMenu_STR[NXSET_SEEK]);
				ImGui::Separator();
				ImGui::Text(SettingsMenu_STR[NXSET_SHORTSEEK]);
				ImGui::SameLine(240,spacing);
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
			
				ImGui::Text(SettingsMenu_STR[NXSET_LONGSEEK]);
				ImGui::SameLine(240,spacing);
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
				
				ImGui::Text("Cache");
				ImGui::Separator();
				ImGui::SameLine(240,spacing);
				ImGui::PushButtonRepeat(true);
				if (ImGui::ArrowButton("##cacheleft", ImGuiDir_Left)) {
					if(configini->getDemuxCache(true)-1 >0){
						configini->setDemuxCache(configini->getDemuxCache(true)-1);
					}
				}
				ImGui::SameLine(0.0f, spacing);
				if (ImGui::ArrowButton("##cacheright", ImGuiDir_Right)) { 
					configini->setDemuxCache(configini->getDemuxCache(true)+1);
				}
				ImGui::PopButtonRepeat();
				ImGui::SameLine();
				ImGui::Text("%d sec", configini->getDemuxCache(true));
				
				
				
				ImGui::Dummy(ImVec2(0.0f,30.0f));
				
				ImGui::Text(SettingsMenu_STR[NXSET_LANG]);
				ImGui::Separator();
				bool alangbool = configini->getUseAlang(true);
				if(ImGui::Checkbox(SettingsMenu_STR[NXSET_USEALANG], &alangbool)){
					configini->setUseAlang(alangbool);
				}
				
				if(!alangbool){
					ImGui::BeginDisabled();
				}

				const char* combo_preview_value = Utility::getLanguages()[configini->getAlang(true)].lang3.c_str();  // Pass in the preview value visible before opening the combo (it could be anything)
				ImGui::PushItemWidth(300);
				if (ImGui::BeginCombo(SettingsMenu_STR[NXSET_ALANG], combo_preview_value, 0))
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
				if(ImGui::Checkbox(SettingsMenu_STR[NXSET_USESLANG], &slangbool)){
					configini->setUseSlang(slangbool);
				}
				
				if(!slangbool){
					ImGui::BeginDisabled();
				}


				const char* combo_preview_value_2 = Utility::getLanguages()[configini->getSlang(true)].lang3.c_str();  // Pass in the preview value visible before opening the combo (it could be anything)
				ImGui::PushItemWidth(300);
				if (ImGui::BeginCombo(SettingsMenu_STR[NXSET_SLANG], combo_preview_value_2, 0))
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
				ImGui::Text(SettingsMenu_STR[NXSET_SUBTITLE]);
				ImGui::Separator();
				ImGui::Text(SettingsMenu_STR[NXSET_SUBFONTSIZE]);
				ImGui::SameLine(240,spacing);
				ImGui::PushButtonRepeat(true);
				if (ImGui::ArrowButton("##subsizeleft", ImGuiDir_Left)) {
					if(configini->getSubFontSize(true)-1 >0){
						configini->setSubFontSize(configini->getSubFontSize(true)-1);
					}
				}
				ImGui::SameLine(0.0f, spacing);
				if (ImGui::ArrowButton("##subsizeright", ImGuiDir_Right)) { 
					if(configini->getSubFontSize(true)+1 <=120){
						configini->setSubFontSize(configini->getSubFontSize(true)+1);
					}
				}
				ImGui::PopButtonRepeat();
				ImGui::SameLine();
				ImGui::Text("%d", configini->getSubFontSize(true));
				ImGui::Dummy(ImVec2(0.0f,10.0f));

				//SubFontScale
				ImGui::Text(SettingsMenu_STR[NXSET_SUBFONTSCALE]);
				ImGui::SameLine(240,spacing);
				ImGui::PushButtonRepeat(true);
				if (ImGui::ArrowButton("##subsizescaleleft", ImGuiDir_Left)) {
					if(configini->getSubFontScale(true)-0.01f >0.0f){
						configini->setSubFontScale(configini->getSubFontScale(true)-0.01f);
					}
				}
				ImGui::SameLine(0.0f, spacing);
				if (ImGui::ArrowButton("##subsizescaleright", ImGuiDir_Right)) { 
					if(configini->getSubFontScale(true)+0.01f <=3.0f){
						configini->setSubFontScale(configini->getSubFontScale(true)+0.01f);
					}
				}
				ImGui::PopButtonRepeat();
				ImGui::SameLine();
				ImGui::Text("%.2f", configini->getSubFontScale(true));
				ImGui::Dummy(ImVec2(0.0f,30.0f));
				//endSubFontScale
				ImGui::Text(SettingsMenu_STR[NXSET_VIDEO]);
				ImGui::Separator();
				std::vector<std::string> deintmenu = {"No","Yes","Auto"};
				const char* combo_deintpreview_value = deintmenu[configini->getDeinterlace(true)].c_str();
				ImGui::PushItemWidth(300);
				if (ImGui::BeginCombo(SettingsMenu_STR[NXSET_VIDEODEINT], combo_deintpreview_value, 0))
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
			if (ImGui::BeginTabItem(SettingsMenu_STR[NXSET_DATABASE])) {
				bool usedbbool = configini->getDbActive(true);
				if(ImGui::Checkbox(SettingsMenu_STR[NXSET_USEDATABASE], &usedbbool)){
					if(sqlitedb != nullptr){
						delete sqlitedb;
						sqlitedb = nullptr;
					}
					if(usedbbool){
						sqlitedb = new SQLiteDB(configini->getDbPath());
						sqlitedb->UpdateDbStats();
					}
					configini->setDbActive(usedbbool);	
					
				}
				if(sqlitedb != nullptr){
					if(sqlitedb->getCorrupted()){
						ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),"Database File Corrupted");
					}else{
						ImGui::Text(SettingsMenu_STR[NXSET_DATABASEVERSION]);
						ImGui::SameLine();
						ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f),sqlitedb->getDbVersion().c_str());
					}
						ImGui::Text(SettingsMenu_STR[NXSET_SQLITEVERSION]);
						ImGui::SameLine();
						ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f),sqlitedb->getSQLiteVersion().c_str());
						int recnum = 0;
						int reccom = 0;
						if(sqlitedb!=nullptr)sqlitedb->GetDbStats(recnum,reccom);
						ImGui::SetCursorPosY(ImGui::GetCursorPosY()+30);
						ImGui::Text("%s%d\n",SettingsMenu_STR[NXSET_RESUMEPOINTCOUNT],recnum);
						ImGui::Text("%s%d\n",SettingsMenu_STR[NXSET_COMPLETEDCOUNT],reccom);
						ImGui::Separator();
						ImGui::SetCursorPosY(ImGui::GetCursorPosY()+50);
						ImGui::Text(SettingsMenu_STR[NXSET_STARTSAVINGINFO]);
						//ImGui::SameLine(400,spacing);
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
						
						
						ImGui::Text(SettingsMenu_STR[NXSET_STOPSAVINGINFO]);
						//ImGui::SameLine(400,spacing);
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
					ImGui::Text(SettingsMenu_STR[NXSET_DATABASENOTACTIVE]);
				}
				
				
					
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem(SettingsMenu_STR[NXSET_TOUCHSETTINGS])) {
				
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
					GUI::GUI_REINIT = true;
					
				}
				
				if(lastthemeprevidx != themeprevidx){
				
					if(themeprevidx == 0){
						Utility::TxtLoadFromFile("romfs:/preview.jpg",&thempreview.id,&thempreview.width,&thempreview.height);
									
					}else{
						Utility::TxtLoadFromFile(themes->themeslist[themeprevidx-1].path+"preview.jpg",&thempreview.id,&thempreview.width,&thempreview.height);
								
					}
					
				}
				
				if(themeprevidx == 0){
					ImGui::Text("Name: %s","Default");
					ImGui::Text("Author: %s","proconsule/bodyXY");			
				}else{
					ImGui::Text("Name: %s",themes->themeslist[themeprevidx-1].name.c_str());
					ImGui::Text("Author: %s",themes->themeslist[themeprevidx-1].author.c_str());
						
				}
				
				
				
				ImGui::Image((void*)(intptr_t)thempreview.id, ImVec2(thempreview.width/2,thempreview.height/2));
				lastthemeprevidx = themeprevidx;
					
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
			if(item.popupstate == POPUP_STATE_NONE){
					//ImGui::SetWindowFocus();
				}
			ImGui::EndTabBar();
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
		ImGui::Text(Common_STR[NXCOMMON_HOMESAVE]);
				
				
		ImGui::EndChild();
			
		}	
		Windows::ExitWindow();
	}
}