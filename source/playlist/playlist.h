#ifndef NXMP_PLAYLIST_H
#define NXMP_PLAYLIST_H

#include <vector>
#include <string>
#include <algorithm>
#include "localfiles.h"


class Playlist{
public:

	struct playlist_struct{
		FS::FileEntry file;
		std::string fulluri;
	};

	Playlist();
	
	std::vector<playlist_struct> getPlaylist();
	void clearPlaylist();
	void appendFile(FS::FileEntry myfile,std::string fulluri);
	void removeFile(FS::FileEntry myfile,std::string fulluri);
	void insertFile(int pos,playlist_struct entry);
	
	playlist_struct getPlaylistItem(int pos);
	void setPlaylistIdx(int pos);
	
	void RemoveElement(int pos);
	void moveBack(int eleidx);
	void moveForw(int eleidx);
	
	playlist_struct getNext();
	playlist_struct getPrev();
	int getCurrIdx();
	bool isPresent(FS::FileEntry myfile,std::string fulluri);

private:
	std::vector<playlist_struct> currplaylist;
	int currentidx = 0;

};

#endif