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

map<const MESSAGE*, MESSAGE *> globalMessTable;
pthread_mutex_t messTableMtx;
MESSAGE* MainMessage;

int main(int argc, const char* argv[]) {

	if (argc == 1){

		//This message is used for consitency checks
		//When a message is deleted, his pointer to set to MainMessage
		//any method call to the main message generates assert
		string empty="EMPTY";
		sockaddr_inX echoClntAddr;
		SysTime inTime;\
		GETTIME(inTime);\
		NEWPTR2(MainMessage, MESSAGE(empty.c_str(), SODE_NOPOINT, inTime, 0, echoClntAddr),"Main Message")
		MainMessage->setValid(1);

		SUDP mystack;

		TRNSPRT transport;

		//Second stage engine: Call Control
		SL_CC sl_cc(SL_CCTH);
		sl_cc.linkTransport(&transport);
		sl_cc.linkSUDP(&mystack);

		//First stage engine: Lazy parser
		SIPENGINE gg(SIPENGINETH);
		gg.setSL_CC(&sl_cc);
		gg.linkSUDP(&mystack);

		DOA doa(&sl_cc, DOA_CLEANUP, 0);
		doa.init();

		//Alarm setup
		//sec , nsec
		ALMGR alarm(&sl_cc, 0, 10000000);
		alarm.initAlarm();



		mystack.init(5060, &gg, &doa, "sipsl.gugli.com", &alarm);
		mystack.start();

		pthread_mutex_t gu = PTHREAD_MUTEX_INITIALIZER;
		int res = pthread_mutex_lock(&gu);
		res = pthread_mutex_lock(&gu);
		res = pthread_mutex_lock(&gu);

		return 0;
	}
	else {
		cout << "test" << endl;

		string s = "SIP/2.0/UDP sipsl.gugli.com:5060;branch=z9hG4bKb0a1b1f81282750073027419;rport";

		C_HeadVia c = C_HeadVia(s);

		DEBOUT("Test",c.getC_AttVia().getViaParms().findRvalue("branch"));

	}

}
