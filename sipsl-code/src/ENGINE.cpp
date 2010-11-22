//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPCSL Sip Core And Service Layer
// Copyright (C) 2009 Guglielmo Incisa di Camerana
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

#include <vector>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <map>

#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stack>

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

extern "C" void* threadparser (void*);

ThreadWrapper::ThreadWrapper(void) {
    pthread_mutex_init(&mutex, NULL);
    return;
};

//**********************************************************************************
//**********************************************************************************
ENGINE::ENGINE(int _i, int _spintype):sb(_spintype) {

    DEBOUT("ENGINE::ENGINE()","")
    DEBOUT("ENGINE::ENGINE() spin buffer ",&sb)

    int res;

    if ( _i > MAXTHREADS)
    	_i = MAXTHREADS;

    ENGtuple *t[MAXTHREADS];

    int i;
    for ( i = 0 ; i < _i ; i++){

    	NEWPTR2(t[i], ENGtuple, "ENGtuple")

        t[i]->ps = this;
        t[i]->id = 0;

    	NEWPTR2(parsethread[i], ThreadWrapper(), "ThreadWrapper()")

        res = pthread_create(&(parsethread[i]->thread), NULL, threadparser, (void *) t[i]);


    }

    sudp = 0x0;
}
//**********************************************************************************
//**********************************************************************************
void ENGINE::parse(MESSAGE* m) {
    DEBERROR("ENGINE::parse illegal invocation")
#ifndef TESTING
    assert(0);
#endif
}
//**********************************************************************************
//**********************************************************************************
void ENGINE::linkSUDP(SUDP *_sudp){
	sudp = _sudp;
}
SUDP* ENGINE::getSUDP(void){
	return sudp;
}
//**********************************************************************************
//**********************************************************************************
void ENGINE::p_w(MESSAGE* _m) {

    GETLOCK(&(sb.condvarmutex),"sb.condvarmutex");
    DEBY
    sb.put(_m);
    pthread_cond_signal(&(sb.condvar));
    DEBY
    RELLOCK(&(sb.condvarmutex),"sb.condvarmutex");
    return;

}

//**********************************************************************************
//**********************************************************************************
void * threadparser (void * _pt){

    ENGtuple *pt = (ENGtuple *)  _pt;
    ENGINE * ps = pt->ps;
    while(true) {
        DEBOUT("ENGINE thread",_pt)
		GETLOCK(&(ps->sb.condvarmutex),"ps->sb.condvarmutex");
        while(ps->sb.isEmpty() ) {
            DEBOUT("ENGINE thread is empty",_pt)
            pthread_cond_wait(&(ps->sb.condvar), &(ps->sb.condvarmutex));
        }
        DEBOUT("ENGINE thread freed", _pt)
        MESSAGE* m = ps->sb.get();
#ifdef USE_SPINB
        if (m == NULL)  {
            DEBOUT("ENGINE thread NULL",_pt)
            ps->sb.move();
            //aggiunta il 30 luglio 2010
            RELLOCK(&(ps->sb.condvarmutex),"ps->sb.condvarmutex");
        }
        else {
#endif
            pt->ps->parse(m);
#ifdef USE_SPINB
        }
#endif
    }
    return (NULL);
}
