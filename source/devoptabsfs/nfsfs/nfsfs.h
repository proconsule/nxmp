#ifndef NFSFS_DEVOPTAB_H
#define NFSFS_DEVOPTAB_H

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

typedef unsigned int u_int;

#include <nfsc/libnfs.h>
#include <nfsc/libnfs-raw.h>
#include <nfsc/libnfs-raw-mount.h>


#include <switch.h>


class CNFSFS{
public:
	CNFSFS(std::string _url,std::string _name,std::string _mount_name);
	~CNFSFS();
	std::string name, mount_name;
	
	
	int unregister_fs() const {
		return RemoveDevice(this->mount_name.data());
	}
	
	int register_fs() const {
		
		//auto id = FindDevice(this->mount_name.data());
		
		//if (id < 0){
		auto id = AddDevice(&this->devoptab);
		//	printf("AddDevice\n");
		//}
		if (id < 0)
			return id;

		return 0;
	}
	
	bool CheckConnection();
	bool RegisterFilesystem();
	bool fs_regisered = false;
	
	static int       nfsfs_open     (struct _reent *r, void *fileStruct, const char *path, int flags, int mode);
	static int       nfsfs_close    (struct _reent *r, void *fd);
	static ssize_t   nfsfs_read     (struct _reent *r, void *fd, char *ptr, size_t len);
	static off_t     nfsfs_seek     (struct _reent *r, void *fd, off_t pos, int dir);
	static int       nfsfs_fstat    (struct _reent *r, void *fd, struct stat *st);
	static int       nfsfs_stat     (struct _reent *r, const char *file, struct stat *st);
	static int       nfsfs_chdir    (struct _reent *r, const char *name);
	static DIR_ITER *nfsfs_diropen  (struct _reent *r, DIR_ITER *dirState, const char *path);
	static int       nfsfs_dirreset (struct _reent *r, DIR_ITER *dirState);
	static int       nfsfs_dirnext  (struct _reent *r, DIR_ITER *dirState, char *filename, struct stat *filestat);
	static int       nfsfs_dirclose (struct _reent *r, DIR_ITER *dirState);
	static int       nfsfs_statvfs  (struct _reent *r, const char *path, struct statvfs *buf);
	static int       nfsfs_lstat    (struct _reent *r, const char *file, struct stat *st);
	
	bool is_connected = false;
	
	struct dircache{
		std::string name;
		std::string fullpathname;
		struct stat st;
	};
	
	std::vector<dircache> cachedirlist;
	
private:
	std::string connect_url;
	
	int connect();
	void disconnect();
	
	
	std::string translate_path(const char *path);

	
	
	
	
	struct CNFSFSFile {
            struct nfsfh *nfsfh = NULL;
			nfs_stat_64 filestat;
			off_t offset;
        };

        struct CNFSFSDir {
			int diridx = 0;
        };
		

	void stat_entry(nfs_stat_64  *entry, struct stat *st);
	struct nfs_context *nfs = NULL;
	std::string cwd = "";
	std::mutex session_mutex;
	struct nfs_url *nfsurl = NULL;
protected:
	devoptab_t devoptab = {};
		

	
};

#endif