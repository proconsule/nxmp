
/* DEKO3D Rendring based on rendering routine from https://github.com/averne/Fizeau and https://github.com/averne/SwitchWave */

#include "nxmp-render.h"
#include <cstring>
#include <numeric>

#include "stb_image.h"


dk::UniqueMemBlock s_codeMemBlock;

	
void NXMPRenderer::Init_ImGui_deko3d(){
	
	auto desc_slot = find_descriptor_slot();
	
	imgui::deko3d::init(s_device,
        s_queue,
        s_cmdBuf,
        s_samplerDescriptors[desc_slot],
        s_imageDescriptors[desc_slot],
        dkMakeTextureHandle(desc_slot, desc_slot),
        FB_NUM);
}
	
void NXMPRenderer::Init_ImGui(){
	
		
		
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

		
}
	
NXMPRenderer::~NXMPRenderer() {
    
	
	this->mpv_render_thread.request_stop();
    this->mpv_redraw_condvar.notify_all();

    this->s_queue.waitIdle();


	
	imgui::nx::exit();
    imgui::deko3d::exit();
	
	
	
    
}


int NXMPRenderer::create_mpv_render_context(libMpv *lmpv) {
    mpv_deko3d_init_params dk_init = {
        .device = this->s_device,
    };

	mpv_context = lmpv->getContext();

    int advanced_control = 1;
    mpv_render_param params[]{
        {MPV_RENDER_PARAM_API_TYPE,
         const_cast<char *>(MPV_RENDER_API_TYPE_DEKO3D)},
        {MPV_RENDER_PARAM_DEKO3D_INIT_PARAMS, &dk_init},
        {MPV_RENDER_PARAM_ADVANCED_CONTROL, &advanced_control},
        {MPV_RENDER_PARAM_INVALID, nullptr}};

    mpv_render_context_create(&mpv_context, lmpv->getHandle(), params);
				
	this->mpv_render_thread = std::jthread(&NXMPRenderer::mpv_render_thread_fn, this);
		
			
	mpv_render_context_set_update_callback(mpv_context,
        +[](void *user) {
            auto self = static_cast<NXMPRenderer *>(user);
            auto lk = std::scoped_lock(self->mpv_redraw_mutex);
            self->mpv_redraw_count++;
            self->mpv_redraw_condvar.notify_one();
        },
        this
    );
	
    return 0;
}

void NXMPRenderer::destroy_mpv_render_context() {
	auto *tmp = std::exchange(mpv_context, nullptr);
	mpv_render_context_free(tmp);
}

int NXMPRenderer::find_descriptor_slot() {
    int slot = -1;
    for (auto &pos: this->allocated_descriptors) {
        if (pos == -1ull)
            continue;
        slot = __builtin_ctzll(~pos);
        pos |= (1ull << slot);
        break;
    }
    return slot;
}


