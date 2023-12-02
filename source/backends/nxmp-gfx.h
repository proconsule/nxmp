#ifndef NXMP_GFX_H
#define NXMP_GFX_H


//#define OPENGL_BACKEND
//#define DEKO3D_BACKEND


#ifdef OPENGL_BACKEND
#include "glad/glad.h"
#include "imgui_impl_glfw.h"
#include <mpv/render_gl.h>
#endif
#include <switch.h>
#include <chrono>

#ifdef DEKO3D_BACKEND
#include "deko3d.hpp"
#include "imgui_deko3d.h"
#include "imgui_nx.h"
#include <mpv/render_dk3d.h>
#endif
#include "logger.h"




#define handheldWidth 1280
#define handheldHeight 720
#define dockedWidth 1920
#define dockedHeight 1080



#define FONT_A_BUTTON "\xEE\x80\x80"
#define FONT_B_BUTTON "\xEE\x80\x81"
#define FONT_X_BUTTON "\xEE\x80\x82"
#define FONT_Y_BUTTON "\xEE\x80\x83"

#define FONT_A_BUTTON_FILLED "\xEE\x83\xA0"
#define FONT_B_BUTTON_FILLED "\xEE\x83\xA1"
#define FONT_X_BUTTON_FILLED "\xEE\x83\xA2"
#define FONT_Y_BUTTON_FILLED "\xEE\x83\xA3"

#define FONT_ZL_BUTTON_FILLED "\xEE\x83\xA6"
#define FONT_ZR_BUTTON_FILLED "\xEE\x83\xA7"


#define FONT_CHECKED_ICON "\xEE\x85\x8B"

#define FONT_DPADUP_BUTTON "\xEE\x82\xAF"
#define FONT_DPADDOWN_BUTTON "\xEE\x82\xB0"

#define FONT_DPADUP_BUTTON_FILLED "\xEE\x83\xAB"
#define FONT_DPADDOWN_BUTTON_FILLED "\xEE\x83\xAC"

#define FONT_DPADLEFT_BUTTON_FILLED "\xEE\x83\xAD"
#define FONT_DPADRIGHT_BUTTON_FILLED "\xEE\x83\xAE"

#define FONT_LOADING_ICON0 "\xEE\x80\xA0"
#define FONT_LOADING_ICON1 "\xEE\x80\xA1"
#define FONT_LOADING_ICON2 "\xEE\x80\xA2"
#define FONT_LOADING_ICON3 "\xEE\x80\xA3"
#define FONT_LOADING_ICON4 "\xEE\x80\xA4"
#define FONT_LOADING_ICON5 "\xEE\x80\xA5"
#define FONT_LOADING_ICON6 "\xEE\x80\xA6"
#define FONT_LOADING_ICON7 "\xEE\x80\xA7"

#define FONT_SPEAKER_ICON "\xEE\x84\xBC"

#define FONT_LEFTSTICK "\xEE\x83\x81"
#define FONT_RIGHTSTICK "\xEE\x83\x82"

#ifdef DEKO3D_BACKEND
struct Texture {
		dk::Image image;
		dk::UniqueMemBlock memblock;
		DkResHandle id = -1;
		unsigned int width;
		unsigned int height;
	};

#endif

#ifdef OPENGL_BACKEND

typedef struct {
    GLuint id = 0;
    int width = 0;
    int height = 0;
} Tex;

#endif

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
	
	struct fonttype_struct{
		std::string filename = "";
		float size;
		ImVector<ImWchar> charrange;
	};
	
	
	/* a but ugly to put here audio stuff, just to quick test */
	
	constexpr static float MasterVolumeMin     = 0.0f;
	constexpr static float MasterVolumeMax     = 5.0f;
	constexpr static float MasterVolumeDefault = 1.0f;
	constexpr static float MasterVolumeExp     = 2.0f;
	extern float CurrentVolume;
	
	
	
	extern ImVec4 Text_color;
	extern ImVec4 Active_color;
	extern ImVec4 Disabled_color;
	
	extern ImVec4 NavHover_color;
	extern ImVec4 HeaderHover_color;
	extern ImVec4 Window_Bg_color;
	extern ImVec4 OptsTab_Bg_color;
	extern ImVec4 Popup_Bg_color;
	extern ImVec4 Button_color;
	extern ImVec4 ButtonActive_color;

	
#ifdef OPENGL_BACKEND
	extern GLuint WIDTH;
	extern GLuint HEIGHT;
	extern GLFWwindow *window;
	
	
#endif


	extern bool docked;
	extern float windowed_width;
	extern float windowed_height;
	
	void loopAppletMode();
	void Init_Backend(bool docked,bool vsync);
	void Init_Backend_AppletMode(bool isdocked);
	void Init_Backend_Splash(bool isdocked);
	void Init_ImGui(bool isdocked);
	void NewFrame();
	
	void Render_PreMPV();
	void Render_PostMPV();
	
	//void Render();
	void Resize(float w,float h);
	void Destroy();
	void Destory_ImGui();
	void Destroy_Backend();
	uint64_t Process_UI_Events(std::chrono::time_point<std::chrono::system_clock> myeventtime);
	bool WindowShouldClose();
	void Quit();
	
	void updateSplash(int perc);
	
	
	void UniFontLoader(bool latinonly = false);
	
	
	
	void Create_VO_FrameBuffer(float w,float h);
	void Rescale_VO_Framebuffer(float width, float height);
	void SetFullScreen(bool fullscreen);
	void Draw_VO();
	void Resize_VO(float w,float h);
	mpv_render_param * getMPV_Params();
	unsigned int getWidth();
	unsigned int getHeight();
	
	void setEnableTouch(bool value);
	
	void SetColorTheme(int themecolor);
	void SetDarkTheme();
	void SetLightTheme();
	
#ifdef OPENGL_BACKEND
	GLuint getFBO_Texture();
#endif
	
#ifdef DEKO3D_BACKEND
	dk::Device getDeko3dDevice();
	dk::Image* getFramebuffer();
	void UpdateFBO();
	void queueWaitFence(DkFence* fence);
	void queueWaitDoneFence();
	void queueSignalFence(DkFence* fence, bool flash = false);
	void queueFlush();
	unsigned int getMaxSamplers();
	
	
	Texture load_texture(std::string path,DkImageFormat format, std::uint32_t flags,int desc_slot);
	Texture load_texture_from_mem(unsigned char *image_data,int width, int height,int channels,DkImageFormat format, std::uint32_t flags,int desc_slot);
	
	
	
#endif

	void setBGAlpha(float value);

	void getSystemVolume(float &value);
	void setSystemVolume(float value);


}


#endif