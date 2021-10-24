#include "gui.h"
#include "windows.h"
#include "imgui.h"
#include "utils.h"
#include "imgui_internal.h"


namespace PlayerWindows{
	
	void PlayerControls(){
		PlayerWindows::SetupWindow();
		if (ImGui::Begin("Player Controls", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoSavedSettings)) {
			
		}
		
		
		Windows::ExitWindow();
	}
	
	
	
}