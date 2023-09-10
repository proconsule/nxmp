#include "sambaDir.h"

sambaDir::sambaDir(std::string _url,Playlist * _playlist){
	url = _url;	
	urlschema thisurl = Utility::parseUrl(url);
	share = std::string("/") + thisurl.path;
	size_t startpos = share.find_first_of("/");
	size_t pos = share.find_last_of("/");
	if(pos == share.length()-1)
	{
		share.erase(share.length()-1);
	}
	if(startpos == 0){
		share.erase(0,1);
	}
	

	NXLOG::DEBUGLOG("Base Path = %s\n",share.c_str());
	currentpath = basepath = "";
	playlist = _playlist;
	
}


void sambaDir::DirList(std::string path,bool showHidden,const std::vector<std::string> &extensions){
	
	struct smb2_context *smb2;
	struct smb2dir *dir;
	struct smb2dirent *ent;

	urlschema thisurl = Utility::parseUrl(url);
	if(thisurl.user.empty())thisurl.user = "Guest";
	currentlist.clear();
	
	
	
	if(path != ""){
		size_t pos = path.find_last_of("/");
		if(pos == path.length()-1)
		{
			path.erase(path.length()-1);
		}
	}
	
	currentpath = path;
	
	smb2 = smb2_init_context();
	if (smb2 == NULL) {
		NXLOG::ERRORLOG("SMB2 Failed to init context\n");
		return;
	
	}
	
	
	struct smb2_url *myurl = smb2_parse_url(smb2, url.c_str());
	
	NXLOG::DEBUGLOG("user: %s\n",myurl->user);
	fflush(stdout);
	char const *search = ":";
	char * token = strtok((char *)myurl->user, search);
	smb2_set_user(smb2,thisurl.user.c_str()); 
	token = strtok(NULL, search);
	smb2_set_password(smb2,thisurl.pass.c_str()); 
	
	smb2_set_security_mode(smb2, SMB2_NEGOTIATE_SIGNING_ENABLED);
	
	NXLOG::DEBUGLOG("SMBURL: %s %s\n",share.c_str(),path.c_str());
	
	if (smb2_connect_share(smb2, myurl->server, myurl->share, NULL) < 0) {
		NXLOG::ERRORLOG("smb2_connect_share failed. %s\n", smb2_get_error(smb2));
		return;
	}
	NXLOG::DEBUGLOG("SMB2: Share Connected\n");
	fflush(stdout);
	dir = smb2_opendir(smb2, path.c_str());
	if (dir == NULL) {
		NXLOG::ERRORLOG("smb2_opendir failed. %s\n", smb2_get_error(smb2));
		return;
	}
	
	while ((ent = smb2_readdir(smb2, dir))) {
                //char const *type;
                //time_t t;
				int entrytype = -1;

                //t = (time_t)ent->st.smb2_mtime;
                switch (ent->st.smb2_type) {
                case SMB2_TYPE_FILE:
                        //type = "FILE";
						entrytype = 2;
                        break;
                case SMB2_TYPE_DIRECTORY:
                        //type = "DIRECTORY";
						entrytype = 1;
                        break;
                default:
                        //type = "unknown";
                        break;
                }
				if(!showHidden){
					if (ent->name[0] == '.') {
						continue;
					}
				}
				if ((strlen(ent->name) == 1) && ent->name[0] == '.') {
					continue;
				}
				if ((strlen(ent->name) == 2) && ent->name[0] == '.' && ent->name[1] == '.') {
					continue;
				}
				
				if(entrytype == -1)continue;
				NXLOG::DEBUGLOG("%s %lu\n",ent->name,ent->st.smb2_size);
                FS::FileEntry tmpentry;
				tmpentry.name = ent->name;
				if(entrytype == 1)tmpentry.type = FS::FileEntryType::Directory;
				if(entrytype == 2)tmpentry.type = FS::FileEntryType::File;
				if(path != ""){
					tmpentry.path = path + std::string("/") + ent->name;
				}else{
					tmpentry.path = ent->name;
				}
				
				tmpentry.size = ent->st.smb2_size;
				tmpentry.is_valid = 1;
				tmpentry.modified = (time_t)ent->st.smb2_mtime;
				tmpentry.accessed = (time_t)ent->st.smb2_atime;
				tmpentry.created = (time_t)ent->st.smb2_ctime;
				
				currentlist.push_back(tmpentry);
        }
	
	//std::sort(currentlist.begin(), currentlist.end(), FS::Sort);
	if(sortOrder == FS::FS_NAME_ASCENDINGORDER){
		std::sort(currentlist.begin(), currentlist.end(), FS::SortNameAsc);
	}
	if(sortOrder == FS::FS_NAME_DESCENDINGORDER){
		std::sort(currentlist.begin(), currentlist.end(), FS::SortNameDesc);
	}
	if(sortOrder == FS::FS_DATE_ASCENDINGORDER){
		std::sort(currentlist.begin(), currentlist.end(), FS::SortDateAsc);
	}
	if(sortOrder == FS::FS_DATE_DESCENDINGORDER){
		std::sort(currentlist.begin(), currentlist.end(), FS::SortDateDesc);
	}
	if(sortOrder == FS::FS_SIZE_ASCENDINGORDER){
		std::sort(currentlist.begin(), currentlist.end(), FS::SortSizeAsc);
	}
	if(sortOrder == FS::FS_SIZE_DESCENDINGORDER){
		std::sort(currentlist.begin(), currentlist.end(), FS::SortSizeDesc);
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
	
 
    

    smb2_closedir(smb2, dir);
    smb2_disconnect_share(smb2);
    smb2_destroy_context(smb2);

}


std::vector<FS::FileEntry> sambaDir::getCurrList(){
	return currentlist;
}

bool *sambaDir::checked(int pos){
	return &currentlist[pos].checked;
}

std::string sambaDir::getUrl(){
	return url;
}

std::string sambaDir::getCurrPath(){
	return currentpath;
}

std::string sambaDir::getBasePath(){
	return basepath;
}

void sambaDir::clearChecked(){
	for(int i=0;i<currentlist.size();i++){
		currentlist[i].checked = false;
	}
}

std::string sambaDir::getShare(){
	return share;
}

void sambaDir::backDir(){
	if(currentpath.find_last_of("/") == 0)currentpath = basepath;
	if(currentpath.find_last_of("/") == -1)currentpath = basepath;
	if(currentpath == basepath)return;
	currentpath = FS::removeLastSlash(currentpath);
	currentpath = currentpath.substr(0, currentpath.find_last_of("/"));
}