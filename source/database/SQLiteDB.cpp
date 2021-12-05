#include "SQLiteDB.h"


SQLiteDB::SQLiteDB(std::string _filename){
	dbfilename = _filename;
	sqlite3_stmt *res;
	bool haveversion = false;
	int rc = sqlite3_open(":memory:",&db);
	loadOrSaveDb(db,_filename.c_str(),0);
	
	if (rc != SQLITE_OK ) {
		printf("Error Opening DB\n");
	}
	char *err_msg = 0;
	
	sqlite3_extended_result_codes(db, 1);
	
	
	char sqlversion[] = "SELECT sqlite_version();";
	rc = sqlite3_prepare_v2(db, sqlversion, -1, &res, NULL);
	rc = sqlite3_step(res);
	if (rc == SQLITE_ROW) {
		sqliteversion = (char *)sqlite3_column_text(res,0);
	}
	
	sqlite3_finalize(res);

	char sql[] = "CREATE TABLE IF NOT EXISTS NXMP(major INTEGER, minor INTEGER, micro INTEGER);";
	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
	if (rc != SQLITE_OK ) {
		
	}
	char sqlfileresume[] = "CREATE TABLE IF NOT EXISTS FILERESUME(Id INTEGER PRIMARY KEY AUTOINCREMENT , Path TEXT, Position INTEGER,Completed INTEGER default 0);";
	rc = sqlite3_exec(db, sqlfileresume, 0, 0, &err_msg);
	
	rc = sqlite3_prepare_v2(db, "SELECT * from NXMP", -1, &res, NULL);
	if (rc != SQLITE_OK) {
	
	}
	rc = sqlite3_step(res);
	
	
	if (rc == SQLITE_ROW) {
		dbmajor = sqlite3_column_int(res, 0);
		dbminor = sqlite3_column_int(res, 1);
		dbmicro = sqlite3_column_int(res, 2);
		printf("File DB Version %d.%d.%d\n", dbmajor,dbminor,dbmicro);
		haveversion = true;
    }
	sqlite3_finalize(res);
	
	if(!haveversion){
		char sqlquery[64];
		sprintf(sqlquery,"INSERT INTO NXMP VALUES(%d, %d, %d)",VERSION_MAJOR,VERSION_MINOR,VERSION_MICRO);
		rc = sqlite3_exec(db, sqlquery, 0, 0, &err_msg);
		
		if (rc != SQLITE_OK ) {
			dbcorrupted = true;
			printf("Error Insert %s\n",err_msg);
			
		}
		if( rc == SQLITE_OK){
			dbmajor = VERSION_MAJOR;
			dbminor = VERSION_MINOR;
			dbmicro = VERSION_MICRO;
		}
	}
	if(haveversion){
		
		int checkmajor = VERSION_MAJOR-dbmajor;
		int checkminor = VERSION_MINOR-dbminor;
		int checkmicro = VERSION_MICRO-dbmicro;
		
		if(checkmajor == 0 && checkminor == 0 && checkmicro == 0){
			printf("DB Same Version\n");
		}else{
			printf("DB Version Mismatch\n");
			dbUpdated = true;
			UpdateDB();
		}
		
	}
	

}

SQLiteDB::~SQLiteDB(){
	loadOrSaveDb(db,dbfilename.c_str(),1);
	sqlite3_close(db);

}

bool SQLiteDB::dbwasUpdated(){
	return dbUpdated;
}

