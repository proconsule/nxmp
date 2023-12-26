/* UGLY FTP lib based on https://github.com/codebrainz/ftplib/blob/master/ftplib.c */

#include "ugly-ftplib.h"
#include <sys/poll.h>

#include <string.h>

#include <algorithm> 
#include <functional> 
#include <regex>
#include <sys/types.h>
#include <sys/stat.h>

#include <iomanip>
#include <iostream>
#include <locale>
#include <sstream>

#include <fcntl.h>

#include <time.h>

extern "C" {
#include "ftpparse.h"
}

int controlSocket = 0;

int ftp_net_read(int fd, char *buf, size_t len) {
	
/* TIMEOUT TEST  */
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	fd_set pfd, *rfd = nullptr;
	rfd = &pfd;
	FD_ZERO(&pfd);
	
	while(true){
		FD_SET(fd, &pfd);
		int rv = select(fd+1, rfd, nullptr, nullptr, &tv);
		if(rv==1){
			break;
		}else{
			return -1;
		}
		
	
	}
	
	
		while (1) {
			int c = recv(fd, buf, len,0 );
			if (c == -1) {
				if (errno != EINTR && errno != EAGAIN)
					return -1;
			} else {
				return c;
			}
		}
	
	
}


int ftp_net_write(int fd, const char *buf, size_t len) {
    int done = 0;
    while (len > 0) {
        int c = write(fd, buf, len);
        if (c == -1) {
            if (errno != EINTR && errno != EAGAIN)
                return -1;
        } else if (c == 0) {
            return done;
        } else {
            buf += c;
            done += c;
            len -= c;
        }
    }
    return done;
}


bool socketStatus(int socket){
	if(socket<=1)return false;
	struct timeval tv;
    fd_set  fd;
    int result = 0;

    tv.tv_sec  = 2;
    tv.tv_usec = 0;

    FD_ZERO(&fd);
    FD_SET(socket, &fd);

    result = select(socket + 1, &fd, 0, 0, &tv);
	if(result>0){
		return true;
	}
	return false;
}


int net_close(int socket){
	close(socket);
	svcSleepThread(10'000'000);
	return 0;
}


void wait_socket(int nData){
	
	
	
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	fd_set fd, *rfd = nullptr;
	rfd = &fd;
	FD_ZERO(&fd);
	
	while(true){
		FD_SET(nData, &fd);
		int rv = select(nData + 1, rfd, nullptr, nullptr, &tv);
		if(rv==1){
			break;
		}
	
	}
	

	/*

	struct pollfd fds[2];
	int    nfds = 1;
	memset(fds, 0 , sizeof(fds));
	fds[0].fd = nData;
	fds[0].events = POLLIN;
	while(true){
		int rc = poll(fds, nfds, 0);
		break;
	}
	
	/*
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	ffd_set fd, *rfd = nullptr;
	rfd = &fd;
	FD_ZERO(&fd);
	while(true){
		rc = poll(fds, nfds, timeout);
	}
	*/
	
}


int parse_ftp_resp(std::string resp){
	int resnum = resp.find_first_of(" ");
	if(resnum <=0){
		return -1;
	}
	int code = std::stoi(resp.substr(0,resnum));
	std::string resstr = resp.substr(resnum+1);
	
	//printf("CODE: %d , %s\n",code,resstr.c_str());
	return code;
	
}

int parse_ftp_size(std::string resp,int &size){
	int resnum = resp.find_first_of(" ");
	int newlinenum = resp.find_first_of("\n");
	if(resnum <=0){
		return 0;
	}
	
	//int ret = sscanf(resp.data(),"%d %d",&code,size);
	int code = std::stoi(resp.substr(0,resnum));
	if(code == 213){
		size = std::stoi(resp.substr(resnum+1,newlinenum-1));
	}else{
		size = 0;
	}
	
	
	
	return code;
	
}


int16_t parse_epsv(std::string resp){
	
	resp = resp.substr(resp.find_first_of("|")+3);
	resp = resp.substr(0,resp.find_first_of("|"));
	
	int16_t port = 0;
	sscanf(resp.c_str(),"%hu",&port);
	return port;
}

void parse_pasv(std::string resp,char * ret){
	int i1 = resp.find_first_of("(")+1;
	int i2 = resp.find_last_of(")");
	std::string pasv_string = resp.substr(i1,i2-i1);
	
	int res = sscanf(pasv_string.data(), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &ret[2], &ret[3], &ret[4], &ret[5], &ret[0], &ret[1]);
	
}


