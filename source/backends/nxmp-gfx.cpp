#include "nxmp-gfx.h"
#include "GLFW/glfw3.h"





namespace nxmpgfx{
	
	GLFWwindow *window;
	GLuint WIDTH;
	GLuint HEIGHT;
	/*
	SDL_Window *window;
	SDL_GLContext context;
	
	GLuint WIDTH;
	GLuint HEIGHT;
	*/
	
	
	
	
	bool B_A_PRESS = false;
	bool B_B_PRESS = false;
	bool B_X_PRESS = false;
	bool B_Y_PRESS = false;
	bool B_DU_PRESS = false;
	bool B_DD_PRESS = false;
	bool B_DL_PRESS = false;
	bool B_DR_PRESS = false;

	bool B_TL_PRESS = false;
	bool B_TR_PRESS = false;


	bool B_R_PRESS = false;
	bool B_L_PRESS = false;
	bool B_ZR_PRESS = false;
	bool B_ZL_PRESS = false;

	bool B_PLUS_PRESS = false;
	bool B_MINUS_PRESS = false;
	
	bool B_AX_L_UP_PRESS = false;
	bool B_AX_L_DOWN_PRESS = false;
	bool B_AX_L_LEFT_PRESS = false;
	bool B_AX_L_RIGHT_PRESS = false;
			
	bool B_AX_R_UP_PRESS = false;
	bool B_AX_R_DOWN_PRESS = false;
	bool B_AX_R_LEFT_PRESS = false;
	bool B_AX_R_RIGHT_PRESS = false;
	
	
	static void errorCallback(int errorCode, const char* description)
	{
		fprintf(stderr, "Glfw Error %d: %s\n", errorCode, description);
	}
	
	inline uint64_t bit_set_to(uint64_t number, unsigned int n, bool x) {
		return (number & ~((unsigned int)1 << n)) | ((unsigned int)x << n);
	}
	
	inline uint64_t bit_set(uint64_t number, unsigned int n) {
    return number | ((unsigned int)1 << n);
}
	
	void Init(bool docked,bool vsync){
	     /*	
		bool success = false;
		
		SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "no");
		if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0 ){
			NXLOG::ERRORLOG("%s: SDL could not initialize! SDL Error: %s", __func__, SDL_GetError());
			success =  false;
		}
		else {
			
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
			SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
			
			
			WIDTH = handheldWidth; HEIGHT = handheldHeight;
			if(docked){
				WIDTH = dockedWidth; HEIGHT = dockedHeight;
			}
			
			window = SDL_CreateWindow(
					"[glad] GL with SDL",
					SDL_WINDOWPOS_CENTERED,
					SDL_WINDOWPOS_CENTERED,
					WIDTH, HEIGHT,
					SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
			);
			if( window == NULL ){
				NXLOG::ERRORLOG("%s: Window could not be created! SDL Error: %s", __func__, SDL_GetError());
				success =  false;
			}
			else {
				context = SDL_GL_CreateContext(window);
				
				if( context == NULL )
				{
					NXLOG::ERRORLOG( "%s: OpenGL context could not be created! SDL Error: %s", __func__, SDL_GetError());
					success =  false;
				}
				else {
					gladLoadGL();
				}
				if(vsync){
					SDL_GL_SetSwapInterval(1);
				}else{
					SDL_GL_SetSwapInterval(0);
				}
			}
			
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO &io = ImGui::GetIO();
			(void) io;
			io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			
			
			
			  
			io.IniFilename = nullptr;
			io.MouseDrawCursor = false;
			
			ImGui::StyleColorsDark();
			
			NXLOG::DEBUGLOG("Init IMGUI SDL BACKEND\n");
			ImGui_ImplSDL2_InitForOpenGL(window, context);
			NXLOG::DEBUGLOG("Init IMGUI OPENGL BACKEND\n");
			const char* glsl_version = "#version 430 core";
			
			ImGui_ImplOpenGL3_Init(glsl_version);
		}
		*/
		glfwSetErrorCallback(errorCallback);
		
		WIDTH = handheldWidth; HEIGHT = handheldHeight;
		if(docked){
			WIDTH = dockedWidth; HEIGHT = dockedHeight;
		}
		
		if (!glfwInit()){
			return;
		}
		const char* glsl_version = "#version 430 core";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		
		window = glfwCreateWindow(WIDTH, HEIGHT, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
		if (window == nullptr)
			return;
		glfwMakeContextCurrent(window);
		if(vsync){
			glfwSwapInterval(1);
		}else{
			glfwSwapInterval(0);
		}
		glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		(void) io;
		//io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		
		io.IniFilename = nullptr;
		io.MouseDrawCursor = false;
			
		ImGui::StyleColorsDark();
			
	
		
		NXLOG::DEBUGLOG("Init IMGUI GLFW BACKEND\n");
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		NXLOG::DEBUGLOG("Init IMGUI OPENGL BACKEND\n");
		ImGui_ImplOpenGL3_Init(glsl_version);
    
	}
	
	void Destroy(){
		/*
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
		SDL_GL_DeleteContext(context);
		
		SDL_DestroyWindow(window);
		SDL_Quit();
		*/
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(window);
		glfwTerminate();
	}
	
	void NewFrame(){
		ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
		//ImGui_ImplOpenGL3_NewFrame();
        //ImGui_ImplSDL2_NewFrame();
	}
	
	void Render(){
		glClearColor(0.00f, 0.00f, 0.00f, 1.00f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		
		//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		//SDL_GL_SwapWindow(window);
	}
	
	void Resize(float w,float h){
		WIDTH = static_cast<int>(w);
		HEIGHT = static_cast<int>(h);
		glfwSetWindowSize(window,w,h);
		//SDL_SetWindowSize(window, w, h);
	}
	void Quit(){
		
	}
	
	uint64_t Process_UI_Events(){
		
		uint64_t ret_event = 0;
		
		glfwPollEvents();
		GLFWgamepadstate state;
		
		
		if ((GLFW_TRUE == glfwGetGamepadState(GLFW_JOYSTICK_1, &state)))
        {
			if(state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS && !B_A_PRESS){
				ret_event = bit_set(ret_event,BUT_A);
			}
			if(state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS && !B_B_PRESS){
				ret_event = bit_set(ret_event,BUT_B);
			}
			if(state.buttons[GLFW_GAMEPAD_BUTTON_X] == GLFW_PRESS && !B_X_PRESS){
				ret_event = bit_set(ret_event,BUT_X);
			}
			if(state.buttons[GLFW_GAMEPAD_BUTTON_Y] == GLFW_PRESS && !B_Y_PRESS){
				ret_event = bit_set(ret_event,BUT_Y);
			}
			
			
			if(state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS && !B_DU_PRESS){
				ret_event = bit_set(ret_event,BUT_DUP);
			}
			if(state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS && !B_DD_PRESS){
				ret_event = bit_set(ret_event,BUT_DDOWN);
			}
			if(state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] == GLFW_PRESS && !B_DL_PRESS){
				ret_event = bit_set(ret_event,BUT_DLEFT);
			}
			if(state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS && !B_DR_PRESS){
				ret_event = bit_set(ret_event,BUT_DRIGHT);
			}
			
			if(state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS && !B_DR_PRESS){
				ret_event = bit_set(ret_event,BUT_DRIGHT);
			}
			
			
			if(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] > 0.25f && !B_ZR_PRESS){
				ret_event = bit_set(ret_event,BUT_ZR);
			}
			
			if(state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] > 0.25f && !B_ZL_PRESS){
				ret_event = bit_set(ret_event,BUT_ZL);
			}
			
			if(state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] == GLFW_PRESS && !B_R_PRESS){
				ret_event = bit_set(ret_event,BUT_R);
			}
			
