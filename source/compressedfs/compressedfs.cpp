#include "compressedfs.h"
#include "zipDir.h"
#include "utils.h"
#include "usbfs.h"



compressedFS::compressedFS(unsigned char * _archive_data,int _archive_size){
	archive_data = _archive_data;
	archive_size = _archive_size;
	zipFs = new zipDir(archive_data,archive_size);

}

void compressedFS::ReadList(){
	zipFs->ReadList();
}

void compressedFS::DirList(std::string path,bool showHidden,const std::vector<std::string> &extensions){
	if(zipFs != nullptr){
		zipFs->DirList(path,showHidden,extensions);
	}
}

std::string compressedFS::backDir(){
	return zipFs->backDir();
}
	
std::string compressedFS::getCurrentPath(){
	return zipFs->getCurrentPath();
}

std::vector<FS::FileEntry> compressedFS::getCurrList(){
	return zipFs->getCurrList();
}

std::vector<FS::FileEntry> compressedFS::getCurrImageList(){
	return zipFs->getCurrImageList();
}

bool compressedFS::getfileContents(std::string filepath,unsigned char ** _filedata,int &_size){
	return zipFs->getfileContents(filepath,_filedata,_size);
}