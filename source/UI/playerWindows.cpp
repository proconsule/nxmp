#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "gui.h"
#include "playerwindows.h"
#include "imgui.h"
#include "utils.h"


#include "SwitchSys.h"
#include "imgui_toggle.h"
#include "imgui_toggle_presets.h"



namespace playerWindows{
	
	static int drag_wmax = 0;
	static int drag_hmax = 0;
	static int drag_w = 0;
	static int drag_h = 0;
	
	static int drag_brightness = 0;
	static int drag_contrast = 0;
	static int drag_saturation = 0;
	static int drag_gamma = 0;
	static int drag_hue = 0;
	static int rotateidx = 0;
	static int ignorestyleidx = 0;
	static int shaderidx = 0;
	static int interpolationidx = 0;
	
	
	
	static int drag_volume = 100;
	static float drag_audiodelay = 0.0f;
	
	static int drag_subpos = 100;
	static float drag_subdelay = 0.0f;
	static int drag_subfontbordersize = 3;
	static float drag_subborderblur = 0.0f;
	static int drag_shadowposition = 1;
	static float drag_shadowintensity = 0.25f;

	static int slider_eq[6] = {0,0,0,0,0,0};
	static char slider_hz[][8] = {"20-200","200-800","800-2K","2K-4K","4K-8K","20K"};
	static float slider_supereq[18] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
	static char slider_superhz[][6] = {"65","92","131","185","262","370","523","740","1K","1.4K","2K","2.9K","4K","5.9K","8.3K","11K","16K","20K"};
	
	static int supereqpresetsidx = 0;
	
	/*
	
	float playertextscrollpos = 0.0f;
	int waitpos = 0;
	
	
	bool playertextforwardscroll = true;
	bool playertextlaststate = true;
	
	*/
	
