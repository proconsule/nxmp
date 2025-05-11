#ifndef NXMP_I18N_EN_H
#define NXMP_I18N_EN_H

/* Default Language */
const char *en_i18n_author = "proconsule";
bool en_i18n_status = true;
bool en_i18n_native = true;

std::vector<std::string> MainMenu_STR_EN = {
	"Local Files",
	"USB",
	"Stream Url",
	"Network",
	"UPNP",
	"Enigma2",
	"Playlist",
	"Settings",
	"Info",
	"Exit"
	
};

std::vector<std::string> Common_STR_EN = {
	"Yes",
	"No",
	"OK",
	"Auto",
	"Navigation",
	"Select",
	"Select/Play",
	"Home",
	"Home/Save",
	"Back",
	"Context Menu",
	"Name",
	"Size",
	"Date",
	"Move Up",
	"Move Down",
	"Delete",
	"Exit",
	"Cancel",
	"Save",
	"Checkbox",
	
};


std::vector<std::string> Popup_STR_EN = {
	"Save Settings",
	"Resume Play",
	"Database Update",
	"Database was updated",
	"Network Menu Popup",
	"File Context Menu Popup",
	"Sort Ascending (Name)",
	"Sort Ascending (Size)",
	"Sort Ascending (Date)",
	"Sort Descending (Name)",
	"Sort Descending (Size)",
	"Sort Descending (Date)",
	"Add File to Playlist",
	"Add Checked to Playlist",
	"Remove Element",
	"Clear Playlist",
	"Set as starting element"
	
};

std::vector<std::string> SettingsMenu_STR_EN = {
	"Generals",
	"File Browser",
	"Show Hidden Files",
	"Navigation",
	"Enable Touch Controls",
	"Overclock",
	"Enable Overclock on Startup",
	"Misc (need restart)",
	"VSYNC",
	"Interface Language",
	"Player Settings",
	"Decoder",
	"HW Decoder",
	"Demux Cache",
	"Demux Cache Sec",
	"Audio",
	"Audio Output",
	"Seek",
	"Short Seek",
	"Long Seek",
	"Language",
	"Use Auto Audio Language",
	"Audio Language",
	"Use Auto Subtitles Language",
	"Subtitles Language",
	"Subtitles",
	"Sub Font Size",
	"Sub Font Scale",
	"Video",
	"Video Deinterlace",
	"Database",
	"Use Database",
	"Database version :",
	"SQLite version :",
	"Resume point count: ",
	"Completed view: ",
	"Database not active",
	"Start saving resume info:",
	"Stop saving resume info:",
	
	"Touch Settings",
};
	
	
std::vector<std::string> Network_STR_EN = {
	"Add Share",
	"Add Share Menu",
	"Share Config",
	"Share Name:",
	"Protocol: ",
	"Credentials",
	"IP Address:",
	"Port:",
	"Username:",
	"Password:",
	"Anonymous",
	"Remote Path:",
	"Remote path on server example: /movie/",
	"Remote share on server (share not path)",
	"Test Connection",
	"Connection OK",
	"Connection Error",
	"Network Source Selection",
	"Network Source Menu",
};


#endif