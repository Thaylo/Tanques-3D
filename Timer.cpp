#include "Timer.h"

long int getCurrentTime(){
	 struct timeval now;
	 long int seconds, useconds;    

    gettimeofday(&now, (timezone*) 0);

    seconds  = now.tv_sec;
    useconds = now.tv_usec;
	 return ((seconds) * 1000 + useconds/1000.0) + 0.5;
}





