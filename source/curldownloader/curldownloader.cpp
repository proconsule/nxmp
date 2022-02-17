
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

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
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

std::string curlDownloader::scrapeHtml(std::string myurl, std::string postcode, bool needpost, std::string referer, bool needreferer)
{
	 CURL *curl;
  CURLcode res;
  std::string readBuffer;

  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, myurl.c_str());
	if (needpost == true)
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postcode.c_str());
  if (needreferer == true)
  curl_easy_setopt(curl, CURLOPT_REFERER, referer.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/98.0.4758.82 Safari/537.36");
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

  }
	return readBuffer;
}

std::string curlDownloader::getRedirection(std::string myurl, std::string postcode, bool needpost, std::string referer, bool needreferer)
{
CURL *curl;
  CURLcode res;
  char *location;
  long response_code;
 std::string tempresult = myurl;
  curl = curl_easy_init();
  if(curl) {
  curl_easy_setopt(curl, CURLOPT_URL, myurl.c_str());
  if (needpost == true)
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postcode.c_str());
  if (needreferer == true)
  curl_easy_setopt(curl, CURLOPT_REFERER, referer.c_str());
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/98.0.4758.82 Safari/537.36");
  curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
 
    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    else {
      res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
      if((res == CURLE_OK) &&
         ((response_code / 100) != 3)) {
        /* a redirect implies a 3xx response code */
        fprintf(stderr, "Not a redirect.\n");
      }
      else {
        res = curl_easy_getinfo(curl, CURLINFO_REDIRECT_URL, &location);
 
        if((res == CURLE_OK) && location) {
          /* This is the new absolute URL that you could redirect to, even if
           * the Location: response header may have been a relative URL. */
          printf("Redirected to: %s\n", location);
		  tempresult = std::string(location);
        }
      }
    }
 
  /* always cleanup */
  curl_easy_cleanup(curl);
  }
  return tempresult;
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
