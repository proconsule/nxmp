#ifndef ARCHIVEFS_DEVOPTAB_H
#define ARCHIVEFS_DEVOPTAB_H

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

#include <archive.h>
#include <archive_entry.h>

#include <switch.h>


class CARCHFS{
public:
	CARCHFS(std::string _url,std::string _name,std::string _mount_name);
	~CARCHFS();
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
	
	static int       archfs_open     (struct _reent *r, void *fileStruct, const char *path, int flags, int mode);
	static int       archfs_close    (struct _reent *r, void *fd);
	static ssize_t   archfs_read     (struct _reent *r, void *fd, char *ptr, size_t len);
	static off_t     archfs_seek     (struct _reent *r, void *fd, off_t pos, int dir);
	static int       archfs_fstat    (struct _reent *r, void *fd, struct stat *st);
	static int       archfs_stat     (struct _reent *r, const char *file, struct stat *st);
	static int       archfs_chdir    (struct _reent *r, const char *name);
	static DIR_ITER *archfs_diropen  (struct _reent *r, DIR_ITER *dirState, const char *path);
	static int       archfs_dirreset (struct _reent *r, DIR_ITER *dirState);
	static int       archfs_dirnext  (struct _reent *r, DIR_ITER *dirState, char *filename, struct stat *filestat);
	static int       archfs_dirclose (struct _reent *r, DIR_ITER *dirState);
	static int       archfs_statvfs  (struct _reent *r, const char *path, struct statvfs *buf);
	static int       archfs_lstat    (struct _reent *r, const char *file, struct stat *st);
	
	bool is_connected = false;
	
private:
	std::string connect_url;
	
	int connect();
	void disconnect();
	
	int find_fileentry(std::string filepath);

	
	struct CARCHFSFile {
			struct archive_entry *entry;
			struct archive *arch_ctx;
			off_t offset;
        };

        struct CARCHFSDir {
			char dirpath[256] = "";
			int dirnext_idx = 0;
        };
	
	
	struct archfileentry_struct{
		std::string parent;
		std::string filename;
		struct stat st = {};
		int level;
		
		
	};
	
	std::vector<archfileentry_struct> totalfilelist;
	std::string cwd = "";
	std::mutex session_mutex;
	protected:
        devoptab_t devoptab = {};
	
};

#endif