int UFTP_Connect(std::string _host,int16_t _port){
	struct sockaddr_in sin;
	int on = 1;
	
	memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(_host.data());
    sin.sin_port = htons(_port);
	
	
	int sControl = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sControl == -1) {
        return -1;
    }
    if (setsockopt(sControl, SOL_SOCKET, SO_REUSEADDR,
                   (void *) &on, sizeof(on)) == -1) {
        net_close(sControl);
        return -1;
    }
    if (connect(sControl, (struct sockaddr *) &sin, sizeof(sin)) == -1) {
        net_close(sControl);
        return -1;
    }
	
	ftp_message msg = UFTP_ReadServerCommand(sControl);
	
	
	return sControl;
}


int UFTP_Login(server_connection * connection) {
	
	
	std::string u_string = "USER " + std::string(connection->user) + "\r\n";
	std::string p_string = "PASS " + std::string(connection->pass) + "\r\n";
	ftp_message msg = UFTP_SendCommand(connection->control_socket,u_string);
	if(msg.code != 331){
		return -1;
	}
	msg = UFTP_SendCommand(connection->control_socket,p_string);
	if(msg.code != 230){
		return -1;
	}
	
	return 0;
	
	
}


int UFTP_OpenDirListing(server_connection *myconnection,std::string path) {
	
	int rc = -1;
	if(myconnection->extra_feat_support.epsv){
		rc = UFTP_OpenPortEPSV(myconnection);
	}else{
		rc = UFTP_OpenPortPASV(myconnection);
	}
	if(rc == -1)return -1;
	
	std::string listcommand = "LIST " +  path + "\r\n";
	ftp_message msg = UFTP_SendCommand(myconnection->control_socket,listcommand);
	if(msg.code != 150){
		return -1;
	}
	wait_socket(myconnection->data_socket);
	return 0;
		
}

bool UFTP_SetBlocking(int fd,bool val){
   int flags = fcntl(fd, F_GETFL, 0);
   if (flags == -1) return false;
   flags = val ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
   return (fcntl(fd, F_SETFL, flags) == 0) ? true : false;}


int UFTP_OpenPortEPSV(server_connection *myconnection) {
	
	
	std::string listcommand = "EPSV\r\n";
	int ret;
	int16_t epsvport = 0;
	
	struct linger lng = {0, 0};
	
	ftp_message msg = UFTP_SendCommand(myconnection->control_socket,listcommand);
	int16_t _port = parse_epsv(msg.resstring);
	
	struct sockaddr_in sin;
	int on = 1;
	
	memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(myconnection->host);
    sin.sin_port = htons(_port);
	
	
	myconnection->data_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (setsockopt(myconnection->data_socket, SOL_SOCKET, SO_REUSEADDR,
                   (void *) &on, sizeof(on)) == -1) {
        net_close(myconnection->data_socket);
        return -1;
    }
	
	if (setsockopt(myconnection->data_socket, SOL_SOCKET, SO_LINGER,
                   (void *) &lng, sizeof(lng)) == -1) {
        net_close(myconnection->data_socket);
        return -1;
    }
	
	
	if (setsockopt(myconnection->data_socket, SOL_SOCKET, SO_REUSEADDR,
                   (void *) &on, sizeof(on)) == -1) {
        net_close(myconnection->data_socket);
        return -1;
    }
	
	
	if (connect(myconnection->data_socket, (struct sockaddr *) &sin, sizeof(sin)) == -1) {
		net_close(myconnection->data_socket);
		return -1;
	}
	
	
	return 0;
}


