#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "appwindows.h"

#include "imgloader.h"

#include "nxmp-i18n.h"

namespace Windows {
	
#ifdef DEKO3D_BACKEND
	Texture currentImg;
#endif
#ifdef OPENGL_BACKEND
	Tex currentImg;
#endif	
	float imgzoom = 1.0f;
	float oldimgzoom = 1.0f;
	float zoomposx = 0.0f;
	float zoomposy = 0.0f;
	
	double imgControlsHide = 0.0f;
	
	bool zooming = false;
	
	ImVec2 imgscrollpos = ImVec2(0.0f,0.0f);
	
	double dAspectRatio = 1280.0f/720.0f;
	
	float textfadealpha = 1.0f;
	
	
	void SetupImgViewerWindow(void) {
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(1280.0f*multiplyRes, 720.0f*multiplyRes), ImGuiCond_Always);
        
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_WindowBg,nxmpgfx::Window_Bg_color);
    };
	
	void ExitImgViewerWindow(void) {
        ImGui::End();
        ImGui::PopStyleVar(3);
		ImGui::PopStyleColor();
    };
	

	void ScrollWhenDraggingOnVoid(const ImVec2& delta) {
		ImGuiContext& g = *ImGui::GetCurrentContext();
		ImGuiWindow* window = g.CurrentWindow;
		bool hovered = false;
		bool held = false;
		ImGuiID id = window->GetID("##imgviewer");
		ImGui::KeepAliveID(id);
		ImGui::ButtonBehavior(window->Rect(), id, &hovered, &held, ImGuiButtonFlags_MouseButtonLeft);
		if(ImGui::IsMouseDragging(0)){
			ImGuiContext& g = *GImGui;
			imgControlsHide = g.Time;
			if (held && delta.x != 0.0f)
				ImGui::SetScrollX(window, ImGui::GetScrollX() + delta.x);
			if (held && delta.y != 0.0f)
				ImGui::SetScrollY(window, ImGui::GetScrollY() + delta.y);
		}
	}
	
	
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
		
	}img_resize_struct;
	
	
	img_resize_struct imgviewer_image;
	
	bool MakeSizes(ImVec2 imgSrcSize,float zoomfactor,ImVec2 scrollPos){
		if(imgSrcSize.x == imgviewer_image.originalSize.x && imgSrcSize.y == imgviewer_image.originalSize.y && zoomfactor == imgviewer_image.zoomfactor){
			imgviewer_image.scrollPos = scrollPos;
			imgviewer_image.newscrollPos = scrollPos;
			imgviewer_image.offsetScrolPos = scrollPos;
			return false;
		}
		imgviewer_image.originalSize = imgSrcSize;
		imgviewer_image.dPictureAspectRatio = imgSrcSize.x/imgSrcSize.y;
		imgviewer_image.scrollPos = scrollPos;
		
		
		int nNewHeight;
		int nNewWidth;
		
		if (imgviewer_image.dPictureAspectRatio > dAspectRatio){
				nNewHeight = (int)((1280.0f*multiplyRes)/imgSrcSize.x*imgSrcSize.y);
				imgviewer_image.nCenteringFactorY = ((720.0f*multiplyRes) - nNewHeight*zoomfactor) / 2;
				imgviewer_image.nCenteringFactorX = 0.0f;
				imgviewer_image.renderSize.x = (1280.0f*multiplyRes)*zoomfactor;
				imgviewer_image.renderSize.y = nNewHeight*zoomfactor;
				
			} else if (imgviewer_image.dPictureAspectRatio < dAspectRatio){
				nNewWidth =  (int)((720.0f*multiplyRes)/imgSrcSize.y*imgSrcSize.x);
				imgviewer_image.nCenteringFactorX = ((1280.0f*multiplyRes) - nNewWidth*zoomfactor) / 2;
				imgviewer_image.nCenteringFactorY = 0.0f;
				imgviewer_image.renderSize.x = nNewWidth*zoomfactor;
				imgviewer_image.renderSize.y = (720.0f*multiplyRes)*zoomfactor;
				
				
			} else{
				imgviewer_image.nCenteringFactorX = 0.0f;
				imgviewer_image.nCenteringFactorY = 0.0f;
				imgviewer_image.renderSize.x = 1280.0f*multiplyRes*zoomfactor;
				imgviewer_image.renderSize.y = 720.0f*multiplyRes*zoomfactor;
		}
		
		
		
		
		imgviewer_image.newscrollPos.x = ((imgviewer_image.renderSize.x-(1280.0f*multiplyRes))/2.0f);
		imgviewer_image.newscrollPos.y = ((imgviewer_image.renderSize.y-(720.0f*multiplyRes))/2.0f);
		
		float offx = imgviewer_image.newscrollPos.x-imgviewer_image.offsetScrolPos.x*zoomfactor;
		float offy = imgviewer_image.newscrollPos.y-imgviewer_image.offsetScrolPos.y*zoomfactor;
		
		//imgviewer_image.newscrollPos.x = imgviewer_image.newscrollPos.x+offx;
		//imgviewer_image.newscrollPos.y = imgviewer_image.newscrollPos.y+offy;
		
		
		imgviewer_image.nCenteringFactorX = imgviewer_image.nCenteringFactorX >= 0.0f ? imgviewer_image.nCenteringFactorX : 0.0f;
		imgviewer_image.nCenteringFactorY = imgviewer_image.nCenteringFactorY >= 0.0f ? imgviewer_image.nCenteringFactorY : 0.0f;
		
		imgviewer_image.zoomfactor = zoomfactor;
		return true;
	}
	
	
	
	void imageViewer() {
		SetupImgViewerWindow();
		bool myimgresize = MakeSizes(ImVec2(currentImg.width,currentImg.height),imgzoom,ImVec2(ImGui::GetScrollX(),ImGui::GetScrollY()));
		if(myimgresize)ImGui::SetNextWindowScroll(imgviewer_image.newscrollPos);
		if (ImGui::Begin("##imgviewer", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_HorizontalScrollbar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove)) {
			
			ImVec2 mouse_delta = ImGui::GetIO().MouseDelta;
			ScrollWhenDraggingOnVoid(ImVec2(-mouse_delta.x, -mouse_delta.y));
			MakeSizes(ImVec2(currentImg.width,currentImg.height),imgzoom,ImVec2(ImGui::GetScrollX(),ImGui::GetScrollY()));
		
			ImGui::SetCursorPos(ImVec2(imgviewer_image.nCenteringFactorX,imgviewer_image.nCenteringFactorY));
			ImGui::Image((void*)(intptr_t)currentImg.id, imgviewer_image.renderSize);
			
			
			ImGuiContext& g = *GImGui;
			if(g.Time - imgControlsHide < 5){
				if(g.Time - imgControlsHide > 3 ){
					textfadealpha = textfadealpha-0.016;
				}
				ImVec4 buttextcol = ImVec4(1.0f,1.0f,1.0f,textfadealpha);
				
				ImDrawList* draw_list = ImGui::GetForegroundDrawList();
				
				std::string myfilename = filebrowser->currentFileinUse.substr(filebrowser->currentFileinUse.find_last_of("\\/")+1);
				ImVec2 filetextsize = ImGui::CalcTextSize(myfilename.c_str());
				draw_list->AddRectFilled(ImVec2(5.0f*multiplyRes,10.0f*multiplyRes),ImVec2(5.0f*multiplyRes+filetextsize.x+10.0f*multiplyRes,10.0f*multiplyRes+filetextsize.y+10.0f*multiplyRes),ImColor(ImVec4(0.0f,0.0f,0.0f, textfadealpha/2.0f)));
				ImVec2 mytitlepos = ImVec2(10.0f*multiplyRes,15.0f*multiplyRes);
				draw_list->AddText(mytitlepos, ImColor(buttextcol),myfilename.c_str());
				
				
				ImVec2 mytextboxpos = ImVec2(5.0f,720.0f*multiplyRes-35.0f*multiplyRes);
				draw_list->AddRectFilled(mytextboxpos,ImVec2(1280.0f*multiplyRes-5.0f*multiplyRes,720.0f*multiplyRes),ImColor(ImVec4(0.0f,0.0f,0.0f, textfadealpha/2.0f)));
				
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
				draw_list->AddText(mytextpos, ImColor(buttextcol),Common_STR[NXCOMMON_BACK]);
				textsize = ImGui::CalcTextSize(Common_STR[NXCOMMON_BACK]);
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
				draw_list->AddText(mytextpos, ImColor(buttextcol),"Prev. Image");
				textsize = ImGui::CalcTextSize("Prev. Image");
				mytextpos.x = mytextpos.x + textsize.x +50.0f*multiplyRes;
				
				draw_list->AddText(mytextpos, ImColor(buttextcol),FONT_ZR_BUTTON_FILLED);
				textsize = ImGui::CalcTextSize(FONT_ZR_BUTTON_FILLED);
				mytextpos.x = mytextpos.x + textsize.x +5.0f;
				draw_list->AddText(mytextpos, ImColor(buttextcol),"Next Image");
				textsize = ImGui::CalcTextSize("Next Image");
				mytextpos.x = mytextpos.x + textsize.x +50.0f*multiplyRes;
				
			}
			
		}
		ExitImgViewerWindow();
	}
	
	void setImageZoom(float zoomfactor){
		ImGuiContext& g = *GImGui;
		imgControlsHide = g.Time;
		textfadealpha = 1.0f;
		imgzoom = zoomfactor;
		zooming = true;
	}
	
	float getImageZoom(){
		return imgzoom;
	}
	
}