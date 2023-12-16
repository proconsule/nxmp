/* based on https://gist.github.com/averne/527f3a739d19c8d573b2d6995a33edea */


#include "smb2fs.h"

CSMB2FS::CSMB2FS(std::string _url,std::string _name,std::string _mount_name){
	this->connect_url = _url;
	
	
	this->name       = _name;
    this->mount_name = _mount_name;

    this->devoptab = {
        .name         = CSMB2FS::name.data(),

        .structSize   = sizeof(CSMB2FSFile),
        .open_r       = CSMB2FS::smb2fs_open,
        .close_r      = CSMB2FS::smb2fs_close,
        .read_r       = CSMB2FS::smb2fs_read,
        .seek_r       = CSMB2FS::smb2fs_seek,
        .fstat_r      = CSMB2FS::smb2fs_fstat,

        .stat_r       = CSMB2FS::smb2fs_stat,
        .chdir_r      = CSMB2FS::smb2fs_chdir,

        .dirStateSize = sizeof(CSMB2FSDir),
        .diropen_r    = CSMB2FS::smb2fs_diropen,
        .dirreset_r   = CSMB2FS::smb2fs_dirreset,
        .dirnext_r    = CSMB2FS::smb2fs_dirnext,
        .dirclose_r   = CSMB2FS::smb2fs_dirclose,

        .statvfs_r    = CSMB2FS::smb2fs_statvfs,

        .deviceData   = this,

        .lstat_r      = CSMB2FS::smb2fs_lstat,
    };
	
	
}

bool CSMB2FS::CheckConnection(){
	if(connect() == 0){
		
		return true;
	}
	
	return false;
	
}

bool CSMB2FS::RegisterFilesystem(){
	if(connect() == 0){
		fs_regisered = true;
		register_fs();
	}
}


CSMB2FS::~CSMB2FS(){
	printf("DECOSTRUCTOR\n");
	auto lk = std::scoped_lock(this->session_mutex);
	if (this->is_connected)
        this->disconnect();
	
	if(fs_regisered){
		unregister_fs();
	}
}

void CSMB2FS::disconnect(){
	if(smb2url!=nullptr)
		smb2_destroy_url(smb2url);
	if(smb2!=nullptr){
		smb2_disconnect_share(smb2);
		smb2_destroy_context(smb2);
	}
}


int CSMB2FS::connect(){
	smb2 = smb2_init_context();
	if (smb2 == NULL) {
		printf("SMB2 Failed to init context\n");
		return -1;
	
	}
	
	smb2url = smb2_parse_url(smb2,connect_url.c_str());
	
	printf("user: %s\n",smb2url->user);
	printf("pass: %s\n",smb2url->pass);
	fflush(stdout);
	if(smb2url->user != NULL){
		smb2_set_user(smb2,smb2url->user); 
	}else{
		smb2_set_user(smb2,"Guest"); 
	}
	//token = strtok(NULL, search);
	if(smb2url->pass != NULL){
		smb2_set_password(smb2,smb2url->pass); 
	}
	smb2_set_security_mode(smb2, SMB2_NEGOTIATE_SIGNING_ENABLED);
	
	
	if (smb2_connect_share(smb2, smb2url->server, smb2url->share, NULL) < 0) {
		printf("smb2_connect_share failed. %s\n", smb2_get_error(smb2));
		return -1;
	}
	
	maxreadsize = smb2_get_max_read_size(smb2);
	
	is_connected = true;
	
	return 0;
}


int       CSMB2FS::smb2fs_open     (struct _reent *r, void *fileStruct, const char *path, int flags, int mode){
	auto *priv      = static_cast<CSMB2FS     *>(r->deviceData);
    auto *priv_file = static_cast<CSMB2FSFile *>(fileStruct);

	if(std::string(path).empty()){
		return -1;
	}

    char* colonPos = strchr(path, ':');
    if (colonPos) path = colonPos+1;

    auto lk = std::scoped_lock(priv->session_mutex);
	priv_file->fh = smb2_open(priv->smb2, path+1, O_RDONLY);
    if (!priv_file->fh) {
        return -1;
    }


	auto rc = smb2_stat(priv->smb2,path+1,&priv_file->smb2st);
	
    
    if (rc) {
        return -1;
    }

    priv_file->offset = 0;

    return 0;
	
}


int       CSMB2FS::smb2fs_close    (struct _reent *r, void *fd){
	auto *priv      = static_cast<CSMB2FS     *>(r->deviceData);
    auto *priv_file = static_cast<CSMB2FSFile *>(fd);

    auto lk = std::scoped_lock(priv->session_mutex);

    auto rc = smb2_close(priv->smb2,priv_file->fh);
    if (rc) {
        
		return -1;
    }

    return 0;
}

ssize_t   CSMB2FS::smb2fs_read     (struct _reent *r, void *fd, char *ptr, size_t len){
	auto *priv      = static_cast<CSMB2FS     *>(r->deviceData);
    auto *priv_file = static_cast<CSMB2FSFile *>(fd);

    auto lk = std::scoped_lock(priv->session_mutex);

    auto rc = smb2_read(priv->smb2,priv_file->fh,(uint8_t *)ptr,len);
    if (rc < 0) {
        return -1;
    }

    priv_file->offset += rc;
	
	return rc;
}


off_t     CSMB2FS::smb2fs_seek     (struct _reent *r, void *fd, off_t pos, int dir){
	auto *priv      = static_cast<CSMB2FS     *>(r->deviceData);
    auto *priv_file = static_cast<CSMB2FSFile *>(fd);

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
            offset = priv_file->smb2st.smb2_size;
            break;
    }

    priv_file->offset = offset + pos;

    auto lk = std::scoped_lock(priv->session_mutex);
	
	smb2_lseek(priv->smb2, priv_file->fh, priv_file->offset, SEEK_SET, NULL);
    return priv_file->offset;
}

