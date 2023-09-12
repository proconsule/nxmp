#include "textscroller.h"


CTextScroller::CTextScroller(std::string _uniqueid){
	uniqueid = _uniqueid;
};

CTextScroller::~CTextScroller(){
	
	
}

void CTextScroller::ResetPosition(){
	textscrollpos = 0.0f;
	waitpos = 0;
}

void CTextScroller::Draw(float w,float h,const char* fmt, ...){
	ImGui::BeginChild(uniqueid.c_str(), ImVec2(w, h), false);
	va_list args;
	va_start(args, fmt);
	ImGui::TextV(fmt, args);
	va_end(args);
	
	float scroll_max_x = ImGui::GetScrollMaxX();
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
		
	textlaststate = textforwardscroll;
	ImGui::SetScrollX(textscrollpos);
	ImGui::EndChild();
	
	
	
	
}