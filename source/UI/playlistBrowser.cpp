#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"


namespace Windows {
	
	bool playlistHoverSet = false;
	bool playlistHoverIdx = 0;
	
	void PlaylistWindow(bool *focus, bool *first_item){
		Windows::SetupWindow();
		if (ImGui::Begin("Playlist Browser", nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_MenuBar)) {
			
			float total_w = ImGui::GetContentRegionAvail().x;
			float total_h = ImGui::GetContentRegionAvail().y;
			std::vector<Playlist::playlist_struct> thislist = playlist->getPlaylist();
			if(thislist.size() >0){
				
				if (ImGui::BeginTable("##tableplaylist", 3)){
					ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthFixed, (1150.0f-2 * ImGui::GetStyle().ItemSpacing.x)*multiplyRes); // Default to 100.0f
					ImGui::TableSetupColumn("duration", ImGuiTableColumnFlags_WidthFixed, 130.0f*multiplyRes); // Default to 200.0f
					for(unsigned int n=0;n<thislist.size();n++){
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						std::string itemid = "##" + std::to_string(n);
						if (ImGui::Selectable(itemid.c_str(), playlist->getCurrIdx() == n)){
							
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
						ImGui::Text("%s",thislist[n].is_valid ? Utility::formatTimeShort(thislist[n].duration).c_str(): "????");
						
					}
					
					ImGui::EndTable();
				}
							
				if(item.playlistUpdateHovered){
					std::string itemid = "##" + std::to_string(item.playlistnewHoverIdx);
					NXLOG::DEBUGLOG("Set hover id %d\n",item.playlistnewHoverIdx);
					fflush(stdout);
					ImGui::SetFocusID(ImGui::GetID((itemid.c_str())), ImGui::GetCurrentWindow());
					item.playlistUpdateHovered = false;
				}
				ImGuiContext& g = *GImGui;
				ImGui::NavMoveRequestTryWrapping(g.CurrentWindow, ImGuiNavMoveFlags_LoopY);
			}else{
				ImGui::SetCursorPosX((total_w - ImGui::CalcTextSize("Empty Playlist", NULL, true).x) * 0.5f);
				ImGui::SetCursorPosY(total_h  * 0.5f);
				ImGui::Text("Empty Playlist");
			}
			
		}
		Windows::ExitWindow();
	}
	
}