#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"


namespace Windows {
#ifdef NXMP_ENIGMASUPPORT
    void EnigmaWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		std::vector<std::string> topmenu = {"Local Files","Network","Enigma2"};
		
        if (ImGui::Begin("Enigma2", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
			ImGui::SetNextWindowFocus();
			if (ImGui::BeginMenuBar()) {
				ImGui::Text("%s",enigma2->getCurrBouquet().name.c_str());
				ImGui::EndMenuBar();
			}
			float total_w = ImGui::GetContentRegionAvail().x;
			float total_h = ImGui::GetContentRegionAvail().y;
			if (ImGui::BeginListBox("Enigma2 Browser Menu",ImVec2(total_w, total_h))){
				if(enigma2->getCurrBouquet().bouquetref == ""){
					for (unsigned int n = 0; n < enigma2->e2services.size(); n++){
						static int selected = -1;
						
						if (ImGui::Selectable(enigma2->e2services[n].name.c_str(), selected == n)){
			
							enigma2->m3uParser((char *)enigma2->e2services[n].bouquetref.c_str());
							enigma2->setCurrBouquet(enigma2->e2services[n]);
						}
						if (selected)
							ImGui::SetItemDefaultFocus();
							
					}
					if (*first_item) {
						ImGui::SetFocusID(ImGui::GetID(enigma2->e2services[0].name.c_str()), ImGui::GetCurrentWindow());
						*first_item = false;
					}
				}else if(enigma2->getCurrBouquet().bouquetref != ""){
					for (unsigned int n = 0; n < enigma2->e2currbouqet.size(); n++){
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
						ImGui::PushFont(fontSmall);
						ImGui::Text(u8"%s",enigma2->e2currbouqet[n].epg.title.c_str());
						ImGui::PopFont();
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
			//ImGuiContext& g = *GImGui;
			//ImGui::NavMoveRequestTryWrapping(g.CurrentWindow, ImGuiNavMoveFlags_LoopY);
			ImGui::EndListBox();
		}
		Windows::ExitWindow();
	}
#endif
}