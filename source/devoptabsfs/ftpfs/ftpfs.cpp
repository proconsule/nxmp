/* based on https://gist.github.com/averne/527f3a739d19c8d573b2d6995a33edea */

/* based also on https://github.com/dborth/wiimc/blob/master/source/utils/ftp_devoptab.c */

#include "ftpfs.h"
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/syslimits.h>
#include <regex>

#include "ugly-ftplib.h"

#define MAX_READBUF 8192


CFTPFS::ftpurlschema CFTPFS::parseFTPUrl(std::string url){
	ftpurlschema res;
	std::regex rgx("(?:([^:]*)://)?(?:([^:@]*)(?::([^@]*))?@)?(?:([^/:]*))?(?::([0-9]*))?/(/[^?#]*(=.*?/)/)?([^?#]*)?(?:([^#]*))?(?:#(.*))?");
	std::smatch match;
	if(std::regex_search(url, match, rgx)) {
		res.scheme = match[1];
		res.user = match[2];
		res.pass = match[3];
		res.server = match[4];
		res.port = match[5];
		res.path = match[8];
	}
	return res;
}

std::vector<std::string> split_newline(const std::string& str)
{
    auto result = std::vector<std::string>{};
    auto ss = std::stringstream{str};

    for (std::string line; std::getline(ss, line, '\n');)
        result.push_back(line);

    return result;
}


CFTPFS::CFTPFS(std::string _url,std::string _name,std::string _mount_name){
	this->connect_url = _url;
	
	
	this->name       = _name;
    this->mount_name = _mount_name;

    this->devoptab = {
        .name         = CFTPFS::name.data(),

        .structSize   = sizeof(CFTPFSFile),
        .open_r       = CFTPFS::ftpfs_open,
        .close_r      = CFTPFS::ftpfs_close,
        .read_r       = CFTPFS::ftpfs_read,
        .seek_r       = CFTPFS::ftpfs_seek,
        .fstat_r      = CFTPFS::ftpfs_fstat,

        .stat_r       = CFTPFS::ftpfs_stat,
        .chdir_r      = CFTPFS::ftpfs_chdir,

        .dirStateSize = sizeof(CFTPFSDir),
        .diropen_r    = CFTPFS::ftpfs_diropen,
        .dirreset_r   = CFTPFS::ftpfs_dirreset,
        .dirnext_r    = CFTPFS::ftpfs_dirnext,
        .dirclose_r   = CFTPFS::ftpfs_dirclose,

        .statvfs_r    = CFTPFS::ftpfs_statvfs,

        .deviceData   = this,

        .lstat_r      = CFTPFS::ftpfs_stat,
    };
	this->urlschema = this->parseFTPUrl(connect_url);
	if(connect(urlschema.server,atoi(urlschema.port.c_str()),urlschema.user,urlschema.pass) == 0){
		this->cwd = "/";
		register_fs();
	}
	
}


CFTPFS::CFTPFS(std::string _server,int _port,std::string _username,std::string _password,std::string _path,std::string _name,std::string _mount_name){
	
	
	this->name       = _name;
    this->mount_name = _mount_name;
	
	this->server = server;
	this->port = _port;
	this->username = _username;
	this->password = _password;
	this->path = _path;

    this->devoptab = {
        .name         = CFTPFS::name.data(),

        .structSize   = sizeof(CFTPFSFile),
        .open_r       = CFTPFS::ftpfs_open,
        .close_r      = CFTPFS::ftpfs_close,
        .read_r       = CFTPFS::ftpfs_read,
        .seek_r       = CFTPFS::ftpfs_seek,
        .fstat_r      = CFTPFS::ftpfs_fstat,

        .stat_r       = CFTPFS::ftpfs_stat,
        .chdir_r      = CFTPFS::ftpfs_chdir,

        .dirStateSize = sizeof(CFTPFSDir),
        .diropen_r    = CFTPFS::ftpfs_diropen,
        .dirreset_r   = CFTPFS::ftpfs_dirreset,
        .dirnext_r    = CFTPFS::ftpfs_dirnext,
        .dirclose_r   = CFTPFS::ftpfs_dirclose,

        .statvfs_r    = CFTPFS::ftpfs_statvfs,

        .deviceData   = this,

        .lstat_r      = CFTPFS::ftpfs_stat,
    };
	//this->urlschema = this->parseFTPUrl(connect_url);
	//if(connect(urlschema.server,atoi(urlschema.port.c_str()),urlschema.user,urlschema.pass) == 0){
	//	this->cwd = "/";
	//	register_fs();
	//}
	
}
	


