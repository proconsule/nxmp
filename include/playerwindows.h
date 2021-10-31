#ifndef NXMP_PLAYERWINDOWS_H
#define NXMP_PLAYERWINDOWS_H

#include "imgui.h"


namespace playerWindows{
	inline void SetupRightWindow(void) {
        ImGui::SetNextWindowPos(ImVec2(1080.0f, 0.0f), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(200.0f, 720.0f), ImGuiCond_Once);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0,0.0,0.0,0.5));
		
    };

    inline void ExitWindow(void) {
        ImGui::End();
        ImGui::PopStyleVar();
		ImGui::PopStyleColor();
    };
	
	
	
	void RightHomeWindow(bool *focus, bool *first_item);
	void RightTrackWindow(bool *focus, bool *first_item);
	void RightChapterWindow(bool *focus, bool *first_item);
	void RightTrackVideoWindow(bool *focus, bool *first_item);
	void RightTrackAudioWindow(bool *focus, bool *first_item);
	void RightTrackSubWindow(bool *focus, bool *first_item);
	void RightHomeARatio(bool *focus, bool *first_item);
	void RightHomeCustomARatio(bool *focus, bool *first_item);
	
}

#endif