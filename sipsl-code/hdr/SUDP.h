//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Service Layer
// Copyright (C) 2016 Guglielmo Incisa di Camerana
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
#ifndef SUDP_H
#define SUDP_H

////**********************************************************************************
////**********************************************************************************
class SUDP;
class ENGINE;
class ALMGR;
class DOA;
class DAO;
class TRNSPRT;
//
//**********************************************************************************
//**********************************************************************************
typedef struct tuple {
    SUDP * st;
    int thid;
} SUDPtuple;
typedef struct addrinfo addrinfoX;
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
		//singleThread will ignore the SUDPTH value and create a single threaded SUDP, that is used to listen to memory instantiations
        //void init(int _PORT, ENGINE* _ENGINE, DOA* _doa, string _domain, ALMGR* _alarm, bool singleThread);
		void init(int _PORT, TRNSPRT* _engine, string _domain, ALMGR* _alarm, bool singleThread);

        // Start call processing
		//set processing type here now
        void start(int processingType);

        // Suspend call processing, finish active calls
        //void suspend(void);

        // Suspend call processing including active calls and purge
        //void abort(void);

        // listen is threaded
        void listen(int);
        string getDomain(void);
        int getPort(void);
        ALMGR* getAlmgr(void);

        void sendRequest(MESSAGE* message);
        void sendReply(MESSAGE* message);

        void sendRawMessage(string* message, string address, int port);

        void setDAO(DAO* dao);

        //ENGINE* getDoa();
        string getLocalIp(void);
        string getLocalPort(void);

        static int getRealm(void);

        TRNSPRT* getTRNSPRT(void);

        int getProcessingType(void);


    private:

        //ENGINE* engine;
        TRNSPRT* transport;
        ThreadWrapper *listenerThread[2*SUDPTH];
        string domain;
        string localip;
        string localport;
        int sock_se[SUDPTH];
        int sock_re;
        sockaddr_inX echoServAddr;
        unsigned short echoServPort;
        unsigned int cliAddrLen;
        sockaddr_inX echoClntAddr;

        int processingType;

        static const int realm;

        DAO* dao;


        ALMGR* alarm;

        //DOA* doa;

        //Seamless failover
        int threadNum; // one

};
#endif
