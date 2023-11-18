#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"

#include "nxmp-gfx.h"

namespace GUI {
	
	// For auto scale images using multiplyRes
	void NXMPImage(ImTextureID user_texture_id, const ImVec2& size){
		ImVec2 newsize = ImVec2(size.x*multiplyRes,size.y*multiplyRes);
#ifdef OPENGL_BACKEND
		//size.x = size.x*multiplyRes;
		//size.y = size.y*multiplyRes;
		ImGui::Image((void*)(intptr_t)user_texture_id, newsize);
#endif
#ifdef DEKO3D_BACKEND
		ImGui::Image(user_texture_id, newsize);
		//ImGui::Image(ImGui::GetIO().Fonts->TexID, newsize);
		
#endif

	}
	
	void cloktimeText(ImVec2 pos,bool absolute,std::string mytext){
		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos(); 
		if(absolute){
			canvas_p0 = pos;
		}
		
		ImGui::GetForegroundDrawList()->AddText(canvas_p0, ImColor(255.0f,255.0f,255.0f,255.0f),mytext.c_str());
		
		
	}
	
	void newbatteryIcon(ImVec2 pos,bool absolute,uint32_t batlevel, float w, float h,bool showperc){
	
		//ImVec2 iconsize = ImVec2(50.0f, 20.0f);
		ImVec2 iconsize = ImVec2(w, h);

		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos(); 
		if(absolute){
			canvas_p0 = pos;
		}
		ImVec2 canvas_pnew	= ImVec2(canvas_p0.x+3.0,canvas_p0.y+3.0f);
		ImVec2 canvas_p1 = ImVec2(canvas_p0.x+iconsize.x, canvas_p0.y + iconsize.y);
		ImVec2 batteryfill = ImVec2(canvas_p0.x+(float)batlevel*(iconsize.x-3.0f)/100.0f, canvas_p0.y-3.0 + iconsize.y);
		ImVec2 batteryicon_p0 = ImVec2(canvas_p1.x,canvas_p0.y+iconsize.y/4);
		ImVec2 batteryicon_p1 = ImVec2(canvas_p1.x+5.0*multiplyRes,canvas_p0.y+(iconsize.y/4)*3);
		


		//ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImGui::GetForegroundDrawList()->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(0, 0, 0, 255));
		//green
		if(batlevel >= 66)
		ImGui::GetForegroundDrawList()->AddRectFilled(canvas_pnew, batteryfill, IM_COL32(50, 255, 50, 255));
		//yellow
		else if(batlevel >= 33 )
		ImGui::GetForegroundDrawList()->AddRectFilled(canvas_pnew, batteryfill, IM_COL32(255, 204, 0, 255));
		//red
		else if(batlevel >= 0 )
		ImGui::GetForegroundDrawList()->AddRectFilled(canvas_pnew, batteryfill, IM_COL32(204, 51, 0, 255));

		ImGui::GetForegroundDrawList()->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));
		
		ImGui::GetForegroundDrawList()->AddRectFilled(batteryicon_p0, batteryicon_p1, IM_COL32(255, 255, 255, 255));
		
		char batterylevelstr[32];
		sprintf(batterylevelstr,"%d%%",batlevel);
		if(showperc){
			ImGui::GetForegroundDrawList()->AddText(ImVec2(canvas_p0.x+w+(10.0f*multiplyRes),canvas_p0.y), ImColor(255.0f,255.0f,255.0f,255.0f),batterylevelstr);
		}
		
		
	
	}
	
}


namespace Windows {
	
