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
			auto windowWidth = ImGui::GetWindowSize().x;
			ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("No Settings right now", NULL, true).x) * 0.5f);
			ImGui::Text("No Settings right now");
			
		}	
		Windows::ExitWindow();
	}
}