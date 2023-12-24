#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"
#include "utils.h"
#include "localfiles.h"
#include "Enigma2.h"

#include "nxmp-i18n.h"

namespace Windows {
	
	bool movenavfocus = false;
	std::string navfocusstring = "";
	
	void SetBrowserNav(std::string foldername){
		movenavfocus = true;
		navfocusstring = foldername;
	}
	
	void UniBrowserWindow(/*bool *focus, bool *first_item*/) {
		 Windows::SetupWindow();
		 if(BrowserTextScroller == nullptr){
			 BrowserTextScroller=new CTextScroller("##filescroller");
		 }
		GUI::cloktimeText(ImVec2((1180.0f*multiplyRes)-ImGui::CalcTextSize(nxmpstats->currentTime).x-(10.0*multiplyRes),2.0f*multiplyRes),true,nxmpstats->currentTime);
		GUI::newbatteryIcon(ImVec2(1180.0f*multiplyRes,2.0f*multiplyRes),true,batteryPercent,40,20,true);
		
		 if (ImGui::Begin(filebrowser->getTitle().c_str(), nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar )) {
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, nxmpgfx::NavHover_color);
			ImGui::PushStyleColor(ImGuiCol_NavHighlight, nxmpgfx::Active_color);
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {0, 5});
			
			if (ImGui::BeginMenuBar()) {
				ImGui::Text("current path: %s",filebrowser->getCurrentPath().c_str());
				ImGui::EndMenuBar();
			}
			float total_w = ImGui::GetContentRegionAvail().x;
			float total_h = ImGui::GetContentRegionAvail().y;
			std::vector<FS::FileEntry> thislist = filebrowser->getCurrList();
			bool triggerselect = false;
			

			if(!libmpv->Stopped() && item.state != MENU_STATE_PLAYER){
#ifdef OPENGL_BACKEND
				ImVec2 image_pnew	= ImVec2((1280.0*multiplyRes-300.0f*multiplyRes-2 * ImGui::GetStyle().ItemSpacing.x),(720.0*multiplyRes-200.0f*multiplyRes-2 * ImGui::GetStyle().ItemSpacing.y));
				ImVec2 image_p1 = ImVec2((1280.0*multiplyRes-2 * ImGui::GetStyle().ItemSpacing.x), (720.0*multiplyRes-2 * ImGui::GetStyle().ItemSpacing.y));
		
				image_pnew = image_pnew - ImVec2(10,10);
				image_p1 = image_p1 - ImVec2(10,10);
				
				ImVec2 image_pnew_border = image_pnew - ImVec2(1,1);
				ImVec2 image_p1_border = image_p1 + ImVec2(1,1);
				
				ImGui::GetForegroundDrawList()->AddRect(image_pnew_border, image_p1_border, ImGui::GetColorU32(ImVec4(1.0f,0.0f,0.0f,1.0f)), 0.0f);
				ImGui::GetForegroundDrawList()->AddImage((void*)(intptr_t)nxmpgfx::getFBO_Texture(), image_pnew, image_p1);
				
#endif
			}

			
			
			ImGui::BeginChild("##tablecontainer",ImVec2(total_w,total_h-45*multiplyRes));
			if(!filebrowser->connected){
				ImGui::SetCursorPosX( (ImGui::GetWindowWidth() - ImGui::CalcTextSize("Error Connecting").x) / 2.f);
				ImGui::TextColored(ImVec4(1.0f,0.0f,0.0f,1.0f),"Error Connecting");
				
				ImGui::SetCursorPosX( (ImGui::GetWindowWidth() - ImGui::CalcTextSize(filebrowser->errormsg.c_str()).x) / 2.f);
				ImGui::SetCursorPosY( (ImGui::GetWindowHeight() - ImGui::CalcTextSize(filebrowser->errormsg.c_str()).y) / 2.f);
				ImGui::Text("%s",filebrowser->errormsg.c_str());
			}else{
				if (ImGui::BeginTable("table1", 3,/*ImGuiTableFlags_RowBg|*/ImGuiTableFlags_ScrollY)){
					ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, (910.0f*multiplyRes -2 * ImGui::GetStyle().ItemSpacing.x)); 
					ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 125.0f*multiplyRes);
					ImGui::TableSetupColumn("Date", ImGuiTableColumnFlags_WidthFixed,255.f*multiplyRes);       
					ImGui::TableSetupScrollFreeze(0, 1);
					//ImGui::TableHeadersRow();
					ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
					ImGui::TableSetColumnIndex(0);
					ImGui::Text(Common_STR[NXCOMMON_NAME]);
					ImGui::TableSetColumnIndex(1);
					ImGui::Text(Common_STR[NXCOMMON_SIZE]);
					ImGui::TableSetColumnIndex(2);
					ImGui::Text(Common_STR[NXCOMMON_DATE]);
					
