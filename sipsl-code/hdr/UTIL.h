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


#define SPARC

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
        //pthread_mutex_t mutex;
        ThreadWrapper();
};





//Tuning
//COMAPS: number of comaps
//COMAPS: number of gloabl message tables
//MAXTHREADS: threads initialized in ENGINE
//SIPENGINETH: SIPENGINE activated threads
//SL_CCTH: SL_CC activated threads
//ARR: dimension of the buffer
//ARR_SHORT: dimension of the overload buffer
//SPINC_MOD: number of locks in the buffer
//DOA_CLEANUP: seconds every loop of the DOA
//TIMER_DOA: milliseconds before a call_oset is set to DELETED
//TIMER_DOA / 2 : milliseconds before a DELETED call_oset if purged

//#define NOLOGATALL

#define SPINSTL

#ifdef SPARC

//The call_oset and valo do not reinject the message
//but recall the sl_co->call
#define NONESTEDPW

//Invert the get call_oset lock and release comap lok
//should core if imverted
#define WRONGLOCKMGMT

//Use the gobalmessagetable
//#define USEMESSAGEMAP

//The state machine will send the message directly to transport
#define USEFASTSEND

//#define USETRYLOCK

//Sudp threads
#define SUDPTH 6

//Max engine threads
#define MAXTHREADS 128

//
#define SIPENGINETH 64
#define SIPENGINMAPS 16

#define TRNSPRTTH 64
#define TRNSPRTMAPS 16

#define SL_CCTH 128
#define SL_CCMAPS 32

//#define ENGINEMAPS 5
#define ALARMTH 16
#define ALARMMAPS 8
#define ALARMENGINE
#define TRYMAXLOCKALARM 9

#define COMAPS 32
#define COMAPS_DIG 2
#define ADDRESSPACE 8
#define MESSAGEMAPS 100

#define ARR 4000
#define ARR_SHORT 30
#define SPINC_MOD 50
#define DOA_CLEANUP 1
#define TIMER_DOA 5000000


#ifndef NOLOGATALL
#define LOGMIN
#define PROFILELOCK

#define LOGSIPHIGH
#define LOGSIPLOW

//#define LOGINF
#define LOGMIN
//#define LOGDEV
#define LOGMEM
#define LOGNTW
#define SELFCHECK
//#define DEBCODE
#define PROFILING
//#define MESSAGEUSAGE
#define DEBCODEALARM1
//#define LOGLOK
#endif


#else
#define LOGALO
#define COMAPS 6
#define MAXTHREADS 8
#define SIPENGINETH 6
#define SL_CCTH 8
#define ARR 100
#define ARR_SHORT 30
#define SPINC_MOD 10
#define DOA_CLEANUP 1
#define TIMER_DOA 4000000
#define LOGINF
#define LOGMIN
#define LOGDEV
#define LOGMEM
#define LOGNTW
#define ADDRESSPACE 8
#define MESSAGEMAPS 20
#define DEBCODE


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

#undef DEBWARNING
#define DEBWARNING(m1,m2)  BDEBUG("******************************\n   WARNING   \n******************************\n"<<m1,m2)


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
#undef DELPTRARR
#define DELPTRARR(m1, mess) \
		delete m1[];


#undef TRYCATCH
#define TRYCATCH(m) try { m; } catch (exception& e) { DEBOUT("TRYCATCH", e.what()) DEBASSERT("Exception")}
#undef PURGEMESSAGE

#ifdef USEMESSAGEMAP
#define PURGEMESSAGE(m1)  { \
	if (m1 == MainMessage){DEBASSERT("Purging MainMessage")}\
	if (m1->getLock()) {DEBASSERT("Purging a locked message")}\
	map<const MESSAGE*, MESSAGE*>::iterator p; \
	int i = getModulus(m1);\
	GETLOCK(&messTableMtx[i],"&messTableMtx"<<i);\
	p = globalMessTable[i].find(m1);\
	if (p !=globalMessTable[i].end()) {\
		globalMessTable[i].erase(m1);\
		DELPTR(m1,"MESSAGE");\
		m1 = MainMessage; \
	}\
	RELLOCK(&messTableMtx[i],"&messTableMtx"<<i);}
