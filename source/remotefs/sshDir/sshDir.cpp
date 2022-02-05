#include "sshDir.h"

#ifdef NXMP_NETWORKSUPPORT

#ifdef __SWITCH__
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#ifdef WIN32
#define __FILESIZE "I64"
#else
#define __FILESIZE "llu"
#endif

sshDir::sshDir(std::string _url,Playlist * _playlist){
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


void sshDir::DirList(std::string path,bool showHidden,const std::vector<std::string> &extensions){
	int rc, sock;
    struct sockaddr_in sin;
	//const char *fingerprint;
	currentpath = path;
	LIBSSH2_SESSION *session;
    LIBSSH2_SFTP *sftp_session;
    LIBSSH2_SFTP_HANDLE *sftp_handle;
	
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
	
	urlschema thisurl = Utility::parseUrl(url);
	
	rc = libssh2_init(0);
	unsigned long hostaddr;
	hostaddr = inet_addr(thisurl.server.c_str());
    if(rc != 0) {
        printf("libssh2 initialization failed (%d)\n", rc);
    }
	sock = socket(AF_INET, SOCK_STREAM, 0);
 
    sin.sin_family = AF_INET;
    sin.sin_port = htons(22);
    sin.sin_addr.s_addr = hostaddr;
    if(connect(sock, (struct sockaddr*)(&sin),
               sizeof(struct sockaddr_in)) != 0) {
        printf("failed to connect!\n");
    }
	session = libssh2_session_init();
    if(!session){
		printf("unable to create session\n");
	}
	rc = libssh2_session_handshake(session, sock);

    if(rc) {
        printf("Failure establishing SSH session: %d\n", rc);
    }
	//fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
	
	if(libssh2_userauth_password(session, thisurl.user.c_str(), thisurl.pass.c_str())) {

		printf("\tAuthentication by password failed!\n");
	}
    else {
		printf("\tAuthentication by password succeeded.\n");
	}
	
	sftp_session = libssh2_sftp_init(session);
	if(!sftp_session) {
        printf("Unable to init SFTP session\n");
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
		printf("Unable to open dir with SFTP\n");

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
	
 
    libssh2_sftp_closedir(sftp_handle);

    libssh2_sftp_shutdown(sftp_session);
	
	libssh2_session_disconnect(session, "Normal Shutdown");

    libssh2_session_free(session);

 
#ifdef _WIN32
    closesocket(sock);
#else
    close(sock);
#endif
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

#endif