#include "FTPDir.h"

FTPDir::FTPDir(std::string _url){
	url = _url;	
	urlschema thisurl = Utility::parseUrl(url);
	basepath = thisurl.path;
}

FTPDir::~FTPDir(){
	
}

std::vector<FS::FileEntry> FTPDir::getDir(std::string path,const std::vector<std::string> &extensions){
	FtpInit();
	urlschema thisurl = Utility::parseUrl(url);
	std::vector<FS::FileEntry> retvector;
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
			retvector = FtpDirList(path.c_str(), ftp_con,extensions);
			FtpQuit(ftp_con);
			
		}
	}
	return retvector;
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

std::string FTPDir::backDir(){
	if(currpath.find_last_of("/") == 0)return basepath;
	if(currpath.find_last_of("/") == -1)return basepath;
	if(currpath == basepath)return basepath;
	currpath = FS::removeLastSlash(currpath);
	currpath = currpath.substr(0, currpath.find_last_of("/"));
	return currpath;
}