#include "fileBrowserClass.h"
	
	


	CFileBrowser::CFileBrowser(std::string _path,Playlist * _playlist,USBMounter * _myusb){
		
		if(_myusb!= nullptr){
			title = "USB Browser";
			myusb = _myusb;
		}else{
			path = _path;
			if(Utility::startWith(path,"/",false)){
				mylocal = new localFs(path,_playlist);
				title = "File Browser";
				timelessFS = true;
			}else if(Utility::startWith(path,"smb",false)){
				mysamba = new sambaDir(path,_playlist);
				title = "SMB Browser";
			}else if(Utility::startWith(path,"sftp",false)){
				myssh = new sshDir(path,_playlist);
				title = "SFTP Browser";
			}else if(Utility::startWith(path,"ftp",false)){
				myftp = new FTPDir(path,_playlist);
				title = "FTP Browser";
			}else if(Utility::startWith(path,"http",false)){
				myhttp = new HTTPDir(path);
				title = "HTTP Browser";
				timelessFS = true;
			}else if(Utility::startWith(path,"nfs",false)){
				title = "NFS Browser";
				mynfs = new nfsDir(path,_playlist);
			}
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
		currshowHidden = showHidden;
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
		if(myusb!= nullptr){
			myusb->DirList(path,showHidden,extensions);
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
		if(myusb!= nullptr){
			myusb->backPath();
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
		if(myusb!= nullptr){
			return myusb->getCurrList();
		}
		std::vector<FS::FileEntry> retnull;
		return retnull;
	}
	
	std::string CFileBrowser::getCurrentPath(){
		if(mylocal!= nullptr){
			return mylocal->getCurrentPath();
		}
		if(mysamba!= nullptr){
			return mysamba->getCurrPath();
		}
		if(myssh!= nullptr){
			return myssh->getCurrPath();
		}
		if(myftp!= nullptr){
			return myftp->getCurrPath();
		}
		if(myhttp!= nullptr){
			return myhttp->getCurrPath();
		}
		if(mynfs!= nullptr){
			return mynfs->getCurrPath();
		}
		if(myusb!= nullptr){
			return myusb->getCurrentPath();
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
		if(myusb!= nullptr){
			return myusb->clearChecked();
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
		if(mynfs!= nullptr){
			return (bool *)mynfs->checked(pos);
		}
		if(myusb!= nullptr){
			return (bool *)myusb->checked(pos);
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
		if(myusb!= nullptr){
			return myusb->getBasePath();
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
	
	std::string CFileBrowser::getTitle(){
		return title;
	}
	
	std::string CFileBrowser::getOpenUrlPart(){
		if(mylocal!= nullptr){
			return "";
		}
		urlschema thisurl = Utility::parseUrl(path); 
		if(mysamba!= nullptr){
			if(thisurl.user.empty()){
				return thisurl.scheme + std::string("://") + "Guest" + std::string("@") + thisurl.server + std::string("/") + getShare() + std::string("/");
			}else{
				
				return thisurl.scheme + std::string("://") + thisurl.user + std::string(":") + thisurl.pass + std::string("@") + thisurl.server + std::string("/") + getShare() + std::string("/");
			}
		}
		if(myssh!= nullptr){
			return thisurl.scheme + std::string("://") + thisurl.user + std::string(":") + thisurl.pass + std::string("@") + thisurl.server + (thisurl.port.empty() ? std::string() : ':' + thisurl.port);							
		}
		if(myftp!= nullptr){
			return thisurl.scheme + std::string("://") + thisurl.user + std::string(":") + thisurl.pass + std::string("@") + thisurl.server + (thisurl.port.empty() ? std::string() : ':' + thisurl.port) + std::string("/");
		}
		if(myhttp!= nullptr){
			return thisurl.scheme + std::string("://") + thisurl.server + (thisurl.port.empty() ? std::string() : ':' + thisurl.port) + std::string("/");
		}
		if(mynfs!= nullptr){
			return thisurl.scheme + std::string("://") + thisurl.server;
		}
		if(myusb!= nullptr){
			return "";
		}
		return "";
	}
	
	void CFileBrowser::setSordOrder(FS::FILESORTORDER myorder){
		sortOrder = myorder;
		if(mylocal!= nullptr){
			mylocal->sortOrder = sortOrder;
			mylocal->DirList(mylocal->getCurrentPath(),currshowHidden,Utility::getMediaExtensions());
		}
		if(mysamba!= nullptr){
			mysamba->sortOrder = sortOrder;
			mysamba->DirList(mysamba->getCurrPath(),currshowHidden,Utility::getMediaExtensions());
		}
		if(myssh!= nullptr){
			myssh->sortOrder = sortOrder;
			myssh->DirList(myssh->getCurrPath(),currshowHidden,Utility::getMediaExtensions());
		}
		if(myftp!= nullptr){
			myftp->sortOrder = sortOrder;
			//myftp->DirList(myftp->getCurrPath(),Utility::getMediaExtensions());
		}
		if(myhttp!= nullptr){
			myhttp->sortOrder = sortOrder;
			myhttp->DirList(myhttp->getCurrPath(),Utility::getMediaExtensions());
		}
		if(mynfs!= nullptr){
			mynfs->sortOrder = sortOrder;
			mynfs->DirList(mynfs->getCurrPath(),currshowHidden,Utility::getMediaExtensions());
		}
		if(myusb!= nullptr){
			myusb->sortOrder = sortOrder;
			myusb->DirList(myusb->getCurrentPath(),currshowHidden,Utility::getMediaExtensions());
		}
	}
	
	std::vector<FS::FileEntry> CFileBrowser::getChecked(){
		std::vector<FS::FileEntry> retvector;
		std::vector<FS::FileEntry> mycurrlist = getCurrList();
		for(int i=0;i<mycurrlist.size();i++){
			if(mycurrlist[i].checked)retvector.push_back(mycurrlist[i]);
		}
		
		return retvector;
		
	}
	
	
	void CFileBrowser::setBasePath(std::string _basepath){
		if(myusb != nullptr){
			myusb->setBasePath(_basepath);
		}
	}
	
	std::vector<usb_devices> CFileBrowser::getUsbDev(){
		if(myusb != nullptr){
			return myusb->mounted_devs;
		}
		return std::vector<usb_devices>();
	}