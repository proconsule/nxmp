#include "logger.h"



namespace NXLOG{
		
	int loglevel = 0;
	
	
	
	std::string stringFmt(const char *fmt, ...) {
		va_list args;
		va_start(args, fmt);
		char buf[32];
		size_t n = std::vsnprintf(buf, sizeof(buf), fmt, args);
		va_end(args);

		// Static buffer large enough?
		if (n < sizeof(buf)) {
			return {buf, n};
		}

		// Static buffer too small
		std::string s(n + 1, 0);
		va_start(args, fmt);
		std::vsnprintf(const_cast<char*>(s.data()), s.size(), fmt, args);
		va_end(args);

		return s;
	}
	
	
	void INFOLOG(const char *fmt, ...){
		if(loglevel > 0){
			char outbuff[1024];
			va_list arglist;
			va_start( arglist, fmt );
			vprintf( fmt, arglist );
			std::vsnprintf(outbuff, sizeof outbuff, fmt, arglist);
			va_end( arglist );
			fflush(stdout);
			
			if(ConsoleWindow != nullptr){
				ConsoleWindow->AddText(outbuff);
			}
		}
		

	
	}
	
	void ERRORLOG(const char *fmt, ...){
		if(loglevel > 0){
			char outbuff[1024];
			printf( "Error: " );
			va_list arglist;
			va_start( arglist, fmt );
			vprintf( fmt, arglist );
			std::vsnprintf(outbuff, sizeof outbuff, fmt, arglist);
			va_end( arglist );
			fflush(stdout);
			
			if(ConsoleWindow != nullptr){
				ConsoleWindow->AddText(outbuff);
			}
		}
	
	}
	
	void DEBUGLOG(const char *fmt, ...){
		if(loglevel > 1){
			char outbuff[1024];
			va_list arglist;
			va_start( arglist, fmt );
			vprintf( fmt, arglist );
			std::vsnprintf(outbuff, sizeof outbuff, fmt, arglist);
			va_end( arglist );
			fflush(stdout);
			
			if(ConsoleWindow != nullptr){
				
				
				ConsoleWindow->AddText(outbuff);
			}
		}
	
	}


}