//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Service Layer
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

#define SUDP_H

////**********************************************************************************
////**********************************************************************************
class SUDP;
class ENGINE;
class ALMGR;
//
//**********************************************************************************
//**********************************************************************************
typedef struct tuple {
    SUDP * st;
} SUDPtuple;
//**********************************************************************************
//**********************************************************************************
//SUDP
// One thread on the socket.
// Future:
// Runs on two threads one for
// listening messages one for
// commands
//**********************************************************************************
//**********************************************************************************
class SUDP {

    public:

        // Init stack
        void init(int _PORT, ENGINE* _ENGINE, DOA* _doa, string _domain, ALMGR* _alarm);

        // Start call processing
        void start(void);

        // Suspend call processing, finish active calls
        //void suspend(void);

        // Suspend call processing including active calls and purge
        //void abort(void);

        // listen is threaded
        void listen(void);
        string getDomain(void);
        int getPort(void);
        ALMGR* getAlmgr(void);

        void sendRequest(MESSAGE* message);
        void sendReply(MESSAGE* message);

        ENGINE* getDoa();

    private:

        ENGINE* engine;
        ThreadWrapper *listenerThread;
        string domain;
        int sock;
        sockaddr_inX echoServAddr;
        unsigned short echoServPort;
        unsigned int cliAddrLen;
        sockaddr_inX echoClntAddr;
        char echoBuffer[ECHOMAX];
        int recvMsgSize;

        ALMGR* alarm;

        DOA* doa;
};