					ImGuiListClipper clipper;
					clipper.Begin(thislist.size());
					
					
					int navfocusid = 0;
					if(movenavfocus){
						for(int i=0;i<thislist.size();i++){
							//NXLOG::DEBUGLOG("%s %s",thislist[i].name.c_str(),navfocusstring.c_str());
						
							if(thislist[i].name == navfocusstring){
								navfocusid = i;
							}
						}
						//movenavfocus=false;
						clipper.ForceDisplayRangeByIndices( navfocusid, navfocusid+41 );
						 
					}
					
					bool changefolder = false;
					while (clipper.Step())
					{
						for (unsigned int n = clipper.DisplayStart; n < clipper.DisplayEnd; n++){
							static int selected = -1;
							
							if(movenavfocus){
								if(navfocusid == n){
									ImGui::SetKeyboardFocusHere();
									
								}
								
							}
							
							
							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							
							std::string itemid = "##" + std::to_string(n);
							
							ImGui::Dummy(ImVec2(0,30 + ImGui::GetStyle().FramePadding.y * 2));
							ImGui::SameLine();
							
							if(item.selectionstate == FILE_SELECTION_CHECKBOX){
								if(thislist[n].type != FS::FileEntryType::Directory){
									if(ImGui::Checkbox(itemid.c_str(), filebrowser->checked(n))){
										
									}
									ImGui::SameLine();
								}
								
							}
							
							if(thislist[n].type == FS::FileEntryType::Directory){
								GUI::NXMPImage((void*)(intptr_t)imgloader->icons.FolderTexture.id, ImVec2(30,30));
							}else{
								if(thislist[n].mediatype == FS::FileMediaType::Media){
									GUI::NXMPImage((void*)(intptr_t)imgloader->icons.FileTexture.id, ImVec2(30,30));
								}else if(thislist[n].mediatype == FS::FileMediaType::Image){
									GUI::NXMPImage((void*)(intptr_t)imgloader->icons.ImageTexture.id, ImVec2(30,30));
								}else if(thislist[n].mediatype == FS::FileMediaType::Archive){
									GUI::NXMPImage((void*)(intptr_t)imgloader->icons.ArchiveTexture.id, ImVec2(30,30));
								}
							}
							
							
							ImGui::SameLine();
							ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns;
							if(item.selectionstate != FILE_SELECTION_CHECKBOX){
								if (ImGui::Selectable(itemid.c_str(), selected == n,selectable_flags,ImVec2(0,30.0f*multiplyRes))){
										if(filebrowser->getCurrList()[n].type == FS::FileEntryType::Directory){
										triggerselect = true;
										changefolder = true;
										filebrowser->DirList(thislist[n].path,configini->getshowHidden(false),Utility::getMediaExtensions());
									} else{
										item.laststate = item.state;
										
										if(Utility::isImageExtension(filebrowser->getOpenUrlPart()+thislist[n].path)){
											//unsigned char * img_data = NULL;
											//int img_size=0;
											/*
											if(filebrowser->getfileContents(thislist[n].path,&img_data,img_size)){
												NXLOG::DEBUGLOG("filesize %d\n",img_size);
												item.state = MENU_STATE_IMGVIEWER;
												Windows::setImageZoom(1.0f);
												//Windows::currentImg =  imgloader->OpenImageMemory(img_data,img_size);
											}
											if(img_data!=NULL)free(img_data);
											*/
											if(filebrowser->getfileContentsThreaded(thislist[n].path)){
												item.state = MENU_STATE_IMGVIEWER;
												Windows::setImageZoom(1.0f);
												Windows::loadimage = true;
											}
											
										}else if(Utility::isArchiveExtension(filebrowser->getOpenUrlPart()+thislist[n].path)){
											filebrowser->OpenArchive(filebrowser->getOpenUrlPart()+thislist[n].path);
											filebrowser->DirList(filebrowser->getBasePath(),configini->getshowHidden(false),Utility::getMediaExtensions());
										}else{
										
											playlist->clearPlaylist();
											filebrowser->clearChecked();
											NXLOG::DEBUGLOG("OPEN FILE: %s\n",(filebrowser->getOpenUrlPart()+thislist[n].path).c_str());
											libmpv->loadFile(filebrowser->getOpenUrlPart()+thislist[n].path);
											if(configini->getDbActive(true)){
												libmpv->getFileInfo()->resume = sqlitedb->getResume(filebrowser->getOpenUrlPart()+thislist[n].path);
												if(libmpv->getFileInfo()->resume>0){
													item.popupstate = POPUP_STATE_RESUME;
												}
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
									BrowserTextScroller->ResetPosition();
									
								}
								item.fileHoveredidx = n;
							}
							ImVec4* colors = ImGui::GetStyle().Colors;
							ImVec4 textcolor = colors[ImGuiCol_Text];
							if(sqlitedb != nullptr && thislist[n].dbread == -1){
								int dbfilestatus = sqlitedb->getFileDbStatus(filebrowser->getOpenUrlPart()+thislist[n].path);
								if(dbfilestatus == 2){
									textcolor = ImVec4(0.0f,1.0f,0.0f,1.0f);
								} else if(dbfilestatus == 1){
									textcolor = ImVec4(0.0f,1.0f,1.0f,1.0f);
								}
								filebrowser->SetFileDbStatus(n,dbfilestatus);
							}else if(sqlitedb != nullptr){
								if(thislist[n].dbread == 2){
									textcolor = ImVec4(0.0f,1.0f,0.0f,1.0f);
								} else if(thislist[n].dbread == 1){
									textcolor = ImVec4(0.0f,1.0f,1.0f,1.0f);
								}
							}
							ImGui::SameLine();
							//ImGui::SetCursorPosY(ImGui::GetCursorPosY() +5.0f);
							
							std::string bsid =std::string("##bs") + std::to_string(n);
							if(item.fileHoveredidx == n){
								ImGui::Dummy(ImVec2(-10.0,(30.0f*multiplyRes-2 * ImGui::GetStyle().ItemSpacing.y)));
								ImGui::SameLine();
								ImGui::SetCursorPosX(ImGui::GetCursorPosX()-10.0f);
								ImGui::SetCursorPosY(ImGui::GetCursorPosY() +5.0f*multiplyRes);
								BrowserTextScroller->DrawColor(bsid.c_str(),textcolor,(910.0f*multiplyRes -2* ImGui::GetStyle().ItemSpacing.x),(30.0f*multiplyRes-2 * ImGui::GetStyle().ItemSpacing.y),thislist[n].name.c_str());	
							}else{
								ImGui::Dummy(ImVec2(-10.0,(30.0f*multiplyRes-2 * ImGui::GetStyle().ItemSpacing.y)));
								
								ImGui::SameLine();
								ImGui::SetCursorPosX(ImGui::GetCursorPosX()-10.0f);
								ImGui::SetCursorPosY(ImGui::GetCursorPosY()+5.0f);
								ImGui::TextColored(textcolor,"%s",thislist[n].name.c_str());
							}
							ImGui::TableSetColumnIndex(1);
							if(thislist[n].type == FS::FileEntryType::File){
								ImGui::SetCursorPosY(ImGui::GetCursorPosY() +5.0f*multiplyRes);
								std::string strsize = Utility::humanSize(thislist[n].size);
								ImVec2 textSize = ImGui::CalcTextSize(strsize.c_str());
								ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() -textSize.x- ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x);
								ImGui::TextColored(textcolor,"%s",strsize.c_str());
							}else{
								ImGui::SetCursorPosY(ImGui::GetCursorPosY() +5.0f*multiplyRes);
							}
							
							ImGui::TableSetColumnIndex(2);
							//ImGui::SetCursorPosY(ImGui::GetCursorPosY() +5.0f*multiplyRes);
							
							if(thislist[n].is_valid){
								ImGui::SetCursorPosY(ImGui::GetCursorPosY() +5.0f*multiplyRes);
								std::string strdate = Utility::formatTimeStamp(thislist[n].modified);
								//ImVec2 textSize = ImGui::CalcTextSize(strdate.c_str());
								ImGui::TextColored(textcolor,"%s",strdate.c_str());
							}
							
							
						}
					}
					movenavfocus = false;
					if(changefolder){
						SetBrowserNav("");
						changefolder = false;
					}
					if(item.popupstate == POPUP_STATE_NONE){
						ImGui::SetWindowFocus();
					}
					ImGui::EndTable();
					
				}
			}
			ImGui::EndChild();
			ImGui::BeginChild("##helpchild",ImVec2(total_w,ImGui::GetContentRegionAvail().y));
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			
			ImGui::Dummy(ImVec2(0,5));
			ImVec2 startpos =  ImGui::GetCursorScreenPos();
			ImGui::Dummy(ImVec2(0,5));
			ImGui::Text(FONT_DPADUP_BUTTON_FILLED);
			ImGui::SameLine();
			ImGui::Text(FONT_DPADDOWN_BUTTON_FILLED);
			ImGui::SameLine();
			ImGui::Text(Common_STR[NXCOMMON_NAVIGATION]);
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
			ImGui::Text(FONT_DPADLEFT_BUTTON_FILLED);
			ImGui::SameLine();
			ImGui::Text(Common_STR[NXCOMMON_CHECKBOX]);
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
			ImGui::Text(FONT_A_BUTTON_FILLED);
			ImGui::SameLine();
			ImGui::Text(Common_STR[NXCOMMON_SELECTPLAY]);
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
			ImGui::Text(FONT_B_BUTTON_FILLED);
			ImGui::SameLine();
			ImGui::Text(Common_STR[NXCOMMON_BACK]);
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
			ImGui::Text(FONT_X_BUTTON_FILLED);
			ImGui::SameLine();
			ImGui::Text(Common_STR[NXCOMMON_CONTEXTMENU]);
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
			ImGui::Text(FONT_Y_BUTTON_FILLED);
			ImGui::SameLine();
			ImGui::Text(Common_STR[NXCOMMON_HOME]);
			