#define CREATEMESSAGE(m1, m2, gen, dest) MESSAGE* m1=0x0; {char bu[512];\
				SysTime inTime;\
				GETTIME(inTime);\
				NEWPTR2(m1, MESSAGE(m2, gen, inTime),"MESSAGE");\
				m1->setDestEntity(dest);\
				int i= m1->getTotLines();\
				DEBDEV("New MESSAGE"," " << i);\
				long long int num = ((long long int) inTime.tv.tv_sec)*1000000+(long long int)inTime.tv.tv_usec;\
				sprintf(bu, "%x%llu",(unsigned int)m1,num);\
				string key(bu);\
				m1->setKey(key);\
				int j = getModulus(m1);\
				GETLOCK(&messTableMtx[j],"&messTableMtx"<<j);\
				globalMessTable[j].insert(pair<const MESSAGE*, MESSAGE*>(m1, m1));\
				RELLOCK(&messTableMtx[j],"&messTableMtx"<<j);}

#define CREATENEWMESSAGE_EXT(__mess, __echob, __sock, __echoAddr, __sode) {char bu[512];\
				SysTime inTime;\
				GETTIME(inTime);\
				NEWPTR2(__mess, MESSAGE(__echob, __sode, inTime, __sock, __echoAddr),"MESSAGE");\
				DEBY \
				if (__mess != 0x0 ) {long long int num = ((long long int) inTime.tv.tv_sec)*1000000+(long long int)inTime.tv.tv_usec;\
				sprintf(bu, "%x%llu",(unsigned int)__mess,num);\
				string key(bu);\
				__mess->setKey(key);\
				int i = getModulus(__mess);\
				GETLOCK(&messTableMtx[i],"&messTableMtx"<<i);\
				globalMessTable[i].insert(pair<const MESSAGE*, MESSAGE*>(__mess, __mess));\
				RELLOCK(&messTableMtx[i],"&messTableMtx"<<i);}}
#else
#define PURGEMESSAGE(m1)  DELPTR(m1,"MESSAGE")
#define CREATEMESSAGE(m1, m2, gen, dest) MESSAGE* m1=0x0; {char bu[512];\
				SysTime inTime;\
				GETTIME(inTime);\
				NEWPTR2(m1, MESSAGE(m2, gen, inTime),"MESSAGE");\
				m1->setDestEntity(dest);\
				int i= m1->fillIn();\
				DEBDEV("New MESSAGE"," " << i);\
				long long int num = ((long long int) inTime.tv.tv_sec)*1000000+(long long int)inTime.tv.tv_usec;\
				sprintf(bu, "%x%llu",(unsigned int)m1,num);\
				string key(bu);\
				m1->setKey(key);}

#define CREATENEWMESSAGE_EXT(__mess, __echob, __sock, __echoAddr, __sode) {char bu[512];\
				SysTime inTime;\
				GETTIME(inTime);\
				NEWPTR2(__mess, MESSAGE(__echob, __sode, inTime, __sock, __echoAddr),"MESSAGE");\
				DEBY \
				if (__mess != 0x0 ) {long long int num = ((long long int) inTime.tv.tv_sec)*1000000+(long long int)inTime.tv.tv_usec;\
				sprintf(bu, "%x%llu",(unsigned int)__mess,num);\
				string key(bu);\
				__mess->setKey(key);}}

#endif
//**********************************************************
#ifdef PROFILING
//#define PROFILE(m) DEBOUT("PROFILING",m)
#define PROFILE(m)
#define TIMEDEF SysTime mytime1111;
#define SETNOW gettimeofday(&mytime1111.tv, &mytime1111.tz);
#define PRINTDIFF(m) {SysTime mytime2222; gettimeofday(&mytime2222.tv, &mytime2222.tz);\
		long long int num = ((long long int) ( mytime2222.tv.tv_sec - mytime1111.tv.tv_sec))*1000000+((long long int)(mytime2222.tv.tv_usec - mytime1111.tv.tv_usec));\
                gettimeofday(&mytime1111.tv, &mytime1111.tz);\
		DEBOUT("PROFILE DIFFERENCE ", m << "]["<<num)}
