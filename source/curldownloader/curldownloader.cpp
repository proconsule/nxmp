
#include "curldownloader.h"

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
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

curlDownloader::curlDownloader(){
	
}

void curlDownloader::Download(char * url ,MemoryStruct * chunk){
	
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

SOAPcurlDownloader::SOAPcurlDownloader(){
	
	
}

void SOAPcurlDownloader::Download(char * url ,MemoryStruct * chunk, char * OID){
	struct curl_slist *headerchunk = NULL;
	CURL *curl_handle;
	CURLcode res;
	chunk->memory = (char *)malloc(1);  
	chunk->size = 0; 
	curl_global_init(CURL_GLOBAL_ALL);
	curl_handle = curl_easy_init();
	char soapdata[4096];
	
	char * soapdatafmt = "<?xml version=\"1.0\"?> \
<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"> \
  <s:Body> \
    <u:Browse xmlns:u=\"urn:schemas-upnp-org:service:ContentDirectory:1\"> \
      <ObjectID>%s</ObjectID> \
      <BrowseFlag>BrowseDirectChildren</BrowseFlag> \
      <Filter>*</Filter> \
      <StartingIndex>0</StartingIndex> \
      <RequestedCount>10000</RequestedCount> \
      <SortCriteria></SortCriteria> \
    </u:Browse> \
  </s:Body> \
</s:Envelope> ";
	sprintf(soapdata,soapdatafmt,OID);
	
	
	headerchunk = curl_slist_append(headerchunk, "Content-Type: text/xml; charset=\"utf-8\"");
	headerchunk = curl_slist_append(headerchunk, "SOAPACTION: \"urn:schemas-upnp-org:service:ContentDirectory:1#Browse\"");
	curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headerchunk);
	curl_easy_setopt(curl_handle, CURLOPT_URL, url);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)chunk);	
	curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, soapdata);
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, strlen(soapdata));
	
	res = curl_easy_perform(curl_handle);
	
	curl_easy_cleanup(curl_handle);
	curl_global_cleanup();
	
}
