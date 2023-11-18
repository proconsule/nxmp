#include "imgloader.h"
#include "nxmp-gfx.h"
#include "logger.h"

CImgLoader::CImgLoader(std::string basepath){

#ifdef OPENGL_BACKEND
	/* Home Icons */
	
	Utility::TxtLoadFromFile(basepath+"/sdcard.png",&icons.SdCardTexture.id,&icons.SdCardTexture.width,&icons.SdCardTexture.height);
	Utility::TxtLoadFromFile(basepath+"/usb.png",&icons.UsbTexture.id,&icons.UsbTexture.width,&icons.UsbTexture.height);
	Utility::TxtLoadFromFile(basepath+"/network.png",&icons.NetworkTexture.id,&icons.NetworkTexture.width,&icons.NetworkTexture.height);
	Utility::TxtLoadFromFile(basepath+"/enigma2.png",&icons.Enigma2Texture.id,&icons.Enigma2Texture.width,&icons.Enigma2Texture.height);
	Utility::TxtLoadFromFile(basepath+"/playlist.png",&icons.PlaylistTexture.id,&icons.PlaylistTexture.width,&icons.PlaylistTexture.height);
	Utility::TxtLoadFromFile(basepath+"/upnp.png",&icons.UPNPTexture.id,&icons.UPNPTexture.width,&icons.UPNPTexture.height);
	Utility::TxtLoadFromFile(basepath+"/info.png",&icons.InfoTexture.id,&icons.InfoTexture.width,&icons.InfoTexture.height);
	Utility::TxtLoadFromFile(basepath+"/settings.png",&icons.SettingsTexture.id,&icons.SettingsTexture.width,&icons.SettingsTexture.height);
	Utility::TxtLoadFromFile(basepath+"/exit.png",&icons.ExitTexture.id,&icons.ExitTexture.width,&icons.ExitTexture.height);
	
	/* File Browser Icons */
	Utility::TxtLoadFromFile(basepath+"/folder.png",&icons.FolderTexture.id,&icons.FolderTexture.width,&icons.FolderTexture.height);
	Utility::TxtLoadFromFile(basepath+"/file.png",&icons.FileTexture.id,&icons.FileTexture.width,&icons.FileTexture.height);
	Utility::TxtLoadFromFile(basepath+"/gui/dpad-up.png",&icons.GUI_D_UP.id,&icons.GUI_D_UP.width,&icons.GUI_D_UP.height);
	Utility::TxtLoadFromFile(basepath+"/gui/dpad-down.png",&icons.GUI_D_DOWN.id,&icons.GUI_D_DOWN.width,&icons.GUI_D_DOWN.height);
	Utility::TxtLoadFromFile(basepath+"/gui/dpad-left.png",&icons.GUI_D_LEFT.id,&icons.GUI_D_LEFT.width,&icons.GUI_D_LEFT.height);
	Utility::TxtLoadFromFile(basepath+"/gui/dpad-right.png",&icons.GUI_D_RIGHT.id,&icons.GUI_D_RIGHT.width,&icons.GUI_D_RIGHT.height);	
	Utility::TxtLoadFromFile(basepath+"/gui/a-but.png",&icons.GUI_A_BUT.id,&icons.GUI_A_BUT.width,&icons.GUI_A_BUT.height);
	Utility::TxtLoadFromFile(basepath+"/gui/b-but.png",&icons.GUI_B_BUT.id,&icons.GUI_B_BUT.width,&icons.GUI_B_BUT.height);
	Utility::TxtLoadFromFile(basepath+"/gui/x-but.png",&icons.GUI_X_BUT.id,&icons.GUI_X_BUT.width,&icons.GUI_X_BUT.height);
	Utility::TxtLoadFromFile(basepath+"/gui/y-but.png",&icons.GUI_Y_BUT.id,&icons.GUI_Y_BUT.width,&icons.GUI_Y_BUT.height);
	
	
	/* Networks Icons */
	Utility::TxtLoadFromFile(basepath+"/shareadd.png",&icons.ShareAddTexture.id,&icons.ShareAddTexture.width,&icons.ShareAddTexture.height);
	Utility::TxtLoadFromFile(basepath+"/http.png",&icons.HTTPTexture.id,&icons.HTTPTexture.width,&icons.HTTPTexture.height);
	Utility::TxtLoadFromFile(basepath+"/ftp.png",&icons.FTPTexture.id,&icons.FTPTexture.width,&icons.FTPTexture.height);
	Utility::TxtLoadFromFile(basepath+"/sftp.png",&icons.SFTPTexture.id,&icons.SFTPTexture.width,&icons.SFTPTexture.height);
	Utility::TxtLoadFromFile(basepath+"/smb.png",&icons.SMBTexture.id,&icons.SMBTexture.width,&icons.SMBTexture.height);
	Utility::TxtLoadFromFile(basepath+"/nfs.png",&icons.NFSTexture.id,&icons.NFSTexture.width,&icons.NFSTexture.height);
	
	/* Player Icons */
	Utility::TxtLoadFromFile(basepath+"/player/play.png",&icons.PlayIcon.id,&icons.PlayIcon.width,&icons.PlayIcon.height);
	Utility::TxtLoadFromFile(basepath+"/player/stop.png",&icons.StopIcon.id,&icons.StopIcon.width,&icons.StopIcon.height);
	Utility::TxtLoadFromFile(basepath+"/player/pause.png",&icons.PauseIcon.id,&icons.PauseIcon.width,&icons.PauseIcon.height);
	Utility::TxtLoadFromFile(basepath+"/player/mute.png",&icons.MuteIcon.id,&icons.MuteIcon.width,&icons.MuteIcon.height);
	Utility::TxtLoadFromFile(basepath+"/player/volume.png",&icons.VolumeIcon.id,&icons.VolumeIcon.width,&icons.VolumeIcon.height);
	Utility::TxtLoadFromFile(basepath+"/player/loop.png",&icons.LoopIcon.id,&icons.LoopIcon.width,&icons.LoopIcon.height);
	Utility::TxtLoadFromFile(basepath+"/player/noloop.png",&icons.NoLoopIcon.id,&icons.NoLoopIcon.width,&icons.NoLoopIcon.height);
	
	/* Info Icons */
	Utility::TxtLoadFromFile(basepath+"/nxmp-banner.jpg",&icons.NXMPBannerTexture.id,&icons.NXMPBannerTexture.width,&icons.NXMPBannerTexture.height);
	Utility::TxtLoadFromFile(basepath+"/ffmpeg.png",&icons.FFMPEGTexture.id,&icons.FFMPEGTexture.width,&icons.FFMPEGTexture.height);
	Utility::TxtLoadFromFile(basepath+"/mpv.png",&icons.MPVTexture.id,&icons.MPVTexture.width,&icons.MPVTexture.height);

#endif
	
#ifdef DEKO3D_BACKEND
	/* Home Icons */
	
	
	icons.SdCardTexture =  nxmpgfx::load_texture(basepath+"/sdcard.png",DkImageFormat_RGBA8_Unorm, 0,1);
	
	icons.UsbTexture =  nxmpgfx::load_texture(basepath+"/usb.png",DkImageFormat_RGBA8_Unorm, 0,2);
	icons.NetworkTexture =  nxmpgfx::load_texture(basepath+"/network.png",DkImageFormat_RGBA8_Unorm, 0,3);
	icons.Enigma2Texture =  nxmpgfx::load_texture(basepath+"/enigma2.png",DkImageFormat_RGBA8_Unorm, 0,4);
	
	icons.PlaylistTexture = nxmpgfx::load_texture(basepath+"/playlist.png",DkImageFormat_RGBA8_Unorm, 0,5);
	icons.UPNPTexture =  nxmpgfx::load_texture(basepath+"/upnp.png",DkImageFormat_RGBA8_Unorm, 0,6);
	icons.InfoTexture =  nxmpgfx::load_texture(basepath+"/info.png",DkImageFormat_RGBA8_Unorm, 0,7);
	icons.SettingsTexture =  nxmpgfx::load_texture(basepath+"/settings.png",DkImageFormat_RGBA8_Unorm, 0,8);
	icons.ExitTexture =  nxmpgfx::load_texture(basepath+"/exit.png",DkImageFormat_RGBA8_Unorm, 0,9);

	icons.FolderTexture =  nxmpgfx::load_texture(basepath+"/folder.png",DkImageFormat_RGBA8_Unorm, 0,10);
	icons.FileTexture =  nxmpgfx::load_texture(basepath+"/file.png",DkImageFormat_RGBA8_Unorm, 0,11);


	icons.ShareAddTexture =  nxmpgfx::load_texture(basepath+"/shareadd.png",DkImageFormat_RGBA8_Unorm, 0,12);
	icons.HTTPTexture =  nxmpgfx::load_texture(basepath+"/http.png",DkImageFormat_RGBA8_Unorm, 0,13);
	icons.FTPTexture =  nxmpgfx::load_texture(basepath+"/ftp.png",DkImageFormat_RGBA8_Unorm, 0,14);
	icons.SFTPTexture =  nxmpgfx::load_texture(basepath+"/sftp.png",DkImageFormat_RGBA8_Unorm, 0,15);
	icons.SMBTexture =  nxmpgfx::load_texture(basepath+"/smb.png",DkImageFormat_RGBA8_Unorm, 0,16);
	icons.NFSTexture =  nxmpgfx::load_texture(basepath+"/nfs.png",DkImageFormat_RGBA8_Unorm, 0,17);

	icons.PlayIcon =  nxmpgfx::load_texture(basepath+"/player/play.png",DkImageFormat_RGBA8_Unorm, 0,18);
	icons.StopIcon =  nxmpgfx::load_texture(basepath+"/player/stop.png",DkImageFormat_RGBA8_Unorm, 0,19);
	icons.PauseIcon =  nxmpgfx::load_texture(basepath+"/player/pause.png",DkImageFormat_RGBA8_Unorm, 0,20);
	icons.MuteIcon =  nxmpgfx::load_texture(basepath+"/player/mute.png",DkImageFormat_RGBA8_Unorm, 0,21);
	icons.VolumeIcon =  nxmpgfx::load_texture(basepath+"/player/volume.png",DkImageFormat_RGBA8_Unorm, 0,22);
	icons.LoopIcon =  nxmpgfx::load_texture(basepath+"/player/loop.png",DkImageFormat_RGBA8_Unorm, 0,23);
	icons.NoLoopIcon =  nxmpgfx::load_texture(basepath+"/player/noloop.png",DkImageFormat_RGBA8_Unorm, 0,24);

	icons.NXMPBannerTexture =  nxmpgfx::load_texture(basepath+"/nxmp-banner.jpg",DkImageFormat_RGBA8_Unorm, 0,25);
	icons.FFMPEGTexture =  nxmpgfx::load_texture(basepath+"/ffmpeg.png",DkImageFormat_RGBA8_Unorm, 0,26);
	icons.MPVTexture =  nxmpgfx::load_texture(basepath+"/mpv.png",DkImageFormat_RGBA8_Unorm, 0,27);


	
	
#endif
}

