#include "imgloader.h"
#include "logger.h"


#include "stb_image.h"
#include <turbojpeg.h>

enum{
	JPGFILE,
	PNGFILE,
	BMPFILE,
	WEBPFILE,
	GIFFILE
	
};



CImgLoader::CImgLoader(){


	

}


void CImgLoader::LoadBaseTextures(std::string basepath){
	
	NXLOG::DEBUGLOG("LOADING TEXTURES\n");

	icons.SdCardTexture =  Renderer->load_texture(basepath+"/sdcard.png",DkImageFormat_RGBA8_Unorm, 0);
	icons.UsbTexture =  Renderer->load_texture(basepath+"/usb.png",DkImageFormat_RGBA8_Unorm, 0);
	icons.NetworkTexture =  Renderer->load_texture(basepath+"/network.png",DkImageFormat_RGBA8_Unorm, 0);
	icons.Enigma2Texture =  Renderer->load_texture(basepath+"/enigma2.png",DkImageFormat_RGBA8_Unorm, 0);
	
	icons.PlaylistTexture = Renderer->load_texture(basepath+"/playlist.png",DkImageFormat_RGBA8_Unorm, 0);
	icons.UPNPTexture =  Renderer->load_texture(basepath+"/upnp.png",DkImageFormat_RGBA8_Unorm, 0);
	icons.InfoTexture =  Renderer->load_texture(basepath+"/info.png",DkImageFormat_RGBA8_Unorm, 0);
	icons.SettingsTexture =  Renderer->load_texture(basepath+"/settings.png",DkImageFormat_RGBA8_Unorm, 0);
	icons.ExitTexture =  Renderer->load_texture(basepath+"/exit.png",DkImageFormat_RGBA8_Unorm, 0);

	icons.FolderTexture =  Renderer->load_texture(basepath+"/folder.png",DkImageFormat_RGBA8_Unorm, 0);
	icons.FileTexture =  Renderer->load_texture(basepath+"/file.png",DkImageFormat_RGBA8_Unorm, 0);
	icons.PDFTexture =  Renderer->load_texture(basepath+"/pdficon.png",DkImageFormat_RGBA8_Unorm, 0);
	icons.ImageTexture =  Renderer->load_texture(basepath+"/image.png",DkImageFormat_RGBA8_Unorm, 0);
	icons.ArchiveTexture =  Renderer->load_texture(basepath+"/archive.png",DkImageFormat_RGBA8_Unorm, 0);

	icons.ShareAddTexture =  Renderer->load_texture(basepath+"/shareadd.png",DkImageFormat_RGBA8_Unorm, 0);
	icons.HTTPTexture =  Renderer->load_texture(basepath+"/http.png",DkImageFormat_RGBA8_Unorm, 0);
	icons.FTPTexture =  Renderer->load_texture(basepath+"/ftp.png",DkImageFormat_RGBA8_Unorm, 0);
	icons.SFTPTexture =  Renderer->load_texture(basepath+"/sftp.png",DkImageFormat_RGBA8_Unorm, 0);
	icons.SMBTexture = Renderer->load_texture(basepath+"/smb.png",DkImageFormat_RGBA8_Unorm, 0);
	icons.NFSTexture =  Renderer->load_texture(basepath+"/nfs.png",DkImageFormat_RGBA8_Unorm, 0);

	icons.PlayIcon =  Renderer->load_texture(basepath+"/player/play.png",DkImageFormat_RGBA8_Unorm, 0);
	icons.StopIcon =  Renderer->load_texture(basepath+"/player/stop.png",DkImageFormat_RGBA8_Unorm, 0);
	icons.PauseIcon =  Renderer->load_texture(basepath+"/player/pause.png",DkImageFormat_RGBA8_Unorm, 0);
	icons.MuteIcon =  Renderer->load_texture(basepath+"/player/mute.png",DkImageFormat_RGBA8_Unorm, 0);
	icons.VolumeIcon =  Renderer->load_texture(basepath+"/player/volume.png",DkImageFormat_RGBA8_Unorm, 0);
	icons.LoopIcon =  Renderer->load_texture(basepath+"/player/loop.png",DkImageFormat_RGBA8_Unorm, 0);
	icons.NoLoopIcon =  Renderer->load_texture(basepath+"/player/noloop.png",DkImageFormat_RGBA8_Unorm, 0);

	icons.NXMPBannerTexture =  Renderer->load_texture(basepath+"/nxmp-banner.jpg",DkImageFormat_RGBA8_Unorm, 0);
	icons.FFMPEGTexture =  Renderer->load_texture(basepath+"/ffmpeg.png",DkImageFormat_RGBA8_Unorm, 0);
	icons.MPVTexture =  Renderer->load_texture(basepath+"/mpv.png",DkImageFormat_RGBA8_Unorm, 0);

	NXLOG::DEBUGLOG("DONE\r\n");
}