			if(state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_PRESS && !B_L_PRESS){
				ret_event = bit_set(ret_event,BUT_L);
			}
			
			if(state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB] == GLFW_PRESS && !B_TR_PRESS){
				ret_event = bit_set(ret_event,BUT_TR);
			}
			
			if(state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB] == GLFW_PRESS && !B_TL_PRESS){
				ret_event = bit_set(ret_event,BUT_TL);
			}
			
			if(state.buttons[GLFW_GAMEPAD_BUTTON_START ] == GLFW_PRESS && !B_PLUS_PRESS){
				ret_event = bit_set(ret_event,BUT_PLUS);
			}
			
			if(state.buttons[GLFW_GAMEPAD_BUTTON_BACK ] == GLFW_PRESS && !B_MINUS_PRESS){
				ret_event = bit_set(ret_event,BUT_MINUS);
			}
			
			
			if(state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]<-0.25f && !B_AX_L_UP_PRESS){
				ret_event = bit_set(ret_event,B_AX_L_UP);
			}
			if(state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]>0.25f && !B_AX_L_DOWN_PRESS){
				ret_event = bit_set(ret_event,B_AX_L_DOWN);
			}
			if(state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]>0.25f && !B_AX_L_RIGHT_PRESS){
				ret_event = bit_set(ret_event,B_AX_L_RIGHT);
			}
			if(state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]<-0.25f && !B_AX_L_LEFT_PRESS){
				ret_event = bit_set(ret_event,B_AX_L_LEFT);
			}
			
			if(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]>0.25f && !B_AX_R_UP_PRESS){
				ret_event = bit_set(ret_event,B_AX_R_UP);
			}
			if(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]<-0.25f && !B_AX_R_DOWN_PRESS){
				ret_event = bit_set(ret_event,B_AX_R_DOWN);
			}
			if(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]>0.25f && !B_AX_R_RIGHT_PRESS){
				ret_event = bit_set(ret_event,B_AX_R_RIGHT);
			}
			if(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]<-0.25f && !B_AX_R_LEFT_PRESS){
				ret_event = bit_set(ret_event,B_AX_R_LEFT);
			}
			
			
			B_ZL_PRESS = state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER]>-0.75f;
			B_ZR_PRESS = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER]>-0.75f;
			
			B_A_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS;
			B_X_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_X] == GLFW_PRESS;
			B_B_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS;
			B_Y_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_Y] == GLFW_PRESS;
			B_DD_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS;
			B_DU_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS;
			
			B_DL_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] == GLFW_PRESS;
			B_DR_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS;

			B_R_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] == GLFW_PRESS;
			B_L_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_PRESS;
			B_TR_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB] == GLFW_PRESS;
			B_TL_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB] == GLFW_PRESS;

			B_PLUS_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS;
			B_MINUS_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_BACK] == GLFW_PRESS;
			
			
			
			B_AX_L_UP_PRESS = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]<-0.25f;
			B_AX_L_DOWN_PRESS = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]>0.25f;
			B_AX_L_LEFT_PRESS = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]>0.25f;
			B_AX_L_RIGHT_PRESS = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]<-0.25f;
			
			B_AX_R_UP_PRESS = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]>0.25f;
			B_AX_R_DOWN_PRESS = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]<-0.25f;
			B_AX_R_LEFT_PRESS = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]>0.25f;
			B_AX_R_RIGHT_PRESS = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]<-0.25f;
			
			
		}
		
		
	
	return ret_event;
	}
	
	
}