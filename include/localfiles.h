#ifndef NXMP_LOCALFILES_H
#define NXMP_LOCALFILES_H

#include <string>
#include <vector>
#include <switch.h>

extern FsFileSystem *fs;
extern FsFileSystem devices[4];

typedef enum FileType {
    FileTypeNone,
    FileTypeVideo,
    FileTypePlaylist
} FileType;

namespace FS {
	Result GetDirList(const char *path, std::vector<FsDirectoryEntry> &entries);
	FileType GetFileType(const std::string &filename);
}

#endif