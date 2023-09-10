#include "apppopups.h"
#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "utils.h"

namespace Popups{
	
	float pupuptextscrollpos = 0.0f;
	int popuptextwaitpos = 0;
	
	
	bool pupuptextforwardscroll = true;
	bool pupuptextlaststate = true;
	
	void PopupScrollText(float w,float h,const char* fmt, ...){
		
		ImGui::BeginChild("###scrollable", ImVec2(w, h), false);
		va_list args;
		va_start(args, fmt);
		ImGui::TextV(fmt, args);
		va_end(args);
		
		float scroll_max_x = ImGui::GetScrollMaxX();
		if(pupuptextscrollpos>=scroll_max_x){
			pupuptextforwardscroll=false;
			if(!pupuptextlaststate==pupuptextforwardscroll){
				popuptextwaitpos = 0;
			}
		}else if(pupuptextscrollpos<=0) {
			pupuptextforwardscroll=true;	
			if(!pupuptextlaststate==pupuptextforwardscroll){
				popuptextwaitpos = 0;
			}
		}
		if(popuptextwaitpos <= 60){
			popuptextwaitpos++;
		}else{
			if(pupuptextforwardscroll){
				pupuptextscrollpos+=0.5;
			}else{
				pupuptextscrollpos-=0.5;
			}
		}
		
		pupuptextlaststate = pupuptextforwardscroll;
		ImGui::SetScrollX(pupuptextscrollpos);
		ImGui::EndChild();
	}
	
	
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
	
