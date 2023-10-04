#if !defined(SQLITE_TEST) || SQLITE_OS_UNIX

#include "sqlite3.h"

#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/param.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>

#include <switch.h>

#include "logger.h"

/*
** Size of the write buffer used by journal files in bytes.
*/
#ifndef SQLITE_HOS_BUFFERSZ
# define SQLITE_HOS_BUFFERSZ 8192
#endif

/*
** The maximum pathname length supported by this VFS.
*/
#define MAXPATHNAME 1024

/*
** When using this VFS, the sqlite3_file* handles that SQLite uses are
** actually pointers to instances of type DemoFile.
*/
typedef struct HOS_File_struct HOS_File;
struct HOS_File_struct {
  sqlite3_file base;              /* Base class. Must be first. */
  //int fd;                         /* File descriptor */
  //FILE *fp;
  FsFile        fd;
  int offset;
  char *aBuffer;                  /* Pointer to malloc'd buffer */
  int nBuffer;                    /* Valid bytes of data in zBuffer */
  sqlite3_int64 iBufferOfst;      /* Offset in file of zBuffer[0] */
};

/*
** Write directly to the file passed as the first argument. Even if the
** file has a write-buffer (DemoFile.aBuffer), ignore it.
*/

static ssize_t
fsdev_write_safe(void          *fd,
                const char    *ptr,
                size_t        len)
{
  Result      rc;
  size_t      bytesWritten = 0;

  /* get pointer to our data */
  HOS_File *p = (HOS_File*)fd;

  /* Copy to internal buffer and transfer in chunks.
   * You cannot use FS read/write with certain memory.
   */
  char tmp_buffer[0x1000];
  while(len > 0)
  {
    size_t toWrite = len;
    if(toWrite > sizeof(tmp_buffer))
      toWrite = sizeof(tmp_buffer);

    /* copy to internal buffer */
    memcpy(tmp_buffer, ptr, toWrite);

    /* write the data */
    rc = fsFileWrite(&p->fd, p->offset, tmp_buffer, toWrite, FsWriteOption_Flush);

    if(R_FAILED(rc))
    {
      /* return partial transfer */
      if(bytesWritten > 0)
        return bytesWritten;

      
      return -1;
    }

    /* check if this is synchronous or not */
   // if(p->flags & O_SYNC)
    //  fsFileFlush(&file->fd);

    p->offset += toWrite;
    bytesWritten += toWrite;
    ptr          += toWrite;
    len          -= toWrite;
  }

  return bytesWritten;
}



static int HOS_DirectWrite(
  HOS_File *p,                    /* File handle */
  const void *zBuf,               /* Buffer containing data to write */
  int iAmt,                       /* Size of data to write in bytes */
  sqlite_int64 iOfst              /* File offset to write to */
){
  //off_t ofst;                     /* Return value from lseek() */
  //size_t nWrite;                  /* Return value from write() */

/*
  int rc = fseek( p->fp, iOfst, SEEK_SET );
  if(rc!=0){
	  return SQLITE_IOERR_WRITE;
  }
  ofst = ftell(p->fp);
   
  if( ofst!=iOfst ){
    return SQLITE_IOERR_WRITE;
  }
*/
  p->offset = iOfst;
  int rc = fsFileWrite(&p->fd, p->offset, zBuf, iAmt, FsWriteOption_None);
  fsFileFlush(&p->fd);
  if(R_VALUE(rc) == 0xD401)
    return fsdev_write_safe(&p->fd, (const char *)zBuf, iAmt);
  //nWrite = fwrite(zBuf , 1 , iAmt , p->fp );

  //fseek(p->fp,0,SEEK_SET);
  //if( nWrite!=iAmt ){
  //  return SQLITE_IOERR_WRITE;
  //}
  
  return SQLITE_OK;
}

/*
** Flush the contents of the DemoFile.aBuffer buffer to disk. This is a
** no-op if this particular file does not have a buffer (i.e. it is not
** a journal file) or if the buffer is currently empty.
*/
static int HOS_FlushBuffer(HOS_File *p){
  int rc = SQLITE_OK;
  if( p->nBuffer ){
    rc = HOS_DirectWrite(p, p->aBuffer, p->nBuffer, p->iBufferOfst);
    p->nBuffer = 0;
  }
  return rc;
}

/*
** Close a file.
*/
static int HOS_Close(sqlite3_file *pFile){
  int rc;
  HOS_File *p = (HOS_File*)pFile;
  rc = HOS_FlushBuffer(p);
  sqlite3_free(p->aBuffer);
  fsFileClose(&p->fd);
  
  return rc;
}