void SQLiteDB::UpdateDB(){
	char *err_msg = 0;
	int rc = sqlite3_exec(db, "DROP TABLE NXMP", 0, 0, &err_msg);
	if (rc == SQLITE_OK ) {
		printf("Drop NXMP Version Table\n");
	}
	char sql[] = "CREATE TABLE IF NOT EXISTS NXMP(major INTEGER, minor INTEGER, micro INTEGER);";
	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
	if (rc == SQLITE_OK ) {
		printf("Create TABLE NXMP\n");
	}
	char sqlquery[64];
	sprintf(sqlquery,"INSERT INTO NXMP VALUES(%d, %d, %d)",VERSION_MAJOR,VERSION_MINOR,VERSION_MICRO);
	rc = sqlite3_exec(db, sqlquery, 0, 0, &err_msg);
	if (rc == SQLITE_OK ) {
		printf("Update DB Version\n");
		dbmajor = VERSION_MAJOR;
		dbminor = VERSION_MINOR;
		dbmicro = VERSION_MICRO;
	}
	char sqlquery2[128];
	sprintf(sqlquery2,"ALTER TABLE FILERESUME ADD COLUMN Completed INTEGER default 0");
	rc = sqlite3_exec(db, sqlquery2, 0, 0, &err_msg);
	if (rc == SQLITE_OK ) {
		printf("Added Completed Column\n");
	}
}


bool SQLiteDB::haveResume(std::string path){
	
	std::string sqlquery = "SELECT * from FILERESUME WHERE Path=";
	sqlquery.append("\"");
	sqlquery.append(path);
	sqlquery.append("\"");
	
	sqlite3_stmt *res;
	int rc = sqlite3_prepare_v2(db, sqlquery.c_str(), -1, &res, NULL);
	if (rc != SQLITE_OK) {
	
	}
	rc = sqlite3_step(res);
	
	bool _haveresume = false;
	if (rc == SQLITE_ROW) {
		_haveresume = true;
    }
	sqlite3_finalize(res);
	return _haveresume;
}

void SQLiteDB::writeResume(std::string path,int64_t position){
	if(haveResume(path)){
		updateResume(path,position);
	}else{
		newResume(path,position);
	}
}

void SQLiteDB::markCompleted(std::string path){
	if(haveResume(path)){
		updateCompleted(path);
	}else{
		newCompleted(path);
	}
}

void SQLiteDB::updateCompleted(std::string path){
	char sqlquery[2048];
	char *err_msg = 0;
	sprintf(sqlquery,"UPDATE FILERESUME set Completed = 1,Position = 0 WHERE Path = \"%s\"",path.c_str());
	printf("UPDATE: %s\n",sqlquery);
	int rc = sqlite3_exec(db, sqlquery, 0, 0, &err_msg);
	if (rc != SQLITE_OK) {
		printf("DB Error: %s\n",err_msg);
	}
}

void SQLiteDB::newCompleted(std::string path){
	char sqlquery[2048];
	char *err_msg = 0;
	sprintf(sqlquery,"INSERT INTO FILERESUME (Path,Position,Completed) VALUES(\"%s\",0,1);",path.c_str());
	printf("INSERT: %s\n",sqlquery);
	int rc = sqlite3_exec(db, sqlquery, 0, 0, &err_msg);
	if (rc != SQLITE_OK) {
		printf("DB Error: %s\n",err_msg);
	}
}


void SQLiteDB::newResume(std::string path,int64_t position){
	char sqlquery[2048];
	char *err_msg = 0;
	sprintf(sqlquery,"INSERT INTO FILERESUME (Path,Position,Completed) VALUES(\"%s\",%ld,0);",path.c_str(),position);
	printf("INSERT: %s\n",sqlquery);
	int rc = sqlite3_exec(db, sqlquery, 0, 0, &err_msg);
	if (rc != SQLITE_OK) {
		printf("DB Error: %s\n",err_msg);
	}
}

void SQLiteDB::updateResume(std::string path,int64_t position){
	char sqlquery[2048];
	char *err_msg = 0;
	sprintf(sqlquery,"UPDATE FILERESUME set Position = %ld,Completed = 0  WHERE Path = \"%s\"",position,path.c_str());
	printf("UPDATE: %s\n",sqlquery);
	int rc = sqlite3_exec(db, sqlquery, 0, 0, &err_msg);
	if (rc != SQLITE_OK) {
		printf("DB Error: %s\n",err_msg);
	}
}

