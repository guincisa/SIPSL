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

ALO::ALO(ENGINE* _sl_cc):ENGINE(){

	sl_cc = _sl_cc;

}
void ALO::parse(MESSAGE* _message) {

	DEBOUT("ALO","Dispatcher")
	pthread_mutex_unlock(&(sb.condvarmutex));

	// check message type and invoke call back...

	if (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST){
		DEBOUT("ALO Dispatches ",_message->getHeadSipRequest().getContent())
		onInvite(_message);
	}
	else {
		DEBOUT("ALO Decoupling ",_message->getHeadSipRequest().getContent())
		sl_cc->p_w(_message);
	}


}
void ALO::onInvite(MESSAGE* m){
	DEBOUT("ALO unoverridded onINVITE called ", m->getIncBuffer())
}
