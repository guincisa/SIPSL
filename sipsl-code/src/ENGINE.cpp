//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPCSL Sip Core And Service Layer 
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
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */

#include "COMPAR.h"
#include "COMM.h"
#include "ENGINE.h"

extern "C" void* threadparser (void*);

//**********************************************************************************
//**********************************************************************************
ENGINE::ENGINE() {
    DEBOUT("ENGINE::ENGINE()","")
    int res;
    // unlock the CONSmutex once the derived class contructor has ended
    //res = pthread_mutex_lock(&CONSmutex);
    //

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
    //pthread_mutex_t gu = PTHREAD_MUTEX_INITIALIZER;
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
}
//**********************************************************************************
//**********************************************************************************
void ENGINE::parse(MESSAGE m) {
    DEBERROR("ENGINE::parse illegal invocation")
    //cout << "RATYPE::parseMessage " << message[i].incomingMessage << endl;
    // invoke AC?
}
//**********************************************************************************
//**********************************************************************************
void ENGINE::p_w(MESSAGE m) {

    pthread_mutex_lock(&(sb.condvarmutex));
    sb.put(m);
    pthread_cond_signal(&(sb.condvar));
    pthread_mutex_unlock(&(sb.condvarmutex));
    return;
}

//**********************************************************************************
//**********************************************************************************
void * threadparser (void * _pt){

    ENGtuple *pt = (ENGtuple *)  _pt;
    int id = pt->id;
    ENGINE * ps = pt->ps;
    int res;
    MESSAGE m;
    while(true) {
        pthread_mutex_lock(&(ps->sb.condvarmutex));
        while(ps->sb.isEmpty() ) {
            DEBOUT("ENGINE thread isempty","")
            pthread_cond_wait(&(ps->sb.condvar), &(ps->sb.condvarmutex));
        }
        m = ps->sb.get();
        if (m.id == -99999)  {
            DEBOUT("ENGINE thread 999999","")
            ps->sb.move();
        }
        else {
            pt->ps->parse(m);
        }
        pthread_mutex_unlock(&(ps->sb.condvarmutex));
    }
    return (NULL);
}

//**********************************************************************************
//**********************************************************************************
//void ENGINE::associateCB(ENGCALLBACK *_engcb) {
//    engcb = _engcb;
//}

//**********************************************************************************
//**********************************************************************************
//ENGCALLBACK::ENGCALLBACK() {
//}

//**********************************************************************************
//**********************************************************************************
//ENGCALLBACK::ENGCALLBACK(ENGINE * eng) {
//        rata = eng;
//}

//**********************************************************************************
//**********************************************************************************
//void ENGCALLBACK::parsecallback(MESSAGE m, ENGINE * ra1, ENGINE * ra2) {
//}