int64_t SQLiteDB::getResume(std::string path){
	if(haveResume(path)){
		sqlite3_stmt *res;
		char sqlquery[2048];
		sprintf(sqlquery,"SELECT * from FILERESUME WHERE Path = \"%s\"",path.c_str());
		int rc = sqlite3_prepare_v2(db, sqlquery, -1, &res, NULL);
		rc = sqlite3_step(res);
		if (rc == SQLITE_ROW) {
			return sqlite3_column_int(res, 2);
		}
		sqlite3_finalize(res);
	}
	return 0;
}

void SQLiteDB::deleteResume(std::string path){
	if(haveResume(path)){
		char sqlquery[2048];
		char *err_msg = 0;
		sprintf(sqlquery,"DELETE FROM FILERESUME WHERE Path = \"%s\"",path.c_str());
		int rc = sqlite3_exec(db, sqlquery, 0, 0, &err_msg);
		if (rc != SQLITE_OK) {
			printf("DB Entry Delete Error: %s\n",err_msg);
		}
	
	}
}

int SQLiteDB::getFileDbStatus(std::string path){
	sqlite3_stmt *res;
	char sqlquery[2048];
	sprintf(sqlquery,"SELECT * from FILERESUME WHERE Path = \"%s\"",path.c_str());
	int rc = sqlite3_prepare_v2(db, sqlquery, -1, &res, NULL);
	rc = sqlite3_step(res);
	if (rc == SQLITE_ROW) {
		if(sqlite3_column_int(res, 3) == 1){
			return 2;
		}else{
			return 1;
		}
	}
	sqlite3_finalize(res);
	return 0;
	
}

std::string SQLiteDB::getDbVersion(){
	
	char outver[32];
	sprintf(outver,"v%d.%d.%d", dbmajor,dbminor,dbmicro);
		
	return outver;
}

std::string SQLiteDB::getSQLiteVersion(){
	return sqliteversion;
}

bool SQLiteDB::getCorrupted(){
	return dbcorrupted;
}

// code taken from https://stackoverflow.com/questions/1437327/saving-to-disk-an-in-memory-database

int SQLiteDB::loadOrSaveDb(sqlite3 *pInMemory, const char *zFilename, int isSave) 
{
   int rc;                   /* Function return code */
   sqlite3 *pFile;           /* Database connection opened on zFilename */
   sqlite3_backup *pBackup;  /* Backup object used to copy data */
   sqlite3 *pTo;             /* Database to copy to (pFile or pInMemory) */
   sqlite3 *pFrom;           /* Database to copy from (pFile or pInMemory) */


#ifdef __SWITCH__
	rc = sqlite3_open_v2(zFilename,&pFile,SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,"unix-none");
#endif
#ifdef _WIN32
	rc = sqlite3_open_v2(zFilename,&pFile,SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,"win32");
#endif
   if (rc == SQLITE_OK) 
   {
	   
	  rc = sqlite3_exec(pFile, "PRAGMA journal_mode=MEMORY;", 0, 0, 0);
	  if (rc != SQLITE_OK ) {
		printf("failed to set journal_mode\n");
	  }
	  rc = sqlite3_exec(pFile, "PRAGMA foreign_keys=ON;", 0, 0, 0);
	  if (rc != SQLITE_OK ) {
		printf("failed to set journal_mode\n");
	  }

      pFrom = (isSave ? pInMemory : pFile);
      pTo = (isSave ? pFile : pInMemory);

      pBackup = sqlite3_backup_init(pTo, "main", pFrom, "main");
      if (pBackup) {
         int rcstep = sqlite3_backup_step(pBackup, -1);
		 printf("RC STEP:%d\n",rcstep);
         int rcfinish = sqlite3_backup_finish(pBackup);
		 printf("RC FINISH:%d\n",rcfinish);
      }
      rc = sqlite3_errcode(pTo);
	  printf("pTO Errocode %d\n",rc);
   }else{
		printf("Error Opening File Database\n");
   }

   (void)sqlite3_close(pFile);
   return rc;
}

