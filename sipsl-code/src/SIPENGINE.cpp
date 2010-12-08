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
SIPENGINE::SIPENGINE(int _i):ENGINE(_i){}
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
void SIPENGINE::parse_s(MESSAGE* _mess) {
	//Message from network cannot be managed since main buffer is full
	//Cases:
	// INVITE
	// Replies
	// ACK
	// BYE
	//
	//  INVITE
	//  - new invite
	//  - retransmission
	// New INVITE
	// resend a 503 Server Unavailable
	// do not allocate any call_oset
	//
	// Retransmission INVITE
	// The call_oset may already exist, two possibilities
	// 1. Ignore the INVITE and let the call continue
	// 2. Reply 503 Server Unavailable and trigger the deletion of the call_oset
	//    also send a CANCEL to B
	//
	// Reply from B (100 Trying, 180 Ringing, 200 Ok
	// 1. Send a CANCEL to B
	//    Send a 503 Server Unavailable to A
	//    Trigger deletion of call_oset
	//
	// ACK from A, BYE from A or B
	// 1. Reply 503 Server Unavailable and trigger the deletion of the call_oset
	//    also send a CANCEL to B
	//
	//




	DEBOUT("SIPENGINE::parse_s", _mess)
	RELLOCK(&(rej.condvarmutex),"rej.condvarmutex");
	PURGEMESSAGE(_mess)
	return;
}

//**********************************************************************************
//**********************************************************************************
void SIPENGINE::parse(MESSAGE* _mess) {

	RELLOCK(&(sb.condvarmutex),"sb.condvarmutex");

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
			return;

		} else {
			transport->upCall(_mess, (SL_CC*)sl_cc);
		}
	}
	else if ( type == REPSUPP) {

		DEBSIP("SIPENGINE::parse getHeadSipReply content", _mess->getHeadSipReply().getContent())
//		int reply_id = _mess->getHeadSipReply().getReply().getReplyID();
//		int code = _mess->getHeadSipReply().getReply().getCode();
//		DEBSIP("SIPENGINE::reply type and code", reply_id << " " << code)

		//All replies must be considered
		transport->upCall(_mess, (SL_CC*)sl_cc);
//		sl_cc->p_w(_mess);

	}
	else {
		// purge it
		PURGEMESSAGE(_mess)
	}
}
