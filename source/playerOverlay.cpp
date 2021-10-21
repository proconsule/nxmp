#include "gui.h"
#include "windows.h"
#include "imgui.h"
#include "utils.h"
#include "imgui_internal.h"


namespace PlayerWindows{
	
	void PlayerControls(){
		PlayerWindows::SetupWindow();
		if (ImGui::Begin("Player Controls", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoSavedSettings)) {
			int pos = mpv->getPercentPos();
			float progress = (float)pos/100.0;
			ImGui::Text("%s/%s",Utility::formatTimeShort(mpv->getPosition()).c_str(),Utility::formatTimeShort(mpv->getDuration()).c_str());
			ImGui::SameLine();
			ImGui::ProgressBar(progress, ImVec2(-1, 0));
		}
		
		
		Windows::ExitWindow();
	}
	
	
	
}