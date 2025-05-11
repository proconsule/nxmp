#include "apppopups.h"
#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "utils.h"

#include "nxmp-i18n.h"


namespace Popups{
	
	
	void PlaylistStartPlaylist(void) {
		Popups::SetupPopup("Playlist Start");
		if (ImGui::BeginPopupModal("Playlist Start", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		if(playlist->getPlaylist().size()>0){
				ImGui::Text("Start Playlist?");
				ImVec2 button_size(ImGui::GetFontSize() * 7.0f, 0.0f);
				if (ImGui::Button("Yes", button_size)) {
					if(item.state == MENU_STATE_FILEBROWSER || item.state == MENU_STATE_USB_BROWSER ||item.state == MENU_STATE_FTPBROWSER || item.state == MENU_STATE_HTTPBROWSER || item.state == MENU_STATE_SSHBROWSER || item.state == MENU_STATE_SAMBABROWSER || item.state == MENU_STATE_NFSBROWSER){
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
		Popups::SetupNativePopup(nxlangs::get_popup_str(nxlangs::NXPOPUP_SAVESETTINGS).c_str());

		if (ImGui::BeginPopupModal(nxlangs::get_popup_str(nxlangs::NXPOPUP_SAVESETTINGS).c_str(), nullptr, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoTitleBar)) {
			
			ImGui::SetWindowFontScale(1.5f);
			ImVec2 textsize = ImGui::CalcTextSize(nxlangs::get_popup_str(nxlangs::NXPOPUP_SAVESETTINGS).c_str());
			ImGui::SetCursorPosX((771.0f*multiplyRes-textsize.x)/2.0);
			ImGui::SetCursorPosY((292.0f*multiplyRes-70-textsize.y)/2.0);
			
			ImGui::Text("%s?",nxlangs::get_popup_str(nxlangs::NXPOPUP_SAVESETTINGS).c_str());
			
			ImGui::SetCursorPosY(292.0f*multiplyRes-70.0f);
			ImVec2 button_size(771.0f*multiplyRes/2.0, 70.0f);
			
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize,1.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			
            
			if (ImGui::Button(nxlangs::get_common_str(nxlangs::NXCOMMON_NO).c_str(), button_size))
			{
				configini->ReadConfig();
				item.state = MENU_STATE_HOME;
				item.popupstate = POPUP_STATE_NONE;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button(nxlangs::get_common_str(nxlangs::NXCOMMON_YES).c_str(), button_size))
            {
				item.state = MENU_STATE_HOME;
				item.popupstate = POPUP_STATE_NONE;
				configini->saveSettings();
				configini->ReadConfig();
				//delete libmpv;
				//GUI::initMpv();
					
			}
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
			
			
		}
		Popups::ExitNativePopup();
	}
	
	void ResumePopup(void) {
		Popups::SetupNativePopup("##resumeplaybackpopup");

		if (ImGui::BeginPopupModal("##resumeplaybackpopup", nullptr, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoTitleBar)) {
			
			ImGui::SetWindowFontScale(1.5f);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()+25.0f);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX()+25.0f);
			ImGui::Text("%s?",nxlangs::get_popup_str(nxlangs::NXPOPUP_RESUMEPLAY).c_str());
			
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()+25.0f);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX()+25.0f);
			
			ImGui::Text("%s @ %s",nxlangs::get_popup_str(nxlangs::NXPOPUP_RESUMEPLAY).c_str(),Utility::formatTimeShort(libmpv->getFileInfo()->resume).c_str());
            
			
			ImGui::SetCursorPosY(292.0f*multiplyRes-70.0f);
			ImVec2 button_size(771.0f*multiplyRes/2.0, 70.0f);
			
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize,1.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			if (ImGui::Button(nxlangs::get_common_str(nxlangs::NXCOMMON_NO).c_str(), button_size))
			{
                    
				
				item.popupstate = POPUP_STATE_NONE;
				ImGui::CloseCurrentPopup();
            }
			ImGui::SameLine();
			if (ImGui::Button(nxlangs::get_common_str(nxlangs::NXCOMMON_YES).c_str(), button_size))
            {
				libmpv->seek(libmpv->getFileInfo()->resume,false);
				item.popupstate = POPUP_STATE_NONE;
				ImGui::CloseCurrentPopup();
					
			}
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
			
		}
		Popups::ExitNativePopup();
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


	
	
	
	void FileContextPopup(void) {
		Popups::SetupPopup(nxlangs::get_popup_str(nxlangs::NXPOPUP_FILEMENU).c_str());
		if(FilePopupTextScroller == nullptr){
			FilePopupTextScroller = new CTextScroller("##filepopuptextscroll");
		}
		
		if (ImGui::BeginPopupModal(nxlangs::get_popup_str(nxlangs::NXPOPUP_FILEMENU).c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			
			ImVec2 button_size(400.0f, 0.0f);
			std::vector<FS::FileEntry> selectionlist = filebrowser->getChecked();
			
			if(selectionlist.size()!=0){
				std::string outstring = "";
				for(int i=0;i<selectionlist.size();i++){
					outstring = outstring + selectionlist[i].name + " ";
				}
				FilePopupTextScroller->Draw("##filepopuptextscroll",500.0f,30.0f,"%s",outstring.c_str());
			}else{
				FilePopupTextScroller->Draw("##filepopuptextscroll",500.0f,30.0f,"%s",filebrowser->getCurrList()[item.fileHoveredidx].name.c_str());
				
			
			}
			
			
			float alignment = 0.5f;
			
			float size = 400.0f + ImGui::GetStyle().FramePadding.x * 2.0f;
			float avail = ImGui::GetContentRegionAvail().x;

			float off = (avail - size) * alignment;
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
			
			if(filebrowser->sortOrder == FS::FS_NAME_ASCENDINGORDER){
				if (ImGui::Button(nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTDESC_NAME).c_str(),button_size))
				{
					filebrowser->setSordOrder(FS::FS_NAME_DESCENDINGORDER);
					item.popupstate = POPUP_STATE_NONE;
				}
			}else{
				if (ImGui::Button(nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTASC_NAME).c_str(),button_size))
				{
					filebrowser->setSordOrder(FS::FS_NAME_ASCENDINGORDER);
					item.popupstate = POPUP_STATE_NONE;
				}
			}
			if(!filebrowser->timelessFS){
				if (off > 0.0f)
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
				if(filebrowser->sortOrder == FS::FS_DATE_ASCENDINGORDER){
					if (ImGui::Button(nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTDESC_DATE).c_str(),button_size))
					{
						filebrowser->setSordOrder(FS::FS_DATE_DESCENDINGORDER);
						item.popupstate = POPUP_STATE_NONE;
					}
				}else{
					if (ImGui::Button(nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTASC_DATE).c_str(),button_size))
					{
						filebrowser->setSordOrder(FS::FS_DATE_ASCENDINGORDER);
						item.popupstate = POPUP_STATE_NONE;
					}
				}
			}
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
			if(filebrowser->sortOrder == FS::FS_SIZE_ASCENDINGORDER){
				if (ImGui::Button(nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTDESC_SIZE).c_str(),button_size))
				{
					filebrowser->setSordOrder(FS::FS_SIZE_DESCENDINGORDER);
					item.popupstate = POPUP_STATE_NONE;
				}
			}else{
				if (ImGui::Button(nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTASC_SIZE).c_str(),button_size))
				{
					filebrowser->setSordOrder(FS::FS_SIZE_ASCENDINGORDER);
					item.popupstate = POPUP_STATE_NONE;
				}
			}
			
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
			
			if(selectionlist.size()!=0){
				if (ImGui::Button(nxlangs::get_popup_str(nxlangs::NXPOPUP_ADDSELEPLAYLIST).c_str(),button_size))
				{
					
					for(int i=0;i<selectionlist.size();i++){
						playlist->appendFile(selectionlist[i],filebrowser->getOpenUrlPart()+selectionlist[i].path);
					}
					item.popupstate = POPUP_STATE_NONE;
				}
			}else{
				if (ImGui::Button(nxlangs::get_popup_str(nxlangs::NXPOPUP_ADDFILEPLAYLIST).c_str(),button_size))
				{
					playlist->appendFile(filebrowser->getCurrList()[item.fileHoveredidx],filebrowser->getOpenUrlPart()+filebrowser->getCurrList()[item.fileHoveredidx].path);
					item.popupstate = POPUP_STATE_NONE;
				}
				
			}
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
			
			if (ImGui::Button(nxlangs::get_common_str(nxlangs::NXCOMMON_EXIT).c_str(), button_size))
			{
                item.popupstate = POPUP_STATE_NONE;
				ImGui::CloseCurrentPopup();
            }
			
		}
		Popups::ExitPopup();
	}
	
	void PlaylistContextPopup(void) {
		Popups::SetupPopup("Playlist Menu Popup");

		if (ImGui::BeginPopupModal("Playlist Menu Popup", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			
			
			ImVec2 button_size(400.0f, 0.0f);
			
			float alignment = 0.5f;
			
			float size = 400.0f + ImGui::GetStyle().FramePadding.x * 2.0f;
			float avail = ImGui::GetContentRegionAvail().x;

			float off = (avail - size) * alignment;
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
			
			
			if (ImGui::Button("Start Playlist",button_size))
			{
				ImGui::CloseCurrentPopup();
				item.popupstate = POPUP_STATE_NONE;
				libmpv->loadFile(playlist->getPlaylist()[playlist->getCurrIdx()].fulluri);
				if(sqlitedb != nullptr){
					libmpv->getFileInfo()->resume = sqlitedb->getResume(playlist->getPlaylist()[playlist->getCurrIdx()].fulluri);
					if(libmpv->getFileInfo()->resume>0){
						item.popupstate = POPUP_STATE_RESUME;
					}
				}
                
            }
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
			
			if (ImGui::Button(nxlangs::get_popup_str(nxlangs::NXPOPUP_SETASSTARTING).c_str(),button_size))
			{
                playlist->setPlaylistIdx(item.playlistitemHighlighted);
				item.popupstate = POPUP_STATE_NONE;
				ImGui::CloseCurrentPopup();
            }
			if(item.playlistitemHighlighted<= 0){
				ImGui::BeginDisabled();
			}
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
			
			if (ImGui::Button(nxlangs::get_common_str(nxlangs::NXCOMMON_MOVEUP).c_str(),button_size))
			{
                playlist->moveBack(item.playlistitemHighlighted);
				item.popupstate = POPUP_STATE_NONE;
				ImGui::CloseCurrentPopup();
            }
			if(item.playlistitemHighlighted<= 0){
				ImGui::EndDisabled();
			}
			
			if(item.playlistitemHighlighted >= playlist->getPlaylist().size()-1){
				ImGui::BeginDisabled();
			}
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
			
			if (ImGui::Button(nxlangs::get_common_str(nxlangs::NXCOMMON_MOVEDOWN).c_str(),button_size))
			{
                playlist->moveForw(item.playlistitemHighlighted);
				item.popupstate = POPUP_STATE_NONE;
				ImGui::CloseCurrentPopup();
            }
			if(item.playlistitemHighlighted >= playlist->getPlaylist().size()-1){
				ImGui::EndDisabled();
			}

			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
			if (ImGui::Button(nxlangs::get_popup_str(nxlangs::NXPOPUP_REMOVEELEMENT).c_str(),button_size))
			{
                playlist->RemoveElement(item.playlistitemHighlighted);
				item.popupstate = POPUP_STATE_NONE;
				ImGui::CloseCurrentPopup();
            }
			
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));
			
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
			if (ImGui::Button(nxlangs::get_popup_str(nxlangs::NXPOPUP_CLEARPLAYLIST).c_str(),button_size))
			{
                playlist->clearPlaylist();
				item.popupstate = POPUP_STATE_NONE;
				ImGui::CloseCurrentPopup();
            }
			ImGui::PopStyleColor(3);
			
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
			if (ImGui::Button(nxlangs::get_common_str(nxlangs::NXCOMMON_EXIT).c_str(), button_size))
			{
                item.popupstate = POPUP_STATE_NONE;
				ImGui::CloseCurrentPopup();
            }
			
		}
		Popups::ExitPopup();
	}
	
	void DBUpdatedPopup(void) {
		Popups::SetupNativePopup("##dbutpadted");
		if (ImGui::BeginPopupModal("##dbutpadted", nullptr, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoTitleBar)) {
			
			
			ImGui::SetWindowFontScale(1.5f);
			std::string dbversionstr = nxlangs::get_settingsmenu_str(nxlangs::NXSET_DATABASEVERSION).c_str() + std::string(" ") + sqlitedb->getDbVersion().c_str();
			ImVec2 textsize = ImGui::CalcTextSize(nxlangs::get_popup_str(nxlangs::NXPOPUP_DBWASUPDATED).c_str());
			ImVec2 textsize2 = ImGui::CalcTextSize(dbversionstr.c_str());
			ImGui::SetCursorPosX((771.0f*multiplyRes-textsize.x)/2.0);
			ImGui::SetCursorPosY((292.0f*multiplyRes-70-textsize.y-textsize2.y)/2.0);
			
			ImGui::Text(nxlangs::get_popup_str(nxlangs::NXPOPUP_DBWASUPDATED).c_str());
			
			ImGui::SetCursorPosX((771.0f*multiplyRes-textsize2.x)/2.0);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()+10.0*multiplyRes);
			ImGui::Text("%s %s",nxlangs::get_settingsmenu_str(nxlangs::NXSET_DATABASEVERSION).c_str(),sqlitedb->getDbVersion().c_str());
			
			ImGui::SetCursorPosY(292.0f*multiplyRes-70.0f);
			ImVec2 button_size(771.0f*multiplyRes, 70.0f);
		
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize,1.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			
			
			if(ImGui::Button(nxlangs::get_common_str(nxlangs::NXCOMMON_OK).c_str(),button_size)){
				item.popupstate = POPUP_STATE_NONE;
			}
			ImGui::SetFocusID(ImGui::GetID(nxlangs::get_common_str(nxlangs::NXCOMMON_OK).c_str()), ImGui::GetCurrentWindow()); 
			ImGuiContext& g = *ImGui::GetCurrentContext();
			g.NavDisableHighlight = false;
			
			
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
			
			
		}
		Popups::ExitNativePopup();
	}
	
	void AppExitPopup(void) {
		Popups::SetupNativePopup("Exit NXMP");

		if (ImGui::BeginPopupModal("Exit NXMP", nullptr, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoTitleBar)) {
			
			
			ImGui::SetWindowFontScale(1.5f);
			ImVec2 textsize = ImGui::CalcTextSize("Exit NXMP");
			ImGui::SetCursorPosX((771.0f*multiplyRes-textsize.x)/2.0);
			ImGui::SetCursorPosY((292.0f*multiplyRes-70-textsize.y)/2.0);
			
			ImGui::Text("%s?","Exit NXMP");
			
			ImGui::SetCursorPosY(292.0f*multiplyRes-70.0f);
			ImVec2 button_size(771.0f*multiplyRes/2.0, 70.0f);
			
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize,1.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			
            
			if (ImGui::Button("Return to HB-Menu", button_size))
			{
				ImGui::CloseCurrentPopup();
				renderloopdone = true;
				app_exit_mode = 0;
				//configini->setExitMode(0);
			}
			ImGui::SameLine();
			if (ImGui::Button("Exit to Home", button_size))
            {
				ImGui::CloseCurrentPopup();
				renderloopdone = true;
				app_exit_mode = 1;
				//configini->setExitMode(1);
					
			}
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
			
		}
		Popups::ExitNativePopup();
	}
	
	void NetMenuPopup(void) {
		if(Windows::netwinselected==-1)return;
		Popups::SetupNativeContextPopup("##netmenupopup",4);

		if (ImGui::BeginPopupModal("##netmenupopup", nullptr, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoTitleBar)) {
			
			ImGui::SetWindowFontScale(1.5f);
			
			ImVec2 textsize = ImGui::CalcTextSize(item.networksources[Windows::netwinselected].name.c_str());
			
			ImGui::SetCursorPosX(ImGui::GetCursorPosX()+((771.0f*multiplyRes-textsize.x)/2.0f));
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()+(70.0f*multiplyRes-multiplyRes-textsize.y)/2.0f);
			ImGui::Text("%s",item.networksources[Windows::netwinselected].name.c_str());
			ImGui::SetCursorPosY(ImGui::GetCursorPosY()+(70.0f*multiplyRes-multiplyRes-textsize.y)/2.0f);
			
			
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize,1.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			
			
			
			ImVec2 button_size(771.0f*multiplyRes, 70.0f);
			if (ImGui::Button(nxlangs::get_common_str(nxlangs::NXCOMMON_MOVEUP).c_str(), button_size))
            {
					if(Windows::netwinselected>0){
						oldnetworkSource v1 = item.networksources[Windows::netwinselected];
						oldnetworkSource v2 = item.networksources[Windows::netwinselected-1];
						
						item.networksources[Windows::netwinselected]  = v2;
						item.networksources[Windows::netwinselected-1]  = v1;
						configini->RefreshNetworkShare(item.networksources);
					}
					item.popupstate = POPUP_STATE_NONE;
					ImGui::CloseCurrentPopup();
					
					
			}
			if (ImGui::Button(nxlangs::get_common_str(nxlangs::NXCOMMON_MOVEDOWN).c_str(), button_size))
            {
					if(Windows::netwinselected<item.networksources.size()-2){
						oldnetworkSource v1 = item.networksources[Windows::netwinselected];
						oldnetworkSource v2 = item.networksources[Windows::netwinselected+1];
					
						item.networksources[Windows::netwinselected]  = v2;
						item.networksources[Windows::netwinselected+1]  = v1;
						configini->RefreshNetworkShare(item.networksources);
					}
					item.popupstate = POPUP_STATE_NONE;
					ImGui::CloseCurrentPopup();
			}
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));
			if (ImGui::Button(nxlangs::get_common_str(nxlangs::NXCOMMON_DELETE).c_str(), button_size))
            {	
				item.networksources.erase(item.networksources.begin() + Windows::netwinselected);
				item.popupstate = POPUP_STATE_NONE;
				ImGui::CloseCurrentPopup();
			}
			ImGui::PopStyleColor(3);
			
			
			
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
			
		}
		Popups::ExitNativePopup();
	}
	void FileContextNativePopup(void) {
		Popups::SetupNativeContextPopup("##filecontext",6);
		if(FilePopupTextScroller == nullptr){
			FilePopupTextScroller = new CTextScroller("##filepopuptextscroll");
		}
		
		if (ImGui::BeginPopupModal("##filecontext", nullptr, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoTitleBar)) {
			ImVec2 popup_screen_pos = ImGui::GetCursorScreenPos();
			ImGui::SetWindowFontScale(1.2f);
			ImVec2 button_size(771.0f*multiplyRes, 70.0f);
			std::vector<FS::FileEntry> selectionlist = filebrowser->getChecked();
			int myorder = filebrowser->sortOrder;
			
			if(selectionlist.size()!=0){
				std::string outstring = "";
				for(int i=0;i<selectionlist.size();i++){
					outstring = outstring + selectionlist[i].name + " ";
				}
				FilePopupTextScroller->DrawPopupNative("##filepopuptextscroll",771.0f,70.0f,"%s",outstring.c_str());
			}else{
				FilePopupTextScroller->DrawPopupNative("##filepopuptextscroll",771.0f,70.0f,"%s",filebrowser->getCurrList()[item.fileHoveredidx].name.c_str());
				
			
			}
			
			
			ImGui::PushStyleColor(ImGuiCol_Text, Text_color);
			
			if(myorder == FS::FS_NAME_ASCENDINGORDER|| myorder == FS::FS_NAME_DESCENDINGORDER){
				ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			}
			
			if(myorder == FS::FS_NAME_ASCENDINGORDER){
				
				if (ImGui::Button(nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTDESC_NAME).c_str(),button_size))
				{
					filebrowser->setSordOrder(FS::FS_NAME_DESCENDINGORDER);
					item.popupstate = POPUP_STATE_NONE;
				}
				
			}else{
				
				if (ImGui::Button(nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTASC_NAME).c_str(),button_size))
				{
					filebrowser->setSordOrder(FS::FS_NAME_ASCENDINGORDER);
					item.popupstate = POPUP_STATE_NONE;
				}
				
			}
			if(myorder == FS::FS_NAME_ASCENDINGORDER|| myorder == FS::FS_NAME_DESCENDINGORDER){
				ImGui::PopStyleColor();
			}
			if(!filebrowser->timelessFS){
				if(myorder == FS::FS_DATE_ASCENDINGORDER|| myorder == FS::FS_DATE_DESCENDINGORDER){
					ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
				}
				if(myorder == FS::FS_DATE_ASCENDINGORDER){
					if (ImGui::Button(nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTDESC_DATE).c_str(),button_size))
					{
						filebrowser->setSordOrder(FS::FS_DATE_DESCENDINGORDER);
						item.popupstate = POPUP_STATE_NONE;
					}
				}else{
					if (ImGui::Button(nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTASC_DATE).c_str(),button_size))
					{
						filebrowser->setSordOrder(FS::FS_DATE_ASCENDINGORDER);
						item.popupstate = POPUP_STATE_NONE;
					}
					
				}
			}
			if(myorder == FS::FS_DATE_ASCENDINGORDER|| myorder == FS::FS_DATE_DESCENDINGORDER){
				ImGui::PopStyleColor();
			}
			if(myorder == FS::FS_SIZE_ASCENDINGORDER|| myorder == FS::FS_SIZE_DESCENDINGORDER){
				ImGui::PushStyleColor(ImGuiCol_Text, Active_color);
			}
			if(myorder == FS::FS_SIZE_ASCENDINGORDER){
				if (ImGui::Button(nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTDESC_SIZE).c_str(),button_size))
				{
					filebrowser->setSordOrder(FS::FS_SIZE_DESCENDINGORDER);
					item.popupstate = POPUP_STATE_NONE;
				}
			}else{
				if (ImGui::Button(nxlangs::get_popup_str(nxlangs::NXPOPUP_SORTASC_SIZE).c_str(),button_size))
				{
					filebrowser->setSordOrder(FS::FS_SIZE_ASCENDINGORDER);
					item.popupstate = POPUP_STATE_NONE;
				}
			}
			if(myorder == FS::FS_SIZE_ASCENDINGORDER|| myorder == FS::FS_SIZE_DESCENDINGORDER){
				ImGui::PopStyleColor();
			}
			
			if(selectionlist.size()!=0){
				if (ImGui::Button(nxlangs::get_popup_str(nxlangs::NXPOPUP_ADDSELEPLAYLIST).c_str(),button_size))
				{
					
					for(int i=0;i<selectionlist.size();i++){
						playlist->appendFile(selectionlist[i],filebrowser->getOpenUrlPart()+selectionlist[i].path);
					}
					item.popupstate = POPUP_STATE_NONE;
				}
			}else{
				if (ImGui::Button(nxlangs::get_popup_str(nxlangs::NXPOPUP_ADDFILEPLAYLIST).c_str(),button_size))
				{
					playlist->appendFile(filebrowser->getCurrList()[item.fileHoveredidx],filebrowser->getOpenUrlPart()+filebrowser->getCurrList()[item.fileHoveredidx].path);
					item.popupstate = POPUP_STATE_NONE;
				}
				
			}
			
			if(ImGui::IsMouseClicked(0)){
				ImVec2 popup_screen_pos2 = ImVec2(popup_screen_pos.x+771.0f*multiplyRes,popup_screen_pos.y+70.0f*multiplyRes*6);
				ImVec2 mousepos = ImGui::GetMousePos();
				if(mousepos.x<popup_screen_pos.x || mousepos.x>popup_screen_pos.x){
					item.popupstate = POPUP_STATE_NONE;
				}
				if(mousepos.y<popup_screen_pos.y || mousepos.y>popup_screen_pos.y){
					item.popupstate = POPUP_STATE_NONE;
				}
				
			}
			ImGui::PopStyleColor();
			
		}
		Popups::ExitNativePopup();
	}
}

