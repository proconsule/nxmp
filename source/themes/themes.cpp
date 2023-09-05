#include "themes.h"

#include "SimpleIni.h"


typedef struct {
	std::string configstring;
	ImGuiCol_ colenum;
}style_struct;


std::vector<style_struct> configstyles = {{"ImGuiCol_Text",ImGuiCol_Text},
{"ImGuiCol_TextDisabled",ImGuiCol_TextDisabled},
{"ImGuiCol_WindowBg",ImGuiCol_WindowBg},
{"ImGuiCol_ChildBg",ImGuiCol_ChildBg},
{"ImGuiCol_PopupBg",ImGuiCol_PopupBg},
{"ImGuiCol_Border",ImGuiCol_Border},
{"ImGuiCol_BorderShadow",ImGuiCol_BorderShadow},
{"ImGuiCol_FrameBg",ImGuiCol_FrameBg},
{"ImGuiCol_FrameBgHovered",ImGuiCol_FrameBgHovered},
{"ImGuiCol_FrameBgActive",ImGuiCol_FrameBgActive},
{"ImGuiCol_TitleBg",ImGuiCol_TitleBg},
{"ImGuiCol_TitleBgActive",ImGuiCol_TitleBgActive},
{"ImGuiCol_TitleBgCollapsed",ImGuiCol_TitleBgCollapsed},
{"ImGuiCol_MenuBarBg",ImGuiCol_MenuBarBg},
{"ImGuiCol_ScrollbarBg",ImGuiCol_ScrollbarBg},
{"ImGuiCol_ScrollbarGrab",ImGuiCol_ScrollbarGrab},
{"ImGuiCol_ScrollbarGrabActive",ImGuiCol_ScrollbarGrabActive},
{"ImGuiCol_CheckMark",ImGuiCol_CheckMark},
{"ImGuiCol_SliderGrab",ImGuiCol_SliderGrab},
{"ImGuiCol_SliderGrabActive",ImGuiCol_SliderGrabActive},
{"ImGuiCol_Button",ImGuiCol_Button},
{"ImGuiCol_ButtonHovered",ImGuiCol_ButtonHovered},
{"ImGuiCol_ButtonActive",ImGuiCol_ButtonActive},
{"ImGuiCol_Header",ImGuiCol_Header},
{"ImGuiCol_HeaderHovered",ImGuiCol_HeaderHovered},
{"ImGuiCol_HeaderActive",ImGuiCol_HeaderActive},
{"ImGuiCol_Separator",ImGuiCol_Separator},
{"ImGuiCol_SeparatorHovered",ImGuiCol_SeparatorHovered},
{"ImGuiCol_SeparatorActive",ImGuiCol_SeparatorActive},
{"ImGuiCol_ResizeGrip",ImGuiCol_ResizeGrip},
{"ImGuiCol_ResizeGripHovered",ImGuiCol_ResizeGripHovered},
{"ImGuiCol_ResizeGripActive",ImGuiCol_ResizeGripActive},
{"ImGuiCol_Tab",ImGuiCol_Tab},
{"ImGuiCol_TabHovered",ImGuiCol_TabHovered},
{"ImGuiCol_TabActive",ImGuiCol_TabActive},
{"ImGuiCol_TabUnfocused",ImGuiCol_TabUnfocused},
{"ImGuiCol_TabUnfocusedActive",ImGuiCol_TabUnfocusedActive},
{"ImGuiCol_PlotLines",ImGuiCol_PlotLines},
{"ImGuiCol_PlotLinesHovered",ImGuiCol_PlotLinesHovered},
{"ImGuiCol_PlotHistogram",ImGuiCol_PlotHistogram},
{"ImGuiCol_PlotHistogramHovered",ImGuiCol_PlotHistogramHovered},
{"ImGuiCol_TableHeaderBg",ImGuiCol_TableHeaderBg},
{"ImGuiCol_TableBorderStrong",ImGuiCol_TableBorderStrong},
{"ImGuiCol_TableBorderLight",ImGuiCol_TableBorderLight},
{"ImGuiCol_TableRowBg",ImGuiCol_TableRowBg},
{"ImGuiCol_TableRowBgAlt",ImGuiCol_TableRowBgAlt},
{"ImGuiCol_TextSelectedBg",ImGuiCol_TextSelectedBg},
{"ImGuiCol_DragDropTarget",ImGuiCol_DragDropTarget},
{"ImGuiCol_NavHighlight",ImGuiCol_NavHighlight},
{"ImGuiCol_NavWindowingHighlight",ImGuiCol_NavWindowingHighlight},
{"ImGuiCol_NavWindowingDimBg",ImGuiCol_NavWindowingDimBg},
{"ImGuiCol_ModalWindowDimBg",ImGuiCol_ModalWindowDimBg},

};

