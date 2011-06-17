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
#include <strings.h>
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <netdb.h>
#include <time.h>



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

// *****************************************************************************************
// Socket listener thread
// *****************************************************************************************
// *****************************************************************************************
extern "C" void* SUPDSTACK (void*);

void * SUDPSTACK(void *_tgtObject) {

	DEBINFSUDP("SUDPSTACK start","")

    SUDPtuple *tgtObject = (SUDPtuple *)_tgtObject;

    tgtObject->st->listen(tgtObject->thid);

    DEBINFSUDP("SUDPSTACK started","")

    return (NULL);
}

// *****************************************************************************************
// SUDP
// Initialize Stack
// *****************************************************************************************
// *****************************************************************************************
void SUDP::init(int _port, ENGINE *_engine, DOA* _doa, string _domain, ALMGR* _alarm){

	DEBINFSUDP("SUDP init",_domain)

    domain = _domain;

    engine = _engine;

    echoServPort = _port;

    cliAddrLen = sizeof(echoClntAddr);

    alarm = _alarm;

    doa = _doa;

    /* Create socket for sending/receiving datagrams */
	for (int i = 0 ; i <SUDPTH ; i++){
		if ((sock_se[i] = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
			DEBASSERT("socket() failed)")
			return;
		}
	}
	if ((sock_re = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		DEBASSERT("socket() failed)")
		return;
	}

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    //Init mutex
#ifdef USEMESSAGEMAP
    for (int i = 0; i < MESSAGEMAPS;i++){
    	pthread_mutex_init(&messTableMtx[i],NULL);
    }
#endif
#ifdef USEMAPMODUL
	pthread_mutex_init(&modulusMapMtx,NULL);
	modulusIter = 0;
#endif

    /* Bind to the local address */
	if (bind(sock_re, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0) {
		DEBERROR("bind() failed)");
	}

    _engine->linkSUDP(this);

    DEBINFSUDP("SUDP init done","")

    return;
}

// *****************************************************************************************
// Start Stack
// *****************************************************************************************
// *****************************************************************************************
void SUDP::start(void) {

    // allocate thread and starts

	DEBINFSUDP("SUDP::start threads",SUDPTH)
	SUDPtuple *t1[SUDPTH*2];

	for (int i = 0 ; i <(2*SUDPTH) ; i++){
	    NEWPTR2(listenerThread[i], ThreadWrapper,"ThreadWrapper"<<i)

	    NEWPTR2(t1[i], SUDPtuple,"SUDPtuple")
	    t1[i]->st = this;
	    t1[i]->thid = i;

	    pthread_create(&(listenerThread[i]->thread), NULL, SUDPSTACK, (void *) t1[i] );

	}

    return;
}

// *****************************************************************************************
// Listen to network
// *****************************************************************************************
void SUDP::listen(int _socknum) {

    TIMEDEF

    DEBINFSUDP("SUDP::listen","listen " << _socknum)
    for (;;){
        /* Block until receive message from a client */
        char echoBuffer[ECHOMAX];
        memset(&echoBuffer, 0x0, ECHOMAX);
        int recvMsgSize;
        int _sok;
        if ( _socknum < SUDPTH ){
        	_sok = sock_se[_socknum];
        }
        else {
        	_sok = sock_re;
        }
        if ((recvMsgSize = recvfrom(_sok, echoBuffer, ECHOMAX, 0,
            (struct sockaddr *) &echoClntAddr, (socklen_t*)&cliAddrLen)) < 0) {
            DEBERROR("SUDP::listen() recvfrom() failed")
        }else if ( recvMsgSize < 1){
        	DEBERROR("SUDP::listen() abnormal message")
        }else {
            SETNOW
            PROFILE("SUDP:Message arrived from socket")
            //Message handling
            MESSAGE* message=0x0;
            CREATENEWMESSAGE_EXT(message, echoBuffer, sock_se[_socknum], echoClntAddr, SODE_NTWPOINT)
            if (message != 0x0 ){
                DEBMESSAGE("New message from buffer ", message)

				//ENGINEMAPS
				//problem if not sip...?
                message->fillIn();

                engine->p_w((void*)message);
                PRINTDIFF("SUDP::listen() Message sent to SIPENGINE")
            }else {
                DEBERROR("SUDP::listen() could not allocate memory for incoming message")
            }
        }
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
	DEBINFSUDP("void SUDP::sendRequest(MESSAGE* _message)",_message)

    struct sockaddr_in si_part;
    struct hostent *host;
    memset((char *) &si_part, 0, sizeof(si_part));

    DEBMESSAGE("SUDP::sendRequest sending Message ", _message)

    si_part.sin_family = AF_INET;
    pair<string,int> _pair = _message->getUri("REQUEST");
    char _hostchar[_pair.first.length()+1];
    strcpy(_hostchar,_pair.first.c_str());
    host = gethostbyname(_hostchar);
    bcopy((char *)host->h_addr, (char *)&si_part.sin_addr.s_addr, host->h_length);
    si_part.sin_port = htons(_pair.second);

//	if( inet_aton(_message->getHeadSipRequest().getC_AttSipUri().getChangeS_AttHostPort().getHostName().c_str(), &si_part.sin_addr) == 0 ){
//		DEBASSERT ("Can't set request address")
//	}

    int i = _message->getModulus() % SUDPTH;

    sendto(sock_se[i], _message->getMessageBuffer(), strlen(_message->getMessageBuffer()) , 0, (struct sockaddr *)&si_part, sizeof(si_part));

    if (!_message->getLock()){
        PURGEMESSAGE(_message)
    }

    return;
}
void SUDP::sendReply(MESSAGE* _message){
	DEBINFSUDP("void SUDP::sendReply(MESSAGE* _message)",_message)

    //Reply uses topmost Via header

    struct sockaddr_in si_part;
    struct hostent *host;
    memset((char *) &si_part, 0, sizeof(si_part));

    DEBMESSAGE("SUDP::sendReply sending Message ", _message)

    si_part.sin_family = AF_INET;
    char _hostchar[_message->getViaUriHost().length()+1];
    strcpy(_hostchar,_message->getViaUriHost().c_str());
    host = gethostbyname(_hostchar);
    bcopy((char *)host->h_addr, (char *)&si_part.sin_addr.s_addr, host->h_length);
    si_part.sin_port = htons(_message->getViaUriPort());

    if( inet_pton(AF_INET, _message->getViaUriHost().c_str(), &si_part.sin_addr) == 0 ){
            DEBASSERT ("can set reply address")
    }

    int i = _message->getModulus() % SUDPTH;
    sendto(sock_se[i],  _message->getMessageBuffer(), strlen(_message->getMessageBuffer()) , 0, (struct sockaddr *)&si_part, sizeof(si_part));
    if (!_message->getLock()){
        PURGEMESSAGE(_message)
    }
    return;
}
