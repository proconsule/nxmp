#include "utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Utility{
	
	std::string formatTimeShort(double seconds) {

    int h((int) seconds / 3600);
    int min((int) seconds / 60 - h * 60);
    int sec((int) seconds - (h * 60 + min) * 60);

    std::ostringstream oss;
    if (h > 0) {
        oss << std::setfill('0') << std::setw(2) << h << ":";
    }
    if (min > 0) {
        oss << std::setfill('0') << std::setw(2) << min << ":";
    }
    oss << std::setfill('0') << std::setw(2) << sec;

    return oss.str();
	}
	
	urlschema parseUrl(std::string url){
	  urlschema res;
	  std::regex rgx("(?:([^:]*)://)?(?:([^:@]*)(?::([^@]*))?@)?(?:([^/:]*))?(?::([0-9]*))?/(/[^?#]*(=.*?/)/)?([^?#]*)?(?:([^#]*))?(?:#(.*))?");
      std::smatch match;
      if(std::regex_search(url, match, rgx)) {
         res.scheme = match[1];
		 res.user = match[2];
		 res.pass = match[3];
         res.server = match[4];
         res.port = match[5];
         res.path = match[8];
      }
	  return res;
	}
	
	std::string str_tolower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), 
                   [](unsigned char c){ return std::tolower(c); }
                  );
    return s;
}


	bool compare(const remotefs_entry &a, const remotefs_entry &b) {

		if (a.isDir && !b.isDir) {
			return true;
		}
		if (!a.isDir && b.isDir) {
			return false;
		}
		return str_tolower(a.name) < str_tolower(b.name);
	}
	
	bool TxtLoadJPGFromFile(std::string filename, GLuint* out_texture, int* out_width, int* out_height)
{
	
	FILE * file = fopen(filename.c_str(), "r");
	if (file == NULL) return false;
	fseek(file, 0, SEEK_END);
	long int size = ftell(file);
	fclose(file);
	// Reading data to array of unsigned chars
	file = fopen(filename.c_str(), "r");
	unsigned char * in = (unsigned char *) malloc(size);
	fread(in, sizeof(unsigned char), size, file);
	
	
	
    int image_width = 0;
    int image_height = 0;
    if (in == NULL){
        return false;
	}
		
	int jpegSubsamp;
	int _jpegSize = 0x20000;
	
	tjhandle _jpegDecompressor = tjInitDecompress();

	tjDecompressHeader2(_jpegDecompressor, in, _jpegSize, &image_width, &image_height, &jpegSubsamp);

	unsigned char buffer[image_width*image_height*3];

	tjDecompress2(_jpegDecompressor, in, _jpegSize, buffer, image_width, 0/*pitch*/, image_height, TJPF_RGB,  TJFLAG_FASTDCT);

	tjDestroy(_jpegDecompressor);

	GLuint id = 0;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
		
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    

    *out_texture = id;
    *out_width = image_width;
    *out_height = image_height;
	
	fclose(file);
	free(in);

    return true;
}

	bool TxtLoadPNGFromFile(std::string filename, GLuint* out_texture, int* out_width, int* out_height){
		int image_width = 0;
		int image_height = 0;
		unsigned char* image_data = stbi_load(filename.c_str(), &image_width, &image_height, NULL, 4);
		if (image_data == NULL)
			return false;
		
		GLuint id = 0;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		//glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    
		*out_texture = id;
		*out_width = image_width;
		*out_height = image_height;
		
		return true;
	}
	
}