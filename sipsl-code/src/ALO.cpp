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
#include <algorithm>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <iostream>
#include <map>
#include <unordered_map>
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
#ifndef ACTION_H
#include "ACTION.h"
#endif
#ifndef CALL_OSET_H
#include "CALL_OSET.h"
#endif
#ifndef COMAP_H
#include "COMAP.h"
#endif
#ifndef SL_CC_H
#include "SL_CC.h"
#endif
#ifndef ALARM_H
#include "ALARM.h"
#endif
#ifndef UTIL_H
#include "UTIL.h"
#endif
#ifndef ALO_H
#include "ALO.h"
#endif


//V6
ALO::ALO(ENGINE* _sl_cc, CALL_OSET* _oset){

	sl_cc = _sl_cc;
	call_oset = _oset;

}
//V6
void ALO::call(MESSAGE* _message) {
	DEBINF("void ALO::call(MESSAGE* _message)",this<<"]["<<_message)
	DEBALO("ALO","call")
	//V6
	//pthread_mutex_unlock(&(sb.condvarmutex));

	// check message type and invoke call back...

	if (_message->getReqRepType() == REQSUPP){
		if (_message->getHeadSipRequestCode() == INVITE_REQUEST){
			DEBALO("ALO Dispatches INVITE_REQUEST",_message->getHeadSipRequestCode())
			onInvite(_message);
		}
		else if (_message->getHeadSipRequestCode()== ACK_REQUEST && _message->getTypeOfInternal() == TYPE_MESS && _message->getType_trnsct() == TYPE_TRNSCT){
			DEBALO("ALO Dispatches ACK_REQUEST TRANSCT",_message->getHeadSipRequestCode())
			onAck(_message);
		}
		else if (_message->getHeadSipRequestCode() == ACK_REQUEST && _message->getTypeOfInternal() == TYPE_MESS && _message->getType_trnsct() == TYPE_NNTRNSCT ){
			DEBALO("ALO Dispatches ACK_REQUEST NOTRANSCT",_message->getHeadSipRequestCode())
			onAckNoTrnsct(_message);
		}
		else if (_message->getHeadSipRequestCode() == BYE_REQUEST){
			DEBALO("ALO Dispatches BYE_REQUEST",_message->getHeadSipRequestCode())
			onBye(_message);
		}
		else {
			noCallBack(_message);
		}
	}
	else if (_message->getReqRepType() == REPSUPP){
		if (_message->getHeadSipReplyCode() == OK_200){
			DEBALO("ALO Dispatches ",_message->getHeadSipReplyCode())
			on200Ok(_message);
		}
		else {
			noCallBack(_message);
		}
	}
}
void ALO::onInvite(MESSAGE* m){
	DEBMESSAGE("ALO unoverridded onINVITE called ", m)
}
void ALO::onAck(MESSAGE* m){
	DEBMESSAGE("ALO unoverridded onAck called ", m)
}
void ALO::onBye(MESSAGE* m){
	DEBMESSAGE("ALO unoverridded onBye called ", m)
}
void ALO::on200Ok(MESSAGE* m){
	DEBMESSAGE("ALO unoverridded on200Ok called ", m)
}
void ALO::onAckNoTrnsct(MESSAGE* m){
	DEBMESSAGE("ALO unoverridded onAckNoTrcts called ", m)
}
void ALO::noCallBack(MESSAGE* _message){

	DEBALO("ALO noCallBack - Decoupling ",_message->getHeadSipRequestCode())
	if(_message->getGenEntity() == SODE_SMCLPOINT)
		_message->setDestEntity(SODE_SMSVPOINT);
	else if (_message->getGenEntity() == SODE_SMSVPOINT)
		_message->setDestEntity(SODE_SMCLPOINT);

	_message->setGenEntity(SODE_ALOPOINT);

	sl_cc->p_w(_message);

}
//**********************************************************************************
//**********************************************************************************
//V6
void ALO::linkSUDP(SUDP *_sudp){
	sudp = _sudp;
}
//V6
SUDP* ALO::getSUDP(void){
	return sudp;
}
