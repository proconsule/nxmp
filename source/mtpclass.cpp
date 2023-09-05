#include "mtpclass.h"
#include <string>

using namespace std::chrono_literals;

CMTP::CMTP(){
	
	should_exit = false;
	printf("Init MTP Server\n");
	
	
}

CMTP::~CMTP(){
	should_exit = true;
	
	
	mtpthread.join();
	printf("DEINIT MTP CLASS\r\n");
}

void mtp_thread_func(bool *should_exit) {
	
				

	
}



void CMTP::StartServer(){
	
	
	
	
	//mtpthread = std::thread(mtp_thread_func, &should_exit);
	
	
}