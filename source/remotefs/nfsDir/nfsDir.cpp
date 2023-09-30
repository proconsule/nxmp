#include "nfsDir.h"
#include <fcntl.h>
#include <nfsc/libnfs.h>
#include <nfsc/libnfs-raw.h>
#include <nfsc/libnfs-raw-mount.h>

struct client {
       char *nfsserver;
       char *nfsexport;
       uint32_t mount_port;
       int is_finished;
};

nfsDir::nfsDir(std::string _url,Playlist * _playlist){

	url = _url;	
	urlschema thisurl = Utility::parseUrl(url);
	basepath = std::string("/") + thisurl.path;
	size_t pos = basepath.find_last_of("/");
	if(pos == basepath.length()-1)
	{
		basepath.erase(basepath.length()-1);
	}
	NXLOG::DEBUGLOG("Base Path = %s\n",basepath.c_str());
	currentpath = basepath;
	playlist = _playlist;
	
}

bool nfsDir::DirList(std::string _path,bool showHidden,const std::vector<std::string> &extensions){
	struct nfs_context *nfs = NULL;
	struct client client;
	struct nfs_url *_url = NULL;
	int ret;
	urlschema thisurl = Utility::parseUrl(url);
	currentpath = _path;
	struct nfsdirent *nfsdirent;
	struct nfsdir *nfsdir;

	
	
	nfs = nfs_init_context();
	if (nfs == NULL) {
		NXLOG::ERRORLOG("failed to init context\n");
		errormsg = "failed to init context";
		return false;
	}
	
	_url = nfs_parse_url_dir(nfs, url.c_str());
	if (_url == NULL) {
		NXLOG::ERRORLOG("%s\n", nfs_get_error(nfs));
		errormsg = nfs_get_error(nfs);
		if (nfs != NULL) {
			nfs_destroy_context(nfs);
		}
		return false;
	}
	
	NXLOG::DEBUGLOG("DIR PATH %s\n",_path.c_str());
	client.nfsserver = _url->server;
	client.nfsexport = (char *)_path.c_str();
	client.is_finished = 0;
	
	
	
	if ((ret = nfs_mount(nfs, client.nfsserver, client.nfsexport)) != 0) {
 		NXLOG::ERRORLOG("Failed to mount nfs share : %s\n", nfs_get_error(nfs));
		errormsg = std::string("Failed to mount nfs share : ") + nfs_get_error(nfs);
		if (nfs != NULL) {
			nfs_destroy_context(nfs);
		}
		return false;
	}
	
	

	ret = nfs_opendir(nfs, "", &nfsdir);
	if (ret != 0) {
		NXLOG::ERRORLOG("Failed to opendir(\"%s\") %s\n", "", nfs_get_error(nfs));
		errormsg = std::string("Failed to opendir : ") + nfs_get_error(nfs);
		if (nfs != NULL) {
			nfs_destroy_context(nfs);
		}
		return false;
	}
	currentlist.clear();
	while((nfsdirent = nfs_readdir(nfs, nfsdir)) != NULL) {
		char path[1024];
		FS::FileEntry tmpentry;
		
		if(!showHidden){
			if (nfsdirent->name[0] == '.') {
				continue;
			}
		}
		if (!strcmp(nfsdirent->name, ".") || !strcmp(nfsdirent->name, "..")) {
			continue;
		}
		snprintf(path, 1024, "%s/%s", "", nfsdirent->name);

		switch (nfsdirent->mode & S_IFMT) {
		
		case S_IFREG:
			tmpentry.type = FS::FileEntryType::File;
			break;
		case S_IFDIR:
			tmpentry.type = FS::FileEntryType::Directory;
			break;
		case S_IFCHR:
			break;
		case S_IFBLK:
			break;
		}
		
		
		
		tmpentry.name = nfsdirent->name;
		tmpentry.path = _path + std::string("/") + std::string(nfsdirent->name);
		tmpentry.size = nfsdirent->size;
		tmpentry.is_valid = 1;
		tmpentry.dbread = -1;
		tmpentry.modified = nfsdirent->mtime.tv_sec;
		tmpentry.accessed = nfsdirent->atime.tv_sec;
		tmpentry.created = nfsdirent->ctime.tv_sec;
		
		currentlist.push_back(tmpentry);
		
	}
	nfs_closedir(nfs, nfsdir);
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
	currentlist.erase(
		std::remove_if(currentlist.begin(), currentlist.end(), [extensions](const FS::FileEntry &file) {
			for (auto &ext : extensions) {
				if (Utility::endsWith(file.name, ext, false)) {
					return false;
				}
			}
			return file.type == FS::FileEntryType::File;
	}), currentlist.end());
	


	
	if (nfs != NULL) {
		nfs_destroy_context(nfs);
	}
	return true;
}


std::vector<FS::FileEntry> nfsDir::getCurrList(){
	return currentlist;
}

bool *nfsDir::checked(int pos){
	return &currentlist[pos].checked;
}

std::string nfsDir::getUrl(){
	return url;
}

std::string nfsDir::getCurrPath(){
	return currentpath;
}

std::string nfsDir::getBasePath(){
	return basepath;
}

void nfsDir::clearChecked(){
	for(int i=0;i<currentlist.size();i++){
		currentlist[i].checked = false;
	}
}



void nfsDir::backDir(){
	if(currentpath.find_last_of("/") == 0)currentpath = basepath;
	if(currentpath.find_last_of("/") == -1)currentpath = basepath;
	if(currentpath == basepath)return;
	currentpath = FS::removeLastSlash(currentpath);
	currentpath = currentpath.substr(0, currentpath.find_last_of("/"));
}

void nfsDir::SetFileDbStatus(int idx,int dbstatus){
		currentlist[idx].dbread = dbstatus;
	}
	
	void nfsDir::ResetDbStatus(){
		for(int i=0;i<currentlist.size();i++){
			currentlist[i].dbread = -1;
		}
	}