/*
** Read data from a file.
*/


static ssize_t
fsdev_read_safe(void          *fd,
                char          *ptr,
                size_t        len)
{
  Result      rc;
  u64         bytesRead = 0, bytes = 0;

  /* get pointer to our data */
  HOS_File *p = (HOS_File*)fd;

  /* Transfer in chunks with internal buffer.
   * You cannot use FS read/write with certain memory.
   */
  char tmp_buffer[0x1000];
  while(len > 0)
  {
    u64 toRead = len;
    if(toRead > sizeof(tmp_buffer))
      toRead = sizeof(tmp_buffer);

    /* read the data */
    rc = fsFileRead(&p->fd, p->offset, tmp_buffer, toRead, FsReadOption_None, &bytes);

    if(bytes > toRead)
      bytes = toRead;

    /* copy from internal buffer */
    memcpy(ptr, tmp_buffer, bytes);

    if(R_FAILED(rc))
    {
      /* return partial transfer */
      if(bytesRead > 0)
        return bytesRead;

      //r->_errno = fsdev_translate_error(rc);
      //return -1;
    }

    p->offset += bytes;
    bytesRead    += bytes;
    ptr          += bytes;
    len          -= bytes;
  }

  return bytesRead;
}


static int HOS_Read(
  sqlite3_file *pFile, 
  void *zBuf, 
  int iAmt, 
  sqlite_int64 iOfst
){
  HOS_File *p = (HOS_File*)pFile;
  //off_t ofst;                     /* Return value from lseek() */
  u64 nRead;                      /* Return value from read() */
  int rc;                         /* Return code from demoFlushBuffer() */

  /* Flush any data in the write buffer to disk in case this operation
  ** is trying to read data the file-region currently cached in the buffer.
  ** It would be possible to detect this case and possibly save an 
  ** unnecessary write here, but in practice SQLite will rarely read from
  ** a journal file when there is data cached in the write-buffer.
  */
  rc = HOS_FlushBuffer(p);
  if( rc!=SQLITE_OK ){
    return rc;
  }

  
  //nRead = read(p->fd, zBuf, iAmt);
  //nRead = fread(zBuf,1,iAmt,p->fp);
  fsFileRead(&p->fd, iOfst, zBuf, iAmt, FsReadOption_None, &nRead);
  if(R_VALUE(rc) == 0xD401)
	  fsdev_read_safe(&p->fd,(char *)zBuf,iAmt);
  //fseek(p->fp,0,SEEK_SET);
  //size_t fread(void *punt, size_t dim, size_t num, FILE *fp)

  if( nRead==iAmt ){
    return SQLITE_OK;
  }else if( nRead>=0 ){
    if( nRead<iAmt ){
      memset(&((char*)zBuf)[nRead], 0, iAmt-nRead);
    }
    return SQLITE_IOERR_SHORT_READ;
  }

  return SQLITE_IOERR_READ;
}

/*
** Write data to a crash-file.
*/
static int HOS_Write(
  sqlite3_file *pFile, 
  const void *zBuf, 
  int iAmt, 
  sqlite_int64 iOfst
){
  HOS_File *p = (HOS_File*)pFile;
  
  if( p->aBuffer ){
    char *z = (char *)zBuf;       /* Pointer to remaining data to write */
    int n = iAmt;                 /* Number of bytes at z */
    sqlite3_int64 i = iOfst;      /* File offset to write to */

    while( n>0 ){
      int nCopy;                  /* Number of bytes to copy into buffer */

      /* If the buffer is full, or if this data is not being written directly
      ** following the data already buffered, flush the buffer. Flushing
      ** the buffer is a no-op if it is empty.  
      */
      if( p->nBuffer==SQLITE_HOS_BUFFERSZ || p->iBufferOfst+p->nBuffer!=i ){
        int rc = HOS_FlushBuffer(p);
        if( rc!=SQLITE_OK ){
          return rc;
        }
      }
      assert( p->nBuffer==0 || p->iBufferOfst+p->nBuffer==i );
      p->iBufferOfst = i - p->nBuffer;

      /* Copy as much data as possible into the buffer. */
      nCopy = SQLITE_HOS_BUFFERSZ - p->nBuffer;
      if( nCopy>n ){
        nCopy = n;
      }
      memcpy(&p->aBuffer[p->nBuffer], z, nCopy);
      p->nBuffer += nCopy;

      n -= nCopy;
      i += nCopy;
      z += nCopy;
    }
  }else{
    return HOS_DirectWrite(p, zBuf, iAmt, iOfst);
  }

  return SQLITE_OK;
}

