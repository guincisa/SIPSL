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

#include <stdio.h>


//#define SPARC

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


#define MAXTHREADS 8

#ifdef SPARC
#define MAXTHREADS 64
#define COMAPS 5
#define SIPENGINETH 10
#define SL_CCTH 64
#define LOGMIN
#else
#define COMAPS 2
#define SIPENGINETH 6
#define SL_CCTH 8
#define LOGSIP
#define LOGINF
#define LOGMIN
#define LOGDEV
#define LOGMEM
#define LOGNTW
//#define LOGLOK
#endif



//Mandatory

#define TIME_S {SysTime mytime; gettimeofday(&mytime.tv, &mytime.tz);\
		long long int num = ((long long int) mytime.tv.tv_sec)*1000000+(long long int)mytime.tv.tv_usec;\
		sprintf(bu, "%llu",num);}

#define BDEBUG(m1, m2) {stringstream xx ; \
		char bu[128];\
		TIME_S\
		string time(bu);\
		xx << m1 << " [" << pthread_self() << " " << time.substr(0,1) << "." << time.substr(1,3) << "." << time.substr(4,3)<< "-" << time.substr(7,3)<< "-" << time.substr(10,3)<< "." << time.substr(13,3) << "]"\
		<<  __FILE__ << " " <<__LINE__ << " [" << m2 << "]\n"; \
		cout << xx.str();cout.flush();}
//**********************************************************
#undef DEBASSERT
#define DEBASSERT(m1)  {stringstream xx ; xx << "\n\nDEBASSERT [" << pthread_self() << "]" <<  __FILE__ <<" " <<__LINE__<< "\n ************ \n"<< m1 << "\n ************ \n"; cout << xx.str();cout.flush();assert(0);}

//#define DEBASSERT(m1) {cout << "\n\nDEBASSERT [" << pthread_self() << "] "<< __FILE__ << " " << __LINE__<< " ******" << m1 << "*****" << endl<<endl<< endl; cout.flush();assert(0);}
//**********************************************************
#define WAITTIME { timespec sleep_time; \
	sleep_time.tv_sec = 20;\
	sleep_time.tv_nsec = 0;\
	nanosleep(&sleep_time,NULL);}


#define GETTIME(mytime) gettimeofday(&mytime.tv, &mytime.tz);

//HANDLER - Reference to Pointer
#define MKHANDMESSAGE(pointer,reference) \
		MESSAGE* pointer;\
		MESSAGE*& reference = pointer;

#define MESSAGEH MESSAGE*&

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
	if (m1 == MainMessage){DEBASSERT("Purging MainMessage")}\
	if (m1->getLock()) {DEBASSERT("Purging a locked message")}\
	map<const MESSAGE*, MESSAGE*>::iterator p; \
	pthread_mutex_lock(&messTableMtx);\
	p = globalMessTable.find(m1);\
	if (p !=globalMessTable.end()) {\
		globalMessTable.erase(m1);\
		DELPTR(m1,"MESSAGE");\
		m1 = MainMessage; \
	}\
	pthread_mutex_unlock(&messTableMtx);}
#define CREATEMESSAGE(m1, m2, gen, dest) MESSAGE* m1=0x0; {char bu[512];\
				SysTime inTime;\
				GETTIME(inTime);\
				NEWPTR2(m1, MESSAGE(m2, gen, inTime),"MESSAGE");\
				m1->setDestEntity(dest);\
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
				DEBY \
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
#define DEBMESSAGE(m1,m2) {stringstream xx ; xx << "DEBMESS [" << pthread_self() << "]" <<  __FILE__ <<" " <<__LINE__<< " "<< m1 << "\n" << "**************** MESSAGE CONTENT ***************************\n[" <<m2->getKey() << "] Gen [" <<m2->getGenEntity() << "] Dest ["<<m2->getDestEntity() <<"]\n["<< m2->getIncBuffer() << "]\n*********************************************************\n"; cout << xx.str();cout.flush();}
//**********************************************************
#undef DEBMESSAGESHORT
#define DEBMESSAGESHORT(m1,m2) {stringstream xx ; xx << "DEBMESS [" << pthread_self() << "]" <<  __FILE__ <<" " <<__LINE__<< " "<< m1 << "\n" << "**************** MESSAGE EXTRACTS ***************************\n[" << m2 << "]\n[" <<m2->getKey() << "]\n["<< m2->getLine(0) << "]\n*********************************************************\n"; cout << xx.str();cout.flush();}
	//**********************************************************
