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
#ifndef SUDP_H
#include "SUDP.h"
#endif
#ifndef CS_HEADERS_H
#include "CS_HEADERS.h"
#endif
#ifndef MESSAGE_H
#include "MESSAGE.h"
#endif
#ifndef ALARM_H
#include "ALARM.h"
#endif
#ifndef SIP_PROPERTIES_H
#include "SIP_PROPERTIES.h"
#endif
#ifndef DOA_H
#include "DOA.h"
#endif

int main(void) {

	SUDP mystack;

	//Second stage engine: Call Control
	SL_CC sl_cc(7);
	sl_cc.linkSUDP(&mystack);

	//First stage engine: Lazy parser
	SIPENGINE gg(2);
	gg.setSL_CC(&sl_cc);
	gg.linkSUDP(&mystack);

	DOA doa(2);

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
