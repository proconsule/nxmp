#ifndef NXMP_CONSOLEWINDOW_H
#define NXMP_CONSOLEWINDOW_H

#include <cstdlib>
#include <string>
#include <vector>
#include "imgui.h"
#include "imgui_internal.h"


class CConsoleWindow{
public:
	CConsoleWindow(std::string c_name = "imgui-console", size_t mybufferSize = 256);
	void AddText(std::string _text);
	
	void AddFmt(std::string _text);
	void Draw();
	
private:
	std::string consoletitle;
	std::vector<std::string> textbuffer;
	int bufferSize;
	
};





#endif