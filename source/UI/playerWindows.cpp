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
	
	
	
	static int drag_volume = 100;
	static float drag_audiodelay = 0.0f;
				
	
	void RightHomeWindow(bool *focus, bool *first_item){
		playerWindows::SetupRightWindow();
		std::vector<std::string> topmenu  = {"Tracks","Chapters","Aspect Ratio","Image","Audio"};
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
				ImGui::SetCursorPosY(ImGui::GetWindowSize().y -50);
				if(ImGui::Button("Reset to Default")){
					drag_brightness = 0;
					drag_contrast = 0;
					drag_saturation = 0;
					drag_gamma = 0;
					drag_hue = 0;
					rotateidx = 0;
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
				if(ImGui::DragInt("Volume", &drag_volume, 0.5f, 0, 200, "%d", ImGuiSliderFlags_NoInput)){
					libmpv->setVolume(drag_volume,item.playershowcontrols);
				}
				ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("Audio Delay", NULL, true).x) * 0.5f);
				ImGui::Text("Audio Delay");
				if(ImGui::DragFloat("Audio Delay", &drag_audiodelay, 0.100f, -5.0f, 5.0f, "%.3f", ImGuiSliderFlags_NoInput)){
					libmpv->setAudioDelay(drag_audiodelay,item.playershowcontrols);
				}
				ImGui::SetCursorPosY(ImGui::GetWindowSize().y -50);
				if(ImGui::Button("Reset to Default")){
					drag_volume = 100;
					drag_audiodelay = 0.0f;
					libmpv->setVolume(drag_volume,false);
					libmpv->setAudioDelay(drag_audiodelay,false);
				}
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
			ImGui::Text("Buffering Media...");
		}
		playerWindows::ExitWindow();
	}		
}
