#include "SQLiteDB.h"


SQLiteDB::SQLiteDB(std::string _filename){
	char cwd[256];
	
	NXLOG::DEBUGLOG("Opening DB\n");
	fflush(stdout);
	dbfilename = _filename;
	sqlite3_stmt *res;
	bool haveversion = false;
	int rc = sqlite3_open_v2(_filename.c_str(),&db,SQLITE_OPEN_READWRITE,"HOS_VFS");
	if (rc != SQLITE_OK ) {
		NXLOG::DEBUGLOG("Error Opening DB: %d\n",rc);
	}
	char *err_msg = 0;
	
	
	
	
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
		NXLOG::ERRORLOG("Error Creating Table NXMP\n");
	}
	
	char sqlfileresume[] = "CREATE TABLE IF NOT EXISTS FILERESUME(Id INTEGER PRIMARY KEY AUTOINCREMENT , Path TEXT, Position INTEGER,Completed INTEGER default 0);";
	rc = sqlite3_exec(db, sqlfileresume, 0, 0, &err_msg);
	if (rc != SQLITE_OK ) {
		NXLOG::ERRORLOG("Error Creating Table FILERESUME\n");
	}
	
	NXLOG::DEBUGLOG("DB JOURNAL MODE\n");
	rc = sqlite3_exec(db, "PRAGMA journal_mode=DELETE;", 0, 0, 0);
	if (rc != SQLITE_OK ) {
		NXLOG::DEBUGLOG("failed to set journal_mode\n");
	}
	
	rc = sqlite3_prepare_v2(db, "SELECT * from NXMP", -1, &res, NULL);
	if (rc != SQLITE_OK) {
	
	}
	
	rc = sqlite3_step(res);
	
	
	if (rc == SQLITE_ROW) {
		dbmajor = sqlite3_column_int(res, 0);
		dbminor = sqlite3_column_int(res, 1);
		dbmicro = sqlite3_column_int(res, 2);
		NXLOG::DEBUGLOG("File DB Version %d.%d.%d\n", dbmajor,dbminor,dbmicro);
		haveversion = true;
    }
	
	sqlite3_finalize(res);
	
	if(!haveversion){
		char sqlquery[64];
		sprintf(sqlquery,"INSERT INTO NXMP VALUES(%d, %d, %d)",VERSION_MAJOR,VERSION_MINOR,VERSION_MICRO);
		rc = sqlite3_exec(db, sqlquery, 0, 0, &err_msg);
		
		if (rc != SQLITE_OK ) {
			dbcorrupted = true;
			NXLOG::ERRORLOG("Error Insert %s\n",err_msg);
			
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
			NXLOG::DEBUGLOG("DB Same Version\n");
		}else{
			NXLOG::DEBUGLOG("DB Version Mismatch\n");
			dbUpdated = true;
			UpdateDB();
		}
		
	}
	
	//this->UpdateDbStats();
	
	
}

SQLiteDB::~SQLiteDB(){
	sqlite3_close_v2(db);
	sqlite3_os_end();
	NXLOG::DEBUGLOG("Deallocator SQLITEDB\n");
}

bool SQLiteDB::dbwasUpdated(){
	return dbUpdated;
}

