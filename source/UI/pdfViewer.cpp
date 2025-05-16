#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"

#include "imgloader.h"
#include "nxmp-i18n.h"

namespace Windows {
	
	
	
	Texture pdf_currentImg;

	float pdf_imgzoom = 1.0f;
	float pdf_oldimgzoom = 1.0f;
	float pdf_zoomposx = 0.0f;
	float pdf_zoomposy = 0.0f;
	
	double pdf_imgControlsHide = 0.0f;
	
	bool pdf_zooming = false;
	
	ImVec2 pdf_imgscrollpos = ImVec2(0.0f,0.0f);
	
	double pdf_dAspectRatio = 1280.0f/720.0f;
	
	float pdf_textfadealpha = 1.0f;
	
	
	bool pdf_loadimage = false;
	
	
	void SetupPDFViewerWindow(void) {
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(1280.0f*multiplyRes, 720.0f*multiplyRes), ImGuiCond_Always);
        
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_WindowBg,Window_Bg_color);
    };
	
	void ExitPDFViewerWindow(void) {
        ImGui::End();
        ImGui::PopStyleVar(3);
		ImGui::PopStyleColor();
    };
	

	
	
	typedef struct {
		ImVec2 originalSize = ImVec2(0.0f,0.0f);
		double dPictureAspectRatio = 0.0;
		float zoomfactor = 1.0f;
		float nCenteringFactorX = 0.0f;
		float nCenteringFactorY = 0.0f;
		ImVec2 scrollPos = ImVec2(0.0f,0.0f);
		
		
		ImVec2 renderSize = ImVec2(0.0f,0.0f);
		ImVec2 newscrollPos = ImVec2(0.0f,0.0f);
		ImVec2 offsetScrolPos = ImVec2(0.0f,0.0f);
		
	}pdf_resize_struct;
	
	
	pdf_resize_struct pdfviewer_image;
	
	bool PDFMakeSizes(ImVec2 imgSrcSize,float zoomfactor,ImVec2 scrollPos){
		if(imgSrcSize.x == pdfviewer_image.originalSize.x && imgSrcSize.y == pdfviewer_image.originalSize.y && zoomfactor == pdfviewer_image.zoomfactor){
			pdfviewer_image.scrollPos = scrollPos;
			pdfviewer_image.newscrollPos = scrollPos;
			pdfviewer_image.offsetScrolPos = scrollPos;
			return false;
		}
		pdfviewer_image.originalSize = imgSrcSize;
		pdfviewer_image.dPictureAspectRatio = imgSrcSize.x/imgSrcSize.y;
		pdfviewer_image.scrollPos = scrollPos;
		
		
		int nNewHeight;
		int nNewWidth;
		
		if (pdfviewer_image.dPictureAspectRatio > pdf_dAspectRatio){
				nNewHeight = (int)((1280.0f*multiplyRes)/imgSrcSize.x*imgSrcSize.y);
				pdfviewer_image.nCenteringFactorY = ((720.0f*multiplyRes) - nNewHeight*zoomfactor) / 2;
				pdfviewer_image.nCenteringFactorX = 0.0f;
				pdfviewer_image.renderSize.x = (1280.0f*multiplyRes)*zoomfactor;
				pdfviewer_image.renderSize.y = nNewHeight*zoomfactor;
				
			} else if (pdfviewer_image.dPictureAspectRatio < pdf_dAspectRatio){
				nNewWidth =  (int)((720.0f*multiplyRes)/imgSrcSize.y*imgSrcSize.x);
				pdfviewer_image.nCenteringFactorX = ((1280.0f*multiplyRes) - nNewWidth*zoomfactor) / 2;
				pdfviewer_image.nCenteringFactorY = 0.0f;
				pdfviewer_image.renderSize.x = nNewWidth*zoomfactor;
				pdfviewer_image.renderSize.y = (720.0f*multiplyRes)*zoomfactor;
				
				
			} else{
				pdfviewer_image.nCenteringFactorX = 0.0f;
				pdfviewer_image.nCenteringFactorY = 0.0f;
				pdfviewer_image.renderSize.x = 1280.0f*multiplyRes*zoomfactor;
				pdfviewer_image.renderSize.y = 720.0f*multiplyRes*zoomfactor;
		}
		
		
		
		
		pdfviewer_image.newscrollPos.x = ((pdfviewer_image.renderSize.x-(1280.0f*multiplyRes))/2.0f);
		pdfviewer_image.newscrollPos.y = ((pdfviewer_image.renderSize.y-(720.0f*multiplyRes))/2.0f);
		
		float offx = pdfviewer_image.newscrollPos.x-pdfviewer_image.offsetScrolPos.x*zoomfactor;
		float offy = pdfviewer_image.newscrollPos.y-pdfviewer_image.offsetScrolPos.y*zoomfactor;
		
		//pdfviewer_image.newscrollPos.x = pdfviewer_image.newscrollPos.x+offx;
		//pdfviewer_image.newscrollPos.y = pdfviewer_image.newscrollPos.y+offy;
		
		
		pdfviewer_image.nCenteringFactorX = pdfviewer_image.nCenteringFactorX >= 0.0f ? pdfviewer_image.nCenteringFactorX : 0.0f;
		pdfviewer_image.nCenteringFactorY = pdfviewer_image.nCenteringFactorY >= 0.0f ? pdfviewer_image.nCenteringFactorY : 0.0f;
		
		pdfviewer_image.zoomfactor = zoomfactor;
		return true;
	}
	
	
	
	void PDFViewer() {
		SetupPDFViewerWindow();
		bool myimgresize = PDFMakeSizes(ImVec2(pdf_currentImg.width,pdf_currentImg.height),pdf_imgzoom,ImVec2(ImGui::GetScrollX(),ImGui::GetScrollY()));
		if(myimgresize)ImGui::SetNextWindowScroll(pdfviewer_image.newscrollPos);
		if (ImGui::Begin("##imgviewer", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_HorizontalScrollbar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove)) {
			
			if(pdf_loadimage){
				if(filebrowser->LoadedFile!=nullptr){
					if(filebrowser->LoadedFile->memvalid){

						if(Windows::pdf_currentImg.id != -1)Renderer->unregister_texture(Windows::pdf_currentImg);
						
						unsigned char * tmpdata;
						unsigned int w,h;
						int pdfret = filebrowser->OpenPDF(&tmpdata,&w,&h);
						Windows::pdf_currentImg = imgloader->OpenRAWImageMemory(tmpdata,w,h);
						free(tmpdata);
						pdf_loadimage=false;
					}
				}
			}
			
			if(filebrowser->LoadedFile!=nullptr){
				if(filebrowser->LoadedFile->memvalid){
					ImVec2 mouse_delta = ImGui::GetIO().MouseDelta;
					ScrollWhenDraggingOnVoid(ImVec2(-mouse_delta.x, -mouse_delta.y));
					PDFMakeSizes(ImVec2(pdf_currentImg.width,pdf_currentImg.height),pdf_imgzoom,ImVec2(ImGui::GetScrollX(),ImGui::GetScrollY()));
				
					ImGui::SetCursorPos(ImVec2(pdfviewer_image.nCenteringFactorX,pdfviewer_image.nCenteringFactorY));
					ImGui::Image((void*)(intptr_t)pdf_currentImg.id, pdfviewer_image.renderSize);
				}
			}
			
			if(filebrowser->LoadedFile!=nullptr){
				if(pdf_loadimage){
					float progress = (filebrowser->LoadedFile->currentOffset*100.0/filebrowser->LoadedFile->size)/100.0f;
					ImGui::SetCursorPosY(720.0f*multiplyRes/2.0f);
					ImGui::PushStyleColor(ImGuiCol_PlotHistogram,ImVec4(1.0f,1.0f,1.0f,1.0f));
					ImGui::ProgressBar(progress, ImVec2(-1.0f, 0.0f));
					ImGui::PopStyleColor();
				}
			}
			
			ImGuiContext& g = *GImGui;
			if(g.Time - pdf_imgControlsHide < 5){
				if(g.Time - pdf_imgControlsHide > 3 ){
					pdf_textfadealpha = pdf_textfadealpha-0.016;
				}
				ImVec4 buttextcol = ImVec4(1.0f,1.0f,1.0f,pdf_textfadealpha);
				
				ImDrawList* draw_list = ImGui::GetForegroundDrawList();
				
				std::string myfilename = filebrowser->currentFileinUse.substr(filebrowser->currentFileinUse.find_last_of("\\/")+1);
				ImVec2 filetextsize = ImGui::CalcTextSize(myfilename.c_str());
				draw_list->AddRectFilled(ImVec2(5.0f*multiplyRes,10.0f*multiplyRes),ImVec2(5.0f*multiplyRes+filetextsize.x+10.0f*multiplyRes,10.0f*multiplyRes+filetextsize.y+10.0f*multiplyRes),ImColor(ImVec4(0.0f,0.0f,0.0f, pdf_textfadealpha/2.0f)));
				ImVec2 mytitlepos = ImVec2(10.0f*multiplyRes,15.0f*multiplyRes);
				draw_list->AddText(mytitlepos, ImColor(buttextcol),myfilename.c_str());
				
				std::string pagestext = std::to_string(filebrowser->getPDFCurrentPage()) + " / " +std::to_string(filebrowser->getPDFPageCount());
				ImVec2 pagestextsize = ImGui::CalcTextSize(pagestext.c_str());
				ImVec2 mypagestextpos = ImVec2(1280.0f*multiplyRes-pagestextsize.x-20*multiplyRes,10.0f*multiplyRes);
				draw_list->AddRectFilled(mypagestextpos,ImVec2(1280.0f*multiplyRes,pagestextsize.y+10.0f*multiplyRes),ImColor(ImVec4(0.0f,0.0f,0.0f, pdf_textfadealpha/2.0f)));
				
				draw_list->AddText(ImVec2(mypagestextpos.x+10.0f,mypagestextpos.y+5.0f), ImColor(buttextcol),pagestext.c_str());
				
				ImVec2 mytextboxpos = ImVec2(5.0f,720.0f*multiplyRes-35.0f*multiplyRes);
				draw_list->AddRectFilled(mytextboxpos,ImVec2(1280.0f*multiplyRes-5.0f*multiplyRes,720.0f*multiplyRes),ImColor(ImVec4(0.0f,0.0f,0.0f, pdf_textfadealpha/2.0f)));
				
				ImVec2 mytextpos = ImVec2(10.0f,720.0f*multiplyRes-30.0f*multiplyRes);
				
				draw_list->AddText(mytextpos, ImColor(buttextcol),FONT_RIGHTSTICK);
				ImVec2 textsize = ImGui::CalcTextSize(FONT_RIGHTSTICK);
				mytextpos.x = mytextpos.x + textsize.x +5.0f;
				draw_list->AddText(mytextpos, ImColor(buttextcol),"Zoom");
				textsize = ImGui::CalcTextSize("Zoom");
				mytextpos.x = mytextpos.x + textsize.x +50.0f*multiplyRes;
				
				
				draw_list->AddText(mytextpos, ImColor(buttextcol),FONT_LEFTSTICK);
				textsize = ImGui::CalcTextSize(FONT_LEFTSTICK);
				mytextpos.x = mytextpos.x + textsize.x +5.0f;
				draw_list->AddText(mytextpos, ImColor(buttextcol),"Pan");
				textsize = ImGui::CalcTextSize("Pan");
				mytextpos.x = mytextpos.x + textsize.x +50.0f*multiplyRes;
				
				
				draw_list->AddText(mytextpos, ImColor(buttextcol),FONT_B_BUTTON_FILLED);
				textsize = ImGui::CalcTextSize(FONT_B_BUTTON_FILLED);
				mytextpos.x = mytextpos.x + textsize.x +5.0f;
				draw_list->AddText(mytextpos, ImColor(buttextcol),nxlangs::get_common_str(nxlangs::NXCOMMON_BACK).c_str());
				textsize = ImGui::CalcTextSize(nxlangs::get_common_str(nxlangs::NXCOMMON_BACK).c_str());
				mytextpos.x = mytextpos.x + textsize.x +50.0f*multiplyRes;
				
				draw_list->AddText(mytextpos, ImColor(buttextcol),FONT_A_BUTTON_FILLED);
				textsize = ImGui::CalcTextSize(FONT_A_BUTTON_FILLED);
				mytextpos.x = mytextpos.x + textsize.x +5.0f;
				draw_list->AddText(mytextpos, ImColor(buttextcol),"Scale to Fit");
				textsize = ImGui::CalcTextSize("Scale to Fit");
				mytextpos.x = mytextpos.x + textsize.x +50.0f*multiplyRes;
				
				draw_list->AddText(mytextpos, ImColor(buttextcol),FONT_ZL_BUTTON_FILLED);
				textsize = ImGui::CalcTextSize(FONT_ZL_BUTTON_FILLED);
				mytextpos.x = mytextpos.x + textsize.x +5.0f;
				draw_list->AddText(mytextpos, ImColor(buttextcol),"Prev. Page");
				textsize = ImGui::CalcTextSize("Prev. Image");
				mytextpos.x = mytextpos.x + textsize.x +50.0f*multiplyRes;
				
				draw_list->AddText(mytextpos, ImColor(buttextcol),FONT_ZR_BUTTON_FILLED);
				textsize = ImGui::CalcTextSize(FONT_ZR_BUTTON_FILLED);
				mytextpos.x = mytextpos.x + textsize.x +5.0f;
				draw_list->AddText(mytextpos, ImColor(buttextcol),"Next Page");
				textsize = ImGui::CalcTextSize("Next Image");
				mytextpos.x = mytextpos.x + textsize.x +50.0f*multiplyRes;
				
			}
			
		}
		ExitPDFViewerWindow();
	}
	
	void setPDFImageZoom(float zoomfactor){
		ImGuiContext& g = *GImGui;
		pdf_imgControlsHide = g.Time;
		pdf_textfadealpha = 1.0f;
		pdf_imgzoom = zoomfactor;
		pdf_zooming = true;
	}
	
	float getPDFImageZoom(){
		return pdf_imgzoom;
	}
	
}