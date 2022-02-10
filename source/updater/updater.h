#ifndef NXMP_UPDATER_H
#define NXMP_UPDATER_H


#include <string>
#include <vector>
#include <stdio.h>


typedef struct{
	std::string tagname;
	std::string downurl;
	bool prerelease;
}gitrelease_struct;

class Updater{
public:
	Updater();
	void fetchReleases();
	std::vector<gitrelease_struct> getReleases();
	
	
	
private:
	std::vector<gitrelease_struct> gitreleases;
	
	
};


#endif