#include "nxmp-gfx.h"

#ifdef OPENGL_BACKEND
#include "imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"
#include "utils.h"
#include <switch.h>
#endif
#ifdef DEKO3D_BACKEND
#include <array>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <numeric>
#include <optional>
#include <vector>
#include <sys/stat.h>
#include <unistd.h>
#include <switch.h>
#endif
#include "stb_image.h"



#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

using namespace glm;

namespace nxmpgfx{
	
	ImVec4 Text_color = ImVec4(255, 255, 255, 1.00f);
	ImVec4 Active_color = ImVec4(0, 255, 203, 1.00f);
	ImVec4 Disabled_color = ImVec4(0.41f, 0.40f, 0.40f, 1.00f);
	ImVec4 Window_Bg_color = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
	ImVec4 OptsTab_Bg_color = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
	ImVec4 Popup_Bg_color = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	ImVec4 NavHover_color = ImVec4(0.0f, 0.0f, 0.0f, 0.20f);
	ImVec4 HeaderHover_color = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	ImVec4 Button_color = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);
	ImVec4 ButtonActive_color = ImVec4(0, 255, 203, 0.30f);
	
	
#ifdef OPENGL_BACKEND	
	const char* glsl_version = "#version 430 core";
	GLFWwindow *window;

	GLuint WIDTH = 1280;
	GLuint HEIGHT = 720;
	
	GLuint mpv_fbo;
	GLuint mpv_fbotexture;	
	mpv_opengl_fbo fbo;
	mpv_render_param fbo_params[3];
	
	GLuint prog;
	GLuint VBO;
	GLuint VAO;
	GLuint EBO;
	
	
	GLuint VBO_Progress;
	GLuint VAO_Progress;
	
#endif	
	
#ifdef DEKO3D_BACKEND

	//unsigned int WIDTH = 1920;
	//unsigned int HEIGHT = 1080;
	
	constexpr auto MAX_SAMPLERS = 28;
	constexpr auto MAX_IMAGES   = 28;

	constexpr auto FB_NUM       = 2u;

	constexpr auto CMDBUF_SIZE  = 1024 * 1024;

	unsigned s_width  = 1920;
	unsigned s_height = 1080;

	dk::UniqueDevice       s_device;

	dk::UniqueMemBlock     s_depthMemBlock;
	dk::Image              s_depthBuffer;

	dk::UniqueMemBlock     s_fbMemBlock;
	dk::Image              s_frameBuffers[FB_NUM];

	dk::UniqueMemBlock     s_cmdMemBlock[FB_NUM];
	dk::UniqueCmdBuf       s_cmdBuf[FB_NUM];

	dk::UniqueMemBlock     s_descriptorMemBlock;
	dk::SamplerDescriptor *s_samplerDescriptors = nullptr;
	dk::ImageDescriptor   *s_imageDescriptors   = nullptr;

	dk::UniqueQueue        s_queue;
	dk::UniqueSwapchain    s_swapchain;
	
	DkFence doneFence;
    DkFence readyFence;
	
	/*
    mpv_deko3d_fbo mpv_fbo{nullptr, &readyFence, &doneFence,
                           s_width,    s_height,         DkImageFormat_RGBA8_Unorm};
    mpv_render_param fbo_params[3] = {{MPV_RENDER_PARAM_DEKO3D_FBO, &mpv_fbo},
                                      { MPV_RENDER_PARAM_INVALID,
                                        nullptr }};
	*/		
	
	mpv_deko3d_fbo mpv_fbo;
	mpv_render_param fbo_params[3] = {{MPV_RENDER_PARAM_DEKO3D_FBO, &mpv_fbo},
                                      { MPV_RENDER_PARAM_INVALID,
                                        nullptr }};
	
	int imageSlot = 0;
	
	/* SHADERS moved out from imgui backend*/
	dk::Shader s_shaders[2];
	dk::UniqueMemBlock s_codeMemBlock;
	
#endif
	
	
	bool docked = false;
	
	bool splashloaded = false;
	
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
	
/*	
	bool B_AX_R_UP_PRESS = false;
	bool B_AX_R_DOWN_PRESS = false;
	bool B_AX_R_LEFT_PRESS = false;
	bool B_AX_R_RIGHT_PRESS = false;
*/
	
	typedef struct{
		bool pressed = false;
		std::chrono::time_point<std::chrono::system_clock> eventchrono;
	}axixstate_struct;
	
	axixstate_struct AX_R_UP;
	axixstate_struct AX_R_DOWN;
	axixstate_struct AX_R_LEFT;
	axixstate_struct AX_R_RIGHT;
	
	
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
	
	
#ifdef DEKO3D_BACKEND

