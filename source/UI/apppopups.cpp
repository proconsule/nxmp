#include "apppopups.h"
#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "utils.h"

namespace Popups{
	
	void PlaylistStartPlaylist(void) {
		Popups::SetupPopup("Playlist Start");
		if (ImGui::BeginPopupModal("Playlist Start", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		if(playlist->getPlaylist().size()>0){
				ImGui::Text("Start Playlist?");
				ImVec2 button_size(ImGui::GetFontSize() * 7.0f, 0.0f);
				if (ImGui::Button("Yes", button_size)) {
					if(item.state == MENU_STATE_FILEBROWSER || item.state == MENU_STATE_USB ||item.state == MENU_STATE_FTPBROWSER || item.state == MENU_STATE_HTTPBROWSER || item.state == MENU_STATE_SSHBROWSER || item.state == MENU_STATE_SAMBABROWSER || item.state == MENU_STATE_NFSBROWSER){
						item.popupstate = POPUP_STATE_NONE;
						Playlist::playlist_struct nextfile = playlist->getPlaylist()[0];
						libmpv->loadFile(nextfile.fulluri);
						if(configini->getDbActive(true)){
							libmpv->getFileInfo()->resume = sqlitedb->getResume(nextfile.fulluri);
							if(libmpv->getFileInfo()->resume>0){
								item.popupstate = POPUP_STATE_RESUME;
							}
						}
					}
				
				
				}
				ImGui::SameLine();
				if (ImGui::Button("No", button_size)) {
					item.popupstate = POPUP_STATE_NONE;
				}
			}else{
				ImGui::Text("Empty playlist");
				ImVec2 button_size(ImGui::GetFontSize() * 7.0f, 0.0f);
				if (ImGui::Button("OK", button_size)) {
					item.popupstate = POPUP_STATE_NONE;
				}
				ImGui::SetFocusID(ImGui::GetID("OK"), ImGui::GetCurrentWindow()); 
				ImGuiContext& g = *ImGui::GetCurrentContext();
				g.NavDisableHighlight = false;

			}
		}
		Popups::ExitPopup();
		
	}
	
	void SaveSettingsPopup(void) {
		Popups::SetupPopup("Save Settings");

		if (ImGui::BeginPopupModal("Save Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			
			ImGui::Text("Save Settings?");
               
			ImVec2 button_size(ImGui::GetFontSize() * 7.0f, 0.0f);
            if (ImGui::Button("Yes", button_size))
            {
				item.state = MENU_STATE_HOME;
				item.popupstate = POPUP_STATE_NONE;
				configini->saveSettings();
					
				}
				ImGui::SameLine();
				if (ImGui::Button("No", button_size))
				{
                    
					item.state = MENU_STATE_HOME;
					item.popupstate = POPUP_STATE_NONE;
					ImGui::CloseCurrentPopup();
                }
			
		}
		Popups::ExitPopup();
	}
	
	void ResumePopup(void) {
		Popups::SetupPopup("Resume Play");

		if (ImGui::BeginPopupModal("Resume Play", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			
			ImGui::Text("Resume Playback?");
			
			ImGui::Text("Resume Playback @ %s",Utility::formatTimeShort(libmpv->getFileInfo()->resume).c_str());
               
			ImVec2 button_size(ImGui::GetFontSize() * 7.0f, 0.0f);
            if (ImGui::Button("Yes", button_size))
            {
				libmpv->seek(libmpv->getFileInfo()->resume,false);
				item.popupstate = POPUP_STATE_NONE;
				ImGui::CloseCurrentPopup();
					
			}
			ImGui::SameLine();
			if (ImGui::Button("No", button_size))
			{
                    
				
				item.popupstate = POPUP_STATE_NONE;
				ImGui::CloseCurrentPopup();
             }
			
		}
		Popups::ExitPopup();
	}
	
	static ImVec4 backup_color;
	static ImVec4 color;	
	static bool saved_palette_init = true;
	static ImVec4 saved_palette[32] = {};
        
	
	void SubFontColorPopup(void) {
		if (saved_palette_init)
        {
            for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
            {
                ImGui::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f,
                    saved_palette[n].x, saved_palette[n].y, saved_palette[n].z);
                saved_palette[n].w = 1.0f; // Alpha
            }
            saved_palette_init = false;
        }
		ImGui::SetNextWindowPos(ImVec2(0,0));
		ImGui::OpenPopup("subfontcolorpicker");
		
		if (ImGui::BeginPopup("subfontcolorpicker")) {
				float * tmpcolor = configini->getSubFontColor(true);
				float * tmpcolor_backup = configini->getSubFontColor(false);
				color = ImVec4(tmpcolor[0],tmpcolor[1],tmpcolor[2],tmpcolor[3]);
				backup_color = ImVec4(tmpcolor_backup[0],tmpcolor_backup[1],tmpcolor_backup[2],tmpcolor_backup[3]);
				ImGui::Separator();
				if(ImGui::ColorPicker3("##picker", (float*)&color,ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview)){
					float setcolor[4];
					setcolor[0] = color.x;
					setcolor[1] = color.y;
					setcolor[2] = color.z;
					setcolor[3] = color.w;
					configini->setSubFontColor(setcolor);
				}
				ImGui::SameLine();

				ImGui::BeginGroup(); // Lock X position
				ImGui::Text("Current");
				ImGui::ColorButton("##current", color, ImGuiColorEditFlags_NoAlpha| ImGuiColorEditFlags_NoPicker  , ImVec2(60, 40));
				ImGui::Text("Previous");
				if (ImGui::ColorButton("##previous", backup_color, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker , ImVec2(60, 40))){
					float setcolor[4];
					setcolor[0] = backup_color.x;
					setcolor[1] = backup_color.y;
					setcolor[2] = backup_color.z;
					setcolor[3] = backup_color.w;
					configini->setSubFontColor(setcolor);
				}
				ImGui::Separator();
				ImGui::Text("Palette");
				for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
				{
					ImGui::PushID(n);
					if ((n % 8) != 0)
						ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);

					ImGuiColorEditFlags palette_button_flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip;
					if (ImGui::ColorButton("##palette", saved_palette[n], palette_button_flags, ImVec2(20, 20))){
						 //color = ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, color.w); // Preserve alpha!
						 float setcolor[4];
						 setcolor[0] = saved_palette[n].x;
						 setcolor[1] = saved_palette[n].y;
						 setcolor[2] = saved_palette[n].z;
						 setcolor[3] = color.w;
						 configini->setSubFontColor(setcolor);
					}

					// Allow user to drop colors into each palette entry. Note that ColorButton() is already a
					// drag source by default, unless specifying the ImGuiColorEditFlags_NoDragDrop flag.
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
							memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 3);
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
							memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 4);
						ImGui::EndDragDropTarget();
					}

					ImGui::PopID();
				}
				if(ImGui::Button("Use Color")){
					float setcolor[4];
					setcolor[0] = color.x;
					setcolor[1] = color.y;
					setcolor[2] = color.z;
					setcolor[3] = color.w;
					configini->setSubFontColor(setcolor);
					libmpv->setSubFontColor(configini->getSubFontColorHex(true));
					item.popupstate = POPUP_STATE_NONE;
				}
				ImGui::SameLine();
				if(ImGui::Button("Cancel")){
					item.popupstate = POPUP_STATE_NONE;
					float setcolor[4];
					setcolor[0] = backup_color.x;
					setcolor[1] = backup_color.y;
					setcolor[2] = backup_color.z;
					setcolor[3] = backup_color.w;
					configini->setSubFontColor(setcolor);
				}
				
