#ifndef NXMP_LOCALFILES_H
#define NXMP_LOCALFILES_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <string>
#include <vector>
#include "utils.h"
/*
#include <switch.h>
*/

typedef enum FileType {
    FileTypeNone,
    FileTypeVideo,
    FileTypePlaylist
} FileType;

namespace FS {
	
	enum class FileEntryType {
		Unknown = 0,
		File = 1,
		Directory = 2
	};
	
	struct FileEntry{
		std::string name;
		std::string path;
		int64_t size = 0;
		FileEntryType type = FileEntryType::Unknown;
		bool checked = false;
	};
	
	std::string removeLastSlash(const std::string &string);
	FileType GetFileType(const std::string &filename);
	std::vector<FileEntry> getDirList(const std::string &path,bool showHidden,const std::vector<std::string> &extensions);
	std::string backPath(std::string path);
	bool Sort(const FileEntry &entryA, const FileEntry &entryB);
	std::string getFilefromPath(std::string path);
}

#endif