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
#include <sstream>
#include <sys/time.h>

using namespace std;

typedef struct {
	struct timeval tv;
	struct timezone tz;
} SysTime;

//#define TESTING

#define ECHOMAX 2048

#define GETTIME(mytime) gettimeofday(&mytime.tv, &mytime.tz);

//#define DECTIME char bu[512];hrtime_t inittime;hrtime_t endtime;
#define STARTTIME {GETTIME(inittime)}
#define ENDTIME {char bu[512];GETTIME(endtime); sprintf(bu, "[%s %d] init %lld end %lld tot %lld\n",__FILE__, __LINE__, inittime.tv.tv_sec*1000000+inittime.tv.tv_usec,endtime.tv.tv_sec*1000000+endtime.tv.tv_usec ,endtime.tv.tv_sec*1000000+endtime.tv.tv_usec  - inittime.tv.tv_sec*1000000+inittime.tv.tv_usec ); cout << bu << flush;}

//#define DEBOUT(m1,m2)  cout << "DEBOUT " << pthread_self() << __FILE__ <<" " <<__LINE__<< " "<< m1 << "[" << m2 << "]"<< endl;
#define DEBOUT(m1,m2)  {stringstream xx ; xx << "DEBOUT " <<  __FILE__ <<" " <<__LINE__<< " "<< m1 << "[" << m2 << "]\n"; cout << xx.str();cout.flush();}
#define DEBERROR(m1)  {stringstream xx ; xx << "**** RUNTIME ERROR **** " << __FILE__ <<" " <<__LINE__<< "[" << m1 << "]\n";cout << xx.str();cout.flush();}
#define DEBY  {stringstream xx ; xx << "DEBY " << __FILE__ <<" " <<__LINE__<< "\n";cout << xx.str();cout.flush();}
#define DEBASSERT(m1) cout << "DEBASSERT " << __FILE__ << " " << __LINE__<< " " << m1 << endl; assert(0);

#define PURGEMESSAGE(m1,m2)  {string key = m1->getKey();\
	pthread_mutex_lock(&messTableMtx);\
	DEBOUT(m2,key << " [" <<m1<<"]")\
	globalMessTable.erase(key);\
	delete m1;\
	pthread_mutex_unlock(&messTableMtx);}

#define ATRANSMIT(message) \
	DEBOUT("SL_CO::call action is send APOINT string:", message->getIncBuffer()) \
	sendto(message->getSock(), \
			message->getIncBuffer().c_str(), \
			message->getIncBuffer().length() , 0, (struct sockaddr *) &(message->getAddress()), \
			sizeof(message->getAddress()));

#define BTRANSMIT(message) { \
		DEBOUT("SL_CO::call action is send APOINT string:", message->getIncBuffer()) \
		struct sockaddr_in si_bpart; \
		memset((char *) &si_bpart, 0, sizeof(si_bpart)); \
		si_bpart.sin_family = AF_INET; \
		si_bpart.sin_port = htons(actionList.top().getMessage()->getHeadTo().getC_AttSipUri().getS_AttHostPort().getPort()); \
		DEBOUT("SL_CO::call action is send BPOINT string:", message->getIncBuffer()) \
		sendto(actionList.top().getMessage()->getSock(), \
							actionList.top().getMessage()->getIncBuffer().c_str(), \
							actionList.top().getMessage()->getIncBuffer().length() , 0, (struct sockaddr *)  &si_bpart, \
							sizeof(si_bpart));}

#define CREATEMESSAGE(m1, m2, m3) char bu[512];\
				SysTime inTime;\
				GETTIME(inTime);\
				MESSAGE* m1 = new MESSAGE(m2, m3, inTime);\
				DEBOUT("NEW MESSAGE"," " + m1->getTotLines());\
				long long int num = ((long long int) inTime.tv.tv_sec)*1000000+(long long int)inTime.tv.tv_usec;\
				sprintf(bu, "%x#%llu",m1,num);\
				string key(bu);\
				m1->setKey(key);\
				pthread_mutex_lock(&messTableMtx);\
				globalMessTable.insert(pair<string, MESSAGE*>(m1->getKey(), m1));\
				pthread_mutex_unlock(&messTableMtx);

#define CREATENEWMESSAGE(__mess, __echob, __sock, __echoAddr, __sode) char bu[512];\
				SysTime inTime;\
				GETTIME(inTime);\
				MESSAGE* __mess = new MESSAGE(__echob, __sode, inTime, __sock, __echoAddr);\
				DEBOUT("NEW MESSAGE"," " + __mess->getTotLines());\
				long long int num = ((long long int) inTime.tv.tv_sec)*1000000+(long long int)inTime.tv.tv_usec;\
				sprintf(bu, "%x#%llu",(unsigned int)__mess,num);\
				string key(bu);\
				__mess->setKey(key);\
				DEBOUT("Incoming\n****************************************************\n",__mess->getIncBuffer() << "]\nkey [" << key)\
				pthread_mutex_lock(&messTableMtx);\
				globalMessTable.insert(pair<string, MESSAGE*>(__mess->getKey(), __mess));\
				pthread_mutex_unlock(&messTableMtx);


#define DUPLICATEMESSAGE(m1, m2, m3) \
		CREATEMESSAGE(m1, m2, m3)\
		m1->setSourceMessage(m2->getSourceMessage());

class ThreadWrapper {
    public:
        pthread_t thread;
        pthread_mutex_t mutex;
        ThreadWrapper();
};

//TODO
//#define STOREMESSAGE ()
//sprintf(bu, "%x#%lld",message,inTime.tv.tv_sec*1000000+inTime.tv.tv_usec);
//string key(bu);
//message->setKey(key);
//pthread_mutex_lock(&messTableMtx);
//globalMessTable.insert(pair<string, MESSAGE*>(key, message));
//pthread_mutex_unlock(&messTableMtx);


