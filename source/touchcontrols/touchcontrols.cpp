#include "touchcontrols.h"


float swipex = 0.0;
float swipey = 0.0;
int fingersum = 0; 

namespace TOUCHCONTROLS{
	
	void fingerDown(SDL_Event *sdlevent){
		swipex = 0.0;
		swipey = 0.0;
		fingersum += 1; 
		//printf("FINGER DOWN %f %f\n", event.tfinger.x, event.tfinger.y);
		fflush(stdout);
		
	}
	void fingerMotion(SDL_Event *sdlevent){
#ifdef _WIN32
		swipex = sdlevent->tfinger.dx;
		swipey = sdlevent->tfinger.dy;
#endif
#ifdef __SWITCH__
		swipex += sdlevent->tfinger.dx;
		swipey += sdlevent->tfinger.dy;
#endif
		
	}
	void fingerUp(SDL_Event *sdlevent){
		if(fingersum == 1 && item.state == MENU_STATE_PLAYER){
			
			if(swipex>0.2){
					if(configini->getPlayerSwipeSeek(false) == 0){
						SDL_Event sdlevent;
						sdlevent.type = SDL_JOYBUTTONDOWN;
						sdlevent.jbutton.button = GUI::SDL_KEYS::SDL_KEY_R;
						SDL_PushEvent(&sdlevent);
					}
					if(configini->getPlayerSwipeSeek(false) == 1){
						SDL_Event sdlevent;
						sdlevent.type = SDL_JOYBUTTONDOWN;
						sdlevent.jbutton.button = GUI::SDL_KEYS::SDL_KEY_ZR;
						SDL_PushEvent(&sdlevent);
					}
				
							
			}else if(swipex<-0.2){
							
					if(configini->getPlayerSwipeSeek(false) == 0){
						SDL_Event sdlevent;	
						sdlevent.type = SDL_JOYBUTTONDOWN;
						sdlevent.jbutton.button = GUI::SDL_KEYS::SDL_KEY_L;
						SDL_PushEvent(&sdlevent);
					}
					if(configini->getPlayerSwipeSeek(false) == 1){
						SDL_Event sdlevent;	
						sdlevent.type = SDL_JOYBUTTONDOWN;
						sdlevent.jbutton.button = GUI::SDL_KEYS::SDL_KEY_ZL;
						SDL_PushEvent(&sdlevent);
					}
				
			}
						
			printf("FINGER UP ONE FINGER %f %f / %f %f\n", sdlevent->tfinger.x, sdlevent->tfinger.y,swipex,swipey);
						
		}
		if(fingersum == 2 && item.state == MENU_STATE_PLAYER){
			if(swipex>0.2){
				SDL_Event sdlevent;
				sdlevent.type = SDL_JOYBUTTONDOWN;
				sdlevent.jbutton.button = GUI::SDL_KEYS::SDL_KEY_DRIGHT;
				SDL_PushEvent(&sdlevent);
			}
			else if(swipex<-0.2){
				SDL_Event sdlevent;
				sdlevent.type = SDL_JOYBUTTONDOWN;
				sdlevent.jbutton.button = GUI::SDL_KEYS::SDL_KEY_DLEFT;
				SDL_PushEvent(&sdlevent);
			}
			else if(swipey>0.2){
				SDL_Event sdlevent;
				sdlevent.type = SDL_JOYBUTTONDOWN;
				sdlevent.jbutton.button = GUI::SDL_KEYS::SDL_KEY_DDOWN;
				SDL_PushEvent(&sdlevent);
			}
			else if(swipey<-0.2){
				SDL_Event sdlevent;
				sdlevent.type = SDL_JOYBUTTONDOWN;
				sdlevent.jbutton.button = GUI::SDL_KEYS::SDL_KEY_DUP;
				SDL_PushEvent(&sdlevent);
			}
						
			printf("FINGER UP TWO FINGER %f %f / %f %f\n", sdlevent->tfinger.x, sdlevent->tfinger.y,swipex,swipey);
			fflush(stdout);
		}
		
		fingersum = 0;
		
	}
	
}





