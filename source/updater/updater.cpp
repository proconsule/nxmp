#include "updater.h"

#include "curldownloader.h"

#include <string.h>
#include <jansson.h>

Updater::Updater(){
	
}

void Updater::fetchReleases(){
	
	curlDownloader * curldownloader = new curlDownloader();
	MemoryStruct *chunk = (MemoryStruct *)malloc(sizeof(MemoryStruct));
	curldownloader->Download("https://api.github.com/repos/proconsule/nxmp/releases",chunk);
	printf(chunk->memory);
	
	
	json_t *root;
    json_error_t error;
	
	root = json_loads(chunk->memory, 0, &error);
	free(chunk->memory);
	free(chunk);
	
	if(!root)
	{
		printf("error: on line %d: %s\n", error.line, error.text);
		json_decref(root);
		return;
	}
	
	for(int i = 0; i < json_array_size(root); i++)
	{
		json_t *data;
		data = json_array_get(root, i);
		
		json_t *tagobj;
		json_t *prereleaseobj;
		json_t *assetsobj;
		tagobj = json_object_get(data, "tag_name");
		prereleaseobj = json_object_get(data, "prerelease");
		//prereleaseobj = json_boolean_value(data, "prerelease");
		assetsobj = json_object_get(data,"assets");
		printf("TAG: %s\n",json_string_value(tagobj));
		
		for(int n = 0; n < json_array_size(assetsobj); n++)
		{
			json_t *dataasset;
			dataasset = json_array_get(assetsobj, n);
			json_t *nameobj = json_object_get(dataasset,"name");
			json_t *downurlobj = json_object_get(dataasset,"browser_download_url");
			
			printf("\tName: %s\n",json_string_value(nameobj));
                        if(strcmp(json_string_value(nameobj),"nxmp.nro") == 0){
				printf("\tURL: %s\n",json_string_value(downurlobj));
			}
			gitrelease_struct tmpgitrelease;
			tmpgitrelease.tagname = json_string_value(tagobj);
			tmpgitrelease.prerelease = json_boolean_value(prereleaseobj);
			tmpgitrelease.downurl = json_string_value(downurlobj);
			gitreleases.push_back(tmpgitrelease);
			
				
		}
				
	}
	
	json_decref(root);

}

std::vector<gitrelease_struct> Updater::getReleases(){
	return gitreleases;
}


