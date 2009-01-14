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
#include "SUDP.h"

// *****************************************************************************************
// ************************************
ParseEx::ParseEx(string s) {
    error = s;
}
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
// Initialize Stack
// *****************************************************************************************
// *****************************************************************************************
void SUDP::init(int port, ENGINE *ra, string _domain){

    DEBOUT("SUDP init","")

    domain = _domain;

    _ENGINE = ra;
    echoServPort = port;

    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        DEBERROR("socket() failed)")
        //perror("socket() failed");
        return;
    }

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */
    
    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0) {
        DEBERROR("bind() failed)")
        //perror("bind() failed");
        return;
    }
    DEBOUT("SUDP inited and ready","")
    return;
}

// *****************************************************************************************
// Start Stack
// *****************************************************************************************
// *****************************************************************************************
void SUDP::start(void) {
    // allocate thread and starts

    cout << "SUDP::start" << endl;

    listenerThread = new ThreadWrapper;
    SUDPtuple *t1;
    t1 = new SUDPtuple;
    t1->st = this;;
    // TODO boooh
    int res;
    res = pthread_create(&(listenerThread->thread), NULL, SUDPSTACK, (void *) t1 );
    return;
}

// *****************************************************************************************
// Listen to network
// *****************************************************************************************
// *****************************************************************************************
void SUDP::listen() {
    for (;;) /* Run forever */ {
        /* Set the size of the in-out parameter */
        cliAddrLen = sizeof(echoClntAddr);

        /* Block until receive message from a client */
        memset(&echoBuffer, 0x0, ECHOMAX);   /* Zero out structure */
        if ((recvMsgSize = recvfrom(sock, echoBuffer, ECHOMAX, 0,
            (struct sockaddr *) &echoClntAddr, (socklen_t*)&cliAddrLen)) < 0) {
            DEBERROR("rcvfrom() failed)")
            //perror("recvfrom() failed");
            return;
        }

    MESSAGE im(echoBuffer,sock, echoClntAddr);
    im.source = 1;
    //im.in_ts = gethrtime();
    GETTIME(im.in_ts)
    DEBOUT("Incoming<n",im.incomingMessage)
    //cout << " Incoming -----------------------------------"<<endl << im.incomingMessage << endl<< "-------------------------" << endl;

    DECTIME
    STARTTIME
    _ENGINE->p_w(im);
    ENDTIME
    
    }
}

// *****************************************************************************************
// getDomain
// *****************************************************************************************
// *****************************************************************************************
string SUDP::getDomain(void) {
    return domain;
}
