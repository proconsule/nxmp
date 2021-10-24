#include "Enigma2.h"


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

string urlencode(string s)
{
    const char *str = s.c_str();
    vector<char> v(s.size());
    v.clear();
    for (size_t i = 0, l = s.size(); i < l; i++)
    {
        char c = str[i];
        
        if (c == ' ')
        {
            v.push_back('%');
			v.push_back('2');
			v.push_back('0');
        }
        else
        {
			v.push_back(c);
        }
    }

    return string(v.cbegin(), v.cend());
}


void Enigma2::curlDownload(char * url ,MemoryStruct * chunk){
	
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


vector<EnigmaServices> Enigma2::parseBouquet(char * data){
	
	vector<EnigmaServices> tmpret;
	XMLDocument doc;
	doc.Parse( data );
	XMLElement * pRootElement = doc.RootElement();
	if (NULL != pRootElement) {
			XMLElement * pe2service = pRootElement->FirstChildElement("e2service");
			while(pe2service){
				EnigmaServices tmpele;
				
				XMLElement * pe2serviceName = pe2service->FirstChildElement("e2servicename");
				XMLElement * pe2serviceRef = pe2service->FirstChildElement("e2servicereference");
				tmpele.name = pe2serviceName->GetText();
				tmpele.bouquetref = pe2serviceRef->GetText();
				tmpret.push_back(tmpele);
				pe2service = pe2service->NextSiblingElement("e2service");
			}
	}else{
		
	}
	return tmpret;
}

void Enigma2::m3uParser(char * url){
	e2currbouqet.clear();
	string m3uurl = "http://";
	m3uurl.append(enigma2ip);
	m3uurl.append("/web/services.m3u?bRef=");
	m3uurl.append(urlencode(url));
	MemoryStruct *chunk = (MemoryStruct *)malloc(sizeof(MemoryStruct));
	curlDownload((char *)m3uurl.c_str(),chunk);
	
	
	string s = chunk->memory;
	std::regex rgx(".*#EXTINF:[[:print:]]*,([[:print:]]*)\n?#?.*\n(http.+)\n");
    std::smatch matches;
    std::smatch sm;
	while (regex_search(s, sm, rgx))
	{
		ChannelStruct tmpchannel;
		tmpchannel.name = sm[1];
		tmpchannel.url = sm[2]; 
		std::size_t found = tmpchannel.url.find_last_of("/");
		tmpchannel.srvref = tmpchannel.url.substr(found+1);
		e2currbouqet.push_back(tmpchannel);
		s = sm.suffix();
	}
	
	string epguurl = "http://";
	epguurl.append(enigma2ip);
	epguurl.append("/web/epgnow?bRef=");
	epguurl.append(urlencode(url));
	MemoryStruct *chunk2 = (MemoryStruct *)malloc(sizeof(MemoryStruct));
	curlDownload((char *)epguurl.c_str(),chunk2);
	
	XMLDocument doc;
	doc.Parse( chunk2->memory );
	XMLElement * pRootElement = doc.RootElement();
	if (NULL != pRootElement) {
		XMLElement * pe2service = pRootElement->FirstChildElement("e2event");
		while(pe2service){
			XMLElement * pe2serviceref = pe2service->FirstChildElement("e2eventservicereference");
			//XMLElement * pe2serviceName = pe2service->FirstChildElement("e2eventservicename");
			XMLElement * pe2eventTitle = pe2service->FirstChildElement("e2eventtitle");
			if(string(pe2eventTitle->GetText()) != "None"){
				for(unsigned int i=0;i<e2currbouqet.size();i++){
					if(string(pe2serviceref->GetText()) == e2currbouqet[i].srvref){
						e2currbouqet[i].epgtitle = pe2eventTitle->GetText();
						break;
					}
				}
			}
			pe2service = pe2service->NextSiblingElement("e2event");
		}
	}
	free(chunk->memory);
	free(chunk);
	free(chunk2->memory);
	free(chunk2);
}

bool Enigma2::getServices(){
	MemoryStruct *chunk = (MemoryStruct *)malloc(sizeof(MemoryStruct));
	string downurl = "http://";
	downurl.append(enigma2ip);
	downurl.append("/web/getservices");
	curlDownload((char *)downurl.c_str(),chunk);
	e2services =  parseBouquet(chunk->memory);
	free(chunk->memory);
	free(chunk);
	return true;
}
 