bool CFTPFS::CheckConnection(){
	if(connect(urlschema.server,atoi(urlschema.port.c_str()),urlschema.user,urlschema.pass) == 0){
		this->cwd = "/";
		register_fs();
		return true;
	}
	return false;
}

bool CFTPFS::RegisterFilesystem(){
	if(connect(urlschema.server,atoi(urlschema.port.c_str()),urlschema.user,urlschema.pass) == 0){
		this->cwd = "/";
		register_fs();
		fs_regisered = true;
		return true;
	}
	return false;
}


bool CFTPFS::RegisterFilesystem_v2(){
	if(connect(this->server,this->port,this->username,this->password) == 0){
		this->cwd = "/";
		register_fs();
		fs_regisered = true;
		return true;
	}
	return false;
}


CFTPFS::~CFTPFS(){
	if (this->is_connected)
        this->disconnect();

	
	if(fs_regisered){
		unregister_fs();
	}
}

void CFTPFS::disconnect(){
	
    auto lk = std::scoped_lock(this->session_mutex);
	

    this->is_connected = false;
}
std::string CFTPFS::translate_path(const char *path){
   return this->cwd + (path + CFTPFS::mount_name.length()+1);
}


int CFTPFS::connect(std::string _host, std::uint16_t _port,
        std::string _username, std::string _password){
	
	this->host = _host;
	this->port = _port;
	this->user = _username;
	this->pass = _password;
	
	
	server_connection testlib;
	
	UFTP_Init_Server_Connection(&testlib,this->host,this->port,this->user,this->pass);
	
	int ret = UFTP_ServerConnect(&testlib);
	if(ret<0){
		return -1;
		
	}
	
	this->is_connected = testlib.connected;
	UFTP_CloseServer(&testlib);
	return 0;
}


int CFTPFS::ftpfs_open(struct _reent *r, void *fileStruct, const char *path, int flags, int mode) {
    auto *priv      = static_cast<CFTPFS     *>(r->deviceData);
    auto *priv_file = static_cast<CFTPFSFile *>(fileStruct);

    auto internal_path = priv->translate_path(path);
	if (internal_path.empty()) {
        __errno_r(r) = EINVAL;
        return -1;
    }

	
	auto lk = std::scoped_lock(priv->session_mutex);
	
	memset(priv_file->filename,0,NAME_MAX);
	memcpy(priv_file->filename,internal_path.data(),internal_path.length());
	
	
	UFTP_Init_Server_Connection(&priv_file->connection,priv->host,priv->port,priv->user,priv->pass);
	
	int ret = UFTP_ServerConnect(&priv_file->connection);
	if(ret == -1)return -1;
	
	bool cachefound = false;
	for(int i=0;i<priv->cachedirlist.size();i++){
		if(internal_path == priv->cachedirlist[i].fullpathname){
			memcpy(&priv_file->st,&priv->cachedirlist[i].st,sizeof(struct stat));
			cachefound=true;
			break;
		}
	}
	
	if(cachefound){
		
	}else{
	
		ret = UFTP_STATFile(&priv_file->connection,priv_file->filename,&priv_file->st);
		if(ret<0)return -1;
	}
	
	priv_file->readoffset = 0;
	priv_file->offset = 0;
	
	
	ret = UFTP_OpenFile(&priv_file->connection,priv_file->filename,priv_file->offset);
	if(ret == -1)return -1;
	priv_file->opened = true;
	
	
	
    return 0;
}

