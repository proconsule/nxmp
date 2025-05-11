/* based on https://github.com/averne/SwitchWave/blob/master/src/fs/fs_nfs.cpp */


#include "nfsfs.h"
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/syslimits.h>
#include <regex>

void nfsstat_entry(nfs_stat_64  &entry, struct stat *st);

CNFSFS::CNFSFS(std::string _url,std::string _name,std::string _mount_name){
	this->connect_url = _url;
	
	
	this->name       = _name;
    this->mount_name = _mount_name;

    this->devoptab = {
        .name         = CNFSFS::name.data(),

        .structSize   = sizeof(CNFSFSFile),
        .open_r       = CNFSFS::nfsfs_open,
        .close_r      = CNFSFS::nfsfs_close,
        .read_r       = CNFSFS::nfsfs_read,
        .seek_r       = CNFSFS::nfsfs_seek,
        .fstat_r      = CNFSFS::nfsfs_fstat,

        .stat_r       = CNFSFS::nfsfs_stat,
        .chdir_r      = CNFSFS::nfsfs_chdir,

        .dirStateSize = sizeof(CNFSFSDir),
        .diropen_r    = CNFSFS::nfsfs_diropen,
        .dirreset_r   = CNFSFS::nfsfs_dirreset,
        .dirnext_r    = CNFSFS::nfsfs_dirnext,
        .dirclose_r   = CNFSFS::nfsfs_dirclose,

        .statvfs_r    = CNFSFS::nfsfs_statvfs,

        .deviceData   = this,

        .lstat_r      = CNFSFS::nfsfs_stat,
    };
	
	
}

CNFSFS::CNFSFS(std::string _server,std::string _exportpath,std::string _name,std::string _mount_name){
	
	this->name       = _name;
    this->mount_name = _mount_name;
	this->server = _server;
	this->exportpath = _exportpath;


    this->devoptab = {
        .name         = CNFSFS::name.data(),

        .structSize   = sizeof(CNFSFSFile),
        .open_r       = CNFSFS::nfsfs_open,
        .close_r      = CNFSFS::nfsfs_close,
        .read_r       = CNFSFS::nfsfs_read,
        .seek_r       = CNFSFS::nfsfs_seek,
        .fstat_r      = CNFSFS::nfsfs_fstat,

        .stat_r       = CNFSFS::nfsfs_stat,
        .chdir_r      = CNFSFS::nfsfs_chdir,

        .dirStateSize = sizeof(CNFSFSDir),
        .diropen_r    = CNFSFS::nfsfs_diropen,
        .dirreset_r   = CNFSFS::nfsfs_dirreset,
        .dirnext_r    = CNFSFS::nfsfs_dirnext,
        .dirclose_r   = CNFSFS::nfsfs_dirclose,

        .statvfs_r    = CNFSFS::nfsfs_statvfs,

        .deviceData   = this,

        .lstat_r      = CNFSFS::nfsfs_stat,
    };
	
}

bool CNFSFS::CheckConnection(){
	if(connect() == 0){
		this->cwd = "/";
		return true;
	}
	return false;
}

bool CNFSFS::RegisterFilesystem(){
	if(connect() == 0){
		this->cwd = "/";
		fs_regisered = true;
		register_fs();
		return true;
	}
	return false;
}

bool CNFSFS::RegisterFilesystem_v2(){
	if(connect_v2() == 0){
		this->cwd = "/";
		fs_regisered = true;
		register_fs();
		return true;
	}
	return false;
}


bool fs_regisered = false;

CNFSFS::~CNFSFS(){
	if (this->is_connected)
        this->disconnect();

    if (this->nfs)
        nfs_destroy_context(nfs);
		nfs_destroy_url(nfsurl);
    
	
	if(fs_regisered){
		unregister_fs();
	}
}

void CNFSFS::disconnect(){
	
    auto lk = std::scoped_lock(this->session_mutex);

    
    this->is_connected = false;
}
std::string CNFSFS::translate_path(const char *path){
   return this->cwd + (path + CNFSFS::mount_name.length()+1);
}

int CNFSFS::connect(){
	
	nfs = nfs_init_context();
	if (nfs == NULL) {
		printf("failed to init context\n");
		return -1;
	}

	nfsurl = nfs_parse_url_full(nfs, connect_url.c_str());
	
    
	if (nfs_mount(nfs, nfsurl->server, nfsurl->path) != 0) {
 		printf("Failed to mount nfs share : %s\n", nfs_get_error(nfs));
		if (nfs != NULL) {
			nfs_destroy_context(nfs);
		}
		return -1;
	}

    this->is_connected = true;

    return 0;
}

