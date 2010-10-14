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

#include <assert.h>
#include <iostream>
#include <sstream>
#include <sys/time.h>
#include <vector>
#include <algorithm>


using namespace std;

#define WINDOWS
#ifdef WINDOWS
typedef time_t __time_t;
#endif

typedef long long int lli;

typedef struct {
	struct timeval tv;
	struct timezone tz;
} SysTime;

#define ECHOMAX 2048

class ThreadWrapper {
    public:
        pthread_t thread;
        pthread_mutex_t mutex;
        ThreadWrapper();
};

#define COMAPS 2

#define LOGSIP
#define LOGINF
#define LOGMIN
#define LOGDEV
#define LOGMEM
#define LOGNTW

//#define LOGLOK
//Mandatory
//**********************************************************
#undef DEBASSERT
#define DEBASSERT(m1) cout << "DEBASSERT " << __FILE__ << " " << __LINE__<< " " << m1 << endl; cout.flush();assert(0);
//**********************************************************
#define WAITTIME { timespec sleep_time; \
	sleep_time.tv_sec = 20;\
	sleep_time.tv_nsec = 0;\
	nanosleep(&sleep_time,NULL);}

#define GETTIME(mytime) gettimeofday(&mytime.tv, &mytime.tz);

#undef NEWPTR
#define NEWPTR(type, m1, m2,mess) type m1 = 0x0;\
	m1 = new (nothrow) m2;\
	if (m1 == 0x0) { DEBASSERT("Alloc failed")}
//no embedded declaration
#undef  NEWPTR2
#define NEWPTR2(m1, m2, mess) m1 = 0x0;\
	m1 = new (nothrow) m2;\
	if (m1 == 0x0) { DEBASSERT("Alloc failed")}
#undef DELPTR
#define DELPTR(m1, mess) \
		delete m1;
#undef GETLOCK
#define GETLOCK(m,message) \
		pthread_mutex_lock(m);
#undef RELLOCK
#define RELLOCK(m,message) \
		pthread_mutex_unlock(m);
#undef TRYCATCH
#define TRYCATCH(m) try { m; } catch (exception& e) { DEBASSERT("Exception" << e.what())}
#undef PURGEMESSAGE
#define PURGEMESSAGE(m1)  { \
	map<const MESSAGE*, MESSAGE*>::iterator p; \
	pthread_mutex_lock(&messTableMtx);\
	p = globalMessTable.find(m1);\
	if (p !=globalMessTable.end()) {\
		globalMessTable.erase(m1);\
		DELPTR(m1,"MESSAGE");\
	}\
	pthread_mutex_unlock(&messTableMtx);}
#define CREATEMESSAGE(m1, m2, m3) MESSAGE* m1=0x0; {char bu[512];\
				SysTime inTime;\
				GETTIME(inTime);\
				NEWPTR2(m1, MESSAGE(m2, m3, inTime),"MESSAGE");\
				int i= m1->getTotLines();\
				DEBDEV("New MESSAGE"," " << i);\
				long long int num = ((long long int) inTime.tv.tv_sec)*1000000+(long long int)inTime.tv.tv_usec;\
				sprintf(bu, "%x%llu",m1,num);\
				string key(bu);\
				m1->setKey(key);\
				pthread_mutex_lock(&messTableMtx);\
				globalMessTable.insert(pair<const MESSAGE*, MESSAGE*>(m1, m1));\
				pthread_mutex_unlock(&messTableMtx);}

#define CREATENEWMESSAGE(__mess, __echob, __sock, __echoAddr, __sode) {char bu[512];\
				SysTime inTime;\
				GETTIME(inTime);\
				NEWPTR2(__mess, MESSAGE(__echob, __sode, inTime, __sock, __echoAddr),"MESSAGE");\
				int i= m1->getTotLines();\
				long long int num = ((long long int) inTime.tv.tv_sec)*1000000+(long long int)inTime.tv.tv_usec;\
				sprintf(bu, "%x%llu",(unsigned int)__mess,num);\
				string key(bu);\
				__mess->setKey(key);\
				pthread_mutex_lock(&messTableMtx);\
				globalMessTable.insert(pair<const MESSAGE*, MESSAGE*>(__mess, _mess));\
				pthread_mutex_unlock(&messTableMtx);}

