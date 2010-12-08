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

    DEBDEV("SUDPSTACK start","")

	SUDPtuple *tgtObject = (SUDPtuple *)_tgtObject;

    tgtObject->st->listen();

    DEBDEV("SUDPSTACK started","")

    return (NULL);
}

// *****************************************************************************************
// SUDP
// Initialize Stack
// *****************************************************************************************
// *****************************************************************************************
void SUDP::init(int _port, ENGINE *_engine, DOA* _doa, string _domain, ALMGR* _alarm){

	DEBDEV("SUDP init",_domain)

    domain = _domain;

    engine = _engine;

    echoServPort = _port;

    alarm = _alarm;

    doa = _doa;

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

    DEBDEV("SUDP init done","")

    return;
}

// *****************************************************************************************
// Start Stack
// *****************************************************************************************
// *****************************************************************************************
void SUDP::start(void) {

    // allocate thread and starts

	DEBDEV("SUDP::start","")

	NEWPTR2(listenerThread, ThreadWrapper,"ThreadWrapper")
    SUDPtuple *t1;
	NEWPTR2(t1, SUDPtuple,"SUDPtuple")
    t1->st = this;

    //TODO result not used
    pthread_create(&(listenerThread->thread), NULL, SUDPSTACK, (void *) t1 );
    return;
}

// *****************************************************************************************
// Listen to network
// *****************************************************************************************
void SUDP::listen() {

	DEBDEV("SUDP::listen","listen")
    for (;;){
        /* Set the size of the in-out parameter */
    	DEBY
        cliAddrLen = sizeof(echoClntAddr);

        /* Block until receive message from a client */
        memset(&echoBuffer, 0x0, ECHOMAX);   /* Zero out structure */

        if ((recvMsgSize = recvfrom(sock, echoBuffer, ECHOMAX, 0,
            (struct sockaddr *) &echoClntAddr, (socklen_t*)&cliAddrLen)) < 0) {
            DEBERROR("SUDP::listen() recvfrom() failed")
            //return;
        }else {
			//Message handling
        	DEBY
        	MESSAGE* message=0x0;
			CREATENEWMESSAGE_EXT(message, echoBuffer, sock, echoClntAddr, SODE_NTWPOINT)
        	if (message != 0x0 ){
    			DEBMESSAGE("New message from buffer ", message)

    			bool r = engine->p_w(message);
    			if (!r){
    				DEBOUT("SUDP::listen() message rejected, put in rejection queue",message)
    				bool rr = engine->p_w_s(message);
    				if (!rr){
        				DEBOUT("SUDP::listen() message rejected by s queue",message)
        				PURGEMESSAGE(message)
    				}
    			}
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

	struct sockaddr_in si_part;
	struct hostent *host;
	memset((char *) &si_part, 0, sizeof(si_part));

	DEBSIP("Request address ", _message->getHeadSipRequest().getC_AttSipUri().getChangeS_AttHostPort().getHostName() <<":"<< _message->getHeadSipRequest().getC_AttSipUri().getChangeS_AttHostPort().getPort())
	DEBSIP("Request message ", _message->getLine(0))

	si_part.sin_family = AF_INET;
	host = gethostbyname(_message->getHeadSipRequest().getC_AttSipUri().getChangeS_AttHostPort().getHostName().c_str());
	bcopy((char *)host->h_addr, (char *)&si_part.sin_addr.s_addr, host->h_length);
	si_part.sin_port = htons(_message->getHeadSipRequest().getC_AttSipUri().getChangeS_AttHostPort().getPort());
//	if( inet_aton(_message->getHeadSipRequest().getC_AttSipUri().getChangeS_AttHostPort().getHostName().c_str(), &si_part.sin_addr) == 0 ){
//		DEBASSERT ("Can't set request address")
//	}
	sendto(sock, _message->getIncBuffer().c_str(), _message->getIncBuffer().length() , 0, (struct sockaddr *)&si_part, sizeof(si_part));

	if (!_message->getLock()){
		PURGEMESSAGE(_message)
	}

	return;
}
void SUDP::sendReply(MESSAGE* _message){

	//Reply uses topmost Via header
	C_HeadVia* viatmp = (C_HeadVia*) _message->getSTKHeadVia().top();

	struct sockaddr_in si_part;
	struct hostent *host;
	memset((char *) &si_part, 0, sizeof(si_part));

	DEBSIP("Reply address ", viatmp->getC_AttVia().getS_HostHostPort().getHostName() <<":"<< viatmp->getC_AttVia().getS_HostHostPort().getPort())
	DEBSIP("Reply message ", _message->getLine(0))

	si_part.sin_family = AF_INET;
	host = gethostbyname(viatmp->getC_AttVia().getS_HostHostPort().getHostName().c_str());
	bcopy((char *)host->h_addr, (char *)&si_part.sin_addr.s_addr, host->h_length);
	si_part.sin_port = htons(viatmp->getC_AttVia().getS_HostHostPort().getPort());
	if( inet_pton(AF_INET, viatmp->getC_AttVia().getS_HostHostPort().getHostName().c_str(), &si_part.sin_addr) == 0 ){
		DEBASSERT ("can set reply address")
	}
	sendto(sock, _message->getIncBuffer().c_str(), _message->getIncBuffer().length() , 0, (struct sockaddr *)&si_part, sizeof(si_part));

	if (!_message->getLock()){
		PURGEMESSAGE(_message)
	}

//	//TODO
//	//check here DOA and kill call_oset
//	if (_message->getDoa() == DOA_REQUESTED){
//		((DOA*)doa)->setComap(((SL_CC*)((SIPENGINE*)engine)->getSL_CC())->getCOMAP());
//		((DOA*)doa)->p_w(_message);
//	}

	return;
}
//void SUDP::sendRequest(MESSAGE* _message){
//
//	struct sockaddr_in si_part;
//	memset((char *) &si_part, 0, sizeof(si_part));
//	si_part.sin_family = AF_INET;
//	si_part.sin_port = htons(_message->getHeadSipRequest().getC_AttSipUri().getChangeS_AttHostPort().getPort());
//	if( inet_aton(_message->getHeadSipRequest().getC_AttSipUri().getChangeS_AttHostPort().getHostName().c_str(), &si_part.sin_addr) == 0 ){
//		DEBASSERT ("can set request address")
//	}
//	sendto(sock, _message->getIncBuffer().c_str(), _message->getIncBuffer().length() , 0, (struct sockaddr *)&si_part, sizeof(si_part));
//
//	return;
//}
//void SUDP::sendReply(MESSAGE* _message){
//
//	//Reply uses topmost Via header
//	C_HeadVia* viatmp = (C_HeadVia*) _message->getSTKHeadVia().top();
//	DEBOUT("Reply to ",  viatmp->getC_AttVia().getS_HostHostPort().getHostName() << " : " << viatmp->getC_AttVia().getS_HostHostPort().getPort())
//
//	struct sockaddr_in si_part;
//	memset((char *) &si_part, 0, sizeof(si_part));
//	si_part.sin_family = AF_INET;
//	si_part.sin_port = htons(viatmp->getC_AttVia().getS_HostHostPort().getPort());
//	if( inet_aton(viatmp->getC_AttVia().getS_HostHostPort().getHostName().c_str(), &si_part.sin_addr) == 0 ){
//		DEBASSERT ("can set reply address")
//	}
//	sendto(sock, _message->getIncBuffer().c_str(), _message->getIncBuffer().length() , 0, (struct sockaddr *)&si_part, sizeof(si_part));
//
//	return;
//}