CImgLoader::~CImgLoader(){


	/* Home Icons */
    Renderer->unregister_texture(icons.SdCardTexture);
	Renderer->unregister_texture(icons.UsbTexture);
	Renderer->unregister_texture(icons.NetworkTexture);
	Renderer->unregister_texture(icons.Enigma2Texture);
	Renderer->unregister_texture(icons.UPNPTexture);
	Renderer->unregister_texture(icons.PlaylistTexture);
	Renderer->unregister_texture(icons.SettingsTexture);
	Renderer->unregister_texture(icons.InfoTexture);
	Renderer->unregister_texture(icons.ExitTexture);
	
	
	/* File Browser Icons */
	
	Renderer->unregister_texture(icons.FileTexture);
	Renderer->unregister_texture(icons.FolderTexture);
	Renderer->unregister_texture(icons.GUI_D_UP);
	Renderer->unregister_texture(icons.GUI_D_DOWN);
	Renderer->unregister_texture(icons.UPNPTexture);
	Renderer->unregister_texture(icons.GUI_D_LEFT);
	Renderer->unregister_texture(icons.GUI_D_RIGHT);
	Renderer->unregister_texture(icons.GUI_A_BUT);
	Renderer->unregister_texture(icons.GUI_B_BUT);
	Renderer->unregister_texture(icons.GUI_X_BUT);
	Renderer->unregister_texture(icons.GUI_Y_BUT);
	
	
	/* Networks Icons */
	
	Renderer->unregister_texture(icons.ShareAddTexture);
	Renderer->unregister_texture(icons.HTTPTexture);
	Renderer->unregister_texture(icons.FTPTexture);
	Renderer->unregister_texture(icons.SFTPTexture);
	Renderer->unregister_texture(icons.SMBTexture);
	Renderer->unregister_texture(icons.NFSTexture);
	
	
	/* Player Icons */
	Renderer->unregister_texture(icons.PlayIcon);
	Renderer->unregister_texture(icons.StopIcon);
	Renderer->unregister_texture(icons.PauseIcon);
	Renderer->unregister_texture(icons.MuteIcon);
	Renderer->unregister_texture(icons.VolumeIcon);
	Renderer->unregister_texture(icons.LoopIcon);
	Renderer->unregister_texture(icons.NoLoopIcon);
	
	
	/* Info Icons */
	
	Renderer->unregister_texture(icons.FFMPEGTexture);
	Renderer->unregister_texture(icons.MPVTexture);
	Renderer->unregister_texture(icons.NXMPBannerTexture);
	
	
	NXLOG::DEBUGLOG("Unregister Textures\r\n");
	
}

int guessImageMemoryFormat(unsigned char *_img_data){
	int guessret = -1;
	if(_img_data[0]=='G' && _img_data[1]=='I' && _img_data[2]=='F' && _img_data[3]=='8') {
		guessret = GIFFILE;
	}
	else if(_img_data[0]==0xff && _img_data[1]==0xd8){
		guessret = JPGFILE;
	}
	else if(_img_data[0]==0x89 && _img_data[1]==0x50 && _img_data[2]==0x4e && _img_data[3]==0x47) {
		guessret = PNGFILE;
	}
	else if(_img_data[0]==0x42 && _img_data[1]==0x4d) {
		guessret = BMPFILE;
	}
	
	
	return guessret;
}	


int guessImageFormat(std::string path){
	char testread[5];
	FILE *fptr = fopen(path.c_str(), "rb");
	int guessret = -1;
	
	
	
	if(fgets(testread,5,fptr)) {
		testread[4]=0;
		if(!strcmp((char *)testread,"GIF8")) {
			guessret = GIFFILE;
		}
		else if(testread[0]==0xff && testread[1]==0xd8){
			guessret = JPGFILE;
		}
		else if(testread[0]==0x89 && testread[1]==0x50 && testread[2]==0x4e && testread[3]==0x47) {
			guessret = PNGFILE;
		}
		else if(testread[0]==0x42 && testread[1]==0x4d) {
			guessret = BMPFILE;
		}
	}
	fclose(fptr);
	return guessret;
}	


unsigned char * jpg_decode(unsigned char *_img_data,int _size,int *width, int *height, int *channels){
	
	tjhandle _jpegDecompressor = tjInitDecompress();

	int jpegSubsamp;
	tjDecompressHeader2(_jpegDecompressor, _img_data, _size, width, height, &jpegSubsamp);

	int img_w = *width;
	int img_h = *height;

	unsigned char * buffer = (unsigned char *)malloc(img_w*img_h*4);
	*channels = 4;
	tjDecompress2(_jpegDecompressor, _img_data, _size, buffer, img_w, 0/*pitch*/, img_h, TJPF_RGBA, TJFLAG_FASTDCT);

	tjDestroy(_jpegDecompressor);
	return buffer;
}


Texture CImgLoader::OpenImageMemory(unsigned char *_img_data,int _size){
	
	int myformat = guessImageMemoryFormat(_img_data);
	if(myformat == -1){
		return {};
	}
	
	if(myformat == PNGFILE || myformat == BMPFILE || myformat == GIFFILE){
		int width, height, channels;
		unsigned char* image_data = stbi_load_from_memory(_img_data,_size, &width, &height, NULL, 4);
		channels = 4;
		Texture rettext = Renderer->load_texture_from_mem(image_data,width,height,channels,DkImageFormat_RGBA8_Unorm, 0);
		free(image_data); 
		return rettext;
	}
	if(myformat == JPGFILE){
		int width, height, channels;
		unsigned char* image_data = jpg_decode(_img_data,_size, &width, &height,&channels);
		Texture rettext =  Renderer->load_texture_from_mem(image_data,width,height,channels,DkImageFormat_RGBA8_Unorm, 0);
		free(image_data); 
		return rettext;
	}
	
	return {};
}

Texture CImgLoader::OpenRAWImageMemory(unsigned char *_img_data,int _w,int _h){
	
	Texture rettext =  Renderer->load_texture_from_mem(_img_data,_w,_h,4,DkImageFormat_RGBA8_Unorm, 0);
	return rettext;
	
}


Texture CImgLoader::OpenImageFile(std::string path){
	
	int myformat = guessImageFormat(path);
	if(myformat == -1){
		return {};
	}
	
	if(myformat == JPGFILE || myformat == PNGFILE || myformat == BMPFILE || myformat == GIFFILE){
		return Renderer->load_texture(path,DkImageFormat_RGBA8_Unorm, 0);
	} 
	
	return {};
}

