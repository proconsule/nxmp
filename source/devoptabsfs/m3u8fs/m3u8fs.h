#ifndef M3U8FS_DEVOPTAB_H
#define M3U8FS_DEVOPTAB_H

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



#include <switch.h>


class CM3U8FS{
public:
	CM3U8FS(std::string _url,std::string _name,std::string _mount_name);
	~CM3U8FS();
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
	
	static int       m3u8fs_open     (struct _reent *r, void *fileStruct, const char *path, int flags, int mode);
	static int       m3u8fs_close    (struct _reent *r, void *fd);
	static ssize_t   m3u8fs_read     (struct _reent *r, void *fd, char *ptr, size_t len);
	static off_t     m3u8fs_seek     (struct _reent *r, void *fd, off_t pos, int dir);
	static int       m3u8fs_fstat    (struct _reent *r, void *fd, struct stat *st);
	static int       m3u8fs_stat     (struct _reent *r, const char *file, struct stat *st);
	static int       m3u8fs_chdir    (struct _reent *r, const char *name);
	static DIR_ITER *m3u8fs_diropen  (struct _reent *r, DIR_ITER *dirState, const char *path);
	static int       m3u8fs_dirreset (struct _reent *r, DIR_ITER *dirState);
	static int       m3u8fs_dirnext  (struct _reent *r, DIR_ITER *dirState, char *filename, struct stat *filestat);
	static int       m3u8fs_dirclose (struct _reent *r, DIR_ITER *dirState);
	static int       m3u8fs_statvfs  (struct _reent *r, const char *path, struct statvfs *buf);
	static int       m3u8fs_lstat    (struct _reent *r, const char *file, struct stat *st);
	
	bool is_connected = false;
	
	struct dircache{
		std::string name;
		std::string fullpathname;
		struct stat st;
	};
	
	std::vector<dircache> cachedirlist;
	
	struct m3uele{
		std::string name;
		std::vector<std::string> url;
		
	};
	
	struct m3urawele{
		std::string name;
		std::string url;
		
	};
	
	std::vector<m3uele> m3u8_list;
	
	time_t plsmtime;
	
	unsigned char * m3u8_file_memory = nullptr;
	
private:
	std::string connect_url;
	
	int connect();
	void disconnect();
	
	
	std::string translate_path(const char *path);

	
	
	std::string m3uabspath = "";
	
	struct CM3U8FSFile {
			int fileidx;
            off_t offset;
			char * filedata;
			size_t filesize;
        };

        struct CM3U8FSDir {
			int diridx = 0;
			int listoffset = -1;
        };
		

	std::string cwd = "";
	std::mutex session_mutex;
	
protected:
	devoptab_t devoptab = {};
		

	
};

#endif