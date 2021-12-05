#ifndef NXMP_PLAYLIST_H
#define NXMP_PLAYLIST_H

#include <vector>
#include <string>
#include <algorithm>

class Playlist{
public:

	struct playlist_struct{
		std::string name;
		std::string fulluri;
	};

	Playlist();
	
	std::vector<playlist_struct> getPlaylist();
	void clearPlaylist();
	void appendFile(std::string name,std::string fulluri);
	void removeFile(std::string name,std::string fulluri);
	void insertFile(int pos,playlist_struct entry);
	
	playlist_struct getPlaylistItem(int pos);
	void setPlaylistIdx(int pos);
	
	void moveBack(int eleidx);
	void moveForw(int eleidx);
	
	playlist_struct getNext();
	playlist_struct getPrev();
	int getCurrIdx();
	bool isPresent(std::string name,std::string fulluri);

private:
	std::vector<playlist_struct> currplaylist;
	int currentidx = 0;

};

#endif