/* based on https://gist.github.com/averne/527f3a739d19c8d573b2d6995a33edea */


#include "sshfs.h"
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/syslimits.h>
#include <regex>

int ssh2_translate_error(int error, LIBSSH2_SFTP *sftp_session) {
    switch (error) {
        default:
        case LIBSSH2_ERROR_NONE:
            return 0;
        case LIBSSH2_ERROR_ALLOC:
            return ENOMEM;
        case LIBSSH2_ERROR_SOCKET_SEND:
            return EIO;
        case LIBSSH2_ERROR_SOCKET_TIMEOUT:
            return ETIMEDOUT;
        case LIBSSH2_ERROR_EAGAIN:
            return EAGAIN;
        case LIBSSH2_ERROR_SFTP_PROTOCOL:
            switch (libssh2_sftp_last_error(sftp_session)) {
                default:
                case LIBSSH2_FX_OK:
                    return 0;
                case LIBSSH2_FX_EOF:
                case LIBSSH2_FX_FAILURE:
                case LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:
                case LIBSSH2_FX_QUOTA_EXCEEDED:
                case LIBSSH2_FX_UNKNOWN_PRINCIPAL:
                    return EIO;
                case LIBSSH2_FX_NO_SUCH_FILE:
                case LIBSSH2_FX_NO_SUCH_PATH:
                case LIBSSH2_FX_NO_MEDIA:
                    return ENOENT;
                case LIBSSH2_FX_PERMISSION_DENIED:
                case LIBSSH2_FX_WRITE_PROTECT:
                    return EPERM;
                case LIBSSH2_FX_BAD_MESSAGE:
                case LIBSSH2_FX_INVALID_HANDLE:
                case LIBSSH2_FX_INVALID_FILENAME:
                    return EINVAL;
                case LIBSSH2_FX_NO_CONNECTION:
                case LIBSSH2_FX_CONNECTION_LOST:
                    return ECONNRESET;
                case LIBSSH2_FX_OP_UNSUPPORTED:
                    return ENOTSUP;
                case LIBSSH2_FX_FILE_ALREADY_EXISTS:
                    return EEXIST;
                case LIBSSH2_FX_LOCK_CONFLICT:
                    return EDEADLK;
                case LIBSSH2_FX_DIR_NOT_EMPTY:
                    return ENOTEMPTY;
                case LIBSSH2_FX_NOT_A_DIRECTORY:
                    return ENOTDIR;
                case LIBSSH2_FX_LINK_LOOP:
                    return ELOOP;
            }
    }
}

int ssh2_translate_open_flags(int flags) {
    int ssh_flags = 0;

    switch (flags & O_ACCMODE) {
        default:
        case O_RDONLY:
            ssh_flags |= LIBSSH2_FXF_READ;
            break;
        case O_WRONLY:
            ssh_flags |= LIBSSH2_FXF_WRITE;
            break;
        case O_RDWR:
            ssh_flags |= LIBSSH2_FXF_READ | LIBSSH2_FXF_WRITE;
            break;
    }

    if (flags & O_CREAT)
        ssh_flags |= LIBSSH2_FXF_CREAT;

    if (flags & O_TRUNC)
        ssh_flags |= LIBSSH2_FXF_TRUNC;

    if (flags & O_EXCL)
        ssh_flags |= LIBSSH2_FXF_EXCL;

    return ssh_flags;
}

void ssh2_translate_stat(LIBSSH2_SFTP_ATTRIBUTES &attrs, struct stat *st) {
    *st = {};

    if (attrs.flags & LIBSSH2_SFTP_ATTR_SIZE)
        st->st_size = attrs.filesize;

    if (attrs.flags & LIBSSH2_SFTP_ATTR_UIDGID)
        st->st_uid = attrs.uid, st->st_gid = attrs.gid;

    if (attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS)
        st->st_mode = attrs.permissions;

    if (attrs.flags & LIBSSH2_SFTP_ATTR_ACMODTIME)
        st->st_atim.tv_sec = attrs.atime, st->st_mtim.tv_sec = attrs.mtime;

    st->st_nlink = 1;
}



CSSHFS::sshurlschema CSSHFS::parseSSHUrl(std::string url){
	sshurlschema res;
	std::regex rgx("(?:([^:]*)://)?(?:([^:@]*)(?::([^@]*))?@)?(?:([^/:]*))?(?::([0-9]*))?/(/[^?#]*(=.*?/)/)?([^?#]*)?(?:([^#]*))?(?:#(.*))?");
	std::smatch match;
	if(std::regex_search(url, match, rgx)) {
		res.scheme = match[1];
		res.user = match[2];
		res.pass = match[3];
		res.server = match[4];
		res.port = match[5];
		res.path = match[8];
		res.path = "/" + res.path;
	}
	return res;
}