void loadShaders (dk::UniqueDevice &device_)
{
	/// \brief Shader file descriptor
	struct ShaderFile
	{
		/// \brief Parameterized constructor
		/// \param shader_ Shader object
		/// \param path_ Path to source code
		ShaderFile (dk::Shader &shader_, char const *const path_)
		    : shader (shader_), path (path_), size (getSize (path_))
		{
		}

		/// \brief Get size of a file
		/// \param path_ Path to file
		static std::size_t getSize (char const *const path_)
		{
			struct stat st;
			auto const rc = ::stat (path_, &st);
			if (rc != 0)
			{
				std::fprintf (stderr, "stat(%s): %s\n", path_, std::strerror (errno));
				std::abort ();
			}

			return st.st_size;
		}

		/// \brief Shader object
		dk::Shader &shader;
		/// \brief Path to source code
		char const *const path;
		/// \brief Source code file size
		std::size_t const size;
	};

	auto shaderFiles = {ShaderFile{s_shaders[0], "romfs:/shaders/imgui_vsh.dksh"},
	    ShaderFile{s_shaders[1], "romfs:/shaders/imgui_fsh.dksh"}};

	// calculate total size of shaders
	auto const codeSize = std::accumulate (std::begin (shaderFiles),
	    std::end (shaderFiles),
	    DK_SHADER_CODE_UNUSABLE_SIZE,
	    [] (auto const sum_, auto const &file_) {
		    return sum_ + imgui::deko3d::align (file_.size, DK_SHADER_CODE_ALIGNMENT);
	    });

	// create shader code memblock
	s_codeMemBlock =
	    dk::MemBlockMaker{device_, imgui::deko3d::align (codeSize, DK_MEMBLOCK_ALIGNMENT)}
	        .setFlags (
	            DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached | DkMemBlockFlags_Code)
	        .create ();

	auto const addr    = static_cast<std::uint8_t *> (s_codeMemBlock.getCpuAddr ());
	std::size_t offset = 0;

	// read shaders into memblock
	for (auto &file : shaderFiles)
	{
		std::uint32_t const codeOffset = offset;

		auto *fp = fopen(file.path, "rb");
		if (!fp)
		{
			std::fprintf (stderr, "open(%s): %s\n", file.path, std::strerror (errno));
			std::abort ();
		}

		if (fread (&addr[offset], 1, file.size, fp) != file.size)
		{
			std::fprintf (stderr, "read(%s): %s\n", file.path, std::strerror (errno));
			std::abort ();
		}

		fclose(fp);

		dk::ShaderMaker{s_codeMemBlock, codeOffset}.initialize (file.shader);

		offset = imgui::deko3d::align (offset + file.size, DK_SHADER_CODE_ALIGNMENT);
	}
}





void rebuildSwapchain(unsigned const width_, unsigned const height_) {
    // destroy old swapchain
	
	s_width = width_;
	s_height = height_;
	
    s_swapchain = nullptr;

    // create new depth buffer image layout
    dk::ImageLayout depthLayout;
    dk::ImageLayoutMaker{s_device}
        .setFlags(DkImageFlags_UsageRender | DkImageFlags_HwCompression)
        .setFormat(DkImageFormat_Z24S8)
        .setDimensions(width_, height_)
        .initialize(depthLayout);

    auto const depthAlign = depthLayout.getAlignment();
    auto const depthSize  = depthLayout.getSize();

    // create depth buffer memblock
    if (!s_depthMemBlock) {
        s_depthMemBlock = dk::MemBlockMaker{s_device,
            imgui::deko3d::align(
                depthSize, std::max<unsigned> (depthAlign, DK_MEMBLOCK_ALIGNMENT))}
                              .setFlags(DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image)
                              .create();
    }

    s_depthBuffer.initialize(depthLayout, s_depthMemBlock, 0);

    // create framebuffer image layout
    dk::ImageLayout fbLayout;
    dk::ImageLayoutMaker{s_device}
        .setFlags(DkImageFlags_UsageRender | DkImageFlags_UsagePresent | DkImageFlags_HwCompression)
        .setFormat(DkImageFormat_RGBA8_Unorm)
        .setDimensions(width_, height_)
        .initialize(fbLayout);

    auto const fbAlign = fbLayout.getAlignment();
    auto const fbSize  = fbLayout.getSize();

    // create framebuffer memblock
    if (!s_fbMemBlock) {
        s_fbMemBlock = dk::MemBlockMaker{s_device,
            imgui::deko3d::align(
                FB_NUM * fbSize, std::max<unsigned> (fbAlign, DK_MEMBLOCK_ALIGNMENT))}
                           .setFlags(DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image)
                           .create();
    }

    // initialize swapchain images
    std::array<DkImage const *, FB_NUM> swapchainImages;
    for (unsigned i = 0; i < FB_NUM; ++i) {
        swapchainImages[i] = &s_frameBuffers[i];
        s_frameBuffers[i].initialize(fbLayout, s_fbMemBlock, i * fbSize);
    }

    // create swapchain
    s_swapchain = dk::SwapchainMaker{s_device, nwindowGetDefault(), swapchainImages}.create();
}


