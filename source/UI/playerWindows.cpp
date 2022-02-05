#include "gui.h"
#include "playerwindows.h"
#include "imgui.h"
#include "utils.h"
#include "imgui_internal.h"


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
	static int shaderidx = 0;
	
	
	
	static int drag_volume = 100;
	static float drag_audiodelay = 0.0f;
	
	static int drag_subpos = 100;
	static float drag_subdelay = 0.0f;
	static int drag_subfontsize = 55;
	
	static int slider_eq[6] = {0,0,0,0,0,0};
	static char slider_hz[][8] = {"20-200","200-800","800-2K","2K-4K","4K-8K","20K"};
	static float slider_supereq[18] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
	static char slider_superhz[][6] = {"65","92","131","185","262","370","523","740","1K","1.4K","2K","2.9K","4K","5.9K","8.3K","11K","16K","20K"};
	
	static int supereqpresetsidx = 0;
	
	
	void RightHomeWindow(bool *focus, bool *first_item){
		rightmenuposX = item.rightmenu_startpos;
		if(item.rightmenu_startpos>1080)item.rightmenu_startpos-=10;
		playerWindows::SetupRightWindow();
		std::vector<std::string> topmenu  = {"Tracks","Chapters","Aspect Ratio","Image","Audio","Subtitle","ShaderMania"};
		if (ImGui::Begin("Right Menu Home", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
			ImGui::SetNextWindowFocus();
			if (ImGui::BeginListBox("Right Menu Home List",ImVec2(1280.0f, 720.0f))){
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
	void RightTrackWindow(bool *focus, bool *first_item){
		playerWindows::SetupRightWindow();
		std::vector<std::string> topmenu  = {"Video","Audio","Sub"};
		if (ImGui::Begin("Right Menu Tracks", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
			ImGui::SetNextWindowFocus();
			if (ImGui::BeginListBox("Tracks Menu",ImVec2(1280.0f, 720.0f))){
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
			if (ImGui::BeginListBox("Video Tracks Menu",ImVec2(1280.0f, 720.0f))){
				if(libmpv->getFileInfo() != nullptr){
				for (unsigned int n = 0; n < libmpv->getFileInfo()->videos.size(); n++){
					static int selected = -1;
					std::string itemid = "##" + std::to_string(n);
					float currstartpos = ImGui::GetCursorPosX();
					if (ImGui::Selectable(itemid.c_str(), selected == n,0,ImVec2(0, 50))){
						
					}
					ImGui::SetCursorPosX(currstartpos);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY()-50.0f);
					ImGui::PushFont(fontSmall);
					ImGui::Text("#%d %s",libmpv->getFileInfo()->videos[n].id,libmpv->getFileInfo()->videos[n].title.c_str());
					ImGui::Text("%dx%d",libmpv->getFileInfo()->videos[n].width,libmpv->getFileInfo()->videos[n].height);
					ImGui::PopFont();
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
			if (ImGui::BeginListBox("Audio Tracks Menu",ImVec2(200.0f, 720.0f))){
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
					ImGui::PushFont(fontSmall);
					ImGui::Text("%s",libmpv->getFileInfo()->audios[n].codec.c_str());
					ImGui::SameLine();
					ImGui::Text("%dch",libmpv->getFileInfo()->audios[n].channels);
					ImGui::Text("%s",libmpv->getFileInfo()->audios[n].language.c_str());
					ImGui::PopFont();
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
			if (ImGui::BeginListBox("Sub Tracks Menu",ImVec2(200.0f, 720.0f))){
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
					ImGui::PushFont(fontSmall);
					ImGui::Text("%s",libmpv->getFileInfo()->subtitles[n].language.c_str());
					ImGui::PopFont();
					
					
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
			if (ImGui::BeginListBox("Chapter Menu",ImVec2(200.0f, 720.0f))){
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
					ImGui::PushFont(fontSmall);
					ImGui::Text("%s",libmpv->getFileInfo()->chapters[n].title.c_str());
					ImGui::PopFont();
					
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
			if (ImGui::BeginListBox("Aspect Ratio",ImVec2(200.0f, 720.0f))){
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
				
				
				ImGui::PopItemWidth();
				
				ImGui::SetCursorPosY(ImGui::GetWindowSize().y -50);
				if(ImGui::Button("Reset to Default")){
					drag_brightness = 0;
					drag_contrast = 0;
					drag_saturation = 0;
					drag_gamma = 0;
					drag_hue = 0;
					rotateidx = 0;
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
				if(ImGui::DragInt("Sub Font Size", &drag_subfontsize, 0.5f, 1, 120, "%d", ImGuiSliderFlags_NoInput)){
					libmpv->setSubFontSize(drag_subfontsize,item.playershowcontrols);
				}
				
				ImGui::Text("Sub Font Color");
				float * subcolor = configini->getSubFontColor(true);
				if(ImGui::ColorButton("##subfontcolor", ImVec4(subcolor[0],subcolor[1],subcolor[2],subcolor[3]), ImGuiColorEditFlags_NoAlpha| ImGuiColorEditFlags_NoPicker|ImGuiColorEditFlags_InputRGB , ImVec2(190, 40))){
					item.popupstate = POPUP_STATE_SUBFONTCOLOR;
				}
				
				//ImGui::EndDisabled();
				ImGui::SetCursorPosY(ImGui::GetWindowSize().y -50);
				if(ImGui::Button("Reset to Default")){
					drag_subpos = 100;
					drag_subdelay = 0.0f;
					drag_subfontsize = configini->getSubFontSize(false);
					libmpv->setSubPos(drag_subpos,false);
					libmpv->setSubDelay(drag_subdelay,false);
					libmpv->setSubFontSize(drag_subfontsize,false);
					configini->setSubFontColor(configini->getSubFontColor(false));
					libmpv->setSubFontColor(configini->getSubFontColorHex(true));
				}
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
			ImGui::PushFont(fontSmall);
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
			ImGui::PopFont();
		
		}
		playerWindows::ExitWindow();
	}
	
	void AudioSuperEqualizer(bool *focus, bool *first_item){
		playerWindows::SetupAudioEqWindow();
		float sliderpos[18];
		if (ImGui::Begin("Super Ex EQ", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
			//const ImVec2 small_slider_size(18, (float)(int)((160.0f - (rows - 1) * spacing) / rows));
			ImGui::PushFont(fontSmall);
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
			ImGui::PopFont();
		}
		playerWindows::ExitWindow();
	}
	
	void CacheWindow(){
		playerWindows::SetupCacheWindow();
		if (ImGui::Begin("Caching", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
			auto windowWidth = ImGui::GetWindowSize().x;
			auto windowheight = ImGui::GetWindowSize().y;
			ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("Buffering Media...", NULL, true).x) * 0.5f);
			ImGui::SetCursorPosY((windowheight - 24) * 0.5f);
			ImGui::Text("Buffering Media %c","|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]);
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
		static bool selected = -1;
		if (ImGui::Begin("Player Controls", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar)) {
			
			float centerposition = ImGui::GetWindowSize().x*0.5;
			
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0,1.0,1.0,0.2));
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(1.0,1.0,1.0,1.0));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
			ImGui::ProgressBar(libmpv->getFileInfo()->playbackInfo.getplayPerc(), ImVec2(-1.0f, 10.0f),"");
			if (ImGui::IsItemClicked()){
				ImGuiIO& io = ImGui::GetIO();
				double barperc = io.MousePos.x*100/1270;
				int64_t seek_val = (barperc*libmpv->getFileInfo()->playbackInfo.duration/100);
				libmpv->seek(seek_val,false);
				printf("Progressbar clicked perc %f %ld\n", barperc, seek_val);
				fflush(stdout);
			}
			ImGui::PopStyleColor(2);
			ImGui::PopStyleVar();
			ImGui::SetCursorPosX(centerposition-90);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()+5.0f);
			
			
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0,0.0,0.0,0.0));
			if (ImGui::Selectable("##Play", selected == 0,0,ImVec2(60, 60))){
				if(libmpv->Paused()){
					libmpv->Resume();
				}else{
					libmpv->Pause();
				}
			}
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SetCursorPosX(centerposition-90);
			if(libmpv->Paused()){
				ImGui::Image((void*)(intptr_t)PlayIcon.id, ImVec2(60,60));
			}else{
				ImGui::Image((void*)(intptr_t)PauseIcon.id, ImVec2(60,60));
			}
			ImGui::SameLine();
			ImGui::SetCursorPosX(centerposition+20);
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0,0.0,0.0,0.0));
			if (ImGui::Selectable("##Stop", selected == 0,0,ImVec2(60, 60))){
				libmpv->Stop();
			}
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowSize().x-100.0f);
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0,0.0,0.0,0.0));
			if (ImGui::Selectable("##Mute", selected == 0,0,ImVec2(60, 60))){
				libmpv->toggleMute();
				printf("MUTE: %d\n",libmpv->getMute());
			}
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowSize().x-100.0f);
			if(libmpv->getMute()){
				ImGui::Image((void*)(intptr_t)MuteIcon.id, ImVec2(60,60));
			}else{
				ImGui::Image((void*)(intptr_t)VolumeIcon.id, ImVec2(60,60));
			}
			
			ImGui::SameLine();
			ImGui::SetCursorPosX(centerposition+20);
			ImGui::Image((void*)(intptr_t)StopIcon.id, ImVec2(60,60));
			ImGui::SameLine();
			
			ImGui::SetCursorPosX(centerposition+(centerposition*0.5));
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0,0.0,0.0,0.0));
			if (ImGui::Selectable("##Loop", selected == 0,0,ImVec2(60, 60))){
				libmpv->setLoop(!libmpv->getLoop());
			}
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::SetCursorPosX(centerposition+(centerposition*0.5));
			if(libmpv->getLoop()){
				ImGui::Image((void*)(intptr_t)LoopIcon.id, ImVec2(60,60));
			}else{
				ImGui::Image((void*)(intptr_t)NoLoopIcon.id, ImVec2(60,60));
			}
			ImGui::SameLine();
			
			ImGui::PushFont(fontSmall);
			ImGui::SetCursorPosX(20);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()+10.0);
			if(libmpv->getFileInfo()->playbackInfo.title ==""){
				ImGui::Text("%s",Utility::truncateLen(FS::getFilefromPath(libmpv->getFileInfo()->path),63).c_str());
			}else{
				if(libmpv->getFileInfo()->playbackInfo.artist ==""){
					ImGui::Text("%s",Utility::truncateLen(libmpv->getFileInfo()->playbackInfo.title.c_str(),63).c_str());
				}else{
					std::string titleandartist = libmpv->getFileInfo()->playbackInfo.title + std::string(" - ") + libmpv->getFileInfo()->playbackInfo.artist;
					ImGui::Text("%s",Utility::truncateLen(titleandartist,63).c_str());
				}
			}
			ImGui::SetCursorPosX(20);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()-40.0f);
			char timetext[64];
			sprintf(timetext,"%s - %s",Utility::formatTimeShort(libmpv->getFileInfo()->playbackInfo.position).c_str(),Utility::formatTimeShort(libmpv->getFileInfo()->playbackInfo.duration).c_str());
			ImGui::SetCursorPosX(20);
			ImGui::Text("%s",timetext);
			ImGui::PopFont();
			
			
			
			
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
								libmpv->clearShader();
							}else{
								printf("PATH: %s\n",shadermania->getCurrList()[n].path.c_str());
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
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
			ImGui::ProgressBar(libmpv->getVolume()/100.0, ImVec2(150.0, 25.0f),"");
			ImGui::PopStyleColor(2);
			ImGui::PopStyleVar();
			ImGui::SameLine();
			ImGui::Image((void*)(intptr_t)VolumeIcon.id, ImVec2(25,25));
			ImGuiContext& g = *GImGui;
			if(item.VolumeHide +2 < g.Time){
				item.showVolume = false;
			}
		
		}
		
		playerWindows::ExitVolumeWindow();
	}

}
