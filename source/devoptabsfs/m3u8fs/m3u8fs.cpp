/* based on https://gist.github.com/averne/527f3a739d19c8d573b2d6995a33edea */


#include "m3u8fs.h"
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/syslimits.h>
#include <regex>

void m3u8stat_entry(struct stat *srcst, struct stat *dstst);

CM3U8FS::CM3U8FS(std::string _url,std::string _name,std::string _mount_name){
	this->connect_url = _url;
	
	
	this->name       = _name;
    this->mount_name = _mount_name;

    this->devoptab = {
        .name         = CM3U8FS::name.data(),

        .structSize   = sizeof(CM3U8FSFile),
        .open_r       = CM3U8FS::m3u8fs_open,
        .close_r      = CM3U8FS::m3u8fs_close,
        .read_r       = CM3U8FS::m3u8fs_read,
        .seek_r       = CM3U8FS::m3u8fs_seek,
        .fstat_r      = CM3U8FS::m3u8fs_fstat,

        .stat_r       = CM3U8FS::m3u8fs_stat,
        .chdir_r      = CM3U8FS::m3u8fs_chdir,

        .dirStateSize = sizeof(CM3U8FSDir),
        .diropen_r    = CM3U8FS::m3u8fs_diropen,
        .dirreset_r   = CM3U8FS::m3u8fs_dirreset,
        .dirnext_r    = CM3U8FS::m3u8fs_dirnext,
        .dirclose_r   = CM3U8FS::m3u8fs_dirclose,

        .statvfs_r    = CM3U8FS::m3u8fs_statvfs,

        .deviceData   = this,

        .lstat_r      = CM3U8FS::m3u8fs_stat,
    };
	
	
}

bool CM3U8FS::CheckConnection(){
	if(connect() == 0){
		this->cwd = "/";
		return true;
	}
	return false;
}

bool CM3U8FS::RegisterFilesystem(){
	if(connect() == 0){
		this->cwd = "/";
		fs_regisered = true;
		register_fs();
		return true;
	}
	return false;
}

CM3U8FS::~CM3U8FS(){
	if (this->is_connected)
        this->disconnect();

     
	
	if(fs_regisered){
		unregister_fs();
	}
}

void CM3U8FS::disconnect(){
	
    auto lk = std::scoped_lock(this->session_mutex);
	if(m3u8_file_memory != nullptr)free(m3u8_file_memory);
    
    this->is_connected = false;
}
std::string CM3U8FS::translate_path(const char *path){
   return this->cwd + (path + CM3U8FS::mount_name.length()+1);
}


static std::string m3u8_format_args = "#EXTM3U\n#EXTINF:-1,%s\n%s\n";


int CM3U8FS::connect(){
	
	struct stat st;
	if (stat(connect_url.c_str(), &st) == 0) {
		int filefd = open(connect_url.c_str(),O_RDONLY);
		if(filefd>0){
			m3u8_file_memory = (unsigned char *)malloc(st.st_size*sizeof(char));
			int ret = read(filefd,m3u8_file_memory,st.st_size);
			if(ret==st.st_size){
				
				
				std::regex plsregex("(?:^|\n)#EXTINF:(.+),(.*?)[\r\n]+(.*)");
				
				std::smatch match;
				std::string testmatch = strdup((const char *)m3u8_file_memory);
				
				int idx = 0;
				
				std::vector<m3urawele> tmpvector;
				
				for(std::sregex_iterator i = std::sregex_iterator(testmatch.begin(), testmatch.end(), plsregex);
                         i != std::sregex_iterator();
                         ++i){
							std::smatch m = *i;
							
							m3urawele entry;
							
							entry.name = m.str(2).c_str();
							entry.url = m.str(3).c_str(); 
							tmpvector.push_back(entry);
							idx++;
							/*
							printf("STRING: %s\n",m.str(0).c_str());
							printf("STRING: %s\n",m.str(1).c_str());
							printf("STRING: %s\n",m.str(2).c_str());
							printf("STRING: %s\n",m.str(3).c_str());
							*/
						 }
				
				for(int i=0;i<tmpvector.size();i++){
					
					int found = -1;
					
					for(int x=0;x<m3u8_list.size();x++){
						if(tmpvector[i].name == m3u8_list[x].name){
							found = x;
							break;
						}
					}
					if(found>-1){
						m3u8_list[found].url.push_back(tmpvector[i].url);
					}else{
						m3uele entry;
						entry.name = tmpvector[i].name;
						entry.url.push_back(tmpvector[i].url);
						m3u8_list.push_back(entry);
					}
				}
				
				
				this->is_connected = true;
				return 0;
			}
			
		}
	}
	
    return -1;
}