void deko3dInit(bool docked_) {
    // create deko3d device
	s_device = dk::DeviceMaker{}.create();

    // initialize swapchain with maximum resolution
	if(docked_){
		rebuildSwapchain(1920, 1080);
	}else{
		rebuildSwapchain(1280, 720);
	}
    // create memblocks for each image slot
    for (std::size_t i = 0; i < FB_NUM; ++i) {
        // create command buffer memblock
        s_cmdMemBlock[i] =
            dk::MemBlockMaker{s_device, imgui::deko3d::align(CMDBUF_SIZE, DK_MEMBLOCK_ALIGNMENT)}
                .setFlags(DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached)
                .create();

        // create command buffer
        s_cmdBuf[i] = dk::CmdBufMaker{s_device}.create();
        s_cmdBuf[i].addMemory(s_cmdMemBlock[i], 0, s_cmdMemBlock[i].getSize());
    }

    // create image/sampler memblock
    static_assert(sizeof(dk::ImageDescriptor)   == DK_IMAGE_DESCRIPTOR_ALIGNMENT);
    static_assert(sizeof(dk::SamplerDescriptor) == DK_SAMPLER_DESCRIPTOR_ALIGNMENT);
    static_assert(DK_IMAGE_DESCRIPTOR_ALIGNMENT == DK_SAMPLER_DESCRIPTOR_ALIGNMENT);
    s_descriptorMemBlock = dk::MemBlockMaker{s_device,
        imgui::deko3d::align(
            (MAX_SAMPLERS + MAX_IMAGES) * sizeof(dk::ImageDescriptor), DK_MEMBLOCK_ALIGNMENT)}
                               .setFlags(DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached)
                               .create();

    // get cpu address for descriptors
    s_samplerDescriptors =
        static_cast<dk::SamplerDescriptor *> (s_descriptorMemBlock.getCpuAddr());
    s_imageDescriptors =
        reinterpret_cast<dk::ImageDescriptor *> (&s_samplerDescriptors[MAX_SAMPLERS]);

    // create queue
    s_queue = dk::QueueMaker{s_device}.setFlags(DkQueueFlags_Graphics).create();

    auto &cmdBuf = s_cmdBuf[0];

    // bind image/sampler descriptors
    cmdBuf.bindSamplerDescriptorSet(s_descriptorMemBlock.getGpuAddr(), MAX_SAMPLERS);
    cmdBuf.bindImageDescriptorSet(
        s_descriptorMemBlock.getGpuAddr() + MAX_SAMPLERS * sizeof(dk::SamplerDescriptor),
        MAX_IMAGES);
    s_queue.submitCommands(cmdBuf.finishList());
    s_queue.waitIdle();

    cmdBuf.clear();
	loadShaders(s_device);
	
	
}

void deko3dExit() {
    // clean up all of the deko3d objects
    s_descriptorMemBlock = nullptr;

    for (unsigned i = 0; i < FB_NUM; ++i) {
        s_cmdBuf[i]      = nullptr;
        s_cmdMemBlock[i] = nullptr;
    }

    s_queue         = nullptr;
    s_swapchain     = nullptr;
    s_fbMemBlock    = nullptr;
    s_depthMemBlock = nullptr;
    s_device        = nullptr;
}


#endif

#ifdef OPENGL_BACKEND

	bool GLTxtLoadFromFile(std::string filename, GLuint* out_texture, int* out_width, int* out_height,bool flip = false){
		int image_width = 0;
		int image_height = 0;
		stbi_set_flip_vertically_on_load( flip? 1:0 );
		unsigned char* image_data = stbi_load(filename.c_str(), &image_width, &image_height, NULL, 4);
		if (image_data == NULL)
			return false;
		
		GLuint id = 0;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    
		*out_texture = id;
		*out_width = image_width;
		*out_height = image_height;
		stbi_set_flip_vertically_on_load(0);
		return true;
	}
	
	void AttachShader( GLuint program, GLenum type, const char* src )
	{
		GLuint shader = glCreateShader( type );
		glShaderSource( shader, 1, &src, NULL );
		glCompileShader( shader );
		//CheckStatus( shader, true );
		glAttachShader( program, shader );
		glDeleteShader( shader );
	}
	
	const char* vert = 1 + R"GLSL(
	#version 330 core
	layout(location = 0) in vec3 aPos;
	layout(location = 1) in vec3 aColor;
	layout(location = 2) in vec2 aTexCoord;

	out vec3 ourColor;
	out vec2 TexCoord;

	void main() {
		gl_Position = vec4(aPos, 1.0);
		ourColor = aColor;
		TexCoord = aTexCoord;
	}
	)GLSL";

	const char* frag = 1 + R"GLSL(
	#version 330 core
	out vec4 FragColor;

	in vec3 ourColor;
	in vec2 TexCoord;

	uniform sampler2D ourTexture;

	void main() {
		FragColor = texture(ourTexture, TexCoord);
	}
	)GLSL";

	float vertices[] =
		{
			-1.0f, -1.0f, 0.0f,  1.0f,1.0f,0.0f,  0.0f,0.0f,
			 1.0f, -1.0f, 0.0f,  0.0f,1.0f,0.0f,  1.0f,0.0f,
			 1.0f,  1.0f, 0.0f,  1.0f,0.0f,0.0f,  1.0f,1.0f,
			-1.0f,  1.0f, 0.0f,  1.0f,1.0f,0.0f,  0.0f,1.0f
		};

		unsigned int indices[] =
		{
			0,1,2,
			2,3,0
		};
