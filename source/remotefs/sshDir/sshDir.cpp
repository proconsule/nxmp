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


void sshDir::DirList(std::string path,bool showHidden,const std::vector<std::string> &extensions){
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
    }
	sock = socket(AF_INET, SOCK_STREAM, 0);
 
    sin.sin_family = AF_INET;
    sin.sin_port = htons(thisurl.port.empty() ? 22 : std::stoi(thisurl.port));
    sin.sin_addr.s_addr = hostaddr;
    if(connect(sock, (struct sockaddr*)(&sin),
               sizeof(struct sockaddr_in)) != 0) {
        NXLOG::ERRORLOG("failed to connect!\n");
    }
	session = libssh2_session_init();
    if(!session){
		NXLOG::ERRORLOG("unable to create session\n");
	}
	rc = libssh2_session_handshake(session, sock);

    if(rc) {
        NXLOG::ERRORLOG("Failure establishing SSH session: %d\n", rc);
    }
	//fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
	
	if(libssh2_userauth_password(session, thisurl.user.c_str(), thisurl.pass.c_str())) {

		NXLOG::ERRORLOG("\tAuthentication by password failed!\n");
	}
    else {
		NXLOG::DEBUGLOG("\tAuthentication by password succeeded.\n");
	}
	
	sftp_session = libssh2_sftp_init(session);
	if(!sftp_session) {
        NXLOG::ERRORLOG("Unable to init SFTP session\n");
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

	
}


std::vector<FS::FileEntry> sshDir::getCurrList(){
	return currentlist;
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



void sshDir::backDir(){
	if(currentpath.find_last_of("/") == 0)currentpath = basepath;
	if(currentpath.find_last_of("/") == -1)currentpath = basepath;
	if(currentpath == basepath)return;
	currentpath = FS::removeLastSlash(currentpath);
	currentpath = currentpath.substr(0, currentpath.find_last_of("/"));
}
