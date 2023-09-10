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
		int64_t duration = 0;
		bool is_valid = false;
	};

	Playlist();
	
	std::vector<playlist_struct> getPlaylist();
	void clearPlaylist();
	void appendFile(FS::FileEntry myfile,std::string fulluri);
	void removeFile(FS::FileEntry myfile,std::string fulluri);
	void insertFile(int pos,playlist_struct entry);
	
	playlist_struct getPlaylistItem(int pos);
	void setPlaylistIdx(int pos);
	
	void Invalidate();
	
	void RemoveElement(int pos);
	void moveBack(int eleidx);
	void moveForw(int eleidx);
	
	playlist_struct getNext();
	playlist_struct getPrev();
	int getCurrIdx();
	bool isPresent(FS::FileEntry myfile,std::string fulluri);
	
	bool HaveUSBEntrys();
	void CalcPlaylist();
	
	std::vector<playlist_struct> currplaylist;
	
	int remtime;
	int totaltime;

private:
	
	int currentidx = 0;

};

#endif