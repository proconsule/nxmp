#include "vout.h"
	



CVOUT::CVOUT(){
	
}

CVOUT::~CVOUT(){
	
	glDeleteFramebuffers(1, &mpv_fbo);
	glDeleteTextures(1, &mpv_fbotexture);
	glDeleteRenderbuffers(1,&mpv_rbo);
}

void CVOUT::Create_Framebuffer(float w,float h)
{
	width = w;
	height = h;
	glGenFramebuffers(1, &mpv_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, mpv_fbo);

	glGenTextures(1, &mpv_fbotexture);
	glBindTexture(GL_TEXTURE_2D, mpv_fbotexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mpv_fbotexture, 0);

	glGenRenderbuffers(1, &mpv_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, mpv_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mpv_rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		NXLOG::ERRORLOG("ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void CVOUT::Rescale_Framebuffer(float _width, float _height)
{
	glBindTexture(GL_TEXTURE_2D, mpv_fbotexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mpv_fbotexture, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, mpv_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mpv_rbo);
}


void CVOUT::Resize(float w,float h){
	width = w;
	height = h;
	Rescale_Framebuffer(w,h);
}

void CVOUT::Draw(){
	if(changevis){
		ImGui::SetNextWindowPos(fullscreen?fullscreen_videopos:windowed_videopos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(fullscreen?ImVec2(width,height): ImVec2(200.0f,200.0f/1.77),ImGuiCond_Always);
		changevis = true;
	}
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	
	if(ImGui::Begin("##videowindow",nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoNavFocus)){
				ImGui::Image((void*)(intptr_t)mpv_fbotexture, fullscreen?ImVec2(width,height):ImVec2(windowed_width,windowed_height),{0, 1}, {1, 0});
	}
	if(!fullscreen)ImGui::SetWindowFocus();
	ImGui::End();
	ImGui::PopStyleVar();

	
}

void CVOUT::SetFullScreen(bool _fullscreen){
	return;
	if(_fullscreen){
		changevis=true;
		//Rescale_Framebuffer(width,height);
		
	}else{
		changevis=true;
		//Rescale_Framebuffer(windowed_width,windowed_height);
		
	}
	fullscreen = _fullscreen;
}
