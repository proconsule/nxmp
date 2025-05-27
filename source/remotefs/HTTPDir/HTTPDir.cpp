#include "HTTPDir.h"

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct HTTPMemoryStruct *mem = (struct HTTPMemoryStruct *)userp;
 
  char *ptr = (char *)realloc(mem->memory, mem->size + realsize + 1);
  if(!ptr) {
    /* out of memory! */
    NXLOG::ERRORLOG("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

void HTTPDir::curlDownload(char * url ,HTTPMemoryStruct * chunk){
	
	CURL *curl_handle;
	CURLcode res;
	chunk->memory = (char *)malloc(1);  
	chunk->size = 0; 
	curl_global_init(CURL_GLOBAL_ALL);
	curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_URL, url);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)chunk);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	res = curl_easy_perform(curl_handle);
	if(res != CURLE_OK) {
		NXLOG::ERRORLOG("curl_easy_perform() failed: %s\n",
        curl_easy_strerror(res));
	}
	else {
		
		
	}
	curl_easy_cleanup(curl_handle);
	curl_global_cleanup();
}

HTTPDir::HTTPDir(std::string _url){
	url = _url;
	urlschema thisurl = Utility::parseUrl(url);
	basepath = thisurl.path; 
	
}

HTTPDir::~HTTPDir(){
	
}

void HTTPDir::DirList(std::string path,const std::vector<std::string> &extensions){
	currentlist.clear();
	urlschema thisurl = Utility::parseUrl(url);
	std::string geturl = thisurl.scheme + std::string("://") + thisurl.server + (thisurl.port.empty() ? std::string() : ':' + thisurl.port) + std::string("/") + path;
	if (thisurl.user != NULL && thisurl.pass != NULL){
		std::string geturl = thisurl.scheme + std::string("://") + thisurl.user + ":" + thisurl.pass + "@" + thisurl.server + (thisurl.port.empty() ? std::string() : ':' + thisurl.port) + std::string("/") + path;
	}
	else if (thisurl.user != NULL){
		std::string geturl = thisurl.scheme + std::string("://") + thisurl.user + "@" + thisurl.server + (thisurl.port.empty() ? std::string() : ':' + thisurl.port) + std::string("/") + path;
	}
	HTTPMemoryStruct *chunk = (HTTPMemoryStruct *)malloc(sizeof(HTTPMemoryStruct));
	curlDownload((char *)geturl.c_str(),chunk);
	std::string s = chunk->memory;
	std::smatch sm;
	currentpath = path;
	
	std::regex rgxlinks("<a href=\"(.*?)\".*?>(?!Parent Directory)(.*?)<\/a>");
	
	while (regex_search(s, sm, rgxlinks))
	{
		FS::FileEntry tmpentry;
		std::string relurlref = sm[1];
		std::string name = sm[2];
		tmpentry.name = sm[2];
		tmpentry.path = sm[1];
		if(tmpentry.name.back() == '/'){
			tmpentry.type = FS::FileEntryType::Directory;
		}else{
			tmpentry.type = FS::FileEntryType::File;
		}
		tmpentry.size = 0;
		tmpentry.dbread = -1;
		currentlist.push_back(tmpentry);
		s = sm.suffix();
	}
	free(chunk->memory);
	if(sortOrder == FS::FS_NAME_ASCENDINGORDER){
		std::sort(currentlist.begin(), currentlist.end(), FS::SortNameAsc);
	}
	if(sortOrder == FS::FS_NAME_DESCENDINGORDER){
		std::sort(currentlist.begin(), currentlist.end(), FS::SortNameDesc);
	}			
	currentlist.erase(
		std::remove_if(currentlist.begin(), currentlist.end(), [extensions](const FS::FileEntry &file) {
			for (auto &ext : extensions) {
				if (Utility::endsWith(file.name, ext, false)) {
					return false;
				}
			}
			return file.type == FS::FileEntryType::File;
	}), currentlist.end());
	
}

std::string HTTPDir::getUrl(){
	return url;
}

std::string HTTPDir::getCurrPath(){
	return currentpath;
}

std::string HTTPDir::getBasePath(){
	return basepath;
}

std::vector<FS::FileEntry> HTTPDir::getCurrList(){
	return currentlist;
}

void HTTPDir::clearChecked(){
	for(int i=0;i<currentlist.size();i++){
		currentlist[i].checked = false;
	}
}

bool *HTTPDir::checked(int pos){
	return &currentlist[pos].checked;
}

std::string HTTPDir::backDir(){
	
	if(currentpath.find_last_of("/") == 0)currentpath = basepath;
	if(currentpath.find_last_of("/") == -1)currentpath = basepath;
	if(currentpath == basepath)return "";
	currentpath = FS::removeLastSlash(currentpath);
	std::string retpath = currentpath.substr(currentpath.find_last_of("\\/")+1);
	currentpath = currentpath.substr(0, currentpath.find_last_of("/"));
	return retpath;
	
}

void HTTPDir::SetFileDbStatus(int idx,int dbstatus){
		currentlist[idx].dbread = dbstatus;
	}
	
	void HTTPDir::ResetDbStatus(){
		for(int i=0;i<currentlist.size();i++){
			currentlist[i].dbread = -1;
		}
	}