#include "apppopups.h"
#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "utils.h"

namespace Popups{
	
	void SaveSettingsPopup(void) {
		Popups::SetupPopup("Save Settings");

		if (ImGui::BeginPopupModal("Save Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			
			ImGui::Text("Save Settings?");
               
			ImVec2 button_size(ImGui::GetFontSize() * 7.0f, 0.0f);
            if (ImGui::Button("Yes", button_size))
            {
				item.state = MENU_STATE_HOME;
				item.popupstate = POPUP_STATE_NONE;
				configini->saveSettings();
					
				}
				ImGui::SameLine();
				if (ImGui::Button("No", button_size))
				{
                    
					item.state = MENU_STATE_HOME;
					item.popupstate = POPUP_STATE_NONE;
					ImGui::CloseCurrentPopup();
                }
			
		}
		Popups::ExitPopup();
	}
	
}