CSSHFS::CSSHFS(std::string _url,std::string _name,std::string _mount_name){
	this->connect_url = _url;
	
	
	this->name       = _name;
    this->mount_name = _mount_name;

    this->devoptab = {
        .name         = CSSHFS::name.data(),

        .structSize   = sizeof(CSSHFSFile),
        .open_r       = CSSHFS::sshfs_open,
        .close_r      = CSSHFS::sshfs_close,
        .read_r       = CSSHFS::sshfs_read,
        .seek_r       = CSSHFS::sshfs_seek,
        .fstat_r      = CSSHFS::sshfs_fstat,

        .stat_r       = CSSHFS::sshfs_stat,
        .chdir_r      = CSSHFS::sshfs_chdir,

        .dirStateSize = sizeof(CSSHFSDir),
        .diropen_r    = CSSHFS::sshfs_diropen,
        .dirreset_r   = CSSHFS::sshfs_dirreset,
        .dirnext_r    = CSSHFS::sshfs_dirnext,
        .dirclose_r   = CSSHFS::sshfs_dirclose,

        .statvfs_r    = CSSHFS::sshfs_statvfs,

        .deviceData   = this,

        .lstat_r      = CSSHFS::sshfs_lstat,
    };
	this->urlschema = this->parseSSHUrl(connect_url);
	
}


CSSHFS::CSSHFS(std::string _server,int _port,std::string _username,std::string _password,std::string _path,std::string _name,std::string _mount_name){
	
	
	this->name       = _name;
    this->mount_name = _mount_name;
	
	this->server = _server;
	this->username = _username;
	this->password = _password;
	this->path = _path;
	this->port = _port;

    this->devoptab = {
        .name         = CSSHFS::name.data(),

        .structSize   = sizeof(CSSHFSFile),
        .open_r       = CSSHFS::sshfs_open,
        .close_r      = CSSHFS::sshfs_close,
        .read_r       = CSSHFS::sshfs_read,
        .seek_r       = CSSHFS::sshfs_seek,
        .fstat_r      = CSSHFS::sshfs_fstat,

        .stat_r       = CSSHFS::sshfs_stat,
        .chdir_r      = CSSHFS::sshfs_chdir,

        .dirStateSize = sizeof(CSSHFSDir),
        .diropen_r    = CSSHFS::sshfs_diropen,
        .dirreset_r   = CSSHFS::sshfs_dirreset,
        .dirnext_r    = CSSHFS::sshfs_dirnext,
        .dirclose_r   = CSSHFS::sshfs_dirclose,

        .statvfs_r    = CSSHFS::sshfs_statvfs,

        .deviceData   = this,

        .lstat_r      = CSSHFS::sshfs_lstat,
    };
	
	
}


CSSHFS::CSSHFS(std::string _server,int _port,std::string _username,std::string _pubkeypath,std::string _privkeypath,std::string _passphrase,std::string _path,std::string _name,std::string _mount_name){
	this->name       = _name;
    this->mount_name = _mount_name;
	
	this->server = _server;
	this->username = _username;
	this->pubkeypath = _pubkeypath;
	this->privkeypath = _privkeypath;
	this->passphrase = _passphrase;
	
	this->path = _path;
	if(_port == 0){
		this->port = 22;
	}else{
		this->port = _port;
	}

    this->devoptab = {
        .name         = CSSHFS::name.data(),

        .structSize   = sizeof(CSSHFSFile),
        .open_r       = CSSHFS::sshfs_open,
        .close_r      = CSSHFS::sshfs_close,
        .read_r       = CSSHFS::sshfs_read,
        .seek_r       = CSSHFS::sshfs_seek,
        .fstat_r      = CSSHFS::sshfs_fstat,

        .stat_r       = CSSHFS::sshfs_stat,
        .chdir_r      = CSSHFS::sshfs_chdir,

        .dirStateSize = sizeof(CSSHFSDir),
        .diropen_r    = CSSHFS::sshfs_diropen,
        .dirreset_r   = CSSHFS::sshfs_dirreset,
        .dirnext_r    = CSSHFS::sshfs_dirnext,
        .dirclose_r   = CSSHFS::sshfs_dirclose,

        .statvfs_r    = CSSHFS::sshfs_statvfs,

        .deviceData   = this,

        .lstat_r      = CSSHFS::sshfs_lstat,
    };
	
}
	



