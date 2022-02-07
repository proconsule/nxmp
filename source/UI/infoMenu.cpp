#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"


namespace Windows {
    void InfoMenuWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		if (ImGui::Begin("Info Menu Window", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
            char nxmptext[32];
			char ffmpegtext[32];
			char mpvtext[32];
			sprintf(nxmptext,"NXMP v%d.%d.%d - %s Version",VERSION_MAJOR,VERSION_MINOR,VERSION_MICRO,RELEASE_TYPE ? "Beta":"Stable");
			sprintf(ffmpegtext,"FFMPEG %s",libmpv->ffmpeg_version.c_str());
			sprintf(mpvtext,"MPV %s",libmpv->mpv_version.c_str());
			auto windowWidth = ImGui::GetWindowSize().x;
			ImGui::SetCursorPosX((windowWidth - nxmpicons.NXMPBannerTexture.width) * 0.5f);
			ImGui::Image((void*)(intptr_t)nxmpicons.NXMPBannerTexture.id, ImVec2(nxmpicons.NXMPBannerTexture.width,nxmpicons.NXMPBannerTexture.height));
			ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize(nxmptext, NULL, true).x) * 0.5f);
			ImGui::Text(nxmptext);
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0.0f, 20.0f));
			ImGui::SetCursorPosX((windowWidth - nxmpicons.FFMPEGTexture.width) * 0.5f);
			ImGui::Image((void*)(intptr_t)nxmpicons.FFMPEGTexture.id, ImVec2(nxmpicons.FFMPEGTexture.width,nxmpicons.FFMPEGTexture.height));
			ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize(ffmpegtext, NULL, true).x) * 0.5f);
			ImGui::Text(ffmpegtext);
			ImGui::SetCursorPosX((windowWidth - nxmpicons.MPVTexture.width) * 0.5f);
			ImGui::Image((void*)(intptr_t)nxmpicons.MPVTexture.id, ImVec2(nxmpicons.MPVTexture.width,nxmpicons.MPVTexture.height));
			ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize(mpvtext, NULL, true).x) * 0.5f);
			ImGui::Text(mpvtext);
			ImGui::Separator();
			ImGui::Text("Decoders List:");
			static int column_count = 2;
			if (ImGui::BeginTable("decoderTable", column_count, ImGuiTableFlags_ScrollY|ImGuiTableFlags_SizingFixedFit))
			{
				ImGui::TableSetupColumn("codec", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn("codecdesc", ImGuiTableColumnFlags_WidthStretch);
				std::vector<decoderlist_struct> mydecoderlist = libmpv->getDecoderList();
				for (int cell = 0; cell < mydecoderlist.size() * column_count; cell++)
				{
					ImGui::TableNextColumn();
					int column = ImGui::TableGetColumnIndex();
					int row = ImGui::TableGetRowIndex();

					ImGui::PushID(cell);
					if(column == 0){
						ImGui::Text("%s",mydecoderlist[row].codecname.c_str());
					}
					if(column == 1){
						ImGui::Text("%s",mydecoderlist[row].codecdesc.c_str());
					}
					
					ImGui::PopID();
				}
				ImGui::EndTable();
			}
		}	
		Windows::ExitWindow();
	}
}