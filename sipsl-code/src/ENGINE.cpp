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
extern "C" void* threadparser_s (void*);

ThreadWrapper::ThreadWrapper(void) {
    pthread_mutex_init(&mutex, NULL);
    return;
};

//**********************************************************************************
//**********************************************************************************
ENGINE::ENGINE(int _i) {

    DEBOUT("ENGINE::ENGINE()","")
    DEBOUT("ENGINE::ENGINE() spin buffer ",&sb)

    int res;

    if ( _i > MAXTHREADS)
    	_i = MAXTHREADS;

    //Check if _i is divisible by ENGINEMAPS
    //if not substract the rest
    // if it gives 0 set to ENGINEMAPS
    int r = _i % ENGINEMAPS;
    if ( r != 0){
    	_i = _i - r;
    	if (_i == 0){
    		_i = ENGINEMAPS;
    	}
    }

    ENGtuple *t[MAXTHREADS];

//    int i;
//    for ( i = 0 ; i < _i ; i++){
//
//    	NEWPTR2(t[i], ENGtuple, "ENGtuple")
//
//        t[i]->ps = this;
//        t[i]->id = 0;
//
//    	NEWPTR2(parsethread[i], ThreadWrapper(), "ThreadWrapper()")
//
//        res = pthread_create(&(parsethread[i]->thread), NULL, threadparser, (void *) t[i]);
//
//
//    }

    //TODO terrible!!!
    // if _i not divisible by ENGINEMAPS
    int k = 0;
    int rapp = _i / ENGINEMAPS;
	for ( int i = 0 ; i < ENGINEMAPS ; i++){
		for (int j = 0 ; j < rapp; j ++){

			NEWPTR2(t[k], ENGtuple, "ENGtuple"<<k)

			t[k]->ps = this;
			t[k]->id = 0;
			t[k]->mmod = i;


			NEWPTR2(parsethread[k], ThreadWrapper(), "ThreadWrapper()"<<k)

			res = pthread_create(&(parsethread[k]->thread), NULL, threadparser, (void *) t[k]);

			k ++;

		}
	}

    sudp = 0x0;
}
//**********************************************************************************
//**********************************************************************************

int ENGINE::modEngineMap(MESSAGE* _message){

	int mm = _message->getModulus() % ENGINEMAPS;
	DEBOUT("ENGINE::modEngineMap", _message << "][" << _message->getModulus() <<"]["<<mm)
	return mm;
}

