#ifndef FTPFS_DEVOPTAB_H
#define FTPFS_DEVOPTAB_H

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

#include <vector>

#include <mutex>


#include <switch.h>

#include "ugly-ftplib.h"


class CFTPFS{
public:
	CFTPFS(std::string _url,std::string _name,std::string _mount_name);
	~CFTPFS();
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
	
	static int       ftpfs_open     (struct _reent *r, void *fileStruct, const char *path, int flags, int mode);
	static int       ftpfs_close    (struct _reent *r, void *fd);
	static ssize_t   ftpfs_read     (struct _reent *r, void *fd, char *ptr, size_t len);
	static off_t     ftpfs_seek     (struct _reent *r, void *fd, off_t pos, int dir);
	static int       ftpfs_fstat    (struct _reent *r, void *fd, struct stat *st);
	static int       ftpfs_stat     (struct _reent *r, const char *file, struct stat *st);
	static int       ftpfs_chdir    (struct _reent *r, const char *name);
	static DIR_ITER *ftpfs_diropen  (struct _reent *r, DIR_ITER *dirState, const char *path);
	static int       ftpfs_dirreset (struct _reent *r, DIR_ITER *dirState);
	static int       ftpfs_dirnext  (struct _reent *r, DIR_ITER *dirState, char *filename, struct stat *filestat);
	static int       ftpfs_dirclose (struct _reent *r, DIR_ITER *dirState);
	static int       ftpfs_statvfs  (struct _reent *r, const char *path, struct statvfs *buf);
	static int       ftpfs_lstat    (struct _reent *r, const char *file, struct stat *st);
	
	bool is_connected = false;
	
	std::string host;
	std::string user;
	std::string pass;
	uint16_t port;
	std::string path;
	
	
private:
	std::string connect_url;
	
	int connect(std::string _host, std::uint16_t _port,
        std::string _username, std::string _password);
	void disconnect();
	
	
	std::string translate_path(const char *path);
	
	struct ftpurlschema{
		std::string scheme;
		std::string server;
		std::string user;
		std::string pass;
		std::string port;
		std::string path;
	};

	ftpurlschema parseFTPUrl(std::string url);
	
	
	
	struct CFTPFSFile {
			int nData = -1;
            struct stat st = {0};
			off_t offset = 0;
			off_t readoffset = 0;
			//std::string filename = "";
			char filename[NAME_MAX];
			bool opened=false;
			server_connection connection;
        };

        struct CFTPFSDir {
			int diridx = 0;
			server_connection connection;
        };
		

	struct dircache{
		std::string name;
		std::string fullpathname;
		struct stat st;
	};
	
	

	int c_socket = 0;
	int data_socket = 0;

	//char *name;             //'ftp1', NULL means free item
	int envIndex;           //position if this item in FTPEnv array

	//char currentpath[FTP_MAXPATH];  //Always terminated by '/'. '/' for share root. Always contains'/', not '\'.


	int socket;
	std::string cwd = "";
	std::mutex session_mutex;
	
	ftpurlschema urlschema;
	
	
	server_connection staticconnection = {0};
	
	std::vector<dircache> cachedirlist;
	
	server_connection *ftpconnection = nullptr;
	

protected:
	devoptab_t devoptab = {};
		

	
};

#endif