Themes::Themes(){
	
}

void Themes::getThemes(){
	struct dirent *ent;
	DIR *dir;
	std::string path = "./themes/";
	themeslist.clear();
	if (!path.empty()) {
		if ((dir = opendir(path.c_str())) != nullptr) {
			while ((ent = readdir(dir)) != nullptr) {
				if ((path == "/" || strlen(ent->d_name) == 1) && ent->d_name[0] == '.') {
					continue;
				}
				if ((path == "/" || strlen(ent->d_name) == 2) && ent->d_name[0] == '.' && ent->d_name[1] == '.') {
					continue;
				}
				if (ent->d_name[0] == '.') {
					if (strlen(ent->d_name) != 2 && ent->d_name[1] != '.') {
						continue;
					}
				}
				std::string dirpath = path + ent->d_name;
				
				struct stat st{};
				if (stat(dirpath.c_str(), &st) == 0) {
					if(S_ISDIR(st.st_mode)){
						
						std::string themeinifile = dirpath + "/"+std::string("theme.ini");
						CSimpleIniA *ini;
						ini = new CSimpleIniA(true,true);
						ini->SetUnicode();
						SI_Error rc = ini->LoadFile(themeinifile.c_str());
						if(rc<0){
							delete ini;
							continue;
						}
						const char* namepv = ini->GetValue("Theme", "Name");
						const char* authorpv = ini->GetValue("Theme", "Author");
						double fontsize =  ini->GetDoubleValue("Theme","FontSize");
						double smallfontsize =  ini->GetDoubleValue("Theme","FontSmallSize");
						
						delete ini;
						
						themes_struct tmptheme;
						tmptheme.name = namepv;
						tmptheme.author = authorpv;
						tmptheme.path = dirpath + "/";
						tmptheme.fontsize = (float)fontsize;
						tmptheme.fontsmallsize = smallfontsize;
						themeslist.push_back(tmptheme);
					}
				}
					
			}
				
		}
		
	}
		
}


