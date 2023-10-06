#ifndef NXMP_APPPOPUPS_H
#define NXMP_APPPOPUPS_H

#include "imgui.h"
#include "textscroller.h"

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