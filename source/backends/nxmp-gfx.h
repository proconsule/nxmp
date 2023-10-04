#ifndef NXMP_GFX_H
#define NXMP_GFX_H


/*
#include <SDL.h>
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
*/
#include "glad/glad.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <chrono>

#include "logger.h"




#define handheldWidth 1280
#define handheldHeight 720
#define dockedWidth 1920
#define dockedHeight 1080




namespace nxmpgfx{
	
	enum BUT_FLAGS{
		BUT_A,
		BUT_B,
		BUT_X,
		BUT_Y,
		
		BUT_DUP,
		BUT_DDOWN,
		BUT_DLEFT,
		BUT_DRIGHT,
		
		BUT_L,
		BUT_R,
		BUT_ZL,
		BUT_ZR,
		
		BUT_TR,
		BUT_TL,
		
		BUT_PLUS,
		BUT_MINUS,
		
		B_AX_L_UP,
		B_AX_L_DOWN,
		B_AX_L_LEFT,
		B_AX_L_RIGHT,
		
	
		B_AX_R_UP,
		B_AX_R_DOWN,
		B_AX_R_LEFT,
		B_AX_R_RIGHT,
		
	};
	
	
	//extern SDL_Window *window;
	//extern SDL_GLContext context;
	extern GLFWwindow *window;
	
	
	extern GLuint WIDTH;
	extern GLuint HEIGHT;
	extern bool docked;
	
	void loopAppletMode();
	void Init_Backend(bool docked,bool vsync);
	void Init_Backend_AppletMode(bool isdocked);
	void Init_Backend_Splash(bool isdocked);
	void Init_ImGui(bool isdocked);
	void NewFrame();
	void Render();
	void Resize(float w,float h);
	void Destroy();
	void Destory_ImGui();
	void Destroy_Backend();
	uint64_t Process_UI_Events();
	void Quit();
	
	void updateSplash(int perc);
	
	
	
	
}


#endif