Texture NXMPRenderer::create_texture(int width, int height,
        DkImageFormat format, std::uint32_t flags) {
    auto desc_slot = find_descriptor_slot();
		auto &cmdbuf = s_cmdBuf;
		if (desc_slot < 0) {
			std::printf("Failed to allocate descriptor slot for image\n");
			return {};
		}

		dk::ImageLayout layout;
		dk::ImageLayoutMaker(s_device)
			.setFlags(flags)
			.setFormat(format)
			.setDimensions(width, height)
			.initialize(layout);

		dk::UniqueMemBlock out_memblock = dk::MemBlockMaker(s_device,
				align_up(layout.getSize(), std::max(layout.getAlignment(), std::uint32_t(DK_MEMBLOCK_ALIGNMENT))))
			.setFlags(DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image)
			.create();
		if (!out_memblock) {
			std::printf("Failed to allocate memblock for image\n");
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

		cmdbuf.barrier(DkBarrier_None, DkInvalidateFlags_Descriptors);
		s_queue.submitCommands(cmdbuf.finishList());
		
		return Texture{out_image, std::move(out_memblock), out_handle,(unsigned int)width,(unsigned int)height};
}

Texture NXMPRenderer::load_texture(std::string_view path, 
        DkImageFormat format, std::uint32_t flags) {
    
    auto &cmdbuf = s_cmdBuf;
		 
		int width, height;
		unsigned char* image_data = stbi_load(path.data(), &width, &height, NULL, 4);
			if (image_data == NULL)
				return {};
		
		auto texture = create_texture(width, height, format, flags);
		if (texture.id == static_cast<DkResHandle>(-1))
			return {};
		
		dk::UniqueMemBlock transfer = dk::MemBlockMaker(s_device, align_up(width*height*4, DK_MEMBLOCK_ALIGNMENT))
        .setFlags(DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached)
        .create();

		memcpy(transfer.getCpuAddr(),image_data,width*height*4);
		
		cmdbuf.copyBufferToImage(DkCopyBuf{transfer.getGpuAddr()}, dk::ImageView(texture.image),
        DkImageRect{0, 0, 0, std::uint32_t(width), std::uint32_t(height), 1});
		s_queue.submitCommands(cmdbuf.finishList());
		s_queue.waitIdle();
		
		free(image_data);

		return Texture{texture.image, std::move(texture.memblock), texture.id,width,height};
}

Texture NXMPRenderer::load_texture_from_mem(unsigned char *image_data,int mywidth, int myheight,int channels,DkImageFormat format, std::uint32_t flags){
	
	if(image_data == NULL)return {};
		auto &cmdbuf = s_cmdBuf;
		
		auto texture = create_texture(mywidth, myheight, format, flags);
		if (texture.id == static_cast<DkResHandle>(-1))
			return {}; 

		dk::UniqueMemBlock transfer = dk::MemBlockMaker(s_device, align_up(mywidth*myheight*4, DK_MEMBLOCK_ALIGNMENT))
        .setFlags(DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached)
        .create();

		memcpy(transfer.getCpuAddr(),image_data,mywidth*myheight*4);
		
		cmdbuf.copyBufferToImage(DkCopyBuf{transfer.getGpuAddr()}, dk::ImageView(texture.image),
        DkImageRect{0, 0, 0, std::uint32_t(mywidth), std::uint32_t(myheight), 1});
		s_queue.submitCommands(cmdbuf.finishList());
		s_queue.waitIdle();

		return Texture{texture.image, std::move(texture.memblock), texture.id,mywidth,myheight};
	
}

void NXMPRenderer::free_descriptor_slot(int slot) {
    this->allocated_descriptors[slot / 64] &= ~(1ull << (slot % 64));
}


void NXMPRenderer::rebuild_swapchain() {
	
	
	if (appletGetOperationMode() == AppletOperationMode_Console)
        s_width = 1920, s_height = 1080;
    else
       s_width = 1280, s_height = 720;
    
	
	printf("Rebuild Swap Chain %d %d\r\n",s_width,s_height);
	
    s_swapchain = nullptr;


    // create framebuffer image layout
    dk::ImageLayout fbLayout;
    dk::ImageLayoutMaker{s_device}
        .setFlags(DkImageFlags_UsageRender | DkImageFlags_UsagePresent | DkImageFlags_HwCompression|DkImageFlags_Usage2DEngine)
        .setFormat(DkImageFormat_RGBA8_Unorm)
        .setDimensions(s_width, s_height)
        .initialize(fbLayout);

    //auto const fbAlign = fbLayout.getAlignment();
    //auto const fbSize  = fbLayout.getSize();
	auto fb_size = align_up(fbLayout.getSize(), fbLayout.getAlignment());

    // create framebuffer memblock
    if (!s_fbMemBlock) {
        s_fbMemBlock = dk::MemBlockMaker{s_device, (FB_NUM + MPV_NUM) * fb_size}
        .setFlags(DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image)
        .create();
    }

    // initialize swapchain images
    std::array<DkImage const *, FB_NUM> swapchainImages;
    for (unsigned i = 0; i < FB_NUM; ++i) {
        swapchainImages[i] = &s_frameBuffers[i];
        s_frameBuffers[i].initialize(fbLayout, s_fbMemBlock, i * fb_size);
    }
	
	s_swapchain = dk::SwapchainMaker{s_device, nwindowGetDefault(), swapchainImages}.create();
	
	for (int i = 0; i < MPV_NUM; ++i)
        this->mpv_images[i].initialize(fbLayout, s_fbMemBlock, (i + FB_NUM) * fb_size);

    // create swapchain
    
}

int NXMPRenderer::initialize() {
	
	// create deko3d device
	s_device = dk::DeviceMaker{}
		.create();
	
	s_queue = dk::QueueMaker{s_device}.setFlags(DkQueueFlags_Graphics).create();
	
	// initialize swapchain with maximum resolution

	rebuild_swapchain();
	
	
	s_cmdMemBlock =
	dk::MemBlockMaker{s_device, align_up(CMDBUF_SIZE*FB_NUM, DK_MEMBLOCK_ALIGNMENT)}
		.setFlags(DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached)
		.create();

        // create command buffer
	s_cmdBuf = dk::CmdBufMaker{s_device}.create();
	s_cmdBuf.addMemory(s_cmdMemBlock, 0, s_cmdMemBlock.getSize());
	
	
	static_assert(sizeof(dk::ImageDescriptor)   == DK_IMAGE_DESCRIPTOR_ALIGNMENT);
    static_assert(sizeof(dk::SamplerDescriptor) == DK_SAMPLER_DESCRIPTOR_ALIGNMENT);
    static_assert(DK_IMAGE_DESCRIPTOR_ALIGNMENT == DK_SAMPLER_DESCRIPTOR_ALIGNMENT);
	
	
	
    s_descriptorMemBlock = dk::MemBlockMaker{s_device,
        align_up(
            NXMPRenderer::MaxNumDescriptors * (sizeof(dk::SamplerDescriptor) + sizeof(dk::ImageDescriptor)), DK_MEMBLOCK_ALIGNMENT)}
                               .setFlags(DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached)
             			 .create();

	

    // get cpu address for descriptors
    s_samplerDescriptors =
        static_cast<dk::SamplerDescriptor *> (s_descriptorMemBlock.getCpuAddr());
    s_imageDescriptors =
        reinterpret_cast<dk::ImageDescriptor *>(this->s_samplerDescriptors + NXMPRenderer::MaxNumDescriptors);
	


	s_cmdBuf.bindSamplerDescriptorSet(s_descriptorMemBlock.getGpuAddr(), NXMPRenderer::MaxNumDescriptors);
    s_cmdBuf.bindImageDescriptorSet(
        s_descriptorMemBlock.getGpuAddr() + 64 * sizeof(dk::SamplerDescriptor),
         NXMPRenderer::MaxNumDescriptors);
    s_queue.submitCommands(s_cmdBuf.finishList());
    
    if(s_width == 1920)ImGui::GetIO().FontGlobalScale = 1.5f;
	if(s_width == 720)ImGui::GetIO().FontGlobalScale = 1.0f;
	
	
	
	return 1;
}

void NXMPRenderer::GetFBImage(){
	
	imageSlot = s_queue.acquireImage(s_swapchain);
	
}

void NXMPRenderer::UpdateVO() {
	
	
	auto libmpv_slot = this->cur_libmpv_image.load();
	if (libmpv_slot >= 0) {
        this->s_cmdBuf.copyImage(
            dk::ImageView(this->mpv_images[libmpv_slot]),          DkImageRect{ 0, 0, 0, this->s_width, this->s_height, 1 },
            dk::ImageView(this->s_frameBuffers[imageSlot]), DkImageRect{ 0, 0, 0, this->s_width, this->s_height, 1 });
        this->s_cmdBuf.signalFence(this->mpv_copy_fences[libmpv_slot]);
        this->s_queue.submitCommands(this->s_cmdBuf.finishList());
    }
	
}

void NXMPRenderer::begin_frame() {
		
		s_cmdBuf.clear();

		s_cmdBuf.addMemory(s_cmdMemBlock, imageSlot * NXMPRenderer::CMDBUF_SIZE, NXMPRenderer::CMDBUF_SIZE);
		auto dst_view = dk::ImageView(s_frameBuffers[imageSlot]);
		s_cmdBuf.bindRenderTargets(&dst_view);
		s_cmdBuf.setViewports(0, DkViewport{ 0.0f, 0.0f, static_cast<float>(s_width), static_cast<float>(s_height) });
		s_cmdBuf.setScissors(0, DkScissor{ 0, 0, s_width, s_height });
		s_cmdBuf.clearColor(0, DkColorMask_RGBA, 0, 0, 0);

		s_queue.submitCommands(s_cmdBuf.finishList());
		
		
	
}

void NXMPRenderer::end_frame() {
	
		
		imgui::deko3d::render(s_device, s_queue, s_cmdBuf, imageSlot);
		s_cmdBufFences[imageSlot].wait();
		s_cmdBuf.barrier(DkBarrier_Fragments, 0);
		
		s_queue.submitCommands(s_cmdBuf.finishList());
		
		s_queue.waitIdle();
		dkQueuePresentImage(s_queue,s_swapchain,imageSlot);
		
		
}

void NXMPRenderer::mpv_render_thread_fn(std::stop_token token) {
	
	dk::Fence done_fence, ready_fence;

    while (true) {
        if (!this->mpv_redraw_count) {
            auto lk = std::unique_lock(this->mpv_redraw_mutex);
            this->mpv_redraw_condvar.wait(lk);
        }

        if (token.stop_requested())
            break;

        this->mpv_redraw_count--;
        if (!this->mpv_context || !((mpv_render_context_update(this->mpv_context) & MPV_RENDER_UPDATE_FRAME)))
			continue;
		
		
		{
            auto lk = std::scoped_lock(this->render_mtx);

            // Duplicate check in case the context was destroyed before we acquire the mutex
            if (!this->mpv_context)
                continue;

            int slot;
            
            slot = (this->cur_libmpv_image.load() + 1) % NXMPRenderer::MPV_NUM;
            

            mpv_deko3d_fbo myfbo = {
                .tex         = &this->mpv_images[slot],
                .ready_fence = &this->mpv_copy_fences[slot],
                .done_fence  = &done_fence,
                .w           = static_cast<int>(this->s_width),
                .h           = static_cast<int>(this->s_height),
                .format      = DkImageFormat_RGBA8_Unorm,
            };

            mpv_render_param myparams[] = {
                {MPV_RENDER_PARAM_DEKO3D_FBO, &myfbo},
                {},
            };

            mpv_render_context_render(this->mpv_context, myparams);

            // Wait for the rendering to complete before presenting
            done_fence.wait();
            this->cur_libmpv_image = slot;
            
            mpv_render_context_report_swap(this->mpv_context);
        }
	
	}
	NXLOG::DEBUGLOG("Exit MPV Render Loop\r\n");
	
}