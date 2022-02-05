#include "FTPDir.h"

#ifdef NXMP_NETWORKSUPPORT

FTPDir::FTPDir(std::string _url,Playlist *_playlist){
	url = _url;	
	urlschema thisurl = Utility::parseUrl(url);
	basepath = thisurl.path;
	currpath = basepath;
	playlist = _playlist;
}

FTPDir::~FTPDir(){
	
}

void FTPDir::DirList(std::string path,const std::vector<std::string> &extensions){
	currpath = path;
	FtpInit();
	urlschema thisurl = Utility::parseUrl(url);
	currentlist.clear();
	if(thisurl.port == "")thisurl.port = "21";
	std::string ftphost = thisurl.server+std::string(":")+thisurl.port;
	if (!FtpConnect(ftphost.c_str(), &ftp_con)) {
		printf("could not connect to ftp server\n");
	}else{
									
		if (!FtpLogin(thisurl.user.c_str(), thisurl.pass.c_str(), ftp_con)) {
			printf("could not connect to ftp server\n");
			FtpQuit(ftp_con);
		}else{
			currpath = path;
			currentlist = FtpDirList(path.c_str(), ftp_con,extensions);
			for(int i=0;i<currentlist.size();i++){
				std::string openurl = thisurl.scheme + std::string("://") + thisurl.user + std::string(":") + thisurl.pass + std::string("@") + thisurl.server + std::string("/") + currentlist[i].path + currentlist[i].name;
				currentlist[i].checked = playlist->isPresent(currentlist[i].name,currentlist[i].path);					
			}
			FtpQuit(ftp_con);
			
		}
	}
}

std::string FTPDir::getUrl(){
	return url;
}

std::string FTPDir::getCurrPath(){
	return currpath;
}

std::string FTPDir::getBasePath(){
	return basepath;
}

std::vector<FS::FileEntry> FTPDir::getCurrList(){
	return currentlist;
}

void FTPDir::clearChecked(){
	for(int i=0;i<currentlist.size();i++){
		currentlist[i].checked = false;
	}
}

bool *FTPDir::checked(int pos){
	return &currentlist[pos].checked;
}

void FTPDir::backDir(){
	if(currpath.find_last_of("/") == 0)currpath = basepath;
	if(currpath.find_last_of("/") == -1)currpath = basepath;
	if(currpath == basepath)return;
	currpath = FS::removeLastSlash(currpath);
	currpath = currpath.substr(0, currpath.find_last_of("/"));
}

#endif