int       CSMB2FS::smb2fs_fstat    (struct _reent *r, void *fd, struct stat *st){
	auto *ctx = static_cast<CSMB2FS *>(r->deviceData);
	auto *priv_file = static_cast<CSMB2FSFile *>(fd);
    ctx->stat_entry(&priv_file->smb2st, st);
    return 0;
}

int       CSMB2FS::smb2fs_stat     (struct _reent *r, const char *file, struct stat *st){
	auto *priv     = static_cast<CSMB2FS    *>(r->deviceData);
	char* colonPos = strchr(file, ':');
    if (colonPos) file = colonPos+1;
	struct smb2_stat_64 smb2st;
	auto lk = std::scoped_lock(priv->session_mutex);
	int rc = smb2_stat(priv->smb2, file+1, &smb2st);
	if(rc < 0) {
		return rc;
	}
	
	priv->stat_entry(&smb2st,st);
	
	return 0;
}
int       CSMB2FS::smb2fs_chdir    (struct _reent *r, const char *name){
	auto *priv = static_cast<CSMB2FS *>(r->deviceData);

    if (!name) {
        __errno_r(r) = EINVAL;
        return -1;
    }

    priv->cwd = name;

    return 0;
}

DIR_ITER * CSMB2FS::smb2fs_diropen  (struct _reent *r, DIR_ITER *dirState, const char *path){
	auto *priv = static_cast<CSMB2FS *>(r->deviceData);
	auto *priv_dir = static_cast<CSMB2FSDir *>(dirState->dirStruct);
	
	char* colonPos = strchr(path, ':');
    if (colonPos) path = colonPos+1;
	
	auto lk = std::scoped_lock(priv->session_mutex);
	
	priv_dir->dir = smb2_opendir(priv->smb2, path+1);
	if(!priv_dir->dir){
		return nullptr;
	}
	
	return dirState;
}

int       CSMB2FS::smb2fs_dirreset (struct _reent *r, DIR_ITER *dirState){
	__errno_r(r) = ENOSYS;
    return -1;
}

int       CSMB2FS::smb2fs_dirnext  (struct _reent *r, DIR_ITER *dirState, char *filename, struct stat *filestat){
	auto *priv     = static_cast<CSMB2FS    *>(r->deviceData);
    auto *priv_dir = static_cast<CSMB2FSDir *>(dirState->dirStruct);

    auto lk = std::scoped_lock(priv->session_mutex);
	
	
	struct smb2dirent *ent;
	while (true) {
		
		ent = smb2_readdir(priv->smb2, priv_dir->dir);
		if (ent == NULL) {
			__errno_r(r) = ENOENT;
			return -1;
		}
		auto fname = std::string(ent->name);
        
		
		memset(filename, 0, NAME_MAX);
		memcpy(filename, ent->name,NAME_MAX);
		
		if (fname != "." && fname != "..")
		break;
			
	}
	
	priv->stat_entry(&ent->st,filestat);
	
	
	return 0;
}

int       CSMB2FS::smb2fs_dirclose (struct _reent *r, DIR_ITER *dirState){
	auto *priv     = static_cast<CSMB2FS    *>(r->deviceData);
    auto *priv_dir = static_cast<CSMB2FSDir *>(dirState->dirStruct);
	
	auto lk = std::scoped_lock(priv->session_mutex);
	
	smb2_closedir(priv->smb2,priv_dir->dir);
	
	return 0;
}

int       CSMB2FS::smb2fs_statvfs  (struct _reent *r, const char *path, struct statvfs *buf){
	auto *priv     = static_cast<CSMB2FS    *>(r->deviceData);
	struct smb2_statvfs vfs;
	char* colonPos = strchr(path, ':');
    if (colonPos) path = colonPos+1;
	
	auto lk = std::scoped_lock(priv->session_mutex);
	if (smb2_statvfs(priv->smb2, path+1, &vfs) < 0) {
		return -1;
	}
	
	*buf = {};
    buf->f_bsize   = vfs.f_bsize;
    buf->f_frsize  = vfs.f_frsize;
    buf->f_blocks  = vfs.f_blocks;
    buf->f_bfree   = vfs.f_bfree;
    buf->f_bavail  = vfs.f_bavail;
    buf->f_files   = vfs.f_files;
    buf->f_ffree   = vfs.f_ffree;
    buf->f_favail  = vfs.f_favail;
    buf->f_fsid    = vfs.f_fsid;
    buf->f_flag    = vfs.f_flag;
    buf->f_namemax = vfs.f_namemax;
	
	return 0;
}


int       CSMB2FS::smb2fs_lstat    (struct _reent *r, const char *file, struct stat *st){
	auto *priv     = static_cast<CSMB2FS    *>(r->deviceData);
	char* colonPos = strchr(file, ':');
    if (colonPos) file = colonPos+1;
	struct smb2_stat_64 smb2st;
	auto lk = std::scoped_lock(priv->session_mutex);
	if (smb2_stat(priv->smb2, file+1, &smb2st) < 0) {
		return -1;
	}
	
	priv->stat_entry(&smb2st,st);
	
	return 0;
}


void CSMB2FS::stat_entry(smb2_stat_64  *entry, struct stat *st)
{
	*st = {};
	
	st->st_mode =  entry->smb2_type == 0 ? S_IFMT : S_IFDIR;
	st->st_nlink = 1;
	st->st_uid = 1;
	st->st_gid = 2;
	st->st_size = entry->smb2_size;
	st->st_atime = entry->smb2_atime;
	st->st_mtime = entry->smb2_mtime;
	st->st_ctime = entry->smb2_ctime;
	
}
	