#ifndef UGLY_FTPLIB_H
#define UGLY_FTPLIB_H

#include <string>
#include <vector>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cctype>

#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <switch.h>

typedef struct {
	bool epsv = false;
}extra_feat_support_struct;

typedef struct {
	int control_socket = -1;
	int data_socket = -1;
	//char data_buf[8192];
	char host[64];
	uint16_t port = 21;
	char user[64];
	char pass[64];
	char path[1024];
	bool connected = false;
	extra_feat_support_struct extra_feat_support;
	
}server_connection;

struct ftp_message{
	int code;
	std::string resstring;
};



int UFTP_Connect(std::string _host,int16_t _port);
int UFTP_Connect2(server_connection * connection);
int UFTP_Login(server_connection * connection);
int UFTP_OpenDirListing(server_connection * myconnection,std::string path);
int UFTP_OpenPASV(int c_socket);
int UFTP_OpenPortPASV(server_connection * myconnection);
int UFTP_OpenPortEPSV(server_connection *myconnection);
int UFTP_DirReadline(int nData,char* buf);
int UFTP_ParseDirLine(char *linebuf,std::string &filename,struct stat * st);
int UFTP_ParseDirLine2(char *linebuf,std::string &filename,struct stat * st);

void UFTP_DataClose(int data_socket);

int UFTP_OpenDirMLSD(int c_socket,std::string path, int *nData);

off_t UFTP_FTPSize(int c_socket,std::string filename);
int UFTP_FTPModDate(int c_socket,std::string filename);

int UFTP_OpenFile(server_connection * myconnection,std::string filename,int offset);

void UFTP_CloseServer(server_connection * myconnection);

void UFTP_AbortTransfer(server_connection * myconnection);

int ftp_net_read(int fd, char *buf, size_t len);

int UFTP_ServerConnect(server_connection * newconnection);
void UFTP_Init_Server_Connection(server_connection * newconnection,std::string _host,int16_t _port,std::string _user,std::string pass);

void UFTP_ReconnectControl(server_connection * connection);

ftp_message UFTP_SendCommand(int c_socket,std::string command);
ftp_message UFTP_ReadServerCommand(int c_socket);
int UFTP_STATFile(server_connection * connection,std::string filename, struct stat * st );
int UFTP_CheckFeat(server_connection *connection);
int UFTP_MLSDList(server_connection * connection,std::string path);

bool UFTP_SetBlocking(int fd,bool val);
	

#endif