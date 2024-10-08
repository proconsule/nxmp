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
	
	enum class FileMediaType {
		Media = 0,
		Image = 1,
		Archive = 2
	};
	
	enum FILESORTORDER{
		FS_NAME_ASCENDINGORDER = 0,
		FS_NAME_DESCENDINGORDER,
		FS_DATE_ASCENDINGORDER,
		FS_DATE_DESCENDINGORDER,
		FS_SIZE_ASCENDINGORDER,
		FS_SIZE_DESCENDINGORDER,
	};
	
	
	struct FileEntry{
		std::string name;
		std::string path;
		int64_t size = 0;
		FileEntryType type = FileEntryType::Unknown;
		FileMediaType mediatype = FileMediaType::Media;
		int is_valid=0;
		time_t modified;
		time_t accessed;
		time_t created;
		int dbread = -1;
		bool checked = false;
	};
	
	
	std::string FormatDate(time_t timestamp);
	
	
	std::string removeLastSlash(const std::string &string);
	FileType GetFileType(const std::string &filename);
	std::vector<FileEntry> getDirList(const std::string &path,bool showHidden,const std::vector<std::string> &extensions);
	std::string backPath(std::string path);
	bool Sort(const FileEntry &entryA, const FileEntry &entryB);
	//bool SortAsc(const FileEntry &entryA, const FileEntry &entryB);
	//bool SortDesc(const FileEntry &entryA, const FileEntry &entryB);
	
	bool SortNameAsc(const FileEntry &entryA, const FileEntry &entryB);
	bool SortNameDesc(const FileEntry &entryA, const FileEntry &entryB);
	
	bool SortDateAsc(const FileEntry &entryA, const FileEntry &entryB);
	bool SortDateDesc(const FileEntry &entryA, const FileEntry &entryB);
	
	bool SortSizeAsc(const FileEntry &entryA, const FileEntry &entryB);
	bool SortSizeDesc(const FileEntry &entryA, const FileEntry &entryB);
	
	std::string getFilefromPath(std::string path);
}

#endif