/*
** Truncate a file. This is a no-op for this VFS (see header comments at
** the top of the file).
*/
static int HOS_Truncate(sqlite3_file *pFile, sqlite_int64 size){

  return SQLITE_OK;
}

/*
** Sync the contents of the file to the persistent media.
*/
static int HOS_Sync(sqlite3_file *pFile, int flags){
  HOS_File *p = (HOS_File*)pFile;
  int rc;

  rc = HOS_FlushBuffer(p);
  if( rc!=SQLITE_OK ){
    return rc;
  }
  rc = fsFileFlush(&p->fd);
  //rc = fsync(p->fd);
  return (rc==0 ? SQLITE_OK : SQLITE_IOERR_FSYNC);
}

/*
** Write the size of the file in bytes to *pSize.
*/
static int HOS_FileSize(sqlite3_file *pFile, sqlite_int64 *pSize){
  HOS_File *p = (HOS_File*)pFile;
  int rc;                         /* Return code from fstat() call */
  //struct stat sStat;              /* Output of fstat() call */

  /* Flush the contents of the buffer to disk. As with the flush in the
  ** demoRead() method, it would be possible to avoid this and save a write
  ** here and there. But in practice this comes up so infrequently it is
  ** not worth the trouble.
  */
  rc = HOS_FlushBuffer(p);
  if( rc!=SQLITE_OK ){
    return rc;
  }
 
  ssize_t sz = 0;	
  fsFileGetSize(&p->fd,&sz);

  if( rc!=0 ) return SQLITE_IOERR_FSTAT;
  *pSize = sz;
  return SQLITE_OK;
}

/*
** Locking functions. The xLock() and xUnlock() methods are both no-ops.
** The xCheckReservedLock() always indicates that no other process holds
** a reserved lock on the database file. This ensures that if a hot-journal
** file is found in the file-system it is rolled back.
*/
static int HOS_Lock(sqlite3_file *pFile, int eLock){
  return SQLITE_OK;
}
static int HOS_Unlock(sqlite3_file *pFile, int eLock){
  return SQLITE_OK;
}
static int HOS_CheckReservedLock(sqlite3_file *pFile, int *pResOut){
  *pResOut = 0;
  return SQLITE_OK;
}

/*
** No xFileControl() verbs are implemented by this VFS.
*/
static int HOS_FileControl(sqlite3_file *pFile, int op, void *pArg){
  return SQLITE_NOTFOUND;
}

/*
** The xSectorSize() and xDeviceCharacteristics() methods. These two
** may return special values allowing SQLite to optimize file-system 
** access to some extent. But it is also safe to simply return 0.
*/
static int HOS_SectorSize(sqlite3_file *pFile){
  return 0;
}
static int HOS_DeviceCharacteristics(sqlite3_file *pFile){
  return 0;
}

