#include "shaderMania.h"


shaderMania::shaderMania(){
	
	std::vector<std::string> shadersext = {"glsl","hook"};
	DirList("./mpv/shaders",true,shadersext);
	
}


std::vector<FS::FileEntry> shaderMania::getCurrList(){
	return currentlist;
}

void shaderMania::DirList(const std::string &path,bool showHidden,const std::vector<std::string> &extensions) {

		currentlist.clear();
		struct dirent *ent;
		DIR *dir;

		if (!path.empty()) {
			if ((dir = opendir(path.c_str())) != nullptr) {
				while ((ent = readdir(dir)) != nullptr) {
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

					FS::FileEntry file;
					file.name = ent->d_name;
					file.path = FS::removeLastSlash(path) + "/" + file.name;

					struct stat st{};
					if (stat(file.path.c_str(), &st) == 0) {
						file.size = (size_t) st.st_size;
						file.type = S_ISDIR(st.st_mode) ? FS::FileEntryType::Directory : FS::FileEntryType::File;
					}
					if(file.type == FS::FileEntryType::File){
						bool isMediafile = false;
						for (auto &ext : extensions) {
							if (Utility::endsWith(file.name, ext, false)) {
								isMediafile = true;
							}
						}
						if(isMediafile){
							currentlist.push_back(file);
						}
					}else if(file.type == FS::FileEntryType::Directory){
						currentlist.push_back(file);
					}

				}
			
				closedir(dir);
				std::sort(currentlist.begin(), currentlist.end(), FS::Sort);
				FS::FileEntry dummyfile;
				dummyfile.name = "None";
				dummyfile.path = "";
				currentlist.insert(currentlist.begin(), dummyfile);
			}
		}

	}