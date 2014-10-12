//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPCSL Sip Core And Service Layer
// Copyright (C) 2014 Guglielmo Incisa di Camerana
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
//**********************************************************************************
// SEAMFAIL
// receives roi commands
// sends roi commands
// heartbeat
//**********************************************************************************

#include <algorithm>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <iostream>
#include <map>
#include <math.h>
#include <memory>
#include <pthread.h>
#include <signal.h>
#include <sstream>
#include <stack>
#include <stdio.h>
#include <stdlib.h>     /* for atoi() and exit() */
#include <string>
#include <string.h>
#include <sys/socket.h> /* for socket() and bind() */
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <vector>
#include <unordered_map>

#include "UTIL.h"

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
#ifndef SEAMFAIL_H
#include "SEAMFAIL.h"
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
#ifndef TRNSPRT_H
#include "TRNSPRT.h"
#endif
#ifndef DAO_H
#include "DAO.h"
#endif


static SIPUTIL SipUtil;
typedef struct ENGTuple;

void * heartBeatMonitor (void * _pt){
    HearBeatTuple *pt = (HearBeatTuple *)  _pt;

    string* messageToPeer = new string("stronzone");
    string peerAddress(pt->peerAddress);
	while (true){
		BDEBUG("Hearbeat Pulse ",&(pt->sleep_time).tv_sec)

		//send heartbeat message to peer
		(pt->hbSUDP)->sendRawMessage(messageToPeer,peerAddress,pt->peerPort);

        nanosleep(&(pt->sleep_time),NULL);
	}
    return (NULL);
}

//**********************************************************************************
//**********************************************************************************
SEAMFAILENG::SEAMFAILENG(int _i, int _m, string _s):ENGINE(_i,_m,_s){

	sleep_time.tv_sec = HEARTBEATPULSE;
	sleep_time.tv_nsec = 0;

	NEWPTR2(heartBeat, ThreadWrapper(), "ThreadWrapper()")

}
//**********************************************************************************
//**********************************************************************************
void SEAMFAILENG::setSUDP(SUDP* _hbSUDP, string _peerAddress, int _peerPort){
	hbSUDP = _hbSUDP;
	peerAddress = _peerAddress;
	peerPort = _peerPort;
	HearBeatTuple* t = new HearBeatTuple;
	t->sleep_time = sleep_time;
	t->hbSUDP = hbSUDP;
	t->peerAddress = peerAddress;
	t->peerPort = peerPort;
	pthread_create(&(heartBeat->thread), NULL, heartBeatMonitor, (void *)t);

}
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//*********************************************************************************
//**********************************************************************************
//**********************************************************************************
void SEAMFAILENG::parse(void* __mess, int _mmod) {
	DEBINF("void SEAMFAILENG::parse(void* __mess, int _mmod)",__mess<<"]["<<_mmod)

    RELLOCK(&(sb[_mmod]->condvarmutex),"sb"<<_mmod<<".condvarmutex");

    PROFILE("SEAMFAILENG::parse() start")

    TIMEDEF
    SETNOW

	MESSAGE* _mess = (MESSAGE*)__mess;
	BDEBUG("received ",_mess->getOriginalString())
//
//    _mess->fillIn();
//
//    int type = _mess->getReqRepType();
//    DEBSIP("SIPENGINE::parse", _mess << "] ["<<_mess->getFirstLine() << "] type[" <<type)
//
//    if (type == REQSUPP) {
//
//        DEBSIP("SIPENGINE::parse getHeadSipRequest content", _mess->getHeadSipRequest())
//
//        int method = _mess->getHeadSipRequestCode();
//
//        if (	// Supported methods
//            method != INVITE_REQUEST &&
//            method != BYE_REQUEST &&
//            method != ACK_REQUEST &&
//            method != REGISTER_REQUEST &&
//            method != MESSAGE_REQUEST) {
//
//			DEBSIP("SIPENGINE::parse unsupported METHOD ",_mess->getOriginalString())
//			PURGEMESSAGE(_mess)
//
//        }
//        else{
//            transport->upCall(_mess, (SL_CC*)sl_cc);
//            PRINTDIFF("SIPENGINE p_w")
//        }
//    }
//    else if ( type == REPSUPP) {
//
//        DEBSIP("SIPENGINE::parse getHeadSipReply content", _mess->getHeadSipReply())
//
//        //All replies must be considered
//        transport->upCall(_mess, (SL_CC*)sl_cc);
//        PRINTDIFF("SIPENGINE p_w")
//
//    }
//    else if (type == RECOMMPD){
//    	DEBOUT("Provisioning and diagnostic",_mess)
//    	dao->p_w(_mess);
//    }
//    else {
//        // purge it
//        PURGEMESSAGE(_mess)
//    }

    CALCPERF("SIPENGINE::parse() end",2)

}
