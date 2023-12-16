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

#include <smb2/smb2.h>
#include <smb2/libsmb2.h>
#include <smb2/libsmb2-raw.h>

#include <switch.h>


class CSMB2FS{
public:
	CSMB2FS(std::string _url,std::string _name,std::string _mount_name);
	~CSMB2FS();
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
	bool fs_regisered = false;
	
private:
	std::string connect_url;
	
	int connect();
	void disconnect();
	
	
	
	struct smb2_context * smb2 = nullptr;
	struct smb2_url *smb2url = nullptr;
	
	
	
	void stat_entry(smb2_stat_64  *entry, struct stat *st);
	
	struct CSMB2FSFile {
			struct smb2fh *fh;
			smb2_stat_64  smb2st;
            off_t offset;
        };

        struct CSMB2FSDir {
			struct smb2dir *dir;
        };
	
	
	
	std::string cwd = "";
	std::mutex session_mutex;
	protected:
        devoptab_t devoptab = {};
	
};

#endif