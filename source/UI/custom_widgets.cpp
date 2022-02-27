#include "custom_widgets.h"

void batteryIcon(ImVec2 pos,bool absolute,uint32_t batlevel){
	
	ImVec2 iconsize = ImVec2(50.0f, 20.0f);
	
	ImVec2 canvas_p0 = ImGui::GetCursorScreenPos(); 
	if(absolute){
		canvas_p0 = pos;
	}
	ImVec2 canvas_pnew	= ImVec2(canvas_p0.x+3.0,canvas_p0.y+3.0f);
	ImVec2 canvas_p1 = ImVec2(canvas_p0.x+iconsize.x, canvas_p0.y + iconsize.y);
	ImVec2 batteryfill = ImVec2(canvas_p0.x+(float)batlevel*(iconsize.x-3.0f)/100.0f, canvas_p0.y-3.0 + iconsize.y);
	ImVec2 batteryicon_p0 = ImVec2(canvas_p1.x,canvas_p0.y+5.0f);
	ImVec2 batteryicon_p1 = ImVec2(canvas_p1.x+5.0,canvas_p0.y+15.0f);
	


	//ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImGui::GetForegroundDrawList()->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(0, 0, 0, 255));
	ImGui::GetForegroundDrawList()->AddRectFilled(canvas_pnew, batteryfill, IM_COL32(50, 255, 50, 255));
	ImGui::GetForegroundDrawList()->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));
	
	ImGui::GetForegroundDrawList()->AddRectFilled(batteryicon_p0, batteryicon_p1, IM_COL32(255, 255, 255, 255));

	
}