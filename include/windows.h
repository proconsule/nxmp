#ifndef NXMP_WINDOWS_H
#define NXMP_WINDOWS_H

#include "imgui.h"

namespace Windows {
    inline void SetupWindow(void) {
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(1280.0f, 720.0f), ImGuiCond_Once);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    };

    inline void ExitWindow(void) {
        ImGui::End();
        ImGui::PopStyleVar();
    };

    void FileBrowserWindow(bool *focus, bool *first_item);
	void USBBrowserWindow(bool *focus, bool *first_item);
	void EnigmaWindow(bool *focus, bool *first_item);
	void MainMenuWindow(bool *focus, bool *first_item);
	void NetworkWindow(bool *focus, bool *first_item);
	void InfoMenuWindow(bool *focus, bool *first_item);
	void VideoWindow(bool *focus, bool *first_item);
	void SettingsMenuWindow(bool *focus, bool *first_item);
    
}

namespace PlayerWindows {
	inline void SetupWindow(void) {
        ImGui::SetNextWindowPos(ImVec2(0.0f, 600.0f), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(1280.0f, 120.0f), ImGuiCond_Once);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    };
	inline void ExitWindow(void) {
        ImGui::End();
        ImGui::PopStyleVar();
    };
	
	void PlayerControls();
	
}

#endif