	void NetMenuPopup(void) {
		Popups::SetupPopup("Network Menu Popup");

		if (ImGui::BeginPopupModal("Network Menu Popup", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			
			ImGui::Text("%s",item.networksources[Windows::netwinselected].name.c_str());
			
			ImVec2 button_size(ImGui::GetFontSize() * 7.0f, 0.0f);
			if(Windows::netwinselected == 0){
				ImGui::BeginDisabled();
			}
			if (ImGui::Button("Move Up", button_size))
            {
					
					networkSource v1 = item.networksources[Windows::netwinselected];
					networkSource v2 = item.networksources[Windows::netwinselected-1];
					
					item.networksources[Windows::netwinselected]  = v2;
					item.networksources[Windows::netwinselected-1]  = v1;
					configini->RefreshNetworkShare(item.networksources);
					item.popupstate = POPUP_STATE_NONE;
					ImGui::CloseCurrentPopup();
					
					
			}
			if(Windows::netwinselected == 0){
				ImGui::EndDisabled();
			}
			if(Windows::netwinselected == item.networksources.size()-1){
				ImGui::BeginDisabled();
			}
			if (ImGui::Button("Move Down", button_size))
            {
					networkSource v1 = item.networksources[Windows::netwinselected];
					networkSource v2 = item.networksources[Windows::netwinselected+1];
					
					item.networksources[Windows::netwinselected]  = v2;
					item.networksources[Windows::netwinselected+1]  = v1;
					configini->RefreshNetworkShare(item.networksources);
					item.popupstate = POPUP_STATE_NONE;
					ImGui::CloseCurrentPopup();
			}
			if(Windows::netwinselected == item.networksources.size()-1){
				ImGui::EndDisabled();
			}
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));
			if (ImGui::Button("Delete", button_size))
            {	
				item.networksources.erase(item.networksources.begin() + Windows::netwinselected);
				item.popupstate = POPUP_STATE_NONE;
				ImGui::CloseCurrentPopup();
			}
			ImGui::PopStyleColor(3);
			
			
			
			if (ImGui::Button("Exit", button_size))
			{
                item.popupstate = POPUP_STATE_NONE;
				ImGui::CloseCurrentPopup();
             }
			
		}
		Popups::ExitPopup();
	}
	
	
	void FileContextPopup(void) {
		Popups::SetupPopup("File Context Menu Popup");

		if (ImGui::BeginPopupModal("File Context Menu Popup", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			
			
			ImVec2 button_size(400.0f, 0.0f);
			std::vector<FS::FileEntry> selectionlist = filebrowser->getChecked();
			
			if(selectionlist.size()!=0){
				std::string outstring = "";
				for(int i=0;i<selectionlist.size();i++){
					outstring = outstring + selectionlist[i].name + " ";
				}
				PopupScrollText(500.0f,30.0f,"%s",outstring.c_str());
			}else{
				PopupScrollText(500.0f,30.0f,"%s",filebrowser->getCurrList()[item.fileHoveredidx].name.c_str());
				
			
			}
			
			
			float alignment = 0.5f;
			
			float size = 400.0f + ImGui::GetStyle().FramePadding.x * 2.0f;
			float avail = ImGui::GetContentRegionAvail().x;

			float off = (avail - size) * alignment;
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
			
			if(filebrowser->sortOrder == FS::FS_NAME_ASCENDINGORDER){
				if (ImGui::Button("Sort Descending (Z-A)",button_size))
				{
					filebrowser->setSordOrder(FS::FS_NAME_DESCENDINGORDER);
					item.popupstate = POPUP_STATE_NONE;
				}
			}else{
				if (ImGui::Button("Sort Ascending (A-Z)",button_size))
				{
					filebrowser->setSordOrder(FS::FS_NAME_ASCENDINGORDER);
					item.popupstate = POPUP_STATE_NONE;
				}
			}
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
			if(filebrowser->sortOrder == FS::FS_DATE_ASCENDINGORDER){
				if (ImGui::Button("Sort Descending (Date)",button_size))
				{
					filebrowser->setSordOrder(FS::FS_DATE_DESCENDINGORDER);
					item.popupstate = POPUP_STATE_NONE;
				}
			}else{
				if (ImGui::Button("Sort Ascending (Date)",button_size))
				{
					filebrowser->setSordOrder(FS::FS_DATE_ASCENDINGORDER);
					item.popupstate = POPUP_STATE_NONE;
				}
			}
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
			if(filebrowser->sortOrder == FS::FS_SIZE_ASCENDINGORDER){
				if (ImGui::Button("Sort Descending (Size)",button_size))
				{
					filebrowser->setSordOrder(FS::FS_SIZE_DESCENDINGORDER);
					item.popupstate = POPUP_STATE_NONE;
				}
			}else{
				if (ImGui::Button("Sort Ascending (Size)",button_size))
				{
					filebrowser->setSordOrder(FS::FS_SIZE_ASCENDINGORDER);
					item.popupstate = POPUP_STATE_NONE;
				}
			}
			
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
			
			if(selectionlist.size()!=0){
				if (ImGui::Button("Add Checked to Playlist",button_size))
				{
					
					for(int i=0;i<selectionlist.size();i++){
						playlist->appendFile(selectionlist[i],filebrowser->getOpenUrlPart()+selectionlist[i].path);
					}
					item.popupstate = POPUP_STATE_NONE;
				}
			}else{
				if (ImGui::Button("Add File to Playlist",button_size))
				{
					playlist->appendFile(filebrowser->getCurrList()[item.fileHoveredidx],filebrowser->getOpenUrlPart()+filebrowser->getCurrList()[item.fileHoveredidx].path);
				}
			}
			if (off > 0.0f)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
			
			if (ImGui::Button("Exit", button_size))
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
			
			
			ImVec2 button_size(ImGui::GetFontSize() * 10.0f, 0.0f);
			
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
			if (ImGui::Button("Set as Startring element",button_size))
			{
                playlist->setPlaylistIdx(item.playlistitemHighlighted);
				item.popupstate = POPUP_STATE_NONE;
				ImGui::CloseCurrentPopup();
            }
			if(item.playlistitemHighlighted<= 0){
				ImGui::BeginDisabled();
			}
			if (ImGui::Button("Move Up Element",button_size))
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
			if (ImGui::Button("Move Down",button_size))
			{
                playlist->moveForw(item.playlistitemHighlighted);
				item.popupstate = POPUP_STATE_NONE;
				ImGui::CloseCurrentPopup();
            }
			if(item.playlistitemHighlighted >= playlist->getPlaylist().size()-1){
				ImGui::EndDisabled();
			}
			
			if (ImGui::Button("Remove Element",button_size))
			{
                playlist->RemoveElement(item.playlistitemHighlighted);
				item.popupstate = POPUP_STATE_NONE;
				ImGui::CloseCurrentPopup();
            }
			
			if (ImGui::Button("Exit", button_size))
			{
                item.popupstate = POPUP_STATE_NONE;
				ImGui::CloseCurrentPopup();
            }
			
		}
		Popups::ExitPopup();
	}
	
}