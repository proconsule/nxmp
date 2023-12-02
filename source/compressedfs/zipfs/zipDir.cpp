#include "zipDir.h"

#include "localFs.h"
#include "sambaDir.h"
#include "sshDir.h"
#include "FTPDir.h"
#include "HTTPDir.h"
#include "nfsDir.h"
#include "usbfs.h"


zipDir::zipDir(unsigned char * _archive_data,int _archive_size){
	archive_data = _archive_data;
	archive_size = _archive_size;
}


void zipDir::ReadList(){
	
	
	
	
	struct archive *a = archive_read_new();
	struct archive_entry *entry;
	
	archive_read_support_filter_all(a);
	archive_read_support_format_all(a);
	
	int flags = 0;
	int res = archive_read_open_memory(a, archive_data, archive_size);
	
	
	res = archive_read_next_header(a, &(entry));
	
	while(res == ARCHIVE_OK) {
		
		archfileentry_struct archentry;
		FS::FileEntry file;
		
		std::string fullpathname = archive_entry_pathname(entry);
		size_t pos = fullpathname.find_last_of("/");
		if(pos == fullpathname.length()-1)
		{
			fullpathname.erase(fullpathname.length()-1);
		}
		
		
		std::string myfilename = fullpathname.substr(fullpathname.find_last_of("/")+1);
		std::string parentname = fullpathname.substr(0,fullpathname.find_last_of("/"));
		
		file.name = myfilename;
		const struct stat *st;
		st = archive_entry_stat(entry);
		
		file.path = fullpathname;
		
		file.size = (size_t) st->st_size;
		file.type = S_ISDIR(st->st_mode) ? FS::FileEntryType::Directory : FS::FileEntryType::File;
		file.is_valid = 1;
		file.created = (time_t)st->st_ctime;
		file.modified = (time_t)st->st_mtime;
		file.accessed = (time_t)st->st_atime;
		
		
		archentry.parent = parentname;
		archentry.fileentry = file;
			
		
		//printf("%s %s\n",myfilename.c_str(),parentname.c_str());
		//printf("File %s %ld %ld\n",file.name.c_str(),file.size,file.modified);
		
		res = archive_read_next_header(a, &(entry));

		totalfilelist.push_back(archentry);


	}
	
	archive_read_finish(a);
	
}

void zipDir::DirList(std::string path,bool showHidden,const std::vector<std::string> &extensions){
	
	currentpath = path;
	currentlist.clear();
	currentimagelist.clear();
	
	for(int i=0;i<totalfilelist.size();i++){
		if(path == "" || path == "/"){
			
			if(totalfilelist[i].parent == totalfilelist[i].fileentry.name){
				if(totalfilelist[i].fileentry.type == FS::FileEntryType::File){
					bool isMediafile = false;
						for (auto &ext : extensions) {
							if (Utility::endsWith(totalfilelist[i].fileentry.name, ext, false)) {
								isMediafile = true;
							}
						}
						if(isMediafile){
							currentlist.push_back(totalfilelist[i].fileentry);
							if(Utility::isImageExtension(totalfilelist[i].fileentry.name)){
								currentimagelist.push_back(totalfilelist[i].fileentry);
							}
						}
					}else if(totalfilelist[i].fileentry.type == FS::FileEntryType::Directory){
						currentlist.push_back(totalfilelist[i].fileentry);
				}
			}
			
		}else{
			if(path == totalfilelist[i].parent){
				if(totalfilelist[i].fileentry.type == FS::FileEntryType::File){
					bool isMediafile = false;
						for (auto &ext : extensions) {
							if (Utility::endsWith(totalfilelist[i].fileentry.name, ext, false)) {
								isMediafile = true;
							}
						}
						if(isMediafile){
							currentlist.push_back(totalfilelist[i].fileentry);
							if(Utility::isImageExtension(totalfilelist[i].fileentry.name)){
								currentimagelist.push_back(totalfilelist[i].fileentry);
							}
						}
					}else if(totalfilelist[i].fileentry.type == FS::FileEntryType::Directory){
						currentlist.push_back(totalfilelist[i].fileentry);
				}
			}
		}
	}
	
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
	
	for(int i=0;i<currentlist.size();i++){
		printf("%s %d\n",currentlist[i].name.c_str(),currentlist[i].modified);
	}
}

std::string zipDir::getCurrentPath(){
	return currentpath;
}

std::vector<FS::FileEntry> zipDir::getCurrList(){
	return currentlist;
}

std::vector<FS::FileEntry> zipDir::getCurrImageList(){
	return currentimagelist;
}

std::string zipDir::backDir(){
	if(currentpath.find_last_of("/") == 0)currentpath = basepath;
	if(currentpath.find_last_of("/") == -1)currentpath = basepath;
	if(currentpath == basepath)return "";
	currentpath = FS::removeLastSlash(currentpath);
	std::string retpath = currentpath.substr(currentpath.find_last_of("\\/")+1);
	currentpath = currentpath.substr(0, currentpath.find_last_of("/"));
	return retpath;
}


bool zipDir::getfileContents(std::string filepath,unsigned char ** _filedata,int &_size){
	
	struct archive *a = archive_read_new();
	struct archive_entry *entry;
	
	archive_read_support_filter_all(a);
	archive_read_support_format_all(a);
	
	int flags = 0;
	int res = archive_read_open_memory(a, archive_data, archive_size);
	
	bool retget = false;
	res = archive_read_next_header(a, &(entry));
	
	while(res == ARCHIVE_OK) {
		
		
		const struct stat *st;
		st = archive_entry_stat(entry);
		if(std::string(archive_entry_pathname(entry)) == filepath){
			_size = st->st_size;
			*_filedata = (unsigned char*)malloc(_size); 
			archive_read_data(a, *_filedata, st->st_size);
			retget = true;
		}
		
		res = archive_read_next_header(a, &(entry));
	}
	
	return retget;
}