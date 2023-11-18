#ifndef NXMP_WINDOWS_H
#define NXMP_WINDOWS_H

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "UIHelpers.h"
#include "textscroller.h"

extern int newResW;
extern int newResH;
extern float multiplyRes;
extern bool isHandheld;

extern CTextScroller * BrowserTextScroller;
extern CTextScroller * playerTextScroller;

namespace Windows {

	extern int netwinselected;
	extern int lastthemeprevidx;
	
	
	extern bool settingsview_open;
	extern bool settingsview_page;
	extern int settingsview_combopopup;
	
	inline void SetupMainWindow(void) {
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(1280.0f*multiplyRes, 720.0f*multiplyRes), ImGuiCond_Once);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    };
	
	
    inline void SetupWindow(void) {
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(1280.0f*multiplyRes, 720.0f*multiplyRes), ImGuiCond_Once);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    };

    inline void ExitMainWindow(void) {
        ImGui::End();
		//ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    };
	inline void ExitWindow(void) {
        ImGui::End();
        ImGui::PopStyleVar();
    };
	
	void UniBrowserWindow(bool *focus, bool *first_item);
	void USBMountWindow(bool *focus, bool *first_item);

    void FileBrowserWindow(bool *focus, bool *first_item);
	//void USBBrowserWindow(bool *focus, bool *first_item);
	void EnigmaWindow(bool *focus, bool *first_item);
	void MainMenuWindow(bool *focus, bool *first_item);
	void ShareAddWindow(bool *focus, bool *first_item);
	void FtpWindow(bool *focus, bool *first_item);
	void HttpWindow(bool *focus, bool *first_item);
	void SSHWindow(bool *focus, bool *first_item);
	void SambaWindow(bool *focus, bool *first_item);
	void NFSWindow(bool *focus, bool *first_item);
	void NetworkWindow(bool *focus, bool *first_item);
	
	void UPNPBrowserWindow(bool *focus, bool *first_item);

	void InfoMenuWindow(bool *focus, bool *first_item);
	void VideoWindow(bool *focus, bool *first_item);
	void SettingsMenuWindow(bool *focus, bool *first_item);
	
	void PlaylistWindow(bool *focus, bool *first_item);
	void MTPServerWindow(bool *focus, bool *first_item);
    
	void SettingsUIWindow(bool *focus, bool *first_item);
	
	
	std::string ShowSWKeyboard(std::string InitialValueStr,std::string headertext = "",bool numeric = false);
	
}


#endif