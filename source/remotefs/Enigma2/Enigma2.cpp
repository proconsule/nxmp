#include "Enigma2.h"
#ifdef NXMP_ENIGMASUPPORT
#include "utils.h"



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
	urlschema e2schema = Utility::parseUrl(enigmaurl);
	string m3uurl =  e2schema.scheme;
	m3uurl.append("://");
	m3uurl.append(e2schema.server);
	if(e2schema.port == "")e2schema.port="80";
	m3uurl.append(":");
	m3uurl.append(e2schema.port);
	m3uurl.append("/web/services.m3u?bRef=");
	m3uurl.append(urlencode(url));
	MemoryStruct *chunk = (MemoryStruct *)malloc(sizeof(MemoryStruct));
	//curlDownload((char *)m3uurl.c_str(),chunk);
	
	curlDownloader * curldownloader = new curlDownloader();
	curldownloader->Download((char *)m3uurl.c_str(),chunk);
	
	
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
	
	string epguurl =  e2schema.scheme;
	epguurl.append("://");
	epguurl.append(e2schema.server);
	if(e2schema.port == "")e2schema.port="80";
	epguurl.append(":");
	epguurl.append(e2schema.port);
	epguurl.append("/web/epgnow?bRef=");
	epguurl.append(urlencode(url));
	MemoryStruct *chunk2 = (MemoryStruct *)malloc(sizeof(MemoryStruct));
	curldownloader->Download((char *)epguurl.c_str(),chunk2);
	 
	XMLDocument doc;
	doc.Parse( chunk2->memory );
	XMLElement * pRootElement = doc.RootElement();
	if (NULL != pRootElement) {
		XMLElement * pe2service = pRootElement->FirstChildElement("e2event");
		while(pe2service){
			XMLElement * pe2serviceref = pe2service->FirstChildElement("e2eventservicereference");
			XMLElement * pe2eventTitle = pe2service->FirstChildElement("e2eventtitle");
			XMLElement * pe2eventStart = pe2service->FirstChildElement("e2eventstart");
			XMLElement * pe2eventDuration = pe2service->FirstChildElement("e2eventduration");
			XMLElement * pe2eventCurrTime = pe2service->FirstChildElement("e2eventcurrenttime");
			if(string(pe2eventTitle->GetText()) != "None"){
				for(unsigned int i=0;i<e2currbouqet.size();i++){
					if(string(pe2serviceref->GetText()) == e2currbouqet[i].srvref){
						e2currbouqet[i].epg.title = pe2eventTitle->GetText();
						pe2eventStart->QueryIntText(&e2currbouqet[i].epg.startTime);
						pe2eventDuration->QueryIntText(&e2currbouqet[i].epg.duration);
						pe2eventCurrTime->QueryIntText(&e2currbouqet[i].epg.currTime);
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
	//string downurl = "http://";
	//downurl.append(enigma2ip);
	urlschema e2schema = Utility::parseUrl(enigmaurl);
	string downurl =  e2schema.scheme;
	downurl.append("://");
	downurl.append(e2schema.server);
	if(e2schema.port == "")e2schema.port="80";
	downurl.append(":");
	downurl.append(e2schema.port);
	downurl.append("/web/getservices");
	curlDownloader * curldownloader = new curlDownloader();
	curldownloader->Download((char *)downurl.c_str(),chunk);
	e2services =  parseBouquet(chunk->memory);
	free(chunk->memory);
	free(chunk);
	return true;
}

EnigmaServices Enigma2::getCurrBouquet(){
	return currbouquet;
}

void Enigma2::setCurrBouquet(EnigmaServices _bouquet){
	currbouquet = _bouquet;
}

void Enigma2::backToTop(){
	EnigmaServices dummy;
	currbouquet = dummy;
}

Enigma2::Enigma2(std::string _url){
	enigmaurl = _url;
}

#endif