int UFTP_OpenPortPASV(server_connection *myconnection) {
	
	
	std::string listcommand = "PASV\r\n";
	char buf[8192];
	int ret;
	char v[6];
	ftp_net_write(myconnection->control_socket,listcommand.c_str(),listcommand.length());
	ret = read(myconnection->control_socket, buf, sizeof(buf)-1);
	parse_pasv(buf,v);
	
	
	
	
	//int sData = 0;
	int on = 1;
	struct linger lng = {0, 0};
	union {
        struct sockaddr sa;
        struct sockaddr_in in;
    } sin;
	memset(&sin, 0, sizeof(sin));
	sin.in.sin_family = AF_INET;
	
	sin.sa.sa_data[2] = v[2];
	sin.sa.sa_data[3] = v[3];
	sin.sa.sa_data[4] = v[4];
	sin.sa.sa_data[5] = v[5];
	sin.sa.sa_data[0] = v[0];
	sin.sa.sa_data[1] = v[1];
	
	
	myconnection->data_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (setsockopt(myconnection->data_socket, SOL_SOCKET, SO_REUSEADDR,
                   (void *) &on, sizeof(on)) == -1) {
        net_close(myconnection->data_socket);
        return -1;
    }
	
	if (setsockopt(myconnection->data_socket, SOL_SOCKET, SO_LINGER,
                   (void *) &lng, sizeof(lng)) == -1) {
        net_close(myconnection->data_socket);
        return -1;
    }
	
	
	if (setsockopt(myconnection->data_socket, SOL_SOCKET, SO_REUSEADDR,
                   (void *) &on, sizeof(on)) == -1) {
        net_close(myconnection->data_socket);
        return -1;
    }
	
	
	if (connect(myconnection->data_socket, &sin.sa, sizeof(sin.sa)) == -1) {
		net_close(myconnection->data_socket);
		return -1;
	}
	
	return 0;
}


int UFTP_DirReadline(int nData,char* buf) {
	int idx = 0;
	
	while(true){
		char tbuf[1] = {0};
		int readnum = ftp_net_read(nData,tbuf,1);
		if(readnum == 0 || readnum == -1)break;
		buf[idx] = tbuf[0];
		if(buf[idx] == '\n'){
			return idx;
		}
		if(idx>=8191)return 0;
		idx++;
		
	}
	return idx;
}


static long getlong(char *buf, int len) {
    long u = 0;
    while (len-- > 0)
        u = u * 10 + (*buf++ - '0');
    return u;
}

int UFTP_ParseDirLine2(char *linebuf,std::string &filename,struct stat * st) {
	struct ftpparse fp{};
	if (ftpparse(&fp, linebuf, strlen(linebuf)) == 1) {
		if (fp.flagtrycwd && fp.flagtryretr) {
			// skip links for now
			return 0;
		}
		std::string name = fp.name;
		if (name.back() == '\n') {
			name.erase(name.size() - 1);
		}
		if (name.back() == '\r') {
			name.erase(name.size() - 1);
		}
		filename = name;
		st->st_size = fp.size;
		st->st_mtime = fp.mtime;
		st->st_mode = fp.flagtrycwd == 1 ? S_IFDIR:S_IFREG;
		
	}
	return 1;
}

int UFTP_ParseDirLine(char *linebuf,std::string &filename,struct stat * st) {
	std::regex re("^([\\-ld])([\\-rwxs]{9})\\s+(\\d+)\\s+(\\w+)\\s+(\\w+)\\s+(\\d+)\\s+(\\w{3}\\s+\\d{1,2}\\s+(?:\\d{1,2}:\\d{1,2}|\\d{4}))\\s+(.+)");
		std::smatch match;
		std::string testmatch = linebuf;
		//memset(filename,0,2048);
		if (regex_search(testmatch, match, re) == true) {
			if(match.size()<9)return 0;
			//printf("MATCH %s\n",match.str(0).c_str());
			for(int i=0;i<match.size();i++){
				//printf("G: %d\n",match[i].groups.size());
				//printf("M: %s\n",match.str(i).c_str());
			}
			
			if(std::string(match.str(1)) == "d"){
				st->st_mode = S_IFDIR;
			}else if(std::string(match.str(1)) == "l"){
				return 0;
			}else{
				st->st_mode = S_IFREG;
			}
			st->st_size =getlong((char *)match.str(6).c_str(),match.str(6).length());
			struct tm ltm = {0};
			std::stringstream ss{match.str(7).c_str()};
			ss >> std::get_time(&ltm, "%b %d %H:M");
			//strptime(buf, "%b %d %H:M", &ltm);
			st->st_mtime = mktime(&ltm);
			if (ss.fail()){
				printf("FAIL PARSE\n");
			}else{
				printf("TIME: %d\n",st->st_mtime);
			}
			
			printf("%s\n",match.str(7).c_str());
			//strncpy(filename,match.str(8).c_str(),match.str(8).length());
			filename = match.str(8);
			return 1;
		}else{
			//printf("NO MATCH\n");
			return -1;
		}
		return 0;
}

void UFTP_DataClose(int data_socket){
	shutdown(data_socket,2);
	net_close(data_socket);
}