	bool decstashoverd = false;
	bool powerstashoverd = false;
	
	
	
	
	void PlayerProgressBar(float fraction1,float fraction2, const ImVec2& size_arg, const char* overlay)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = ImGui::CalcItemSize(size_arg, ImGui::CalcItemWidth(), g.FontSize + style.FramePadding.y * 2.0f);
		ImRect bb(pos, pos + size);
		ImGui::ItemSize(size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, 0))
			return;

		// Render
		fraction1 = ImSaturate(fraction1);
		//fraction2 = ImSaturate(fraction2);
		
		ImGui::RenderFrame(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);
		bb.Expand(ImVec2(-style.FrameBorderSize, -style.FrameBorderSize));
		const ImVec2 fill1_br = ImVec2(ImLerp(bb.Min.x, bb.Max.x, fraction1), bb.Max.y);
		ImGui::RenderRectFilledRangeH(window->DrawList, bb, ImGui::GetColorU32(ImVec4(1.0,1.0,1.0,0.5)), 0.0f, fraction1, style.FrameRounding);
		ImGui::RenderRectFilledRangeH(window->DrawList, bb, ImGui::GetColorU32(ImVec4(1.0,1.0,1.0,1.0)), 0.0f, fraction2, style.FrameRounding);
		


		// Default displaying the fraction as percentage string, but user can override it
		char overlay_buf[32];
		if (!overlay)
		{
			ImFormatString(overlay_buf, IM_ARRAYSIZE(overlay_buf), "%.0f%%", fraction1 * 100 + 0.01f);
			overlay = overlay_buf;
		}

		ImVec2 overlay_size = ImGui::CalcTextSize(overlay, NULL);
		if (overlay_size.x > 0.0f)
			ImGui::RenderTextClipped(ImVec2(ImClamp(fill1_br.x + style.ItemSpacing.x, bb.Min.x, bb.Max.x - overlay_size.x - style.ItemInnerSpacing.x), bb.Min.y), bb.Max, overlay, NULL, &overlay_size, ImVec2(0.0f, 0.5f), &bb);
	}
	
	
	void RightHomeWindow(bool *focus, bool *first_item){
		rightmenuposX = item.rightmenu_startpos;
		if(item.rightmenu_startpos>1080)item.rightmenu_startpos-=10;
		playerWindows::SetupRightWindow();
		std::vector<std::string> topmenu  = {"Tracks","Chapters","Aspect Ratio","Image","Audio","Subtitle","ShaderMania"};
		if (ImGui::Begin("Right Menu Home", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
			ImGui::SetNextWindowFocus();
			if (ImGui::BeginListBox("Right Menu Home List",ImVec2(1280.0f*multiplyRes, 720.0f*multiplyRes))){
				for (unsigned int n = 0; n < topmenu.size(); n++){
					static int selected = -1;
					if (ImGui::Selectable(topmenu[n].c_str(), selected == n)){
						if(topmenu[n] == "Tracks"){
							item.rightmenustate = PLAYER_RIGHT_MENU_TRACKS;
						}
						if(topmenu[n] == "Chapters"){
							item.rightmenustate = PLAYER_RIGHT_MENU_CHAPTERS;
						}
						if(topmenu[n] == "Aspect Ratio"){
							item.rightmenustate = PLAYER_RIGHT_MENU_ARATIO;
						}
						if(topmenu[n] == "Image"){
							item.rightmenustate = PLAYER_RIGHT_MENU_IMAGE;
						}
						if(topmenu[n] == "Audio"){
							item.rightmenustate = PLAYER_RIGHT_MENU_AUDIO;
						}
						if(topmenu[n] == "Subtitle"){
							item.rightmenustate = PLAYER_RIGHT_MENU_SUB;
						}
						if(topmenu[n] == "ShaderMania"){
							item.rightmenustate = PLAYER_RIGHT_MENU_SHADERMANIA;
						}
						/*if(topmenu[n] == "Anime4K v4.0.1"){
							item.rightmenustate = PLAYER_RIGHT_MENU_ANIME4K;
						}*/
					}
				}
				
				const ImVec4 green(0.16f, 0.66f, 0.45f, 1.0f);
				const ImVec4 green_hover(0.0f, 1.0f, 0.57f, 1.0f);
				const ImVec4 offgreen(0.31f, 0.39f, 0.52f, 1.0f);
				const ImVec4 offgreen_hover(0.48f, 0.57f, 0.70f, 1.0f);
				
				
				
				ImGui::PushStyleColor(ImGuiCol_Button, green);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, green_hover);
				
				ImVec4 color = ImGui::GetStyleColorVec4(ImGuiCol_Text);
				//ImVec4 colorMark = color;
				color.w *= 1.0f;
				
				if(powerstashoverd){
					ImGui::PushStyleColor(ImGuiCol_FrameBg, offgreen_hover);
					ImGui::PushStyleColor(ImGuiCol_Button, green_hover);
				}
				ImGui::Toggle("Power Stats", &item.showstats);
				if(powerstashoverd){
					ImGui::PopStyleColor(2);
				}
				
				if(ImGui::IsItemHovered()){
					powerstashoverd = true;
				}else{
					powerstashoverd	 = false;
				}
				
				if(decstashoverd){
					ImGui::PushStyleColor(ImGuiCol_FrameBg, offgreen_hover);
					ImGui::PushStyleColor(ImGuiCol_Button, green_hover);
					 
				}
				ImGui::Toggle("Dec Stats", &item.showdecstats);
				if(decstashoverd){
					ImGui::PopStyleColor(2);
				}
				if(ImGui::IsItemHovered()){
					decstashoverd = true;
				}else{
					decstashoverd = false;
				}
				
				ImGui::PopStyleColor(2);
				
				if (*first_item) {
					ImGui::SetFocusID(ImGui::GetID(topmenu[0].c_str()), ImGui::GetCurrentWindow());
					*first_item = false;
				}
				
				
				if(item.showstats)item.showdecstats=false;
				
				//battery
				
				ImGui::SetCursorPosY(ImGui::GetWindowSize().y -40*multiplyRes);
				ImGui::Separator();
				GUI::newbatteryIcon(ImVec2((rightmenuposX*multiplyRes) + 13.0f,ImGui::GetWindowSize().y -24*multiplyRes),true,batteryPercent,40*multiplyRes,18*multiplyRes,true);
				

				ImGui::EndListBox();
			}
			
		
		}
		playerWindows::ExitWindow();
	}
	void RightTrackWindow(bool *focus, bool *first_item){
		playerWindows::SetupRightWindow();
		std::vector<std::string> topmenu  = {"Video","Audio","Sub"};
		if (ImGui::Begin("Right Menu Tracks", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
			ImGui::SetNextWindowFocus();
			if (ImGui::BeginListBox("Tracks Menu",ImVec2(1280.0f*multiplyRes, 720.0f*multiplyRes))){
				for (unsigned int n = 0; n < topmenu.size(); n++){
					static int selected = -1;
					if (ImGui::Selectable(topmenu[n].c_str(), selected == n)){
						if(topmenu[n] == "Video"){
							item.rightmenustate = PLAYER_RIGHT_MENU_TRACKS_VIDEO;
						}
						if(topmenu[n] == "Audio"){
							item.rightmenustate = PLAYER_RIGHT_MENU_TRACKS_AUDIO;
						}
						if(topmenu[n] == "Sub"){
							item.rightmenustate = PLAYER_RIGHT_MENU_TRACKS_SUB;
						}
					}
				}
				if (*first_item) {
					ImGui::SetFocusID(ImGui::GetID(topmenu[0].c_str()), ImGui::GetCurrentWindow());
					*first_item = false;
				}
			}
			ImGui::EndListBox();
		}
		playerWindows::ExitWindow();
	}
	
	void RightTrackVideoWindow(bool *focus, bool *first_item){
		playerWindows::SetupRightWindow();
		if (ImGui::Begin("Right Menu Tracks Video", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
			ImGui::SetNextWindowFocus();
			if (ImGui::BeginListBox("Video Tracks Menu",ImVec2(1280.0f*multiplyRes, 720.0f*multiplyRes))){
				if(libmpv->getFileInfo() != nullptr){
				for (unsigned int n = 0; n < libmpv->getFileInfo()->videos.size(); n++){
					static int selected = -1;
					std::string itemid = "##" + std::to_string(n);
					float currstartpos = ImGui::GetCursorPosX();
					if (ImGui::Selectable(itemid.c_str(), selected == n,0,ImVec2(0, 50))){
						
					}
					ImGui::SetCursorPosX(currstartpos);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY()-50.0f);
					ImGui::Text("#%d %s",libmpv->getFileInfo()->videos[n].id,libmpv->getFileInfo()->videos[n].title.c_str());
					ImGui::Text("%dx%d",libmpv->getFileInfo()->videos[n].width,libmpv->getFileInfo()->videos[n].height);
					
				}
			}
			}
			ImGui::EndListBox();
		}

		playerWindows::ExitWindow();
	}
	
	void RightTrackAudioWindow(bool *focus, bool *first_item){
		playerWindows::SetupRightWindow();
		if (ImGui::Begin("Right Menu Tracks Audio", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
			ImGui::SetNextWindowFocus();
			if (ImGui::BeginListBox("Audio Tracks Menu",ImVec2(200.0f, 720.0f*multiplyRes))){
				if(libmpv->getFileInfo() != nullptr){
				for (unsigned int n = 0; n < libmpv->getFileInfo()->audios.size(); n++){
					std::string itemid = "##" + std::to_string(n);
					float currstartpos = ImGui::GetCursorPosX();
					if (ImGui::Selectable(itemid.c_str(), libmpv->getFileInfo()->audios[n].selected,0,ImVec2(0, 70))){
						libmpv->setAid(libmpv->getFileInfo()->audios[n].id,item.playershowcontrols);
					}
					ImGui::SameLine();
					ImGui::SetCursorPosX(currstartpos);
					ImGui::Text("#%d %s",libmpv->getFileInfo()->audios[n].id,libmpv->getFileInfo()->audios[n].title.c_str());
					ImGui::SetCursorPosY(ImGui::GetCursorPosY()-40.0f);
					ImGui::Text("%s",libmpv->getFileInfo()->audios[n].codec.c_str());
					ImGui::SameLine();
					ImGui::Text("%dch",libmpv->getFileInfo()->audios[n].channels);
					ImGui::Text("%s",libmpv->getFileInfo()->audios[n].language.c_str());
				}
				if (*first_item) {
						std::string itemid = "##" + std::to_string(0);
						ImGui::SetFocusID(ImGui::GetID(itemid.c_str()), ImGui::GetCurrentWindow());
						*first_item = false;
					}
			}
			}
			ImGui::EndListBox();
		}
		playerWindows::ExitWindow();
	}
	
	void RightTrackSubWindow(bool *focus, bool *first_item){
		playerWindows::SetupRightWindow();
		if (ImGui::Begin("Right Menu Tracks Subs", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
			ImGui::SetNextWindowFocus();
			if (ImGui::BeginListBox("Sub Tracks Menu",ImVec2(200.0f, 720.0f*multiplyRes))){
				if(libmpv->getFileInfo() != nullptr){
				for (unsigned int n = 0; n < libmpv->getFileInfo()->subtitles.size(); n++){
					static int selected = -1;
					std::string itemid = "##" + std::to_string(n);
					float currstartpos = ImGui::GetCursorPosX();
					if (ImGui::Selectable(itemid.c_str(), selected == n,0,ImVec2(0, 50))){
						libmpv->setSid(libmpv->getFileInfo()->subtitles[n].id,item.playershowcontrols);
					}
					ImGui::SameLine();
					ImGui::SetCursorPosX(currstartpos);
					ImGui::Text("#%d %s",libmpv->getFileInfo()->subtitles[n].id,libmpv->getFileInfo()->subtitles[n].title.c_str());
					ImGui::SetCursorPosY(ImGui::GetCursorPosY()-20.0f);
					ImGui::Text("%s",libmpv->getFileInfo()->subtitles[n].language.c_str());
		
				}
				if (*first_item) {
						std::string itemid = "##" + std::to_string(0);
						ImGui::SetFocusID(ImGui::GetID(itemid.c_str()), ImGui::GetCurrentWindow());
						*first_item = false;
					}
				}
			}
			ImGui::EndListBox();
		}
		playerWindows::ExitWindow();
	}
	
	void RightChapterWindow(bool *focus, bool *first_item){
		playerWindows::SetupRightWindow();
		std::vector<std::string> topmenu  = {"Video","Audio","Sub"};
		if (ImGui::Begin("Right Menu Chapters", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
			ImGui::SetNextWindowFocus();
			if (ImGui::BeginListBox("Chapter Menu",ImVec2(200.0f, 720.0f*multiplyRes))){
				if(libmpv->getFileInfo() != nullptr){
				for (unsigned int n = 0; n < libmpv->getFileInfo()->chapters.size(); n++){
					static int selected = -1;
					std::string itemid = "##" + std::to_string(n);
					float currstartpos = ImGui::GetCursorPosX();
					if (ImGui::Selectable(itemid.c_str(), selected == n,0,ImVec2(0, 50))){
						libmpv->seek(libmpv->getFileInfo()->chapters[n].time,item.playershowcontrols);
						
					}
					ImGui::SameLine();
					ImGui::SetCursorPosX(currstartpos);
					ImGui::Text("%s",libmpv->getFileInfo()->chapters[n].title.c_str());
					
				}
				if (*first_item) {
						std::string itemid = "##" + std::to_string(0);
						ImGui::SetFocusID(ImGui::GetID(itemid.c_str()), ImGui::GetCurrentWindow());
						*first_item = false;
					}
				}
			}
			ImGui::EndListBox();
		}
		playerWindows::ExitWindow();
	}
	
	void RightHomeARatio(bool *focus, bool *first_item){
		playerWindows::SetupRightWindow();
		std::vector<std::string> topmenu  = {"Default","16:9","16:10","4:3","Custom Ratio"};
		if (ImGui::Begin("Right Menu ARatio", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
			ImGui::SetNextWindowFocus();
			if (ImGui::BeginListBox("Aspect Ratio",ImVec2(200.0f, 720.0f*multiplyRes))){
				for (unsigned int n = 0; n < topmenu.size(); n++){
					static int selected = -1;
					if (ImGui::Selectable(topmenu[n].c_str(), selected == n)){
						if(n==0){
							libmpv->setAspectRatio(-1,item.playershowcontrols);
						}
						if(n==1){
							libmpv->setAspectRatio(1.777,item.playershowcontrols);
						}
						if(n==2){
							libmpv->setAspectRatio(1.6,item.playershowcontrols);
						}
						if(n==3){
							libmpv->setAspectRatio(1.333,item.playershowcontrols);
						}
						if(n==4){
							drag_wmax = libmpv->getVideoWidth();
							drag_hmax = libmpv->getVideoHeight();
							drag_w = libmpv->getVideoWidth();
							drag_h = libmpv->getVideoHeight();
							item.rightmenustate = PLAYER_RIGHT_MENU_CUSTOMARATIO;
						}
					}
				}
				if (*first_item) {
					ImGui::SetFocusID(ImGui::GetID(topmenu[0].c_str()), ImGui::GetCurrentWindow());
					*first_item = false;
				}
				ImGui::EndListBox();
			}
		}
		playerWindows::ExitWindow();
	}
	
	void RightHomeCustomARatio(bool *focus, bool *first_item){
		playerWindows::SetupRightWindow();
		std::vector<std::string> topmenu  = {"Default","16:9","16:10","4:3","Custom Ratio"};
		if (ImGui::Begin("Right Menu CustomAratio", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
				ImGui::PushItemWidth(200-10);
				ImGui::Text("Width");
				if(ImGui::DragInt("Width", &drag_w, 0.5f, 0, drag_wmax, "%d", ImGuiSliderFlags_NoInput)){
					double aratio = (double)drag_w/(double)drag_h;
					libmpv->setAspectRatio(aratio,item.playershowcontrols);
				}
				ImGui::Text("Height");
				if(ImGui::DragInt("Height", &drag_h, 0.5f, 0, drag_hmax, "%d", ImGuiSliderFlags_NoInput)){
					double aratio = (double)drag_w/(double)drag_h;
					libmpv->setAspectRatio(aratio,item.playershowcontrols);
				}
				ImGui::Text("Ratio: %.3f",(double)drag_w/(double)drag_h);
		}
		
		playerWindows::ExitWindow();
	}
	
	void RightHomeImage(bool *focus, bool *first_item){
		playerWindows::SetupRightWindow();
		std::vector<std::string> topmenu  = {"Default","16:9","16:10","4:3","Custom Ratio"};
		if (ImGui::Begin("Right Menu Image", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
				ImGui::PushItemWidth(200-10);
				auto windowWidth = ImGui::GetWindowSize().x;
				ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("Brightness", NULL, true).x) * 0.5f);
				ImGui::Text("Brightness");
				if(ImGui::DragInt("Brightness", &drag_brightness, 0.5f, -100, 100, "%d", ImGuiSliderFlags_NoInput)){
					libmpv->setBrightness(drag_brightness,item.playershowcontrols);
				}
				ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("Contrast", NULL, true).x) * 0.5f);
				ImGui::Text("Contrast");
				if(ImGui::DragInt("Contrast", &drag_contrast, 0.5f, -100, 100, "%d", ImGuiSliderFlags_NoInput)){
					libmpv->setContrast(drag_contrast,item.playershowcontrols);
				}
				ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("Saturation", NULL, true).x) * 0.5f);
				ImGui::Text("Saturation");
				if(ImGui::DragInt("Saturation", &drag_saturation, 0.5f, -100, 100, "%d", ImGuiSliderFlags_NoInput)){
					libmpv->setSaturation(drag_saturation,item.playershowcontrols);
				}
				ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("Gamma", NULL, true).x) * 0.5f);
				ImGui::Text("Gamma");
				if(ImGui::DragInt("Gamma", &drag_gamma, 0.5f, -100, 100, "%d", ImGuiSliderFlags_NoInput)){
					libmpv->setGamma(drag_gamma,item.playershowcontrols);
				}
				ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("Hue", NULL, true).x) * 0.5f);
				ImGui::Text("Hue");
				if(ImGui::DragInt("Hue", &drag_hue, 0.5f, -100, 100, "%d", ImGuiSliderFlags_NoInput)){
					libmpv->setHue(drag_hue,item.playershowcontrols);
				}
				
				ImGui::Dummy(ImVec2(0.0f, 20.0f));
				ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("Rotate", NULL, true).x) * 0.5f);
				ImGui::PushItemWidth(200-10);
				ImGui::Text("Rotate");
				std::vector<std::string> rotatemenu = {"0°","90°","180°","270°"};
				if (ImGui::BeginCombo("Image Rotate", rotatemenu[rotateidx].c_str(), 0))
				{	
					for (int n = 0; n < rotatemenu.size(); n++)
					{
						const bool is_selected = (rotateidx == n);
						if (ImGui::Selectable(rotatemenu[n].c_str(), is_selected)){
							if(n == 0){
								rotateidx = 0;
								libmpv->setRotate(-1,item.playershowcontrols);
							}
							if(n == 1){
								rotateidx = 1;
								libmpv->setRotate(90,item.playershowcontrols);
							}
							if(n == 2){
								rotateidx = 2;
								libmpv->setRotate(180,item.playershowcontrols);
							}
							if(n == 3){
								rotateidx = 3;
								libmpv->setRotate(270,item.playershowcontrols);
							}
						}
							

						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
					ImGui::PopItemWidth();
				}
				ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("Deinterlace", NULL, true).x) * 0.5f);
				ImGui::Text("Deinterlace");
				std::vector<std::string> deintmenu = {"No","Yes","Auto"};
				const char* combo_deintpreview_value = deintmenu[configini->getDeinterlace(true)].c_str();
				ImGui::PushItemWidth(190);
				if (ImGui::BeginCombo("Deinterlace", combo_deintpreview_value, 0))
				{	
					for (int n = 0; n < deintmenu.size(); n++)
					{
						const bool is_selected = (configini->getDeinterlace(true) == n);
						if (ImGui::Selectable(deintmenu[n].c_str(), is_selected)){
							configini->setDeinterlace(n);
							libmpv->setDeinterlace(n);
						}

						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				//New interpolation
				ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("Interpolation", NULL, true).x) * 0.5f);
				ImGui::PushItemWidth(200-10);
				ImGui::Text("Interpolation");
				std::vector<std::string> intermenu = {"No","Catmull_Rom","Mitchell","Bicubic","OverSample"};
				if (ImGui::BeginCombo("Interpolation", intermenu[interpolationidx].c_str(), 0))
				{	
					for (int n = 0; n < intermenu.size(); n++)
					{
						const bool is_selected = (interpolationidx == n);
						if (ImGui::Selectable(intermenu[n].c_str(), is_selected)){
							if(n == 0){
								interpolationidx = 0;
								mpv_command_string(libmpv->getHandle(),"set video-sync audio ; set interpolation no ; show-text \"Interpolation: ${interpolation}\nVideoSync: ${video-sync}\"");
							}
							if(n == 1){
								interpolationidx = 1;
								mpv_command_string(libmpv->getHandle(),"set video-sync display-resample ; set interpolation yes ; set tscale catmull_rom ; show-text \"Interpolation: ${interpolation}\nMethod: ${tscale}\nVideoSync: ${video-sync}\"");
							}
							if(n == 2){
								interpolationidx = 2;
								mpv_command_string(libmpv->getHandle(),"set video-sync display-resample ; set interpolation yes ; set tscale mitchell ; show-text \"Interpolation: ${interpolation}\nMethod: ${tscale}\nVideoSync: ${video-sync}\"");
							}
							if(n == 3){
								interpolationidx = 3;
								mpv_command_string(libmpv->getHandle(),"set video-sync display-resample ; set interpolation yes ; set tscale bicubic ; show-text \"Interpolation: ${interpolation}\nMethod: ${tscale}\nVideoSync: ${video-sync}\"");
							}
							if(n == 4){
								interpolationidx = 4;
								mpv_command_string(libmpv->getHandle(),"set video-sync display-resample ; set interpolation yes ; set tscale oversample ; show-text \"Interpolation: ${interpolation}\nMethod: ${tscale}\nVideoSync: ${video-sync}\"");
							}
						}
							

						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
					ImGui::PopItemWidth();
				}
				//end new interpolation
				
				
				ImGui::PopItemWidth();
				
				ImGui::SetCursorPosY(ImGui::GetWindowSize().y -50);
				if(ImGui::Button("Reset to Default")){
					drag_brightness = 0;
					drag_contrast = 0;
					drag_saturation = 0;
					drag_gamma = 0;
					drag_hue = 0;
					rotateidx = 0;
					interpolationidx = 0;
					mpv_command_string(libmpv->getHandle(),"set video-sync audio ; set interpolation no ; show-text \"Interpolation: ${interpolation}\nVideoSync: ${video-sync}\"");
					configini->setDeinterlace(configini->getDeinterlace(false));
					libmpv->setDeinterlace(configini->getDeinterlace(false));
					libmpv->setBrightness(drag_hue,false);
					libmpv->setContrast(drag_hue,false);
					libmpv->setSaturation(drag_hue,false);
					libmpv->setGamma(drag_hue,false);
					libmpv->setHue(drag_hue,false);
					libmpv->setRotate(-1,false);
				}
		}
		
		playerWindows::ExitWindow();
	}
	
	void RightHomeAudio(bool *focus, bool *first_item){
		playerWindows::SetupRightWindow();
		if (ImGui::Begin("Right Menu Audio", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
				ImGui::PushItemWidth(200-10);
				auto windowWidth = ImGui::GetWindowSize().x;
				ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("Volume", NULL, true).x) * 0.5f);
				ImGui::Text("Volume");
				drag_volume = libmpv->getVolume();
				if(ImGui::DragInt("Volume", &drag_volume, 0.5f, 0, 200, "%d", ImGuiSliderFlags_NoInput)){
					libmpv->setVolume(drag_volume,item.playershowcontrols);
				}
				
				bool audionrombool = libmpv->getAudioNormalize();
				if(ImGui::Checkbox("Normalize", &audionrombool)){
					libmpv->setAudioNormalize(audionrombool);
				}
				
				ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("Audio Delay", NULL, true).x) * 0.5f);
				ImGui::Text("Audio Delay");
				if(ImGui::DragFloat("Audio Delay", &drag_audiodelay, 0.100f, -5.0f, 5.0f, "%.3f", ImGuiSliderFlags_NoInput)){
					libmpv->setAudioDelay(drag_audiodelay,item.playershowcontrols);
				}
				ImGui::Dummy(ImVec2(0, 20));
				if(ImGui::Button("Audio EQ",ImVec2(190, 30))){
					item.rightmenustate = PLAYER_AUDIOEQ;
				}
				if(ImGui::Button("Audio Ex EQ",ImVec2(190, 30))){
					item.rightmenustate = PLAYER_SUPERAUDIOEQ;
				}
				ImGui::SetCursorPosY(ImGui::GetWindowSize().y -50);
				if(ImGui::Button("Reset to Default")){
					drag_volume = 100;
					drag_audiodelay = 0.0f;
					libmpv->setVolume(drag_volume,false);
					libmpv->setAudioDelay(drag_audiodelay,false);
					libmpv->setAudioNormalize(false);
				}
		}
		playerWindows::ExitWindow();
	}
	
	void RightHomeSub(bool *focus, bool *first_item){
		playerWindows::SetupRightWindow();
		if (ImGui::Begin("Right Menu Sub", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
				ImGui::PushItemWidth(200-10);
				auto windowWidth = ImGui::GetWindowSize().x;
				if(ImGui::Button("Reset to Default",ImVec2(190, 30))){
					drag_subpos = 100;
					drag_subdelay = 0.0f;
					initSize = configini->getSubFontSize(false);
					drag_subfontbordersize = 3;
					drag_shadowposition = 1;
					drag_shadowintensity = 0.25f;
					drag_subborderblur = 0.0f;
					ignorestyleidx = 0;
					initScale = configini->getSubFontScale(false);
					
					mpv_command_string(libmpv->getHandle(),"no-osd set sub-ass-override no ; no-osd seek 0");
					libmpv->setSubScaleSize(initScale,false);
					libmpv->setShadowOffset(drag_shadowposition,false);
					libmpv->setShadowIntensity(drag_shadowintensity,false);
					libmpv->setSubBorderSize(drag_subfontbordersize,false);
					libmpv->setSubBorderBlur(drag_subborderblur,false);
					libmpv->setSubPos(drag_subpos,false);
					libmpv->setSubDelay(drag_subdelay,false);
					libmpv->setSubFontSize(initSize,false);
					configini->setSubFontColor(configini->getSubFontColor(false));
					libmpv->setSubFontColor(configini->getSubFontColorHex(true));
					//bordercolor
					configini->setSubBorderColor(configini->getSubBorderColor(false));
					libmpv->setSubBorderColor(configini->getSubBorderColorHex(true));
					//endbordercolor
				}
				ImGui::Separator();
				ImGui::Dummy(ImVec2(0.0f,1.0f));
				//ignore styles
				ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("Styles Override", NULL, true).x) * 0.5f);
				ImGui::PushItemWidth(200-10);
				ImGui::Text("Styles Override");
				std::vector<std::string> stylemenu = {"No","Yes","Force","Scale","Strip"};
				if (ImGui::BeginCombo("Styles Override", stylemenu[ignorestyleidx].c_str(), 0))
				{	
					for (int n = 0; n < stylemenu.size(); n++)
					{
						const bool is_selected = (ignorestyleidx == n);
						if (ImGui::Selectable(stylemenu[n].c_str(), is_selected)){
							if(n == 0){
								ignorestyleidx = 0;
								mpv_command_string(libmpv->getHandle(),"no-osd set sub-ass-override no ; no-osd seek 0");
							}
							if(n == 1){
								ignorestyleidx = 1;
								mpv_command_string(libmpv->getHandle(),"no-osd set sub-ass-override yes ; no-osd seek 0");
							}
							if(n == 2){
								ignorestyleidx = 2;
								mpv_command_string(libmpv->getHandle(),"no-osd set sub-ass-override force ; no-osd seek 0");
							}
							if(n == 3){
								ignorestyleidx = 3;
								mpv_command_string(libmpv->getHandle(),"no-osd set sub-ass-override scale ; no-osd seek 0");
							}
							if(n == 4){
								ignorestyleidx = 4;
								mpv_command_string(libmpv->getHandle(),"no-osd set sub-ass-override strip ; no-osd seek 0");
							}
						}
							

						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
					ImGui::PopItemWidth();
				}
				//end ignore styles
				ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("Audio Delay", NULL, true).x) * 0.5f);
				ImGui::Text("Sub Delay");
				if(ImGui::DragFloat("Sub Delay", &drag_subdelay, 0.100f, -5.0f, 5.0f, "%.3f", ImGuiSliderFlags_NoInput)){
					libmpv->setSubDelay(drag_subdelay,item.playershowcontrols);
				}
				ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("Sub Position", NULL, true).x) * 0.5f);
				ImGui::Text("Sub Position");
				if(ImGui::DragInt("Sub Position", &drag_subpos, 0.5f, 0, 100, "%d", ImGuiSliderFlags_NoInput)){
					libmpv->setSubPos(drag_subpos,item.playershowcontrols);
				}
				//ImGui::BeginDisabled();
				ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("Sub Font Size", NULL, true).x) * 0.5f);
				ImGui::Text("Sub Font Size");
				if(ImGui::DragInt("Sub Font Size", &initSize, 0.5f, 1, 120, "%d", ImGuiSliderFlags_NoInput)){
					configini->setSubFontSize(initSize);
					libmpv->setSubFontSize(initSize,item.playershowcontrols);
				}
				//fontscale
				ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("Sub Font Scale", NULL, true).x) * 0.5f);
				ImGui::Text("Sub Font Scale");
				if(ImGui::DragFloat("Sub Font Scale", &initScale, 0.01f, 0.0f, 3.0f, "%.2f", ImGuiSliderFlags_NoInput)){
					configini->setSubFontScale(initScale);
					libmpv->setSubScaleSize(initScale,item.playershowcontrols);
				}
				//fontscale
				//bordersize
				ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("Sub Border Size", NULL, true).x) * 0.5f);
				ImGui::Text("Sub Border Size");
				if(ImGui::DragInt("Sub Border Size", &drag_subfontbordersize, 0.5f, 0, 8, "%d", ImGuiSliderFlags_NoInput)){
					libmpv->setSubBorderSize(drag_subfontbordersize,item.playershowcontrols);
				}
				//endbordersize
				//blursize
				ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("Blur Intensity", NULL, true).x) * 0.5f);
				ImGui::Text("Blur Intensity");
				if(ImGui::DragFloat("Blur Intensity", &drag_subborderblur, 0.050f, 0.0f, 5.0f, "%.2f", ImGuiSliderFlags_NoInput)){
					libmpv->setSubBorderBlur(drag_subborderblur,item.playershowcontrols);
				}
				//endblursize
				//shadowintensity
				ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("Shadow Intensity", NULL, true).x) * 0.5f);
				ImGui::Text("Shadow Intensity");
				if(ImGui::DragFloat("Shadow Intensity", &drag_shadowintensity, 0.010f, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_NoInput)){
					libmpv->setShadowIntensity(drag_shadowintensity,item.playershowcontrols);
				}
				//shadowintensity
				//shadowOffset
				ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("Shadow Position", NULL, true).x) * 0.5f);
				ImGui::Text("Shadow Position");
				if(ImGui::DragInt("Shadow Position", &drag_shadowposition, 0.5f, 0, 5, "%d", ImGuiSliderFlags_NoInput)){
					libmpv->setShadowOffset(drag_shadowposition,item.playershowcontrols);
				}
				//ShadowOffset
				ImGui::Text("Sub Font Color");
				float * subcolor = configini->getSubFontColor(true);
				if(ImGui::ColorButton("##subfontcolor", ImVec4(subcolor[0],subcolor[1],subcolor[2],subcolor[3]), ImGuiColorEditFlags_NoAlpha| ImGuiColorEditFlags_NoPicker|ImGuiColorEditFlags_InputRGB , ImVec2(190, 40))){
					item.popupstate = POPUP_STATE_SUBFONTCOLOR;
				}

				//bordercolor
				ImGui::Text("Sub Border Color");
				float * subcolor2 = configini->getSubBorderColor(true);
				if(ImGui::ColorButton("##subbordercolor", ImVec4(subcolor2[0],subcolor2[1],subcolor2[2],subcolor2[3]), ImGuiColorEditFlags_NoAlpha| ImGuiColorEditFlags_NoPicker|ImGuiColorEditFlags_InputRGB , ImVec2(190, 40))){
					item.popupstate = POPUP_STATE_SUBBORDERCOLOR;
				}
				//endbordercolor
				
				//ImGui::EndDisabled();
				//ImGui::SetCursorPosY(ImGui::GetWindowSize().y -50);
				
		}
		playerWindows::ExitWindow();
	}
	
	void AudioEqualizer(bool *focus, bool *first_item){
		playerWindows::SetupAudioEqWindow();
		float sliderpos[6];
		if (ImGui::Begin("Super EQ", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
			ImGui::BeginGroup();
            for (int y = 0; y < 6; y++)
            {
                ImGui::PushID(y);
				sliderpos[y] = ImGui::GetCursorPosX();
                if(ImGui::VSliderInt("##v", ImVec2(111.5f,150), &slider_eq[y], -10, 10, "%d dB")){
					libmpv->setAudioEQ(slider_eq,false);
				}
				ImGui::SameLine();
                ImGui::PopID();
            }
			ImGui::EndGroup();
			for (int y = 0; y < 6; y++){
				ImGui::SetCursorPosX(sliderpos[y] + ((111.5f - ImGui::CalcTextSize(slider_hz[y], NULL, true).x))*0.5f);
				ImGui::Text("%s",slider_hz[y]);
				ImGui::SameLine();
			}
			ImGui::Dummy(ImVec2(0.0f, 0.0f));
			if(ImGui::Button("Reset Default",ImVec2(710, 20))){
				for (int y = 0; y < 6; y++){
					slider_eq[y] = 0;
				}
				libmpv->setAudioEQ(slider_eq,false);
			}

		}
		playerWindows::ExitWindow();
	}
	
	void AudioSuperEqualizer(bool *focus, bool *first_item){
		playerWindows::SetupAudioEqWindow();
		float sliderpos[18];
		if (ImGui::Begin("Super Ex EQ", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
			//const ImVec2 small_slider_size(18, (float)(int)((160.0f - (rows - 1) * spacing) / rows));

			ImGui::BeginGroup();
            for (int y = 0; y < 18; y++)
            {
                ImGui::PushID(y);
				
				float colorid = 3.0;
				if(slider_supereq[y]> 3.0 && slider_supereq[y]< 5.0){
					colorid = 2.0;
				}else if(slider_supereq[y]>= 5.0){
					colorid = 1.0;
				}
				ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(colorid / 7.0f, 0.5f, 0.5f));
				ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(colorid / 7.0f, 0.6f, 0.5f));
				ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(colorid / 7.0f, 0.7f, 0.5f));
				ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(colorid / 7.0f, 0.9f, 0.9f));
				
				sliderpos[y] = ImGui::GetCursorPosX();
				std::string itemid = "##vsuperqslider" + std::to_string(y);
                if(ImGui::VSliderFloat(itemid.c_str(), ImVec2(32,150), &slider_supereq[y], 0.0f, 10.0f, "%.1f")){
					libmpv->setAudioSuperEQband(slider_supereq[y],y,false);
					//libmpv->setAudioSuperEQ(slider_supereq,false);
				}
				ImGui::PopStyleColor(4);
				ImGui::SameLine();
                ImGui::PopID();
            }
			ImGui::EndGroup();
			for (int y = 0; y < 18; y++){
				ImGui::SetCursorPosX(sliderpos[y] + ((32 - ImGui::CalcTextSize(slider_superhz[y], NULL, true).x))*0.5f);
				ImGui::Text("%s",slider_superhz[y]);
				ImGui::SameLine();
			}
			ImGui::Dummy(ImVec2(0.0f, 0.0f));
			if(ImGui::Button("Reset Default",ImVec2(710*0.5, 20))){
				for (int y = 0; y < 18; y++){
					slider_supereq[y] = 1;
				}
				supereqpresetsidx = 0;
				libmpv->setAudioSuperEQ(slider_supereq,false);
			}
			const char* combo_previewpreset_value = eqpreset->getPresets()[supereqpresetsidx].name.c_str();
			ImGui::PushItemWidth(710*0.5);
			ImGui::SameLine();
			if (ImGui::BeginCombo("Presets", combo_previewpreset_value, 0))
			{	
				for (int n = 0; n < eqpreset->getPresets().size(); n++)
				{
				const bool is_selected = (supereqpresetsidx == n);
				
				if (ImGui::Selectable(eqpreset->getPresets()[n].name.c_str(), is_selected)){
					for(int y=0;y<18;y++){
						slider_supereq[y] = eqpreset->getPresets()[n].eqvals[y];
					}
					libmpv->setAudioSuperEQ(slider_supereq,false);
					supereqpresetsidx = n;
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

		}
		playerWindows::ExitWindow();
	}
	
	const char * buffericon[] = {FONT_LOADING_ICON0,FONT_LOADING_ICON1,FONT_LOADING_ICON2,FONT_LOADING_ICON3,FONT_LOADING_ICON4,FONT_LOADING_ICON5,FONT_LOADING_ICON6,FONT_LOADING_ICON7};
	
	void CacheWindow(){
		playerWindows::SetupCacheWindow();
		if (ImGui::Begin("Caching", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
			auto windowWidth = ImGui::GetWindowSize().x;
			auto windowheight = ImGui::GetWindowSize().y;
			ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("Buffering Media - ", NULL, true).x) * 0.5f);
			ImGui::SetCursorPosY((windowheight - 24) * 0.5f);
			ImGui::Text("Buffering Media %s",buffericon[(int)(ImGui::GetTime() / 0.05f) & 3]);
		}
		playerWindows::ExitWindow();
	}
	
	void AudioplayerWindow(bool *focus, bool *first_item){
		playerWindows::SetupAudioPlayerWindow();
		if (ImGui::Begin("Audio Playback Window", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
			auto windowWidth = ImGui::GetWindowSize().x;
			auto windowheight = ImGui::GetWindowSize().y;
			ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("Audio Playback", NULL, true).x) * 0.5f);
			ImGui::SetCursorPosY((windowheight - 24) * 0.5f);
			ImGui::Text("Audio Playback");
			
		}
		playerWindows::ExitWindow();
	}
	
	void playerControls(){
		playerWindows::SetupPlayerControlsWindow();
		if(playerTextScroller == nullptr){
			playerTextScroller = new CTextScroller("##scrollerplayer");
		}
		static bool selected = -1;
		if (ImGui::Begin("Player Controls", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
			
			float centerposition = ImGui::GetWindowSize().x*0.5;
			
			/*
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0,1.0,1.0,0.0));
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.0,1.0,0.0,0.5));
			
			ImGui::ProgressBar(libmpv->getFileInfo()->playbackInfo.getplayPercCache(playercachesec), ImVec2(-1.0f, 10.0f*multiplyRes),"");
			ImGui::PopStyleColor(2);
			
			ImGui::SameLine();
			*/
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0,1.0,1.0,0.2));
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(1.0,1.0,1.0,1.0));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
			
			//ImGui::ProgressBar(libmpv->getFileInfo()->playbackInfo.getplayPerc(), ImVec2(-1.0f, 10.0f*multiplyRes),"");
			PlayerProgressBar(libmpv->getFileInfo()->playbackInfo.getplayPercCache(playercachesec),libmpv->getFileInfo()->playbackInfo.getplayPerc(), ImVec2(-1.0f, 10.0f*multiplyRes),"");
			
			
			if (ImGui::IsItemClicked()){
				ImGuiIO& io = ImGui::GetIO();
				double barperc = io.MousePos.x*100/1270;
				int64_t seek_val = (barperc*libmpv->getFileInfo()->playbackInfo.duration/100);
				libmpv->seek(seek_val,false);
				NXLOG::DEBUGLOG("Progressbar clicked perc %f %ld\n", barperc, seek_val);
			}
			ImGui::PopStyleColor(2);
			ImGui::PopStyleVar();
			ImGui::SetCursorPosX(centerposition-90*multiplyRes);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()+5.0f*multiplyRes);
			
			
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0,0.0,0.0,0.0));
			if (ImGui::Selectable("##Play", selected == 0,0,ImVec2(60*multiplyRes, 60*multiplyRes))){
				if(libmpv->Paused()){
					libmpv->Resume();
				}else{
					libmpv->Pause();
				}
			}
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SetCursorPosX(centerposition-90*multiplyRes);
			if(libmpv->Paused()){
				ImGui::Image((void*)(intptr_t)imgloader->icons.PlayIcon.id, ImVec2(60*multiplyRes,60*multiplyRes));
			}else{
				ImGui::Image((void*)(intptr_t)imgloader->icons.PauseIcon.id, ImVec2(60*multiplyRes,60*multiplyRes));
			}
			ImGui::SameLine();
			ImGui::SetCursorPosX(centerposition+20*multiplyRes);
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0,0.0,0.0,0.0));
			if (ImGui::Selectable("##Stop", selected == 0,0,ImVec2(60*multiplyRes, 60*multiplyRes))){
				libmpv->Stop();
			}
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowSize().x-100.0f);
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0,0.0,0.0,0.0));
			if (ImGui::Selectable("##Mute", selected == 0,0,ImVec2(60*multiplyRes, 60*multiplyRes))){
				libmpv->toggleMute();
				NXLOG::DEBUGLOG("MUTE: %d\n",libmpv->getMute());
			}
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowSize().x-100.0f);
			if(libmpv->getMute()){
				ImGui::Image((void*)(intptr_t)imgloader->icons.MuteIcon.id, ImVec2(60*multiplyRes,60*multiplyRes));
			}else{
				ImGui::Image((void*)(intptr_t)imgloader->icons.VolumeIcon.id, ImVec2(60*multiplyRes,60*multiplyRes));
			}
			
			ImGui::SameLine();
			ImGui::SetCursorPosX(centerposition+20*multiplyRes);
			ImGui::Image((void*)(intptr_t)imgloader->icons.StopIcon.id, ImVec2(60*multiplyRes,60*multiplyRes));
			ImGui::SameLine();
			
			ImGui::SetCursorPosX(centerposition+(centerposition*0.5));
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0,0.0,0.0,0.0));
			if (ImGui::Selectable("##Loop", selected == 0,0,ImVec2(60*multiplyRes, 60*multiplyRes))){
				libmpv->setLoop(!libmpv->getLoop());
			}
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SetCursorPosX(centerposition+(centerposition*0.5));
			if(libmpv->getLoop()){
				ImGui::Image((void*)(intptr_t)imgloader->icons.LoopIcon.id, ImVec2(60*multiplyRes,60*multiplyRes));
			}else{
				ImGui::Image((void*)(intptr_t)imgloader->icons.NoLoopIcon.id, ImVec2(60*multiplyRes,60*multiplyRes));
			}
			ImGui::SameLine();
			
			ImGui::SetCursorPosX(20);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()+10.0*multiplyRes);
			
			if(libmpv->getFileInfo()->playbackInfo.title ==""){
				//PlayerScrollText(centerposition-120,40,"%s",FS::getFilefromPath(libmpv->getFileInfo()->path).c_str());
				playerTextScroller->Draw("##scrollerplayer",centerposition-120,40,"%s",FS::getFilefromPath(libmpv->getFileInfo()->path).c_str());
			}else{
				if(libmpv->getFileInfo()->playbackInfo.artist ==""){
					playerTextScroller->Draw("##scrollerplayer",centerposition-120*multiplyRes,40*multiplyRes,"%s",libmpv->getFileInfo()->playbackInfo.title.c_str());
				}else{
					std::string titleandartist = libmpv->getFileInfo()->playbackInfo.title + std::string(" - ") + libmpv->getFileInfo()->playbackInfo.artist;
					playerTextScroller->Draw("##scrollerplayer",centerposition-120*multiplyRes,40*multiplyRes,"%s",titleandartist.c_str());
				}
			}
			ImGui::SetCursorPosX(20);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()-30.0f*multiplyRes);
			char timetext[64];
			sprintf(timetext,"%s - %s",Utility::formatTimeShort(libmpv->getFileInfo()->playbackInfo.position).c_str(),Utility::formatTimeShort(libmpv->getFileInfo()->playbackInfo.duration).c_str());
			ImGui::SetCursorPosX(20);
			ImGui::Text("%s",timetext);
			GUI::cloktimeText(ImVec2((1180.0f*multiplyRes)-ImGui::CalcTextSize(nxmpstats->currentTime).x-(10.0*multiplyRes),5.0f),true,nxmpstats->currentTime);
			GUI::newbatteryIcon(ImVec2(1180.0f*multiplyRes,5.0f),true,batteryPercent,40*multiplyRes,20*multiplyRes,true);
			
		}
		playerWindows::ExitControlsWindow();
	}
	
	void RightHomeShaderMania(){
		playerWindows::SetupRightWindow();
		std::vector<std::string> topmenu  = {"Default","16:9","16:10","4:3","Custom Ratio"};
		if (ImGui::Begin("Shader Mania", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
			ImGui::PushItemWidth(200-10);
				ImGui::Text("Shaders");
				if (ImGui::BeginCombo("Shaders Combo", shadermania->getCurrList()[shaderidx].name.c_str(), 0))
				{	
					for (int n = 0; n < shadermania->getCurrList().size(); n++)
					{
						const bool is_selected = (shaderidx == n);
						std::string itemid = "##" + std::to_string(n);
						if (ImGui::Selectable(itemid.c_str(), is_selected)){
							
							if(n == 0){
								#ifdef NXMP_SWITCH
								clockoc = false;
								SwitchSys::defaultClock(SwitchSys::stock_cpu_clock, SwitchSys::stock_gpu_clock, SwitchSys::stock_emc_clock); 
								#endif
								libmpv->clearShader();
							}else{
								NXLOG::DEBUGLOG("PATH: %s\n",shadermania->getCurrList()[n].path.c_str());
								libmpv->setShader(shadermania->getCurrList()[n].path);
							}
							shaderidx = n;
						}
						ImGui::SameLine();
						ImGui::Text("%s",shadermania->getCurrList()[n].name.c_str());
						
							

						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGuiContext& g = *GImGui;
					ImGuiWindow* window = g.CurrentWindow;
					ImGui::NavMoveRequestTryWrapping(window, ImGuiNavMoveFlags_LoopX);
					
					ImGui::EndCombo();
					
					ImGui::PopItemWidth();
				}	
		}
		
		playerWindows::ExitWindow();
	}
	
	void VolumeWindow(){
		playerWindows::SetupVolumeWindow();
		if (ImGui::Begin("Volume Window", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
			
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0,1.0,1.0,0.2));
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(1.0,1.0,1.0,1.0));
			//ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0,0.0,0.0,1.0));
			//ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.66f, 0.66f, 0.66f, 1.00f));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
			
			float oldsize = ImGui::GetFont()->Scale;
			ImGui::GetFont()->Scale *=2.0;
			ImGui::PushFont(ImGui::GetFont());
			ImGui::ProgressBar(libmpv->getVolume()/100.0, ImVec2(150.0, 25.0f),"");
			
			ImGui::SameLine();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()-10.0f*multiplyRes);
			ImGui::Text("%s",FONT_SPEAKER_ICON);
			ImGui::PopStyleColor(2);
			ImGui::PopStyleVar(2);
			ImGui::GetFont()->Scale = oldsize;
			ImGui::PopFont();
			//ImGui::Image((void*)(intptr_t)imgloader->icons.VolumeIcon.id, ImVec2(25,25));
			ImGuiContext& g = *GImGui;
			if(item.VolumeHide +2 < g.Time){
				item.showVolume = false;
			}
		
		}
		
		playerWindows::ExitVolumeWindow();
	}
	
	void StatsWindow(){
		playerWindows::SetupStatsWindow();
		if (ImGui::Begin("Stats Window", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0,1.0,1.0,0.2));
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(1.0,1.0,1.0,1.0));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
			ImGui::Text("CPU %s %s %s %s",nxmpstats->CPU_Hz_c ,nxmpstats->CPU_Usage0,nxmpstats->CPU_Usage1,nxmpstats->CPU_Usage2);
			ImGui::Text("GPU %s %s",nxmpstats->GPU_Hz_c ,nxmpstats->GPU_Load_c);
			ImGui::Text("%s",nxmpstats->Battery_c);
			
			ImGui::Text("%s",nxmpstats->loopstat_c);
			
			ImGui::PopStyleColor(2);
			ImGui::PopStyleVar();
			
		}
		
		playerWindows::ExitStatsWindow();
	}
	
	void DecodingStatsWindow(){
		playerWindows::SetupStatsWindow();
		if (ImGui::Begin("Decoding Stats Window", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0,1.0,1.0,0.2));
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(1.0,1.0,1.0,1.0));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
			if(libmpv->getFileInfo()->videos.size()>0){
				ImGui::Text("Video Codec: %s (%s)",nxmpstats->decodingstats.videodecstats.codec.c_str(),nxmpstats->decodingstats.videodecstats.hwdec.c_str()==std::string("tx1")?"HW (GPU)":nxmpstats->decodingstats.videodecstats.hwdec.c_str()==std::string("tx1-copy")?"HW (Copy)" : "SW");
				ImGui::Text("Video Resolution: %dx%d",nxmpstats->decodingstats.videodecstats.width,nxmpstats->decodingstats.videodecstats.height);
				ImGui::Text("Video Bitrate: %.0f kbps/s",nxmpstats->decodingstats.videodecstats.bitrate/1024.0);
				ImGui::Text("Pixel Format: %s Color Matrix: %s",nxmpstats->decodingstats.videodecstats.pixelformat.c_str(),nxmpstats->decodingstats.videodecstats.colormatrix.c_str());
				ImGui::Text("FPS: %.1f Frame drop count: %d",nxmpstats->decodingstats.videodecstats.fps,nxmpstats->decodingstats.videodecstats.framedropcount);
			}
			if(libmpv->getFileInfo()->audios.size()>0){
				ImGui::Separator();
				ImGui::Text("Audio Codec: %s",nxmpstats->decodingstats.audiodecstats.codec.c_str());
				ImGui::Text("Audio Bitrate: %.0f kbps/s",nxmpstats->decodingstats.audiodecstats.bitrate/1024.0);
				ImGui::Text("Audio Sample Rate: %ld Hz",nxmpstats->decodingstats.audiodecstats.samplerate);
				ImGui::Text("Audio Channels: %s",nxmpstats->decodingstats.audiodecstats.hrchannels.c_str());
			}
			
			ImGui::PopStyleColor(2);
			ImGui::PopStyleVar();
			
		}
		
		playerWindows::ExitStatsWindow();
	}
	
/*
	void PlayerScrollText(float w,float h,const char* fmt, ...){
		
		ImGui::BeginChild("##scrolling", ImVec2(w, h), false);
		va_list args;
		va_start(args, fmt);
		ImGui::TextV(fmt, args);
		va_end(args);
		
		float scroll_max_x = ImGui::GetScrollMaxX();
		if(playertextscrollpos>=scroll_max_x){
			playertextforwardscroll=false;
			if(!playertextlaststate==playertextforwardscroll){
				waitpos = 0;
			}
		}else if(playertextscrollpos<=0) {
			playertextforwardscroll=true;	
			if(!playertextlaststate==playertextforwardscroll){
				waitpos = 0;
			}
		}
		if(waitpos <= 60){
			waitpos++;
		}else{
			if(playertextforwardscroll){
				playertextscrollpos+=0.5;
			}else{
				playertextscrollpos-=0.5;
			}
		}
		
		playertextlaststate = playertextforwardscroll;
		ImGui::SetScrollX(playertextscrollpos);
		ImGui::EndChild();
	}
*/


}