#define PRINTDIFFMIN(m,min) {SysTime mytime2222; gettimeofday(&mytime2222.tv, &mytime2222.tz);\
		long long int num = ((long long int) ( mytime2222.tv.tv_sec - mytime1111.tv.tv_sec))*1000000+((long long int)(mytime2222.tv.tv_usec - mytime1111.tv.tv_usec));\
                gettimeofday(&mytime1111.tv, &mytime1111.tz);\
		if (num >= min )DEBOUT("PROFILE DIFFERENCE ", m << "][#"<<num<<"#")}
#else
#define PROFILE(m)
#endif
//**********************************************************
#ifdef LOGALO
#define DEBALO(m1,m2) DEBOUT("DEBALO " << m1, m2)
#else
#define DEBALO(m1,m2)
#endif
#ifdef LOGSIPLOW
#define LOGSIPHIGH

	//**********************************************************
#undef DEBSIP
#define DEBSIP(m1,m2)  {stringstream xx ; xx << "DEBSIP [" << pthread_self() << "]" <<  __FILE__ <<" " <<__LINE__<< " "<< m1 << "[" << m2 << "]\n"; cout << xx.str();cout.flush();}
	//**********************************************************
#else
#define DEBSIP(m1,m2)
#define DEBMESSAGE(m1,m2)
#define DEBMESSAGESHORT(m1,m2)
#endif
#ifdef LOGSIPHIGH
#undef DEBMESSAGE
#define DEBMESSAGE(m1,m2) {stringstream xx ; xx << "DEBMESS [" << pthread_self() << "]" <<  __FILE__ <<" " <<__LINE__<< " "<< m1 << "\n" << "**************** MESSAGE CONTENT ***************************\n[" <<m2->getKey() << "] Gen [" <<m2->getGenEntity() << "] Dest ["<<m2->getDestEntity() <<"]\n["<< m2->getOriginalString() << "]\n*********************************************************\n"; cout << xx.str();cout.flush();}
//**********************************************************
#undef DEBMESSAGESHORT
#define DEBMESSAGESHORT(m1,m2) {stringstream xx ; xx << "DEBMESS [" << pthread_self() << "]" <<  __FILE__ <<" " <<__LINE__<< " "<< m1 << "\n" << "**************** MESSAGE EXTRACTS ***************************\n[" << m2 << "]\n[" <<m2->getKey() << "]\n["<< m2->getFirstLine() << "]\n*********************************************************\n"; cout << xx.str();cout.flush();}
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
#define PRTIME
#define PRTIME_F(m)
#endif
//**********************************************************
//**********************************************************
#ifdef LOGINF
	//**********************************************************
#undef DEBINF
#define DEBINF(m1,m2) BDEBUG("DEBINF", m1 << "[" << m2)
//#define DEBINF(m1,m2)  {stringstream xx ; xx << "DEBINF [" << pthread_self() << "]" <<  __FILE__ <<" " <<__LINE__<< " "<< m1 << "[" << m2 << "]\n"; cout << xx.str();cout.flush();}
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
//**********************************************************
#undef DEBOUT
//#define DEBOUT(m1,m2)  {stringstream xx ; xx << "DEBOUT [" << pthread_self() << "]" <<  __FILE__ <<" " <<__LINE__<< " "<< m1 << "[" << m2 << "]\n"; cout << xx.str();cout.flush();}
#define DEBOUT(m1,m2) BDEBUG("DEBOUT", m1 << "[" << m2 << "]")
#else
#define DEBERROR(m)
#endif
//**********************************************************
//**********************************************************

