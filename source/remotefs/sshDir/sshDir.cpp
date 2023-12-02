#include "sshDir.h"

#include <sys/socket.h>
#include <arpa/inet.h>



sshDir::sshDir(std::string _url,Playlist * _playlist){
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


bool sshDir::DirList(std::string path,bool showHidden,const std::vector<std::string> &extensions){
	int rc, sock;
    struct sockaddr_in sin;
	//const char *fingerprint;
	currentpath = path;
	LIBSSH2_SESSION *session;
    LIBSSH2_SFTP *sftp_session;
    LIBSSH2_SFTP_HANDLE *sftp_handle;
	
	urlschema thisurl = Utility::parseUrl(url);
	
	rc = libssh2_init(0);
	unsigned long hostaddr;
	hostaddr = inet_addr(thisurl.server.c_str());
    if(rc != 0) {
        NXLOG::ERRORLOG("libssh2 initialization failed (%d)\n", rc);
		errormsg = "libssh2 initialization failed";
		return false;
    }
	sock = socket(AF_INET, SOCK_STREAM, 0);
 
    sin.sin_family = AF_INET;
    sin.sin_port = htons(thisurl.port.empty() ? 22 : std::stoi(thisurl.port));
    sin.sin_addr.s_addr = hostaddr;
    if(connect(sock, (struct sockaddr*)(&sin),
               sizeof(struct sockaddr_in)) != 0) {
        NXLOG::ERRORLOG("failed to connect!\n");
		errormsg = "failed to connect!";
		return false;
    }
	session = libssh2_session_init();
    if(!session){
		NXLOG::ERRORLOG("unable to create session\n");
		errormsg = "failed to connect!";
		return false;
	}
	rc = libssh2_session_handshake(session, sock);

    if(rc) {
        NXLOG::ERRORLOG("Failure establishing SSH session: %d\n", rc);
		errormsg = "Failure establishing SSH session: " + std::to_string(rc);
		return false;
    }
	//fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
	
	if(libssh2_userauth_password(session, thisurl.user.c_str(), thisurl.pass.c_str())) {

		NXLOG::ERRORLOG("\tAuthentication by password failed!\n");
		errormsg = "Authentication by password failed!";
		return false;
	}
    else {
		NXLOG::DEBUGLOG("\tAuthentication by password succeeded.\n");
	}
	
	sftp_session = libssh2_sftp_init(session);
	if(!sftp_session) {
        NXLOG::ERRORLOG("Unable to init SFTP session\n");
		errormsg = "Unable to init SFTP session";
		return false;
    }
	
	libssh2_session_set_blocking(session, 1);


	char tempHome[512];
	
	if(path == ""){
		libssh2_sftp_realpath(sftp_session, ".", tempHome, 512);
		path = tempHome;
		basepath = path;
		currentpath = path;
	}
	
    sftp_handle = libssh2_sftp_opendir(sftp_session, path.c_str());
	
	if(!sftp_handle) {
		NXLOG::ERRORLOG("Unable to open dir with SFTP\n");
		errormsg = "Unable to open dir with SFTP";
		return false;
	}
	currentlist.clear();
	do {
        char mem[512];
        char longentry[512];
        LIBSSH2_SFTP_ATTRIBUTES attrs;
 
        /* loop until we fail */ 
        rc = libssh2_sftp_readdir_ex(sftp_handle, mem, sizeof(mem),longentry, sizeof(longentry), &attrs);
        if(rc > 0) {
			FS::FileEntry tmpentry;
            /* rc is the length of the file name in the mem
               buffer */ 
 
            if ((attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) &&
				((attrs.permissions & LIBSSH2_SFTP_S_IFMT) ==
				LIBSSH2_SFTP_S_IFDIR)) {
				tmpentry.type = FS::FileEntryType::Directory;
				    
		   }else{
				tmpentry.type = FS::FileEntryType::File;
		   }
				
			 if (attrs.flags & LIBSSH2_SFTP_ATTR_SIZE) {
				tmpentry.size = attrs.filesize;
			
			}
			if (attrs.flags & LIBSSH2_SFTP_ATTR_ACMODTIME) {
				tmpentry.is_valid = 1;
				tmpentry.accessed = attrs.atime;
				tmpentry.modified = attrs.mtime;
				tmpentry.created = 0;
				//tmpentry.timestamp.created = attrs.ctime;
			}
			
			tmpentry.name = mem;
			tmpentry.dbread = -1;
			tmpentry.path = path + std::string("/") + std::string(mem);
			if(!showHidden){
				if (mem[0] == '.') {
					continue;
				}
			}
			if ((strlen(mem) == 1) && mem[0] == '.') {
				continue;
			}
			if ((strlen(mem) == 2) && mem[0] == '.' && mem[1] == '.') {
				continue;
			}
			if(Utility::isImageExtension(tmpentry.name)){
				tmpentry.mediatype = FS::FileMediaType::Image;
				currentimagelist.push_back(tmpentry);
			}
			if(Utility::isArchiveExtension(tmpentry.name)){
				tmpentry.mediatype = FS::FileMediaType::Archive;
			}
			currentlist.push_back(tmpentry);
        }
        else
            break;
 
    } while(1);
	
	
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
	
 
    libssh2_sftp_closedir(sftp_handle);

    libssh2_sftp_shutdown(sftp_session);
	
	libssh2_session_disconnect(session, "Normal Shutdown");

    libssh2_session_free(session);

    close(sock);
    libssh2_exit();

	return true;
}


std::vector<FS::FileEntry> sshDir::getCurrList(){
	return currentlist;
}

std::vector<FS::FileEntry> sshDir::getCurrImageList(){
	return currentimagelist;
}

bool *sshDir::checked(int pos){
	return &currentlist[pos].checked;
}

std::string sshDir::getUrl(){
	return url;
}

std::string sshDir::getCurrPath(){
	return currentpath;
}

std::string sshDir::getBasePath(){
	return basepath;
}

void sshDir::clearChecked(){
	for(int i=0;i<currentlist.size();i++){
		currentlist[i].checked = false;
	}
}

bool sshDir::getfileContents(std::string filepath,unsigned char ** _filedata,int &_size){
	
	int rc, sock;
    struct sockaddr_in sin;
	//const char *fingerprint;
	LIBSSH2_SESSION *session;
    LIBSSH2_SFTP *sftp_session;
    LIBSSH2_SFTP_HANDLE *sftp_handle;
	LIBSSH2_CHANNEL *channel;
    libssh2_struct_stat fileinfo;
	
	urlschema thisurl = Utility::parseUrl(url);
	
	rc = libssh2_init(0);
	unsigned long hostaddr;
	hostaddr = inet_addr(thisurl.server.c_str());
    if(rc != 0) {
        NXLOG::ERRORLOG("libssh2 initialization failed (%d)\n", rc);
		errormsg = "libssh2 initialization failed";
		return false;
    }
	sock = socket(AF_INET, SOCK_STREAM, 0);
 
    sin.sin_family = AF_INET;
    sin.sin_port = htons(thisurl.port.empty() ? 22 : std::stoi(thisurl.port));
    sin.sin_addr.s_addr = hostaddr;
    if(connect(sock, (struct sockaddr*)(&sin),
               sizeof(struct sockaddr_in)) != 0) {
        NXLOG::ERRORLOG("failed to connect!\n");
		errormsg = "failed to connect!";
		return false;
    }
	session = libssh2_session_init();
    if(!session){
		NXLOG::ERRORLOG("unable to create session\n");
		errormsg = "failed to connect!";
		return false;
	}
	rc = libssh2_session_handshake(session, sock);

    if(rc) {
        NXLOG::ERRORLOG("Failure establishing SSH session: %d\n", rc);
		errormsg = "Failure establishing SSH session: " + std::to_string(rc);
		return false;
    }
	//fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
	
	if(libssh2_userauth_password(session, thisurl.user.c_str(), thisurl.pass.c_str())) {

		NXLOG::ERRORLOG("\tAuthentication by password failed!\n");
		errormsg = "Authentication by password failed!";
		return false;
	}
    else {
		NXLOG::DEBUGLOG("\tAuthentication by password succeeded.\n");
	}
	
	
	libssh2_session_set_blocking(session, 1);
	channel = libssh2_scp_recv2(session, filepath.c_str(), &fileinfo);
	
	_size = fileinfo.st_size;
	*_filedata = (unsigned char *)malloc(fileinfo.st_size);
	
	int chunksize = 1024*1024;
	off_t chunk=0;
	
	int ret = 0;
	
	while ( chunk < fileinfo.st_size ){
		size_t readnow;
		readnow = libssh2_channel_read(channel, *_filedata+chunk, chunksize);
		
		if(readnow > 0) {

        }
        else if(readnow < 0) {
            NXLOG::ERRORLOG("libssh2_channel_read() failed: %d\n",(int)readnow);
            ret = 1;
			break;
        }
		
		
		chunk=chunk+readnow;
	}
	
	libssh2_channel_free(channel);

	libssh2_session_disconnect(session, "Normal Shutdown");

    libssh2_session_free(session);

    close(sock);
    libssh2_exit();
	if(ret == 1){
		NXLOG::DEBUGLOG("\tFile Read Error\n");
		return false;
	}
	
	NXLOG::DEBUGLOG("\tFile Read OK\n");
	return true;
}

std::string sshDir::backDir(){
	if(currentpath.find_last_of("/") == 0)currentpath = basepath;
	if(currentpath.find_last_of("/") == -1)currentpath = basepath;
	if(currentpath == basepath)return "";
	currentpath = FS::removeLastSlash(currentpath);
	std::string retpath = currentpath.substr(currentpath.find_last_of("\\/")+1);
	currentpath = currentpath.substr(0, currentpath.find_last_of("/"));
	return retpath;
}

void sshDir::SetFileDbStatus(int idx,int dbstatus){
		currentlist[idx].dbread = dbstatus;
	}
	
void sshDir::ResetDbStatus(){
	for(int i=0;i<currentlist.size();i++){
		currentlist[i].dbread = -1;
	}
}
