#ifndef NXMP_PLAYERWINDOWS_H
#define NXMP_PLAYERWINDOWS_H

#include "imgui.h"
#include "gui.h"

extern int newResW;
extern int newResH;
extern float multiplyRes;
extern bool isHandheld;
namespace playerWindows{
	
	inline void SetupCacheWindow(void){
		ImGui::SetNextWindowPos(ImVec2(10.0f*multiplyRes, 600.0f*multiplyRes), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(250.0f*multiplyRes, 80.0f*multiplyRes), ImGuiCond_Once);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0,0.0,0.0,0.5));
	}
	
	inline void SetupVolumeWindow(void){
		ImGui::SetNextWindowPos(ImVec2(10.0f*multiplyRes, 10.0f*multiplyRes), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(200.0f, 50.0f), ImGuiCond_Once);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0,0.0,0.0,0.5));
	}
	
	static float rightmenuposX = 1280.0f;
	inline void SetupRightWindow(void) {
        if(isHandheld == true)
        ImGui::SetNextWindowPos(ImVec2(rightmenuposX, 0.0f), ImGuiCond_Always);
		else
		ImGui::SetNextWindowPos(ImVec2((rightmenuposX*multiplyRes)+100, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(200.0f, 720.0f*multiplyRes), ImGuiCond_Once);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0,0.0,0.0,0.5));
		
    };

	inline void SetupAudioEqWindow(void) {
        ImGui::SetNextWindowPos(ImVec2(277.0f*multiplyRes, 480.0f*multiplyRes), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(725.0f, 210.0f), ImGuiCond_Once);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0,0.0,0.0,0.5));
		
    };
	
	inline void SetupAudioPlayerWindow(void) {
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(1280.0f, 720.0f), ImGuiCond_Once);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0,0.0,0.0,0.5));
    };
	
	inline void SetupPlayerControlsWindow(void) {
        ImGui::SetNextWindowPos(ImVec2(0.0f, 620.0f*multiplyRes), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(1280.0f*multiplyRes, 720.0f*multiplyRes), ImGuiCond_Once);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0,0.0,0.0,0.5));
    };

    inline void ExitWindow(void) {
        ImGui::End();
        ImGui::PopStyleVar();
		ImGui::PopStyleColor();
    };
	
	inline void ExitVolumeWindow(void) {
        ImGui::End();
        ImGui::PopStyleVar(2);
		ImGui::PopStyleColor();
    };
	
	inline void ExitControlsWindow(void) {
        ImGui::End();
        ImGui::PopStyleVar(2);
		ImGui::PopStyleColor();
    };
	
	void RightHomeWindow(bool *focus, bool *first_item);
	void RightTrackWindow(bool *focus, bool *first_item);
	void RightChapterWindow(bool *focus, bool *first_item);
	void RightTrackVideoWindow(bool *focus, bool *first_item);
	void RightTrackAudioWindow(bool *focus, bool *first_item);
	void RightTrackSubWindow(bool *focus, bool *first_item);
	//void RightHomeAnime4K(bool *focus, bool *first_item);
	void RightHomeARatio(bool *focus, bool *first_item);
	void RightHomeCustomARatio(bool *focus, bool *first_item);
	void RightHomeImage(bool *focus, bool *first_item);
	void RightHomeAudio(bool *focus, bool *first_item);
	void RightHomeSub(bool *focus, bool *first_item);
	void AudioEqualizer(bool *focus, bool *first_item);
	void AudioSuperEqualizer(bool *focus, bool *first_item);
	
	void AudioplayerWindow(bool *focus, bool *first_item);
	
	
	void CacheWindow();
	
	void playerControls();
	
	void RightHomeShaderMania();
	void VolumeWindow();
	
}

#endif