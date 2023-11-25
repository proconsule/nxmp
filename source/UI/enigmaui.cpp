#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"
#include "nxmp-i18n.h"


namespace Windows {
    void EnigmaWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		
        if (ImGui::Begin("Enigma2", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
			
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, nxmpgfx::NavHover_color);
			ImGui::PushStyleColor(ImGuiCol_NavHighlight, nxmpgfx::Active_color);
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {0, 5});
			
			
			if(enigma2->getUrl().empty()){
				std::string emptytext = "Not Configured";
				auto cwindowWidth = ImGui::GetWindowSize().x;
				auto ctextWidth   = ImGui::CalcTextSize(emptytext.c_str()).x;
			
				auto cwindowHeight = ImGui::GetWindowSize().y;
				auto ctextHeight   = ImGui::CalcTextSize(emptytext.c_str()).y;

				ImGui::SetCursorPosX((cwindowWidth - ctextWidth) * 0.5f);
				ImGui::SetCursorPosY((cwindowHeight - ctextHeight) * 0.5f);
				ImGui::Text(emptytext.c_str());
			}else{
				
				//ImGui::SetNextWindowFocus();
				if (ImGui::BeginMenuBar()) {
					ImGui::Text("%s",enigma2->getCurrBouquet().name.c_str());
					ImGui::EndMenuBar();
				}
				
				float total_w = ImGui::GetContentRegionAvail().x;
				float total_h = ImGui::GetContentRegionAvail().y;
				ImGui::BeginChild("##tablecontainer",ImVec2(total_w,total_h-45*multiplyRes));
				if (ImGui::BeginTable("Enigma2 Browser Menu",1)){
					if(enigma2->getCurrBouquet().bouquetref == ""){
						for (unsigned int n = 0; n < enigma2->e2services.size(); n++){
							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							static int selected = -1;
							
							if (ImGui::Selectable(std::string("##item" + std::to_string(n)).c_str(), selected == n)){
				
								enigma2->m3uParser((char *)enigma2->e2services[n].bouquetref.c_str());
								enigma2->setCurrBouquet(enigma2->e2services[n]);
							}
							ImGui::SameLine();
							ImGui::Text(enigma2->e2services[n].name.c_str());
							if (selected)
								ImGui::SetItemDefaultFocus();
								
						}
						if (*first_item) {
							ImGui::SetFocusID(ImGui::GetID(enigma2->e2services[0].name.c_str()), ImGui::GetCurrentWindow());
							*first_item = false;
						}
					}else if(enigma2->getCurrBouquet().bouquetref != ""){
						for (unsigned int n = 0; n < enigma2->e2currbouqet.size(); n++){
							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							static int selected = -1;
							std::string itemid = "##" + std::to_string(n);
							float currstartpos = ImGui::GetCursorPosX();
							if (ImGui::Selectable(itemid.c_str(), selected == n, 0, ImVec2(0, 70))){
								item.laststate = item.state;
								libmpv->loadFileLive(enigma2->e2currbouqet[n].url,enigma2->e2currbouqet[n].name);
								
									
							}
							ImGui::SameLine();
							ImGui::SetCursorPosX(currstartpos);
							ImGui::Text("%s",enigma2->e2currbouqet[n].name.c_str());
							ImGui::SetCursorPosY(ImGui::GetCursorPosY()-40.0f);
							ImGui::Text("%s",enigma2->e2currbouqet[n].epg.title.c_str());
							float progressval = (float)(enigma2->e2currbouqet[n].epg.currTime-enigma2->e2currbouqet[n].epg.startTime)/(float)enigma2->e2currbouqet[n].epg.duration;
							std::string progressid = std::string("progress") + itemid;
							if(enigma2->e2currbouqet[n].epg.title == "")progressval=0.0f;
							ImGui::PushStyleColor(ImGuiCol_PlotHistogram,ImVec4(1.0f,1.0f,1.0f,0.6f));
							ImGui::ProgressBar(progressval,ImVec2(-1.0f,15),"");
							ImGui::PopStyleColor(1);
							ImGui::Separator();
							if (selected)
							ImGui::SetItemDefaultFocus();
						
						}
						if (*first_item) {
							std::string itemid = "##" + std::to_string(0);
							ImGui::SetFocusID(ImGui::GetID(itemid.c_str()), ImGui::GetCurrentWindow());
							*first_item = false;
						}
					}
				}
				if(item.popupstate == POPUP_STATE_NONE){
					ImGui::SetWindowFocus();
				}
				ImGui::EndTable();
				
				ImGui::EndChild();
				
				
				ImGui::BeginChild("##helpchild",ImVec2(total_w,ImGui::GetContentRegionAvail().y));
				ImGuiWindow* window = ImGui::GetCurrentWindow();
				
				ImGui::Dummy(ImVec2(0,5));
				ImVec2 startpos =  ImGui::GetCursorScreenPos();
				ImGui::Dummy(ImVec2(0,5));
				ImGui::Text(FONT_DPADUP_BUTTON_FILLED);
				ImGui::SameLine();
				ImGui::Text(FONT_DPADDOWN_BUTTON_FILLED);
				ImGui::SameLine();
				ImGui::Text(Common_STR[NXCOMMON_NAVIGATION]);
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
				ImGui::Text(FONT_DPADLEFT_BUTTON_FILLED);
				ImGui::SameLine();
				ImGui::Text(Common_STR[NXCOMMON_CHECKBOX]);
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
				ImGui::Text(FONT_A_BUTTON_FILLED);
				ImGui::SameLine();
				ImGui::Text(Common_STR[NXCOMMON_SELECTPLAY]);
				if(enigma2->getCurrBouquet().bouquetref != ""){
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
					ImGui::Text(FONT_B_BUTTON_FILLED);
					ImGui::SameLine();
					ImGui::Text(Common_STR[NXCOMMON_BACK]);
				}
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
				ImGui::Text(FONT_X_BUTTON_FILLED);
				ImGui::SameLine();
				ImGui::Text(Common_STR[NXCOMMON_CONTEXTMENU]);
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
				ImGui::Text(FONT_Y_BUTTON_FILLED);
				ImGui::SameLine();
				ImGui::Text(Common_STR[NXCOMMON_HOME]);
				
				window->DrawList->AddLine(startpos,ImVec2(startpos.x+1280*multiplyRes,startpos.y) , ImGui::GetColorU32(ImVec4(1.0f,1.0f,1.0f,1.0f)), 1.0f);
				
				ImGui::EndChild();
				
				
				/*
				
				ImGui::BeginChild("##helpchild",ImVec2(total_w,total_h-30-2*ImGui::GetStyle().FramePadding.y*multiplyRes));
				GUI::NXMPImage((void*)(intptr_t)imgloader->icons.GUI_D_UP.id, ImVec2(30,30));
				ImGui::SameLine();
				GUI::NXMPImage((void*)(intptr_t)imgloader->icons.GUI_D_DOWN.id, ImVec2(30,30));
				ImGui::SameLine();
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Navigation");
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
				GUI::NXMPImage((void*)(intptr_t)imgloader->icons.GUI_A_BUT.id, ImVec2(30,30));
				ImGui::SameLine();
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Select");
				if(enigma2->getCurrBouquet().bouquetref != ""){
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
					GUI::NXMPImage((void*)(intptr_t)imgloader->icons.GUI_B_BUT.id, ImVec2(30,30));
					ImGui::SameLine();
					ImGui::AlignTextToFramePadding();
					ImGui::Text("Back");
				}
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
				GUI::NXMPImage((void*)(intptr_t)imgloader->icons.GUI_Y_BUT.id, ImVec2(30,30));
				ImGui::SameLine();
				ImGui::AlignTextToFramePadding();
				ImGui::Text("NXMP Home");
				
				
				ImGui::EndChild();
				
				*/
				
			}
			ImGui::PopStyleColor(2);
			ImGui::PopStyleVar();
		}
		Windows::ExitWindow();
	}
}