/*
** Open a file handle.
*/
static int HOS_Open(
  sqlite3_vfs *pVfs,              /* VFS */
  const char *zName,              /* File to open, or 0 for a temp file */
  sqlite3_file *pFile,            /* Pointer to DemoFile struct to populate */
  int flags,                      /* Input SQLITE_OPEN_XXX flags */
  int *pOutFlags                  /* Output SQLITE_OPEN_XXX flags (or NULL) */
){
  static const sqlite3_io_methods HOS_io = {
    1,                            /* iVersion */
    HOS_Close,                    /* xClose */
    HOS_Read,                     /* xRead */
    HOS_Write,                    /* xWrite */
    HOS_Truncate,                 /* xTruncate */
    HOS_Sync,                     /* xSync */
    HOS_FileSize,                 /* xFileSize */
    HOS_Lock,                     /* xLock */
    HOS_Unlock,                   /* xUnlock */
    HOS_CheckReservedLock,        /* xCheckReservedLock */
    HOS_FileControl,              /* xFileControl */
    HOS_SectorSize,               /* xSectorSize */
    HOS_DeviceCharacteristics     /* xDeviceCharacteristics */
  };


  HOS_File *p = (HOS_File*)pFile; /* Populate this structure */
  int oflags = 0;                 /* flags to pass to open() call */
  char *aBuf = 0;
  memset(p, 0, sizeof(HOS_File));

  if( zName==0 ){
    return SQLITE_IOERR;
  }


  if( flags&SQLITE_OPEN_MAIN_JOURNAL ){
	aBuf = (char *)sqlite3_malloc(SQLITE_HOS_BUFFERSZ);
    if( !aBuf ){
      return SQLITE_NOMEM;
    }
  }


  if( flags&SQLITE_OPEN_EXCLUSIVE ) oflags |= O_EXCL;
  if( flags&SQLITE_OPEN_CREATE )    oflags |= O_CREAT;
  if( flags&SQLITE_OPEN_READONLY )  oflags |= O_RDONLY;
  if( flags&SQLITE_OPEN_READWRITE ) oflags |= O_RDWR;


  int native_flags = (FsOpenMode_Read | FsOpenMode_Write | FsOpenMode_Append );
  
  
  FsFileSystem fsSdmc;
  if (R_FAILED(fsOpenSdCardFileSystem(&fsSdmc)))
      return -15;

   fsSdmc = *fsdevGetDeviceFileSystem("sdmc");
   Result rc;
   struct stat file_stat = { 0 };
   if(stat(zName, &file_stat) == 0 && S_ISREG(file_stat.st_mode)){
	   
   }else{
	   rc = fsFsCreateFile(&fsSdmc, zName,0,0);
	   if( !R_SUCCEEDED(rc) ){
		sqlite3_free(aBuf);
		return SQLITE_CANTOPEN;
	  }
	   
   }
   rc = fsFsOpenFile(&fsSdmc, zName, native_flags, &p->fd);
  
  p->offset = 0;
 
 
  if( !R_SUCCEEDED(rc) ){
    sqlite3_free(aBuf);
    return SQLITE_CANTOPEN;
  }
  p->aBuffer = aBuf;

  if( pOutFlags ){
    *pOutFlags = flags;
  }
  p->base.pMethods = &HOS_io;
  return SQLITE_OK;
}

/*
** Delete the file identified by argument zPath. If the dirSync parameter
** is non-zero, then ensure the file-system modification to delete the
** file has been synced to disk before returning.
*/
static int HOS_Delete(sqlite3_vfs *pVfs, const char *zPath, int dirSync){
  int rc;                         /* Return code */

  rc = remove(zPath);
  if( rc!=0 && errno==ENOENT ) return SQLITE_OK;

  if( rc==0 && dirSync ){
    int dfd;                      /* File descriptor open on directory */
    char *zSlash;
    char zDir[MAXPATHNAME+1];     /* Name of directory containing file zPath */

    /* Figure out the directory name from the path of the file deleted. */
    sqlite3_snprintf(MAXPATHNAME, zDir, "%s", zPath);
    zDir[MAXPATHNAME] = '\0';
    zSlash = strrchr(zDir,'/');
    if( zSlash ){
      /* Open a file-descriptor on the directory. Sync. Close. */
      zSlash[0] = 0;
      dfd = open(zDir, O_RDONLY, 0);
      if( dfd<0 ){
        rc = -1;
      }else{
        rc = fsync(dfd);
        close(dfd);
      }
    }
  }
  return (rc==0 ? SQLITE_OK : SQLITE_IOERR_DELETE);
}

#ifndef F_OK
# define F_OK 0
#endif
#ifndef R_OK
# define R_OK 4
#endif
#ifndef W_OK
# define W_OK 2
#endif

/*
** Query the file-system to see if the named file exists, is readable or
** is both readable and writable.
*/
static int HOS_Access(
  sqlite3_vfs *pVfs, 
  const char *zPath, 
  int flags, 
  int *pResOut
){
	/*
  int rc;                         
  int eAccess = F_OK;             

  assert( flags==SQLITE_ACCESS_EXISTS      
       || flags==SQLITE_ACCESS_READ        
       || flags==SQLITE_ACCESS_READWRITE    
  );

  if( flags==SQLITE_ACCESS_READWRITE ) eAccess = R_OK|W_OK;
  if( flags==SQLITE_ACCESS_READ )      eAccess = R_OK;

  rc = access(zPath, eAccess);
  *pResOut = (rc==0);
  */
  
  return SQLITE_OK;
}

