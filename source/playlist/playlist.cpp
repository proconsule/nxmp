#include "playlist.h"


void playlistmove(std::vector<Playlist::playlist_struct>& v, size_t oldIndex, size_t newIndex)
{
    if (oldIndex > newIndex)
        std::rotate(v.rend() - oldIndex - 1, v.rend() - oldIndex, v.rend() - newIndex);
    else        
        std::rotate(v.begin() + oldIndex, v.begin() + oldIndex + 1, v.begin() + newIndex + 1);
}

Playlist::Playlist(){
	
}

void Playlist::Invalidate(){
	for(int i=0;i<currplaylist.size();i++){
		currplaylist[i].is_valid = false;
	}
}

std::vector<Playlist::playlist_struct> Playlist::getPlaylist(){
	return currplaylist;
}

void Playlist::clearPlaylist(){
	currplaylist.clear();
	currentidx = 0;
}

void Playlist::appendFile(FS::FileEntry myfile,std::string fulluri){
	Playlist::playlist_struct addentry;
	addentry.file = myfile;
	addentry.fulluri = fulluri;
	currplaylist.push_back(addentry);
}

void Playlist::removeFile(FS::FileEntry myfile,std::string fulluri){
	for(int i=0;i<currplaylist.size();i++){
		if(currplaylist[i].file.name == myfile.name && currplaylist[i].fulluri == fulluri){
			currplaylist.erase (currplaylist.begin() + i);
		}
	}
	
}

Playlist::playlist_struct Playlist::getNext(){
	if(currentidx+1 <currplaylist.size()){
		currentidx = currentidx+1;
	}
	return currplaylist[currentidx];
}
Playlist::playlist_struct Playlist::getPrev(){
	if(currentidx-1 >=0){
		currentidx = currentidx-1;
	}
	return currplaylist[currentidx];
}

Playlist::playlist_struct Playlist::getPlaylistItem(int pos){
	return currplaylist[pos];
}

void Playlist::RemoveElement(int pos){
	currplaylist.erase(currplaylist.begin()+pos);
	CalcPlaylist();
}

void Playlist::setPlaylistIdx(int pos){
	if(pos >=0 && pos < currplaylist.size()){
		currentidx = pos;
	}
	CalcPlaylist();
}

int Playlist::getCurrIdx(){
	return currentidx;
}

void Playlist::moveBack(int eleidx){
	if(eleidx > 0){
		playlistmove(currplaylist,eleidx,eleidx-1);
	}
}

void Playlist::moveForw(int eleidx){
	if(eleidx+1 < currplaylist.size()){
		playlistmove(currplaylist,eleidx,eleidx+1);
	}
}


bool Playlist::HaveUSBEntrys(){
	for(int i=0;i<currplaylist.size();i++){
		if(Utility::startWith(currplaylist[i].fulluri,"ums",false)){
			return true;
		}
	}
}



bool Playlist::isPresent(FS::FileEntry myfile,std::string fulluri){
	for(int i=0;i<currplaylist.size();i++){
		if(currplaylist[i].file.name == myfile.name && currplaylist[i].fulluri == fulluri){
			return true;
		}
	}
	return false;
}

void Playlist::CalcPlaylist(){
	remtime = 0;
	totaltime = 0;
	for(int i=0;i<currplaylist.size();i++){
		if(i>=currentidx){
			remtime+=currplaylist[i].duration;
		}
		totaltime += currplaylist[i].duration;
	}
}