void Themes::setDefault(){

	
	dochangethemefont = true;

	themefontpath = "romfs:/DejaVuSans.ttf";
	themefontsize = 24.0f;
	themefontsmall = 16.0f;
	
	
	glDeleteTextures(1, &nxmpicons.SdCardTexture.id);
	glDeleteTextures(1, &nxmpicons.UsbTexture.id);
	glDeleteTextures(1, &nxmpicons.NetworkTexture.id);
	glDeleteTextures(1, &nxmpicons.Enigma2Texture.id);
	glDeleteTextures(1, &nxmpicons.FileTexture.id);
	glDeleteTextures(1, &nxmpicons.PlaylistTexture.id);
	glDeleteTextures(1, &nxmpicons.InfoTexture.id);
	glDeleteTextures(1, &nxmpicons.FFMPEGTexture.id);
	glDeleteTextures(1, &nxmpicons.HTTPTexture.id);
	glDeleteTextures(1, &nxmpicons.FTPTexture.id);
	glDeleteTextures(1, &nxmpicons.SFTPTexture.id);
	glDeleteTextures(1, &nxmpicons.SMBTexture.id);
	glDeleteTextures(1, &nxmpicons.MPVTexture.id);
	glDeleteTextures(1, &nxmpicons.ExitTexture.id);
	glDeleteTextures(1, &nxmpicons.FolderTexture.id);
	glDeleteTextures(1, &nxmpicons.NXMPBannerTexture.id);
	glDeleteTextures(1, &nxmpicons.SettingsTexture.id);
	glDeleteTextures(1, &nxmpicons.UPNPTexture.id);
	glDeleteTextures(1, &nxmpicons.PlayIcon.id);
	glDeleteTextures(1, &nxmpicons.StopIcon.id);
	glDeleteTextures(1, &nxmpicons.PauseIcon.id);
	glDeleteTextures(1, &nxmpicons.MuteIcon.id);
	glDeleteTextures(1, &nxmpicons.VolumeIcon.id);
	glDeleteTextures(1, &nxmpicons.LoopIcon.id);
	glDeleteTextures(1, &nxmpicons.NoLoopIcon.id);
	
	Utility::TxtLoadFromFile("romfs:/sdcard.png",&nxmpicons.SdCardTexture.id,&nxmpicons.SdCardTexture.width,&nxmpicons.SdCardTexture.height);
	Utility::TxtLoadFromFile("romfs:/usb.png",&nxmpicons.UsbTexture.id,&nxmpicons.UsbTexture.width,&nxmpicons.UsbTexture.height);
	Utility::TxtLoadFromFile("romfs:/network.png",&nxmpicons.NetworkTexture.id,&nxmpicons.NetworkTexture.width,&nxmpicons.NetworkTexture.height);
	Utility::TxtLoadFromFile("romfs:/enigma2.png",&nxmpicons.Enigma2Texture.id,&nxmpicons.Enigma2Texture.width,&nxmpicons.Enigma2Texture.height);
	Utility::TxtLoadFromFile("romfs:/folder.png",&nxmpicons.FolderTexture.id,&nxmpicons.FolderTexture.width,&nxmpicons.FolderTexture.height);
	Utility::TxtLoadFromFile("romfs:/file.png",&nxmpicons.FileTexture.id,&nxmpicons.FileTexture.width,&nxmpicons.FileTexture.height);
	Utility::TxtLoadFromFile("romfs:/playlist.png",&nxmpicons.PlaylistTexture.id,&nxmpicons.PlaylistTexture.width,&nxmpicons.PlaylistTexture.height);
	Utility::TxtLoadFromFile("romfs:/info.png",&nxmpicons.InfoTexture.id,&nxmpicons.InfoTexture.width,&nxmpicons.InfoTexture.height);
	Utility::TxtLoadFromFile("romfs:/settings.png",&nxmpicons.SettingsTexture.id,&nxmpicons.SettingsTexture.width,&nxmpicons.SettingsTexture.height);
	Utility::TxtLoadFromFile("romfs:/ffmpeg.png",&nxmpicons.FFMPEGTexture.id,&nxmpicons.FFMPEGTexture.width,&nxmpicons.FFMPEGTexture.height);
	Utility::TxtLoadFromFile("romfs:/http.png",&nxmpicons.HTTPTexture.id,&nxmpicons.HTTPTexture.width,&nxmpicons.HTTPTexture.height);
	Utility::TxtLoadFromFile("romfs:/ftp.png",&nxmpicons.FTPTexture.id,&nxmpicons.FTPTexture.width,&nxmpicons.FTPTexture.height);
	Utility::TxtLoadFromFile("romfs:/sftp.png",&nxmpicons.SFTPTexture.id,&nxmpicons.SFTPTexture.width,&nxmpicons.SFTPTexture.height);
	Utility::TxtLoadFromFile("romfs:/smb.png",&nxmpicons.SMBTexture.id,&nxmpicons.SMBTexture.width,&nxmpicons.SMBTexture.height);
	Utility::TxtLoadFromFile("romfs:/nfs.png",&nxmpicons.NFSTexture.id,&nxmpicons.NFSTexture.width,&nxmpicons.NFSTexture.height);
	Utility::TxtLoadFromFile("romfs:/upnp.png",&nxmpicons.UPNPTexture.id,&nxmpicons.UPNPTexture.width,&nxmpicons.UPNPTexture.height);
	Utility::TxtLoadFromFile("romfs:/mpv.png",&nxmpicons.MPVTexture.id,&nxmpicons.MPVTexture.width,&nxmpicons.MPVTexture.height);
	Utility::TxtLoadFromFile("romfs:/exit.png",&nxmpicons.ExitTexture.id,&nxmpicons.ExitTexture.width,&nxmpicons.ExitTexture.height);
	Utility::TxtLoadFromFile("romfs:/nxmp-banner.jpg",&nxmpicons.NXMPBannerTexture.id,&nxmpicons.NXMPBannerTexture.width,&nxmpicons.NXMPBannerTexture.height);
	Utility::TxtLoadFromFile("romfs:/player/play.png",&nxmpicons.PlayIcon.id,&nxmpicons.PlayIcon.width,&nxmpicons.PlayIcon.height);
	Utility::TxtLoadFromFile("romfs:/player/stop.png",&nxmpicons.StopIcon.id,&nxmpicons.StopIcon.width,&nxmpicons.StopIcon.height);
	Utility::TxtLoadFromFile("romfs:/player/pause.png",&nxmpicons.PauseIcon.id,&nxmpicons.PauseIcon.width,&nxmpicons.PauseIcon.height);
	Utility::TxtLoadFromFile("romfs:/player/mute.png",&nxmpicons.MuteIcon.id,&nxmpicons.MuteIcon.width,&nxmpicons.MuteIcon.height);
	Utility::TxtLoadFromFile("romfs:/player/volume.png",&nxmpicons.VolumeIcon.id,&nxmpicons.VolumeIcon.width,&nxmpicons.VolumeIcon.height);
	Utility::TxtLoadFromFile("romfs:/player/loop.png",&nxmpicons.LoopIcon.id,&nxmpicons.LoopIcon.width,&nxmpicons.LoopIcon.height);
	Utility::TxtLoadFromFile("romfs:/player/noloop.png",&nxmpicons.NoLoopIcon.id,&nxmpicons.NoLoopIcon.width,&nxmpicons.NoLoopIcon.height);

	
}

