#ifndef NXMP_REMOTEFS_H
#define NXMP_REMOTEFS_H

struct remotefs_entry{
	std::string name;
	std::string path;
	bool isDir=false;
	uint64_t size;
};


#endif
