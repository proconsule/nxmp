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
						const char* namepv = ini->GetValue("Theme", "Name","Anonymous");
						const char* authorpv = ini->GetValue("Theme", "Author","Anonymous");
						double handledfontsize =  ini->GetDoubleValue("Theme","HandledFontSize",20.0f);
						double dockedfontsize =  ini->GetDoubleValue("Theme","DockedFontSize",30.0f);
						
						const char* latinfontpv = ini->GetValue("Theme", "LatinFont");
						const char* kanjifontpv = ini->GetValue("Theme", "KanjiFont");
									
						std::string latinfontstr = dirpath + std::string("/") + latinfontpv;
						std::string kanjifontstr = dirpath + std::string("/") + kanjifontpv;
						
						
						delete ini;
						
						themes_struct tmptheme;
						tmptheme.name = namepv;
						tmptheme.author = authorpv;
						tmptheme.path = dirpath + "/";
						tmptheme.handledfontsize = (float)handledfontsize;
						tmptheme.dockedfontsize = (float)dockedfontsize;
						tmptheme.latinfontstr = latinfontstr;
						tmptheme.kanjifontstr = kanjifontstr;
						
						themeslist.push_back(tmptheme);
					}
				}
					
			}
				
		}
		
	}
		
}

void Themes::setDefault(){

	dochangethemefont = true;
	if(imgloader != nullptr){
		delete imgloader;
	}
	
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Header]                 = ImVec4(0.0f, 0.8f, 0.1f, 0.31f);
	colors[ImGuiCol_HeaderHovered]          = ImVec4(0.0f, 0.8f, 0.1f, 0.80f);
	colors[ImGuiCol_HeaderActive]           = ImVec4(0.0f, 0.8f, 0.1f, 1.00f);
	
	imgloader = new CImgLoader("romfs:");
	//Utility::FontLoader("romfs:/DejaVuSans.ttf",currFontsize,"romfs:/Source Han Sans CN Light.otf",currFontsize);
	nxmpgfx::UniFontLoader(getThemeFonts(-1,configini->getOnlyLatinRange(false)),true,configini->getOnlyLatinRange(false));
	
}

void Themes::setThemeColor(std::string themefolder){
	CSimpleIniA *ini;
	ini = new CSimpleIniA(true,true);
	ini->SetUnicode();
	std::string themeinifile = themefolder + "/theme.ini";
	SI_Error rc = ini->LoadFile(themeinifile.c_str());
	if(rc<0){
		delete ini;
	}
	ImVec4* colors = ImGui::GetStyle().Colors;
	for(int i=0;i<configstyles.size();i++){
		double redval = ini->GetDoubleValue(configstyles[i].configstring.c_str(), "Red");
		double greenval = ini->GetDoubleValue(configstyles[i].configstring.c_str(), "Green");
		double blueval = ini->GetDoubleValue(configstyles[i].configstring.c_str(), "Blue");
		double alphaval = ini->GetDoubleValue(configstyles[i].configstring.c_str(), "Alpha");
		
		colors[configstyles[i].colenum] = ImVec4((float)redval,(float)greenval,(float)blueval,(float)alphaval);
		
	}
}