void Themes::setTheme(std::string themefolder){
	
	CSimpleIniA *ini;
	ini = new CSimpleIniA(true,true);
	ini->SetUnicode();
	std::string themeinifile = themefolder + "theme.ini";
	SI_Error rc = ini->LoadFile(themeinifile.c_str());
	if(rc<0){
		delete ini;
	}
	//const char* namepv = ini->GetValue("Theme", "Name");
	//const char* authorpv = ini->GetValue("Theme", "Author");
	double fontsize =  ini->GetDoubleValue("Theme","FontSize");
	double smallfontsize =  ini->GetDoubleValue("Theme","FontSmallSize");
				
				
	dochangethemefont = true;
	themefontpath = themefolder+"font.ttf";
	themefontsize = 24.0f;
	themefontsmall = 16.0f;
	
	themefontsize = fontsize;
	themefontsmall = smallfontsize;
	
	glDeleteTextures(1, &nxmpicons.SdCardTexture.id);
	glDeleteTextures(1, &nxmpicons.UsbTexture.id);
	glDeleteTextures(1, &nxmpicons.NetworkTexture.id);
	glDeleteTextures(1, &nxmpicons.Enigma2Texture.id);
	glDeleteTextures(1, &nxmpicons.FileTexture.id);
	glDeleteTextures(1, &nxmpicons.PlaylistTexture.id);
	glDeleteTextures(1, &nxmpicons.InfoTexture.id);
	glDeleteTextures(1, &nxmpicons.FFMPEGTexture.id);
	glDeleteTextures(1, &nxmpicons.HTTPTexture.id);
	glDeleteTextures(1, &nxmpicons.FTPTexture.id);
	glDeleteTextures(1, &nxmpicons.SFTPTexture.id);
	glDeleteTextures(1, &nxmpicons.SMBTexture.id);
	glDeleteTextures(1, &nxmpicons.NFSTexture.id);
	glDeleteTextures(1, &nxmpicons.MPVTexture.id);
	glDeleteTextures(1, &nxmpicons.ExitTexture.id);
	glDeleteTextures(1, &nxmpicons.FolderTexture.id);
	glDeleteTextures(1, &nxmpicons.NXMPBannerTexture.id);
	glDeleteTextures(1, &nxmpicons.SettingsTexture.id);
	glDeleteTextures(1, &nxmpicons.UPNPTexture.id);
	glDeleteTextures(1, &nxmpicons.PlayIcon.id);
	glDeleteTextures(1, &nxmpicons.StopIcon.id);
	glDeleteTextures(1, &nxmpicons.PauseIcon.id);
	glDeleteTextures(1, &nxmpicons.MuteIcon.id);
	glDeleteTextures(1, &nxmpicons.VolumeIcon.id);
	glDeleteTextures(1, &nxmpicons.LoopIcon.id);
	glDeleteTextures(1, &nxmpicons.NoLoopIcon.id);
	
	
	Utility::TxtLoadFromFile(themefolder+"sdcard.png",&nxmpicons.SdCardTexture.id,&nxmpicons.SdCardTexture.width,&nxmpicons.SdCardTexture.height);
	Utility::TxtLoadFromFile(themefolder+"usb.png",&nxmpicons.UsbTexture.id,&nxmpicons.UsbTexture.width,&nxmpicons.UsbTexture.height);
	Utility::TxtLoadFromFile(themefolder+"network.png",&nxmpicons.NetworkTexture.id,&nxmpicons.NetworkTexture.width,&nxmpicons.NetworkTexture.height);
	Utility::TxtLoadFromFile(themefolder+"enigma2.png",&nxmpicons.Enigma2Texture.id,&nxmpicons.Enigma2Texture.width,&nxmpicons.Enigma2Texture.height);
	Utility::TxtLoadFromFile(themefolder+"folder.png",&nxmpicons.FolderTexture.id,&nxmpicons.FolderTexture.width,&nxmpicons.FolderTexture.height);
	Utility::TxtLoadFromFile(themefolder+"file.png",&nxmpicons.FileTexture.id,&nxmpicons.FileTexture.width,&nxmpicons.FileTexture.height);
	Utility::TxtLoadFromFile(themefolder+"playlist.png",&nxmpicons.PlaylistTexture.id,&nxmpicons.PlaylistTexture.width,&nxmpicons.PlaylistTexture.height);
	Utility::TxtLoadFromFile(themefolder+"info.png",&nxmpicons.InfoTexture.id,&nxmpicons.InfoTexture.width,&nxmpicons.InfoTexture.height);
	Utility::TxtLoadFromFile(themefolder+"settings.png",&nxmpicons.SettingsTexture.id,&nxmpicons.SettingsTexture.width,&nxmpicons.SettingsTexture.height);
	Utility::TxtLoadFromFile(themefolder+"ffmpeg.png",&nxmpicons.FFMPEGTexture.id,&nxmpicons.FFMPEGTexture.width,&nxmpicons.FFMPEGTexture.height);
	Utility::TxtLoadFromFile(themefolder+"http.png",&nxmpicons.HTTPTexture.id,&nxmpicons.HTTPTexture.width,&nxmpicons.HTTPTexture.height);
	Utility::TxtLoadFromFile(themefolder+"ftp.png",&nxmpicons.FTPTexture.id,&nxmpicons.FTPTexture.width,&nxmpicons.FTPTexture.height);
	Utility::TxtLoadFromFile(themefolder+"sftp.png",&nxmpicons.SFTPTexture.id,&nxmpicons.SFTPTexture.width,&nxmpicons.SFTPTexture.height);
	Utility::TxtLoadFromFile(themefolder+"smb.png",&nxmpicons.SMBTexture.id,&nxmpicons.SMBTexture.width,&nxmpicons.SMBTexture.height);
	Utility::TxtLoadFromFile(themefolder+"nfs.png",&nxmpicons.NFSTexture.id,&nxmpicons.NFSTexture.width,&nxmpicons.NFSTexture.height);
	Utility::TxtLoadFromFile(themefolder+"upnp.png",&nxmpicons.UPNPTexture.id,&nxmpicons.UPNPTexture.width,&nxmpicons.UPNPTexture.height);
	Utility::TxtLoadFromFile(themefolder+"mpv.png",&nxmpicons.MPVTexture.id,&nxmpicons.MPVTexture.width,&nxmpicons.MPVTexture.height);
	Utility::TxtLoadFromFile(themefolder+"exit.png",&nxmpicons.ExitTexture.id,&nxmpicons.ExitTexture.width,&nxmpicons.ExitTexture.height);
	Utility::TxtLoadFromFile(themefolder+"nxmp-banner.jpg",&nxmpicons.NXMPBannerTexture.id,&nxmpicons.NXMPBannerTexture.width,&nxmpicons.NXMPBannerTexture.height);
	Utility::TxtLoadFromFile(themefolder+"player/play.png",&nxmpicons.PlayIcon.id,&nxmpicons.PlayIcon.width,&nxmpicons.PlayIcon.height);
	Utility::TxtLoadFromFile(themefolder+"player/stop.png",&nxmpicons.StopIcon.id,&nxmpicons.StopIcon.width,&nxmpicons.StopIcon.height);
	Utility::TxtLoadFromFile(themefolder+"player/pause.png",&nxmpicons.PauseIcon.id,&nxmpicons.PauseIcon.width,&nxmpicons.PauseIcon.height);
	Utility::TxtLoadFromFile(themefolder+"player/mute.png",&nxmpicons.MuteIcon.id,&nxmpicons.MuteIcon.width,&nxmpicons.MuteIcon.height);
	Utility::TxtLoadFromFile(themefolder+"player/volume.png",&nxmpicons.VolumeIcon.id,&nxmpicons.VolumeIcon.width,&nxmpicons.VolumeIcon.height);
	Utility::TxtLoadFromFile(themefolder+"player/loop.png",&nxmpicons.LoopIcon.id,&nxmpicons.LoopIcon.width,&nxmpicons.LoopIcon.height);
	Utility::TxtLoadFromFile(themefolder+"player/noloop.png",&nxmpicons.NoLoopIcon.id,&nxmpicons.NoLoopIcon.width,&nxmpicons.NoLoopIcon.height);
	
	
	ImVec4* colors = ImGui::GetStyle().Colors;
	for(int i=0;i<configstyles.size();i++){
		double redval = ini->GetDoubleValue(configstyles[i].configstring.c_str(), "Red");
		double greenval = ini->GetDoubleValue(configstyles[i].configstring.c_str(), "Green");
		double blueval = ini->GetDoubleValue(configstyles[i].configstring.c_str(), "Blue");
		double alphaval = ini->GetDoubleValue(configstyles[i].configstring.c_str(), "Alpha");
		
		colors[configstyles[i].colenum] = ImVec4((float)redval,(float)greenval,(float)blueval,(float)alphaval);
		
	}
	
	
	delete ini;
	
}

int Themes::getThemeIDX(std::string themename){
	for(int i=0;i<themeslist.size();i++){
		if(themename == themeslist[i].name){
			return i;
		}
	}
	return -1;
}

