#include "consolewindow.h"

CConsoleWindow::CConsoleWindow(std::string c_name, size_t mybufferSize){
	consoletitle = c_name;
	bufferSize = mybufferSize; 
	
}


void CConsoleWindow::Draw(){
	
	ImGui::SetNextWindowPos(ImVec2(0, 720.0/2.0));
	ImGui::SetNextWindowSize(ImVec2(1280, (720.0/2.0)-ImGui::GetStyle().FramePadding.y*2));
	 if (!ImGui::Begin("##ConsoleLog", nullptr, ImGuiWindowFlags_NoFocusOnAppearing))
    {
	    ImGui::End();
        return;
    }
	
	float total_w = ImGui::GetContentRegionAvail().x;
	float total_h = ImGui::GetContentRegionAvail().y;
	if (ImGui::BeginListBox("Console",ImVec2(total_w, total_h))){
		for(int i=0;i<textbuffer.size();i++){
			ImGui::TextWrapped("%s",textbuffer.at(i).c_str());
		}
		
	}
	ImGui::SetScrollHereY(1.0f);
	ImGui::EndListBox();
			
	
	
	ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow()); 
	
	
	ImGui::End();
	
	
}

void CConsoleWindow::AddText(std::string _text){
	textbuffer.push_back(_text);
	if(textbuffer.size()>bufferSize){
			textbuffer.pop_front();
	}
	
}