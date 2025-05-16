#ifndef NXMP_IMGLOADER_H
#define NXMP_IMGLOADER_H

#include "utils.h"
//#include "nxmp-gfx.h"
#include "nxmp-render.h"
#include "deko3d.hpp"




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
	Texture PDFTexture;
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
	


class CImgLoader{
public:
		CImgLoader();
		void LoadBaseTextures(std::string basepath);
		~CImgLoader();
		nxmpicon_struct icons;
		
		NXMPRenderer *Renderer;
		
		Texture OpenImageFile(std::string path);
		Texture OpenImageMemory(unsigned char *_img_data,int _size);
		Texture OpenRAWImageMemory(unsigned char *_img_data,int _w,int _h);

};




#endif