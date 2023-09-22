#ifndef NXMP_TEXTSCROLLER_H
#define NXMP_TEXTSCROLLER_H

#include <string>
#include "imgui.h"
#include "imgui_internal.h"


class CTextScroller{
public:
	CTextScroller(std::string _uniqueid);
	~CTextScroller();
	
	void Draw(std::string myuniqueid,float w,float h,const char* fmt, ...);
	void DrawColor(std::string myuniqueid,const ImVec4& col,float w,float h,const char* fmt, ...);
	
	void ResetPosition();
	
	std::string uniqueid;
	
	
	float textscrollpos = 0.0f;
	int waitpos = 0;
	
	float scroll_max_x;
	
	bool textforwardscroll = true;
	bool textlaststate = true;
	
	
};


#endif