/* based on https://gist.github.com/averne/527f3a739d19c8d573b2d6995a33edea */


#include "smb2fs.h"

void smb2stat_entry(smb2_stat_64  *entry, struct stat *st);

CSMB2FS::CSMB2FS(std::string _server,std::string _username,std::string _password,std::string _path,std::string _name,std::string _mount_name){
	
	name       = _name;
    mount_name = _mount_name;
	
	server = _server;
	username = _username;
	password = _password;
	path = _path;

    devoptab = {
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


CSMB2FS::CSMB2FS(std::string _url,std::string _name,std::string _mount_name){
	
	
	
	connect_url = _url;
	
	SMB2_PARSER = new CSMB2_PARSER(connect_url);
	
	name       = _name;
    mount_name = _mount_name;
	

    devoptab = {
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

bool CSMB2FS::RegisterFilesystem_v2(){
	if(connect_v2() == 0){
		cwd = "/";
		fs_regisered = true;
		register_fs();
		return true;
	}
	return false;
}

bool CSMB2FS::RegisterFilesystem(){
	if(connect() == 0){
		cwd = "/";
		fs_regisered = true;
		register_fs();
		return true;
	}
	return false;
}


CSMB2FS::~CSMB2FS(){
	auto lk = std::scoped_lock(this->session_mutex);
	if (this->is_connected)
        this->disconnect();
	
	if(fs_regisered){
		unregister_fs();
	}
}

void CSMB2FS::disconnect(){
	if(smb2!=nullptr){
		smb2_disconnect_share(smb2);
	}
	if(smb2url!=nullptr)
		smb2_destroy_url(smb2url);
	if(smb2!=nullptr){
		
		smb2_destroy_context(smb2);
	}
}


int CSMB2FS::connect_v2(){
	smb2 = smb2_init_context();
	if (smb2 == NULL) {
		printf("SMB2 Failed to init context\n");
		return -1;
	
	}
	smb2_set_user(smb2,username.c_str()); 
	smb2_set_password(smb2,password.c_str()); 
	smb2_set_security_mode(smb2, SMB2_NEGOTIATE_SIGNING_ENABLED);
	
	
	
	if (smb2_connect_share(smb2, server.c_str(), path.c_str(), username.c_str()) < 0) {
		printf("smb2_connect_share failed. %s\n", smb2_get_error(smb2));
		return -1;
	}
	
	
	maxreadsize = smb2_get_max_read_size(smb2);
	
	is_connected = true;
	
	return 0;
}

int CSMB2FS::connect(){
	smb2 = smb2_init_context();
	if (smb2 == NULL) {
		printf("SMB2 Failed to init context\n");
		return -1;
	
	}
	
	smb2_set_user(smb2,SMB2_PARSER->user.c_str()); 
	smb2_set_password(smb2,SMB2_PARSER->pass.c_str()); 
	smb2_set_security_mode(smb2, SMB2_NEGOTIATE_SIGNING_ENABLED);
	
	if (smb2_connect_share(smb2, SMB2_PARSER->server.c_str(), SMB2_PARSER->share.c_str(), SMB2_PARSER->user.c_str()) < 0) {
		printf("smb2_connect_share failed. %s\n", smb2_get_error(smb2));
		return -1;
	}
	
	
	maxreadsize = smb2_get_max_read_size(smb2);
	
	is_connected = true;
	
	return 0;
}


std::string CSMB2FS::translate_path(const char *path){
   return this->cwd + (path + CSMB2FS::mount_name.length()+1);
}


int       CSMB2FS::smb2fs_open     (struct _reent *r, void *fileStruct, const char *path, int flags, int mode){
	auto *priv      = static_cast<CSMB2FS     *>(r->deviceData);
    auto *priv_file = static_cast<CSMB2FSFile *>(fileStruct);

	auto internal_path = priv->translate_path(path);
	if (internal_path.empty()) {
        __errno_r(r) = EINVAL;
        return -1;
    }

    auto lk = std::scoped_lock(priv->session_mutex);
	
	priv_file->fh = smb2_open(priv->smb2, internal_path.c_str()+1, O_RDONLY);
    if (!priv_file->fh) {
        return -1;
    }


	auto rc = smb2_stat(priv->smb2,internal_path.c_str()+1,&priv_file->smb2st);
	
    
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
	auto rc = smb2_pread(priv->smb2,priv_file->fh,(uint8_t *)ptr,len,priv_file->offset);
	
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

    auto lk = std::scoped_lock(priv->session_mutex);
	priv_file->offset = offset + pos;
	
	return priv_file->offset;
}

int       CSMB2FS::smb2fs_fstat    (struct _reent *r, void *fd, struct stat *st){
	auto *priv_file = static_cast<CSMB2FSFile *>(fd);
    smb2stat_entry(&priv_file->smb2st, st);
    return 0;
}

int       CSMB2FS::smb2fs_stat     (struct _reent *r, const char *file, struct stat *st){
	auto *priv     = static_cast<CSMB2FS    *>(r->deviceData);
	auto internal_path = priv->translate_path(file);
	if (internal_path.empty()) {
        __errno_r(r) = EINVAL;
        return -1;
    }
	
	for(int i=0;i<priv->cachedirlist.size();i++){
		if(internal_path == priv->cachedirlist[i].fullpathname){
			memcpy(st,&priv->cachedirlist[i].st,sizeof(struct stat));
			return 0;
		}
	}
	
	struct smb2_stat_64 smb2st;
	auto lk = std::scoped_lock(priv->session_mutex);
	int rc = smb2_stat(priv->smb2, internal_path.c_str()+1, &smb2st);
	if(rc < 0) {
		return rc;
	}
	
	smb2stat_entry(&smb2st,st);
	
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
	
	auto internal_path = priv->translate_path(path);
	
	auto lk = std::scoped_lock(priv->session_mutex);
	
	priv->cachedirlist.clear();
	struct smb2dir *dir = smb2_opendir(priv->smb2, internal_path.c_str()+1);
	if(!dir){
		return nullptr;
	}
	 struct smb2dirent *ent;
	while ((ent = smb2_readdir(priv->smb2, dir))) {
		dircache entry;
		
		entry.name = ent->name;
		entry.fullpathname =  internal_path + entry.name;
		
		smb2stat_entry(&ent->st,&entry.st);
		priv->cachedirlist.push_back(entry);
	}
	priv_dir->diridx = 0;
	smb2_closedir(priv->smb2,dir);
	
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
	
	if(priv_dir->diridx >= priv->cachedirlist.size()){
		priv_dir->diridx = 0;
		return -1;
	}
	
	memset(filename,0,NAME_MAX);
	memcpy(filename,priv->cachedirlist[priv_dir->diridx].name.c_str(),priv->cachedirlist[priv_dir->diridx].name.length());
    memcpy(filestat,&priv->cachedirlist[priv_dir->diridx].st,sizeof(struct stat));
	
	priv_dir->diridx++;
	
		
	
	return 0;
}

int       CSMB2FS::smb2fs_dirclose (struct _reent *r, DIR_ITER *dirState){
	auto *priv     = static_cast<CSMB2FS    *>(r->deviceData);
    
	auto lk = std::scoped_lock(priv->session_mutex);
	
	
	
	return 0;
}

int       CSMB2FS::smb2fs_statvfs  (struct _reent *r, const char *path, struct statvfs *buf){
	auto *priv     = static_cast<CSMB2FS    *>(r->deviceData);
	struct smb2_statvfs vfs;
	auto internal_path = priv->translate_path(path);
	if (internal_path.empty()) {
        __errno_r(r) = EINVAL;
        return -1;
    }
	
	auto lk = std::scoped_lock(priv->session_mutex);
	if (smb2_statvfs(priv->smb2, internal_path.c_str(), &vfs) < 0) {
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
	auto internal_path = priv->translate_path(file);
	if (internal_path.empty()) {
        __errno_r(r) = EINVAL;
        return -1;
    }
	struct smb2_stat_64 smb2st;
	auto lk = std::scoped_lock(priv->session_mutex);
	if (smb2_stat(priv->smb2, internal_path.c_str()+1, &smb2st) < 0) {
		return -1;
	}
	
	smb2stat_entry(&smb2st,st);
	
	return 0;
}


void smb2stat_entry(smb2_stat_64  *entry, struct stat *st)
{
	*st = {};
	
	st->st_mode =  entry->smb2_type == 0 ? S_IFREG : S_IFDIR;
	st->st_nlink = 1;
	st->st_uid = 1;
	st->st_gid = 2;
	st->st_size = entry->smb2_size;
	st->st_atime = entry->smb2_atime;
	st->st_mtime = entry->smb2_mtime;
	st->st_ctime = entry->smb2_ctime;
	
}
	