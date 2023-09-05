#ifndef NXMP_MTPCLASS_H
#define NXMP_MTPCLASS_H

#include <chrono>
#include <thread>




class CMTP{
public:
	 CMTP();
	 ~CMTP();
	 
	 void StartServer();
	 

	 std::thread mtpthread;
	 bool should_exit = false;
	
	 
};



#endif