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
			//auto windowWidth = ImGui::GetWindowSize().x;
			//ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("No Settings right now", NULL, true).x) * 0.5f);
			//static int counter = configini->getShortSeek();
			float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
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
				item.state = MENU_STATE_HOME;
			}
			
			

		}	
		Windows::ExitWindow();
	}
}