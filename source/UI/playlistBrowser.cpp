#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"
#include "nxmp-i18n.h"


namespace Windows {
	
	bool playlistHoverSet = false;
	bool playlistHoverIdx = 0;
	
	void PlaylistWindow(bool *focus, bool *first_item){
		Windows::SetupWindow();
		if (ImGui::Begin("Playlist Browser", nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_MenuBar)) {
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, NavHover_color);
			ImGui::PushStyleColor(ImGuiCol_NavHighlight, Active_color);
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {0, 5});
			
			
			float total_w = ImGui::GetContentRegionAvail().x;
			float total_h = ImGui::GetContentRegionAvail().y;
			std::vector<Playlist::playlist_struct> thislist = playlist->getPlaylist();
			if(thislist.size() >0){
				ImGui::BeginChild("##tablecontainer",ImVec2(total_w,total_h-45*multiplyRes));
			
				if (ImGui::BeginTable("##tableplaylist", 3)){
					ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthFixed, (1150.0f-2 * ImGui::GetStyle().ItemSpacing.x)*multiplyRes); // Default to 100.0f
					ImGui::TableSetupColumn("duration", ImGuiTableColumnFlags_WidthFixed, 130.0f*multiplyRes); // Default to 200.0f
					ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns;
						
					for(unsigned int n=0;n<thislist.size();n++){
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						std::string itemid = "##" + std::to_string(n);
						if (ImGui::Selectable(itemid.c_str(), playlist->getCurrIdx() == n,selectable_flags)){
							
							item.laststate = item.state;
							playlist->setPlaylistIdx(n);
							
							libmpv->loadFile(thislist[n].fulluri);
							if(sqlitedb != nullptr){
								libmpv->getFileInfo()->resume = sqlitedb->getResume(thislist[n].fulluri);
								if(libmpv->getFileInfo()->resume>0){
									item.popupstate = POPUP_STATE_RESUME;
								}
							}
						}
						if (ImGui::IsItemHovered()){
							item.playlistitemHighlighted = n;
						}
						ImGui::SameLine();
						ImGui::Text("%d. %s",n+1,thislist[n].file.name.c_str());
						ImGui::TableSetColumnIndex(1);
						std::string itemdurationstr = thislist[n].is_valid ? Utility::formatTimeShort(thislist[n].duration).c_str(): "????";
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - ImGui::CalcTextSize(itemdurationstr.c_str()).x  - ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x);
						ImGui::Text("%s",itemdurationstr.c_str());
						
					}
					
					ImGui::EndTable();
					if(item.popupstate == POPUP_STATE_NONE){
						ImGui::SetWindowFocus();
					}
				}
				std::string playtime = Utility::formatTimeShort(playlist->remtime) + " / " + Utility::formatTimeShort(playlist->totaltime);
				ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x -  ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x - ImGui::CalcTextSize(playtime.c_str(), NULL, true).x);
				ImGui::Text("%s",playtime.c_str());				
				if(item.playlistUpdateHovered){
					std::string itemid = "##" + std::to_string(item.playlistnewHoverIdx);
					NXLOG::DEBUGLOG("Set hover id %d\n",item.playlistnewHoverIdx);
					fflush(stdout);
					ImGui::SetFocusID(ImGui::GetID((itemid.c_str())), ImGui::GetCurrentWindow());
					item.playlistUpdateHovered = false;
				}
				ImGui::EndChild();
				ImGuiContext& g = *GImGui;
				ImGui::NavMoveRequestTryWrapping(g.CurrentWindow, ImGuiNavMoveFlags_LoopY);
			}else{
				ImGui::BeginChild("##tableemptycontainer",ImVec2(total_w,total_h-45*multiplyRes));
				ImGui::SetCursorPosX((total_w - ImGui::CalcTextSize("Empty Playlist", NULL, true).x) * 0.5f);
				ImGui::SetCursorPosY(total_h  * 0.5f);
				ImGui::Text("Empty Playlist");
				ImGui::EndChild();
			}
			
			ImGui::BeginChild("##helpchild",ImVec2(total_w,ImGui::GetContentRegionAvail().y));
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			
			ImGui::Dummy(ImVec2(0,5));
			ImVec2 startpos =  ImGui::GetCursorScreenPos();
			ImGui::Dummy(ImVec2(0,5));
			ImGui::Text(FONT_DPADUP_BUTTON_FILLED);
			ImGui::SameLine();
			ImGui::Text(FONT_DPADDOWN_BUTTON_FILLED);
			ImGui::SameLine();
			ImGui::Text(nxlangs::get_common_str(nxlangs::NXCOMMON_NAVIGATION).c_str());
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
			ImGui::Text(FONT_DPADLEFT_BUTTON_FILLED);
			ImGui::SameLine();
			ImGui::Text(nxlangs::get_common_str(nxlangs::NXCOMMON_CHECKBOX).c_str());
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
			ImGui::Text(FONT_A_BUTTON_FILLED);
			ImGui::SameLine();
			ImGui::Text(nxlangs::get_common_str(nxlangs::NXCOMMON_SELECTPLAY).c_str());
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
			ImGui::Text(FONT_B_BUTTON_FILLED);
			ImGui::SameLine();
			ImGui::Text(nxlangs::get_common_str(nxlangs::NXCOMMON_BACK).c_str());
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
			ImGui::Text(FONT_X_BUTTON_FILLED);
			ImGui::SameLine();
			ImGui::Text(nxlangs::get_common_str(nxlangs::NXCOMMON_CONTEXTMENU).c_str());
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
			ImGui::Text(FONT_Y_BUTTON_FILLED);
			ImGui::SameLine();
			ImGui::Text(nxlangs::get_common_str(nxlangs::NXCOMMON_HOME).c_str());
			
			window->DrawList->AddLine(startpos,ImVec2(startpos.x+1280*multiplyRes,startpos.y) , ImGui::GetColorU32(ImVec4(1.0f,1.0f,1.0f,1.0f)), 1.0f);
			
			ImGui::EndChild();
			
			
			ImGui::PopStyleColor(2);
			ImGui::PopStyleVar();
		}
		Windows::ExitWindow();
	}
	
}