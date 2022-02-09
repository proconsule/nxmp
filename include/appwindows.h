#ifndef NXMP_WINDOWS_H
#define NXMP_WINDOWS_H

#include "imgui.h"
#include "platforms.h"

extern int newResW;
extern int newResH;
extern float multiplyRes;
extern bool isHandheld;
namespace Windows {
	
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

    void FileBrowserWindow(bool *focus, bool *first_item);
#ifdef NXMP_USBSUPPORT
	void USBBrowserWindow(bool *focus, bool *first_item);
#endif
#ifdef NXMP_ENIGMASUPPORT
	void EnigmaWindow(bool *focus, bool *first_item);
#endif
	void MainMenuWindow(bool *focus, bool *first_item);
#ifdef NXMP_NETWORKSUPPORT
	void FtpWindow(bool *focus, bool *first_item);
	void HttpWindow(bool *focus, bool *first_item);
	void SSHWindow(bool *focus, bool *first_item);
	void SambaWindow(bool *focus, bool *first_item);
	void NFSWindow(bool *focus, bool *first_item);
	void NetworkWindow(bool *focus, bool *first_item);
#endif
#ifdef NXMP_UPNPSUPPORT
	void UPNPBrowserWindow(bool *focus, bool *first_item);
#endif
	void InfoMenuWindow(bool *focus, bool *first_item);
	void VideoWindow(bool *focus, bool *first_item);
	void SettingsMenuWindow(bool *focus, bool *first_item);
	
	void PlaylistWindow(bool *focus, bool *first_item);
	
    
}


#endif