void SQLiteDB::UpdateDB(){
	char *err_msg = 0;
	int rc = sqlite3_exec(db, "DROP TABLE NXMP", 0, 0, &err_msg);
	if (rc == SQLITE_OK ) {
		NXLOG::DEBUGLOG("Drop NXMP Version Table\n");
	}
	char sql[] = "CREATE TABLE IF NOT EXISTS NXMP(major INTEGER, minor INTEGER, micro INTEGER);";
	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
	if (rc == SQLITE_OK ) {
		NXLOG::DEBUGLOG("Create TABLE NXMP\n");
	}
	char sqlquery[64];
	sprintf(sqlquery,"INSERT INTO NXMP VALUES(%d, %d, %d)",VERSION_MAJOR,VERSION_MINOR,VERSION_MICRO);
	rc = sqlite3_exec(db, sqlquery, 0, 0, &err_msg);
	if (rc == SQLITE_OK ) {
		NXLOG::DEBUGLOG("Update DB Version\n");
		dbmajor = VERSION_MAJOR;
		dbminor = VERSION_MINOR;
		dbmicro = VERSION_MICRO;
	}
	char sqlquery2[128];
	sprintf(sqlquery2,"ALTER TABLE FILERESUME ADD COLUMN Completed INTEGER default 0");
	rc = sqlite3_exec(db, sqlquery2, 0, 0, &err_msg);
	if (rc == SQLITE_OK ) {
		NXLOG::DEBUGLOG("Added Completed Column\n");
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
	sqlite3_finalize(res);
	
	bool _haveresume = false;
	if (rc == SQLITE_ROW) {
		_haveresume = true;
    }
	
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
	NXLOG::DEBUGLOG("UPDATE: %s\n",sqlquery);
	int rc = sqlite3_exec(db, sqlquery, 0, 0, &err_msg);
	if (rc != SQLITE_OK) {
		NXLOG::ERRORLOG("DB Error: %s\n",err_msg);
	}
}

void SQLiteDB::newCompleted(std::string path){
	char sqlquery[2048];
	char *err_msg = 0;
	sprintf(sqlquery,"INSERT INTO FILERESUME (Path,Position,Completed) VALUES(\"%s\",0,1);",path.c_str());
	NXLOG::DEBUGLOG("INSERT: %s\n",sqlquery);
	int rc = sqlite3_exec(db, sqlquery, 0, 0, &err_msg);
	if (rc != SQLITE_OK) {
		NXLOG::ERRORLOG("DB Error: %s\n",err_msg);
	}
}


void SQLiteDB::newResume(std::string path,int64_t position){
	char sqlquery[2048];
	char *err_msg = 0;
	sprintf(sqlquery,"INSERT INTO FILERESUME (Path,Position,Completed) VALUES(\"%s\",%ld,0);",path.c_str(),position);
	NXLOG::DEBUGLOG("INSERT: %s\n",sqlquery);
	int rc = sqlite3_exec(db, sqlquery, 0, 0, &err_msg);
	if (rc != SQLITE_OK) {
		NXLOG::ERRORLOG("DB Error: %s\n",err_msg);
	}
}

void SQLiteDB::updateResume(std::string path,int64_t position){
	char sqlquery[2048];
	char *err_msg = 0;
	sprintf(sqlquery,"UPDATE FILERESUME set Position = %ld,Completed = 0  WHERE Path = \"%s\"",position,path.c_str());
	NXLOG::DEBUGLOG("UPDATE: %s\n",sqlquery);
	int rc = sqlite3_exec(db, sqlquery, 0, 0, &err_msg);
	if (rc != SQLITE_OK) {
		NXLOG::ERRORLOG("DB Error: %s\n",err_msg);
	}
}

int64_t SQLiteDB::getResume(std::string path){
	int myret = 0;
	if(haveResume(path)){
		sqlite3_stmt *res;
		char sqlquery[2048];
		sprintf(sqlquery,"SELECT * from FILERESUME WHERE Path = \"%s\"",path.c_str());
		int rc = sqlite3_prepare_v2(db, sqlquery, -1, &res, NULL);
		rc = sqlite3_step(res);
		
		if (rc == SQLITE_ROW) {
			myret =  sqlite3_column_int(res, 2);
		}
		sqlite3_finalize(res);
	}
	return myret;
}

void SQLiteDB::deleteResume(std::string path){
	if(haveResume(path)){
		char sqlquery[2048];
		char *err_msg = 0;
		sprintf(sqlquery,"DELETE FROM FILERESUME WHERE Path = \"%s\"",path.c_str());
		int rc = sqlite3_exec(db, sqlquery, 0, 0, &err_msg);
		if (rc != SQLITE_OK) {
			NXLOG::ERRORLOG("DB Entry Delete Error: %s\n",err_msg);
		}
	
	}
}

int SQLiteDB::getFileDbStatus(std::string path){
	sqlite3_stmt *res;
	char sqlquery[2048];
	sprintf(sqlquery,"SELECT * from FILERESUME WHERE Path = \"%s\"",path.c_str());
	int rc = sqlite3_prepare_v2(db, sqlquery, -1, &res, NULL);
	rc = sqlite3_step(res);
	int statusret = 0;
	
	if (rc == SQLITE_ROW) {
		if(sqlite3_column_int(res, 3) == 1){
			statusret = 2;
		}else{
			statusret =  1;
		}
	}
	sqlite3_finalize(res);
	return statusret;
	
}

std::string SQLiteDB::getDbVersion(){
	
	char outver[32];
	sprintf(outver,"v%d.%d.%d", dbmajor,dbminor,dbmicro);
		
	return outver;
}

void SQLiteDB::GetDbStats(int &recnum,int &reccomp){
	recnum = recordCount;
	reccomp = completedCount;
}

static int rowcount_callback(void *count, int argc, char **argv, char **azColName) {
    int *c = (int *)count;
    *c = atoi(argv[0]);
    return 0;
}

void SQLiteDB::UpdateDbStats(){
	NXLOG::DEBUGLOG("Updating Db Stats\n");
	int rc = sqlite3_exec(db, "select count(*) from FILERESUME", rowcount_callback, &recordCount, NULL);
    if (rc != SQLITE_OK) {
		NXLOG::DEBUGLOG("ERROR ON COUNT RECORD\n");
        recordCount = 0;
    }
	rc = sqlite3_exec(db, "select count(*) from FILERESUME WHERE Completed==1", rowcount_callback, &completedCount, NULL);
    if (rc != SQLITE_OK) {
		NXLOG::DEBUGLOG("ERROR ON COMPLETED RECORD\n");
        completedCount = 0;
    }
}

std::string SQLiteDB::getSQLiteVersion(){
	return sqliteversion;
}

bool SQLiteDB::getCorrupted(){
	return dbcorrupted;
}
