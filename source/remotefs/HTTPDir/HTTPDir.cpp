#include "HTTPDir.h"

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct HTTPMemoryStruct *mem = (struct HTTPMemoryStruct *)userp;
 
  char *ptr = (char *)realloc(mem->memory, mem->size + realsize + 1);
  if(!ptr) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
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
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
        curl_easy_strerror(res));
	}
	else {
		
		
	}
	curl_easy_cleanup(curl_handle);
	curl_global_cleanup();
}

HTTPDir::HTTPDir(){

}

std::vector<remotefs_entry> HTTPDir::dirList(std::string path){
	std::vector<remotefs_entry> tmpret;
	std::string geturl = url + currentrelpath + path;
	HTTPMemoryStruct *chunk = (HTTPMemoryStruct *)malloc(sizeof(HTTPMemoryStruct));
	curlDownload((char *)geturl.c_str(),chunk);
	std::string s = chunk->memory;
	std::smatch sm;
	currentrelpath = currentrelpath+path;
	std::regex rgxdirlist("<h1>Index of");
	if(!regex_search(s, sm, rgxdirlist)){
		return tmpret;
	}
	
	std::regex rgxlinks("<tr>.*?<td><a href=\"(.*?)\".*?>(?!Parent Directory)(.*?)<\/a>.*?<\/td><\/tr>");
	
	while (regex_search(s, sm, rgxlinks))
	{
		remotefs_entry tmpentry;
		std::string relurlref = sm[1];
		std::string name = sm[2];
		tmpentry.name = sm[2];
		tmpentry.path = sm[1];
		if(tmpentry.name.back() == '/')tmpentry.isDir = true;
		tmpentry.size = 0;
		tmpret.push_back(tmpentry);
		s = sm.suffix();
	}
	free(chunk->memory);
	return tmpret;
	
}

void HTTPDir::backDir(){
	currentrelpath = currentrelpath.substr(0, currentrelpath.find_last_of("\\/"));
	currentrelpath = currentrelpath.substr(0, currentrelpath.find_last_of("\\/")+1);
}

void HTTPDir::setCurrentRelPath(std::string _path){
	currentrelpath = _path;
}
std::string HTTPDir::getCurrentRelPath(){
	return currentrelpath;
}
void HTTPDir::setUrl(std::string _path){
	url = _path;
}
std::string HTTPDir::getUrl(){
	return url;
}