bool CSSHFS::CheckConnection(){
	if(connect(urlschema.server,atoi(urlschema.port.c_str()),urlschema.user,urlschema.pass) == 0){
		return true;
	}
	
	return false;
}

bool CSSHFS::RegisterFilesystem_v2(){
	
	if(connect(server,port,username,password) == 0){
		this->cwd = path;
		register_fs();
		this->fs_regisered = true;
		return true;
	}
	
	return false;
}

bool CSSHFS::RegisterFilesystem_pubkey_v2(){
	
	if(connect_pubkey(server,port,username,pubkeypath,privkeypath,passphrase) == 0){
		this->cwd = path;
		register_fs();
		this->fs_regisered = true;
		return true;
	}
	
	return false;
}

bool CSSHFS::RegisterFilesystem(){
	
	if(connect(urlschema.server,atoi(urlschema.port.c_str()),urlschema.user,urlschema.pass) == 0){
		this->cwd = urlschema.path;
		register_fs();
		this->fs_regisered = true;
		return true;
	}
	
	return false;
}

CSSHFS::~CSSHFS(){
	if (this->is_connected)
        this->disconnect();

    if (this->ssh_session)
        libssh2_session_free(this->ssh_session);

    libssh2_exit();
	if(fs_regisered){
		unregister_fs();
	}
}

void CSSHFS::disconnect(){
	int rc = 0;

    auto lk = std::scoped_lock(this->session_mutex);

    if (this->sftp_session)
        rc |= libssh2_sftp_shutdown(this->sftp_session);

    if (this->ssh_session)
        rc |= libssh2_session_disconnect(this->ssh_session, "Normal Shutdown");

    if (this->socket > 0)
        rc |= close(this->socket);

    this->is_connected = false;
}
std::string CSSHFS::translate_path(const char *path){
   return this->cwd + (path + CSSHFS::mount_name.length()+1);
}


int CSSHFS::connect_pubkey(std::string host, std::uint16_t port,
        std::string username, std::string pubkeypath,std::string privkeypath,std::string passphrase){
		
	
	if (auto rc = libssh2_init(0); rc)
        return rc;

    this->ssh_session = libssh2_session_init();
    if (!this->ssh_session){
        return -1;	
	}
	
	this->socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (this->socket < 0){
        return -1;
	}
	

    auto hostaddr = inet_addr(host.data());

    sockaddr_in sin = {
        .sin_family = AF_INET,
        .sin_port   = htons(port),
        .sin_addr   = {
            .s_addr = hostaddr,
        },
    };

	
    if (auto rc = ::connect(this->socket, reinterpret_cast<sockaddr *>(&sin), sizeof(sin)); rc)
        return errno;

	
    auto lk = std::scoped_lock(this->session_mutex);

	
    if (auto rc = libssh2_session_handshake(this->ssh_session, this->socket); rc){
        return ssh2_translate_error(rc, nullptr);
	}

	if (auto rc = libssh2_userauth_publickey_fromfile(this->ssh_session, username.data(), pubkeypath.data(),privkeypath.data(),""); rc){
        return ssh2_translate_error(rc, nullptr);
	}

    this->sftp_session = libssh2_sftp_init(this->ssh_session);
    if (!this->sftp_session)
        return ssh2_translate_error(libssh2_session_last_errno(this->ssh_session), this->sftp_session);

    libssh2_session_set_blocking(this->ssh_session, 1);

    this->is_connected = true;

    return 0;	
			
}


int CSSHFS::connect(std::string host, std::uint16_t port,
        std::string username, std::string password){
	
	
	if (auto rc = libssh2_init(0); rc)
        return rc;

    this->ssh_session = libssh2_session_init();
    if (!this->ssh_session){
        return -1;
	}
	
	this->socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (this->socket < 0){
        return -1;
	}
	

    auto hostaddr = inet_addr(host.data());

    sockaddr_in sin = {
        .sin_family = AF_INET,
        .sin_port   = htons(port),
        .sin_addr   = {
            .s_addr = hostaddr,
        },
    };

	
    if (auto rc = ::connect(this->socket, reinterpret_cast<sockaddr *>(&sin), sizeof(sin)); rc)
        return errno;

	
    auto lk = std::scoped_lock(this->session_mutex);

	
    if (auto rc = libssh2_session_handshake(this->ssh_session, this->socket); rc)
        return ssh2_translate_error(rc, nullptr);

    if (auto rc = libssh2_userauth_password(this->ssh_session, username.data(), password.data()); rc)
        return ssh2_translate_error(rc, nullptr);

    this->sftp_session = libssh2_sftp_init(this->ssh_session);
    if (!this->sftp_session)
        return ssh2_translate_error(libssh2_session_last_errno(this->ssh_session), this->sftp_session);

    libssh2_session_set_blocking(this->ssh_session, 1);

    this->is_connected = true;

    return 0;
}