int CNFSFS::connect_v2(){
	
	nfs = nfs_init_context();
	if (nfs == NULL) {
		printf("failed to init context\n");
		return -1;
	}

	if (nfs_mount(nfs, this->server.c_str(), this->exportpath.c_str()) != 0) {
 		printf("Failed to mount nfs share : %s\n", nfs_get_error(nfs));
		if (nfs != NULL) {
			nfs_destroy_context(nfs);
		}
		return -1;
	}

    this->is_connected = true;

    return 0;
}


int CNFSFS::nfsfs_open(struct _reent *r, void *fileStruct, const char *path, int flags, int mode) {
    auto *priv      = static_cast<CNFSFS     *>(r->deviceData);
    auto *priv_file = static_cast<CNFSFSFile *>(fileStruct);

    auto internal_path = priv->translate_path(path);
	if (internal_path.empty()) {
       __errno_r(r) = EINVAL;
        return -1;
    }

    auto lk = std::scoped_lock(priv->session_mutex);

	if (nfs_open(priv->nfs, internal_path.data(), O_RDONLY,&priv_file->nfsfh) != 0) {
			printf("Failed to open file %s: %s\n",
				       path,
				       nfs_get_error(priv->nfs));
					   
			 return -1;
	}
	if (nfs_fstat64(priv->nfs, priv_file->nfsfh, &priv_file->filestat) < 0) {
		printf("Failed to stat %s\n", internal_path.c_str());
		return -1;
	}

    priv_file->offset = 0;

    return 0;
}

int CNFSFS::nfsfs_close(struct _reent *r, void *fd) {
    auto *priv      = static_cast<CNFSFS     *>(r->deviceData);
    auto *priv_file = static_cast<CNFSFSFile *>(fd);

    auto lk = std::scoped_lock(priv->session_mutex);

    auto rc = nfs_close(priv->nfs,priv_file->nfsfh);
    if (rc) {
        return -1;
    }

    return 0;
}

ssize_t CNFSFS::nfsfs_read(struct _reent *r, void *fd, char *ptr, size_t len) {
    auto *priv      = static_cast<CNFSFS     *>(r->deviceData);
    auto *priv_file = static_cast<CNFSFSFile *>(fd);

    auto lk = std::scoped_lock(priv->session_mutex);

    auto rc = nfs_read(priv->nfs,priv_file->nfsfh, (uint64_t)len, ptr);
    if (rc < 0) {
        return rc;
    }

    priv_file->offset += rc;

    return rc;
}

off_t CNFSFS::nfsfs_seek(struct _reent *r, void *fd, off_t pos, int dir) {
    auto *priv      = static_cast<CNFSFS     *>(r->deviceData);
    auto *priv_file = static_cast<CNFSFSFile *>(fd);

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
            offset = priv_file->filestat.nfs_size;
            break;
    }

    priv_file->offset = offset + pos;

    auto lk = std::scoped_lock(priv->session_mutex);

    nfs_lseek(priv->nfs,priv_file->nfsfh, priv_file->offset,SEEK_SET,NULL);
    return priv_file->offset;
}

int CNFSFS::nfsfs_fstat(struct _reent *r, void *fd, struct stat *st) {
    auto *priv = static_cast<CNFSFS *>(r->deviceData);
	auto *priv_file = static_cast<CNFSFSFile *>(fd);
	
	auto lk = std::scoped_lock(priv->session_mutex);
    nfsstat_entry(priv_file->filestat, st);
    return 0;
}

int CNFSFS::nfsfs_stat(struct _reent *r, const char *file, struct stat *st) {
    auto *priv = static_cast<CNFSFS *>(r->deviceData);
	
	auto internal_path = priv->translate_path(file);
    if (internal_path.empty()) {
        __errno_r(r) = EINVAL;
        return -1;
    }

    auto lk = std::scoped_lock(priv->session_mutex);

	nfs_stat_64 nfsfilestat;
	
	int rc = nfs_stat64(priv->nfs,internal_path.data(),&nfsfilestat);
	
   
	if (rc!=0) {
        return -1;
    }

    nfsstat_entry(nfsfilestat, st);
    return 0;
}


