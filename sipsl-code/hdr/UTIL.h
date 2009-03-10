//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Service Layer
// Copyright (C) 2007 Guglielmo Incisa di Camerana
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************

#define UTIL_H
//#include <pthread.h>
//#include <string>

//#include <memory>

//#include <sys/socket.h>
//#include <arpa/inet.h>

#include <iostream>
#include <sys/time.h>

using namespace std;

typedef struct {
	struct timeval tv;
	struct timezone tz;
} SysTime;

#define TESTING

#define ECHOMAX 2048

#define GETTIME(mytime) gettimeofday(&mytime.tv, &mytime.tz);

//#define DECTIME char bu[512];hrtime_t inittime;hrtime_t endtime;
#define STARTTIME {GETTIME(inittime)}
#define ENDTIME {char bu[512];GETTIME(endtime); sprintf(bu, "[%s %d] init %lld end %lld tot %lld\n",__FILE__, __LINE__, inittime.tv.tv_sec*1000000+inittime.tv.tv_usec,endtime.tv.tv_sec*1000000+endtime.tv.tv_usec ,endtime.tv.tv_sec*1000000+endtime.tv.tv_usec  - inittime.tv.tv_sec*1000000+inittime.tv.tv_usec ); cout << bu << flush;}

#define DEBOUT(m1,m2)  cout << "DEBOUT " << __FILE__ <<" " <<__LINE__<< " "<< m1 << "[" << m2 << "]"<< endl;
#define DEBERROR(m1)  cout << "**** RUNTIME ERROR **** " << __FILE__ <<" " <<__LINE__<< "[" << m1 << "]"<< endl;
#define DEBY  cout << "DEBY " << __FILE__ <<" " <<__LINE__<< endl;

class ThreadWrapper {
    public:
        pthread_t thread;
        pthread_mutex_t mutex;
        ThreadWrapper();
};

