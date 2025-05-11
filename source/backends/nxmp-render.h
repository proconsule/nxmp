/* DEKO3D Rendring based on rendering routine from https://github.com/averne/Fizeau and https://github.com/averne/SwitchWave */


#ifndef NXMP_RENDER_H
#define NXMP_RENDER_H

#include <array>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

#include <switch.h>
#include <deko3d.hpp>
#include <mpv/render_dk3d.h>
#include "imgui_deko3d.h"
#include "imgui_nx.h"

#include "imgui.h"

#include "libmpv.h"
//#include "utils.hpp"

struct Texture {
	dk::Image image;
	dk::UniqueMemBlock memblock;
	DkResHandle id = -1;
	unsigned int width;
	unsigned int height;
};


class NXMPRenderer {
    public:
        
		constexpr static auto FB_NUM       = 3u;
		constexpr static auto MPV_NUM       = 3u;
		constexpr static auto CMDBUF_SIZE  = 0x10000;
		unsigned int s_width  = 1920;
		unsigned int s_height = 1080;
		constexpr static auto MaxNumDescriptors  = 64;  
		
		
		constexpr auto align_down(auto v, auto a) {
			return v & ~(a - 1);
		}

		constexpr auto align_up(auto v, auto a) {
			return align_down(v + a - 1, a);
		}

    public:
        

    public:
        ~NXMPRenderer();

        int initialize();
		void Init_ImGui();
		void Init_ImGui_deko3d();
		
		void GetFBImage();
		void UpdateVO();
		
        dk::Device get_device() const {
            return this->s_device;
        }

        dk::Queue get_queue() const {
            return this->s_queue;
        }
		
		
        int create_mpv_render_context(libMpv *lmpv);
        void destroy_mpv_render_context();

        Texture create_texture(int width, int height,
            DkImageFormat format, std::uint32_t flags = 0);
        Texture load_texture(std::string_view path,
            DkImageFormat format, std::uint32_t flags = 0);
		Texture load_texture_from_mem(unsigned char *image_data,int width, int height,int channels,DkImageFormat format, std::uint32_t flags);
	
	
		
        void begin_frame();
        void end_frame();
		
		
        void wait_idle() {
            this->s_queue.waitIdle();
        }
		
		
		mpv_render_context *mpv_context;
		
        constexpr auto mask(auto bit) {
			return (static_cast<decltype(bit)>(1) << bit) - 1;
		}


		void unregister_texture(const Texture &tex) {
			free_descriptor_slot(tex.id & mask(20));
		}

    private:
        
        void rebuild_swapchain();
        static void applet_hook_cb(AppletHookType hook, void *param);


        int find_descriptor_slot();
        void free_descriptor_slot(int slot);

    

    private:
	
	    void mpv_render_thread_fn(std::stop_token token);
	
		std::jthread     mpv_render_thread;
        
		std::mutex       render_mtx;
        std::atomic_bool force_mpv_render = false;

        std::condition_variable mpv_redraw_condvar;
        std::mutex              mpv_redraw_mutex;
        std::atomic_int         mpv_redraw_count = 0;
	
		
		dk::UniqueDevice       s_device;
		
		dk::UniqueMemBlock     s_fbMemBlock;
		dk::Image              s_frameBuffers[FB_NUM];
		dk::Image    		   mpv_images[MPV_NUM];
		dk::Fence			   mpv_copy_fences[MPV_NUM];
		
			
		dk::UniqueMemBlock     s_cmdMemBlock;
		dk::UniqueCmdBuf       s_cmdBuf;
		dk::Fence              s_cmdBufFences[FB_NUM];

		dk::UniqueMemBlock     s_descriptorMemBlock;
		dk::SamplerDescriptor *s_samplerDescriptors = nullptr;
		dk::ImageDescriptor   *s_imageDescriptors   = nullptr;
		
		dk::UniqueQueue        s_queue;
		dk::UniqueSwapchain    s_swapchain;
		
		
		std::array<std::uint64_t, 2> allocated_descriptors = {};
		
		
		int imageSlot = 0;
		std::atomic_int                           cur_libmpv_image = -1;
		
		int mpv_imgslot = 0;
			
		Texture splash_texture;
		
		
	
};


#endif