	SwkbdTextCheckResult validate_text_ISINT(char* tmp_string, size_t tmp_string_size,bool numeric) {
		std::string s = tmp_string;
		bool checkstring = !s.empty() && std::find_if(s.begin(), 
        s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
		
		if (!checkstring) {
			return SwkbdTextCheckResult_Bad;
		}

		return SwkbdTextCheckResult_OK;
	}
	
	
	std::string ShowSWKeyboard(std::string InitialValueStr,std::string headertext,bool numeric){
		SwkbdConfig kbd;
		int rc = 0;
		char tmpoutstr[16] = {0};
		rc = swkbdCreate(&kbd, 0);
		printf("swkbdCreate(): 0x%x\n", rc);

		if (R_SUCCEEDED(rc)) {
			// Select a Preset to use, if any.
			swkbdConfigMakePresetDefault(&kbd);
			if(numeric){
				swkbdConfigSetType(&kbd, SwkbdType_NumPad);
			}
			//swkbdConfigMakePresetPassword(&kbd);
			//swkbdConfigMakePresetUserName(&kbd);
			//swkbdConfigMakePresetDownloadCode(&kbd);

			// Optional, set any text if you want (see swkbd.h).
			//swkbdConfigSetOkButtonText(&kbd, "Submit");
			//swkbdConfigSetLeftOptionalSymbolKey(&kbd, "a");
			//swkbdConfigSetRightOptionalSymbolKey(&kbd, "b");
			if(headertext.size()>0){
				swkbdConfigSetHeaderText(&kbd, headertext.c_str());
			}
			//swkbdConfigSetSubText(&kbd, "Sub");
			//swkbdConfigSetGuideText(&kbd, "Guide");
			if(numeric){
				swkbdConfigSetTextCheckCallback(&kbd, validate_text_ISINT);//Optional, can be removed if not using TextCheck.
			}
			
			//swkbdConfigSetTextCheckCallback(&kbd, validate_text);//Optional, can be removed if not using TextCheck.

			// Set the initial string if you want.
			//swkbdConfigSetOkButtonText(&kbd, "Submit");
			swkbdConfigSetInitialText(&kbd, InitialValueStr.c_str());

			// You can also use swkbdConfigSet*() funcs if you want.

			rc = swkbdShow(&kbd, tmpoutstr, sizeof(tmpoutstr));
			
			if (R_SUCCEEDED(rc)) {
				swkbdClose(&kbd);
				return tmpoutstr;
			}
			swkbdClose(&kbd);
			
		}
		return InitialValueStr;
	}		
	
	std::string InputSwitchKeyboard(std::string uuid,std::string label,std::string data){
		ImGui::Text(label.c_str());
		ImGui::SameLine();
		float currstartpos = ImGui::GetCursorPosX();
		ImGui::Text(data.c_str());
		ImGui::SetCursorPosX(currstartpos);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY()-26.0f*multiplyRes);
		if (ImGui::Selectable(uuid.c_str(), false)){
				SwkbdConfig kbd;
				char tmpoutstr[256] = {0};
				Result rc = swkbdCreate(&kbd, 0);
				
				if (R_SUCCEEDED(rc)) {
					// Select a Preset to use, if any.
					swkbdConfigMakePresetDefault(&kbd);
					//swkbdConfigMakePresetPassword(&kbd);
					//swkbdConfigMakePresetUserName(&kbd);
					//swkbdConfigMakePresetDownloadCode(&kbd);

					// Optional, set any text if you want (see swkbd.h).
					//swkbdConfigSetOkButtonText(&kbd, "Submit");
					//swkbdConfigSetLeftOptionalSymbolKey(&kbd, "a");
					//swkbdConfigSetRightOptionalSymbolKey(&kbd, "b");
					swkbdConfigSetHeaderText(&kbd, label.c_str());
					//swkbdConfigSetSubText(&kbd, "Sub");
					//swkbdConfigSetGuideText(&kbd, "Guide");

					//swkbdConfigSetTextCheckCallback(&kbd, validate_text);//Optional, can be removed if not using TextCheck.

					// Set the initial string if you want.
					swkbdConfigSetInitialText(&kbd, data.c_str());

					// You can also use swkbdConfigSet*() funcs if you want.

					//printf("Running swkbdShow...\n");
					rc = swkbdShow(&kbd, tmpoutstr, sizeof(tmpoutstr));
					//printf("swkbdShow(): 0x%x\n", rc);
					swkbdClose(&kbd);
					
					if (R_SUCCEEDED(rc)) {
						return tmpoutstr;
						//printf("out str: %s\n", tmpoutstr);
						//data=tmpoutstr;
						//printf("out str: %s\n", NewNetworkShare->address.c_str());
					}
					
				}
				
				
			}
			return data;
	}
	
}