#ifndef NXMP_SQLITEDB_H
#define NXMP_SQLITEDB_H

#include <string> 
#include <sqlite3.h>

class SQLiteDB{
public:
	SQLiteDB(std::string _filename);
	~SQLiteDB();
	
	bool haveResume(std::string path);
	void writeResume(std::string path,int64_t position);
	void deleteResume(std::string path);
	void newResume(std::string path,int64_t position);
	void updateResume(std::string path,int64_t position);
	int64_t getResume(std::string path);
	std::string getDbVersion();
	std::string getSQLiteVersion();
	bool getCorrupted();
	
	int loadOrSaveDb(sqlite3 *pInMemory, const char *zFilename, int isSave);
	
private:


	bool dbcorrupted = false;
	int dbmajor;
	int dbminor;
	int dbmicro;
	std::string dbfilename;
	std::string sqliteversion;
	sqlite3 *db;
};

#endif