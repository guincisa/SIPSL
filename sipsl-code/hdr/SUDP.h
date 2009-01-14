//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Service Layer 
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
//
// *****************************************************************************************
/*****************************************************************************************
  SUPD.h
  UDP layer for incoming UDP messages
  creates a socket and listens
  invokes (using thread pool) the parseMessage method
  which is virtual and needs to be overridden by the
  ENGINE class which will parse message
*****************************************************************************************/

//#include <pthread.h>
//#include <unistd.h>
//#include <iostream>
//#include <stdio.h>
//#include <string>
//#include <sys/socket.h> /* for socket() and bind() */
//#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
//#include <stdlib.h>     /* for atoi() and exit() */
//#include <string.h>     /* for memset() */
//#include "ENGINE.h"



//**********************************************************************************
//**********************************************************************************
class ParseEx {
    public:
    string error;
    ParseEx(string);
};

//**********************************************************************************
//**********************************************************************************
class SUDP;

//**********************************************************************************
//**********************************************************************************
typedef struct tuple {
    SUDP *st;
} SUDPtuple;



//**********************************************************************************
//**********************************************************************************
class SUDP {
    //TODO must be singleton
    //
    //Must run on two threads one for 
    //listening messages one for 
    //commands
    public:
        //SUDP stack

        // Init stack
        void init(int _PORT, ENGINE *_ENGINE,string);

        // Start call processing
        void start(void); 

        // Suspend call processing, finish active calls
        //void suspend(void);

        // Suspend call processing
        //void abort(void);

        // listen is threaded
        void listen(void);

        string getDomain(void);

    private:
        ENGINE *_ENGINE;

        ThreadWrapper *listenerThread;

        string domain;

        int sock;
        struct sockaddr_in echoServAddr;
        unsigned short echoServPort;
        unsigned int cliAddrLen;
        struct sockaddr_in echoClntAddr;
        char echoBuffer[ECHOMAX];
        int recvMsgSize;        
};
