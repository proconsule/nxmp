#ifndef NXMP_APPPOPUPS_H
#define NXMP_APPPOPUPS_H

#include "imgui.h"
#include "textscroller.h"
#include "nxmp-gfx.h"

extern int newResW;
extern int newResH;
extern float multiplyRes;
extern bool isHandheld;

extern CTextScroller * FilePopupTextScroller;


namespace Popups {
    inline void SetupPopup(const char *id) {
        ImGui::OpenPopup(id);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowPos(ImVec2(640.0f*multiplyRes, 360.0f*multiplyRes), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    };
    
    inline void ExitPopup(void) {
        ImGui::PopStyleVar(2);
		ImGui::EndPopup();
        
    };
	
	
	inline void SetupNativePopup(const char *id) {
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		//ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
		
		
		ImGui::PushStyleColor(ImGuiCol_PopupBg,nxmpgfx::Popup_Bg_color);
		ImGui::PushStyleColor(ImGuiCol_Button,nxmpgfx::Button_color);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,nxmpgfx::ButtonActive_color);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered,nxmpgfx::NavHover_color);
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, nxmpgfx::NavHover_color);
		ImGui::PushStyleColor(ImGuiCol_NavHighlight, nxmpgfx::Active_color);
        ImGui::SetNextWindowPos(ImVec2(255.0f*multiplyRes, 214.0f*multiplyRes), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(771.0f*multiplyRes, 292.0f*multiplyRes), ImGuiCond_Once);
		ImGui::OpenPopup(id);
	};
    
    inline void ExitNativePopup(void) {
        ImGui::PopStyleVar(5);
		ImGui::PopStyleColor(6);
		ImGui::EndPopup();
        
    };
	

	void SaveSettingsPopup(void);
	void ResumePopup(void);
	void SubFontColorPopup(void);
    void SubBorderColorPopup(void);
	void PlaylistStartPlaylist(void);
	void DBUpdatedPopup(void);
	void NetMenuPopup(void);
	void FileContextPopup(void);
	void PlaylistContextPopup(void);
	void AppExitPopup(void);
	//void PopupScrollText(float w,float h,const char* fmt, ...);
	
}

#endif