#include <vector>
#include <string>
#include <map>
#include <assert.h>
#include <sys/types.h>
#include <stack>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <signal.h>

#ifndef UTIL_H
#include "UTIL.h"
#endif
#ifndef CS_HEADERS_H
#include "CS_HEADERS.h"
#endif
#ifndef MESSAGE_H
#include "MESSAGE.h"
#endif
#ifndef SPIN_H
#include "SPIN.h"
#endif
#ifndef ENGINE_H
#include "ENGINE.h"
#endif
#ifndef SIPENGINE_H
#include "SIPENGINE.h"
#endif
#ifndef SL_CC_H
#include "SL_CC.h"
#endif
#ifndef ACTION_H
#include "ACTION.h"
#endif
#ifndef DOA_H
#include "DOA.h"
#endif
#ifndef SUDP_H
#include "SUDP.h"
#endif
#ifndef CALL_OSET_H
#include "CALL_OSET.h"
#endif
#ifndef COMAP_H
#include "COMAP.h"
#endif
#ifndef ALO_H
#include "ALO.h"
#endif
#ifndef VALO_H
#include "VALO.h"
#endif
#ifndef SIP_PROPERTIES_H
#include "SIP_PROPERTIES.h"
#endif
#ifndef ALARM_H
#include "ALARM.h"
#endif
#ifndef SIPUTIL_H
#include "SIPUTIL.h"
#endif
#ifndef TRNSPRT_H
#include "TRNSPRT.h"
#endif
#ifndef DAO_H
#include "DAO.h"
#endif
#ifndef LOGGER_H
#include "LOGGER.h"
#endif


#ifdef USEMESSAGEMAP
map<const MESSAGE*, MESSAGE *> globalMessTable[MESSAGEMAPS];
pthread_mutex_t messTableMtx[MESSAGEMAPS];
#endif
#ifdef USEMAPMODUL
#ifndef MAPMODULHYBRID
map<const string, int> modulusMap;
int modulusIter = 0;
pthread_mutex_t modulusMapMtx;
#else
map<const string, int> modulusMap[PREMODMAP];
int modulusIter[PREMODMAP];
pthread_mutex_t modulusMapMtx[PREMODMAP];
#endif
#endif

MESSAGE* MainMessage;
COMAP* Comap;
LOGGER logger = LOGGER(1000,8);
//CALL_OSET* MainOset;

//Perf array
//[0] max
//[1] min
//[2] measures
//[3] average
double PERFARRAY[4][50];



void ex_program(int sig) {
	DEBOUT("SIG", sig);
	DEBASSERT("...")
// (void) signal(SIGINT, SIG_DFL);
}

class TESTMESS {
public:
	string message;
	int id;
};
//class TESTENGINE_1 : public ENGINE {
//
//    private:
//        int dummy;
//
//    public:
//
//        TESTENGINE_1(int);
//
//        void parse(void*); //downcall
//
//        pthread_mutex_t lockth[8];
//
//
//};

//TESTENGINE_1::TESTENGINE_1(int _i):ENGINE(_i){
//
//	pthread_mutex_lock(&lockth[0]);
//	pthread_mutex_lock(&lockth[1]);
//	pthread_mutex_lock(&lockth[2]);
//	pthread_mutex_lock(&lockth[3]);
//	pthread_mutex_lock(&lockth[4]);
//	pthread_mutex_lock(&lockth[5]);
//	pthread_mutex_lock(&lockth[6]);
//	pthread_mutex_lock(&lockth[7]);
//
//};

//void TESTENGINE_1::parse(void* __message, int _mmod){
//    RELLOCK(&(sb.condvarmutex),"sb.condvarmutex");
//    TIMEDEF
//    SETNOW
//
//    if (((TESTMESS*)__message)->id >= 0){
//		pthread_mutex_lock(&lockth[((TESTMESS*)__message)->id]);
//    }
//    else{
//
//    }
//    DEBOUT("TESTENGINE_1 parse Processed",((TESTMESS*)__message)->id)
//    PRINTDIFF("TESTENGINE_1::parse "<< ((TESTMESS*)__message)->id)
//};