CImgLoader::~CImgLoader(){

#ifdef OPENGL_BACKEND

	/* Home Icons */
	glDeleteTextures(1, &icons.SdCardTexture.id);
	glDeleteTextures(1, &icons.UsbTexture.id);
	glDeleteTextures(1, &icons.NetworkTexture.id);
	glDeleteTextures(1, &icons.Enigma2Texture.id);
	glDeleteTextures(1, &icons.UPNPTexture.id);
	glDeleteTextures(1, &icons.PlaylistTexture.id);
	glDeleteTextures(1, &icons.SettingsTexture.id);
	glDeleteTextures(1, &icons.InfoTexture.id);
	glDeleteTextures(1, &icons.ExitTexture.id);
	
	/* File Browser Icons */
	glDeleteTextures(1, &icons.FileTexture.id);
	glDeleteTextures(1, &icons.FolderTexture.id);
	glDeleteTextures(1, &icons.GUI_D_UP.id);
	glDeleteTextures(1, &icons.GUI_D_DOWN.id);
	glDeleteTextures(1, &icons.GUI_D_LEFT.id);
	glDeleteTextures(1, &icons.GUI_D_RIGHT.id);
	glDeleteTextures(1, &icons.GUI_A_BUT.id);
	glDeleteTextures(1, &icons.GUI_B_BUT.id);
	glDeleteTextures(1, &icons.GUI_X_BUT.id);
	glDeleteTextures(1, &icons.GUI_Y_BUT.id);
	
	/* Networks Icons */
	glDeleteTextures(1, &icons.ShareAddTexture.id);
	glDeleteTextures(1, &icons.HTTPTexture.id);
	glDeleteTextures(1, &icons.FTPTexture.id);
	glDeleteTextures(1, &icons.SFTPTexture.id);
	glDeleteTextures(1, &icons.SMBTexture.id);
	glDeleteTextures(1, &icons.NFSTexture.id);
	
	/* Player Icons */
	glDeleteTextures(1, &icons.PlayIcon.id);
	glDeleteTextures(1, &icons.StopIcon.id);
	glDeleteTextures(1, &icons.PauseIcon.id);
	glDeleteTextures(1, &icons.MuteIcon.id);
	glDeleteTextures(1, &icons.VolumeIcon.id);
	glDeleteTextures(1, &icons.LoopIcon.id);
	glDeleteTextures(1, &icons.NoLoopIcon.id);
	
	/* Info Icons */
	
	glDeleteTextures(1, &icons.FFMPEGTexture.id);
	glDeleteTextures(1, &icons.MPVTexture.id);
	glDeleteTextures(1, &icons.NXMPBannerTexture.id);
	
	
	
	
	/* Home Icons */
	icons.SdCardTexture.id = 0;
	icons.UsbTexture.id = 0;
	icons.NetworkTexture.id = 0;
	icons.Enigma2Texture.id = 0;
	icons.UPNPTexture.id = 0;
	icons.PlaylistTexture.id = 0;
	icons.SettingsTexture.id = 0;
	icons.InfoTexture.id = 0;
	icons.ExitTexture.id = 0;
	
	/* File Browser Icons */
	icons.FileTexture.id = 0;
	icons.FolderTexture.id = 0;
	icons.GUI_D_UP.id = 0;
	icons.GUI_D_DOWN.id = 0;
	icons.GUI_D_LEFT.id = 0;
	icons.GUI_D_RIGHT.id = 0;
	icons.GUI_A_BUT.id = 0;
	icons.GUI_B_BUT.id = 0;
	icons.GUI_X_BUT.id = 0;
	icons.GUI_Y_BUT.id = 0;
	
	/* Networks Icons */
	icons.ShareAddTexture.id = 0;
	icons.HTTPTexture.id = 0;
	icons.FTPTexture.id = 0;
	icons.SFTPTexture.id = 0;
	icons.SMBTexture.id = 0;
	icons.NFSTexture.id = 0;
	
	/* Player Icons */
	icons.PlayIcon.id = 0;
	icons.StopIcon.id = 0;
	icons.PauseIcon.id = 0;
	icons.MuteIcon.id = 0;
	icons.VolumeIcon.id = 0;
	icons.LoopIcon.id = 0;
	icons.NoLoopIcon.id = 0;
	
	/* Info Icons */
	
	icons.FFMPEGTexture.id = 0;
	icons.MPVTexture.id = 0;
	icons.NXMPBannerTexture.id = 0;
	
	
	
	
	
#endif
	
}
