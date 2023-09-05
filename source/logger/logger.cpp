#include "logger.h"



namespace NXLOG{
		
	int loglevel = 0;
	
	void INFOLOG(const char *fmt, ...){
		if(loglevel > 0){
			va_list arglist;
			va_start( arglist, fmt );
			vprintf( fmt, arglist );
			va_end( arglist );
		}
	
	}
	
	void ERRORLOG(const char *fmt, ...){
		if(loglevel > 0){
			printf( "Error: " );
			va_list arglist;
			va_start( arglist, fmt );
			vprintf( fmt, arglist );
			va_end( arglist );
		}
	
	}
	
	void DEBUGLOG(const char *fmt, ...){
		if(loglevel > 1){
			va_list arglist;
			va_start( arglist, fmt );
			vprintf( fmt, arglist );
			va_end( arglist );
		}
	
	}


}