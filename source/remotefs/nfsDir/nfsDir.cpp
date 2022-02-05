#include "nfsDir.h"

#ifdef _WIN32
#include <winsock2.h>
#endif

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
#ifdef _WIN32
	WSADATA wsadata;
	int err;
 
	err = WSAStartup(MAKEWORD(2, 0), &wsadata);
	if(err != 0) {
		printf("WSAStartup failed with error: %d\n", err);

    }else{
		printf("WSAStartup ok\n");
	}
#endif	

	url = _url;	
	urlschema thisurl = Utility::parseUrl(url);
	basepath = std::string("/") + thisurl.path;
	size_t pos = basepath.find_last_of("/");
	if(pos == basepath.length()-1)
	{
		basepath.erase(basepath.length()-1);
	}
	printf("Base Path = %s\n",basepath.c_str());
	currentpath = basepath;
	playlist = _playlist;
	
}

void nfsDir::DirList(std::string _path,bool showHidden,const std::vector<std::string> &extensions){
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
		printf("failed to init context\n");
		goto finished;
	}
	
	_url = nfs_parse_url_dir(nfs, url.c_str());
	if (_url == NULL) {
		printf("%s\n", nfs_get_error(nfs));
		goto finished;
	}
	
	printf("DIR PATH %s\n",_path.c_str());
	client.nfsserver = _url->server;
	client.nfsexport = (char *)_path.c_str();
	client.is_finished = 0;
	
	
	
	if ((ret = nfs_mount(nfs, client.nfsserver, client.nfsexport)) != 0) {
 		printf("Failed to mount nfs share : %s\n", nfs_get_error(nfs));
		goto finished;
	}
	
	

	ret = nfs_opendir(nfs, "", &nfsdir);
	if (ret != 0) {
		printf("Failed to opendir(\"%s\") %s\n", "", nfs_get_error(nfs));
		goto finished;
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
		currentlist.push_back(tmpentry);
		
	}
	nfs_closedir(nfs, nfsdir);
	std::sort(currentlist.begin(), currentlist.end(), FS::Sort);
				
	currentlist.erase(
		std::remove_if(currentlist.begin(), currentlist.end(), [extensions](const FS::FileEntry &file) {
			for (auto &ext : extensions) {
				if (Utility::endsWith(file.name, ext, false)) {
					return false;
				}
			}
			return file.type == FS::FileEntryType::File;
	}), currentlist.end());
	

finished:
	
	if (nfs != NULL) {
		nfs_destroy_context(nfs);
	}
	
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