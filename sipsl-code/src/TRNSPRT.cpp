//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Core And Service Layer
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

#include <vector>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <map>
#include <string.h>

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
#ifndef TRNSCT_SM_H
#include "TRNSCT_SM.h"
#endif
#ifndef TRNSPRT_H
#include "TRNSPRT.h"
#endif


void TRNSPRT::upCall(MESSAGE* _message, SL_CC* _sl_cc){
	DEBNTW("TRNSPRT::upCall", _message)
	bool r = _sl_cc->p_w(_message);
	if(!r){
		DEBOUT("TRNSPRT::upCall message rejected, put in rejection queue",_message)
		bool ret2 = _sl_cc->p_w_s(_message);
		if (!ret2){
			if (!_message->getLock()){
				PURGEMESSAGE(_message)
			}
		}

	}
}
void TRNSPRT::downCall(MESSAGE* _message, CALL_OSET* _call_oset){

	//RETRANSMISSIONS
	//INVITE_B retransmission is setup using ALARM
	//1) ALARM sends INVITE_B with destination = NTW
	//2) INVITE_B arrives here and ALARM is armed again with higher timeout
	//3) if max resends reached then ALARM is not triggered error is sent to CL
	//   CL send error to VALO
	//   VALO sends error to SV
	//   SV sends error to A
	//4) if 1xx arrives in upCall, ALARM is dearmed

	// requiremets:
	// - ALARM are armed using (call-id, branch, orderOfOp)
	// max-resend are referred using same tuple
	// 1xx are identified by (call-id,branch) orderOfOp is predefined for this resend logic


	//200OK_A retransmission in case ACK_A is missing
	//ALARM is armed all is like above

	//ACK_B Retransmission in case B resends 200OK_B
	//ACK_B is stored and resent
	//in case no ACK is found error is sent

	//BYE_B in case 200Ok is missing

	//TRNSPRT can be Engine:
	//need map for max-invite retransm <(callid-branch), int>
	// and for all other counters
	// map for message to resend <(callid-branch),MESSAGE*>
	// map indicating is call_oset is alive (?)

	//CALL ABORT
	//for timeouts (
	//for errors messages from network



	DEBNTW("TRNSPRT::downCall", _message)

	if(_message->getTypeOfInternal() == TYPE_OP){
		//Thsi rule is needed to clear alarms when running ~CALL_OSET
		DEBOUT("RULE Break, can't send a message with type op",this)
		DEBASSERT("RULE Break, can't send a message with type op")
	}

	if (_message->getReqRepType() == REPSUPP) {
		//TODO Check if there is a ROUTE header
		_call_oset->getENGINE()->getSUDP()->sendReply(_message);
	}
	else if (_message->getReqRepType() == REQSUPP) {
		_call_oset->getENGINE()->getSUDP()->sendRequest(_message);
	}
	else {
		DEBASSERT("Unexpected sending to network")
	}

}




