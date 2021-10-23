#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "windows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"


namespace Windows {
    void InfoMenuWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		if (ImGui::Begin("Info Menu Window", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
            if (!*focus) {
                ImGui::SetNextWindowFocus();
                *focus = true;
            }
			char nxmptext[32];
			char ffmpegtext[32];
			char mpvtext[32];
			sprintf(nxmptext,"NXMP v%d.%d.%d",VERSION_MAJOR,VERSION_MINOR,VERSION_MICRO);
			sprintf(ffmpegtext,"FFMPEG %s",mpv->ffmpeg_version.c_str());
			sprintf(mpvtext,"MPV %s",mpv->mpv_version.c_str());
			auto windowWidth = ImGui::GetWindowSize().x;
			ImGui::SetCursorPosX((windowWidth - NXMPBannerTexture.width) * 0.5f);
			ImGui::Image((void*)(intptr_t)NXMPBannerTexture.id, ImVec2(NXMPBannerTexture.width,NXMPBannerTexture.height));
			ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize(nxmptext, NULL, true).x) * 0.5f);
			ImGui::Text(nxmptext);
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0f, 20.0f));
			ImGui::SetCursorPosX((windowWidth - FFMPEGTexture.width) * 0.5f);
			ImGui::Image((void*)(intptr_t)FFMPEGTexture.id, ImVec2(FFMPEGTexture.width,FFMPEGTexture.height));
			ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize(ffmpegtext, NULL, true).x) * 0.5f);
			ImGui::Text(ffmpegtext);
			ImGui::SetCursorPosX((windowWidth - MPVTexture.width) * 0.5f);
			ImGui::Image((void*)(intptr_t)MPVTexture.id, ImVec2(MPVTexture.width,MPVTexture.height));
			ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize(mpvtext, NULL, true).x) * 0.5f);
			ImGui::Text(mpvtext);
			
		}	
		Windows::ExitWindow();
	}
}