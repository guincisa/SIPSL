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

// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
//SPINBUFFER
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************

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


#ifndef SPINSTL
SPINC::SPINC(){
    s = 0;
    l = -1;
    DIM = 0;
    forcedState = false;

    pthread_mutex_init(&readmu, NULL);
    pthread_mutex_init(&writemu,NULL);
    pthread_mutex_init(&dimmu,NULL);
    
    for (int i = 0 ;i < SPINC_MOD; i++){
    	pthread_mutex_init(&buffmu[i],NULL);
    }

    pthread_mutex_init(&condvarmutex, NULL);
    pthread_cond_init(&condvar, NULL);

    for (int i = 0 ;i < ARR; i++){
    	BUFF[i] = (void*) MainMessage;
    }


}

void SPINC::lockBuffer(void){
    GETLOCK(&dimmu, "dimmu")
    forcedState = true;
    RELLOCK(&dimmu, "dimmu")
    return;
}

void SPINC::unLockBuffer(void){
    GETLOCK(&dimmu, "dimmu")
    forcedState = false;
    RELLOCK(&dimmu, "dimmu")
    return;
}

bool SPINC::put(void* _message){

    GETLOCK(&dimmu, "dimmu")
    if (forcedState == true){
            RELLOCK(&dimmu, "dimmu")
            return false;
    }
    RELLOCK(&dimmu, "dimmu")

    GETLOCK(&writemu, "writemu")
    int ts = s+1;
    ts = ts % ARR;
    int n = ts % SPINC_MOD;

    GETLOCK(&dimmu, "dimmu")
    int dim = DIM;
    RELLOCK(&dimmu, "dimmu")

    if ( ts == l && dim != 0){
        DEBASSERT("BUFFER FULL")
        RELLOCK(&writemu, "writemu")
        return false;
    }
    if(BUFF[ts] != (void*)MainMessage){
        DEBASSERT("BUFF[s] != MainMessage")
    }
    GETLOCK(&buffmu[n],"buffmu[" << n <<"]")
    BUFF[ts] = _message;
    s = ts;
    RELLOCK(&buffmu[n],"buffmu[" << n <<"]")
    if (l == -1)
            l = 1;

    GETLOCK(&dimmu, "dimmu")
    DIM++;
    RELLOCK(&dimmu, "dimmu")
    RELLOCK(&writemu, "writemu")
    return true;

}
void* SPINC::get(void){

    GETLOCK(&readmu,"readmu")
    int n = l % SPINC_MOD;
    GETLOCK(&buffmu[n],"buffmu[" << n <<"]")
    void* m = BUFF[l];
    if(BUFF[l] == (void*)MainMessage){
            DEBASSERT("BUFF[l] == MainMessage")
    }

    BUFF[l] = (void*)MainMessage;
    RELLOCK(&buffmu[n],"buffmu[" << n <<"]")
    l++;
    l = l % ARR;
    GETLOCK(&dimmu, "dimmu")
    DIM--;
    RELLOCK(&dimmu, "dimmu")
    //RELLOCK(&full,"full")
    RELLOCK(&readmu, "readmu")

    return m;

}

bool SPINC::isEmpty(void){

    GETLOCK(&dimmu, "dimmu")
    bool ise = (DIM == 0);
    RELLOCK(&dimmu, "dimmu")

    return ise;

}
SPINS::SPINS(){
    s = 0;
    l = -1;
    DIM = 0;
    forcedState = false;


    pthread_mutex_init(&readmu, NULL);
    pthread_mutex_init(&writemu,NULL);
    pthread_mutex_init(&dimmu,NULL);
    for (int i = 0 ;i < SPINC_MOD; i++){
    	pthread_mutex_init(&buffmu[i],NULL);
    }

    pthread_mutex_init(&condvarmutex, NULL);
    pthread_cond_init(&condvar, NULL);

    for (int i = 0 ;i < ARR_SHORT; i++){
    	BUFF[i] = (void*)MainMessage;
    }


}
void SPINS::lockBuffer(void){
    DEBOUT("SPIN locked", 0)
    GETLOCK(&dimmu, "dimmu")
    forcedState = true;
    RELLOCK(&dimmu, "dimmu")
    return;
}