#endif 
	unsigned int splashtexture;
	
	void loopAppletMode(){
#ifdef OPENGL_BACKEND
		while (appletMainLoop())
		{
			glfwPollEvents();
			GLFWgamepadstate state;
			
			
			if ((GLFW_TRUE == glfwGetGamepadState(GLFW_JOYSTICK_1, &state)))
			{
				if(state.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS){
					break;
				}
			}
			
		
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			// Render stuff
			glUseProgram( prog );
			unsigned int texture_unit = 0;
			glActiveTexture( GL_TEXTURE0 + texture_unit );
			glUniform1i( glGetUniformLocation( prog, "ourTexture" ), texture_unit );
			glBindTexture(GL_TEXTURE_2D, splashtexture);
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			
			
			glfwSwapBuffers(window);
		}
#endif
	}
	
	
	void Init_Backend(bool isdocked,bool vsync){
		
#ifdef OPENGL_BACKEND
		glfwSetErrorCallback(errorCallback);
		docked = isdocked;
		
		
		WIDTH = handheldWidth; HEIGHT = handheldHeight;
		if(docked){
			WIDTH = dockedWidth; HEIGHT = dockedHeight;
		}
		
		if (!glfwInit()){
			return;
		}
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

#endif

	}
	
	
	void Init_Backend_AppletMode(bool isdocked){
#ifdef OPENGL_BACKEND
		docked = isdocked;
		prog = glCreateProgram();
		AttachShader( prog, GL_VERTEX_SHADER, vert );
		AttachShader( prog, GL_FRAGMENT_SHADER, frag );
		glLinkProgram( prog );
		
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		
		int image_width,image_height;
		
		GLTxtLoadFromFile("romfs:/appletmode.png",&splashtexture,&image_width,&image_height,true);
		
		if(docked){
			glViewport(0,0,1920,1080);
		
		}else{
			glViewport(0,0,1280,720);
		
		}
#endif
		
	}
	
	
	struct vertex_data{
		vec4 coord;
		vec2 uv;
	};
	
	
	
	void Init_Backend_Splash(bool isdocked){
		docked = isdocked;
#ifdef OPENGL_BACKEND

		if(!splashloaded){
			prog = glCreateProgram();
			AttachShader( prog, GL_VERTEX_SHADER, vert );
			AttachShader( prog, GL_FRAGMENT_SHADER, frag );
			glLinkProgram( prog );
		
		
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(2);
			
			
			

			int image_width,image_height;
			
			GLTxtLoadFromFile("romfs:/splash.png",&splashtexture,&image_width,&image_height,true);
			splashloaded=true;
		}
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

		if(docked){
			glViewport(0,0,1920,1080);
		
		}else{
			glViewport(0,0,1280,720);
		
		}
        
		glUseProgram( prog );
		
		
		
        unsigned int texture_unit = 0;
        glActiveTexture( GL_TEXTURE0 + texture_unit );
        glUniform1i( glGetUniformLocation( prog, "ourTexture" ), texture_unit );
        glBindTexture(GL_TEXTURE_2D, splashtexture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
		
		/*
		glEnable(GL_SCISSOR_TEST);
		glScissor(0.5, 0.5, 100.0, 100.0);
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		*/
		
        glfwSwapBuffers(window);

#endif	

#ifdef DEKO3D_BACKEND
		deko3dInit(isdocked);
		
#endif
	
	}
	
	void updateSplash(int perc){

#ifdef OPENGL_BACKEND

		int barsize = 980;
		if(docked)barsize=1620;
		
		int wsize = perc*barsize/100;
		
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram( prog );
		
        unsigned int texture_unit = 0;
        glActiveTexture( GL_TEXTURE0 + texture_unit );
        glUniform1i( glGetUniformLocation( prog, "ourTexture" ), texture_unit );
        glBindTexture(GL_TEXTURE_2D, splashtexture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
		
		glEnable(GL_SCISSOR_TEST);
		glScissor(150.0,40.0, wsize, 30.0);
		glClearColor(0.859f, 0.118f, 0.29f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
		
        glfwSwapBuffers(window);
		
#endif
		
	}
	
	
	void Init_ImGui(bool isdocked){
#ifdef OPENGL_BACKEND
		docked = isdocked;
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		(void) io;
		io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		
		io.IniFilename = nullptr;

			
		ImGui::StyleColorsDark();
			
	

		NXLOG::DEBUGLOG("Init IMGUI GLFW BACKEND\n");
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		NXLOG::DEBUGLOG("Init IMGUI OPENGL BACKEND\n");
		ImGui_ImplOpenGL3_Init(glsl_version);
#endif

#ifdef DEKO3D_BACKEND
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		
		
		
		ImGui::StyleColorsDark();
		imgui::nx::init();
		ImGuiIO &io = ImGui::GetIO();
		//ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0.0f,0.0f,0.0f,0.0f));
		
		io.BackendPlatformName = "Switch";
		io.BackendRendererName = "imgui_impl_deko3d";
		io.IniFilename = nullptr;
		io.MouseDrawCursor = false;
		io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
		io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

	    
		imgui::deko3d::init(s_device,
        s_queue,
        s_cmdBuf[0],
        s_samplerDescriptors[0],
        s_imageDescriptors[0],
        dkMakeTextureHandle(0, 0),
		s_shaders,
        FB_NUM);
		
		
#endif
		//ImGuiStyle& style = ImGui::GetStyle();
		//style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.62f);
		
	}
	
	
	void Destory_ImGui(){
#ifdef OPENGL_BACKEND
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
#endif
#ifdef DEKO3D_BACKEND
		
		
#endif

	}
	
	void Destroy_Backend(){
#ifdef OPENGL_BACKEND
		glfwDestroyWindow(window);
		glfwTerminate();
#endif
#ifdef DEKO3D_BACKEND
		imgui::nx::exit();

		// wait for queue to be idle
		s_queue.waitIdle();

		imgui::deko3d::exit();
		deko3dExit();
#endif
	}
	
	void Destroy(){
#ifdef OPENGL_BACKEND
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(window);
		glfwTerminate();
#endif
	
	}
	
	void NewFrame(){

#ifdef OPENGL_BACKEND
		ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
#endif
#ifdef DEKO3D_BACKEND
		
		
#endif
	}

	void Render_PreMPV(){
		ImGui::Render();
		ImGuiIO &io = ImGui::GetIO();
#ifdef OPENGL_BACKEND
		glClearColor(0.00f, 0.00f, 0.00f, 1.00f);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));  // we have to set the Viewport on every cycle because mpv_render_context_render internally rescales the fb of the context(?!)...
			
#endif
#ifdef DEKO3D_BACKEND

		if (s_width != io.DisplaySize.x || s_height != io.DisplaySize.y) {
			s_width  = io.DisplaySize.x;
			s_height = io.DisplaySize.y;
			rebuildSwapchain(s_width, s_height);
		}

		// get image from queue
		imageSlot = s_queue.acquireImage(s_swapchain);
		auto &cmdBuf    = s_cmdBuf[imageSlot];

		cmdBuf.clear();

		// bind frame/depth buffers and clear them
		dk::ImageView colorTarget{s_frameBuffers[imageSlot]};
		dk::ImageView depthTarget{s_depthBuffer};
		cmdBuf.bindRenderTargets(&colorTarget, &depthTarget);
		cmdBuf.setScissors(0, DkScissor{0, 0, s_width, s_height});
		cmdBuf.clearColor(0, DkColorMask_RGBA, 0.35f, 0.30f, 0.35f, 1.0f);
		cmdBuf.clearDepthStencil(true, 1.0f, 0xFF, 0);
		s_queue.submitCommands(cmdBuf.finishList());
		s_queue.waitIdle();
#endif
	}
	void Render_PostMPV(){
#ifdef OPENGL_BACKEND
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
#endif
#ifdef DEKO3D_BACKEND
		auto &cmdBuf    = s_cmdBuf[imageSlot];
		imgui::deko3d::render(s_device, s_queue, cmdBuf, imageSlot);

		// wait for fragments to be completed before discarding depth/stencil buffer
		cmdBuf.barrier(DkBarrier_Fragments, 0);
		cmdBuf.discardDepthStencil();

		// present image
		s_queue.presentImage(s_swapchain, imageSlot);
#endif
	}
	
	void Resize(float w,float h){
#ifdef OPENGL_BACKEND
		WIDTH = static_cast<int>(w);
		HEIGHT = static_cast<int>(h);
		glfwSetWindowSize(window,w,h);
#endif
	
	}
	void Quit(){
		
	}
	
	uint64_t Process_UI_Events(std::chrono::time_point<std::chrono::system_clock> myeventtime){
		
		uint64_t ret_event = 0;
		

#ifdef DEKO3D_BACKEND		
		
		uint64_t keydown =  imgui::nx::newFrame();
		
		if (keydown & HidNpadButton_A && !B_A_PRESS){
			ret_event = bit_set(ret_event,BUT_A);
		}
		if (keydown & HidNpadButton_B && !B_B_PRESS){
			ret_event = bit_set(ret_event,BUT_B);
		}
		if (keydown & HidNpadButton_Y && !B_Y_PRESS){
			ret_event = bit_set(ret_event,BUT_Y);
		}
		if (keydown & HidNpadButton_X && !B_X_PRESS){
			ret_event = bit_set(ret_event,BUT_X);
		}
		if (keydown & HidNpadButton_Up && !B_DU_PRESS){
			ret_event = bit_set(ret_event,BUT_DUP);
		}
		if (keydown & HidNpadButton_Down && !B_DD_PRESS){
			ret_event = bit_set(ret_event,BUT_DDOWN);
		}
		if (keydown & HidNpadButton_Left && !B_DL_PRESS){
			ret_event = bit_set(ret_event,BUT_DLEFT);
		}
		if (keydown & HidNpadButton_Right && !B_DR_PRESS){
			ret_event = bit_set(ret_event,BUT_DRIGHT);
		}
		if (keydown & HidNpadButton_L && !B_L_PRESS){
			ret_event = bit_set(ret_event,BUT_L);
		}
		if (keydown & HidNpadButton_R && !B_R_PRESS){
			ret_event = bit_set(ret_event,BUT_R);
		}
		if (keydown & HidNpadButton_ZL && !B_ZL_PRESS){
			ret_event = bit_set(ret_event,BUT_ZL);
		}
		if (keydown & HidNpadButton_ZR && !B_ZR_PRESS){
			ret_event = bit_set(ret_event,BUT_ZR);
		}
		if (keydown & HidNpadButton_StickL && !B_TL_PRESS){
			ret_event = bit_set(ret_event,BUT_TL);
		}
		if (keydown & HidNpadButton_StickR && !B_TR_PRESS){
			ret_event = bit_set(ret_event,BUT_TR);
		}
		if (keydown & HidNpadButton_StickRUp){
			ret_event = bit_set(ret_event,B_AX_R_UP);
		}
		if (keydown & HidNpadButton_StickRDown){
			ret_event = bit_set(ret_event,B_AX_R_DOWN);
		}
		if (keydown & HidNpadButton_Minus && !B_MINUS_PRESS){
			ret_event = bit_set(ret_event,BUT_MINUS);
		}
		   
		   
		B_ZL_PRESS = keydown & HidNpadButton_ZL;
		B_ZR_PRESS = keydown & HidNpadButton_ZR;
			
		B_A_PRESS = keydown & HidNpadButton_A;
		B_X_PRESS = keydown & HidNpadButton_X;
		B_B_PRESS = keydown & HidNpadButton_B;
		B_Y_PRESS = keydown & HidNpadButton_Y;
		B_DD_PRESS = keydown & HidNpadButton_Down;
		B_DU_PRESS = keydown & HidNpadButton_Up;
			
		B_DL_PRESS = keydown & HidNpadButton_Left;
		B_DR_PRESS = keydown & HidNpadButton_Right;

		B_R_PRESS = keydown & HidNpadButton_R;
		B_L_PRESS = keydown & HidNpadButton_L;
		B_TR_PRESS = keydown & HidNpadButton_StickR;
		B_TL_PRESS = keydown & HidNpadButton_StickL;

		//B_PLUS_PRESS = state.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS;
		B_MINUS_PRESS = keydown & HidNpadButton_Minus;
			
			
			
		B_AX_L_UP_PRESS = keydown & HidNpadButton_StickLUp;
		B_AX_L_DOWN_PRESS = keydown & HidNpadButton_StickLDown;
		B_AX_L_LEFT_PRESS = keydown & HidNpadButton_StickLLeft;
		B_AX_L_RIGHT_PRESS = keydown & HidNpadButton_StickLRight;
		    

#endif

#ifdef OPENGL_BACKEND	
		
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
			
			if(state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB] == GLFW_PRESS && state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB] == GLFW_PRESS && !B_TR_PRESS){
				ret_event = bit_set(ret_event,BUT_TR);
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
			
			if(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]>0.25f && !AX_R_DOWN.pressed){
				ret_event = bit_set(ret_event,B_AX_R_DOWN);
			}
			if(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]<-0.25f && !AX_R_UP.pressed){
				ret_event = bit_set(ret_event,B_AX_R_UP);
			}
			if(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]>0.25f && !AX_R_RIGHT.pressed){
				ret_event = bit_set(ret_event,B_AX_R_RIGHT);
			}
			if(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]<-0.25f && !AX_R_LEFT.pressed){
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
			
			/*
			B_AX_R_UP_PRESS = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]>0.25f;
			B_AX_R_DOWN_PRESS = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]<-0.25f;
			B_AX_R_LEFT_PRESS = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]>0.25f;
			B_AX_R_RIGHT_PRESS = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]<-0.25f;
			*/
			
			//AX_R_UP.pressed = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]>0.25f;
			//AX_R_DOWN.pressed = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]<-0.25f;
			//AX_R_LEFT.pressed = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]>0.25f;
			//AX_R_RIGHT.pressed = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]<-0.25f;
			
			
			//auto AX_R_UP_CHRONO = std::chrono::duration_cast<std::chrono::microseconds>(myeventtime-AX_R_UP.eventchrono);
			
			
		}
		
	#endif	
	
	return ret_event;
	}
	
	bool WindowShouldClose(){
#ifdef OPENGL_BACKEND
		return glfwWindowShouldClose(window);
#endif
#ifdef DEKO3D_BACKEND
		return false;
#endif
	}
	
	void UniFontLoader(std::vector<fonttype_struct> fontsarray,bool LoadSystemFonts,bool latinonly){
		NXLOG::DEBUGLOG("Init Font Array\n");
		if(LoadSystemFonts){
			
			PlFontData standard, extended, chinese, korean;
			static ImWchar extended_range[] = {0xE000, 0xE152};
        
			if ((R_SUCCEEDED(plGetSharedFontByType(std::addressof(standard), PlSharedFontType_Standard))) &&
				R_SUCCEEDED(plGetSharedFontByType(std::addressof(extended), PlSharedFontType_NintendoExt)) &&
				R_SUCCEEDED(plGetSharedFontByType(std::addressof(chinese), PlSharedFontType_ChineseSimplified)) &&
				R_SUCCEEDED(plGetSharedFontByType(std::addressof(korean), PlSharedFontType_KO))) {
					
				u8 *px = nullptr;
				int w = 0, h = 0, bpp = 0;
				ImFontConfig font_cfg;
				
				font_cfg.FontDataOwnedByAtlas = false;
				ImGui::GetIO().Fonts->AddFontFromMemoryTTF(standard.address, standard.size, fontsarray[0].size, std::addressof(font_cfg), ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
				font_cfg.MergeMode = true;
				
				ImGui::GetIO().Fonts->AddFontFromMemoryTTF(extended.address, extended.size, fontsarray[0].size, std::addressof(font_cfg), extended_range);
				
				if (!latinonly) {
					ImGui::GetIO().Fonts->AddFontFromMemoryTTF(standard.address, standard.size, fontsarray[0].size, std::addressof(font_cfg), ImGui::GetIO().Fonts->GetGlyphRangesJapanese());
					ImGui::GetIO().Fonts->AddFontFromMemoryTTF(chinese.address,  chinese.size,  fontsarray[0].size, std::addressof(font_cfg), ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());
					ImGui::GetIO().Fonts->AddFontFromMemoryTTF(korean.address,   korean.size,   fontsarray[0].size, std::addressof(font_cfg), ImGui::GetIO().Fonts->GetGlyphRangesKorean());
					
					
				}
				
				// build font atlas
				ImGui::GetIO().Fonts->GetTexDataAsAlpha8(std::addressof(px), std::addressof(w), std::addressof(h), std::addressof(bpp));
				ImGui::GetIO().Fonts->Flags |= ImFontAtlasFlags_NoPowerOfTwoHeight;
				ImGui::GetIO().Fonts->Build();
			}
			
		}else{
			
			unsigned char *pixels = nullptr;
			int width = 0, height = 0, bpp = 0;
			ImFontConfig font_cfg = ImFontConfig();
				
			font_cfg.OversampleH = font_cfg.OversampleV = 1;
			font_cfg.PixelSnapH = true;
				
			font_cfg.OversampleH = font_cfg.OversampleV = 1;
			font_cfg.PixelSnapH = true;
			NXLOG::DEBUGLOG("Loading TTF\n");
			
			font_cfg.OversampleH = font_cfg.OversampleV = 1;
			
			
			for(int i=0;i<fontsarray.size();i++){
				ImGui::GetIO().Fonts->AddFontFromFileTTF(fontsarray[i].filename.c_str(), fontsarray[i].size, &font_cfg, fontsarray[i].charrange.Data);
				font_cfg.MergeMode = true;
			}
			
			
			ImGui::GetIO().Fonts->Flags |= ImFontAtlasFlags_NoPowerOfTwoHeight;
			
			ImGui::GetIO().Fonts->Build();
		
		}
	}

#ifdef DEKO3D_BACKEND	

	Texture load_texture(std::string path,DkImageFormat format, std::uint32_t flags,int desc_slot) {
		//auto desc_slot = this->find_descriptor_slot();
		if (desc_slot < 0) {
			std::printf("Failed to allocate descriptor slot for %s\n", path.data());
			return {};
		}
		
		auto &cmdbuf = s_cmdBuf[0];
		 
		int width, height;
		unsigned char* image_data = stbi_load(path.c_str(), &width, &height, NULL, 4);
			if (image_data == NULL)
				return {};
		
		 

		dk::UniqueMemBlock transfer = dk::MemBlockMaker(s_device, imgui::deko3d::align(width*height*4, DK_MEMBLOCK_ALIGNMENT))
			.setFlags(DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached)
			.create();

		
		memcpy(transfer.getCpuAddr(),image_data,width*height*4);
		

		dk::ImageLayout layout;
		dk::ImageLayoutMaker(s_device)
			.setFlags(flags)
			.setFormat(format)
			.setDimensions(width, height)
			.initialize(layout);

		dk::UniqueMemBlock out_memblock = dk::MemBlockMaker(s_device,
				imgui::deko3d::align(layout.getSize(), std::max(layout.getAlignment(), std::uint32_t(DK_MEMBLOCK_ALIGNMENT))))
			.setFlags(DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image)
			.create();
		if (!out_memblock) {
			std::printf("Failed to allocate memblock for %s\n", path.data());
			return {};
		}

		dk::Image out_image;
		out_image.initialize(layout, out_memblock, 0);

		auto out_view = dk::ImageView(out_image);
		auto sampler = dk::Sampler()
			.setFilter(DkFilter_Linear, DkFilter_Linear)
			.setWrapMode(DkWrapMode_ClampToEdge, DkWrapMode_ClampToEdge, DkWrapMode_ClampToEdge);

		
		s_samplerDescriptors[desc_slot].initialize(sampler);
		s_imageDescriptors  [desc_slot].initialize(out_view);
		auto out_handle = dkMakeTextureHandle(desc_slot, desc_slot);

		cmdbuf.copyBufferToImage(DkCopyBuf{transfer.getGpuAddr()}, out_view,
			DkImageRect{0, 0, 0, std::uint32_t(width), std::uint32_t(height), 1});
		cmdbuf.barrier(DkBarrier_None, DkInvalidateFlags_Descriptors);
		s_queue.submitCommands(cmdbuf.finishList());
		s_queue.waitIdle();

		return Texture{out_image, std::move(out_memblock), out_handle,width,height};
	}

#endif
	
	void Create_VO_FrameBuffer(float w,float h){
#ifdef OPENGL_BACKEND	
	
		glGenFramebuffers(1, &mpv_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, mpv_fbo);

		glGenTextures(1, &mpv_fbotexture);
		glBindTexture(GL_TEXTURE_2D, mpv_fbotexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mpv_fbotexture, 0);

		//glGenRenderbuffers(1, &mpv_rbo);
		//glBindRenderbuffer(GL_RENDERBUFFER, mpv_rbo);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mpv_rbo);

		//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		//	NXLOG::ERRORLOG("ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glBindTexture(GL_TEXTURE_2D, 0);
		//glBindRenderbuffer(GL_RENDERBUFFER, 0);
		
		
		fbo = {
					.fbo = static_cast<int>(mpv_fbo),
					.w = static_cast<int>(w),
					.h = static_cast<int>(h),
			};
			
		int __fbo_one = 1;
		fbo_params[0] = {MPV_RENDER_PARAM_OPENGL_FBO, &fbo};
		fbo_params[1] = {MPV_RENDER_PARAM_FLIP_Y, &__fbo_one};
		fbo_params[2] = {(mpv_render_param_type)0};

#endif

#ifdef DEKO3D_BACKEND
		mpv_fbo = {nullptr, &readyFence, &doneFence,
                           w,    h,         DkImageFormat_RGBA8_Unorm};
		/*
		mpv_render_param fbo_params[3] = {{MPV_RENDER_PARAM_DEKO3D_FBO, &mpv_fbo},
                                      { MPV_RENDER_PARAM_INVALID,
                                        nullptr }};
		*/
		//fbo_params[0] = {MPV_RENDER_PARAM_DEKO3D_FBO, &mpv_fbo};
		//fbo_params[1] = {MPV_RENDER_PARAM_INVALID};
		//fbo_params[2] = {nullptr};

#endif

	}
	
	void Rescale_VO_Framebuffer(float _width, float _height){
#ifdef OPENGL_BACKEND	
		glBindTexture(GL_TEXTURE_2D, mpv_fbotexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mpv_fbotexture, 0);

		//glBindRenderbuffer(GL_RENDERBUFFER, mpv_rbo);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mpv_rbo);

		fbo = {
					.fbo = static_cast<int>(mpv_fbo),
					.w = static_cast<int>(_width),
					.h = static_cast<int>(_height),
			};
#endif
	}
	
	void SetFullScreen(bool fullscreen){
		
	}
	
	
	void Draw_VO(){
#ifdef OPENGL_BACKEND
		ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(WIDTH,HEIGHT),ImGuiCond_Always);
		
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0,0.0,0.0,0.0));
		
		if(ImGui::Begin("##videowindow",nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoNavFocus)){
					ImGui::Image((void*)(intptr_t)mpv_fbotexture, ImVec2(WIDTH,HEIGHT));
		}
		ImGui::End();
		ImGui::PopStyleVar(3);
		ImGui::PopStyleColor();
	