				ImGui::EndGroup();
				ImGui::EndPopup();
		}
	}
	

	static ImVec4 backup_color2;
	static ImVec4 color2;	
	static bool saved_palette_init2 = true;
	static ImVec4 saved_palette2[32] = {};

	//bordercolor
	void SubBorderColorPopup(void) {
		if (saved_palette_init2)
        {
            for (int n = 0; n < IM_ARRAYSIZE(saved_palette2); n++)
            {
                ImGui::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f,
                    saved_palette2[n].x, saved_palette2[n].y, saved_palette2[n].z);
                saved_palette2[n].w = 1.0f; // Alpha
            }
            saved_palette_init2 = false;
        }
		ImGui::SetNextWindowPos(ImVec2(0,0));
		ImGui::OpenPopup("subbordercolorpicker");
		
		if (ImGui::BeginPopup("subbordercolorpicker")) {
				float * tmpcolor = configini->getSubBorderColor(true);
				float * tmpcolor_backup = configini->getSubBorderColor(false);
				color2 = ImVec4(tmpcolor[0],tmpcolor[1],tmpcolor[2],tmpcolor[3]);
				backup_color2 = ImVec4(tmpcolor_backup[0],tmpcolor_backup[1],tmpcolor_backup[2],tmpcolor_backup[3]);
				ImGui::Separator();
				if(ImGui::ColorPicker3("##picker", (float*)&color2,ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview)){
					float setcolor[4];
					setcolor[0] = color2.x;
					setcolor[1] = color2.y;
					setcolor[2] = color2.z;
					setcolor[3] = color2.w;
					configini->setSubBorderColor(setcolor);
				}
				ImGui::SameLine();

				ImGui::BeginGroup(); // Lock X position
				ImGui::Text("Current");
				ImGui::ColorButton("##current", color2, ImGuiColorEditFlags_NoAlpha| ImGuiColorEditFlags_NoPicker  , ImVec2(60, 40));
				ImGui::Text("Previous");
				if (ImGui::ColorButton("##previous", backup_color2, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker , ImVec2(60, 40))){
					float setcolor[4];
					setcolor[0] = backup_color2.x;
					setcolor[1] = backup_color2.y;
					setcolor[2] = backup_color2.z;
					setcolor[3] = backup_color2.w;
					configini->setSubBorderColor(setcolor);
				}
				ImGui::Separator();
				ImGui::Text("Palette");
				for (int n = 0; n < IM_ARRAYSIZE(saved_palette2); n++)
				{
					ImGui::PushID(n);
					if ((n % 8) != 0)
						ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);

					ImGuiColorEditFlags palette_button_flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip;
					if (ImGui::ColorButton("##palette", saved_palette2[n], palette_button_flags, ImVec2(20, 20))){
						 //color = ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, color.w); // Preserve alpha!
						 float setcolor[4];
						 setcolor[0] = saved_palette2[n].x;
						 setcolor[1] = saved_palette2[n].y;
						 setcolor[2] = saved_palette2[n].z;
						 setcolor[3] = color2.w;
						 configini->setSubBorderColor(setcolor);
					}

					// Allow user to drop colors into each palette entry. Note that ColorButton() is already a
					// drag source by default, unless specifying the ImGuiColorEditFlags_NoDragDrop flag.
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
							memcpy((float*)&saved_palette2[n], payload->Data, sizeof(float) * 3);
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
							memcpy((float*)&saved_palette2[n], payload->Data, sizeof(float) * 4);
						ImGui::EndDragDropTarget();
					}

					ImGui::PopID();
				}
				if(ImGui::Button("Use Color")){
					float setcolor[4];
					setcolor[0] = color2.x;
					setcolor[1] = color2.y;
					setcolor[2] = color2.z;
					setcolor[3] = color2.w;
					configini->setSubBorderColor(setcolor);
					libmpv->setSubBorderColor(configini->getSubBorderColorHex(true));
					item.popupstate = POPUP_STATE_NONE;
				}
				ImGui::SameLine();
				if(ImGui::Button("Cancel")){
					item.popupstate = POPUP_STATE_NONE;
					float setcolor[4];
					setcolor[0] = backup_color2.x;
					setcolor[1] = backup_color2.y;
					setcolor[2] = backup_color2.z;
					setcolor[3] = backup_color2.w;
					configini->setSubBorderColor(setcolor);
				}
				
				ImGui::EndGroup();
				ImGui::EndPopup();
		}
	}
	//bordercolorend

	void DBUpdatedPopup(void) {
		Popups::SetupPopup("Database Updated");
		if (ImGui::BeginPopupModal("Database Updated", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Database was Updated");
			ImGui::Text("DB Version: %s",sqlitedb->getDbVersion().c_str());
			
			if(ImGui::Button("OK",ImVec2(ImGui::GetContentRegionAvail().x,0.0f))){
				item.popupstate = POPUP_STATE_NONE;
			}
			ImGui::SetFocusID(ImGui::GetID("OK"), ImGui::GetCurrentWindow()); 
			ImGuiContext& g = *ImGui::GetCurrentContext();
			g.NavDisableHighlight = false;
			
		}
		Popups::ExitPopup();
	}
	
}