void SPINS::unLockBuffer(void){
    DEBOUT("SPIN unlocked", 0)
    GETLOCK(&dimmu, "dimmu")
    forcedState = false;
    RELLOCK(&dimmu, "dimmu")
    return;
}


bool SPINS::put(void* _message){

    GETLOCK(&dimmu, "dimmu")
    if (forcedState == true){
            RELLOCK(&dimmu, "dimmu")
            return false;
    }
    RELLOCK(&dimmu, "dimmu")


    GETLOCK(&writemu, "writemu")
    int ts = s+1;
    ts = ts % ARR_SHORT;
    int n = ts % SPINC_MOD;

    GETLOCK(&dimmu, "dimmu")
    int dim = DIM;
    RELLOCK(&dimmu, "dimmu")

    if ( ts == l && dim != 0){
            RELLOCK(&writemu, "writemu")
            return false;
    }
    if(BUFF[ts] != (void*)MainMessage){
            DEBASSERT("BUFF[s] != MainMessage")
    }
    GETLOCK(&buffmu[n],"buffmu[" << n <<"]")
    BUFF[ts] = _message;
    s = ts;
    RELLOCK(&buffmu[n],"buffmu[" << n <<"]")
    if (l == -1)
            l = 1;

    GETLOCK(&dimmu, "dimmu")
    DIM++;
    RELLOCK(&dimmu, "dimmu")
    RELLOCK(&writemu, "writemu")
    return true;

}
void* SPINS::get(void){

    GETLOCK(&readmu,"readmu")
    int n = l % SPINC_MOD;
    GETLOCK(&buffmu[n],"buffmu[" << n <<"]")
    void* m = BUFF[l];
    if(BUFF[l] == (void*)MainMessage){
        DEBASSERT("BUFF[l] == MainMessage")
    }

    BUFF[l] = (void*)MainMessage;
    RELLOCK(&buffmu[n],"buffmu[" << n <<"]")
    l++;
    l = l % ARR_SHORT;
    GETLOCK(&dimmu, "dimmu")
    DIM--;
    RELLOCK(&dimmu, "dimmu")
    //RELLOCK(&full,"full")
    RELLOCK(&readmu, "readmu")

    return m;

}

bool SPINS::isEmpty(void){

    GETLOCK(&dimmu, "dimmu")
    bool ise = (DIM == 0);
    RELLOCK(&dimmu, "dimmu")

    return ise;

}
#else
SPINC::SPINC(void) {

	//pthread_mutex_init(&spinm, NULL);
    pthread_mutex_init(&condvarmutex, NULL);
    pthread_cond_init(&condvar, NULL);
}
bool SPINC::put(void* _message){
    //GETLOCK(&(spinm),"SPINC::spinm",26);
	Q.push(_message);
    //RELLOCK(&(spinm),"SPINC::spinm");
    return true;
}
void* SPINC::get(void){
	void* t;
    //GETLOCK(&(spinm),"SPINC::spinm",27);
	t = Q.front();
	Q.pop();
    //RELLOCK(&(spinm),"SPINC::spinm");
	return t;
}
bool SPINC::isEmpty(void){
	return Q.empty();
}
void SPINC::lockBuffer(void){
}
void SPINC::unLockBuffer(void){
}
SPINS::SPINS(void) {

	pthread_mutex_init(&spinm, NULL);
    pthread_mutex_init(&condvarmutex, NULL);
    pthread_cond_init(&condvar, NULL);
}
bool SPINS::put(void* _message){
    GETLOCK(&(spinm),"SPINC::spinm",28);
	Q.push(_message);
    RELLOCK(&(spinm),"SPINC::spinm");
    return true;
}
void* SPINS::get(void){
	void* t;
    GETLOCK(&(spinm),"SPINC::spinm",29);
	t = Q.front();
	Q.pop();
    RELLOCK(&(spinm),"SPINC::spinm");
	return t;
}
bool SPINS::isEmpty(void){
	return Q.empty();
}
void SPINS::lockBuffer(void){
}
void SPINS::unLockBuffer(void){
}
#endif