int CSSHFS::sshfs_open(struct _reent *r, void *fileStruct, const char *path, int flags, int mode) {
    auto *priv      = static_cast<CSSHFS     *>(r->deviceData);
    auto *priv_file = static_cast<CSSHFSFile *>(fileStruct);

    auto internal_path = priv->translate_path(path);
	if (internal_path.empty()) {
        __errno_r(r) = EINVAL;
        return -1;
    }

    auto lk = std::scoped_lock(priv->session_mutex);

    priv_file->handle = libssh2_sftp_open_ex(priv->sftp_session, internal_path.data(), internal_path.length(),
        ssh2_translate_open_flags(flags), 0, LIBSSH2_SFTP_OPENFILE);
    if (!priv_file->handle) {
        __errno_r(r) = ssh2_translate_error(libssh2_session_last_errno(priv->ssh_session), priv->sftp_session);
        return -1;
    }

    auto rc = libssh2_sftp_fstat(priv_file->handle, &priv_file->attrs);
    if (rc) {
        __errno_r(r) = ssh2_translate_error(rc, priv->sftp_session);
        return -1;
    }

    priv_file->offset = 0;

    return 0;
}

int CSSHFS::sshfs_close(struct _reent *r, void *fd) {
    auto *priv      = static_cast<CSSHFS     *>(r->deviceData);
    auto *priv_file = static_cast<CSSHFSFile *>(fd);

    auto lk = std::scoped_lock(priv->session_mutex);

    auto rc = libssh2_sftp_close(priv_file->handle);
    if (rc) {
        __errno_r(r) = ssh2_translate_error(rc, priv->sftp_session);
        return -1;
    }

    return 0;
}

ssize_t CSSHFS::sshfs_read(struct _reent *r, void *fd, char *ptr, size_t len) {
    auto *priv      = static_cast<CSSHFS     *>(r->deviceData);
    auto *priv_file = static_cast<CSSHFSFile *>(fd);

    auto lk = std::scoped_lock(priv->session_mutex);

    auto rc = libssh2_sftp_read(priv_file->handle, ptr, len);
    if (rc < 0) {
        __errno_r(r) = ssh2_translate_error(rc, priv->sftp_session);
        return -1;
    }

    priv_file->offset += rc;

    return rc;
}

off_t CSSHFS::sshfs_seek(struct _reent *r, void *fd, off_t pos, int dir) {
    auto *priv      = static_cast<CSSHFS     *>(r->deviceData);
    auto *priv_file = static_cast<CSSHFSFile *>(fd);

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
            offset = priv_file->attrs.filesize;
            break;
    }

    priv_file->offset = offset + pos;

    auto lk = std::scoped_lock(priv->session_mutex);

    libssh2_sftp_seek64(priv_file->handle, priv_file->offset);
    return priv_file->offset;
}

int CSSHFS::sshfs_fstat(struct _reent *r, void *fd, struct stat *st) {
    auto *priv_file = static_cast<CSSHFSFile *>(fd);

    ssh2_translate_stat(priv_file->attrs, st);
    return 0;
}

int CSSHFS::sshfs_stat(struct _reent *r, const char *file, struct stat *st) {
    auto *priv = static_cast<CSSHFS *>(r->deviceData);
	auto internal_path = priv->translate_path(file);
    if (internal_path.empty()) {
        __errno_r(r) = EINVAL;
        return -1;
    }

    auto lk = std::scoped_lock(priv->session_mutex);

    LIBSSH2_SFTP_ATTRIBUTES attrs;
    
	auto rc = libssh2_sftp_stat_ex(priv->sftp_session, internal_path.data(),internal_path.length(),LIBSSH2_SFTP_STAT, &attrs);
    
	if (rc) {
        __errno_r(r) = ssh2_translate_error(rc, priv->sftp_session);
        return -1;
    }

    ssh2_translate_stat(attrs, st);
    return 0;
}

