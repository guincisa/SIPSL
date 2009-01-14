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

/****************************************************
  ENGINE.h
*****************************************************/
//#include <pthread.h>
//#include <unistd.h>
//#include <iostream>
//#include <stdio.h>
//#include <string>
//#include <sys/socket.h> /* for socket() and bind() */
//#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
//#include <stdlib.h>     /* for atoi() and exit() */
//#include <string.h>     /* for memset() */
//#include "COMM.h"

using namespace std;

class ENGINE;

typedef struct tuple2 {
    ENGINE * ps;
    int id;
} ENGtuple;

//class ENGCALLBACK {
//    public:
//       ENGINE * rata;
//       ENGCALLBACK();
//       ENGCALLBACK(ENGINE * );
//       virtual void parsecallback(MESSAGE, ENGINE *, ENGINE *); //TODO RESPONSE
//};

class ENGINE {
    //TODO construtor mutex
    public:
    ENGINE();
    //ENGCALLBACK *engcb;

    //SPINBUFFER
    SPINB sb;

    //MESSAGE message[5];
    void p_w(MESSAGE);


    //reply socket?
    //int sock;
    //struct sockaddr_in echoClntAddr;

    //int busy[5];
    //int avail;
    //pthread_mutex_t waitavail, accessavail;
    ThreadWrapper * parsethread[5];
    //virtual void parse(int);
    virtual void parse(MESSAGE);

    //void associateCB(ENGCALLBACK*);
};