#endif
	}
#ifdef DEKO3D_BACKEND
	dk::Device getDeko3dDevice() { 
		return s_device; 
	}

	dk::Image* getFramebuffer() {
		imageSlot = s_queue.acquireImage(s_swapchain);
		return &s_frameBuffers[imageSlot];
	}
	
	void UpdateFBO(){
		mpv_fbo.tex = &s_frameBuffers[imageSlot];
		queueSignalFence(&readyFence);
		
	}
	
	void queueWaitFence(DkFence* fence)
	{
		dkQueueWaitFence(s_queue, fence);
		queueFlush();
	}

	void queueSignalFence(DkFence* fence, bool flash)
	{
		dkQueueSignalFence(s_queue, fence, flash);
		queueFlush();
	}

	void queueFlush()
	{
		dkQueueFlush(s_queue);
	}
	
	void queueWaitDoneFence(){
		dkQueueWaitFence(s_queue, &doneFence);
	}
		

#endif
	unsigned int getWidth(){
#ifdef DEKO3D_BACKEND
		return s_width;
#endif
#ifdef OPENGL_BACKEND
		return docked?1920:1280;
#endif
	}
	unsigned int getHeight(){
#ifdef DEKO3D_BACKEND
		return s_height;
#endif
#ifdef OPENGL_BACKEND
		return docked?1080:720;
#endif
	}
	
	void Resize_VO(float w,float h){
#ifdef DEKO3D_BACKEND
		
#endif
	}
	
	mpv_render_param * getMPV_Params(){
		return fbo_params;
	}
	
