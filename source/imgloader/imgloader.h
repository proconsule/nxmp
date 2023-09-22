#ifndef NXMP_IMGLOADER_H
#define NXMP_IMGLOADER_H

#include "utils.h"

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
	
class CImgLoader{
public:
		CImgLoader(std::string basepath);
		~CImgLoader();
		nxmpicon_struct icons;

};




#endif