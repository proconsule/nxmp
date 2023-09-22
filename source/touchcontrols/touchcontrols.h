#ifndef NXMP_TOUCHCONTROLS_H
#define NXMP_TOUCHCONTROLS_H

#include "config.h"
#include "gui.h"
#include <SDL.h>


namespace TOUCHCONTROLS{
	
	void fingerDown(SDL_Event *sdlevent);
	void fingerMotion(SDL_Event *sdlevent);
	void fingerUp(SDL_Event *sdlevent);
	
}



#endif