#include <algorithm>
#include <cstdio>
#include <cstring>
#include <filesystem>

#include "localfiles.h"

FsFileSystem *fs;
FsFileSystem devices[4];

namespace FS {
	
	std::string GetFileExt(const std::string &filename) {
		std::string ext = std::filesystem::path(filename).extension();
		std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);
		return ext;
	}
	
	static bool Sort(const FsDirectoryEntry &entryA, const FsDirectoryEntry &entryB) {
		if ((entryA.type == FsDirEntryType_Dir) && !(entryB.type == FsDirEntryType_Dir))
			return true;
		else if (!(entryA.type == FsDirEntryType_Dir) && (entryB.type == FsDirEntryType_Dir))
			return false;
		else {
			if (strcasecmp(entryA.name, entryB.name) < 0)
				return true;
					
		}
		
		return false;
	}
	
	FileType GetFileType(const std::string &filename) {
		std::string ext = FS::GetFileExt(filename);
		
		if ((!ext.compare(".MP4")) || (!ext.compare(".MKV")) || (!ext.compare(".TS")) || (!ext.compare(".MOV")) || (!ext.compare(".MPEG")) || (!ext.compare(".MPG")) || (!ext.compare(".FLV")))
			return FileTypeVideo;
		
		return FileTypeNone;
	}
	
	Result GetDirList(const char *path, std::vector<FsDirectoryEntry> &entries) {
		FsDir dir;
		Result ret = 0;
		
		if (R_FAILED(ret = fsFsOpenDirectory(fs, path, FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles, &dir))) {
			printf("fsFsOpenDirectory(%s) failed: 0x%x\n", path, ret);
			return ret;
		}
		s64 entry_count = 0;
		if (R_FAILED(ret = fsDirGetEntryCount(&dir, &entry_count))) {
			printf("fsDirGetEntryCount(%s) failed: 0x%x\n", path, ret);
			return ret;
		}
		entries.resize(entry_count);
		if (R_FAILED(ret = fsDirRead(&dir, nullptr, static_cast<size_t>(entry_count), entries.data()))) {
			printf("fsDirRead(%s) failed: 0x%x\n", path, ret);
			return ret;
		}

		std::sort(entries.begin(), entries.end(), FS::Sort);
		fsDirClose(&dir);
		return 0;
	}
	
}