int CNFSFS::nfsfs_chdir(struct _reent *r, const char *name) {
    auto *priv = static_cast<CNFSFS *>(r->deviceData);

    if (!name) {
        __errno_r(r) = EINVAL;
        return -1;
    }

    priv->cwd = name;

    return 0;
}

DIR_ITER *CNFSFS::nfsfs_diropen(struct _reent *r, DIR_ITER *dirState, const char *path) {
    
	auto *priv     = static_cast<CNFSFS    *>(r->deviceData);
    auto *priv_dir = static_cast<CNFSFSDir *>(dirState->dirStruct);

    auto internal_path = priv->translate_path(path);
    if (internal_path.empty()) {
        __errno_r(r) = EINVAL;
        return nullptr;
    }

    auto lk = std::scoped_lock(priv->session_mutex);

    auto rc = ::nfs_opendir(priv->nfs, internal_path.data(), &priv_dir->handle);
    if (!priv_dir->handle) {
        __errno_r(r) = -rc;
        return nullptr;
    }

    return dirState;
	
}

int CNFSFS::nfsfs_dirreset(struct _reent *r, DIR_ITER *dirState) {
    auto *priv     = static_cast<CNFSFS *>(r->deviceData);
    auto *priv_dir = static_cast<CNFSFSDir *>(dirState->dirStruct);

    nfs_rewinddir(priv->nfs, priv_dir->handle);
	return 0;
	
}

int CNFSFS::nfsfs_dirnext(struct _reent *r, DIR_ITER *dirState, char *filename, struct stat *filestat) {
    
	auto *priv     = static_cast<CNFSFS *>(r->deviceData);
    auto *priv_dir = static_cast<CNFSFSDir *>(dirState->dirStruct);

    struct nfsdirent *node;
    while (true) {
        node = ::nfs_readdir(priv->nfs, priv_dir->handle);
        if (!node) {
            __errno_r(r) = ENOENT;
            return -1;
        }

        auto fname = std::string_view(node->name);
        if (fname != "." && fname != "..")
            break;
    }

    std::strncpy(filename, node->name, NAME_MAX);

    *filestat = {
        .st_mode     = mode_t(node->mode),
        .st_uid      =  uid_t(node->uid),
        .st_gid      =  gid_t(node->gid),
        .st_size     =  off_t(node->size),
        .st_atim     = {
            .tv_sec  = node->atime.tv_sec,
            .tv_nsec = node->atime_nsec,
        },
        .st_mtim = {
            .tv_sec  = node->mtime.tv_sec,
            .tv_nsec = node->mtime_nsec,
        },
        .st_ctim = {
            .tv_sec  = node->ctime.tv_sec,
            .tv_nsec = node->ctime_nsec,
        },
    };

    return 0;

}

int CNFSFS::nfsfs_dirclose(struct _reent *r, DIR_ITER *dirState) {
    auto *priv     = static_cast<CNFSFS *>(r->deviceData);
    auto *priv_dir = static_cast<CNFSFSDir *>(dirState->dirStruct);

    nfs_closedir(priv->nfs, priv_dir->handle);
    return 0;
}

int CNFSFS::nfsfs_statvfs(struct _reent *r, const char *path, struct statvfs *buf) {
     auto *priv = static_cast<CNFSFS *>(r->deviceData);

    auto internal_path = priv->translate_path(path);
    if (internal_path.empty()) {
        __errno_r(r) = EINVAL;
        return -1;
    }

    auto lk = std::scoped_lock(priv->session_mutex);

    if (auto rc = ::nfs_statvfs(priv->nfs, internal_path.data(), buf); rc < 0) {
        __errno_r(r) = -rc;
        return -1;
    }

	return 0;
}

void nfsstat_entry(nfs_stat_64  &entry, struct stat *st)
{
	
	
	*st = {
        .st_mode     = mode_t(entry.nfs_mode),
        .st_uid      =  uid_t(entry.nfs_uid),
        .st_gid      =  gid_t(entry.nfs_gid),
        .st_size     =  off_t(entry.nfs_size),
        .st_atim     = {
            .tv_sec  = long(entry.nfs_atime),
            .tv_nsec = long(entry.nfs_atime_nsec),
        },
        .st_mtim = {
            .tv_sec  = long(entry.nfs_mtime),
            .tv_nsec = long(entry.nfs_mtime_nsec),
        },
        .st_ctim = {
            .tv_sec  = long(entry.nfs_ctime),
            .tv_nsec = long(entry.nfs_ctime_nsec),
        },
    };
	
	
}
