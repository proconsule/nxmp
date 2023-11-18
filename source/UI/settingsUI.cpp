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
	
	unsigned int settings_focused = 0;
	bool settingsview_page = false;
	bool settingsview_laststate = true;
	int settingsview_combopopup = -1;
	bool settingsview_open;
	
	ImVec4 highlight_color = ImVec4(0, 255, 203, 1.00f);
	ImVec4 disable_color = ImVec4(0.41f, 0.40f, 0.40f, 1.00f);
	
	
	enum {
		SETTINGSPOPUP_FILEORDERING = 0,
		SETTINGSPOPUP_I18N,
		SETTINGSPOPUP_AUDIOOUT,
		SETTINGSPOPUP_ALANG,
		SETTINGSPOPUP_SLANG
		
		
		
	};
	
	void SetupComboPopup(const char *id) {
        ImGui::OpenPopup(id);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowPos(ImVec2(0, 280.0f*multiplyRes), ImGuiCond_Always, ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(1280.0f*multiplyRes, 440.0f*multiplyRes), ImGuiCond_Once);
	};
    
   void ExitComboPopup(void) {
        ImGui::PopStyleVar(2);
		ImGui::EndPopup();
        
    };
	
	
	void SettingsUIWindow(bool *focus, bool *first_item) {
		Windows::SetupWindow();
		
		if (ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
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
		ImGui::BeginChild("##settingsmainview",ImVec2(total_w,total_h-45*multiplyRes),false,ImGuiWindowFlags_NoScrollbar);
			
		
		
		ImGui::PushStyleColor(ImGuiCol_ChildBg,ImVec4(0.16f, 0.16f, 0.16f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0f, 0.0f, 0.0f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_NavHighlight, highlight_color);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(40.0f,0));
		
		ImGui::BeginChild("##settingsoptions", ImVec2(300-2 * ImGui::GetStyle().ItemSpacing.x, 0),false,ImGuiWindowFlags_AlwaysUseWindowPadding);
		
		
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {20.0f*multiplyRes, 15.0f*multiplyRes});
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, ImGui::GetStyle().CellPadding.y * 2)); // Fix
    				
		ImGui::BeginTable("##settingstable",1);
		
		char * settingsopts_strings[] = {SettingsMenu_STR[NXSET_GENERALS],SettingsMenu_STR[NXSET_PLAYERSETTINGS],SettingsMenu_STR[NXSET_DATABASE]/*,"Themes"*/};
		
		for(int i=0;i<IM_ARRAYSIZE(settingsopts_strings);i++){
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			float itemx = ImGui::GetCursorPosX()+20.0f;
			float itemy = ImGui::GetCursorPosY();
			std::string itemid = "##settingsitem" + std::to_string(i);
			
			if(settingsview_open){
				if(i==0){
					ImGui::SetKeyboardFocusHere();
				}
			}
			
			if (settingsview_page == !settingsview_laststate){
				if(!settingsview_page){
					if(settings_focused == i){
						ImGui::SetKeyboardFocusHere();
					}
				}
			}
			
			if (ImGui::Selectable(itemid.c_str(), false)){
				
			}
			if (ImGui::IsItemFocused()){
				settings_focused = i;
			}
			//if (!settingsview_page && settings_focused == i) ImGui::SetKeyboardFocusHere();		
			
			ImGui::SameLine();
			if(settings_focused == i){
				ImGui::PushStyleColor(ImGuiCol_Text, highlight_color);

			}
			ImGui::SetCursorPosX(itemx);
			ImGui::Text(settingsopts_strings[i]);
			if(settings_focused == i){
				ImGui::PopStyleColor();
			}
			if(settings_focused == i){
				ImGuiWindow* window = ImGui::GetCurrentWindow();
				window->DrawList->AddLine(ImVec2(itemx,itemy),ImVec2(itemx,itemy+35.0f*multiplyRes) , ImGui::GetColorU32(highlight_color), 2.0f);
			}
				
		}
		settingsview_open=false;
		ImGui::EndTable();
		ImGui::PopStyleVar(2);
		
		if (settingsview_page == !settingsview_laststate){
			if(!settingsview_page){
				std::string itemid = "##settingsitem" + std::to_string(settings_focused);
				ImGui::FocusWindow(ImGui::GetCurrentWindow());
				ImGui::SetFocusID(ImGui::GetID(itemid.c_str()), ImGui::GetCurrentWindow());
				
			}
		}
		
		ImGui::EndChild();
		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar();
		
		ImGui::SameLine();
		
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {20.0f*multiplyRes, 15.0f*multiplyRes});
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, ImGui::GetStyle().CellPadding.y * 2)); // Fix
    	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(30.0f,0));
		
		//ImGui::PushStyleColor(ImGuiCol_ChildBg,ImVec4(0.16f, 0.16f, 0.16f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0f, 0.0f, 0.0f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_NavHighlight, highlight_color);
		
		
		ImGui::BeginChild("##settingview", ImVec2(ImGui::GetContentRegionAvail().x, 0),false,ImGuiWindowFlags_AlwaysUseWindowPadding);
		if(settings_focused == 0){
			if (settingsview_page == !settingsview_laststate){
				if(settingsview_page){
					ImGui::SetKeyboardFocusHere();
				}
			}
			ImGui::Separator();
			if (ImGui::Selectable("##showhidden", false)){
				configini->setshowHidden(!configini->getshowHidden(true));
			}
			ImGui::SameLine();
			ImGui::Text(SettingsMenu_STR[NXSET_HIDDENFILES]);
			std::string showhiddentext = configini->getshowHidden(true)? Common_STR[NXCOMMON_YES]:Common_STR[NXCOMMON_NO];
			ImVec2 textsize = ImGui::CalcTextSize(showhiddentext.c_str());
			ImGui::SameLine(900-textsize.x);
			if(configini->getshowHidden(true)){
				ImGui::PushStyleColor(ImGuiCol_Text, highlight_color);
			}else{
				ImGui::PushStyleColor(ImGuiCol_Text, disable_color);
			}
			ImGui::Text(showhiddentext.c_str());
			ImGui::PopStyleColor();
			ImGui::Separator();
			if (ImGui::Selectable("##fileordering", false  )){
				settingsview_combopopup = SETTINGSPOPUP_FILEORDERING;
			}
			ImGui::SameLine();
			ImGui::Text("Default File Ordering");
			char *orderlistcombo[] = {Popup_STR[NXPOPUP_SORTASC_NAME],Popup_STR[NXPOPUP_SORTDESC_NAME],Popup_STR[NXPOPUP_SORTASC_DATE],Popup_STR[NXPOPUP_SORTDESC_DATE],Popup_STR[NXPOPUP_SORTASC_SIZE],Popup_STR[NXPOPUP_SORTDESC_SIZE]};
			std::string fileorderingtext = orderlistcombo[configini->getsortOrder(true)];
			textsize = ImGui::CalcTextSize(fileorderingtext.c_str());
			ImGui::SameLine(900*multiplyRes-textsize.x);
			ImGui::PushStyleColor(ImGuiCol_Text, highlight_color);
			ImGui::Text(fileorderingtext.c_str());
			ImGui::PopStyleColor();
			
			ImGui::Separator();
			if (ImGui::Selectable("##ocstartupsel", false)){
				configini->setUseOc(!configini->getUseOc(true));
			}
			ImGui::SameLine();
			ImGui::Text(SettingsMenu_STR[NXSET_OVERCLOCK_STARTUP]);
			std::string showocstartuptext = configini->getUseOc(true)? Common_STR[NXCOMMON_YES]:Common_STR[NXCOMMON_NO];
			textsize = ImGui::CalcTextSize(showocstartuptext.c_str());
			ImGui::SameLine(900-textsize.x);
			if(configini->getUseOc(true)){
				ImGui::PushStyleColor(ImGuiCol_Text, highlight_color);
			}else{
				ImGui::PushStyleColor(ImGuiCol_Text, disable_color);
			}
			ImGui::Text(showocstartuptext.c_str());
			ImGui::PopStyleColor();
			
			ImGui::Separator();
			if (ImGui::Selectable("##i18nsettingsel", false)){
				settingsview_combopopup = SETTINGSPOPUP_I18N;
			}
			ImGui::SameLine();
			ImGui::Text("I18N");
			std::string i18nsettingtext = NXLANGNAME_ENG[configini->getInterfaceLang(true)];
			textsize = ImGui::CalcTextSize(i18nsettingtext.c_str());
			ImGui::SameLine(900-textsize.x);
			ImGui::PushStyleColor(ImGuiCol_Text, highlight_color);
			ImGui::Text(i18nsettingtext.c_str());
			ImGui::PopStyleColor();
			
			
		}
		else if(settings_focused == 1){
			ImGui::Separator();
			if (settingsview_page == !settingsview_laststate){
				if(settingsview_page){
					ImGui::SetKeyboardFocusHere();
				}
			}
			if (ImGui::Selectable("##hwdecoder", false)){
				configini->setHWDec(!configini->getHWDec(true));
			}
			ImGui::SameLine();
			ImGui::Text(SettingsMenu_STR[NXSET_HWDECODER]);
			std::string showhiddentext = configini->getHWDec(true)? Common_STR[NXCOMMON_YES]:Common_STR[NXCOMMON_NO];
			ImVec2 textsize = ImGui::CalcTextSize(showhiddentext.c_str());
			ImGui::SameLine(900-textsize.x);
			if(configini->getHWDec(true)){
				ImGui::PushStyleColor(ImGuiCol_Text, highlight_color);
			}else{
				ImGui::PushStyleColor(ImGuiCol_Text, disable_color);
			}
			ImGui::Text(showhiddentext.c_str());
			ImGui::PopStyleColor();
			ImGui::Separator();
;
			if (ImGui::Selectable("##audiooutdev", false  )){
				settingsview_combopopup = SETTINGSPOPUP_AUDIOOUT;
			}
			ImGui::SameLine();
			ImGui::Text(SettingsMenu_STR[NXSET_AUDIOOUTPUT]);
			char *audiooutlist[] = {"SDL","Audren"};
			std::string curr_audioout = audiooutlist[configini->getAout(true)];
			textsize = ImGui::CalcTextSize(curr_audioout.c_str());
			ImGui::SameLine(900*multiplyRes-textsize.x);
			ImGui::PushStyleColor(ImGuiCol_Text, highlight_color);
			ImGui::Text(curr_audioout.c_str());
			ImGui::PopStyleColor();
			
			ImGui::Separator();
			std::string curr_demuxcachestr = std::to_string(configini->getDemuxCache(true));
			if (ImGui::Selectable("##demuxcachesel", false  )){
				curr_demuxcachestr = ShowSWKeyboard(curr_demuxcachestr,SettingsMenu_STR[NXSET_DEMUXCACHE],true);
				configini->setDemuxCache(atoi(curr_demuxcachestr.c_str()));
			}
			ImGui::SameLine();
			ImGui::Text(SettingsMenu_STR[NXSET_DEMUXCACHE]);
			textsize = ImGui::CalcTextSize(curr_demuxcachestr.c_str());
			ImGui::SameLine(900*multiplyRes-textsize.x);
			ImGui::PushStyleColor(ImGuiCol_Text, highlight_color);
			ImGui::Text(curr_demuxcachestr.c_str());
			ImGui::PopStyleColor();
			
			
			ImGui::Separator();
			std::string curr_shortseekstr = std::to_string(configini->getShortSeek(true));
			if (ImGui::Selectable("##shortseeksel", false  )){
				curr_shortseekstr = ShowSWKeyboard(curr_shortseekstr,SettingsMenu_STR[NXSET_SHORTSEEK],true);
				configini->setShortSeek(atoi(curr_shortseekstr.c_str()));
			}
			ImGui::SameLine();
			ImGui::Text(SettingsMenu_STR[NXSET_SHORTSEEK]);
			textsize = ImGui::CalcTextSize(curr_shortseekstr.c_str());
			ImGui::SameLine(900*multiplyRes-textsize.x);
			ImGui::PushStyleColor(ImGuiCol_Text, highlight_color);
			ImGui::Text(curr_shortseekstr.c_str());
			ImGui::PopStyleColor();
			
			ImGui::Separator();
			std::string curr_longseekstr = std::to_string(configini->getLongSeek(true));
			if (ImGui::Selectable("##longseeksel", false  )){
				curr_longseekstr = ShowSWKeyboard(curr_longseekstr,SettingsMenu_STR[NXSET_LONGSEEK],true);
				configini->setLongSeek(atoi(curr_longseekstr.c_str()));
			}
			ImGui::SameLine();
			ImGui::Text(SettingsMenu_STR[NXSET_LONGSEEK]);
			textsize = ImGui::CalcTextSize(curr_longseekstr.c_str());
			ImGui::SameLine(900*multiplyRes-textsize.x);
			ImGui::PushStyleColor(ImGuiCol_Text, highlight_color);
			ImGui::Text(curr_longseekstr.c_str());
			ImGui::PopStyleColor();
			
			
			ImGui::Separator();
			if (ImGui::Selectable("##usealangsel", false)){
				configini->setUseAlang(!configini->getUseAlang(true));
			}
			ImGui::SameLine();
			ImGui::Text(SettingsMenu_STR[NXSET_USEALANG]);
			std::string curr_usealangstr = configini->getUseAlang(true)? Common_STR[NXCOMMON_YES]:Common_STR[NXCOMMON_NO];
			textsize = ImGui::CalcTextSize(curr_usealangstr.c_str());
			ImGui::SameLine(900-textsize.x);
			if(configini->getUseAlang(true)){
				ImGui::PushStyleColor(ImGuiCol_Text, highlight_color);
			}else{
				ImGui::PushStyleColor(ImGuiCol_Text, disable_color);
			}
			ImGui::Text(curr_usealangstr.c_str());
			ImGui::PopStyleColor();
			
			if(configini->getUseAlang(true)){
				if (ImGui::Selectable("##alangselcombo", false  )){
					settingsview_combopopup = SETTINGSPOPUP_ALANG;
				}
				ImGui::SameLine();
				ImGui::Text(SettingsMenu_STR[NXSET_ALANG]);
				std::string curr_alang = Utility::getLanguages()[configini->getAlang(true)].lang3.c_str();
				textsize = ImGui::CalcTextSize(curr_alang.c_str());
				ImGui::SameLine(900*multiplyRes-textsize.x);
				ImGui::PushStyleColor(ImGuiCol_Text, highlight_color);
				ImGui::Text(curr_alang.c_str());
				ImGui::PopStyleColor();
			}
			
			ImGui::Separator();
			if (ImGui::Selectable("##useslangsel", false)){
				configini->setUseSlang(!configini->getUseSlang(true));
			}
			ImGui::SameLine();
			ImGui::Text(SettingsMenu_STR[NXSET_USESLANG]);
			std::string curr_useslangstr = configini->getUseSlang(true)? Common_STR[NXCOMMON_YES]:Common_STR[NXCOMMON_NO];
			textsize = ImGui::CalcTextSize(curr_useslangstr.c_str());
			ImGui::SameLine(900-textsize.x);
			if(configini->getUseSlang(true)){
				ImGui::PushStyleColor(ImGuiCol_Text, highlight_color);
			}else{
				ImGui::PushStyleColor(ImGuiCol_Text, disable_color);
			}
			ImGui::Text(curr_useslangstr.c_str());
			ImGui::PopStyleColor();
			
			if(configini->getUseSlang(true)){
				if (ImGui::Selectable("##slangselcombo", false  )){
					settingsview_combopopup = SETTINGSPOPUP_SLANG;
				}
				ImGui::SameLine();
				ImGui::Text(SettingsMenu_STR[NXSET_SLANG]);
				std::string curr_slang = Utility::getLanguages()[configini->getSlang(true)].lang3.c_str();
				textsize = ImGui::CalcTextSize(curr_slang.c_str());
				ImGui::SameLine(900*multiplyRes-textsize.x);
				ImGui::PushStyleColor(ImGuiCol_Text, highlight_color);
				ImGui::Text(curr_slang.c_str());
				ImGui::PopStyleColor();
			}
			
			ImGui::Separator();
			std::string curr_subfontsizestr = std::to_string(configini->getSubFontSize(true));
			if (ImGui::Selectable("##subfontsizesel", false  )){
				curr_subfontsizestr = ShowSWKeyboard(curr_subfontsizestr,SettingsMenu_STR[NXSET_SUBFONTSIZE],true);
				configini->setSubFontSize(atoi(curr_subfontsizestr.c_str()));
			}
			ImGui::SameLine();
			ImGui::Text(SettingsMenu_STR[NXSET_SUBFONTSIZE]);
			textsize = ImGui::CalcTextSize(curr_subfontsizestr.c_str());
			ImGui::SameLine(900*multiplyRes-textsize.x);
			ImGui::PushStyleColor(ImGuiCol_Text, highlight_color);
			ImGui::Text(curr_subfontsizestr.c_str());
			ImGui::PopStyleColor();
			
			ImGui::Separator();
			std::string curr_subfontscalestr = std::to_string(configini->getSubFontScale(true));
			if (ImGui::Selectable("##subfontscalesel", false  )){
				curr_subfontscalestr = ShowSWKeyboard(curr_subfontscalestr,SettingsMenu_STR[NXSET_SUBFONTSCALE]);
				configini->setSubFontScale(atof(curr_subfontscalestr.c_str()));
			}
			ImGui::SameLine();
			ImGui::Text(SettingsMenu_STR[NXSET_SUBFONTSCALE]);
			textsize = ImGui::CalcTextSize(curr_subfontscalestr.c_str());
			ImGui::SameLine(900*multiplyRes-textsize.x);
			ImGui::PushStyleColor(ImGuiCol_Text, highlight_color);
			ImGui::Text(curr_subfontscalestr.c_str());
			ImGui::PopStyleColor();
			
			
		}else if(settings_focused == 2){
			
			ImGui::Separator();
			if (settingsview_page == !settingsview_laststate){
				if(settingsview_page){
					ImGui::SetKeyboardFocusHere();
				}
			}
			if (ImGui::Selectable("##usedbsel", false)){
				configini->setDbActive(!configini->getDbActive(true));
				if(sqlitedb != nullptr){
					delete sqlitedb;
					sqlitedb = nullptr;
				}
				if(configini->getDbActive(true)){
					sqlitedb = new SQLiteDB(configini->getDbPath());
					sqlitedb->UpdateDbStats();
				}
			}
			ImGui::SameLine();
			ImGui::Text(SettingsMenu_STR[NXSET_USEDATABASE]);
			std::string usedbtext = configini->getDbActive(true)? Common_STR[NXCOMMON_YES]:Common_STR[NXCOMMON_NO];
			ImVec2 textsize = ImGui::CalcTextSize(usedbtext.c_str());
			ImGui::SameLine(900-textsize.x);
			if(configini->getDbActive(true)){
				ImGui::PushStyleColor(ImGuiCol_Text, highlight_color);
			}else{
				ImGui::PushStyleColor(ImGuiCol_Text, disable_color);
			}
			ImGui::Text(usedbtext.c_str());
			ImGui::PopStyleColor();
			if(sqlitedb != nullptr){
			ImGui::Separator();
				if(sqlitedb->getCorrupted()){
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),"Database File Corrupted");
				}else{
					ImGui::Text(SettingsMenu_STR[NXSET_DATABASEVERSION]);
					ImVec2 textsize = ImGui::CalcTextSize(sqlitedb->getDbVersion().c_str());
					ImGui::SameLine(900-textsize.x);
					ImGui::Text(sqlitedb->getDbVersion().c_str());
					ImGui::Separator();
					ImGui::Text(SettingsMenu_STR[NXSET_SQLITEVERSION]);
					ImGui::SameLine();
					textsize = ImGui::CalcTextSize(sqlitedb->getSQLiteVersion().c_str());
					ImGui::SameLine(900-textsize.x);
					ImGui::Text(sqlitedb->getSQLiteVersion().c_str());
					int recnum = 0;
					int reccom = 0;
					sqlitedb->GetDbStats(recnum,reccom);
					std::string recnumstring = std::to_string(recnum);
					std::string reccomstring = std::to_string(reccom);
					ImGui::Separator();
					ImGui::Text(SettingsMenu_STR[NXSET_RESUMEPOINTCOUNT]);
					textsize = ImGui::CalcTextSize(recnumstring.c_str());
					ImGui::SameLine(900-textsize.x);
					ImGui::Text(recnumstring.c_str());
					ImGui::Separator();
					ImGui::Text(SettingsMenu_STR[NXSET_COMPLETEDCOUNT]);
					textsize = ImGui::CalcTextSize(reccomstring.c_str());
					ImGui::SameLine(900-textsize.x);
					ImGui::Text(reccomstring.c_str());
					
					
						
				}
			}else{
				ImGui::Separator();
				ImGui::Text(SettingsMenu_STR[NXSET_DATABASENOTACTIVE]);
			}
		}
		
		
		/*
		if (settingsview_page == !settingsview_laststate){
			if(settingsview_page){
					ImGui::FocusWindow(ImGui::GetCurrentWindow());
					ImGui::SetFocusID(ImGui::GetID("##showhidden"), ImGui::GetCurrentWindow());
					//ImGui::SetNavID(ImGui::GetID("##showhidden"),0,0,0);
			}
		}
		*/		
		ImGui::EndChild();
		ImGui::PopStyleVar(3);
		ImGui::PopStyleColor(2);
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
		ImGui::Text(Common_STR[NXCOMMON_NAVIGATION]);
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
		ImGui::Text(FONT_A_BUTTON_FILLED);
		ImGui::SameLine();
		ImGui::Text(Common_STR[NXCOMMON_SELECT]);
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
		ImGui::Text(FONT_Y_BUTTON_FILLED);
		ImGui::SameLine();
		ImGui::Text(Common_STR[NXCOMMON_HOMESAVE]);
		
		window->DrawList->AddLine(startpos,ImVec2(startpos.x+1280*multiplyRes,startpos.y) , ImGui::GetColorU32(ImVec4(1.0f,1.0f,1.0f,1.0f)), 1.0f);
		
		ImGui::EndChild();
		
		
	
		settingsview_laststate = settingsview_page;
		Windows::ExitWindow();
		if(settingsview_combopopup>-1){
			if(settingsview_combopopup == SETTINGSPOPUP_FILEORDERING){
				SetupComboPopup("##fileorderingcombo");
				char *orderlistcombo[] = {Popup_STR[NXPOPUP_SORTASC_NAME],Popup_STR[NXPOPUP_SORTDESC_NAME],Popup_STR[NXPOPUP_SORTASC_DATE],Popup_STR[NXPOPUP_SORTDESC_DATE],Popup_STR[NXPOPUP_SORTASC_SIZE],Popup_STR[NXPOPUP_SORTDESC_SIZE]};
				
				ImGui::PushStyleColor(ImGuiCol_ChildBg,ImVec4(0.16f, 0.16f, 0.16f, 1.00f));
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0f, 0.0f, 0.0f, 1.00f));
				ImGui::PushStyleColor(ImGuiCol_NavHighlight, highlight_color);
		
				ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {20.0f*multiplyRes, 15.0f*multiplyRes});
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, ImGui::GetStyle().CellPadding.y * 2)); // Fix
    	
				if (ImGui::BeginPopupModal("##fileorderingcombo", nullptr, ImGuiWindowFlags_NoTitleBar)) {
					ImGui::Text("File Ordering");
					ImGui::Separator();
					
					for (int n = 0; n < IM_ARRAYSIZE(orderlistcombo); n++){
						ImGui::Dummy(ImVec2(300*multiplyRes,0));
						ImGui::SameLine();
						std::string ordercomboid = "##ordercombo" + std::to_string(n);
						if(ImGui::Selectable(ordercomboid.c_str(),false)){
							settingsview_combopopup = -1;
							configini->setsortOrder(n);
						}	
						ImGui::SameLine();
						ImGui::Text(orderlistcombo[n]);	
						if(n  == configini->getsortOrder(true)){
							
							ImVec2 optstextsize = ImGui::CalcTextSize(orderlistcombo[n]);
							ImGui::SameLine(1000*multiplyRes);
							ImGui::PushStyleColor(ImGuiCol_Text, highlight_color);
							ImGui::Text(FONT_CHECKED_ICON);
							ImGui::PopStyleColor();
							
							
						}
					}
					
				}
				ExitComboPopup();
				ImGui::PopStyleColor(3);
				ImGui::PopStyleVar(2);
			} else if(settingsview_combopopup == SETTINGSPOPUP_I18N){
				SetupComboPopup("##i18nsettingscombo");
				char *orderlistcombo[] = {Popup_STR[NXPOPUP_SORTASC_NAME],Popup_STR[NXPOPUP_SORTDESC_NAME],Popup_STR[NXPOPUP_SORTASC_DATE],Popup_STR[NXPOPUP_SORTDESC_DATE],Popup_STR[NXPOPUP_SORTASC_SIZE],Popup_STR[NXPOPUP_SORTDESC_SIZE]};
				
				ImGui::PushStyleColor(ImGuiCol_ChildBg,ImVec4(0.16f, 0.16f, 0.16f, 1.00f));
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0f, 0.0f, 0.0f, 1.00f));
				ImGui::PushStyleColor(ImGuiCol_NavHighlight, highlight_color);
		
				ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {20.0f*multiplyRes, 15.0f*multiplyRes});
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, ImGui::GetStyle().CellPadding.y * 2)); // Fix
    	
				if (ImGui::BeginPopupModal("##i18nsettingscombo", nullptr, ImGuiWindowFlags_NoTitleBar)) {
					ImGui::Text("I18N");
					ImGui::Separator();
					
					for (int n = 0; n < NX_LANGS_NR_ITEMS; n++){
						ImGui::Dummy(ImVec2(300*multiplyRes,0));
						ImGui::SameLine();
						std::string ordercomboid = "##i18nlangcombo" + std::to_string(n);
						if(ImGui::Selectable(ordercomboid.c_str(),false)){
							settingsview_combopopup = -1;
							configini->setInterfaceLang(n);
							InitLang((NX_LANGS)n);
						}	
						ImGui::SameLine();
						ImGui::Text(NXLANGNAME_ENG[n]);	
						if(n  == configini->getInterfaceLang(true)){
							
							ImVec2 optstextsize = ImGui::CalcTextSize(NXLANGNAME_ENG[n]);
							ImGui::SameLine(1000*multiplyRes);
							ImGui::PushStyleColor(ImGuiCol_Text, highlight_color);
							ImGui::Text(FONT_CHECKED_ICON);
							ImGui::PopStyleColor();
							
							
						}
					}
					
				} 
				ExitComboPopup();
				ImGui::PopStyleColor(3);
				ImGui::PopStyleVar(2);
			}else if(settingsview_combopopup == SETTINGSPOPUP_AUDIOOUT){
				SetupComboPopup("##audiooutcombo");
				
				ImGui::PushStyleColor(ImGuiCol_ChildBg,ImVec4(0.16f, 0.16f, 0.16f, 1.00f));
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0f, 0.0f, 0.0f, 1.00f));
				ImGui::PushStyleColor(ImGuiCol_NavHighlight, highlight_color);
		
				ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {20.0f*multiplyRes, 15.0f*multiplyRes});
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, ImGui::GetStyle().CellPadding.y * 2)); // Fix
    	
				if (ImGui::BeginPopupModal("##audiooutcombo", nullptr, ImGuiWindowFlags_NoTitleBar)) {
					ImGui::Text(SettingsMenu_STR[NXSET_AUDIOOUTPUT]);
					ImGui::Separator();
					char *audiooutlist[] = {"SDL","Audren"};
					for (int n = 0; n < IM_ARRAYSIZE(audiooutlist); n++){
#ifdef DEKO3D_BACKEND
						if(n==0)ImGui::BeginDisabled();
#endif
						ImGui::Dummy(ImVec2(300*multiplyRes,0));
						ImGui::SameLine();
						std::string ordercomboid = "##audiooutcomboitem" + std::to_string(n);
						if(ImGui::Selectable(ordercomboid.c_str(),false)){
							settingsview_combopopup = -1;
							configini->setAout(n);
						}	
						ImGui::SameLine();
						ImGui::Text(audiooutlist[n]);	
						if(n  == configini->getAout(true)){
							
							ImVec2 optstextsize = ImGui::CalcTextSize(audiooutlist[n]);
							ImGui::SameLine(1000*multiplyRes);
							ImGui::PushStyleColor(ImGuiCol_Text, highlight_color);
							ImGui::Text(FONT_CHECKED_ICON);
							ImGui::PopStyleColor();
							
							
						}
#ifdef DEKO3D_BACKEND
						if(n==0)ImGui::EndDisabled();
#endif
					}
					
				} 
				ExitComboPopup();
				ImGui::PopStyleColor(3);
				ImGui::PopStyleVar(2);	
					
			}else if(settingsview_combopopup == SETTINGSPOPUP_ALANG){
				SetupComboPopup("##alangcombo");
				
				ImGui::PushStyleColor(ImGuiCol_ChildBg,ImVec4(0.16f, 0.16f, 0.16f, 1.00f));
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0f, 0.0f, 0.0f, 1.00f));
				ImGui::PushStyleColor(ImGuiCol_NavHighlight, highlight_color);
		
				ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {20.0f*multiplyRes, 15.0f*multiplyRes});
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, ImGui::GetStyle().CellPadding.y * 2)); // Fix
    	
				if (ImGui::BeginPopupModal("##alangcombo", nullptr, ImGuiWindowFlags_NoTitleBar)) {
					ImGui::Text(SettingsMenu_STR[NXSET_ALANG]);
					ImGui::Separator();
					std::vector<Utility::lang_code_struct> tmplangs = Utility::getLanguages();
					for (int n = 0; n < tmplangs.size(); n++){
						ImGui::Dummy(ImVec2(300*multiplyRes,0));
						ImGui::SameLine();
						std::string ordercomboid = "##alangcomboitem" + std::to_string(n);
						if(ImGui::Selectable(ordercomboid.c_str(),false)){
							settingsview_combopopup = -1;
							configini->setAlang(n);
						}	
						ImGui::SameLine();
						ImGui::Text(tmplangs[n].lang3.c_str());	
						if(n  == configini->getAlang(true)){
							
							ImVec2 optstextsize = ImGui::CalcTextSize(tmplangs[n].lang3.c_str());
							ImGui::SameLine(1000*multiplyRes);
							ImGui::PushStyleColor(ImGuiCol_Text, highlight_color);
							ImGui::Text(FONT_CHECKED_ICON);
							ImGui::PopStyleColor();
							
							
						}
					}
					
				} 
				ExitComboPopup();
				ImGui::PopStyleColor(3);
				ImGui::PopStyleVar(2);	
					
			}else if(settingsview_combopopup == SETTINGSPOPUP_SLANG){
				SetupComboPopup("##slangcombo");
				
				ImGui::PushStyleColor(ImGuiCol_ChildBg,ImVec4(0.16f, 0.16f, 0.16f, 1.00f));
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0f, 0.0f, 0.0f, 1.00f));
				ImGui::PushStyleColor(ImGuiCol_NavHighlight, highlight_color);
		
				ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {20.0f*multiplyRes, 15.0f*multiplyRes});
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, ImGui::GetStyle().CellPadding.y * 2)); // Fix
    	
				if (ImGui::BeginPopupModal("##slangcombo", nullptr, ImGuiWindowFlags_NoTitleBar)) {
					ImGui::Text(SettingsMenu_STR[NXSET_SLANG]);
					ImGui::Separator();
					std::vector<Utility::lang_code_struct> tmplangs = Utility::getLanguages();
					for (int n = 0; n < tmplangs.size(); n++){
						ImGui::Dummy(ImVec2(300*multiplyRes,0));
						ImGui::SameLine();
						std::string ordercomboid = "##slangcomboitem" + std::to_string(n);
						if(ImGui::Selectable(ordercomboid.c_str(),false)){
							settingsview_combopopup = -1;
							configini->setSlang(n);
						}	
						ImGui::SameLine();
						ImGui::Text(tmplangs[n].lang3.c_str());	
						if(n  == configini->getSlang(true)){
							
							ImVec2 optstextsize = ImGui::CalcTextSize(tmplangs[n].lang3.c_str());
							ImGui::SameLine(1000*multiplyRes);
							ImGui::PushStyleColor(ImGuiCol_Text, highlight_color);
							ImGui::Text(FONT_CHECKED_ICON);
							ImGui::PopStyleColor();
							
							
						}
					}
					
				} 
				ExitComboPopup();
				ImGui::PopStyleColor(3);
				ImGui::PopStyleVar(2);	
					
			}
		}
		
	}
	
	
	
}