//**********************************************************************************
//**********************************************************************************
void ENGINE::parse(void* m, int mm) {
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
bool ENGINE::p_w(void* _m) {

    TIMEDEF

	int mmod = modEngineMap((MESSAGE*)_m);
    DEBOUT("bool ENGINE::p_w(void* _m) ", _m << "] modulus SP["<<mmod)

    GETLOCK(&(sb[mmod].condvarmutex),"[" << this << "] sb["<< mmod << "].condvarmutex");

    SETNOW

#ifdef MESSAGEUSAGE
    int i = getModulus((MESSAGE*)_m);
    GETLOCK(&messTableMtx[i],"&messTableMtx"<<i);
    ((MESSAGE*)_m)->inuse = 0;
    RELLOCK(&messTableMtx[i],"&messTableMtx"<<i);
#endif

    sb[mmod].put(_m);
    pthread_cond_signal(&(sb[mmod].condvar));
//    bool r = sb.put(_m);
//    DEBOUT("ENGINE::p_w put returned",_m << " "<<r)
//    if (!r){
//    	DEBOUT("ENGINE::p_w put returned false","")
//    }else {
//        //Otherwise the message is parsed
//        pthread_cond_signal(&(sb.condvar));
//    }
    PRINTDIFF("ENGINE::p_w")
    RELLOCK(&(sb[mmod].condvarmutex),"sb["<< mod<<"].condvarmutex");
    return true;


}
//**********************************************************************************
//**********************************************************************************
//bool ENGINE::p_w_s(void* _m) {
//
//    GETLOCK(&(rej.condvarmutex),"rej.condvarmutex");
//    bool r = rej.put(_m);
//    DEBOUT("ENGINE::p_w_s put returned",_m << " "<<r)
//    if (!r){
//    	DEBOUT("ENGINE::p_w_s put returned false", _m)
//    }else {
//        //Otherwise the message is parsed
//        pthread_cond_signal(&(rej.condvar));
//    }
//    RELLOCK(&(rej.condvarmutex),"rej.condvarmutex");
//    return r;
//
//}
//void ENGINE::lockBuffer(void){
//    DEBOUT("ENGINE::lockBuffer",this)
//    sb.lockBuffer();
//}
//void ENGINE::unLockBuffer(void){
//    DEBOUT("ENGINE::unLockBuffer",this)
//    sb.unLockBuffer();
//}

//**********************************************************************************
//**********************************************************************************
void * threadparser (void * _pt){

    ENGtuple *pt = (ENGtuple *)  _pt;
    int mmod = ((ENGtuple *)  _pt)->mmod;
    ENGINE * ps = pt->ps;
    while(true) {
        DEBOUT("ENGINE thread",_pt)
            GETLOCK(&(ps->sb[mmod].condvarmutex),"ps->sb["<<mmod<<"].condvarmutex");
        while(ps->sb[mmod].isEmpty() ) {
            DEBOUT("ENGINE thread is empty",_pt)
            pthread_cond_wait(&(ps->sb[mmod].condvar), &(ps->sb[mmod].condvarmutex));
        }
        DEBOUT("ENGINE thread freed", _pt)
        void* m = ps->sb[mmod].get();
#ifdef USE_SPINB
        if (m == NULL)  {
            DEBOUT("ENGINE thread NULL",_pt)
            ps->sb.move();
            //aggiunta il 30 luglio 2010
            RELLOCK(&(ps->sb.condvarmutex),"ps->sb.condvarmutex");
        }
        else {
#endif


#ifdef MESSAGEUSAGE
    int i = getModulus((MESSAGE*)m);
    GETLOCK(&messTableMtx[i],"&messTableMtx"<<i);
    if ( ((MESSAGE*)m)->inuse != 0){
    	DEBOUT("MESSAGE ALREADY RUNNING", m << "]["<<((MESSAGE*)m)->inuse)
    	DEBASSERT("MESSAGE ALREADY RUNNING")
    }
    ((MESSAGE*)m)->inuse = (int) pthread_self();
    RELLOCK(&messTableMtx[i],"&messTableMtx"<<i);
#endif

            pt->ps->parse(m, mmod);

#ifdef USE_SPINB
        }
#endif
    }
    return (NULL);
}
//**********************************************************************************
//**********************************************************************************
//void * threadparser_s (void * _pt){
//
//    ENGtuple *pt = (ENGtuple *)  _pt;
//    ENGINE * ps = pt->ps;
//    while(true) {
//        DEBOUT("ENGINE thread",_pt)
//            GETLOCK(&(ps->rej.condvarmutex),"ps->rej.condvarmutex");
//        while(ps->rej.isEmpty() ) {
//            DEBOUT("ENGINE thread is empty",_pt)
//            pthread_cond_wait(&(ps->rej.condvar), &(ps->rej.condvarmutex));
//        }
//        DEBOUT("ENGINE thread freed", _pt)
//        void* m = ps->rej.get();
//#ifdef USE_SPINB
//        if (m == NULL)  {
//            DEBOUT("ENGINE thread NULL",_pt)
//            ps->sb.move();
//            //aggiunta il 30 luglio 2010
//            RELLOCK(&(ps->sb.condvarmutex),"ps->rej.condvarmutex");
//        }
//        else {
//#endif
//            pt->ps->parse_s(m);
//#ifdef USE_SPINB
//        }
//#endif
//    }
//    return (NULL);
//}
