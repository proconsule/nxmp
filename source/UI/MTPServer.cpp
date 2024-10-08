#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"


namespace Windows {
    void MTPServerWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		if (ImGui::Begin("MPT Window", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
            ImGui::Text("MPT Server");
		}	
		Windows::ExitWindow();
	}
}