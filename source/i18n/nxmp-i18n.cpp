#include "nxmp-i18n.h"

#include "nxmp-en.h"
#include "nxmp-it.h"
#include "nxmp-es.h"
#include "nxmp-de.h"
#include "nxmp-fr.h"
#include "nxmp-pt.h"
#include "nxmp-jp.h"
#include "nxmp-cn_s.h"
#include "nxmp-cn_t.h"
#include "nxmp-ru.h"


char *MainMenu_STR[NX_MAINMENU_STR_NR_ITEMS];
char *Common_STR[NX_COMMON_STR_NR_ITEMS];
char *Popup_STR[NX_COMMON_STR_NR_ITEMS];
char *SettingsMenu_STR[NX_SETTINGS_STR_NR_ITEMS];
char *Network_STR[NX_NETWORK_STR_NR_ITEMS];

const char *NXLANGNAME[] = {
	"English",
	"Italiano",
	"Español",
	"Deutsch",
	"Français",
	"Português",
	"日本語",
	"中国人 （简体）",
	"中國人 （傳統的）",
	"Русский",
	
};

char *NXLANGAUTHORS[NX_LANGS_NR_ITEMS];
bool NXLANGACTIVE[NX_LANGS_NR_ITEMS];
bool NXLANGNATIVE[NX_LANGS_NR_ITEMS];

