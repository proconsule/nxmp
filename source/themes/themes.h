#ifndef NXMP_THEMES_H
#define NXMP_THEMES_H

#include "gui.h"


typedef struct {
	std::string name;
	std::string author;
	std::string path;
	float handledfontsize;
	float dockedfontsize;
	std::string latinfontstr;
	std::string kanjifontstr;
	ImVec4* colors;
} themes_struct;

class Themes{
public:
	Themes();
	
	void setTheme(std::string themefolder);
	void setDefault();
	
	void getThemes();
	
	int getThemeIDX(std::string themename);
	void setThemeColor(std::string themefolder);
	
	std::vector<themes_struct> themeslist;
	
	std::vector<nxmpgfx::fonttype_struct> getThemeFonts(int themeidx,bool onlylatinrange);
	
	
	
};


#endif