#else
#define DEBSIP(m1,m2)
#define DEBMESSAGE(m1,m2)
#define DEBMESSAGESHORT(m1,m2)
#endif
//**********************************************************
//**********************************************************
#ifdef LOGNTW
//1 288 696 804 323 901
//1.288.696.804.323.901
#undef PRTIME
#define PRTIME {SysTime mytime; gettimeofday(&mytime.tv, &mytime.tz);\
		long long int num = ((long long int) mytime.tv.tv_sec)*1000000+(long long int)mytime.tv.tv_usec;\
		char bu[128];\
		sprintf(bu, "%llu",num);\
		string time(bu);\
		stringstream xx ; xx << "TIME [" << time.substr(0,1) << "." << time.substr(1,3) << "." << time.substr(4,3)<< "-" << time.substr(7,3)<< "-" << time.substr(10,3)<< "." << time.substr(13,3)<<"]\n"; cout << xx.str();cout.flush();}
#undef PRTIME_F
#define PRTIME_F(m) {\
		char bu[128];\
		sprintf(bu, "%llu",m);\
		string time(bu);\
		stringstream xx ; xx << "TIME [" << time.substr(0,1) << "." << time.substr(1,3) << "." << time.substr(4,3)<< "-" << time.substr(7,3)<< "-" << time.substr(10,3)<< "." << time.substr(13,3)<<"]\n"; cout << xx.str();cout.flush();}


#undef DEBNTW
#define DEBNTW(m1,m2)  {stringstream xx ; xx << "DEBINF [" << pthread_self() << "]" <<  __FILE__ <<" " <<__LINE__<< " "<< m1 << "[" << m2 << "]\n"; cout << xx.str();cout.flush();}
#else
#define DEBNTW(m1,m2)
#endif
//**********************************************************
//**********************************************************
#ifdef LOGINF
	//**********************************************************
#undef DEBINF
#define DEBINF(m1,m2) BDEBUG("DEBINF", m1 << "[" << m2)
//#define DEBINF(m1,m2)  {stringstream xx ; xx << "DEBINF [" << pthread_self() << "]" <<  __FILE__ <<" " <<__LINE__<< " "<< m1 << "[" << m2 << "]\n"; cout << xx.str();cout.flush();}
	//**********************************************************
#undef DEBOUT
//#define DEBOUT(m1,m2)  {stringstream xx ; xx << "DEBOUT [" << pthread_self() << "]" <<  __FILE__ <<" " <<__LINE__<< " "<< m1 << "[" << m2 << "]\n"; cout << xx.str();cout.flush();}
#define DEBOUT(m1,m2) BDEBUG("DEBOUT", m1 << "[" << m2 << "]")
	//**********************************************************
#undef DEBOUT_UTIL
#define DEBOUT_UTIL(m1,m2)  {stringstream xx ; xx << "DEBOUT_UTIL [" << pthread_self() << "]"<<  __FILE__ <<" " <<__LINE__<< " "<< m1 << "[" << m2 << "]\n"; cout << xx.str();cout.flush();}
	//**********************************************************
#else
#define DEBOUT(m1,m2)
#define DEBOUT_UTIL(m1,m2)
#define DEBINF(m1,m2)
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
#define DEBDEV(m1,m2) BDEBUG("DEBDEV", m1 << "[" << m2)
#undef DEBY
#define DEBY  BDEBUG("DEBY", "")
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

#define DEBMEM(m1,m2) BDEBUG("DEBMEM", m1 << "[" << m2)

#undef DUMPMESSTABLE
#define DUMPMESSTABLE {map<const MESSAGE*, MESSAGE *>::iterator p;\
	pthread_mutex_lock(&messTableMtx);\
	DEBOUT("GLOBALMESSAGETABLE",&globalMessTable << "]["<<globalMessTable.size())\
	for (p=globalMessTable.begin() ; p != globalMessTable.end() ; p ++){\
		DEBOUT("***********MESSAGE in table", (MESSAGE*)p->second)\
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

#else
#define DEBMEM(m1,m2)
#define DUMPMESSTABLE
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


