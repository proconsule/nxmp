#include "localFs.h"
#include <stdio.h>

localFs::localFs(std::string _path,Playlist * _playlist){
	currentpath = _path;
	playlist = _playlist;
}

localFs::~localFs(){
	
}

std::string localFs::getCurrentPath(){
	return currentpath;
}

std::vector<FS::FileEntry> localFs::getCurrList(){
	return currentlist;
}

std::vector<FS::FileEntry> localFs::getCurrImageList(){
	return currentimagelist;
}

void localFs::clearChecked(){
	for(int i=0;i<currentlist.size();i++){
		currentlist[i].checked = false;
	}
}

bool *localFs::checked(int pos){
	return &currentlist[pos].checked;
}

void localFs::DirList(const std::string &path,bool showHidden,const std::vector<std::string> &extensions) {

		currentpath = path;
		currentlist.clear();
		currentimagelist.clear();
		struct dirent *ent;
		DIR *dir;

		if (!path.empty()) {
			if ((dir = opendir(path.c_str())) != nullptr) {
				FsFileSystem sdmc;
				fsOpenSdCardFileSystem(&sdmc);
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
					file.dbread = -1;
					file.path = FS::removeLastSlash(path) + "/" + file.name;
					file.checked = playlist->isPresent(file,file.path);
					
					FsTimeStampRaw timestamp = {0};
					char safe_buf[FS_MAX_PATH];
					strcpy(safe_buf, file.path.c_str());
					fsFsGetFileTimeStampRaw(&sdmc, safe_buf, &timestamp);
					
					
					
					struct stat st{0};
					if (stat(file.path.c_str(), &st) == 0) {
						file.size = (size_t) st.st_size;
						file.type = S_ISDIR(st.st_mode) ? FS::FileEntryType::Directory : FS::FileEntryType::File;
						file.is_valid = 1;
						file.created = timestamp.created;
						file.modified = timestamp.modified;
						file.accessed = timestamp.accessed;
					}
					
					
					if(file.type == FS::FileEntryType::File){
						bool isMediafile = false;
						for (auto &ext : extensions) {
							if (Utility::endsWith(file.name, ext, false)) {
								isMediafile = true;
							}
						}
						if(isMediafile){
							if(Utility::isImageExtension(file.name)){
								file.mediatype = FS::FileMediaType::Image;
								currentimagelist.push_back(file);
							}
							if(Utility::isArchiveExtension(file.name)){
								file.mediatype = FS::FileMediaType::Archive;
							}
							currentlist.push_back(file);
						}
					}else if(file.type == FS::FileEntryType::Directory){
						currentlist.push_back(file);
					}

				}
				fsFsClose(&sdmc);
				closedir(dir);
				if(sortOrder == FS::FS_NAME_ASCENDINGORDER){
					std::sort(currentlist.begin(), currentlist.end(), FS::SortNameAsc);
				}
				if(sortOrder == FS::FS_NAME_DESCENDINGORDER){
					std::sort(currentlist.begin(), currentlist.end(), FS::SortNameDesc);
				}
				
				if(sortOrder == FS::FS_DATE_ASCENDINGORDER){
					std::sort(currentlist.begin(), currentlist.end(), FS::SortDateAsc);
				}
				if(sortOrder == FS::FS_DATE_DESCENDINGORDER){
					std::sort(currentlist.begin(), currentlist.end(), FS::SortDateDesc);
				}
				
				if(sortOrder == FS::FS_SIZE_ASCENDINGORDER){
					std::sort(currentlist.begin(), currentlist.end(), FS::SortSizeAsc);
				}
				if(sortOrder == FS::FS_SIZE_DESCENDINGORDER){
					std::sort(currentlist.begin(), currentlist.end(), FS::SortSizeDesc);
				}
				
			}
		}

	}
	
	std::string localFs::backPath(){
		std::string retpath = currentpath.substr(currentpath.find_last_of("\\/")+1);
		currentpath = currentpath.substr(0, currentpath.find_last_of("\\/"));
		if(currentpath == "")currentpath = "/";
		return retpath;
	}
	

	void localFs::SetFileDbStatus(int idx,int dbstatus){
		currentlist[idx].dbread = dbstatus;
	}
	
	void localFs::ResetDbStatus(){
		for(int i=0;i<currentlist.size();i++){
			currentlist[i].dbread = -1;
		}
	}
	
	
	bool localFs::getfileContents(std::string filepath,unsigned char ** _filedata,int &_size){
		FILE * infile = fopen(filepath.c_str(), "rb");
		if(infile == NULL)return false;
		fseek(infile, 0L, SEEK_END);
		_size = ftell(infile);
		fseek(infile, 0L, SEEK_SET);
		*_filedata = (unsigned char*)malloc(_size); 
		
		size_t bytesRead = 0;
		size_t offset = 0;
		char buffer[4096];
		
		while ((bytesRead = fread(buffer, 1, sizeof(buffer), infile)) > 0) {
			memcpy(*_filedata+offset,buffer,bytesRead);
			offset+=bytesRead;
		}

		fclose(infile);
		return true;
	}
	
	
	bool localFs::OpenFile(std::string filepath){
		open_file = fopen(filepath.c_str(), "rb");
		if(open_file == NULL)return false;
		
	}
	int localFs::FileRead(unsigned char * data,unsigned int count,unsigned int offset){
		if(open_file!=NULL){
			fseek(open_file, offset, SEEK_SET);
			return fread(data, sizeof(unsigned char), count, open_file);
		}
		
		return -1;
	}
	
	void localFs::FileClose(){
		if(open_file != NULL)fclose(open_file);
	}