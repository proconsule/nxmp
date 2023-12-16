/* based on https://gist.github.com/averne/527f3a739d19c8d573b2d6995a33edea */


#include "nfsfs.h"
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/syslimits.h>
#include <regex>



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

	//stat_entry(&nfsst,st);

    
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
    priv->stat_entry(&priv_file->filestat, st);
    return 0;
}

int CNFSFS::nfsfs_stat(struct _reent *r, const char *file, struct stat *st) {
    auto *priv = static_cast<CNFSFS *>(r->deviceData);
	//printf("STAT: %s\n",file);
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

    priv->stat_entry(&nfsfilestat, st);
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
    
	auto lk = std::scoped_lock(priv->session_mutex);

	int ret = nfs_opendir(priv->nfs, internal_path.data(), &priv_dir->nfsdir);

	if (ret!=0) {
        return nullptr;
    }

    return dirState;
}

int CNFSFS::nfsfs_dirreset(struct _reent *r, DIR_ITER *dirState) {
    __errno_r(r) = ENOSYS;
    return -1;
}

int CNFSFS::nfsfs_dirnext(struct _reent *r, DIR_ITER *dirState, char *filename, struct stat *filestat) {
    auto *priv     = static_cast<CNFSFS    *>(r->deviceData);
    auto *priv_dir = static_cast<CNFSFSDir *>(dirState->dirStruct);

    auto lk = std::scoped_lock(priv->session_mutex);

	struct nfsdirent *mynfsdirent;
	while(true) {
		mynfsdirent = nfs_readdir(priv->nfs, priv_dir->nfsdir);
		if(mynfsdirent == NULL)return -1;
		
		*filestat = {};
		filestat->st_size = mynfsdirent->size;
		filestat->st_mode = mynfsdirent->mode;
		filestat->st_mtime = mynfsdirent->mtime.tv_sec;
		
		memset(filename,0,NAME_MAX);
		memcpy(filename,mynfsdirent->name,NAME_MAX);
		auto fname = std::string(filename);
        if (fname != "." && fname != "..")
        break;
	}

    return 0;
}

int CNFSFS::nfsfs_dirclose(struct _reent *r, DIR_ITER *dirState) {
    auto *priv     = static_cast<CNFSFS    *>(r->deviceData);
    auto *priv_dir = static_cast<CNFSFSDir *>(dirState->dirStruct);

    auto lk = std::scoped_lock(priv->session_mutex);
	nfs_closedir(priv->nfs,priv_dir->nfsdir);
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

    struct statvfs svfs = {};
	auto rc = nfs_statvfs(priv->nfs,internal_path.data(),&svfs);
	if (rc) {
        return -1;
    }

    *buf = {};
    buf->f_bsize   = svfs.f_bsize;
    buf->f_frsize  = svfs.f_frsize;
    buf->f_blocks  = svfs.f_blocks;
    buf->f_bfree   = svfs.f_bfree;
    buf->f_bavail  = svfs.f_bavail;
    buf->f_files   = svfs.f_files;
    buf->f_ffree   = svfs.f_ffree;
    buf->f_favail  = svfs.f_favail;
    buf->f_fsid    = svfs.f_fsid;
    buf->f_flag    = svfs.f_flag;
    buf->f_namemax = svfs.f_namemax;

    return 0;
}

void CNFSFS::stat_entry(nfs_stat_64  *entry, struct stat *st)
{
	*st = {};
	
	switch (entry->nfs_mode & S_IFMT) {
		case S_IFREG:
			st->st_mode = S_IFREG;
			break;
		case S_IFDIR:
			st->st_mode = S_IFDIR;
			break;
	}
	
	
	st->st_nlink = entry->nfs_nlink;
	st->st_uid = 1;
	st->st_gid = 2;
	st->st_size = entry->nfs_size;
	st->st_atime = entry->nfs_atime;
	st->st_mtime = entry->nfs_mtime;
	st->st_ctime = entry->nfs_ctime;
	
}
