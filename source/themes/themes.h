#ifndef NXMP_THEMES_H
#define NXMP_THEMES_H

#include "gui.h"


typedef struct {
	std::string name;
	std::string author;
	std::string path;
} themes_struct;

class Themes{
public:
	Themes();
	
	void setTheme(std::string themefolder);
	void setDefault();
	
	void getThemes();
	
	int getThemeIDX(std::string themename);
	
	std::vector<themes_struct> themeslist;
	
	
};


#endif