#define CREATENEWMESSAGE_EXT(__mess, __echob, __sock, __echoAddr, __sode) {char bu[512];\
				SysTime inTime;\
				GETTIME(inTime);\
				NEWPTR2(__mess, MESSAGE(__echob, __sode, inTime, __sock, __echoAddr),"MESSAGE");\
				if (__mess != 0x0 ) {long long int num = ((long long int) inTime.tv.tv_sec)*1000000+(long long int)inTime.tv.tv_usec;\
				sprintf(bu, "%x%llu",(unsigned int)__mess,num);\
				string key(bu);\
				__mess->setKey(key);\
				pthread_mutex_lock(&messTableMtx);\
				globalMessTable.insert(pair<const MESSAGE*, MESSAGE*>(__mess, __mess));\
				pthread_mutex_unlock(&messTableMtx);}}

//**********************************************************
//**********************************************************
#ifdef LOGSIP
	//**********************************************************
#undef DEBSIP
#define DEBSIP(m1,m2)  {stringstream xx ; xx << "DEBSIP [" << pthread_self() << "]" <<  __FILE__ <<" " <<__LINE__<< " "<< m1 << "[" << m2 << "]\n"; cout << xx.str();cout.flush();}
	//**********************************************************
#undef DEBMESSAGE
#define DEBMESSAGE(m1,m2) {stringstream xx ; xx << "DEBMESS [" << pthread_self() << "]" <<  __FILE__ <<" " <<__LINE__<< " "<< m1 << "\n" << "**************** MESSAGE CONTENT ***************************\n[" <<m2->getKey() << "]\n["<< m2->getIncBuffer() << "]\n*********************************************************\n"; cout << xx.str();cout.flush();}
//**********************************************************
#undef DEBMESSAGESHORT
#define DEBMESSAGESHORT(m1,m2) {stringstream xx ; xx << "DEBMESS [" << pthread_self() << "]" <<  __FILE__ <<" " <<__LINE__<< " "<< m1 << "\n" << "**************** MESSAGE EXTRACTS ***************************\n[" << m2 << "]\n[" <<m2->getKey() << "]\n["<< m2->getLine(0) << "]\n*********************************************************\n"; cout << xx.str();cout.flush();}
	//**********************************************************
#endif
//**********************************************************
//**********************************************************
#ifdef LOGNTW
#undef DEBNTW
#define DEBNTW(m1,m2)  {stringstream xx ; xx << "DEBINF [" << pthread_self() << "]" <<  __FILE__ <<" " <<__LINE__<< " "<< m1 << "[" << m2 << "]\n"; cout << xx.str();cout.flush();}
#endif
//**********************************************************
//**********************************************************
#ifdef LOGINF
	//**********************************************************
#undef DEBINF
#define DEBINF(m1,m2)  {stringstream xx ; xx << "DEBINF [" << pthread_self() << "]" <<  __FILE__ <<" " <<__LINE__<< " "<< m1 << "[" << m2 << "]\n"; cout << xx.str();cout.flush();}
	//**********************************************************
#undef DEBOUT
#define DEBOUT(m1,m2)  {stringstream xx ; xx << "DEBOUT [" << pthread_self() << "]" <<  __FILE__ <<" " <<__LINE__<< " "<< m1 << "[" << m2 << "]\n"; cout << xx.str();cout.flush();}
	//**********************************************************
#undef DEBOUT_UTIL
#define DEBOUT_UTIL(m1,m2)  {stringstream xx ; xx << "DEBOUT_UTIL " <<  __FILE__ <<" " <<__LINE__<< " "<< m1 << "[" << m2 << "]\n"; cout << xx.str();cout.flush();}
	//**********************************************************
#endif
//**********************************************************
//**********************************************************


//**********************************************************
//**********************************************************
#ifdef LOGMIN
	//**********************************************************
#undef DEBERROR
#define DEBERROR(m1)  {stringstream xx ; xx << "**** RUNTIME ERROR **** " << __FILE__ <<" " <<__LINE__<< "[" << m1 << "]\n";cout << xx.str();cout.flush();}
	//**********************************************************
