#include "fileBrowserClass.h"


	CFileBrowser::CFileBrowser(std::string _path,Playlist * _playlist,USBMounter * _myusb){
		
		if(_myusb!= nullptr){
			title = "USB Browser";
			myusb = _myusb;
			basepath = myusb->getBasePath();
			connected = true;
		}else{
			path = _path;
			urlschema thisurl = Utility::parseUrl(_path); 
			if(Utility::startWith(path,"/",false)){
				title = "File Browser";
				basepath = "/";
			}else if(Utility::startWith(path,"smb",false)){
				smb2fs = new CSMB2FS(_path,"smb0","smb0:");
				connected = smb2fs->RegisterFilesystem();
				basepath = "smb0:/";
				title = "SMB Browser " + thisurl.server;
				
			}else if(Utility::startWith(path,"sftp",false)){
				//myssh = new sshDir(path,_playlist);
				sshfs = new CSSHFS(_path,"ssh0","ssh0:");
				connected = sshfs->RegisterFilesystem();
				basepath = "ssh0:/";
				title = "SFTP Browser " + thisurl.server;
			}else if(Utility::startWith(path,"ftp",false)){
				//myftp = new FTPDir(path,_playlist);
				ftpfs = new CFTPFS(_path,"ftp0","ftp0:");
				connected = ftpfs->RegisterFilesystem();
				basepath = "ftp0:" + thisurl.path;
				currentpath = basepath;
				title = "FTP Browser " + thisurl.server;
				maxreadsize = 65536;
			}else if(Utility::startWith(path,"http",false)){
				myhttp = new HTTPDir(path);
				title = "HTTP Browser " + thisurl.server;
				timelessFS = true;
			}else if(Utility::startWith(path,"nfs",false)){
				title = "NFS Browser " + thisurl.server;
				nfsfs = new CNFSFS(_path,"nfs0","nfs0:");
				connected = nfsfs->RegisterFilesystem();
				basepath = "nfs0:/";
				currentpath = basepath;
			}
		}
	}
	
	CFileBrowser::~CFileBrowser(){
		
		if(myhttp!= nullptr){
			delete myhttp;
		}
		if(sshfs!= nullptr){
			delete sshfs;
		}
		if(smb2fs!= nullptr){
			delete smb2fs;
		}
		if(nfsfs!= nullptr){
			delete nfsfs;
		}
		if(archfs!= nullptr){
			delete archfs;
		}
		if(ftpfs!= nullptr){
			delete archfs;
		}
	
	}
	
	void CFileBrowser::OpenArchive(std::string _path){
		oldtitle = title;
		oldmount = currentpath;
		title = "Compressed Archive - " + _path.substr(_path.find_last_of("/") + 1);
		archfs = new CARCHFS(_path,"arc0","arc0:");
		connected = archfs->is_connected;
		basepath = archfs->mount_name+"/";
		currentpath = archfs->mount_name+"/";
		
	}
	std::string CFileBrowser::CloseArchive(){
		title = oldtitle;
		basepath = oldmount.substr(0,oldmount.find_first_of("/")+1);
		connected = true;
		delete archfs;
		archfs = nullptr;
		return oldmount;
	}
	
	void CFileBrowser::DirList(std::string path,bool showHidden,const std::vector<std::string> &extensions){
		currshowHidden = showHidden;
		if(myhttp!= nullptr){
			myhttp->DirList(path,extensions);
		}
		/*
		if(myusb!= nullptr){
			myusb->DirList(path,showHidden,extensions);
		}
		*/
		if(smb2fs!= nullptr || sshfs!=nullptr || nfsfs!=nullptr || archfs!=nullptr || myusb!= nullptr|| ftpfs!= nullptr){
			currentpath = path;
			currentlist.clear();
			//struct dirent *ent;
			DIR *dir;
			//NXLOG::DEBUGLOG("DIRLIST: %s\n",path.c_str());
			if (!path.empty()) {
				if ((dir = opendir(path.c_str())) != nullptr) {
				//NXLOG::DEBUGLOG("DIR OPEN",path.c_str());
					auto *reent    = __syscall_getreent();
					auto *devoptab = devoptab_list[dir->dirData->device];	
					
					
					
					while (/*(ent = readdir(dir)) != nullptr*/ true) {
							reent->deviceData = devoptab->deviceData;
							struct stat st{0};
							if (devoptab->dirnext_r(reent, dir->dirData, dir->fileData.d_name, &st))
							break;
							
							if ((/*path == "/" ||*/ strlen(dir->fileData.d_name) == 1) && dir->fileData.d_name[0] == '.') {
								continue;
							}
							if ((/*path == "/" ||*/ strlen(dir->fileData.d_name) == 2) && dir->fileData.d_name[0] == '.' && dir->fileData.d_name[1] == '.') {
								continue;
							}
							if (!showHidden && dir->fileData.d_name[0] == '.') {
								if (strlen(dir->fileData.d_name) != 2 && dir->fileData.d_name[1] != '.') {
									continue;
								}
							}
							FS::FileEntry file;
							file.name = dir->fileData.d_name;
							
							
							file.path = FS::removeLastSlash(path) + "/" + file.name;
							file.size = (size_t) st.st_size;
							file.type = S_ISDIR(st.st_mode) ? FS::FileEntryType::Directory : FS::FileEntryType::File;
							file.is_valid = 1;
							file.created = (time_t)st.st_ctime;
							file.modified = (time_t)st.st_mtime;
							file.accessed = (time_t)st.st_atime;
						
							
							
							if(Utility::isImageExtension(file.name)){
								file.mediatype = FS::FileMediaType::Image;
								currentimagelist.push_back(file);
							}
							if(Utility::isArchiveExtension(file.name)){
								file.mediatype = FS::FileMediaType::Archive;
							}
							currentlist.push_back(file);
							
						}
						
					
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
						currentlist.erase(
							std::remove_if(currentlist.begin(), currentlist.end(), [extensions](const FS::FileEntry &file) {
								for (auto &ext : extensions) {
									if (Utility::endsWith(file.name, ext, false)) {
										return false;
									}
								}
								return file.type == FS::FileEntryType::File;
						}), currentlist.end());
					}
				
			}
		}else if(basepath.find_first_of("/") == 0){
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
	}
	
	std::string CFileBrowser::backDir(){
		if(basepath.find_first_of("/") == 0 || smb2fs!=nullptr || sshfs!=nullptr || nfsfs!=nullptr || archfs !=nullptr || myusb!= nullptr|| ftpfs!= nullptr){
			std::string oldpath = currentpath;
			std::string relpath = currentpath.substr(currentpath.find_last_of("\\/")+1);
			
			currentpath = currentpath.substr(0, currentpath.find_last_of("\\/"));
			int pos = currentpath.find_last_of("\\/");
			if(pos<0)currentpath = currentpath+"/";
			if(oldpath == currentpath && archfs != nullptr){
				currentpath = CloseArchive();
				
			}
			if(currentpath == "")currentpath = "/";
			return relpath;
		}else if(myhttp!= nullptr){
			return myhttp->backDir();
		}/*else if(myusb!= nullptr){
			return myusb->backDir();
		}
		*/
		return "";
	}
	
	std::vector<FS::FileEntry> CFileBrowser::getCurrImageList(){
		
			if(basepath.find_first_of("/") == 0 || smb2fs!=nullptr || sshfs!=nullptr || nfsfs!=nullptr || archfs !=nullptr || myusb!= nullptr|| ftpfs!= nullptr){
				return currentimagelist;
			}
			if(myhttp!= nullptr){
				//return myhttp->getCurrList();
			}
			/*
			if(myusb!= nullptr){
				return myusb->getCurrImageList();
			}
			*/
		
		std::vector<FS::FileEntry> retnull;
		return retnull;
	}
	
	std::vector<FS::FileEntry> CFileBrowser::getCurrList(){
		
		if(basepath.find_first_of("/") == 0 || smb2fs!=nullptr|| sshfs!=nullptr || nfsfs!=nullptr || archfs !=nullptr || myusb!= nullptr|| ftpfs!= nullptr){
			return currentlist;
		}
			if(myhttp!= nullptr){
				return myhttp->getCurrList();
			}
			/*
			if(myusb!= nullptr){
				return myusb->getCurrList();
			}
			*/
		
		std::vector<FS::FileEntry> retnull;
		return retnull;
	}
	
	std::string CFileBrowser::getCurrentPath(){
		
			if(basepath.find_first_of("/") == 0 || smb2fs!=nullptr || sshfs!=nullptr || nfsfs!=nullptr || archfs !=nullptr || myusb!= nullptr || ftpfs!= nullptr){
				return currentpath;
			}
			
			
			
			if(myhttp!= nullptr){
				return myhttp->getCurrPath();
			}
			/*
			if(myusb!= nullptr){
				return myusb->getCurrentPath();
			}
				*/	
		
		return "";
	}
	
	void CFileBrowser::clearChecked(){
		
				
		if(myhttp!= nullptr){
			myhttp->clearChecked();
		}
		
	}
	
	bool *CFileBrowser::checked(int pos){
		
		
		
		if(myhttp!= nullptr){
			return (bool *)myhttp->checked(pos);
		}
		
		return (bool *)false;
	}
	
	std::string CFileBrowser::getBasePath(){
		if(basepath.find_first_of("/") == 0 || smb2fs!=nullptr || sshfs!= nullptr || nfsfs!=nullptr || archfs !=nullptr || myusb!= nullptr|| ftpfs!= nullptr){
			return basepath;
		}
		if(myhttp!= nullptr){
			return myhttp->getBasePath();
		}
		/*
		if(myusb!= nullptr){
			return myusb->getBasePath();
		}
		*/
		return "";
	}
	std::string CFileBrowser::getShare(){
		/*
		if(mysamba!= nullptr){
			return mysamba->getShare();
		}
		*/
		return "";
	}
	
	std::string CFileBrowser::getUrl(){
		
		
		if(myhttp!= nullptr){
			return myhttp->getUrl();
		}
		
		return "";
	}
	
	std::string CFileBrowser::getTitle(){
		return title;
	}
	
	std::string CFileBrowser::getOpenUrlPart(){
		
		
		urlschema thisurl = Utility::parseUrl(path); 
		
		if(myhttp!= nullptr){
			return thisurl.scheme + std::string("://") + thisurl.server + (thisurl.port.empty() ? std::string() : ':' + thisurl.port) + std::string("/");
		}
		
		if(myusb!= nullptr){
			return "";
		}
		return "";
	}
	
	void CFileBrowser::setSordOrder(FS::FILESORTORDER myorder){
		sortOrder = myorder;
		if(timelessFS){
			if(sortOrder == FS::FS_DATE_ASCENDINGORDER|| sortOrder == FS::FS_DATE_DESCENDINGORDER)sortOrder = FS::FS_NAME_ASCENDINGORDER;
		}
		
			
			if(myhttp!= nullptr){
				myhttp->sortOrder = sortOrder;
				myhttp->DirList(myhttp->getCurrPath(),Utility::getMediaExtensions());
			}
			DirList(getCurrentPath(),currshowHidden,Utility::getMediaExtensions());	
	}
	
	std::vector<FS::FileEntry> CFileBrowser::getChecked(){
		std::vector<FS::FileEntry> retvector;
		std::vector<FS::FileEntry> mycurrlist = getCurrList();
		for(int i=0;i<mycurrlist.size();i++){
			if(mycurrlist[i].checked)retvector.push_back(mycurrlist[i]);
		}
		
		return retvector;
		
	}
	
	void CFileBrowser::ResetDbStatus(){
		
		if(myhttp!= nullptr){
			myhttp->ResetDbStatus();
		}
		/*
		if(myusb!= nullptr){
			myusb->ResetDbStatus();
		}
		*/
		if(basepath.find_first_of("/") == 0 || smb2fs!=nullptr || sshfs!=nullptr || nfsfs!=nullptr || archfs !=nullptr|| myusb!= nullptr || ftpfs!= nullptr){
			for(int i=0;i<currentlist.size();i++)
			{		
				currentlist[i].dbread = -1;
			}
		}
	}
	
	void CFileBrowser::SetFileDbStatus(int idx,int dbstatus){
		
		if(myhttp!= nullptr){
			myhttp->SetFileDbStatus(idx,dbstatus);
		}
		/*
		if(myusb!= nullptr){
			myusb->SetFileDbStatus(idx,dbstatus);
		}
		*/
		if(basepath.find_first_of("/") == 0 || smb2fs!=nullptr || sshfs!=nullptr || nfsfs!=nullptr || archfs !=nullptr|| myusb!= nullptr|| ftpfs!= nullptr){
			currentlist[idx].dbread = dbstatus;
		}
		
	}
	
	
	void CFileBrowser::setBasePath(std::string _basepath){
		
		if(myusb != nullptr){
			basepath = _basepath;
			myusb->setBasePath(_basepath);
		}
		
	}
	
	std::vector<usb_devices> CFileBrowser::getUsbDev(bool dummy){
		if(myusb != nullptr){
			return myusb->mounted_devs;
		}
		
		if(dummy){
			std::vector<usb_devices> retdevicearray;
			usb_devices dummy;
			dummy.mount_point = "";
			dummy.name = "DUMMY";
			dummy.fstype = "FAT32";
			dummy.capacity = 1073741824;
			std::string mount_point;
			retdevicearray.push_back(dummy);
			return  retdevicearray;
		}
		return std::vector<usb_devices>();
	}
	
	
	void file_read_Thread(void *arg) {
		//CFileBrowser * ctx = (CFileBrowser *)arg;
		fileload_struct *fileref  = (fileload_struct *)arg;
		
		
		fileref->currentOffset = 0;
		//size_t offset = 0;
		size_t bytesRead = 0;
		
		if(fileref->mem != nullptr)free(fileref->mem);
		
		int myfd = open(fileref->path,O_RDONLY);
		if(myfd<0){
				return;
		}
		
		fileref->mem = (unsigned char*)malloc(fileref->size*sizeof(unsigned char)); 
			
		char buffer[8192];
		while (true){
			bytesRead = read(myfd, buffer, 8192);
			if(bytesRead<=0)break;
			memcpy(fileref->mem+fileref->currentOffset,buffer,bytesRead);
			fileref->currentOffset+=bytesRead;
		}
		
		close(myfd);
		fileref->memvalid = true;
		
		/*
		if (stat(ctx->LoadedFileName.c_str(), &st) == 0) {
			
			if(ctx->LoadedFileBuffer!=nullptr)free(ctx->LoadedFileBuffer);
					
			ctx->LoadedFileBuffer = (unsigned char*)malloc(st.st_size*sizeof(unsigned char)); 
			
			int myfd = open(ctx->LoadedFileName.c_str(),O_RDONLY);
			
			if(myfd<0){
				free(ctx->LoadedFileBuffer);
				return;
			}
			
			//size_t offset = 0;
			char buffer[ctx->maxreadsize];
			
			
			while (true){
				bytesRead = read(myfd, buffer, ctx->maxreadsize);
				if(bytesRead<=0)break;
				memcpy(ctx->LoadedFileBuffer+ctx->CurrentReadOffset,buffer,bytesRead);
				ctx->CurrentReadOffset+=bytesRead;
			}
			
			if(ctx->CurrentReadOffset>=st.st_size){
				
				ctx->LoadedFileSize = st.st_size;
				ctx->LoadedFileStatus = true;
			}
			
			close(myfd);
			
			
			
		}
		*/
		
	}
	
	
	bool CFileBrowser::getfileContentsThreaded(std::string filepath){
		currentFileinUse = filepath;
		
			
		if(myhttp!= nullptr){
				//myhttp->SetFileDbStatus(idx,dbstatus);
		}
			
		if(basepath.find_first_of("/") == 0 || smb2fs!=nullptr || sshfs!=nullptr || nfsfs!=nullptr || archfs !=nullptr|| myusb!= nullptr || ftpfs!= nullptr){
				struct stat st = {0}; 
				if (stat(filepath.c_str(), &st) == 0) {
					if(LoadedFile!=nullptr){
						if(LoadedFile->mem !=nullptr){
							free(LoadedFile->mem);
						}
						free(LoadedFile);
					}
					
					
					LoadedFile = (fileload_struct *)malloc(sizeof(fileload_struct));
					LoadedFile->memvalid = false;
					LoadedFile->readbuffersize = maxreadsize;
					memset(LoadedFile->path,0,NAME_MAX);
					memcpy(LoadedFile->path,filepath.c_str(),filepath.length());
					//LoadedFile->path = filepath;
					LoadedFile->size = st.st_size;
					LoadedFile->mem = (unsigned char*)malloc(st.st_size*sizeof(unsigned char)); 
		
					LoadedFile->currentOffset = 0;
					threadCreate(&readThreadref, file_read_Thread, LoadedFile, NULL, 0x100000, 0x3B, -2);
					threadStart(&readThreadref);
					
					/*
					LoadedFile.memvalid = false;
					LoadedFile.readbuffersize = maxreadsize;
					LoadedFile.path = filepath;
					LoadedFile.size = st.st_size;
					LoadedFile.currentOffset = 0;
					threadCreate(&readThreadref, file_read_Thread, (void *)&LoadedFile, NULL, 0x100000, 0x3B, -2);
					threadStart(&readThreadref);
					*/
					return true;
				}
				
			
		}
		return false;
	}
	
	
	bool CFileBrowser::getfileContents(std::string filepath,unsigned char ** _filedata,int &_size){
		currentFileinUse = filepath;
		
			
		
		return false;
	}
	
	
	int CFileBrowser::getNextImg(){
		std::vector<FS::FileEntry>myimglist = getCurrImageList();
		for(int i=0;i<myimglist.size();i++){
			if(currentFileinUse == myimglist[i].path && i<myimglist.size()-1){
				return i+1;
			}
		}
		
		return -1;
	}
	int CFileBrowser::getPrevImg(){
		
		std::vector<FS::FileEntry>myimglist = getCurrImageList();
		for(int i=0;i<myimglist.size();i++){
			if(currentFileinUse == myimglist[i].path && i>0){
				return i-1;
			}
		}
		
		return -1;
	}
	