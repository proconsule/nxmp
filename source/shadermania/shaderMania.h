#ifndef NXMP_SHADERMANIA_H
#define NXMP_SHADERMANIA_H

#include "localfiles.h"



class shaderMania{
public:
	shaderMania();
	~shaderMania();
	
	std::vector<FS::FileEntry> getCurrList();
	void DirList(const std::string &path,bool showHidden,const std::vector<std::string> &extensions);
	
private:
    std::vector<FS::FileEntry> currentlist;
	
};


#endif