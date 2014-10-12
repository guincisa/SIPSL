//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Core And Service Layer
// Copyright (C) 2011 Guglielmo Incisa di Camerana
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
//#ifndef CALL_OSET_H
//#include "CALL_OSET.h"
//#endif

#include <algorithm>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <iostream>
#include <map>
#include <math.h>
#include <memory>
#include <pthread.h>
#include <signal.h>
#include <sstream>
#include <stack>
#include <stdio.h>
#include <stdlib.h>     /* for atoi() and exit() */
#include <string>
#include <string.h>
#include <sys/socket.h> /* for socket() and bind() */
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <vector>
#include <unordered_map>

#include "UTIL.h"

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

//**********************************************************************************
//**********************************************************************************
typedef struct tuple2 {
	DOA* st;
	int map;
} DOAtuple;
//**********************************************************************************
extern "C" void* DOASTACK (void*);
//**********************************************************************************
void * DOASTACK(void *_tgtObject) {

	BDEBUG("DOASTACK thread id",pthread_self())

    DEBDEV("DOASTACK start","")

		DOAtuple *tgtObject = (DOAtuple *)_tgtObject;

    tgtObject->st->doa(tgtObject->map);

    DEBDEV("DOASTACK started","")

    return (NULL);
}
//**********************************************************************************
DOA::DOA(SL_CC* _sl_cc, __time_t _sec, long int _nsec){
	sleep_time.tv_sec = _sec;
	sleep_time.tv_nsec = _nsec;
	sl_cc = _sl_cc;
	DEBDEV("DOA::DOA", "DOA created")
}

void DOA::init(void) {
	DEBDEV("DOA::init", "init")

    int res;
    for (int i = 0 ; i < DOATH; i ++){
    	DOAtuple *t1;
        NEWPTR2(t1,DOAtuple,"DOAtuple");
        t1->st = this;
        NEWPTR2(listenerThread[i],ThreadWrapper,"ThreadWrapper");
    	t1->map = i;
    	res = pthread_create(&(listenerThread[i]->thread), NULL, DOASTACK, (void *) t1 );
    }

    //TODO check consistency!!!
    //pthread_mutex_init(&mutex, NULL);

	DEBDEV("DOA::init", "started")
    return;

}void DOA::doa(int _map) {

	for(;;) {
		nanosleep(&sleep_time,NULL);
		DEBDEV("DOA::doa", "started")
		//DUMPMESSTABLE
		sl_cc->getCOMAP()->purgeDOA(_map);
	}

}
