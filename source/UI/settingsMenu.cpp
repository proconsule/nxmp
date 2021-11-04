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
				if (ImGui::BeginTabItem("Player Settings"))
                {
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
				item.state = MENU_STATE_HOME;
			}
		}	
		Windows::ExitWindow();
	}
}