int CSSHFS::sshfs_lstat(struct _reent *r, const char *file, struct stat *st) {
    auto *priv = static_cast<CSSHFS *>(r->deviceData);
	auto internal_path = priv->translate_path(file);
    if (internal_path.empty()) {
        __errno_r(r) = EINVAL;
        return -1;
    }

    auto lk = std::scoped_lock(priv->session_mutex);

    LIBSSH2_SFTP_ATTRIBUTES attrs;
    auto rc = libssh2_sftp_lstat(priv->sftp_session, internal_path.data(), &attrs);
    if (rc) {
        __errno_r(r) = ssh2_translate_error(rc, priv->sftp_session);
        return -1;
    }

    ssh2_translate_stat(attrs, st);
    return 0;
}

int CSSHFS::sshfs_chdir(struct _reent *r, const char *name) {
    auto *priv = static_cast<CSSHFS *>(r->deviceData);

    if (!name) {
        __errno_r(r) = EINVAL;
        return -1;
    }

    priv->cwd = name;

    return 0;
}

DIR_ITER *CSSHFS::sshfs_diropen(struct _reent *r, DIR_ITER *dirState, const char *path) {
    auto *priv     = static_cast<CSSHFS    *>(r->deviceData);
    auto *priv_dir = static_cast<CSSHFSDir *>(dirState->dirStruct);

    auto internal_path = priv->translate_path(path);
    
	
    auto lk = std::scoped_lock(priv->session_mutex);

	priv_dir->handle = libssh2_sftp_open_ex(priv->sftp_session, internal_path.data(), internal_path.length(),
        0, 0, LIBSSH2_SFTP_OPENDIR);
    if (!priv_dir->handle) {
        __errno_r(r) = ssh2_translate_error(libssh2_session_last_errno(priv->ssh_session), priv->sftp_session);
        return nullptr;
    }

    return dirState;
}

int CSSHFS::sshfs_dirreset(struct _reent *r, DIR_ITER *dirState) {
    __errno_r(r) = ENOSYS;
    return -1;
}

int CSSHFS::sshfs_dirnext(struct _reent *r, DIR_ITER *dirState, char *filename, struct stat *filestat) {
    auto *priv     = static_cast<CSSHFS    *>(r->deviceData);
    auto *priv_dir = static_cast<CSSHFSDir *>(dirState->dirStruct);

    auto lk = std::scoped_lock(priv->session_mutex);

    LIBSSH2_SFTP_ATTRIBUTES attrs;
    while (true) {
        auto rc = libssh2_sftp_readdir(priv_dir->handle, filename, NAME_MAX, &attrs);
        if (rc == 0) {
            __errno_r(r) = ENOENT;
            return -1;
        } else if (rc < 0) {
            __errno_r(r) = ssh2_translate_error(rc, priv->sftp_session);
            return -1;
        }

        auto fname = std::string(filename);
        if (fname != "." && fname != "..")
        break;
    }

    ssh2_translate_stat(attrs, filestat);
    return 0;
}

int CSSHFS::sshfs_dirclose(struct _reent *r, DIR_ITER *dirState) {
    auto *priv     = static_cast<CSSHFS    *>(r->deviceData);
    auto *priv_dir = static_cast<CSSHFSDir *>(dirState->dirStruct);

    auto lk = std::scoped_lock(priv->session_mutex);
	return ssh2_translate_error(libssh2_sftp_closedir(priv_dir->handle), priv->sftp_session);
}

int CSSHFS::sshfs_statvfs(struct _reent *r, const char *path, struct statvfs *buf) {
    auto *priv = static_cast<CSSHFS *>(r->deviceData);

    auto internal_path = priv->translate_path(path);
    if (internal_path.empty()) {
        __errno_r(r) = EINVAL;
        return -1;
    }

    auto lk = std::scoped_lock(priv->session_mutex);

    LIBSSH2_SFTP_STATVFS st;
    auto rc = libssh2_sftp_statvfs(priv->sftp_session, internal_path.data(), internal_path.length(), &st);
    if (rc) {
        __errno_r(r) = ssh2_translate_error(rc, priv->sftp_session);
        return -1;
    }

    *buf = {};
    buf->f_bsize   = st.f_bsize;
    buf->f_frsize  = st.f_frsize;
    buf->f_blocks  = st.f_blocks;
    buf->f_bfree   = st.f_bfree;
    buf->f_bavail  = st.f_bavail;
    buf->f_files   = st.f_files;
    buf->f_ffree   = st.f_ffree;
    buf->f_favail  = st.f_favail;
    buf->f_fsid    = st.f_fsid;
    buf->f_flag    = st.f_flag;
    buf->f_namemax = st.f_namemax;

    return 0;
}
