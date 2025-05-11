#ifndef NXMP_IMGLOADER_H
#define NXMP_IMGLOADER_H

#include "utils.h"
//#include "nxmp-gfx.h"
#include "nxmp-render.h"
#include "deko3d.hpp"

#ifdef OPENGL_BACKEND

typedef struct {
	/* Home Icons */
	Tex SdCardTexture;
	Tex UsbTexture;
	Tex NetworkTexture;
	Tex UPNPTexture;
	Tex Enigma2Texture;
	Tex PlaylistTexture;
	Tex InfoTexture;
	Tex SettingsTexture;
	Tex ExitTexture;
	
	/* File Browser Icons */
	Tex FolderTexture;
	Tex FileTexture;
	Tex ImageTexture;
	Tex ArchiveTexture;
	Tex GUI_D_UP;
	Tex GUI_D_DOWN;
	Tex GUI_D_LEFT;
	Tex GUI_D_RIGHT;
	Tex GUI_A_BUT;
	Tex GUI_B_BUT;
	Tex GUI_X_BUT;
	Tex GUI_Y_BUT;
	
	/* Networks Icons */
	Tex ShareAddTexture;
	Tex FTPTexture;
	Tex HTTPTexture;
	Tex SFTPTexture;
	Tex SMBTexture;
	Tex NFSTexture;
	
	/* Info Icons */
	Tex NXMPBannerTexture;
	Tex FFMPEGTexture;
	Tex MPVTexture;
	
	
	/* Player Icons */
	Tex PlayIcon;
	Tex PauseIcon;
	Tex StopIcon;
	Tex MuteIcon;
	Tex VolumeIcon;
	Tex LoopIcon;
	Tex NoLoopIcon;
		
} nxmpicon_struct;
	
#endif

#ifdef DEKO3D_BACKEND

typedef struct {
	/* Home Icons */
	Texture SdCardTexture;
	Texture UsbTexture;
	Texture NetworkTexture;
	Texture UPNPTexture;
	Texture Enigma2Texture;
	Texture PlaylistTexture;
	Texture InfoTexture;
	Texture SettingsTexture;
	Texture ExitTexture;
	
	/* File Browser Icons */
	Texture FolderTexture;
	Texture FileTexture;
	Texture ImageTexture;
	Texture ArchiveTexture;
	Texture GUI_D_UP;
	Texture GUI_D_DOWN;
	Texture GUI_D_LEFT;
	Texture GUI_D_RIGHT;
	Texture GUI_A_BUT;
	Texture GUI_B_BUT;
	Texture GUI_X_BUT;
	Texture GUI_Y_BUT;
	
	/* Networks Icons */
	Texture ShareAddTexture;
	Texture FTPTexture;
	Texture HTTPTexture;
	Texture SFTPTexture;
	Texture SMBTexture;
	Texture NFSTexture;
	
	/* Info Icons */
	Texture NXMPBannerTexture;
	Texture FFMPEGTexture;
	Texture MPVTexture;
	
	
	/* Player Icons */
	Texture PlayIcon;
	Texture PauseIcon;
	Texture StopIcon;
	Texture MuteIcon;
	Texture VolumeIcon;
	Texture LoopIcon;
	Texture NoLoopIcon;
		
} nxmpicon_struct;
	
#endif

class CImgLoader{
public:
		CImgLoader();
		void LoadBaseTextures(std::string basepath);
		~CImgLoader();
		nxmpicon_struct icons;
		
		NXMPRenderer *Renderer;
		
#ifdef OPENGL_BACKEND
		Tex OpenImageFile(std::string path);
		Tex OpenImageMemory(unsigned char *_img_data,int _size);
#endif
#ifdef DEKO3D_BACKEND
		Texture OpenImageFile(std::string path);
		Texture OpenImageMemory(unsigned char *_img_data,int _size);
#endif

};




#endif