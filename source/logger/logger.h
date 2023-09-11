#ifndef NXMP_LOGGER_H
#define NXMP_LOGGER_H

#include <iostream>
#include <cstdio>
#include <cstdarg>

#include "consolewindow.h"

namespace NXLOG{
	
	extern int loglevel;
	extern CConsoleWindow *ConsoleWindow;
	
	void INFOLOG(const char *fmt, ...);
	void ERRORLOG(const char *fmt, ...);
	void DEBUGLOG(const char *fmt, ...);
	
	
}


#endif