int CFTPFS::ftpfs_close(struct _reent *r, void *fd) {
    auto *priv      = static_cast<CFTPFS     *>(r->deviceData);
    auto *priv_file = static_cast<CFTPFSFile *>(fd);

    auto lk = std::scoped_lock(priv->session_mutex);
	UFTP_CloseServer(&priv_file->connection);
	priv_file->opened = false;
	return 0;
}

ssize_t CFTPFS::ftpfs_read(struct _reent *r, void *fd, char *ptr, size_t len) {
    auto *priv      = static_cast<CFTPFS     *>(r->deviceData);
    auto *priv_file = static_cast<CFTPFSFile *>(fd);
	
	auto lk = std::scoped_lock(priv->session_mutex);

	
	if(!priv_file->connection.connected){
		UFTP_ServerConnect(&priv_file->connection);
		
	}
	if(priv_file->connection.data_socket==-1){
		UFTP_OpenFile(&priv_file->connection,priv_file->filename,priv_file->offset);
		priv_file->opened = true;
	}

	if(priv_file->offset>=priv_file->st.st_size){
		return 0;
	}
	
	int readnum = len;
	
	if(readnum>MAX_READBUF)readnum=MAX_READBUF;
	
	if(priv_file->offset+readnum>=priv_file->st.st_size){
		readnum=priv_file->st.st_size-priv_file->offset;
	}	

	int rc = read(priv_file->connection.data_socket,ptr,readnum);
	if(rc<=0)return rc;
	

	priv_file->offset = priv_file->offset+rc;
	priv_file->readoffset = priv_file->offset;
	return rc;
}

off_t CFTPFS::ftpfs_seek(struct _reent *r, void *fd, off_t pos, int dir) {
    auto *priv      = static_cast<CFTPFS     *>(r->deviceData);
    auto *priv_file = static_cast<CFTPFSFile *>(fd);

    off_t offset;
    switch (dir) {
        default:
        case SEEK_SET:
            offset = 0;
            break;
        case SEEK_CUR:
            offset = priv_file->offset;
            break;
        case SEEK_END:
            offset = priv_file->st.st_size;
            break;
    }

    
    auto lk = std::scoped_lock(priv->session_mutex);
	
	priv_file->offset = offset + pos;
	
	if(priv_file->connection.data_socket >0){
		UFTP_AbortTransfer(&priv_file->connection);
	}
	
	priv_file->readoffset = -1;
	return priv_file->offset;
}

int CFTPFS::ftpfs_fstat(struct _reent *r, void *fd, struct stat *st) {
    auto *priv_file = static_cast<CFTPFSFile *>(fd);
	
	
	memcpy(st,&priv_file->st,sizeof(struct stat));
	
    return 0;
}

int CFTPFS::ftpfs_stat(struct _reent *r, const char *file, struct stat *st) {
    auto *priv = static_cast<CFTPFS *>(r->deviceData);
	auto internal_path = priv->translate_path(file);
    if (internal_path.empty()) {
        __errno_r(r) = EINVAL;
        return -1;
    }
	
    auto lk = std::scoped_lock(priv->session_mutex);
	
	for(int i=0;i<priv->cachedirlist.size();i++){
		if(internal_path == priv->cachedirlist[i].fullpathname){
			memcpy(st,&priv->cachedirlist[i].st,sizeof(struct stat));
			return 0;
		}
	}
	
	
	server_connection testlib;
	UFTP_Init_Server_Connection(&testlib,priv->host,priv->port,priv->user,priv->pass);
	int ret = UFTP_ServerConnect(&testlib);
	if(ret == -1){
		UFTP_CloseServer(&testlib);
		return -1;
	}
		
	ret = UFTP_STATFile(&testlib,internal_path,st);
	if(ret<0){
		UFTP_CloseServer(&testlib);
		return -1;
	
	}
	UFTP_CloseServer(&testlib);
	return 0;
}


