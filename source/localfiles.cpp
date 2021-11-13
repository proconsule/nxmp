#include <algorithm>
#include <cstdio>
#include <cstring>
#include <filesystem>

#include "localfiles.h"

namespace FS {
	std::string removeLastSlash(const std::string &string) {

    std::string str = string;
    size_t pos = str.find_last_of('/');
    if (pos == str.length() - 1) {
        str.erase(str.length() - 1);
    }

    return str;
	}
	
	bool Sort(const FileEntry &entryA, const FileEntry &entryB) {
		if ((entryA.type == FileEntryType::Directory) && !(entryB.type == FileEntryType::Directory))
			return true;
		else if (!(entryA.type == FileEntryType::Directory) && (entryB.type == FileEntryType::Directory))
			return false;
		else {
			if (strcasecmp(entryA.name.c_str(), entryB.name.c_str()) < 0)
				return true;
					
		}
		
		return false;
	}
	
	std::vector<FileEntry> getDirList(const std::string &path,bool showHidden,const std::vector<std::string> &extensions) {

		std::vector<FileEntry> files;
		struct dirent *ent;
		DIR *dir;

		if (!path.empty()) {
			if ((dir = opendir(path.c_str())) != nullptr) {
				while ((ent = readdir(dir)) != nullptr) {

					fflush(stdout);
					if ((path == "/" || strlen(ent->d_name) == 1) && ent->d_name[0] == '.') {
						continue;
					}
					if ((path == "/" || strlen(ent->d_name) == 2) && ent->d_name[0] == '.' && ent->d_name[1] == '.') {
						continue;
					}
					if (!showHidden && ent->d_name[0] == '.') {
						if (strlen(ent->d_name) != 2 && ent->d_name[1] != '.') {
							continue;
						}
					}

					FileEntry file;
					file.name = ent->d_name;
					file.path = removeLastSlash(path) + "/" + file.name;
#if 0
					auto *dirSt = (fsdev_dir_t *) dir->dirData->dirStruct;
					FsDirectoryEntry *entry = &dirSt->entry_data[dirSt->index];
					file.type = entry->type == ENTRYTYPE_DIR ? FileEntryType::Directory : FileEntryType::File;
					file.size = entry->fileSize;
#else
					struct stat st{};
					if (stat(file.path.c_str(), &st) == 0) {
						file.size = (size_t) st.st_size;
						file.type = S_ISDIR(st.st_mode) ? FileEntryType::Directory : FileEntryType::File;
					}
#endif
					files.push_back(file);
				}
			
				closedir(dir);
				std::sort(files.begin(), files.end(), FS::Sort);
				
				files.erase(
					std::remove_if(files.begin(), files.end(), [extensions](const FileEntry &file) {
                    for (auto &ext : extensions) {
                        if (Utility::endsWith(file.name, ext, false)) {
                            return false;
                        }
                    }
                    return file.type == FileEntryType::File;
                }), files.end());
				
			}
		}

		return files;
	}
	
	std::string backPath(std::string path){
		std::string retpath = path.substr(0, path.find_last_of("\\/"));
		if(retpath == "")retpath = "/";
		return retpath;
	}
	
	std::string getFilefromPath(std::string path){
		std::string retpath = path.substr(path.find_last_of("\\/")+1);
		return retpath;
	}
	
}