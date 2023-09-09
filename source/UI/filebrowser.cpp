#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"


namespace Windows {
	
	float playertextscrollpos = 0.0f;
	int waitpos = 0;
	
	
	bool playertextforwardscroll = true;
	bool playertextlaststate = true;
	
	void BrowserScrollText(float w,float h,const char* fmt, ...){
		
		ImGui::BeginChild("###scrollable", ImVec2(w, h), false);
		va_list args;
		va_start(args, fmt);
		ImGui::TextV(fmt, args);
		va_end(args);
		
		float scroll_max_x = ImGui::GetScrollMaxX();
		if(playertextscrollpos>=scroll_max_x){
			playertextforwardscroll=false;
			if(!playertextlaststate==playertextforwardscroll){
				waitpos = 0;
			}
		}else if(playertextscrollpos<=0) {
			playertextforwardscroll=true;	
			if(!playertextlaststate==playertextforwardscroll){
				waitpos = 0;
			}
		}
		if(waitpos <= 60){
			waitpos++;
		}else{
			if(playertextforwardscroll){
				playertextscrollpos+=0.5;
			}else{
				playertextscrollpos-=0.5;
			}
		}
		
		playertextlaststate = playertextforwardscroll;
		ImGui::SetScrollX(playertextscrollpos);
		ImGui::EndChild();
	}
	
	
	void UniBrowserWindow(bool *focus, bool *first_item) {
		 Windows::SetupWindow();
		 if (ImGui::Begin(filebrowser->getTitle().c_str(), nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
            
			if (ImGui::BeginMenuBar()) {
				ImGui::Text("current path: %s",filebrowser->getCurrentPath().c_str());
				ImGui::EndMenuBar();
			}
			float total_w = ImGui::GetContentRegionAvail().x;
			float total_h = ImGui::GetContentRegionAvail().y;
			std::vector<FS::FileEntry> thislist = filebrowser->getCurrList();
			bool triggerselect = false;
			
			
			if (ImGui::BeginTable("table1", 3)){
				ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthFixed, (950.0f-2 * ImGui::GetStyle().ItemSpacing.x)*multiplyRes); // Default to 100.0f
				ImGui::TableSetupColumn("size", ImGuiTableColumnFlags_WidthFixed, 130.0f*multiplyRes); // Default to 200.0f
				ImGui::TableSetupColumn("date", ImGuiTableColumnFlags_WidthFixed,200.f*multiplyRes);       // Default to auto
           
				ImGuiListClipper clipper;
				clipper.Begin(thislist.size());
				while (clipper.Step())
				{
					for (unsigned int n = clipper.DisplayStart; n < clipper.DisplayEnd; n++){
						static int selected = -1;
						
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						std::string itemid = "##" + std::to_string(n);
						
						ImGui::Dummy(ImVec2(0,ImGui::CalcTextSize("A").y + ImGui::GetStyle().FramePadding.y * 2));
						ImGui::SameLine();
						
						if(item.selectionstate == FILE_SELECTION_CHECKBOX){
							if(thislist[n].type != FS::FileEntryType::Directory){
								if(ImGui::Checkbox(itemid.c_str(), filebrowser->checked(n))){
									
								}
								ImGui::SameLine();
							}
							
						}
						
						if(thislist[n].type == FS::FileEntryType::Directory){
							GUI::NXMPImage((void*)(intptr_t)nxmpicons.FolderTexture.id, ImVec2(30,30));
						}else{
							GUI::NXMPImage((void*)(intptr_t)nxmpicons.FileTexture.id, ImVec2(30,30));
						}
						ImGui::SameLine();
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() +5.0f);
						//std::string itemid = "##" + std::to_string(n);
						ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns;
						
						if(item.selectionstate != FILE_SELECTION_CHECKBOX){
							if (ImGui::Selectable(itemid.c_str(), selected == n,selectable_flags)){
									if(filebrowser->getCurrList()[n].type == FS::FileEntryType::Directory){
									triggerselect = true;
									filebrowser->DirList(thislist[n].path,configini->getshowHidden(false),Utility::getMediaExtensions());
								} else{
									item.laststate = item.state;
									playlist->clearPlaylist();
									filebrowser->clearChecked();
									libmpv->loadFile(filebrowser->getOpenUrlPart()+thislist[n].path);
									if(configini->getDbActive(true)){
										libmpv->getFileInfo()->resume = sqlitedb->getResume(filebrowser->getOpenUrlPart()+thislist[n].path);
										if(libmpv->getFileInfo()->resume>0){
											item.popupstate = POPUP_STATE_RESUME;
										}
									}
								}
							}
						}else{
							ImGui::Dummy(ImVec2(0,ImGui::CalcTextSize("A").y + ImGui::GetStyle().FramePadding.y * 2));
							ImGui::SameLine();
						
						}
						if (ImGui::IsItemHovered()){
							if(item.fileHoveredidx != n){
								playertextscrollpos=0.0f;
								waitpos = 0;
							}
							item.fileHoveredidx = n;
						}
						ImVec4* colors = ImGui::GetStyle().Colors;
						ImVec4 textcolor = colors[ImGuiCol_Text];
						if(sqlitedb != nullptr){
							int dbfilestatus = sqlitedb->getFileDbStatus(thislist[n].path);
							if(dbfilestatus == 2){
								textcolor = ImVec4(0.0f,1.0f,0.0f,1.0f);
							}
							if(dbfilestatus == 1){
								textcolor = ImVec4(0.0f,1.0f,1.0f,1.0f);
							}
								
						}
						ImGui::SameLine();
						if(item.fileHoveredidx == n){
							BrowserScrollText((950.0f-2 * ImGui::GetStyle().ItemSpacing.x)*multiplyRes,30.0f*multiplyRes,"%s",thislist[n].name.c_str());
						}else{
							ImGui::TextColored(textcolor,"%s",thislist[n].name.c_str());
						}
						ImGui::TableSetColumnIndex(1);
						if(thislist[n].type == FS::FileEntryType::File){
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() +5.0f*multiplyRes);
							std::string strsize = Utility::humanSize(thislist[n].size);
							ImVec2 textSize = ImGui::CalcTextSize(strsize.c_str());
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() -textSize.x- ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x);
							ImGui::TextColored(textcolor,"%s",strsize.c_str());
						}
						
						ImGui::TableSetColumnIndex(2);
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() +5.0f*multiplyRes);
						
						if(thislist[n].is_valid){
							std::string strdate = Utility::formatTimeStamp(thislist[n].modified);
							ImVec2 textSize = ImGui::CalcTextSize(strdate.c_str());
							ImGui::TextColored(textcolor,"%s",strdate.c_str());
						}
						
					}
				}
				ImGui::EndTable();
			}
			if (*first_item && thislist.size() >0) {
				std::string itemid = "##" + std::to_string(0);
				ImGui::SetFocusID(ImGui::GetID(itemid.c_str()), ImGui::GetCurrentWindow());
				ImGui::SetScrollY(0.0f);
				*first_item = false;
			}
			if(*focus){
				if(item.selectionstate == FILE_SELECTION_NONE){
					std::string itemid = "##" + std::to_string(item.fileHoveredidx);
					ImGui::SetFocusID(ImGui::GetID((itemid.c_str())), ImGui::GetCurrentWindow());
				}
				if(item.selectionstate == FILE_SELECTION_CHECKBOX){
					std::string checkitemid = "##check" + std::to_string(item.fileHoveredidx);
					ImGui::SetFocusID(ImGui::GetID((checkitemid.c_str())), ImGui::GetCurrentWindow());
				}
				*focus = false;
			}
			if(triggerselect == true){
				*first_item = true;
			}
				
			
        }
        Windows::ExitWindow();
		 
	}
	
    void FileBrowserWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
		std::vector<std::string> topmenu = {"Local Files","Network","Enigma2"};
		
        if (ImGui::Begin("File Browser", nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
            if(item.popupstate == POPUP_STATE_NONE){
				ImGui::SetNextWindowFocus();
			}
            
			if (ImGui::BeginMenuBar()) {
				ImGui::Text("current path: %s",filebrowser->getCurrentPath().c_str());
				ImGui::EndMenuBar();
			}
			float total_w = ImGui::GetContentRegionAvail().x;
			float total_h = ImGui::GetContentRegionAvail().y;
			if (ImGui::BeginListBox("File Browser Menu",ImVec2(total_w, total_h))){
				
				std::vector<FS::FileEntry> thislist = filebrowser->getCurrList();
				bool triggerselect = false;
				for (unsigned int n = 0; n < thislist.size(); n++){
					
					static int selected = -1;
					
						if(thislist[n].type == FS::FileEntryType::Directory){
							GUI::NXMPImage((void*)(intptr_t)nxmpicons.FolderTexture.id, ImVec2(40,40));
						}else{
							GUI::NXMPImage((void*)(intptr_t)nxmpicons.FileTexture.id, ImVec2(40,40));
						}
						ImGui::SameLine();
						std::string itemid = "##" + std::to_string(n);
						if(item.selectionstate == FILE_SELECTION_CHECKBOX){
							if(thislist[n].type == FS::FileEntryType::Directory){
								ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40*multiplyRes - ImGui::GetFont()->FontSize) / 2});
								if (ImGui::Selectable(itemid.c_str(), selected == n)){
									triggerselect = true;
									filebrowser->DirList(thislist[n].path,configini->getshowHidden(false),Utility::getMediaExtensions());
								}
							}else{
								ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40*multiplyRes - ImGui::GetFont()->FontSize - ImGui::GetStyle().FramePadding.y * 2) / 2});
								std::string checkitemid = "##check" + std::to_string(n);
								if(thislist[n].type != FS::FileEntryType::Directory){
									if(ImGui::Checkbox(checkitemid.c_str(), filebrowser->checked(n))){
										if(*filebrowser->checked(n)){
											playlist->appendFile(thislist[n],thislist[n].path);
										} else {
											playlist->removeFile(thislist[n],thislist[n].path);
										}
									}
								}
							}
							if (ImGui::IsItemHovered()){
								item.fileHoveredidx = n;
							}
						}
						if(item.selectionstate == FILE_SELECTION_NONE){
							ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + (40*multiplyRes - ImGui::GetFont()->FontSize) / 2});
							if (ImGui::Selectable(itemid.c_str(), selected == n)){
								if(filebrowser->getCurrList()[n].type == FS::FileEntryType::Directory){
									triggerselect = true;
									filebrowser->DirList(thislist[n].path,configini->getshowHidden(false),Utility::getMediaExtensions());
								}
								else{
									item.laststate = item.state;
									playlist->clearPlaylist();
									filebrowser->clearChecked();
									libmpv->loadFile(thislist[n].path);
									if(configini->getDbActive(true)){
										libmpv->getFileInfo()->resume = sqlitedb->getResume(thislist[n].path);
										if(libmpv->getFileInfo()->resume>0){
											item.popupstate = POPUP_STATE_RESUME;
										}
									}
								}
							}
							if (ImGui::IsItemHovered()){
								item.fileHoveredidx = n;
							}
						}
						ImVec4* colors = ImGui::GetStyle().Colors;
						ImVec4 textcolor = colors[ImGuiCol_Text];
						if(sqlitedb != nullptr){
							int dbfilestatus = sqlitedb->getFileDbStatus(thislist[n].path);
							if(dbfilestatus == 2){
								textcolor = ImVec4(0.0f,1.0f,0.0f,1.0f);
							}
							if(dbfilestatus == 1){
								textcolor = ImVec4(0.0f,1.0f,1.0f,1.0f);
							}
							
						}
						ImGui::SameLine();
						ImGui::TextColored(textcolor,"%s",thislist[n].name.c_str());
						if(thislist[n].type == FS::FileEntryType::File){
							ImGui::SameLine(total_w-150);
							ImGui::TextColored(textcolor,"%s",Utility::humanSize(thislist[n].size).c_str());
						}
						
						
						if(item.selectionstate == FILE_SELECTION_CHECKBOX && thislist[n].type == FS::FileEntryType::File){
							ImGui::Dummy(ImVec2(0.0,0.0));
							ImGui::SetCursorPos({ImGui::GetCursorPos().x, ImGui::GetCursorPos().y-1.0f});
							
						}
					if (selected)
						ImGui::SetItemDefaultFocus();
				}
				if (*first_item && thislist.size() >0) {
					std::string itemid = "##" + std::to_string(0);
					ImGui::SetFocusID(ImGui::GetID(itemid.c_str()), ImGui::GetCurrentWindow());
					ImGui::SetScrollY(0.0f);
					*first_item = false;
				}
				if(*focus){
					if(item.selectionstate == FILE_SELECTION_NONE){
						std::string itemid = "##" + std::to_string(item.fileHoveredidx);
						ImGui::SetFocusID(ImGui::GetID((itemid.c_str())), ImGui::GetCurrentWindow());
					}
					if(item.selectionstate == FILE_SELECTION_CHECKBOX){
						std::string checkitemid = "##check" + std::to_string(item.fileHoveredidx);
						ImGui::SetFocusID(ImGui::GetID((checkitemid.c_str())), ImGui::GetCurrentWindow());
					}
					*focus = false;
				}
				if(triggerselect == true){
					*first_item = true;
				}
			}
			ImGui::EndListBox();		
			
        }
        Windows::ExitWindow();
    }
	
	void USBMountWindow(bool *focus, bool *first_item){
		Windows::SetupWindow();
		
        if (ImGui::Begin("USB Mount", nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
            ImGui::SetNextWindowFocus();
            	float total_w = ImGui::GetContentRegionAvail().x;
				float total_h = ImGui::GetContentRegionAvail().y;
				if (ImGui::BeginListBox("USB Browser Menu",ImVec2(total_w, total_h))){
					bool triggerselect = false;
					static int selected = -1;	
					//std::vector<usb_devices> thislist = usbmounter->mounted_devs;
					std::vector<usb_devices> thislist = filebrowser->getUsbDev();
					for (unsigned int n = 0; n < thislist.size(); n++){
						std::string itemid = "##" + std::to_string(n);
						ImGui::SameLine();
						if (ImGui::Selectable(itemid.c_str(), selected == n,0, ImVec2(0, 60))){
							//item.usbpath = thislist[n].mount_point + std::string("/");
							filebrowser->setBasePath(thislist[n].mount_point + std::string("/"));
							filebrowser->DirList(filebrowser->getBasePath(),configini->getshowHidden(false),Utility::getMediaExtensions());
							triggerselect = true;
							item.state = MENU_STATE_USB_BROWSER;
						}
						ImGui::SameLine();
						float currstartYpos = ImGui::GetCursorPosY();
						ImGui::SetCursorPosY(ImGui::GetCursorPosY()+10.0f);
						ImGui::Image((void*)(intptr_t)nxmpicons.UsbTexture.id, ImVec2(40,40));
						ImGui::SameLine();
						ImGui::SetCursorPosY(currstartYpos);
						float currstartXpos = ImGui::GetCursorPosX();
						std::string devname = Utility::trim(thislist[n].name)  + std::string(" -> ") + thislist[n].mount_point;
						ImGui::Text("%s",devname.c_str());
						ImGui::SetCursorPosY(ImGui::GetCursorPosY()-30.0f);
						ImGui::SetCursorPosX(currstartXpos);
						ImGui::Text("%s %s",thislist[n].fstype.c_str(),Utility::humanSize(thislist[n].capacity).c_str());
						
					
						if (selected)
							ImGui::SetItemDefaultFocus();
					}
					if (*first_item) {
						std::string itemid = "##" + std::to_string(0);
						ImGui::SetFocusID(ImGui::GetID(itemid.c_str()), ImGui::GetCurrentWindow());
						*first_item = false;
					}
					if(triggerselect == true){
						*first_item = true;
					}
				}	
				ImGui::EndListBox();	
        }
        Windows::ExitWindow();
	}
}