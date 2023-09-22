#ifndef NXMP_NETWORKSHARECLASS_H
#define NXMP_NETWORKSHARECLASS_H

#include <string>
#include <vector>
#include "iniparser.h"


class CNetworkShare{
public:
	CNetworkShare();
	~CNetworkShare();
	
	oldnetworkSource GenConfigLine();
	
	bool isValidShare();
	
	char const *protonames[5] = {"HTTP","FTP","SSH","SMB","NFS"};
	int protodefports[5] = {80,21,22,0,0};

	std::string name = "";
	unsigned int type;
	
	std::string httpstring = "";
	
	bool anon;
	std::string address = "";
	unsigned int port;
	std::string username= "";
	std::string password = "";
	std::string remoteshare = "";
	
	
};



#endif