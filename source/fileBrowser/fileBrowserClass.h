#ifndef NXMP_FILEBROWSER_H
#define NXMP_FILEBROWSER_H

#include "HTTPDir.h"
#include "usbfs.h"


#include "smb2fs.h"
#include "sshfs.h"
#include "nfsfs.h"
#include "libarchivefs.h"
#include "ftpfs.h"
#include "m3u8fs.h"
#include "PDFClass.h"


#include "utils.h"
#include "imgloader.h"
#include  "iniparser.h"


typedef struct{
	char path[NAME_MAX];
	size_t size;
	size_t currentOffset;
	size_t readbuffersize;
	bool memvalid = false;
	unsigned char * mem = nullptr;
}fileload_struct;


class CFileBrowser{
public:
	CFileBrowser(std::string _path,Playlist * _playlist);
	CFileBrowser(networkstruct_v2 netconfdata,Playlist * _playlist);
	CFileBrowser(USBMounter * _myusb,Playlist * _playlist);
	~CFileBrowser();
	
	HTTPDir *myhttp = nullptr;
	USBMounter *myusb = nullptr;
	Playlist * currentplaylist;
	
	
	void OpenArchive(std::string _path);
	std::string CloseArchive();
	
	void OpenM3U(std::string _path);
	std::string CloseM3U();
	
	void DirList(std::string path,bool showHidden,const std::vector<std::string> &extensions);
	std::vector<FS::FileEntry> getCurrList();
	std::vector<FS::FileEntry> getCurrImageList();
	std::vector<FS::FileEntry> getChecked();
	
	std::string backDir();
	std::string getUrl();
	std::string getCurrentPath();
	std::string getBasePath();
	std::string getShare();
	std::string getOpenUrlPart();
	
	void setBasePath(std::string _basepath);
	bool remotefs = false;
	
	void clearChecked();
	bool *checked(int pos);
	
#ifdef DEKO3D_BACKEND
	Texture OpenImageMemory(unsigned char *_img_data,int _size);
#endif
	bool getfileContents(std::string filepath,unsigned char ** _filedata,int &_size);
	bool getfileContentsThreaded(std::string filepath);
	
	
	
	void setSordOrder(FS::FILESORTORDER myorder);
	void SetFileDbStatus(int idx,int dbstatus);
	void ResetDbStatus();
	
	
	std::string getTitle(); 
	std::string path;
	
	std::string currentFileinUse;
	
	FS::FILESORTORDER sortOrder = FS::FS_NAME_ASCENDINGORDER;
	bool currshowHidden = false;
	
	bool timelessFS = false;
	bool connected = true;
	std::string errormsg = "";
	std::vector<usb_devices> getUsbDev(bool dummy = false);
	
	
	
	bool LoadedFileStatus = false;
	std::string LoadedFileName;
	unsigned char * LoadedFileBuffer = nullptr;
	size_t LoadedFileSize = 0;
	uint32_t maxreadsize = 1024*1024;
	size_t CurrentReadOffset = 0; 
	
	int getNextImg();
	int getPrevImg();
	
	Thread readThreadref;
	fileload_struct *LoadedFile = nullptr;

	
	int OpenPDF(unsigned char ** _image_data,int *_w,int *_h);
	int getNextPDFPage(unsigned char ** _image_data,int *_w,int *_h);
	int getPrevPDFPage(unsigned char ** _image_data,int *_w,int *_h);
	int getPDFPageCount();
	int getPDFCurrentPage();
	
protected:
	CSSHFS * sshfs = nullptr;
	CSMB2FS * smb2fs = nullptr;
	CNFSFS * nfsfs = nullptr;
	CARCHFS * archfs = nullptr;
	CFTPFS *ftpfs = nullptr;
	CM3U8FS *m3u8fs = nullptr;
	CPDFClass *PDF = nullptr;
	

private:
	
	

	std::string title;
	
	unsigned char *archive_data = NULL;
	int archive_datasize = 0;
	
	std::string oldtitle = "";
	std::string oldmount = "";
	
	
	std::string basepath = "/";
	std::string currentpath = "";
	std::vector<FS::FileEntry> currentlist;
	std::vector<FS::FileEntry> currentimagelist;

	
	
};




#endif