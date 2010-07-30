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


#ifndef SPIN_H
#include "SPIN.h"
#endif
#ifndef CS_HEADERS_H
#include "CS_HEADERS.h"
#endif
#ifndef ENGINE_H
#include "ENGINE.h"
#endif
#ifndef SUDP_H
#include "SUDP.h"
#endif


extern "C" void* threadparser (void*);

ThreadWrapper::ThreadWrapper(void) {
    pthread_mutex_init(&mutex, NULL);
    return;
};

//**********************************************************************************
//**********************************************************************************
ENGINE::ENGINE(void) {

    DEBOUT("ENGINE::ENGINE()","")
    int res;
    // unlock the CONSmutex once the derived class constructor has ended
    //res = pthread_mutex_lock(&CONSmutex);

    ENGtuple *t1,*t2,*t3,*t4,*t5;
    t1 = new ENGtuple;
    t2 = new ENGtuple;
    t3 = new ENGtuple;
    t4 = new ENGtuple;
    t5 = new ENGtuple;
    t1->ps = this;
    t1->id = 0;
    t2->ps = this;
    t2->id = 1;
    t3->ps = this;
    t3->id = 2;
    t4->ps = this;
    t4->id = 3;
    t5->ps = this;
    t5->id = 4;

    parsethread[0] = new ThreadWrapper();
    parsethread[1] = new ThreadWrapper();
    parsethread[2] = new ThreadWrapper();
    parsethread[3] = new ThreadWrapper();
    parsethread[4] = new ThreadWrapper();

    res = pthread_create(&(parsethread[0]->thread), NULL, threadparser, (void *) t1);
    res = pthread_create(&(parsethread[1]->thread), NULL, threadparser, (void *) t2);
    res = pthread_create(&(parsethread[2]->thread), NULL, threadparser, (void *) t3);
    res = pthread_create(&(parsethread[3]->thread), NULL, threadparser, (void *) t4);
    res = pthread_create(&(parsethread[4]->thread), NULL, threadparser, (void *) t5);

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

    pthread_mutex_lock(&(sb.condvarmutex));
    sb.put(_m);
    pthread_cond_signal(&(sb.condvar));
    pthread_mutex_unlock(&(sb.condvarmutex));
    return;

}

//**********************************************************************************
//**********************************************************************************
void * threadparser (void * _pt){

    ENGtuple *pt = (ENGtuple *)  _pt;
    ENGINE * ps = pt->ps;
    while(true) {
        DEBOUT("ENGINE thread",_pt)
        pthread_mutex_lock(&(ps->sb.condvarmutex));
        while(ps->sb.isEmpty() ) {
            DEBOUT("ENGINE thread is empty",_pt)
            pthread_cond_wait(&(ps->sb.condvar), &(ps->sb.condvarmutex));
        }
        DEBOUT("ENGINE thread freed", _pt)
        MESSAGE* m = ps->sb.get();
        if (m == NULL)  {
            DEBOUT("ENGINE thread NULL",_pt)
            ps->sb.move();
            //aggiunta il 30 luglio 2010
            pthread_mutex_unlock(&(ps->sb.condvarmutex));
        }
        else {
            pt->ps->parse(m);
        }
        //TODO ??? 30 luglio 2010
        //pthread_mutex_unlock(&(ps->sb.condvarmutex));
    }
    return (NULL);
}
