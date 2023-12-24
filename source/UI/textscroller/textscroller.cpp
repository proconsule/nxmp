#include "textscroller.h"


CTextScroller::CTextScroller(std::string _uniqueid){
	uniqueid = _uniqueid;
	textscrollpos = 0.0f;
	waitpos = 0;
};

CTextScroller::~CTextScroller(){
	
	
}

void CTextScroller::ResetPosition(){
	textscrollpos = 0.0f;
	waitpos = 0;
}

void CTextScroller::Draw(std::string myuniqueid,float w,float h,const char* fmt, ...){
	ImGui::SetNextItemWidth(w);
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0,0));
	ImGui::BeginChild(uniqueid.c_str(), ImVec2(w, h), false,0);
	va_list args;
	va_start(args, fmt);
	size_t needed = snprintf(NULL, 0, fmt, args) + 1;
    char  *buffer = (char *)malloc(needed);
	sprintf(buffer, fmt, args);
	ImGui::TextV(fmt, args);
	va_end(args);
	
	ImVec2 textsize = ImGui::CalcTextSize(buffer);
	free(buffer);

	float scroll_max_x = ImGui::GetScrollMaxX();

	//if(textsize.x>w){
		
		if(textscrollpos>=scroll_max_x){
			textforwardscroll=false;
			if(!textlaststate==textforwardscroll){
				waitpos = 0;
			}
		}else if(textscrollpos<=0) {
			textforwardscroll=true;	
			if(!textlaststate==textforwardscroll){
				waitpos = 0;
			}
		}
		if(waitpos <= 60){
			waitpos++;
		}else{
			if(textforwardscroll){
				textscrollpos+=0.5;
			}else{
				textscrollpos-=0.5;
			}
		}
	//}

	textlaststate = textforwardscroll;
	ImGui::SetScrollX(textscrollpos);
	ImGui::EndChild();
	ImGui::PopStyleColor();
	
	
	
}
	

void CTextScroller::DrawColor(std::string myuniqueid,const ImVec4& col,float w,float h,const char* fmt, ...){
	ImGui::SetNextItemWidth(w);
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0,0));
	ImGui::BeginChild(uniqueid.c_str(), ImVec2(w, h), false,0);
	va_list args;
	va_start(args, fmt);
	size_t needed = snprintf(NULL, 0, fmt, args) + 1;
    char  *buffer = (char *)malloc(needed);
	sprintf(buffer, fmt, args);
	ImGui::TextColoredV(col, fmt, args);
	va_end(args);
	
	ImVec2 textsize = ImGui::CalcTextSize(buffer);
	free(buffer);

	float scroll_max_x = ImGui::GetScrollMaxX();

	if(textsize.x>w){
		
		if(textscrollpos>=scroll_max_x){
			textforwardscroll=false;
			if(!textlaststate==textforwardscroll){
				waitpos = 0;
			}
		}else if(textscrollpos<=0) {
			textforwardscroll=true;	
			if(!textlaststate==textforwardscroll){
				waitpos = 0;
			}
		}
		if(waitpos <= 60){
			waitpos++;
		}else{
			if(textforwardscroll){
				textscrollpos+=0.5;
			}else{
				textscrollpos-=0.5;
			}
		}
	}

	textlaststate = textforwardscroll;
	ImGui::SetScrollX(textscrollpos);
	ImGui::EndChild();
	ImGui::PopStyleColor();
	
}


void CTextScroller::DrawPopupNative(std::string myuniqueid,float w,float h,const char* fmt, ...){
	ImGui::SetNextItemWidth(w);
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0,0));
	ImGui::BeginChild(uniqueid.c_str(), ImVec2(w, h), false,0);
	
	va_list args;
	va_start(args, fmt);
	std::string message;
    va_list ap_copy;
    va_copy(ap_copy, args);
    size_t len = vsnprintf(0, 0, fmt, ap_copy);
    message.resize(len + 1);  // need space for NUL
    vsnprintf(&message[0], len + 1,fmt, args);
    message.resize(len);  // remove the NUL
	
	/*
	
	va_list args;
	va_start(args, fmt);
	size_t needed = snprintf(NULL, 0, fmt, args) + 1;
    char  *buffer = (char *)malloc(needed);
	sprintf(buffer, fmt, args);
	ImGui::TextV(fmt, args);
	va_end(args);
	*/
	ImVec2 textsize = ImGui::CalcTextSize(message.c_str());
	float centerposx = (w-textsize.x)/2.0f;
	float centerposy = (h-textsize.y)/2.0f;
	if(centerposx<0.0f)centerposx = 0.0f;
	ImGui::SetCursorPosX(ImGui::GetCursorPosX()+centerposx);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY()+centerposy);
	ImGui::Text("%s",message.c_str());
	

	float scroll_max_x = ImGui::GetScrollMaxX();

	//if(textsize.x>w){
		
		if(textscrollpos>=scroll_max_x){
			textforwardscroll=false;
			if(!textlaststate==textforwardscroll){
				waitpos = 0;
			}
		}else if(textscrollpos<=0) {
			textforwardscroll=true;	
			if(!textlaststate==textforwardscroll){
				waitpos = 0;
			}
		}
		if(waitpos <= 60){
			waitpos++;
		}else{
			if(textforwardscroll){
				textscrollpos+=0.5;
			}else{
				textscrollpos-=0.5;
			}
		}
	//}

	textlaststate = textforwardscroll;
	ImGui::SetScrollX(textscrollpos);
	ImGui::EndChild();
	ImGui::PopStyleColor();
	
}