#ifdef OPENGL_BACKEND
	GLuint getFBO_Texture(){
		return mpv_fbotexture;
	}
#endif
	
	void setEnableTouch(bool value){
#ifdef DEKO3D_BACKEND
		imgui::nx::setEnableTouch(value);
#endif
	}
	
	
	void SetColorTheme(int themecolor){
		
		ColorSetId currentTheme;
		Result sysGetColorSetIdResult = setsysGetColorSetId(&currentTheme);
		
		
		if(themecolor == 0){
			SetDarkTheme();
		}else if(themecolor == 1){
			SetLightTheme();
		} else{
			if(ColorSetId::ColorSetId_Light){
				SetDarkTheme();
			}else{
				SetLightTheme();
			}
		}
	}
	
	
	void SetDarkTheme(){
		Text_color = ImVec4(255, 255, 255, 1.00f);
		Active_color = ImVec4(0, 255, 203, 1.00f);
		Disabled_color = ImVec4(0.41f, 0.40f, 0.40f, 1.00f);
		Window_Bg_color = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
		OptsTab_Bg_color = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
		Popup_Bg_color = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
		NavHover_color = ImVec4(0.0f, 0.0f, 0.0f, 0.20f);
		HeaderHover_color = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
		Button_color = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);
		ButtonActive_color = ImVec4(0, 255, 203, 0.30f);
		
		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.62f);
		style.Colors[ImGuiCol_Text] = Text_color;
		
	}
	void SetLightTheme(){
		Text_color = ImVec4(0, 0, 0, 1.00f);
		Active_color = ImVec4(0, 0, 255, 1.00f);
		Disabled_color = ImVec4(0.41f, 0.40f, 0.40f, 1.00f);
		Window_Bg_color = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
		OptsTab_Bg_color = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
		Popup_Bg_color = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
		NavHover_color = ImVec4(1.0f, 1.0f, 1.0f, 0.20f);
		HeaderHover_color = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
		Button_color = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);
		ButtonActive_color = ImVec4(0, 0, 255, 0.30f);
		
		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.62f);
		style.Colors[ImGuiCol_Text] = Text_color;
		
	}
	
}