			window->DrawList->AddLine(startpos,ImVec2(startpos.x+1280*multiplyRes,startpos.y) , ImGui::GetColorU32(ImVec4(1.0f,1.0f,1.0f,1.0f)), 1.0f);
			
			ImGui::EndChild();
			
			/*
			
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
			*/
			
			
			
			ImGui::PopStyleColor(2);
			ImGui::PopStyleVar();
        }
        Windows::ExitWindow();
		 
	}

	void USBMountWindow(bool *focus, bool *first_item){
		Windows::SetupWindow();
#ifdef USB_USE_CALLBACK_SYSTEM
		if(MyUSBMount != nullptr)MyUSBMount->usbMscTestDevices();
#endif
        if (ImGui::Begin("USB Mount", nullptr, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, nxmpgfx::NavHover_color);
			ImGui::PushStyleColor(ImGuiCol_NavHighlight, nxmpgfx::Active_color);
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {0, 5});
			ImGui::SetNextWindowFocus();
            	float total_w = ImGui::GetContentRegionAvail().x;
				float total_h = ImGui::GetContentRegionAvail().y;
				ImGui::BeginChild("##tablecontainer",ImVec2(total_w,total_h-45*multiplyRes));
				//if (ImGui::BeginListBox("USB Browser Menu",ImVec2(total_w, total_h))){
					bool triggerselect = false;
					static int selected = -1;
					
					//std::vector<usb_devices> thislist = usbmounter->mounted_devs;
					std::vector<usb_devices> thislist = filebrowser->getUsbDev(configini->getEmuOverrides());
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
						ImGui::Image((void*)(intptr_t)imgloader->icons.UsbTexture.id, ImVec2(40,40));
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
				//}	
				//ImGui::EndListBox();
				
				ImGui::EndChild();
				ImGui::BeginChild("##helpchild",ImVec2(total_w,ImGui::GetContentRegionAvail().y));
				ImGuiWindow* window = ImGui::GetCurrentWindow();
				
				ImGui::Dummy(ImVec2(0,5));
				ImVec2 startpos =  ImGui::GetCursorScreenPos();
				ImGui::Dummy(ImVec2(0,5));
				ImGui::Text(FONT_DPADUP_BUTTON_FILLED);
				ImGui::SameLine();
				ImGui::Text(FONT_DPADDOWN_BUTTON_FILLED);
				ImGui::SameLine();
				ImGui::Text(Common_STR[NXCOMMON_NAVIGATION]);
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
				ImGui::Text(FONT_A_BUTTON_FILLED);
				ImGui::SameLine();
				ImGui::Text(Common_STR[NXCOMMON_SELECT]);
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX()+50.0f*multiplyRes);
				ImGui::Text(FONT_Y_BUTTON_FILLED);
				ImGui::SameLine();
				ImGui::Text(Common_STR[NXCOMMON_HOME]);
				
				window->DrawList->AddLine(startpos,ImVec2(startpos.x+1280*multiplyRes,startpos.y) , ImGui::GetColorU32(ImVec4(1.0f,1.0f,1.0f,1.0f)), 1.0f);
				
				ImGui::EndChild();
				
				ImGui::PopStyleColor(2);
				ImGui::PopStyleVar();
        }
        Windows::ExitWindow();
	}
}