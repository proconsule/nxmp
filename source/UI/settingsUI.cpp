#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"

#include "updater.h"

#include "nxmp-i18n.h"


namespace Windows {
	
	unsigned int settings_focused = 0;
	bool settingsview_page = false;
	bool settingsview_laststate = true;
	int settingsview_combopopup = -1;
	bool settingsview_open;
	
	enum {
		SETTINGSPOPUP_FILEORDERING = 0,
		SETTINGSPOPUP_I18N,
		SETTINGSPOPUP_AUDIOOUT,
		SETTINGSPOPUP_ALANG,
		SETTINGSPOPUP_SLANG,
		SETTINGSPOPUP_THEMECOLOR,
		SETTINGSPOPUP_EXITMODE
		
		
		
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
			
		}
		
		float total_w = ImGui::GetContentRegionAvail().x;
		float total_h = ImGui::GetContentRegionAvail().y;
		float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
		ImGui::BeginChild("##settingsmainview",ImVec2(total_w,total_h-45*multiplyRes),false,ImGuiWindowFlags_NoScrollbar);
			
		
		
		ImGui::PushStyleColor(ImGuiCol_ChildBg,OptsTab_Bg_color);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, HeaderHover_color);
		ImGui::PushStyleColor(ImGuiCol_NavHighlight, Active_color);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(40.0f,0));
		
		ImGui::BeginChild("##settingsoptions", ImVec2(300-2 * ImGui::GetStyle().ItemSpacing.x, 0),false,ImGuiWindowFlags_AlwaysUseWindowPadding);
		
		
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {20.0f*multiplyRes, 15.0f*multiplyRes});
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, ImGui::GetStyle().CellPadding.y * 2)); // Fix
    				
		ImGui::BeginTable("##settingstable",1);
		
		//char * settingsopts_strings[] = {(char *)nxlangs::get_settingsmenu_str(nxlangs::NXSET_GENERALS).c_str(),(char *)nxlangs::get_settingsmenu_str(nxlangs::NXSET_PLAYERSETTINGS).c_str(),(char *)nxlangs::get_settingsmenu_str(nxlangs::NXSET_DATABASE).c_str()/*,"Themes"*/};
		std::vector<std::string> settingsopts_strings = {nxlangs::get_settingsmenu_str(nxlangs::NXSET_GENERALS),nxlangs::get_settingsmenu_str(nxlangs::NXSET_PLAYERSETTINGS),nxlangs::get_settingsmenu_str(nxlangs::NXSET_DATABASE)/*,"Themes"*/};
		
		
		
		for(int i=0;i<settingsopts_strings.size();i++){
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
				ImGui::PushStyleColor(ImGuiCol_Text, Active_color);

			}
			ImGui::SetCursorPosX(itemx);
			ImGui::Text(settingsopts_strings[i].c_str());
			if(settings_focused == i){
				ImGui::PopStyleColor();
			}
			if(settings_focused == i){
				ImGuiWindow* window = ImGui::GetCurrentWindow();
				window->DrawList->AddLine(ImVec2(itemx,itemy),ImVec2(itemx,itemy+35.0f*multiplyRes) , ImGui::GetColorU32(Active_color), 2.0f);
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
		
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, HeaderHover_color);
		ImGui::PushStyleColor(ImGuiCol_NavHighlight, Active_color);
		
		
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
			ImGui::Text(nxlangs::get_settingsmenu_str(nxlangs::NXSET_HIDDENFILES).c_str());
			std::string showhiddentext = configini->getshowHidden(true)? nxlangs::get_common_str(nxlangs::NXCOMMON_YES).c_str():nxlangs::get_common_str(nxlangs::NXCOMMON_NO).c_str();
			ImVec2 textsize = ImGui::CalcTextSize(showhiddentext.c_str());
			ImGui::SameLine(900-textsize.x);
			if(configini->getshowHidden(true)){
				ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			}else{
				ImGui::PushStyleColor(ImGuiCol_Text, Disabled_color);
			}
			ImGui::Text(showhiddentext.c_str());
			ImGui::PopStyleColor();
			ImGui::Separator();
			if (ImGui::Selectable("##fileordering", false  )){
				settingsview_combopopup = SETTINGSPOPUP_FILEORDERING;
			}
			ImGui::SameLine();
			ImGui::Text("Default File Ordering");
			//char *orderlistcombo[] = {(char *)nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTASC_NAME).c_str(),(char *)nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTDESC_NAME).c_str(),(char *)nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTASC_DATE).c_str(),(char *)nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTDESC_DATE).c_str(),(char *)nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTASC_SIZE).c_str(),(char *)nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTDESC_SIZE).c_str()};
			std::vector<std::string> orderlistcombo = {nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTASC_NAME),nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTDESC_NAME),nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTASC_DATE),nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTDESC_DATE),nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTASC_SIZE),nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTDESC_SIZE)};
			//std::string fileorderingtext = orderlistcombo[configini->getsortOrder(true)];
			textsize = ImGui::CalcTextSize(orderlistcombo[configini->getsortOrder(true)].c_str());
			ImGui::SameLine(900*multiplyRes-textsize.x);
			ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			ImGui::Text(orderlistcombo[configini->getsortOrder(true)].c_str());
			ImGui::PopStyleColor();
			
			ImGui::Separator();
			if (ImGui::Selectable("##touchenablesel", false)){
				configini->setTouchEnable(!configini->getTouchEnable(true));
				//setEnableTouch(configini->getTouchEnable(true));
			}
			ImGui::SameLine();
			ImGui::Text(nxlangs::get_settingsmenu_str(nxlangs::NXSET_TOUCHCONTROL).c_str());
			std::string touchenabletext = configini->getTouchEnable(true)? nxlangs::get_common_str(nxlangs::NXCOMMON_YES).c_str():nxlangs::get_common_str(nxlangs::NXCOMMON_NO).c_str();
			textsize = ImGui::CalcTextSize(touchenabletext.c_str());
			ImGui::SameLine(900-textsize.x);
			if(configini->getTouchEnable(true)){
				ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			}else{
				ImGui::PushStyleColor(ImGuiCol_Text, Disabled_color);
			}
			ImGui::Text(touchenabletext.c_str());
			ImGui::PopStyleColor();
			
			ImGui::Separator();
			if (ImGui::Selectable("##ocstartupsel", false)){
				configini->setUseOc(!configini->getUseOc(true));
			}
			ImGui::SameLine();
			ImGui::Text(nxlangs::get_settingsmenu_str(nxlangs::NXSET_OVERCLOCK_STARTUP).c_str());
			std::string showocstartuptext = configini->getUseOc(true)? nxlangs::get_common_str(nxlangs::NXCOMMON_YES).c_str():nxlangs::get_common_str(nxlangs::NXCOMMON_NO).c_str();
			textsize = ImGui::CalcTextSize(showocstartuptext.c_str());
			ImGui::SameLine(900-textsize.x);
			if(configini->getUseOc(true)){
				ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			}else{
				ImGui::PushStyleColor(ImGuiCol_Text, Disabled_color);
			}
			ImGui::Text(showocstartuptext.c_str());
			ImGui::PopStyleColor();
			
			ImGui::Separator();
			
			if (ImGui::Selectable("##i18nsettingsel", false)){
				settingsview_combopopup = SETTINGSPOPUP_I18N;
			}
			ImGui::SameLine();
			ImGui::Text("I18N");
			std::string i18nsettingtext = nxlangs::GetCurrentLangName();
			textsize = ImGui::CalcTextSize(i18nsettingtext.c_str());
			ImGui::SameLine(900-textsize.x);
			ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			ImGui::Text(i18nsettingtext.c_str());
			ImGui::PopStyleColor();
			
			ImGui::Separator();
			if (ImGui::Selectable("##themecolorsel", false)){
				settingsview_combopopup = SETTINGSPOPUP_THEMECOLOR;
			}
			ImGui::SameLine();
			ImGui::Text("Theme Color");
			std::string themecolorsettingtext = (configini->getThemeColor(true) == 0) ? "Dark": (configini->getThemeColor(true) == 1) ? "Light" : "Auto";
			textsize = ImGui::CalcTextSize(themecolorsettingtext.c_str());
			ImGui::SameLine(900-textsize.x);
			ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			ImGui::Text(themecolorsettingtext.c_str());
			ImGui::PopStyleColor();
			
			ImGui::Separator();
			if (ImGui::Selectable("##exitmodesel", false)){
				settingsview_combopopup = SETTINGSPOPUP_EXITMODE;
			}
			ImGui::SameLine();
			ImGui::Text("Exit Behaviour");
			char * exitmodearray[] = {"Ask","Exit to Hb-Menu","Exit to Home"};
			std::string exitmodetext = exitmodearray[configini->getExitMode(true)];
			textsize = ImGui::CalcTextSize(exitmodetext.c_str());
			ImGui::SameLine(900-textsize.x);
			ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			ImGui::Text(exitmodetext.c_str());
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
			ImGui::Text(nxlangs::get_settingsmenu_str(nxlangs::NXSET_HWDECODER).c_str());
			std::string showhiddentext = configini->getHWDec(true)? nxlangs::get_common_str(nxlangs::NXCOMMON_YES).c_str():nxlangs::get_common_str(nxlangs::NXCOMMON_NO).c_str();
			ImVec2 textsize = ImGui::CalcTextSize(showhiddentext.c_str());
			ImGui::SameLine(900-textsize.x);
			if(configini->getHWDec(true)){
				ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			}else{
				ImGui::PushStyleColor(ImGuiCol_Text, Disabled_color);
			}
			ImGui::Text(showhiddentext.c_str());
			ImGui::PopStyleColor();
			ImGui::Separator();
			/*
			if (ImGui::Selectable("##audiooutdev", false  )){
				settingsview_combopopup = SETTINGSPOPUP_AUDIOOUT;
			}
			ImGui::SameLine();
			ImGui::Text(nxlangs::get_settingsmenu_str(nxlangs::NXSET_AUDIOOUTPUT).c_str());
			char *audiooutlist[] = {"SDL","Audren"};
			std::string curr_audioout = audiooutlist[configini->getAout(true)];
			textsize = ImGui::CalcTextSize(curr_audioout.c_str());
			ImGui::SameLine(900*multiplyRes-textsize.x);
			ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			ImGui::Text(curr_audioout.c_str());
			ImGui::PopStyleColor();
			*/
			ImGui::Separator();
			std::string curr_demuxcachestr = std::to_string(configini->getDemuxCache(true));
			if (ImGui::Selectable("##demuxcachesel", false  )){
				curr_demuxcachestr = ShowSWKeyboard(curr_demuxcachestr,nxlangs::get_settingsmenu_str(nxlangs::NXSET_DEMUXCACHE).c_str(),true);
				configini->setDemuxCache(atoi(curr_demuxcachestr.c_str()));
			}
			ImGui::SameLine();
			ImGui::Text(nxlangs::get_settingsmenu_str(nxlangs::NXSET_DEMUXCACHE).c_str());
			textsize = ImGui::CalcTextSize(curr_demuxcachestr.c_str());
			ImGui::SameLine(900*multiplyRes-textsize.x);
			ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			ImGui::Text(curr_demuxcachestr.c_str());
			ImGui::PopStyleColor();
			
			
			ImGui::Separator();
			std::string curr_shortseekstr = std::to_string(configini->getShortSeek(true));
			if (ImGui::Selectable("##shortseeksel", false  )){
				curr_shortseekstr = ShowSWKeyboard(curr_shortseekstr,nxlangs::get_settingsmenu_str(nxlangs::NXSET_SHORTSEEK).c_str(),true);
				configini->setShortSeek(atoi(curr_shortseekstr.c_str()));
			}
			ImGui::SameLine();
			ImGui::Text(nxlangs::get_settingsmenu_str(nxlangs::NXSET_SHORTSEEK).c_str());
			textsize = ImGui::CalcTextSize(curr_shortseekstr.c_str());
			ImGui::SameLine(900*multiplyRes-textsize.x);
			ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			ImGui::Text(curr_shortseekstr.c_str());
			ImGui::PopStyleColor();
			
			ImGui::Separator();
			std::string curr_longseekstr = std::to_string(configini->getLongSeek(true));
			if (ImGui::Selectable("##longseeksel", false  )){
				curr_longseekstr = ShowSWKeyboard(curr_longseekstr,nxlangs::get_settingsmenu_str(nxlangs::NXSET_LONGSEEK).c_str(),true);
				configini->setLongSeek(atoi(curr_longseekstr.c_str()));
			}
			ImGui::SameLine();
			ImGui::Text(nxlangs::get_settingsmenu_str(nxlangs::NXSET_LONGSEEK).c_str());
			textsize = ImGui::CalcTextSize(curr_longseekstr.c_str());
			ImGui::SameLine(900*multiplyRes-textsize.x);
			ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			ImGui::Text(curr_longseekstr.c_str());
			ImGui::PopStyleColor();
			
			
			ImGui::Separator();
			if (ImGui::Selectable("##usealangsel", false)){
				configini->setUseAlang(!configini->getUseAlang(true));
			}
			ImGui::SameLine();
			ImGui::Text(nxlangs::get_settingsmenu_str(nxlangs::NXSET_USEALANG).c_str());
			std::string curr_usealangstr = configini->getUseAlang(true)? nxlangs::get_common_str(nxlangs::NXCOMMON_YES).c_str():nxlangs::get_common_str(nxlangs::NXCOMMON_NO).c_str();
			textsize = ImGui::CalcTextSize(curr_usealangstr.c_str());
			ImGui::SameLine(900-textsize.x);
			if(configini->getUseAlang(true)){
				ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			}else{
				ImGui::PushStyleColor(ImGuiCol_Text, Disabled_color);
			}
			ImGui::Text(curr_usealangstr.c_str());
			ImGui::PopStyleColor();
			
			if(configini->getUseAlang(true)){
				if (ImGui::Selectable("##alangselcombo", false  )){
					settingsview_combopopup = SETTINGSPOPUP_ALANG;
				}
				ImGui::SameLine();
				ImGui::Text(nxlangs::get_settingsmenu_str(nxlangs::NXSET_ALANG).c_str());
				std::string curr_alang = Utility::getLanguages()[configini->getAlang(true)].lang3.c_str();
				textsize = ImGui::CalcTextSize(curr_alang.c_str());
				ImGui::SameLine(900*multiplyRes-textsize.x);
				ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
				ImGui::Text(curr_alang.c_str());
				ImGui::PopStyleColor();
			}
			
			ImGui::Separator();
			if (ImGui::Selectable("##useslangsel", false)){
				configini->setUseSlang(!configini->getUseSlang(true));
			}
			ImGui::SameLine();
			ImGui::Text(nxlangs::get_settingsmenu_str(nxlangs::NXSET_USESLANG).c_str());
			std::string curr_useslangstr = configini->getUseSlang(true)? nxlangs::get_common_str(nxlangs::NXCOMMON_YES).c_str():nxlangs::get_common_str(nxlangs::NXCOMMON_NO).c_str();
			textsize = ImGui::CalcTextSize(curr_useslangstr.c_str());
			ImGui::SameLine(900-textsize.x);
			if(configini->getUseSlang(true)){
				ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			}else{
				ImGui::PushStyleColor(ImGuiCol_Text, Disabled_color);
			}
			ImGui::Text(curr_useslangstr.c_str());
			ImGui::PopStyleColor();
			
			if(configini->getUseSlang(true)){
				if (ImGui::Selectable("##slangselcombo", false  )){
					settingsview_combopopup = SETTINGSPOPUP_SLANG;
				}
				ImGui::SameLine();
				ImGui::Text(nxlangs::get_settingsmenu_str(nxlangs::NXSET_SLANG).c_str());
				std::string curr_slang = Utility::getLanguages()[configini->getSlang(true)].lang3.c_str();
				textsize = ImGui::CalcTextSize(curr_slang.c_str());
				ImGui::SameLine(900*multiplyRes-textsize.x);
				ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
				ImGui::Text(curr_slang.c_str());
				ImGui::PopStyleColor();
			}
			
			ImGui::Separator();
			std::string curr_subfontsizestr = std::to_string(configini->getSubFontSize(true));
			if (ImGui::Selectable("##subfontsizesel", false  )){
				curr_subfontsizestr = ShowSWKeyboard(curr_subfontsizestr,nxlangs::get_settingsmenu_str(nxlangs::NXSET_SUBFONTSIZE).c_str(),true);
				configini->setSubFontSize(atoi(curr_subfontsizestr.c_str()));
			}
			ImGui::SameLine();
			ImGui::Text(nxlangs::get_settingsmenu_str(nxlangs::NXSET_SUBFONTSIZE).c_str());
			textsize = ImGui::CalcTextSize(curr_subfontsizestr.c_str());
			ImGui::SameLine(900*multiplyRes-textsize.x);
			ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			ImGui::Text(curr_subfontsizestr.c_str());
			ImGui::PopStyleColor();
			
			ImGui::Separator();
			std::string curr_subfontscalestr = std::to_string(configini->getSubFontScale(true));
			if (ImGui::Selectable("##subfontscalesel", false  )){
				curr_subfontscalestr = ShowSWKeyboard(curr_subfontscalestr,nxlangs::get_settingsmenu_str(nxlangs::NXSET_SUBFONTSCALE).c_str());
				configini->setSubFontScale(atof(curr_subfontscalestr.c_str()));
			}
			ImGui::SameLine();
			ImGui::Text(nxlangs::get_settingsmenu_str(nxlangs::NXSET_SUBFONTSCALE).c_str());
			textsize = ImGui::CalcTextSize(curr_subfontscalestr.c_str());
			ImGui::SameLine(900*multiplyRes-textsize.x);
			ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
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
			ImGui::Text(nxlangs::get_settingsmenu_str(nxlangs::NXSET_USEDATABASE).c_str());
			std::string usedbtext = configini->getDbActive(true)? nxlangs::get_common_str(nxlangs::NXCOMMON_YES).c_str():nxlangs::get_common_str(nxlangs::NXCOMMON_NO).c_str();
			ImVec2 textsize = ImGui::CalcTextSize(usedbtext.c_str());
			ImGui::SameLine(900-textsize.x);
			if(configini->getDbActive(true)){
				ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			}else{
				ImGui::PushStyleColor(ImGuiCol_Text, Disabled_color);
			}
			ImGui::Text(usedbtext.c_str());
			ImGui::PopStyleColor();
			if(sqlitedb != nullptr){
			ImGui::Separator();
				if(sqlitedb->getCorrupted()){
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),"Database File Corrupted");
				}else{
					ImGui::Text(nxlangs::get_settingsmenu_str(nxlangs::NXSET_DATABASEVERSION).c_str());
					ImVec2 textsize = ImGui::CalcTextSize(sqlitedb->getDbVersion().c_str());
					ImGui::SameLine(900-textsize.x);
					ImGui::Text(sqlitedb->getDbVersion().c_str());
					ImGui::Separator();
					ImGui::Text(nxlangs::get_settingsmenu_str(nxlangs::NXSET_SQLITEVERSION).c_str());
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
					ImGui::Text(nxlangs::get_settingsmenu_str(nxlangs::NXSET_RESUMEPOINTCOUNT).c_str());
					textsize = ImGui::CalcTextSize(recnumstring.c_str());
					ImGui::SameLine(900-textsize.x);
					ImGui::Text(recnumstring.c_str());
					ImGui::Separator();
					ImGui::Text(nxlangs::get_settingsmenu_str(nxlangs::NXSET_COMPLETEDCOUNT).c_str());
					textsize = ImGui::CalcTextSize(reccomstring.c_str());
					ImGui::SameLine(900-textsize.x);
					ImGui::Text(reccomstring.c_str());
					
					
						
				}
			}else{
				ImGui::Separator();
				ImGui::Text(nxlangs::get_settingsmenu_str(nxlangs::NXSET_DATABASENOTACTIVE).c_str());
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
		ImGui::Text(nxlangs::get_common_str(nxlangs::NXCOMMON_HOMESAVE).c_str());
		
		window->DrawList->AddLine(startpos,ImVec2(startpos.x+1280*multiplyRes,startpos.y) , ImGui::GetColorU32(Text_color), 1.0f);
		
		ImGui::EndChild();
		
		
	
		settingsview_laststate = settingsview_page;
		Windows::ExitWindow();
		if(settingsview_combopopup>-1){
			if(settingsview_combopopup == SETTINGSPOPUP_FILEORDERING){
				SetupComboPopup("##fileorderingcombo");
				//char *orderlistcombo[] = {(char *)nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTASC_NAME).c_str(),(char *)nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTDESC_NAME).c_str(),(char *)nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTASC_DATE).c_str(),(char *)nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTDESC_DATE).c_str(),(char *)nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTASC_SIZE).c_str(),(char *)nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTDESC_SIZE).c_str()};
				std::vector<std::string> orderlistcombo = {nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTASC_NAME),nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTDESC_NAME),nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTASC_DATE),nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTDESC_DATE),nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTASC_SIZE),nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTDESC_SIZE)};
				
				
				ImGui::PushStyleColor(ImGuiCol_PopupBg,Popup_Bg_color);
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, HeaderHover_color);
				ImGui::PushStyleColor(ImGuiCol_NavHighlight, Active_color);
		
				ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {20.0f*multiplyRes, 15.0f*multiplyRes});
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, ImGui::GetStyle().CellPadding.y * 2)); // Fix
    	
				if (ImGui::BeginPopupModal("##fileorderingcombo", nullptr, ImGuiWindowFlags_NoTitleBar)) {
					ImGui::Text("File Ordering");
					ImGui::Separator();
					
					for (int n = 0; n < orderlistcombo.size(); n++){
						ImGui::Dummy(ImVec2(300*multiplyRes,0));
						ImGui::SameLine();
						std::string ordercomboid = "##ordercombo" + std::to_string(n);
						if(ImGui::Selectable(ordercomboid.c_str(),false)){
							settingsview_combopopup = -1;
							configini->setsortOrder(n);
						}	
						ImGui::SameLine();
						ImGui::Text(orderlistcombo[n].c_str());	
						if(n  == configini->getsortOrder(true)){
							
							ImVec2 optstextsize = ImGui::CalcTextSize(orderlistcombo[n].c_str());
							ImGui::SameLine(1000*multiplyRes);
							ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
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
				//char *orderlistcombo[] = {(char *)nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTASC_NAME).c_str(),(char *)nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTDESC_NAME).c_str(),(char *)nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTASC_DATE).c_str(),(char *)nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTDESC_DATE).c_str(),(char *)nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTASC_SIZE).c_str(),(char *)nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTDESC_SIZE).c_str()};
				
				ImGui::PushStyleColor(ImGuiCol_PopupBg,Popup_Bg_color);
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, HeaderHover_color);
				ImGui::PushStyleColor(ImGuiCol_NavHighlight, Active_color);
		
				ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {20.0f*multiplyRes, 15.0f*multiplyRes});
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, ImGui::GetStyle().CellPadding.y * 2)); // Fix
    	
				if (ImGui::BeginPopupModal("##i18nsettingscombo", nullptr, ImGuiWindowFlags_NoTitleBar)) {
					ImGui::Text("I18N");
					ImGui::Separator();
					
					for (int n = 0; n < nxlangs::langslist.size(); n++){
						ImGui::Dummy(ImVec2(300*multiplyRes,0));
						ImGui::SameLine();
						std::string ordercomboid = "##i18nlangcombo" + std::to_string(n);
						if(ImGui::Selectable(ordercomboid.c_str(),false)){
							settingsview_combopopup = -1;
							configini->setInterfaceLang(nxlangs::langslist[n].filepath);
							nxlangs::selectedlang = n;
							
						}	
						ImGui::SameLine();
						ImGui::Text(nxlangs::langslist[n].name.c_str());	
						if(n  == configini->getConfigLangId(false)){
							
							ImVec2 optstextsize = ImGui::CalcTextSize(nxlangs::langslist[n].name.c_str());
							ImGui::SameLine(1000*multiplyRes);
							ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
							ImGui::Text(FONT_CHECKED_ICON);
							ImGui::PopStyleColor();
							
							
						}
					}
					
				} 
				ExitComboPopup();
				ImGui::PopStyleColor(3);
				ImGui::PopStyleVar(2);
			}else if(settingsview_combopopup == SETTINGSPOPUP_AUDIOOUT){
					
					
			}else if(settingsview_combopopup == SETTINGSPOPUP_ALANG){
				SetupComboPopup("##alangcombo");
				
				ImGui::PushStyleColor(ImGuiCol_PopupBg,Popup_Bg_color);
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, HeaderHover_color);
				ImGui::PushStyleColor(ImGuiCol_NavHighlight, Active_color);
		
				ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {20.0f*multiplyRes, 15.0f*multiplyRes});
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, ImGui::GetStyle().CellPadding.y * 2)); // Fix
    	
				if (ImGui::BeginPopupModal("##alangcombo", nullptr, ImGuiWindowFlags_NoTitleBar)) {
					ImGui::Text(nxlangs::get_settingsmenu_str(nxlangs::NXSET_ALANG).c_str());
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
							ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
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
				
				ImGui::PushStyleColor(ImGuiCol_PopupBg,Popup_Bg_color);
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, HeaderHover_color);
				ImGui::PushStyleColor(ImGuiCol_NavHighlight, Active_color);
		
				ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {20.0f*multiplyRes, 15.0f*multiplyRes});
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, ImGui::GetStyle().CellPadding.y * 2)); // Fix
    	
				if (ImGui::BeginPopupModal("##slangcombo", nullptr, ImGuiWindowFlags_NoTitleBar)) {
					ImGui::Text(nxlangs::get_settingsmenu_str(nxlangs::NXSET_SLANG).c_str());
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
							ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
							ImGui::Text(FONT_CHECKED_ICON);
							ImGui::PopStyleColor();
							
							
						}
					}
					
				} 
				ExitComboPopup();
				ImGui::PopStyleColor(3);
				ImGui::PopStyleVar(2);	
					
			}else if(settingsview_combopopup == SETTINGSPOPUP_THEMECOLOR){
				SetupComboPopup("##themecolorcombo");
				
				ImGui::PushStyleColor(ImGuiCol_PopupBg,Popup_Bg_color);
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, HeaderHover_color);
				ImGui::PushStyleColor(ImGuiCol_NavHighlight, Active_color);
		
				ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {20.0f*multiplyRes, 15.0f*multiplyRes});
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, ImGui::GetStyle().CellPadding.y * 2)); // Fix
    	
				if (ImGui::BeginPopupModal("##themecolorcombo", nullptr, ImGuiWindowFlags_NoTitleBar)) {
					ImGui::Text("AAAA");
					ImGui::Separator();
					char *themecolorlist[] = {"Dark","Light","Auto"};
					for (int n = 0; n < IM_ARRAYSIZE(themecolorlist); n++){
						ImGui::Dummy(ImVec2(300*multiplyRes,0));
						ImGui::SameLine();
						std::string ordercomboid = "##themecoloritem" + std::to_string(n);
						if(ImGui::Selectable(ordercomboid.c_str(),false)){
							settingsview_combopopup = -1;
							configini->setThemeColor(n);
							//SetColorTheme(configini->getThemeColor(true));
						}	
						ImGui::SameLine();
						ImGui::Text(themecolorlist[n]);	
						if(n  == configini->getThemeColor(true)){
							
							ImVec2 optstextsize = ImGui::CalcTextSize(themecolorlist[n]);
							ImGui::SameLine(1000*multiplyRes);
							ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
							ImGui::Text(FONT_CHECKED_ICON);
							ImGui::PopStyleColor();
							
							
						}
					}
					
				} 
				ExitComboPopup();
				ImGui::PopStyleColor(3);
				ImGui::PopStyleVar(2);	
					
			}else if(settingsview_combopopup == SETTINGSPOPUP_EXITMODE){
				SetupComboPopup("##exitmodecombo");
				
				ImGui::PushStyleColor(ImGuiCol_PopupBg,Popup_Bg_color);
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, HeaderHover_color);
				ImGui::PushStyleColor(ImGuiCol_NavHighlight, Active_color);
		
				ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {20.0f*multiplyRes, 15.0f*multiplyRes});
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, ImGui::GetStyle().CellPadding.y * 2)); // Fix
    	
				if (ImGui::BeginPopupModal("##exitmodecombo", nullptr, ImGuiWindowFlags_NoTitleBar)) {
					ImGui::Text(nxlangs::get_settingsmenu_str(nxlangs::NXSET_SLANG).c_str());
					ImGui::Separator();
					char *exitmodearray[] = {"Ask","Exit to Hb-Menu","Exit to Home"};
					for (int n = 0; n < IM_ARRAYSIZE(exitmodearray); n++){
						ImGui::Dummy(ImVec2(300*multiplyRes,0));
						ImGui::SameLine();
						std::string ordercomboid = "##exitmodeitem" + std::to_string(n);
						if(ImGui::Selectable(ordercomboid.c_str(),false)){
							settingsview_combopopup = -1;
							configini->setExitMode(n);
						}	
						ImGui::SameLine();
						ImGui::Text(exitmodearray[n]);	
						if(n  == configini->getExitMode(true)){
							
							ImVec2 optstextsize = ImGui::CalcTextSize(exitmodearray[n]);
							ImGui::SameLine(1000*multiplyRes);
							ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
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