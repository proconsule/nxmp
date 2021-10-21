#ifndef NXMP_UTILITY_H
#define NXMP_UTILITY_H

#include <string>
#include <iostream>    
#include <sstream>    
#include <iomanip>
#include <regex>
#include <switch.h>
#include <remotefs.h>

#include <glad/glad.h>
#include <turbojpeg.h>
#include <png.h>

typedef struct {
    GLuint id = 0;
    int width = 0;
    int height = 0;
} Tex;

struct urlschema{
	std::string scheme;
	std::string server;
	std::string user;
	std::string pass;
	std::string port;
	std::string path;
};


namespace Utility{
	
	
	std::string formatTimeShort(double seconds);
	urlschema parseUrl(std::string url); 
	bool compare(const remotefs_entry &a, const remotefs_entry &b);
	bool TxtLoadJPGFromFile(std::string filename, GLuint* out_texture, int* out_width, int* out_height);
	bool TxtLoadPNGFromFile(std::string filename, GLuint* out_texture, int* out_width, int* out_height);
}

#endif