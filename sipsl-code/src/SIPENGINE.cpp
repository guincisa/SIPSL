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
#ifndef ENGINE_H
#include "ENGINE.h"
#endif
#ifndef SIPENGINE_H
#include "SIPENGINE.h"
#endif

//**********************************************************************************
//**********************************************************************************
ENGINE * SIPENGINE::getSL_CC(void){
    return sl_cc;
}
//**********************************************************************************
//**********************************************************************************
void SIPENGINE::setSL_CC(ENGINE* _sl_cc) {
    sl_cc = _sl_cc;
}
//**********************************************************************************
//**********************************************************************************

//**********************************************************************************
//**********************************************************************************
void SIPENGINE::parse(MESSAGE* _mess) {

	pthread_mutex_unlock(&(sb.condvarmutex));

	//B2BUA, NGApplication, SBC

	//Check if Request or Reply
	DEBOUT("SIPENGINE::parse",_mess->getIncBuffer())


//	timespec sleep_time;
//	sleep_time.tv_sec = 40;
//	sleep_time.tv_nsec = 0;
//	nanosleep(&sleep_time,NULL);

	int tl = _mess->getTotLines();
	DEBOUT("SIPENGINE::parse tot lines",tl)

	int type = _mess->getReqRepType();
	DEBOUT("SIPENGINE::parse request type",type)

	if (type == REQSUPP) {

		DEBOUT("SIPENGINE::parse getHeadSipRequest content", _mess->getHeadSipRequest().getContent())

		int method = _mess->getHeadSipRequest().getS_AttMethod().getMethodID();
		if (	// Supported methods
				method != INVITE_REQUEST &&
				method != BYE_REQUEST &&
				method != ACK_REQUEST) {

			DEBOUT("SIPENGINE::parse unsupported METHOD ",_mess->getIncBuffer())
			PURGEMESSAGE(_mess,"SIPENGINE::parse message discarded")
			return;

		} else {

			sl_cc->p_w(_mess);

		}
	}
	else if ( type == REPSUPP) {

		DEBOUT("SIPENGINE::parse getHeadSipReply content", _mess->getHeadSipReply().getReply().getContent())
		int reply_id = _mess->getHeadSipReply().getReply().getReplyID();
		int code = _mess->getHeadSipReply().getReply().getCode();
		DEBOUT("SIPENGINE::reply type and code", reply_id << " " << code)

		//All replies must be considered
		sl_cc->p_w(_mess);

	}
	else {
		// purge it
		PURGEMESSAGE(_mess, "SIPENGINE::parse delete message")
	}
}