int main(int argc, const char* argv[]) {

	 (void) signal(SIGSEGV, ex_program);
         (void) signal(SIGBUS, ex_program);


	BDEBUG("SIPSL main thread",pthread_self())

	if (argc == 1){

		//clear perfaray
		for (int i = 0 ; i < 50 ; i ++){
			PERFARRAY[0][i] = 0;
			PERFARRAY[1][i] = 999999999;
			PERFARRAY[2][i] = 0;
			PERFARRAY[3][i] = 0;
		}

		//This message is used for consitency checks
		//When a message is deleted, his pointer to set to MainMessage
		//any method call to the main message generates assert
		string empty="EMPTY";
		sockaddr_inX echoClntAddr;
		SysTime inTime;
		GETTIME(inTime);
		NEWPTR2(MainMessage, MESSAGE(empty.c_str(), SODE_NOPOINT, inTime, 0, echoClntAddr),"Main Message")
		MainMessage->setValid(1);

		char pippo[5];
		strcpy(pippo,"abcd");
		DEBOUT("pippo",pippo<<"]["<<&pippo<<"]["<<(&pippo)+1<<"]["<<(&pippo)+2)


//		NEWPTR2(MainOset, CALL_OSET((ENGINE*)0x0, (TRNSPRT*)0x0, "", 0),"Main CallOset")


		NEWPTR(SUDP*, mystack, SUDP(),"SUDP")
		//SUDP* mystack ;

		NEWPTR(TRNSPRT*, transport, TRNSPRT(TRNSPRTTH,TRNSPRTMAPS,"TRNSPRT"),"TRNSPRT")
		transport->linkSUDP(mystack);

		//Second stage engine: Call Control
		NEWPTR(SL_CC*, sl_cc, SL_CC(SL_CCTH,SL_CCMAPS,"SL_CC"),"SL_CC")
		//SL_CC sl_cc(SL_CCTH);
		sl_cc->linkTransport(transport);
		sl_cc->linkSUDP(mystack);

		//First stage engine: Lazy parser
		NEWPTR(SIPENGINE*, sipeng, SIPENGINE(SIPENGINETH,SIPENGINMAPS,"SIPENGINE"), "SIPENGINE")
//		SIPENGINE gg(SIPENGINETH);
		sipeng->setSL_CC(sl_cc);
		sipeng->linkSUDP(mystack);
		sipeng->linkTransport(transport);

		sl_cc->linkSipEngine(sipeng);

		//data layer
		NEWPTR(DAO*, daog, DAO(1,1,"DAO",sl_cc, sipeng, transport), "DAO")
		sipeng->setDAO(daog);
		sl_cc->setDAO(daog);


		NEWPTR(DOA*, doa, DOA(sl_cc, DOA_CLEANUP, 0),"DOA")
//		DOA doa(&sl_cc, DOA_CLEANUP, 0);
		doa->init();

		//Alarm setup
		//sec , nsec
		NEWPTR(ALMGR*, alarm, ALMGR(ALARMTH,ALARMMAPS,"ALMGR",sl_cc, 0, 10000000), "ALMGR")
//		ALMGR alarm(&sl_cc, 0, 10000000);
		alarm->initAlarm();



		mystack->init(5060, sipeng, doa, "grog.sipsl.org", alarm);
		mystack->start();

		pthread_mutex_t gu = PTHREAD_MUTEX_INITIALIZER;
		int res = pthread_mutex_lock(&gu);
		res = pthread_mutex_lock(&gu);
		return 0;

//    	//UDP perf begin
//
//		//Wait and send a message
//		timespec sleep_time;
//		sleep_time.tv_sec = 10;
//		sleep_time.tv_nsec = 0;
//		BDEBUG("start","start")
//
//		nanosleep(&sleep_time,NULL);
//
//		string message_char="INVITE sip:gugli_linphone@127.0.0.1:5060 SIP/2.0\
//		\nSIP/2.0 200 OK\
//		\nVia: SIP/2.0/UDP 127.0.0.1:5062;rport;branch=z9hG4bKmzisdwji\
//		\nVia: SIP/2.0/UDP 127.0.0.1:5062;rport;branch=secondo\
//		\nMax-Forwards: 70\
//		\nTo: sto <sip:gugli_linphone@172.21.160.181:5061>\
//		\nFrom: sfrom <sip:gugli_twinkle@guglicorp.com>;tag=zylee\
//		\nCall-ID: pyqilxurpvljjey@grog\
//		\nCSeq: 728 INVITE\
//		\nContact: <sip:gugli_twinkle@127.0.0.1:5062>\
//		\nContent-Type: application/sdp\
//		\nAllow: INVITE,ACK,BYE,CANCEL,OPTIONS,PRACK,REFER,NOTIFY,SUBSCRIBE,INFO,MESSAGE\
//		\nSupported: replaces,norefersub,100rel\
//		\nUser-Agent: Twinkle/1.4.2\
//		\nContent-Length: 301\
//		\n\
//		\nv=0\
//		\no=twinkle 259098868 421636418 IN IP4 127.0.0.1\
//		\ns=-\
//		\nc=IN IP4 127.0.0.1\
//		\nt=0 0\
//		\nm=audio 8000 RTP/AVP 98 97 8 0 3 101\
//		\na=rtpmap:98 speex/16000\
//		\na=rtpmap:97 speex/8000\
//		\na=rtpmap:8 PCMA/8000\
//		\na=rtpmap:0 PCMU/8000\
//		\na=rtpmap:3 GSM/8000\
//		\na=rtpmap:101 telephone-event/8000\
//		\na=fmtp:101 0-15\
//		\na=ptime:20\n";
//		BDEBUG("start","start 10")
//		MESSAGE* MYMESS;
//		NEWPTR2(MYMESS, MESSAGE(message_char.c_str(), SODE_NOPOINT, inTime, 0, echoClntAddr),"MY Message")
//		MYMESS->lock = true;
//		for(int kk = 0; kk < 1000;kk++)
//			mystack->sendRequest(MYMESS);
//    	//UDP perf end




	}
	else {
		//NANOSPEE TEST
		timespec sleep_time;

		sleep_time.tv_sec = 0;
		sleep_time.tv_nsec = 10000000;

		TIMEDEF
		SETNOW
		nanosleep(&sleep_time,NULL);
		PRINTDIFF("10000000")

		SETNOW
		sleep_time.tv_sec = 0;
		sleep_time.tv_nsec = 20000000;
		nanosleep(&sleep_time,NULL);
		PRINTDIFF("20000000")

		SETNOW
		sleep_time.tv_sec = 0;
		sleep_time.tv_nsec = 40000000;
		nanosleep(&sleep_time,NULL);
		PRINTDIFF("40000000")

		SETNOW
		sleep_time.tv_sec = 1;
		sleep_time.tv_nsec = 0;
		nanosleep(&sleep_time,NULL);
		PRINTDIFF("1 sec")

	}
//		//Engine test
//		cout << "Engine test" << endl;
//
//		NEWPTR(TESTENGINE_1*, testengine, TESTENGINE_1(8),"TESTENGINE_1")
//
//		TESTMESS* mess0 = new TESTMESS;
//		mess0->id = 0;
//		TESTMESS* mess1 = new TESTMESS;
//		mess1->id = 1;
//		TESTMESS* mess2 = new TESTMESS;
//		mess2->id = 2;
//
//		TESTMESS* mess3 = new TESTMESS;
//		mess3->id = 3;
//
//		TESTMESS* mess4= new TESTMESS;
//		mess4->id = 4;
//
//		TESTMESS* mess5= new TESTMESS;
//		mess5->id = 5;
//
//		TESTMESS* mess6= new TESTMESS;
//		mess6->id = 6;
//
//		TESTMESS* mess7= new TESTMESS;
//		mess7->id = 7;
//
//		TESTMESS* mess8= new TESTMESS;
//		mess8->id = -8;
//
//		TESTMESS* mess9= new TESTMESS;
//		mess9->id = -9;
//
//		TESTMESS* mess10= new TESTMESS;
//		mess10->id = -10;
//
//
//        TIMEDEF
//
//        SETNOW
//		testengine->p_w((void*)mess0);
//        PRINTDIFF("testengine->p_w((void*)mess0)")
//
//        SETNOW
//		testengine->p_w((void*)mess1);
//        PRINTDIFF("testengine->p_w((void*)mess1)")
//
//        SETNOW
//		testengine->p_w((void*)mess2);
//        PRINTDIFF("testengine->p_w((void*)mess2)")
//
//        SETNOW
//		testengine->p_w((void*)mess3);
//        PRINTDIFF("testengine->p_w((void*)mess3)")
//
//        SETNOW
//		testengine->p_w((void*)mess4);
//        PRINTDIFF("testengine->p_w((void*)mess4)")
//        SETNOW
//		testengine->p_w((void*)mess5);
//        PRINTDIFF("testengine->p_w((void*)mess5)")
//        SETNOW
//		testengine->p_w((void*)mess6);
//        PRINTDIFF("testengine->p_w((void*)mess6)")
//        SETNOW
//		testengine->p_w((void*)mess7);
//        PRINTDIFF("testengine->p_w((void*)mess7)")
//
//
//        SETNOW
//		testengine->p_w((void*)mess8);
//        PRINTDIFF("testengine->p_w((void*)mess8)")
//
//        SETNOW
//		testengine->p_w((void*)mess9);
//        PRINTDIFF("testengine->p_w((void*)mess9)")
//
//        SETNOW
//		testengine->p_w((void*)mess10);
//        PRINTDIFF("testengine->p_w((void*)mess10)")
//
//
//		//Test per vedere tempo medio di inserimento
////      SETNOW
////		for (int ii = 0; ii < 1000 ; ii ++){
////			testengine->p_w((void*)mess10);
////		}
////        PRINTDIFF("1000 inserts")
//
//
//        SETNOW
//			timespec sleep_time;
//			sleep_time.tv_sec = 120;
//			sleep_time.tv_nsec = 10000000;
//
//			nanosleep(&sleep_time,NULL);
//
//
//    	pthread_mutex_unlock(&testengine->lockth[0]);
//    	pthread_mutex_unlock(&testengine->lockth[1]);
//    	pthread_mutex_unlock(&testengine->lockth[2]);
//    	pthread_mutex_unlock(&testengine->lockth[3]);
//    	pthread_mutex_unlock(&testengine->lockth[4]);
//    	pthread_mutex_unlock(&testengine->lockth[5]);
//    	pthread_mutex_unlock(&testengine->lockth[6]);
//    	pthread_mutex_unlock(&testengine->lockth[7]);
//
//        PRINTDIFF("test")
//
//		pthread_mutex_t gu = PTHREAD_MUTEX_INITIALIZER;
//		int res = pthread_mutex_lock(&gu);
//		res = pthread_mutex_lock(&gu);
//		res = pthread_mutex_lock(&gu);
//
//
//		return 0;
//		cout << "test 1 Via" << endl;
//
//		string s = "SIP/2.0/UDP sipsl.gugli.com:5060;branch=z9hG4bKb0a1b1f81282750073027419;rport";
//
//		C_HeadVia c = C_HeadVia(s);
//
//		DEBOUT("Test",c.getC_AttVia().getViaParms().findRvalue("branch"));
//
////		cout << "test 2 Message Handle" << endl;
////
////		string empty="EMPTY";
////		sockaddr_inX echoClntAddr;
////		SysTime inTime;
////		GETTIME(inTime);
////		NEWPTR2(MainMessage, MESSAGE(empty.c_str(), SODE_NOPOINT, inTime, 0, echoClntAddr),"Main Message")
////		MainMessage->setValid(1);
////
////		//Nuovo messaggio h
////		MESSAGE* h;
////		CREATENEWMESSAGE_EXT(h, empty, 0, echoClntAddr, SODE_NTWPOINT)
////		cout << "Nuovo h " << h << endl;
////
////		//copio il puntatore al messaggio
////		MESSAGE* k = h;
////		cout << "Copio k " << k << endl;
////
////		//cancello h
////		PURGEMESSAGE(h)
////		cout << "Purgo h " << h << endl;
////		cout << "Invalido k " << k << endl;
//
//		//ma k ha adesso un indirizzo invalido
//
//		/////////////////////////////////////////////////////////////////
//		{
//		cout << "test 2 Message Handle" << endl;
//
//		string empty="EMPTY";
//		sockaddr_inX echoClntAddr;
//		SysTime inTime;
//		GETTIME(inTime);
//		NEWPTR2(MainMessage, MESSAGE(empty.c_str(), SODE_NOPOINT, inTime, 0, echoClntAddr),"Main Message")
//		MainMessage->setValid(1);
//
//		string empty2="EMPTY\nEMPTY";
//		MKHANDMESSAGE(y,_2_z)
//		CREATENEWMESSAGE_EXT(y, empty2, 0, echoClntAddr, SODE_NTWPOINT)
//		cout << "Handler _2_z " << _2_z << endl;
//
//		MESSAGEH _2_k = _2_z;
//		cout << "Handler _2_k " << _2_k << endl;
//
//		cout << "Access message using _2_z (getTotLines)" << _2_z->getTotLines() << endl;
//
//		//Since _k is also a reference to _z
//		//also _k will get modified by PURGEMESSAGE
//		PURGEMESSAGE(_2_z)
//		cout << "Purged Handler _2_z " << _2_z << endl;
//
//		//Adesso _k punta anche esso al MainMessage!!!
//		cout << "Handler is not purged but pints now to MainMessage_2_k " << _2_k << endl;
//		//Will assert here!!!
//		//cout << "Access message using _2_k (getTotLines)" << _2_k->getTotLines() << endl;
//
//		cout << "test 3 Message Handle" << endl;
//
//		MKHANDMESSAGE(m,_2_m)
//		m = 0x0;
//		cout << "_2_m " << _2_m << endl;
//		}
//		/////////////////////////////////////////////////////////////////
//		{
//		cout << "test 4 Message Handle" << endl;
//
//		string empty="EMPTY";
//		sockaddr_inX echoClntAddr;
//		SysTime inTime;
//		GETTIME(inTime);
//		NEWPTR2(MainMessage, MESSAGE(empty.c_str(), SODE_NOPOINT, inTime, 0, echoClntAddr),"Main Message")
//		MainMessage->setValid(1);
//
//		string empty2="EMPTY\nEMPTY";
//		MESSAGE* y;
//		MESSAGE** _2_z = &y;
//		CREATENEWMESSAGE_EXT(y, empty2, 0, echoClntAddr, SODE_NTWPOINT)
//		cout << "Handler _2_z " << *_2_z << endl;
//
//		MESSAGE** _2_k = _2_z;
//		cout << "Handler _2_k " << *_2_k << endl;
//
//		cout << "Access message using _2_z (getTotLines)" << (*_2_z)->getTotLines() << endl;
//
//		//Since _k is also a reference to _z
//		//also _k will get modified by PURGEMESSAGE
//		PURGEMESSAGE((*_2_z))
//		cout << "Purged Handler _2_z " << *_2_z << endl;
//
//		//Adesso _k punta anche esso al MainMessage!!!
//		cout << "Handler is not purged but pints now to MainMessage_2_k " << *_2_k << endl;
//		//Will assert here!!!
//		cout << "Access message using _2_k (getTotLines)" << (*_2_k)->getTotLines() << endl;
//
//		cout << "test 4 Message Handle" << endl;
//
//		}
//
//
//	}

}