void Themes::setTheme(std::string themefolder){
	
	CSimpleIniA *ini;
	ini = new CSimpleIniA(true,true);
	ini->SetUnicode();
	std::string themeinifile = themefolder + "/theme.ini";
	SI_Error rc = ini->LoadFile(themeinifile.c_str());
	if(rc<0){
		delete ini;
	}
	//const char* namepv = ini->GetValue("Theme", "Name");
	//const char* authorpv = ini->GetValue("Theme", "Author");
	
	double fontsize =  ini->GetDoubleValue("Theme","FontSize");
	//double smallfontsize =  ini->GetDoubleValue("Theme","FontSmallSize");
		

	const char* latinfontpv = ini->GetValue("Theme", "LatinFont");
	const char* kanjifontpv = ini->GetValue("Theme", "KanjiFont");
				
	std::string latinfontstr = themefolder + std::string("/") + latinfontpv;
	std::string kanjifontstr = themefolder + std::string("/") + kanjifontpv;
		
	dochangethemefont = true;
	themefontpath = themefolder+"/font.ttf";
	themefontsize = fontsize;
	
	
	//Utility::FontLoader(latinfontstr.c_str(),fontsize,kanjifontstr.c_str(),fontsize,ImGui::GetIO());
	
	if(imgloader != nullptr){
		delete imgloader;
	}
	imgloader = new CImgLoader(themefolder);
	
	
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

std::vector<nxmpgfx::fonttype_struct> Themes::getThemeFonts(int themeidx,bool onlylatinrange){
	std::vector<nxmpgfx::fonttype_struct> tmpfonts;
	if(themeidx == -1){
		
		nxmpgfx::fonttype_struct tmpentry;
		tmpentry.filename = "romfs:/DejaVuSans.ttf";

		ImFontGlyphRangesBuilder range;
		range.Clear();
		tmpentry.charrange.clear();
		range.AddRanges(ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
		range.BuildRanges(&tmpentry.charrange);
		tmpentry.size = currFontsize;
		tmpfonts.push_back(tmpentry);
		
		if(!onlylatinrange){
			tmpentry.filename = "romfs:/Source Han Sans CN Light.otf";
			tmpentry.size = currFontsize;
			
			range.Clear();
			range.AddRanges(ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());
			range.AddRanges(ImGui::GetIO().Fonts->GetGlyphRangesJapanese());
			tmpentry.charrange.clear();
			range.BuildRanges(&tmpentry.charrange);
			tmpfonts.push_back(tmpentry);
			
			
			tmpentry.filename = "romfs:/un-graphic.ttf";
			tmpentry.size = currFontsize;
			
			range.Clear();
			range.AddRanges(ImGui::GetIO().Fonts->GetGlyphRangesKorean());
			tmpentry.charrange.clear();
			range.BuildRanges(&tmpentry.charrange);
			tmpfonts.push_back(tmpentry);
		}
		
		
	} else {
		
		
		nxmpgfx::fonttype_struct tmpentry;
		tmpentry.filename = themeslist[themeidx].latinfontstr;
		if(isHandheld){
			tmpentry.size = themeslist[themeidx].handledfontsize;
		} else {
			tmpentry.size = themeslist[themeidx].dockedfontsize;
		}
		
		ImFontGlyphRangesBuilder range;
		range.Clear();
		tmpentry.charrange.clear();
		range.AddRanges(ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
		range.BuildRanges(&tmpentry.charrange);
		tmpfonts.push_back(tmpentry);
		
		if(!onlylatinrange){
		
			tmpentry.filename = themeslist[themeidx].kanjifontstr;
			if(isHandheld){
				tmpentry.size = themeslist[themeidx].handledfontsize;
			} else {
				tmpentry.size = themeslist[themeidx].dockedfontsize;
			}
			
			range.Clear();
			range.AddRanges(ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());
			range.AddRanges(ImGui::GetIO().Fonts->GetGlyphRangesJapanese());
			tmpentry.charrange.clear();
			range.BuildRanges(&tmpentry.charrange);
			tmpfonts.push_back(tmpentry);
			
			
			tmpentry.filename = "romfs:/un-graphic.ttf";
			if(isHandheld){
				tmpentry.size = themeslist[themeidx].handledfontsize;
			} else {
				tmpentry.size = themeslist[themeidx].dockedfontsize;
			}
			
			range.Clear();
			range.AddRanges(ImGui::GetIO().Fonts->GetGlyphRangesKorean());
			tmpentry.charrange.clear();
			range.BuildRanges(&tmpentry.charrange);
			tmpfonts.push_back(tmpentry);
		}
		
		
		
	}
	return tmpfonts;
}

