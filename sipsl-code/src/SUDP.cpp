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
#include <string.h>
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
#ifndef ALARM_H
#include "ALARM.h"
#endif

// *****************************************************************************************
// Socket listener thread
// *****************************************************************************************
// *****************************************************************************************
extern "C" void* SUPDSTACK (void*);

void * SUDPSTACK(void *_tgtObject) {

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
// *****************************************************************************************
void SUDP::init(int _port, ENGINE *_engine, string _domain, ALMGR* _alarm){

    DEBOUT("SUDP init",_domain)

    domain = _domain;

    engine = _engine;

    echoServPort = _port;

    alarm = _alarm;

    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        DEBASSERT("socket() failed)")
        return;
    }

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    //Init mutex
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

    //TODO result not used
    int res = pthread_create(&(listenerThread->thread), NULL, SUDPSTACK, (void *) t1 );
    return;
}

// *****************************************************************************************
// Listen to network
// *****************************************************************************************
void SUDP::listen() {

    for (;;){
        /* Set the size of the in-out parameter */
        cliAddrLen = sizeof(echoClntAddr);

        /* Block until receive message from a client */
        memset(&echoBuffer, 0x0, ECHOMAX);   /* Zero out structure */

        if ((recvMsgSize = recvfrom(sock, echoBuffer, ECHOMAX, 0,
            (struct sockaddr *) &echoClntAddr, (socklen_t*)&cliAddrLen)) < 0) {
            DEBERROR("SUDP::listen() recvfrom() failed")
            return;
        }

        //Message handling
        CREATENEWMESSAGE(message, echoBuffer, sock, echoClntAddr, SODE_NTWPOINT)

		//DECTIME
		//STARTTIME
		engine->p_w(message);
		//ENDTIME

	}
}
// *****************************************************************************************
// getters
// *****************************************************************************************
// *****************************************************************************************
string SUDP::getDomain(void) {
    return domain;
}
int SUDP::getPort(void){
	return echoServPort;
}
ALMGR* SUDP::getAlmgr(void){
	return alarm;
}
void SUDP::sendRequest(MESSAGE* _message){

	struct sockaddr_in si_part;
	memset((char *) &si_part, 0, sizeof(si_bpart));
	si_bpart.sin_family = AF_INET;
	si_bpart.sin_port = htons(_message->getHeadSipRequest().getC_AttSipUri().getChangeS_AttHostPort().getPort());
	if( inet_aton(_message->getHeadSipRequest().getC_AttSipUri().getChangeS_AttHostPort().getHostName(), si_part.sin_addr) == 0 ){
		DEBASSERT ("can set request address")
	}
	sendto(sock, _message->getIncBuffer().c_str(), _message->getIncBuffer().length() , 0, &si_part, sizeof(si_part));

	return;
}
void SUDP::sendReply(MESSAGE* _message){

	//Reply uses topmost Via header
	C_HeadVia* viatmp = (C_HeadVia*) _message->getSTKHeadVia().top();

	struct sockaddr_in si_part;
	memset((char *) &si_part, 0, sizeof(si_part));
	si_part.sin_family = AF_INET;
	si_part.sin_port = htons(viatmp->getC_AttVia().getS_HostHostPort().getPort());
	if( inet_aton(viatmp->getC_AttVia().getS_HostHostPort().getHostName(), si_part.sin_addr) == 0 ){
		DEBASSERT ("can set reply address")
	}
	sendto(sock, _message->getIncBuffer().c_str(), _message->getIncBuffer().length() , 0, &si_part, sizeof(si_part));

	return;
}



