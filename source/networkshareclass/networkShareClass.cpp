#include "networkShareClass.h"


CNetworkShare::CNetworkShare(){
	
	name = "New Share 1";
	anon = true;
	port = 0;
}

CNetworkShare::~CNetworkShare(){
	
	
}

bool CNetworkShare::isValidShare(){
	if(name=="")return false;

	if(type== 0){
		if(httpstring == "")return false;
	}else{
		if(address=="")return false;
		if(!anon && username == "")return false;
		if(!anon && password == "")return false;
		if(remoteshare == "" )return false;
	}

	return true;
}


oldnetworkSource CNetworkShare::GenConfigLine(){
	if(type==0){
		oldnetworkSource tmpret;
		tmpret.name = name;
		tmpret.url = httpstring;
		return tmpret;
	}
	if(type==1){
		oldnetworkSource tmpret;
		std::string urlstring;
		tmpret.name = name;
		if(anon){
			urlstring = "ftp://" + address + remoteshare;
		}else{
			urlstring = "ftp://"+ username+":" + password + "@" + address + ":" + std::to_string(port) + remoteshare;
		}
		
		tmpret.url = urlstring;
		return tmpret;
	}
	if(type==2){
		oldnetworkSource tmpret;
		std::string urlstring;
		tmpret.name = name;
		if(anon){
			urlstring = "sftp://" + address + remoteshare;
		}else{
			urlstring = "sftp://"+ username+":" + password + "@" + address + ":" + std::to_string(port) + remoteshare;
		}
		
		tmpret.url = urlstring;
		return tmpret;
	}
	
	if(type==3){
		oldnetworkSource tmpret;
		std::string urlstring;
		tmpret.name = name;
		if(anon){
			urlstring = "smb://" + address + remoteshare;
		}else{
			urlstring = "smb://"+ username+":" + password + "@" + address + remoteshare;
		}
		
		tmpret.url = urlstring;
		return tmpret;
	}
	if(type==4){
		oldnetworkSource tmpret;
		std::string urlstring;
		tmpret.name = name;
		if(anon){
			urlstring = "nfs://" + address + remoteshare;
		}else{
			urlstring = "nfs://"+ username+":" + password + "@" + address + remoteshare;
		}
		
		tmpret.url = urlstring;
		return tmpret;
	}
	
	oldnetworkSource tmpret;
	return tmpret;
}