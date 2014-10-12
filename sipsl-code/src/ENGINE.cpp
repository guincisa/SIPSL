//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPCSL Sip Core And Service Layer
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

extern "C" void* threadparser (void*);
extern "C" void* threadparser_s (void*);

ThreadWrapper::ThreadWrapper(void) {
    //pthread_mutex_init(&mutex, NULL);
    return;
};

//**********************************************************************************
//**********************************************************************************
ENGINE::ENGINE(int _i, int _em, string _type) {

    DEBDEV("ENGINE::ENGINE()","")
    DEBDEV("ENGINE::ENGINE() spin buffer ",&sb)


    EngineMaps = _em;
    if ( EngineMaps > MAXMAPS)
    	EngineMaps = MAXMAPS;

    objectType = _type;

    int res;

    if ( _i > MAXTHREADS)
    	_i = MAXTHREADS;

    //Check if _i is divisible by ENGINEMAPS
    //if not substract the rest
    // if it gives 0 set to ENGINEMAPS
    int r = _i % EngineMaps;
    if ( r != 0){
    	_i = _i - r;
    	if (_i == 0){
    		_i = EngineMaps;
    	}
    }
    DEBOUT("ENGINE::ENGINE MAXTHREADS", MAXTHREADS <<"]["<<_type)
    ENGtuple *t[MAXTHREADS];

#ifdef TUNEPERF

    int mxp;
    int mxt;

    if (objectType.compare("DAO") == 0 || objectType.compare("ALMGR") == 0){
        mxp = _em;
        mxt = _i;
    }else{
        mxp = MAXMAPS;
        mxt = MAXTHREADS;

    }
#endif


    // if _i not divisible by ENGINEMAPS...
    int k = 0;
#ifdef TUNEPERF

    int rapp = mxt /  mxp;
	for ( int i = 0 ; i < mxp ; i++){
#else
	int rapp = _i / EngineMaps;
	for ( int i = 0 ; i < EngineMaps ; i++){
#endif

		sb[i] = new SPINC();

		for (int j = 0 ; j < rapp; j ++){

			NEWPTR2(t[k], ENGtuple, "ENGtuple"<<k)

			t[k]->ps = this;
			t[k]->id = 0;
			t[k]->mmod = i;
			t[k]->type = objectType.c_str();


			NEWPTR2(parsethread[k], ThreadWrapper(), "ThreadWrapper()"<<k)

			res = pthread_create(&(parsethread[k]->thread), NULL, threadparser, (void *) t[k]);


			k ++;

		}
	}

    sudp = 0x0;
}
//**********************************************************************************
//**********************************************************************************
void ENGINE::changeEngineMaps(int _m){
	EngineMaps = _m;
}
int ENGINE::modEngineMap(MESSAGE* _message){

	int mm = _message->getModulus() % EngineMaps;
	DEBDEV("ENGINE::modEngineMap", _message << "][" << _message->getModulus() <<"]["<<mm)
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
    DEBDEV("bool ENGINE::p_w(void* _m) ", _m << "] modulus SP["<<mmod)

#ifdef ENGINETRYLOCK
    int trylok;
    TRYLOCK(&unique_exx[mod]," purgeDOA unique_exx"<<mod, trylok)
    if(trylok != 0){
#else

    GETLOCK(&(sb[mmod]->condvarmutex),"[" << objectType << "] sb["<< mmod << "].condvarmutex",21);

#endif
    SETNOW

#ifdef MESSAGEUSAGE
    int i = getModulus((MESSAGE*)_m);
    GETLOCK(&messTableMtx[i],"&messTableMtx"<<i);
    ((MESSAGE*)_m)->inuse = 0;
    RELLOCK(&messTableMtx[i],"&messTableMtx"<<i);
#endif

    sb[mmod]->put(_m);
    pthread_cond_signal(&(sb[mmod]->condvar));

    RELLOCK(&(sb[mmod]->condvarmutex),objectType<<" sb["<< mod<<"].condvarmutex");
    PRINTDIFF("ENGINE"<<objectType<<"::p_w")

    return true;

}
//**********************************************************************************
//**********************************************************************************
void * threadparser (void * _pt){

    ENGtuple *pt = (ENGtuple *)  _pt;
    int mmod = pt->mmod;
    ENGINE * ps = pt->ps;
	BDEBUG("ENGINE THREAD CREATED ",pt->type <<"] id ["<<pthread_self())
    while(true) {
        DEBDEV("ENGINE thread",_pt)
            GETLOCK(&(ps->sb[mmod]->condvarmutex),"ps->sb["<<mmod<<"].condvarmutex",22);

        //if empty will entere here and wait signal
        while(ps->sb[mmod]->isEmpty() ) {
            DEBDEV("ENGINE thread is empty",_pt)
            //this be definition will unlock above lock while it waits
            pthread_cond_wait(&(ps->sb[mmod]->condvar), &(ps->sb[mmod]->condvarmutex));
            //now here there is a message in queue
            //get it and parse
        }
        //if not empty will be here
        //so get it and parse

        //e' gia cosi...

        DEBDEV("ENGINE thread freed", _pt)
        void* m = ps->sb[mmod]->get();
#ifdef USE_SPINB
        if (m == NULL)  {
            DEBDEV("ENGINE thread NULL",_pt)
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
    	DEBDEV("MESSAGE ALREADY RUNNING", m << "]["<<((MESSAGE*)m)->inuse)
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
//        DEBDEV("ENGINE thread",_pt)
//            GETLOCK(&(ps->rej.condvarmutex),"ps->rej.condvarmutex");
//        while(ps->rej.isEmpty() ) {
//            DEBDEV("ENGINE thread is empty",_pt)
//            pthread_cond_wait(&(ps->rej.condvar), &(ps->rej.condvarmutex));
//        }
//        DEBDEV("ENGINE thread freed", _pt)
//        void* m = ps->rej.get();
//#ifdef USE_SPINB
//        if (m == NULL)  {
//            DEBDEV("ENGINE thread NULL",_pt)
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
//**********************************************************************************
//**********************************************************************************
//bool ENGINE::p_w_s(void* _m) {
//
//    GETLOCK(&(rej.condvarmutex),"rej.condvarmutex");
//    bool r = rej.put(_m);
//    DEBDEV("ENGINE::p_w_s put returned",_m << " "<<r)
//    if (!r){
//    	DEBDEV("ENGINE::p_w_s put returned false", _m)
//    }else {
//        //Otherwise the message is parsed
//        pthread_cond_signal(&(rej.condvar));
//    }
//    RELLOCK(&(rej.condvarmutex),"rej.condvarmutex");
//    return r;
//
//}
//void ENGINE::lockBuffer(void){
//    DEBDEV("ENGINE::lockBuffer",this)
//    sb.lockBuffer();
//}
//void ENGINE::unLockBuffer(void){
//    DEBDEV("ENGINE::unLockBuffer",this)
//    sb.unLockBuffer();
//}