void InitLang(NX_LANGS mylang){
	
	
	/* Clear Previous allocs if exists */
	DestroyLang();
	
	/* DUP Strings */
	NXLANGAUTHORS[English] = strdup(en_i18n_author);
	NXLANGAUTHORS[Italian] = strdup(it_i18n_author);
	NXLANGAUTHORS[Spanish] = strdup(es_i18n_author);
	NXLANGAUTHORS[Deutsch] = strdup(de_i18n_author);
	NXLANGAUTHORS[French] = strdup(fr_i18n_author);
	NXLANGAUTHORS[Portuguese] = strdup(pt_i18n_author);
	NXLANGAUTHORS[Japanese] = strdup(jp_i18n_author);
	NXLANGAUTHORS[Chinese_Simplified] = strdup(cn_s_i18n_author);
	NXLANGAUTHORS[Chinese_Traditional] = strdup(cn_t_i18n_author);
	NXLANGAUTHORS[Russian] = strdup(ru_i18n_author);
	
	
	NXLANGACTIVE[English] = en_i18n_status;
	NXLANGACTIVE[Italian] = it_i18n_status;
	NXLANGACTIVE[Spanish] = es_i18n_status;
	NXLANGACTIVE[Deutsch] = de_i18n_status;
	NXLANGACTIVE[French] = fr_i18n_status;
	NXLANGACTIVE[Portuguese] = pt_i18n_status;
	NXLANGACTIVE[Japanese] = jp_i18n_status;
	NXLANGACTIVE[Chinese_Simplified] = cn_s_i18n_status;
	NXLANGACTIVE[Chinese_Traditional] = cn_t_i18n_status;
	NXLANGACTIVE[Russian] = ru_i18n_status;
	
	NXLANGNATIVE[English] = en_i18n_native;
	NXLANGNATIVE[Italian] = it_i18n_native;
	NXLANGNATIVE[Spanish] = es_i18n_native;
	NXLANGNATIVE[Deutsch] = de_i18n_native;
	NXLANGNATIVE[French] = fr_i18n_native;
	NXLANGNATIVE[Portuguese] = pt_i18n_native;
	NXLANGNATIVE[Japanese] = jp_i18n_native;
	NXLANGNATIVE[Chinese_Simplified] = cn_s_i18n_native;
	NXLANGNATIVE[Chinese_Traditional] = cn_t_i18n_native;
	NXLANGNATIVE[Russian] = ru_i18n_native;
	
	for(int i=0;i<NX_MAINMENU_STR_NR_ITEMS;i++){
		if(mylang == English){
			MainMenu_STR[i] = strdup(MainMenu_STR_EN[i]);
		}else if(mylang == Italian){
			MainMenu_STR[i] = strdup(MainMenu_STR_IT[i]);
		}else if(mylang == Spanish){
			MainMenu_STR[i] = strdup(MainMenu_STR_ES[i]);
		}else if(mylang == Deutsch){
			MainMenu_STR[i] = strdup(MainMenu_STR_DE[i]);
		}else if(mylang == French){
			MainMenu_STR[i] = strdup(MainMenu_STR_FR[i]);
		}else if(mylang == Portuguese){
			MainMenu_STR[i] = strdup(MainMenu_STR_PT[i]);
		}else if(mylang == Japanese){
			MainMenu_STR[i] = strdup(MainMenu_STR_JP[i]);
		}else if(mylang == Chinese_Simplified){
			MainMenu_STR[i] = strdup(MainMenu_STR_CN_S[i]);
		}else if(mylang == Chinese_Traditional){
			MainMenu_STR[i] = strdup(MainMenu_STR_CN_T[i]);
		}else if(mylang == Russian){
			MainMenu_STR[i] = strdup(MainMenu_STR_RU[i]);
		}else{
			MainMenu_STR[i] = strdup(MainMenu_STR_EN[i]);
		}
		
	}
	
	for(int i=0;i<NX_COMMON_STR_NR_ITEMS;i++){
		if(mylang == English){
			Common_STR[i] = strdup(Common_STR_EN[i]);
		}else if(mylang == Italian){
			Common_STR[i] = strdup(Common_STR_IT[i]);
		}else if(mylang == Spanish){
			Common_STR[i] = strdup(Common_STR_ES[i]);
		}else if(mylang == Deutsch){
			Common_STR[i] = strdup(Common_STR_DE[i]);
		}else if(mylang == French){
			Common_STR[i] = strdup(Common_STR_FR[i]);
		}else if(mylang == Portuguese){
			Common_STR[i] = strdup(Common_STR_PT[i]);
		}else if(mylang == Japanese){
			Common_STR[i] = strdup(Common_STR_JP[i]);
		}else if(mylang == Chinese_Simplified){
			Common_STR[i] = strdup(Common_STR_CN_S[i]);
		}else if(mylang == Chinese_Traditional){
			Common_STR[i] = strdup(Common_STR_CN_T[i]);
		}else if(mylang == Russian){
			Common_STR[i] = strdup(Common_STR_RU[i]);
		}else{
			Common_STR[i] = strdup(Common_STR_EN[i]);
		}
		
	}
	
	for(int i=0;i<NX_POPUP_STR_NR_ITEMS;i++){
		if(mylang == English){
			Popup_STR[i] = strdup(Popup_STR_EN[i]);
		}else if(mylang == Italian){
			Popup_STR[i] = strdup(Popup_STR_IT[i]);
		}else if(mylang == Spanish){
			Popup_STR[i] = strdup(Popup_STR_ES[i]);
		}else if(mylang == Deutsch){
			Popup_STR[i] = strdup(Popup_STR_DE[i]);
		}else if(mylang == French){
			Popup_STR[i] = strdup(Popup_STR_FR[i]);
		}else if(mylang == Portuguese){
			Popup_STR[i] = strdup(Popup_STR_PT[i]);
		}else if(mylang == Japanese){
			Popup_STR[i] = strdup(Popup_STR_JP[i]);
		}else if(mylang == Chinese_Simplified){
			Popup_STR[i] = strdup(Popup_STR_CN_S[i]);
		}else if(mylang == Chinese_Traditional){
			Popup_STR[i] = strdup(Popup_STR_CN_T[i]);
		}else if(mylang == Russian){
			Popup_STR[i] = strdup(Popup_STR_RU[i]);
		}else{
			Popup_STR[i] = strdup(Popup_STR_EN[i]);
		}
		
	}
	
	for(int i=0;i<NX_SETTINGS_STR_NR_ITEMS;i++){
		if(mylang == English){
			SettingsMenu_STR[i] = strdup(SettingsMenu_STR_EN[i]);
		}else if(mylang == Italian){
			SettingsMenu_STR[i] = strdup(SettingsMenu_STR_IT[i]);
		}else if(mylang == Spanish){
			SettingsMenu_STR[i] = strdup(SettingsMenu_STR_ES[i]);
		}else if(mylang == Deutsch){
			SettingsMenu_STR[i] = strdup(SettingsMenu_STR_DE[i]);
		}else if(mylang == French){
			SettingsMenu_STR[i] = strdup(SettingsMenu_STR_FR[i]);
		}else if(mylang == Portuguese){
			SettingsMenu_STR[i] = strdup(SettingsMenu_STR_PT[i]);
		}else if(mylang == Japanese){
			SettingsMenu_STR[i] = strdup(SettingsMenu_STR_JP[i]);
		}else if(mylang == Chinese_Simplified){
			SettingsMenu_STR[i] = strdup(SettingsMenu_STR_CN_S[i]);
		}else if(mylang == Chinese_Traditional){
			SettingsMenu_STR[i] = strdup(SettingsMenu_STR_CN_T[i]);
		}else if(mylang == Russian){
			SettingsMenu_STR[i] = strdup(SettingsMenu_STR_RU[i]);
		}else{
			SettingsMenu_STR[i] = strdup(SettingsMenu_STR_EN[i]);
		}
		
	}
	
	for(int i=0;i<NX_NETWORK_STR_NR_ITEMS;i++){
		if(mylang == English){
			Network_STR[i] = strdup(Network_STR_EN[i]);
		}else if(mylang == Italian){
			Network_STR[i] = strdup(Network_STR_IT[i]);
		}else if(mylang == Spanish){
			Network_STR[i] = strdup(Network_STR_ES[i]);
		}else if(mylang == Deutsch){
			Network_STR[i] = strdup(Network_STR_DE[i]);
		}else if(mylang == French){
			Network_STR[i] = strdup(Network_STR_FR[i]);
		}else if(mylang == Portuguese){
			Network_STR[i] = strdup(Network_STR_PT[i]);
		}else if(mylang == Japanese){
			Network_STR[i] = strdup(Network_STR_JP[i]);
		}else if(mylang == Chinese_Simplified){
			Network_STR[i] = strdup(Network_STR_CN_S[i]);
		}else if(mylang == Chinese_Traditional){
			Network_STR[i] = strdup(Network_STR_CN_T[i]);
		}else if(mylang == Russian){
			Network_STR[i] = strdup(Network_STR_RU[i]);
		}else{
			Network_STR[i] = strdup(Network_STR_EN[i]);
		}
	}
	
	
}


void DestroyLang(){
	
	for(int i=0;i<NX_LANGS_NR_ITEMS;i++){
		if(NXLANGAUTHORS[i] != NULL)free(NXLANGAUTHORS[i]);
		NXLANGACTIVE[i] = false;
		NXLANGNATIVE[i] = false;
	}
	
	for(int i=0;i<NX_MAINMENU_STR_NR_ITEMS;i++){
		if(MainMenu_STR[i] != NULL)free(MainMenu_STR[i]);
	}
	for(int i=0;i<NX_COMMON_STR_NR_ITEMS;i++){
		if(Common_STR[i] != NULL)free(Common_STR[i]);
	}
	for(int i=0;i<NX_POPUP_STR_NR_ITEMS;i++){
		if(Popup_STR[i] != NULL)free(Popup_STR[i]);
	}
	for(int i=0;i<NX_SETTINGS_STR_NR_ITEMS;i++){
		if(SettingsMenu_STR[i] != NULL)free(SettingsMenu_STR[i]);
	}
	for(int i=0;i<NX_NETWORK_STR_NR_ITEMS;i++){
		if(Network_STR[i] != NULL)free(Network_STR[i]);
	}
	
}