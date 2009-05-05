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
#ifndef SL_CC_H
#include "SL_CC.h"
#endif
#ifndef MESSAGE_H
#include "MESSAGE.h"
#endif
#ifndef COMAP_H
#include "COMAP.h"
#endif
#ifndef CALL_OSET_H
#include "CALL_OSET.h"
#endif
#ifndef ACTION_H
#include "ACTION.h"
#endif
#ifndef ALO_H
#include "ALO.h"
#endif

//**********************************************************************************
//**********************************************************************************
CALL_OSET::CALL_OSET(ENGINE* _engine){

	engine = _engine;

}
//**********************************************************************************
//**********************************************************************************
void CALL_OSET::setSL_X(string _callId_X, SL_CO* _sl_co, SL_SM_SV* _sl_sm_sv, ALO* _alo){

	callId_X = _callId_X;
	sl_co = _sl_co;
	sl_sm_sv = _sl_sm_sv;
	alo = _alo;

}
//**********************************************************************************
//**********************************************************************************
SL_SM_SV* CALL_OSET::getSL_SM_SV(void){

	return sl_sm_sv;
}
//		SL_CO* getSL_CO(void);
//
//		void addSL_SM_CL(string callId_Y, SL_SM_CL*);
//		SL_SM_CL* getSL_SM_SL(string callId_Y);
//		ALO* getALO(void);
//**********************************************************************************
//**********************************************************************************
SL_CO::SL_CO(CALL_OSET* _call_oset){
	call_oset = _call_oset;
}
//**********************************************************************************
//**********************************************************************************
void SL_CO::call(MESSAGE* _message){

	DEBOUT("SL_CO::call", _message->getIncBuffer())

	SL_SM_SV* sl_sm_sv = call_oset->getSL_SM_SV();

	ACTION* action = sl_sm_sv->event(_message);
}
//**********************************************************************************
//**********************************************************************************
//ACTION SL_SM::event(MESSAGE* _message){
//	DEBOUT("SL_SM::event", _message->getIncBuffer())
//}

//**********************************************************************************
//**********************************************************************************
SL_SM_SV::SL_SM_SV(void){

	DEBOUT("SL_SM_SV::state","0")

	State = 0;

}
//**********************************************************************************
//**********************************************************************************
ACTION* SL_SM_SV::event(MESSAGE* _message){

	DEBOUT("SL_SM_SV::event", _message->getHeadCallId().getContent())

	if (_message->getReqRepType() == REQSUPP) {
		DEBOUT("SL_SM_SV::event", _message->getHeadSipRequest().getContent())

		if (State == 0){
			if ((_message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST)
				&& _message->getDestEntity() == SODE_SMSVPOINT
				&& _message->getGenEntity() ==  SODE_APOINT) {

				DEBOUT("SL_SM_SV::event move to state 1", _message->getHeadSipRequest().getContent())

				ACTION* action = new ACTION();

				SingleAction sa_1 = SingleAction(_message);

				MESSAGE* etry = new MESSAGE(_message);

				//etry->


				SingleAction sa_2 = SingleAction(etry);

				State = 2;
			}
		}

	}
	if (_message->getReqRepType() == REPSUPP) {
		DEBOUT("SL_SM_SV::event", _message->getHeadSipReply().getContent())
	}

	State = 0;

}