/*
** Argument zPath points to a nul-terminated string containing a file path.
** If zPath is an absolute path, then it is copied as is into the output 
** buffer. Otherwise, if it is a relative path, then the equivalent full
** path is written to the output buffer.
**
** This function assumes that paths are UNIX style. Specifically, that:
**
**   1. Path components are separated by a '/'. and 
**   2. Full paths begin with a '/' character.
*/
static int HOS_FullPathname(
  sqlite3_vfs *pVfs,              /* VFS */
  const char *zPath,              /* Input path (possibly a relative path) */
  int nPathOut,                   /* Size of output buffer in bytes */
  char *zPathOut                  /* Pointer to output buffer */
){
  char zDir[MAXPATHNAME+1];
  if( zPath[0]=='/' ){
    zDir[0] = '\0';
  }else{
    if( getcwd(zDir, sizeof(zDir))==0 ) return SQLITE_IOERR;
  }
  zDir[MAXPATHNAME] = '\0';

  sqlite3_snprintf(nPathOut, zPathOut, "%s/%s", zDir, zPath);
  zPathOut[nPathOut-1] = '\0';

  return SQLITE_OK;
}

/*
** The following four VFS methods:
**
**   xDlOpen
**   xDlError
**   xDlSym
**   xDlClose
**
** are supposed to implement the functionality needed by SQLite to load
** extensions compiled as shared objects. This simple VFS does not support
** this functionality, so the following functions are no-ops.
*/
static void *HOS_DlOpen(sqlite3_vfs *pVfs, const char *zPath){
  return 0;
}
static void HOS_DlError(sqlite3_vfs *pVfs, int nByte, char *zErrMsg){
  sqlite3_snprintf(nByte, zErrMsg, "Loadable extensions are not supported");
  zErrMsg[nByte-1] = '\0';
}
static void (*HOS_DlSym(sqlite3_vfs *pVfs, void *pH, const char *z))(void){
  return 0;
}
static void HOS_DlClose(sqlite3_vfs *pVfs, void *pHandle){
  return;
}

/*
** Parameter zByte points to a buffer nByte bytes in size. Populate this
** buffer with pseudo-random data.
*/
static int HOS_Randomness(sqlite3_vfs *pVfs, int nByte, char *zByte){
  return SQLITE_OK;
}

/*
** Sleep for at least nMicro microseconds. Return the (approximate) number 
** of microseconds slept for.
*/
static int HOS_Sleep(sqlite3_vfs *pVfs, int nMicro){
  sleep(nMicro / 1000000);
  usleep(nMicro % 1000000);
  return nMicro;
}

/*
** Set *pTime to the current UTC time expressed as a Julian day. Return
** SQLITE_OK if successful, or an error code otherwise.
**
**   http://en.wikipedia.org/wiki/Julian_day
**
** This implementation is not very good. The current time is rounded to
** an integer number of seconds. Also, assuming time_t is a signed 32-bit 
** value, it will stop working some time in the year 2038 AD (the so-called
** "year 2038" problem that afflicts systems that store time this way). 
*/
static int HOS_CurrentTime(sqlite3_vfs *pVfs, double *pTime){
  time_t t = time(0);
  *pTime = t/86400.0 + 2440587.5; 
  return SQLITE_OK;
}

/*
** This function returns a pointer to the VFS implemented in this file.
** To make the VFS available to SQLite:
**
**   sqlite3_vfs_register(sqlite3_demovfs(), 0);
*/
sqlite3_vfs *sqlite3_HOSvfs(void){
  static sqlite3_vfs HOSvfs = {
    1,                            /* iVersion */
    sizeof(HOS_File),             /* szOsFile */
    MAXPATHNAME,                  /* mxPathname */
    0,                            /* pNext */
    "HOS_VFS",                    /* zName */
    0,                            /* pAppData */
    HOS_Open,                     /* xOpen */
    HOS_Delete,                   /* xDelete */
    HOS_Access,                   /* xAccess */
    HOS_FullPathname,             /* xFullPathname */
    HOS_DlOpen,                   /* xDlOpen */
    HOS_DlError,                  /* xDlError */
    HOS_DlSym,                    /* xDlSym */
    HOS_DlClose,                  /* xDlClose */
    HOS_Randomness,               /* xRandomness */
    HOS_Sleep,                    /* xSleep */
    HOS_CurrentTime,              /* xCurrentTime */
  };
  return &HOSvfs;
}



int sqlite3_os_init(void) {
	sqlite3_vfs_register(sqlite3_HOSvfs(), 1);
	return 0;
}

int sqlite3_os_end(void) {
	return 0;
}

#endif /* !defined(SQLITE_TEST) || SQLITE_OS_UNIX */