off_t UFTP_FTPSize(int c_socket,std::string filename){
	
	std::string listcommand = "SIZE " +  filename + "\r\n";
	ftp_message msg = UFTP_SendCommand(c_socket,listcommand);
	if(msg.code!= 213)return -1;
	off_t size = std::stoll(msg.resstring.data());
	
	return size;
}

int UFTP_FTPModDate(int c_socket,std::string filename){
	
	std::string listcommand = "MDTM " +  filename + "\r\n";
	ftp_message msg = UFTP_SendCommand(c_socket,listcommand);
	
	struct tm ts;
	memset(&ts, 0, sizeof(ts));
	sscanf(msg.resstring.data(), "%*s %04d%02d%02d%02d%02d%02d", &ts.tm_year,
           &ts.tm_mon, &ts.tm_mday, &ts.tm_hour, &ts.tm_min, &ts.tm_sec);
	ts.tm_year -= 1900;
    ts.tm_mon--;
	return mktime(&ts);
	
}


  int UFTP_OpenFile(server_connection * myconnection,std::string filename,int offset){
	
	if(myconnection->connected){
		int rc = -1;
		if(myconnection->extra_feat_support.epsv){
			rc = UFTP_OpenPortEPSV(myconnection);
		}else{
			rc = UFTP_OpenPortPASV(myconnection);
		}
		if(rc == -1)return -1;
		
		
		std::string typecommand = "TYPE I\r\n";
		ftp_message msg = UFTP_SendCommand(myconnection->control_socket,typecommand);
		if(msg.code != 200)return -1;
		
		std::string restcommand = "REST " +  std::to_string(offset) + "\r\n";
		msg = UFTP_SendCommand(myconnection->control_socket,restcommand);
		if(msg.code != 350)return -1;
		
		std::string retrcommand = "RETR " +  filename + "\r\n";
		msg = UFTP_SendCommand(myconnection->control_socket,retrcommand);
		if(msg.code == 150){
			wait_socket(myconnection->data_socket);
			//printf("DATA SOCKET!\n");
			return 0;
		}
	}
	return -1;
}


void UFTP_Init_Server_Connection(server_connection * newconnection,std::string _host,int16_t _port,std::string _user,std::string _pass){
	
	memset(newconnection->host,0,sizeof(newconnection->host));
	memset(newconnection->user,0,sizeof(newconnection->user));
	memset(newconnection->pass,0,sizeof(newconnection->pass));
	
	
	strncpy(newconnection->host,_host.data(),_host.length());
	strncpy(newconnection->user,_user.data(),_user.length());
	strncpy(newconnection->pass,_pass.data(),_pass.length());
	
	newconnection->port = _port;
	newconnection->control_socket = -1;
	newconnection->data_socket = -1;
	
}


int UFTP_ServerConnect( server_connection * newconnection){

	
	int ret = UFTP_Connect2(newconnection);
	if(ret<0){
		return -1;
	}
	ret = UFTP_Login(newconnection);
	newconnection->extra_feat_support.epsv = true;
	//UFTP_CheckFeat(newconnection);
	if(ret==0){

		newconnection->connected = true;
		return 0;
	}
	
	return -1;
}


void UFTP_CloseServer(server_connection * myconnection){
	if(myconnection->data_socket != -1){
		net_close(myconnection->data_socket);
		myconnection->data_socket = -1;
		ftp_message msg = UFTP_ReadServerCommand(myconnection->control_socket);
	
	}
	
	if(myconnection->control_socket != -1){
		UFTP_SendCommand(myconnection->control_socket,"QUIT\r\n");
		ftp_message msg = UFTP_ReadServerCommand(myconnection->control_socket);
		net_close(myconnection->control_socket);
		myconnection->control_socket = -1;
	}
	
	myconnection->connected = false;
	
}

bool data_is_connected(int sock)
{
    unsigned char buf;
    int err = recv(sock,&buf,1,MSG_PEEK);
    return err == -1 ? false : true;
}

void UFTP_AbortTransfer(server_connection * myconnection){
	
	if(myconnection->data_socket > -1){
		shutdown(myconnection->data_socket,2);
		net_close(myconnection->data_socket);
		ftp_message msg = UFTP_ReadServerCommand(myconnection->control_socket);
		myconnection->data_socket = -1;
	}
}


