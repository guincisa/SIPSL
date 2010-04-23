//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPCSL Sip Core And Service Layer
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


#include <stdio.h>


#ifndef MESSAGE_H
#include "MESSAGE.h"
#endif
#ifndef ALO_H
#include "ALO.h"
#endif
#ifndef CALL_OSET_H
#include "CALL_OSET.h"
#endif

ALO::ALO(ENGINE* _sl_cc, CALL_OSET* _oset):ENGINE(){

	sl_cc = _sl_cc;
	call_oset = _oset;

}
void ALO::parse(MESSAGE* _message) {

	DEBOUT("ALO","Dispatcher")
	pthread_mutex_unlock(&(sb.condvarmutex));

	// check message type and invoke call back...

	if (_message->getReqRepType() == REQSUPP){
		if (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST){
			DEBOUT("ALO Dispatches ",_message->getHeadSipRequest().getContent())
			onInvite(_message);
		}
		else if (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == ACK_REQUEST){
			DEBOUT("ALO Dispatches ",_message->getHeadSipRequest().getContent())
			onAck(_message);
		}
		else if (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == BYE_REQUEST){
			DEBOUT("ALO Dispatches ",_message->getHeadSipRequest().getContent())
			if (_message->getGenEntity() == SODE_SMCLPOINT){
				DEBOUT("Backward BYE","")
				onBye(_message, -1);
			}
			if(_message->getGenEntity() == SODE_SMSVPOINT){
				DEBOUT("Forward BYE","")
				onBye(_message, 1);
			}
			else {
				DEBOUT("Unqualified BYE","")
			}
		}
		else {
			noCallBack(_message);
		}
	}
	else if (_message->getReqRepType() == REPSUPP){
		if (_message->getHeadSipReply().getReply().getCode() == OK_200){
			DEBOUT("ALO Dispatches ",_message->getHeadSipRequest().getContent())
			on200Ok(_message);
		}
		else {
			noCallBack(_message);
		}
	}
}
void ALO::onInvite(MESSAGE* m){
	DEBOUT("ALO unoverridded onINVITE called ", m->getIncBuffer())
}
void ALO::onAck(MESSAGE* m){
	DEBOUT("ALO unoverridded onAck called ", m->getIncBuffer())
}
void ALO::onBye(MESSAGE* m, int _dir){
	DEBOUT("ALO unoverridded onBye called ", m->getIncBuffer())
}
void ALO::on200Ok(MESSAGE* m){
	DEBOUT("ALO unoverridded on200Ok called ", m->getIncBuffer())
}

void ALO::noCallBack(MESSAGE* _message){

	DEBOUT("ALO noCallBack - Decoupling ",_message->getHeadSipRequest().getContent())
	if(_message->getGenEntity() == SODE_SMCLPOINT)
		_message->setDestEntity(SODE_SMSVPOINT);
	else if (_message->getGenEntity() == SODE_SMSVPOINT)
		_message->setDestEntity(SODE_SMCLPOINT);

	_message->setGenEntity(SODE_ALOPOINT);

	sl_cc->p_w(_message);

}

