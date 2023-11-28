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
	float zoomposx = 0.0f;
	float zoomposy = 0.0f;
	
	double imgControlsHide = 0.0f;
	
	
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
	
	
	void imageViewer() {
		SetupImgViewerWindow();
		if (ImGui::Begin("##imgviewer", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_HorizontalScrollbar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove)) {
			
			ImVec2 mouse_delta = ImGui::GetIO().MouseDelta;
			ScrollWhenDraggingOnVoid(ImVec2(-mouse_delta.x, -mouse_delta.y));
			
			
			float imgx = currentImg.width;
			float imgy = currentImg.height;
						
			double dAspectRatio = 1280.0f/720.0f;
			double dPictureAspectRatio = imgx/imgy;
			
			if (dPictureAspectRatio > dAspectRatio){
				int nNewHeight = (int)((1280.0f*multiplyRes)/imgx*imgy);
				int nCenteringFactor = ((720.0f*multiplyRes) - nNewHeight) / 2;
				ImGui::SetCursorPos(ImVec2(0,nCenteringFactor));
				ImGui::Image((void*)(intptr_t)currentImg.id, ImVec2((1280.0f*multiplyRes)*imgzoom,(nNewHeight)*imgzoom));
				
			} else if (dPictureAspectRatio < dAspectRatio){
				int nNewWidth =  (int)((720.0f*multiplyRes)/imgy*imgx);
				int nCenteringFactor = ((1280.0f*multiplyRes) - nNewWidth) / 2;
				ImGui::SetCursorPos(ImVec2(nCenteringFactor,0));
				ImGui::Image((void*)(intptr_t)currentImg.id, ImVec2((nNewWidth)*imgzoom,(720.0f*multiplyRes)*imgzoom));
			} else{
				ImGui::Image((void*)(intptr_t)currentImg.id, ImVec2((1280.0f*multiplyRes*imgzoom),(720.0f*multiplyRes*imgzoom)));
			}
			
			ImGuiContext& g = *GImGui;
			if(imgControlsHide+5 > g.Time){
				
				ImVec4 buttextcol = ImVec4(1.0f,1.0f,1.0f,1.0f);
				
				ImDrawList* draw_list = ImGui::GetForegroundDrawList();
				
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
				
			}
			
		}
		ExitImgViewerWindow();
	}
	
	void setImageZoom(float zoomfactor){
		ImGuiContext& g = *GImGui;
		imgControlsHide = g.Time;
		imgzoom = zoomfactor;
	}
	
	float getImageZoom(){
		return imgzoom;
	}
	
}