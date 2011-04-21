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
#include <vector>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <map>

#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stack>

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
#ifndef TRNSPRT_H
#include "TRNSPRT.h"
#endif

//**********************************************************************************
//**********************************************************************************
SIPENGINE::SIPENGINE(int _i, int _m, string _s):ENGINE(_i,_m,_s){}
//**********************************************************************************
//**********************************************************************************
ENGINE * SIPENGINE::getSL_CC(void){
    return sl_cc;
}
//**********************************************************************************
//**********************************************************************************
void SIPENGINE::linkTransport(TRNSPRT* _transport){
	transport = _transport;
}

//**********************************************************************************
//**********************************************************************************
void SIPENGINE::setSL_CC(ENGINE* _sl_cc) {
    sl_cc = _sl_cc;
}
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//*********************************************************************************
//**********************************************************************************
//**********************************************************************************
void SIPENGINE::parse(void* __mess, int _mmod) {

    RELLOCK(&(sb[_mmod]->condvarmutex),"sb"<<_mmod<<".condvarmutex");

    PROFILE("SIPENGINE::parse() start")
    TIMEDEF
    SETNOW

	MESSAGE* _mess = (MESSAGE*)__mess;


    //Check if Request or Reply
    _mess->getTotLines();
    DEBSIP("SIPENGINE::parse", _mess << "] ["<<_mess->getLine(0) << "] [" <<_mess->getKey())

    int type = _mess->getReqRepType();

    if (type == REQSUPP) {

        DEBSIP("SIPENGINE::parse getHeadSipRequest content", _mess->getHeadSipRequest().getContent())

        int method = _mess->getHeadSipRequest().getS_AttMethod().getMethodID();

        if (	// Supported methods
            method != INVITE_REQUEST &&
            method != BYE_REQUEST &&
            method != ACK_REQUEST) {

                DEBSIP("SIPENGINE::parse unsupported METHOD ",_mess->getIncBuffer())
                PURGEMESSAGE(_mess)

        } else {
            transport->upCall(_mess, (SL_CC*)sl_cc);
        }
    }
    else if ( type == REPSUPP) {

        DEBSIP("SIPENGINE::parse getHeadSipReply content", _mess->getHeadSipReply().getContent())

        //All replies must be considered
        transport->upCall(_mess, (SL_CC*)sl_cc);

    }
    else {
        // purge it
        PURGEMESSAGE(_mess)
    }

    PRINTDIFF("SIPENGINE::parse() end")

}