//**********************************************************
//**********************************************************
#ifdef LOGDEV
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
	for(int i = 0; i < COMAPS;i++){\
	pthread_mutex_lock(&messTableMtx[i]);\
	DEBOUT("GLOBALMESSAGETABLE",i << "]["<<&globalMessTable[i] << "]["<<globalMessTable[i].size())\
	for (p=globalMessTable[i].begin() ; p != globalMessTable[i].end() ; p ++){\
		DEBOUT("***********MESSAGE in table ", i<<"]["<<(MESSAGE*)p->second)\
	}\
	pthread_mutex_unlock(&messTableMtx[i]);}}
	//**********************************************************
#undef NEWPTR
#define NEWPTR(type, m1, m2,mess) type m1 = 0x0;\
	try {\
		m1 = new m2;\
		DEBOUT("NEW ", mess <<"]["<<m1)\
		if (m1 == 0x0){\
			DEBASSERT("NEWPTR failed")\
		}\
	}\
	catch (bad_alloc &b){\
		DEBOUT("NEW BAD ALLOC",b.what())\
		DEBASSERT("BAD ALLOC")\
	}
	//**********************************************************
//no embedded declaration
#undef NEWPTR2
#define NEWPTR2(m1, m2,mess) m1 = 0x0;\
	try {\
		m1 = new m2;\
		DEBOUT("NEW ", mess <<"]["<<m1)\
		if (m1 == 0x0){\
			DEBASSERT("NEWPTR failed")\
		}\
	}\
	catch (bad_alloc &b){\
		DEBOUT("NEW BAD ALLOC",b.what())\
		DEBASSERT("BAD ALLOC")\
	}

	//**********************************************************
#undef DELPTR
#define DELPTR(m1, mess) \
		DEBOUT("DELPTR",mess<<"]["<<m1)\
		delete m1;
#undef DELPTRARR
#define DELPTRARR(m1, mess) \
		DEBOUT("DELPTRARR",mess<<"]["<<m1)\
		delete[] m1;

#else
#define DEBMEM(m1,m2)
#define DUMPMESSTABLE
#endif
//**********************************************************
//**********************************************************

//**********************************************************
//**********************************************************
#undef GETLOCK
#define GETLOCK(m,message) \
		pthread_mutex_lock(m);
#undef RELLOCK
#define RELLOCK(m,message) \
		pthread_mutex_unlock(m);
#undef TRYLOCK
#define TRYLOCK(m,message,r)\
		r = pthread_mutex_trylock(m);
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
#undef TRYLOCK
#define TRYLOCK(m,message,r)\
		DEBOUT("Trying lock " << message, m)\
		r = pthread_mutex_trylock(m);\
		DEBOUT("Trying lock result "<< r , m)
#endif
#ifdef PROFILELOCK
#undef GETLOCK
#define GETLOCK(m,message) \
    {TIMEDEF SETNOW pthread_mutex_lock(m); PRINTDIFFMIN("Wait on lock " << message,10)}
#undef RELLOCK
#define RELLOCK(m,message) \
		pthread_mutex_unlock(m);
#undef TRYLOCK
#define TRYLOCK(m,message,r)\
		r = pthread_mutex_trylock(m);
#endif
#ifdef PROFILELOCKVERB
#undef GETLOCK
#define GETLOCK(m,message) \
    {TIMEDEF SETNOW pthread_mutex_lock(m); PRINTDIFFMIN("Got lock - Wait on lock " << message,10)}
#undef RELLOCK
#define RELLOCK(m,message) \
		DEBOUT("Releasing lock " << message, m)\
		pthread_mutex_unlock(m);
#undef TRYLOCK
#define TRYLOCK(m,message,r)\
		r = pthread_mutex_trylock(m);
#endif
//**********************************************************
//**********************************************************

inline int getModulus(void* pointer) {

	//Addresses are all multiple of ADDRESSPACE
	//DEBOUT("MESSAGE pointer modulus",(long long unsigned int)pointer)
	int i = (int)((long long unsigned int)pointer % (MESSAGEMAPS*ADDRESSPACE));
	DEBOUT("MESSAGE pointer modulus",pointer<<"]["<<i/ADDRESSPACE)
	return i/ADDRESSPACE;

}
inline string b2S(bool _b){
    if (_b)
        return "true";
    else
        return "false";
}