int UFTP_Connect2(server_connection * connection){
	
	if(connection == nullptr){
		//printf("Connect2 nullptr\n");
		return -1;
	}
	struct sockaddr_in sin;
	int on = 1;
	
	memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(connection->host);
    sin.sin_port = htons(connection->port);
	
	
	int sControl = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sControl == -1) {
        return -1;
    }
	
    if (setsockopt(sControl, SOL_SOCKET, SO_REUSEADDR,
                   (void *) &on, sizeof(on)) == -1) {
        net_close(sControl);
        return -1;
    }
    if (connect(sControl, (struct sockaddr *) &sin, sizeof(sin)) == -1) {
        net_close(sControl);
        return -1;
    }
	
	ftp_message msg = UFTP_ReadServerCommand(sControl);
	
	connection->control_socket = sControl;
	
	return 0;
}


void UFTP_ReconnectControl(server_connection * connection){
	int ret = UFTP_Connect2(connection);
	UFTP_Login(connection);
	
}

ftp_message UFTP_ReadServerCommand(int c_socket){
	ftp_message msg;
	msg.code = -1;
	int ret;
	char buf[8192] = {0};
	ret = ftp_net_read(c_socket, buf, sizeof(buf)-1);
	std::string resp = buf;
	int resnum = resp.find_first_of(" ");
	if(resnum <=0){
		return msg;
	}
	msg.code = std::stoi(resp.substr(0,resnum));
	msg.resstring = resp.substr(resnum+1);
	return msg;
}

ftp_message UFTP_SendCommand(int c_socket,std::string command){
	ftp_message msg;
	msg.code = -1;
	int ret;
	char buf[8192] = {0};
	ftp_net_write(c_socket,command.c_str(),command.length());
	
	ret = ftp_net_read(c_socket, buf, sizeof(buf)-1);
	std::string resp = buf;
	int resnum = resp.find_first_of(" ");
	if(resnum <=0){
		return msg;
	}
	msg.code = std::stoi(resp.substr(0,resnum));
	msg.resstring = resp.substr(resnum+1);
	
	return msg;
}


int UFTP_STATFile(server_connection * connection,std::string filename, struct stat * st ){
	
	struct stat myst;
	
	off_t fsize = UFTP_FTPSize(connection->control_socket,filename);
	if(fsize <=0)return -1;
	int mtime = 0;
	if(fsize>0){
		mtime = UFTP_FTPModDate(connection->control_socket,filename);
	}
	
	st->st_mode = S_IFREG;
	if(fsize == 0)st->st_mode = S_IFDIR;
	st->st_mtime = mtime;
	st->st_size = fsize; 
	
	return 0;
}

int UFTP_CheckFeat(server_connection *connection){
	
	int ret;
	char buf[8192];
	memset(buf,0,sizeof(buf));
	std::string command = "FEAT\r\n";
	ftp_net_write(connection->control_socket,command.c_str(),command.length());
	svcSleepThread(100000000);
	ret = read(connection->control_socket, buf, sizeof(buf)-1);
	std::string resp = buf;
	
	bool size_support = false;
	bool epsv_support = false;
	bool pasv_support = false;
	bool rest_support = false;
	
	if(resp.find("EPSV")!=std::string::npos){
		connection->extra_feat_support.epsv = true;
	}
	if(resp.find("SIZE")!=std::string::npos){
		size_support = true;
	}
	if(resp.find("REST")!=std::string::npos){
		rest_support = true;
	}
	if(!size_support||!rest_support){
		printf("FTP SERVER NOT SUPPORTED\n");
		return -1;
	}
	
	
	return 0;
}



int UFTP_MLSDList(server_connection * connection,std::string path){
	
	int rc = -1;
	if(connection->extra_feat_support.epsv){
		rc = UFTP_OpenPortEPSV(connection);
	}else{
		rc = UFTP_OpenPortPASV(connection);
	}
	if(rc == -1)return -1;
	
	std::string listcommand = "MLSD " +  path + "\r\n";
	ftp_message msg = UFTP_SendCommand(connection->control_socket,listcommand);
	if(msg.code != 150){
		return -1;
	}
	wait_socket(connection->data_socket);
	
	
	char buf[8192];
	memset(buf,0,sizeof(buf));
	
	
	while(UFTP_DirReadline(connection->data_socket,buf) > 0){
		//dircache entry;
		char *p = strtok(buf, ";"); 
		while (p != NULL) {
			//printf("%s\n",p);
			p = strtok(NULL, ";");
		}
		//printf("Line %s\n",buf);
		memset(buf,0,8192);
	}
	//UFTP_CloseServer(connection);
	
	return 0;
}