int CM3U8FS::m3u8fs_open(struct _reent *r, void *fileStruct, const char *path, int flags, int mode) {
    auto *priv      = static_cast<CM3U8FS     *>(r->deviceData);
    auto *priv_file = static_cast<CM3U8FSFile *>(fileStruct);

    auto internal_path = priv->translate_path(path);
	if (internal_path.empty()) {
       __errno_r(r) = EINVAL;
        return -1;
    }

    auto lk = std::scoped_lock(priv->session_mutex);

	priv_file->fileidx = -1;

	for(int i=0;i<priv->m3u8_list.size();i++){
		if(priv->m3u8_list[i].name + ".m3u8" == internal_path.substr(internal_path.find_last_of("/")+1)){
			priv_file->fileidx=i;
			priv_file->filesize = priv->m3u8_list[i].url[0].length()+priv->m3u8_list[i].name.length()+m3u8_format_args.length()-4;
			priv_file->filedata = (char *)malloc(priv_file->filesize+1);
			memset(priv_file->filedata,0,priv_file->filesize+1);
			sprintf(priv_file->filedata,m3u8_format_args.c_str(),priv->m3u8_list[i].name.c_str(),priv->m3u8_list[i].url[0].c_str());
			return 0;
		}
	}
	
	if(priv_file->fileidx==-1)return -1;
	
    
    priv_file->offset = 0;

    return 0;
}

int CM3U8FS::m3u8fs_close(struct _reent *r, void *fd) {
    auto *priv      = static_cast<CM3U8FS     *>(r->deviceData);
    auto *priv_file = static_cast<CM3U8FSFile *>(fd);

    auto lk = std::scoped_lock(priv->session_mutex);

	if(priv_file->filedata)free(priv_file->filedata);

    return 0;
}

ssize_t CM3U8FS::m3u8fs_read(struct _reent *r, void *fd, char *ptr, size_t len) {
    auto *priv      = static_cast<CM3U8FS     *>(r->deviceData);
    auto *priv_file = static_cast<CM3U8FSFile *>(fd);

    auto lk = std::scoped_lock(priv->session_mutex);


	if(priv_file->offset>=priv_file->filesize)return 0;

	if(len+priv_file->offset>=priv_file->filesize)len = priv_file->filesize-priv_file->offset;

	memcpy(ptr,&priv_file->filedata[priv_file->offset],len);
	
	

    priv_file->offset += len;

    return len;
}

off_t CM3U8FS::m3u8fs_seek(struct _reent *r, void *fd, off_t pos, int dir) {
    auto *priv      = static_cast<CM3U8FS     *>(r->deviceData);
    auto *priv_file = static_cast<CM3U8FSFile *>(fd);

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
            offset = priv_file->filesize;
            break;
    }

    priv_file->offset = offset + pos;

    auto lk = std::scoped_lock(priv->session_mutex);


    return priv_file->offset;
}

int CM3U8FS::m3u8fs_fstat(struct _reent *r, void *fd, struct stat *st) {
    auto *priv = static_cast<CM3U8FS *>(r->deviceData);
	auto *priv_file = static_cast<CM3U8FSFile *>(fd);
	
	auto lk = std::scoped_lock(priv->session_mutex);

	st->st_size = priv_file->filesize;
	st->st_mode = S_IFREG;

    return 0;
}

