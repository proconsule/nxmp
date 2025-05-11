#ifndef NXMP_WINDOWS_H
#define NXMP_WINDOWS_H

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "UIHelpers.h"
#include "textscroller.h"
#include "globals.h"
#include "nxmp-render.h"

extern int newResW;
extern int newResH;
extern float multiplyRes;
extern bool isHandheld;

extern CTextScroller * BrowserTextScroller;
extern CTextScroller * playerTextScroller;

namespace Windows {

#ifdef DEKO3D_BACKEND
	extern Texture currentImg;
#endif
#ifdef OPENGL_BACKEND
	extern Tex currentImg;
#endif		


	extern int netwinselected;
	extern int lastthemeprevidx;
	
	
	extern bool settingsview_open;
	extern bool settingsview_page;
	extern int settingsview_combopopup;
	
	inline void SetupMainWindow(void) {
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(1280.0f*multiplyRes, 720.0f*multiplyRes), ImGuiCond_Always);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg,Window_Bg_color);
    };
	
	
    inline void SetupWindow(void) {
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(1280.0f*multiplyRes, 720.0f*multiplyRes), ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg,Window_Bg_color);
    };

    inline void ExitMainWindow(void) {
        ImGui::End();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
        
    };
	inline void ExitWindow(void) {
        ImGui::End();
        ImGui::PopStyleVar();
		ImGui::PopStyleColor();
    };
	
	void UniBrowserWindow(/*bool *focus, bool *first_item*/);
	void UniUPNPServersWindow(/*bool *focus, bool *first_item*/);
	void UniUPNPBrowserWindow(/*bool *focus, bool *first_item*/);
	void SetBrowserNav(std::string foldername);
	
	void USBMountWindow(bool *focus, bool *first_item);

    void FileBrowserWindow(bool *focus, bool *first_item);
	void EnigmaWindow(bool *focus, bool *first_item);
	void MainMenuWindow(bool *focus, bool *first_item);
	void ShareAddWindow(bool *focus, bool *first_item);
	void NewShareWindow();
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
	
	void imageViewer();
	void setImageZoom(float zoomfactor);
	float getImageZoom();
	
	extern bool loadimage;
	
	std::string ShowSWKeyboard(std::string InitialValueStr,std::string headertext = "",bool numeric = false);
	
}


#endif