int CFTPFS::ftpfs_chdir(struct _reent *r, const char *name) {
    auto *priv = static_cast<CFTPFS *>(r->deviceData);

    if (!name) {
        __errno_r(r) = EINVAL;
        return -1;
    }

    priv->cwd = name;

    return 0;
}

DIR_ITER *CFTPFS::ftpfs_diropen(struct _reent *r, DIR_ITER *dirState, const char *path) {
    auto *priv     = static_cast<CFTPFS    *>(r->deviceData);
    auto *priv_dir = static_cast<CFTPFSDir *>(dirState->dirStruct);

    auto internal_path = priv->translate_path(path);
    
	
	
	auto lk = std::scoped_lock(priv->session_mutex);
	priv_dir->diridx=0;
	priv->cachedirlist.clear();
    
	
	server_connection testdirlist;
	
	UFTP_Init_Server_Connection(&testdirlist,priv->host,priv->port,priv->user,priv->pass);
	int ret = UFTP_ServerConnect(&testdirlist);
	if(ret<0){
		UFTP_CloseServer(&testdirlist);
		return NULL;
	}
	ret = UFTP_OpenDirListing(&testdirlist,internal_path.data());
	if(ret<0){
		UFTP_CloseServer(&testdirlist);
		return NULL;
	}
	char buf[8192];
	memset(buf,0,sizeof(buf));
	while(UFTP_DirReadline(testdirlist.data_socket,buf) > 0){
		dircache entry;
		if(UFTP_ParseDirLine2(buf,entry.name,&entry.st)>0){
			entry.fullpathname =  internal_path + entry.name;
			priv->cachedirlist.push_back(entry);
		}
		/*
		if(UFTP_ParseDirLine(buf,entry.name,&entry.st)>0){
			entry.fullpathname =  internal_path + entry.name;
			priv->cachedirlist.push_back(entry);
		}
		*/
		
		
		
		memset(buf,0,8192);
	}
	UFTP_CloseServer(&testdirlist);

    return dirState;
}

int CFTPFS::ftpfs_dirreset(struct _reent *r, DIR_ITER *dirState) {
    __errno_r(r) = ENOSYS;
    return -1;
}

int CFTPFS::ftpfs_dirnext(struct _reent *r, DIR_ITER *dirState, char *filename, struct stat *filestat) {
    auto *priv     = static_cast<CFTPFS    *>(r->deviceData);
    auto *priv_dir = static_cast<CFTPFSDir *>(dirState->dirStruct);

    auto lk = std::scoped_lock(priv->session_mutex);
	
	if(priv_dir->diridx >= priv->cachedirlist.size()){
		return -1;
	}
	
	memset(filename,0,NAME_MAX);
	memcpy(filename,priv->cachedirlist[priv_dir->diridx].name.c_str(),priv->cachedirlist[priv_dir->diridx].name.length());
    memcpy(filestat,&priv->cachedirlist[priv_dir->diridx].st,sizeof(struct stat));
	
	priv_dir->diridx++;
	return 0;
}

int CFTPFS::ftpfs_dirclose(struct _reent *r, DIR_ITER *dirState) {
    auto *priv     = static_cast<CFTPFS    *>(r->deviceData);
    auto *priv_dir = static_cast<CFTPFSDir *>(dirState->dirStruct);

    auto lk = std::scoped_lock(priv->session_mutex);
	priv_dir->diridx = 0;
	return 0;
}

int CFTPFS::ftpfs_statvfs(struct _reent *r, const char *path, struct statvfs *buf) {
    auto *priv = static_cast<CFTPFS *>(r->deviceData);

    auto internal_path = priv->translate_path(path);
    if (internal_path.empty()) {
        __errno_r(r) = EINVAL;
        return -1;
    }

    auto lk = std::scoped_lock(priv->session_mutex);

    

    return 0;
}