int CM3U8FS::m3u8fs_stat(struct _reent *r, const char *file, struct stat *st) {
    auto *priv = static_cast<CM3U8FS *>(r->deviceData);
	
    auto internal_path = priv->translate_path(file);
    if (internal_path.empty()) {
        __errno_r(r) = EINVAL;
        return -1;
    }

    auto lk = std::scoped_lock(priv->session_mutex);

	
	for(int i=0;i<priv->m3u8_list.size();i++){
		
		if(priv->m3u8_list[i].name + ".m3u8" == internal_path.substr(internal_path.find_last_of("/")+1)){
			st->st_size = priv->m3u8_list[i].name.length()+priv->m3u8_list[i].url[0].length()+m3u8_format_args.length()-4;
			st->st_mode = S_IFREG;
			return 0;
		}
	}
   
	
    return -1;
}


int CM3U8FS::m3u8fs_chdir(struct _reent *r, const char *name) {
    auto *priv = static_cast<CM3U8FS *>(r->deviceData);

    if (!name) {
        __errno_r(r) = EINVAL;
        return -1;
    }

    priv->cwd = name;

    return 0;
}

DIR_ITER *CM3U8FS::m3u8fs_diropen(struct _reent *r, DIR_ITER *dirState, const char *path) {
    auto *priv     = static_cast<CM3U8FS    *>(r->deviceData);
    auto *priv_dir = static_cast<CM3U8FSDir *>(dirState->dirStruct);

    auto internal_path = priv->translate_path(path);
    
	auto lk = std::scoped_lock(priv->session_mutex);
	if(internal_path == "/"){
		priv_dir->listoffset = -1;
	}else{
		for(int i=0;i<priv->m3u8_list.size();i++){
			if( priv->m3u8_list[i].name == internal_path.substr(1)){
				priv_dir->listoffset = i;
			}
		}
	}
	priv_dir->diridx = 0;
	
    return dirState;
}

int CM3U8FS::m3u8fs_dirreset(struct _reent *r, DIR_ITER *dirState) {
    __errno_r(r) = ENOSYS;
    return -1;
}

int CM3U8FS::m3u8fs_dirnext(struct _reent *r, DIR_ITER *dirState, char *filename, struct stat *filestat) {
    auto *priv     = static_cast<CM3U8FS    *>(r->deviceData);
    auto *priv_dir = static_cast<CM3U8FSDir *>(dirState->dirStruct);

    auto lk = std::scoped_lock(priv->session_mutex);

	
	if(priv_dir->listoffset == -1){

		if(priv_dir->diridx >= priv->m3u8_list.size()){
				priv_dir->diridx = 0;
				return -1;
			}
		
		memset(filename,0,NAME_MAX);
		memcpy(filename,priv->m3u8_list[priv_dir->diridx].name.c_str(),priv->m3u8_list[priv_dir->diridx].name.length());
		filestat->st_mode = S_IFDIR;
			
		//filestat->st_size = priv->m3u8_list[priv_dir->diridx].url.length();
		priv_dir->diridx++;
		return 0;
	}else{
		if(priv_dir->diridx >= priv->m3u8_list[priv_dir->listoffset].url.size()){
			priv_dir->diridx = 0;
			return -1;
		}
		
		std::string newpath = priv->m3u8_list[priv_dir->listoffset].name + ".m3u8";
		
		memset(filename,0,NAME_MAX);
		memcpy(filename,newpath.c_str(),newpath.length());
		filestat->st_mode = S_IFREG;
		filestat->st_size = priv->m3u8_list[priv_dir->listoffset].name.length()+priv->m3u8_list[priv_dir->listoffset].url[0].length()+m3u8_format_args.length()-4;
		
		priv_dir->diridx++;
		return 0;
	}
	
    return -1;
}

int CM3U8FS::m3u8fs_dirclose(struct _reent *r, DIR_ITER *dirState) {
    auto *priv     = static_cast<CM3U8FS    *>(r->deviceData);
    auto *priv_dir = static_cast<CM3U8FSDir *>(dirState->dirStruct);

    auto lk = std::scoped_lock(priv->session_mutex);
	
	return 0;
}

int CM3U8FS::m3u8fs_statvfs(struct _reent *r, const char *path, struct statvfs *buf) {
    auto *priv = static_cast<CM3U8FS *>(r->deviceData);

    auto internal_path = priv->translate_path(path);
    

    return 0;
}

void m3u8stat_entry(struct stat *srcst, struct stat *dstst)
{
	memcpy(dstst,srcst,sizeof(struct stat));
	
}
