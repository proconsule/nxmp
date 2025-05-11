#ifndef SMB2FS_DEVOPTAB_H
#define SMB2FS_DEVOPTAB_H

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/dirent.h>
#include <sys/iosupport.h>
#include <sys/param.h>
#include <unistd.h>

#include <mutex>

#include <vector>

#include <smb2/smb2.h>
#include <smb2/libsmb2.h>
#include <smb2/libsmb2-raw.h>

#include <switch.h>

class CSMB2_PARSER{
public:
	
	CSMB2_PARSER(std::string myurl){
		int smbpos = myurl.rfind("smb://", 0);
		if (smbpos != 0) { // pos=0 limits the search to the prefix
            valid = false;
			return;
        }
		size_t found = myurl.find(":",6);
		size_t found2 = myurl.find("@");
		size_t found3 = myurl.find("/",6);
		size_t found4 = myurl.find("/",found3+1);
		size_t found5 = myurl.find("/",found4+1);
		
		if(found == -1 || found2 == -1 || found3 == -1){
			valid = false;
			return;
		}
		
		user = myurl.substr(6,found-6);
		pass = myurl.substr(found+1,found2-found-1);
		server = myurl.substr(found2+1,found3-found2-1);
		share = myurl.substr(found3+1,found4-found3-1);
		path = myurl.substr(found4+1,found5-found4-1);
		valid = true;
	}
	
	bool valid = false;
	
	
	std::string domain;
    std::string user;
    std::string pass;
    std::string server;
    std::string share;
    std::string path;
	
};

class CSMB2FS{
public:
	CSMB2FS(std::string _url,std::string _name,std::string _mount_name);
	CSMB2FS(std::string _server,std::string _username,std::string _password,std::string _path,std::string _name,std::string _mount_name);
	~CSMB2FS();
	std::string name, mount_name;
	
	CSMB2_PARSER * SMB2_PARSER;
	
	int unregister_fs() const {
		return RemoveDevice(this->mount_name.data());
	}
	
	int register_fs() const {
		
		auto id = FindDevice(this->mount_name.data());
		
		if (id < 0){
			id = AddDevice(&this->devoptab);
			
		}
		if (id < 0)
			return id;

		return 0;
	}
	
	static int       smb2fs_open     (struct _reent *r, void *fileStruct, const char *path, int flags, int mode);
	static int       smb2fs_close    (struct _reent *r, void *fd);
	static ssize_t   smb2fs_read     (struct _reent *r, void *fd, char *ptr, size_t len);
	static off_t     smb2fs_seek     (struct _reent *r, void *fd, off_t pos, int dir);
	static int       smb2fs_fstat    (struct _reent *r, void *fd, struct stat *st);
	static int       smb2fs_stat     (struct _reent *r, const char *file, struct stat *st);
	static int       smb2fs_chdir    (struct _reent *r, const char *name);
	static DIR_ITER *smb2fs_diropen  (struct _reent *r, DIR_ITER *dirState, const char *path);
	static int       smb2fs_dirreset (struct _reent *r, DIR_ITER *dirState);
	static int       smb2fs_dirnext  (struct _reent *r, DIR_ITER *dirState, char *filename, struct stat *filestat);
	static int       smb2fs_dirclose (struct _reent *r, DIR_ITER *dirState);
	static int       smb2fs_statvfs  (struct _reent *r, const char *path, struct statvfs *buf);
	static int       smb2fs_lstat    (struct _reent *r, const char *file, struct stat *st);
	
	bool is_connected = false;
	
	uint32_t maxreadsize = 1024*1024;
	
	bool CheckConnection();
	bool RegisterFilesystem();
	bool RegisterFilesystem_v2();
	bool fs_regisered = false;
	
	struct dircache{
		std::string name;
		std::string fullpathname;
		struct stat st;
	};
	
	std::vector<dircache> cachedirlist;
	
private:
	std::string connect_url;
	
	int connect();
	int connect_v2();
	void disconnect();
	
	std::string translate_path(const char *path);
	
	struct smb2_context * smb2 = nullptr;
	struct smb2_url *smb2url = nullptr;
	
	
	
	void stat_entry(smb2_stat_64  *entry, struct stat *st);
	
	struct CSMB2FSFile {
			struct smb2fh *fh = nullptr;;
			smb2_stat_64  smb2st;
            off_t offset;
        };

        struct CSMB2FSDir {
			int diridx = 0;
			
        };
	
	
	std::string server = "";
	std::string username = "";
	std::string password = "";
	std::string path = "";
	
	
	
	std::string cwd = "";
	std::mutex session_mutex;
	protected:
        devoptab_t devoptab = {};
	
};

#endif