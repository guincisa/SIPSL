//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPCSL Sip Core And Service Layer
// Copyright (C) 2011 Guglielmo Incisa di Camerana
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
// SIPENGINE Syntax Layer
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
#ifndef DAO_H
#include "DAO.h"
#endif


static SIPUTIL SipUtil;

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
TRNSPRT* SIPENGINE::getTRNSPRT(void){
	return transport;
}

//**********************************************************************************
//**********************************************************************************
void SIPENGINE::setSL_CC(ENGINE* _sl_cc) {
    sl_cc = _sl_cc;
}
void SIPENGINE::setDAO(DAO* _dao) {
    dao = _dao;
}
DAO* SIPENGINE::getDAO(void) {
    return dao;
}

//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//*********************************************************************************
//**********************************************************************************
//**********************************************************************************
void SIPENGINE::parse(void* __mess, int _mmod) {
	DEBUGSIPENGINE_3("void SIPENGINE::parse(void* __mess, int _mmod)",__mess<<"]["<<_mmod)

    RELLOCK(&(sb[_mmod]->condvarmutex),"sb"<<_mmod<<".condvarmutex");

    PROFILE("SIPENGINE::parse() start")

    TIMEDEF
    SETNOW

	MESSAGE* _mess = (MESSAGE*)__mess;

    //TODO is that needed? already done in SUDP)
    _mess->fillIn();

    //will crash if invalid
    int type = _mess->getReqRepType();

    if (type == REQSUPP) {

    	DEBUGSIPENGINE("SIPENGINE::parse getHeadSipRequest content", _mess->getHeadSipRequest())

        int method = _mess->getHeadSipRequestCode();

        if (	// Supported methods
            method != INVITE_REQUEST &&
            method != BYE_REQUEST &&
            method != ACK_REQUEST &&
            method != REGISTER_REQUEST &&
            method != MESSAGE_REQUEST) {

        	DEBUGSIPENGINE("SIPENGINE::parse unsupported METHOD ",_mess->getOriginalString())
			PURGEMESSAGE(_mess)

        }
        else{
            sl_cc->p_w((void*)_mess);
            //transport->upCall(_mess, (SL_CC*)sl_cc);
            PRINTDIFF("SIPENGINE p_w")
        }
    }
    else if ( type == REPSUPP) {

    	DEBUGSIPENGINE("SIPENGINE::parse getHeadSipReply content", _mess->getHeadSipReply())

        //All replies must be considered
		sl_cc->p_w((void*)_mess);
        PRINTDIFF("SIPENGINE p_w")

    }
    else if (type == RECOMMPD){
    	DEBUGSIPENGINE("Provisioning and diagnostic",_mess)
    	dao->p_w(_mess);
    }
    else {
        // purge it
        PURGEMESSAGE(_mess)
    }

    CALCPERF("SIPENGINE::parse() end",2)

}