#endif
//**********************************************************
//**********************************************************

//**********************************************************
//**********************************************************
#ifdef LOGDEV
#undef DEBCODE
#define DEBCODE(m) {m}
#undef DEBDEV
#define DEBDEV(m1,m2)  {stringstream xx ; xx << "DEBDEV [" << pthread_self() << "]" <<  __FILE__ <<" " <<__LINE__<< " "<< m1 << "[" << m2 << "]\n"; cout << xx.str();cout.flush();}
#undef DEBY
#define DEBY  {stringstream xx ; xx << "DEBY " << __FILE__ <<" " <<__LINE__<< "\n";cout << xx.str();cout.flush();}
#undef PRINTTIME
#define PRINTTIME(starttime,endtime){char bu[1024];sprintf(bu, "init %lld end %lld diff %lld",(lli)starttime.tv.tv_sec*1000000+(lli)starttime.tv.tv_usec, (lli)endtime.tv.tv_sec*1000000+(lli)bbb.tv.tv_usec, (lli)endtime.tv.tv_sec*1000000+(lli)bbb.tv.tv_usec - (lli)starttime.tv.tv_sec*1000000-(lli)starttime.tv.tv_usec );DEBOUT("TIME INTERVAL", bu )}
#undef PRINTTIMESHORT
#define PRINTTIMESHORT(m,starttime){char bu[1024];sprintf(bu, "time %lld",(lli)starttime.tv.tv_sec*1000000+(lli)starttime.tv.tv_usec);DEBOUT(m, bu )}
#else
#define DEBY
#define DEBDEV(m1,m2)
#define PRINTTIME(starttime,endtime)
#define PRINTTIMESHORT(m,starttime)
#define DEBCODE(m)
#endif
//**********************************************************
//**********************************************************

//**********************************************************
//**********************************************************
#ifdef LOGMEM
	//**********************************************************

#define DEBMEM(m1,m2)  {stringstream xx ; xx << "DEBMEM [" << pthread_self() << "]" <<  __FILE__ <<" " <<__LINE__<< " "<< m1 << "[" << m2 << "]\n"; cout << xx.str();cout.flush();}
#undef DUMPMESSTABLE
#define DUMPMESSTABLE {map<const MESSAGE*, MESSAGE *>::iterator p;\
	pthread_mutex_lock(&messTableMtx);\
	DEBOUT("GLOBALMESSAGETABLE",&globalMessTable << "]["<<globalMessTable.size())\
	for (p=globalMessTable.begin() ; p != globalMessTable.end() ; p ++){\
		DEBOUT("***********MESSAGE in table", (MESSAGE*)p->second)\
		DEBMESSAGE("MESSAGE in table" ,((MESSAGE*)p->second) ) \
	}\
	pthread_mutex_unlock(&messTableMtx);}
	//**********************************************************
#undef NEWPTR
#define NEWPTR(type, m1, m2,mess) type m1 = 0x0;\
	m1 = new (nothrow) m2;\
	DEBOUT("NEW ", mess << "][" <<m1)\
	if (m1 == 0x0) { DEBOUT("NEW allocation failed", "") DEBASSERT("Alloc failed")}
	//**********************************************************
//no embedded declaration
#undef NEWPTR2
#define NEWPTR2(m1, m2, mess) m1 = 0x0;\
	m1 = new (nothrow) m2;\
	DEBOUT("NEW ", mess << "][" <<m1)\
	if (m1 == 0x0) { DEBERROR("NEW allocation failed")}
	//**********************************************************
#undef DELPTR
#define DELPTR(m1, mess) \
		DEBOUT("DELPTR",mess<<"]["<<m1)\
		delete m1;

#endif
//**********************************************************
//**********************************************************

//**********************************************************
//**********************************************************
#ifdef LOGLOK
#undef GETLOCK
#define GETLOCK(m,message) \
		DEBOUT("Reaching lock " << message, m)\
		pthread_mutex_lock(m);\
		DEBOUT("Acquired lock " << message, m)
#undef RELLOCK
#define RELLOCK(m,message) \
		DEBOUT("Releasing lock " << message, m)\
		pthread_mutex_unlock(m);
#endif
//**********************************************************
//**********************************************************


