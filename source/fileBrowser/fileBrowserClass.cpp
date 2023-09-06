#include "fileBrowserClass.h"

	CFileBrowser::CFileBrowser(std::string _path,Playlist * _playlist){
		path = _path;
		if(Utility::startWith(path,"/",false)){
			mylocal = new localFs(path,_playlist);
		}else if(Utility::startWith(path,"smb",false)){
			mysamba = new sambaDir(path,_playlist);
		}else if(Utility::startWith(path,"sftp",false)){
			myssh = new sshDir(path,_playlist);
		}else if(Utility::startWith(path,"ftp",false)){
			myftp = new FTPDir(path,_playlist);
		}else if(Utility::startWith(path,"http",false)){
			myhttp = new HTTPDir(path);
		}else if(Utility::startWith(path,"nfs",false)){
			mynfs = new nfsDir(path,_playlist);
		}
	}
	
	CFileBrowser::~CFileBrowser(){
		if(mylocal!= nullptr){
			delete mylocal;
		}
		if(mysamba!= nullptr){
			delete mysamba;
		}
		if(myssh!= nullptr){
			delete myssh;
		}
		if(myftp!= nullptr){
			delete myftp;
		}
		if(myhttp!= nullptr){
			delete myhttp;
		}
		if(mynfs!= nullptr){
			delete mynfs;
		}
	}
	
	void CFileBrowser::DirList(std::string path,bool showHidden,const std::vector<std::string> &extensions){
		if(mylocal!= nullptr){
			mylocal->DirList(path,showHidden,extensions);
		}
		if(mysamba!= nullptr){
			mysamba->DirList(path,showHidden,extensions);
		}
		if(myssh!= nullptr){
			myssh->DirList(path,showHidden,extensions);
		}
		if(myftp!= nullptr){
			myftp->DirList(path,extensions);
		}
		if(myhttp!= nullptr){
			myhttp->DirList(path,extensions);
		}
		if(mynfs!= nullptr){
			mynfs->DirList(path,showHidden,extensions);
		}
	}
	
	void CFileBrowser::backDir(){
		if(mylocal!= nullptr){
			mylocal->backPath();
		}
		if(mysamba!= nullptr){
			mysamba->backDir();
		}
		if(myssh!= nullptr){
			myssh->backDir();
		}
		if(myftp!= nullptr){
			myftp->backDir();
		}
		if(myhttp!= nullptr){
			myhttp->backDir();
		}
		if(mynfs!= nullptr){
			mynfs->backDir();
		}
	}
	
	std::vector<FS::FileEntry> CFileBrowser::getCurrList(){
		if(mylocal!= nullptr){
			return mylocal->getCurrList();
		}
		if(mysamba!= nullptr){
			return mysamba->getCurrList();
		}
		if(myssh!= nullptr){
			return myssh->getCurrList();
		}
		if(myftp!= nullptr){
			return myftp->getCurrList();
		}
		if(myhttp!= nullptr){
			return myhttp->getCurrList();
		}
		if(mynfs!= nullptr){
			return mynfs->getCurrList();
		}
		std::vector<FS::FileEntry> retnull;
		return retnull;
	}
	
	std::string CFileBrowser::getCurrentPath(){
		if(mylocal!= nullptr){
			return mylocal->getCurrentPath();
		}
		if(mysamba!= nullptr){
			mysamba->getCurrPath();
		}
		if(myssh!= nullptr){
			myssh->getCurrPath();
		}
		if(myftp!= nullptr){
			myftp->getCurrPath();
		}
		if(myhttp!= nullptr){
			myhttp->getCurrPath();
		}
		if(mynfs!= nullptr){
			mynfs->getCurrPath();
		}
		
		return "";
	}
	
	void CFileBrowser::clearChecked(){
		if(mylocal!= nullptr){
			mylocal->clearChecked();
		}
		if(mysamba!= nullptr){
			mysamba->clearChecked();
		}
		if(myssh!= nullptr){
			myssh->clearChecked();
		}
		if(myftp!= nullptr){
			myftp->clearChecked();
		}
		if(myhttp!= nullptr){
			myhttp->clearChecked();
		}
		if(mynfs!= nullptr){
			mynfs->clearChecked();
		}
	}
	
	bool *CFileBrowser::checked(int pos){
		if(mylocal!= nullptr){
			return (bool *)mylocal->checked(pos);
		}
		if(mysamba!= nullptr){
			return (bool *)mysamba->checked(pos);
		}
		if(myssh!= nullptr){
			return (bool *)myssh->checked(pos);
		}
		if(myftp!= nullptr){
			return (bool *)myftp->checked(pos);
		}
		if(myhttp!= nullptr){
			return (bool *)myhttp->checked(pos);
		}
		if(mynfs!= nullptr){
			return (bool *)mynfs->checked(pos);
		}
		return (bool *)false;
	}
	
	std::string CFileBrowser::getBasePath(){
		if(mysamba!= nullptr){
			return mysamba->getBasePath();
		}
		if(myssh!= nullptr){
			return myssh->getBasePath();
		}
		if(myftp!= nullptr){
			return myftp->getBasePath();
		}
		if(myhttp!= nullptr){
			return myhttp->getBasePath();
		}
		if(mynfs!= nullptr){
			return mynfs->getBasePath();
		}
		return "";
	}
	std::string CFileBrowser::getShare(){
		if(mysamba!= nullptr){
			return mysamba->getShare();
		}
		return "";
	}
	
	std::string CFileBrowser::getUrl(){
		if(mysamba!= nullptr){
			return mysamba->getUrl();
		}
		if(myssh!= nullptr){
			return myssh->getUrl();
		}
		if(myftp!= nullptr){
			return myftp->getUrl();
		}
		if(myhttp!= nullptr){
			return myhttp->getUrl();
		}
		if(mynfs!= nullptr){
			return mynfs->getUrl();
		}
		return "";
	}