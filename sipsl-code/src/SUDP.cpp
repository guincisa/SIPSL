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
#ifndef SUDP_H
#include "SUDP.h"
#endif
#ifndef CS_HEADERS_H
#include "CS_HEADERS.h"
#endif
#ifndef MESSAGE_H
#include "MESSAGE.h"
#endif

//// *****************************************************************************************
//// ************************************
//ParseEx::ParseEx(string s) {
//    error = s;
//}
// *****************************************************************************************


// *****************************************************************************************
// Socket listener thread
// *****************************************************************************************
// *****************************************************************************************
extern "C" void* SUPDSTACK (void*);

void * SUDPSTACK(void *_tgtObject) {
    int res;

DEBOUT("SUDPSTACK start","")

	SUDPtuple *tgtObject = (SUDPtuple *)_tgtObject;

    tgtObject->st->listen();

DEBOUT("SUDPSTACK started","")

    return (NULL);
}

// *****************************************************************************************
// SUDP
// Initialize Stack
// *****************************************************************************************
//// *****************************************************************************************
//SUDP::SUDP(void) {
////	instance == NULL;
//}

//SUDP * SUDP::getInstance(void){
//
//	if (instance == NULL){
//		instance = new SUDP;
//	}
//	return instance;
//}

void SUDP::init(int _port, ENGINE *_engine, string _domain){

    DEBOUT("SUDP init",_domain)

    domain = _domain;

    engine = _engine;

    echoServPort = _port;

    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        DEBERROR("socket() failed)")
        return;
    }

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    //Init other things
    pthread_mutex_init(&messTableMtx,NULL);

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0) {
        DEBERROR("bind() failed)");
        return;
    }

    _engine->linkSUDP(this);

    DEBOUT("SUDP init done","")

    return;
}

// *****************************************************************************************
// Start Stack
// *****************************************************************************************
// *****************************************************************************************
void SUDP::start(void) {
    // allocate thread and starts

    DEBOUT("SUDP::start","")

    listenerThread = new ThreadWrapper;
    SUDPtuple *t1;
    t1 = new SUDPtuple;
    t1->st = this;
    // TODO ???
    int res;
    res = pthread_create(&(listenerThread->thread), NULL, SUDPSTACK, (void *) t1 );
    return;
}

// *****************************************************************************************
// Listen to network
// *****************************************************************************************
void SUDP::listen() {

	char bu[512];

    for (;;){
        /* Set the size of the in-out parameter */
        cliAddrLen = sizeof(echoClntAddr);

        /* Block until receive message from a client */
        memset(&echoBuffer, 0x0, ECHOMAX);   /* Zero out structure */

        if ((recvMsgSize = recvfrom(sock, echoBuffer, ECHOMAX, 0,
            (struct sockaddr *) &echoClntAddr, (socklen_t*)&cliAddrLen)) < 0) {
            DEBERROR("SUDP::listen() rcvfrom() failed")
            return;
        }

        //Message handling
		SysTime inTime;
		GETTIME(inTime);
		MESSAGE* message;
		message = new MESSAGE(echoBuffer, SODE_APOINT, inTime, sock, echoClntAddr);
		DEBOUT("Incoming\n****************************************************\n",message->getIncBuffer())

		sprintf(bu, "%x#%lld",message,inTime.tv.tv_sec*1000000+inTime.tv.tv_usec);
		string key(bu);
		message->setKey(key);
		DEBOUT("",bu)
		pthread_mutex_lock(&messTableMtx);
		globalMessTable.insert(pair<string, MESSAGE*>(key, message));
		pthread_mutex_unlock(&messTableMtx);



		//DECTIME
		//STARTTIME
		engine->p_w(message);
		//ENDTIME

	}
}
//void SUDP::listen() {
//    for (;;) /* Run forever */ {
//        /* Set the size of the in-out parameter */
//        cliAddrLen = sizeof(echoClntAddr);
//
//        /* Block until receive message from a client */
//        memset(&echoBuffer, 0x0, ECHOMAX);   /* Zero out structure */
//        if ((recvMsgSize = recvfrom(sock, echoBuffer, ECHOMAX, 0,
//            (struct sockaddr *) &echoClntAddr, (socklen_t*)&cliAddrLen)) < 0) {
//            DEBERROR("rcvfrom() failed")
//            return;
//        }
//		SysTime mytime;
//		GETTIME(mytime);
//		MESSAGE im(echoBuffer, mytime);
//		//im.etGenEntity = SODE_APOINT;
//
//		//GETTIME(im.in_ts)
//
//		DEBOUT("Incoming",im.getIncBuffer())
//
//		//DECTIME
//		//STARTTIME
//		engine->p_w(im);
//		//ENDTIME
//
//	}
//}
// *****************************************************************************************
// getDomain
// *****************************************************************************************
// *****************************************************************************************
string SUDP::getDomain(void) {
    return domain;
}
