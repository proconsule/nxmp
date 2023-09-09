#ifndef NXMP_UIHELPERS_H
#define NXMP_UIHELPERS_H

namespace GUI{
	void newbatteryIcon(ImVec2 pos,bool absolute,uint32_t batlevel, float w, float h,bool showperc);
	
}

namespace Windows {
	std::string InputSwitchKeyboard(std::string uuid,std::string label,std::string data);
	void newbatteryIcon(ImVec2 pos,bool absolute,uint32_t batlevel, float w, float h,bool showperc);
}



#endif