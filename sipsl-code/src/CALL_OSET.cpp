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
#ifndef SIP_PROPERTIES_H
#include "SIP_PROPERTIES.h"
#endif
#ifndef ALARM_H
#include "ALARM.h"
#endif
//**********************************************************************************
//**********************************************************************************
// CALL_OSET
//**********************************************************************************
//**********************************************************************************
CALL_OSET::CALL_OSET(ENGINE* _engine){

	engine = _engine;

}
//**********************************************************************************
//**********************************************************************************
ENGINE* CALL_OSET::getENGINE(void){
	return engine;
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
//**********************************************************************************
//**********************************************************************************
SL_SM_CL* CALL_OSET::getSL_SM_CL(string _callidy){

	map<string, SL_SM_CL*>::iterator iter = mm_sl_sm_cl.find(_callidy);
    if( iter != mm_sl_sm_cl.end() ) {
		return iter->second;
    }
    else {
    	return 0x0;
    }
}
//**********************************************************************************
//**********************************************************************************
void CALL_OSET::addSL_SM_CL(string _callId_Y, SL_SM_CL* _sl_cl){

	mm_sl_sm_cl.insert(make_pair(_callId_Y,  _sl_cl));

	return;
}
//**********************************************************************************
//**********************************************************************************
string CALL_OSET::getCallIdX(void){
	return callId_X;
}

//**********************************************************************************
//**********************************************************************************
ALO* CALL_OSET::getALO(void){

	return alo;
}
//**********************************************************************************
//**********************************************************************************
SL_CO* CALL_OSET::getSL_CO(void){
	return sl_co;
}

//**********************************************************************************
//**********************************************************************************
// SL_CO
//**********************************************************************************
//**********************************************************************************
SL_CO::SL_CO(CALL_OSET* _call_oset){
	call_oset = _call_oset;
}
//**********************************************************************************
//**********************************************************************************
void SL_CO::call(MESSAGE* _message){

	DEBOUT("SL_CO::call", _message->getHeadSipRequest().getContent())
    DEBOUT("SL_CO::call", _message->getExtendedInternalCID())

    ACTION* action = 0x0;

	if (_message->getDestEntity() == SODE_SMSVPOINT) {
		DEBOUT("********************************************************************","")
		DEBOUT("*******************to server state machine**************************","")


		SL_SM_SV* sl_sm_sv = call_oset->getSL_SM_SV();

		action = sl_sm_sv->event(_message);

		if (action != 0x0){

			// now read actions
			stack<SingleAction> actionList;
			actionList = action->getActionList();

			while (!actionList.empty()){

				DEBOUT("SL_CO::reading action stack, message:", actionList.top().getMessage()->getIncBuffer())

				if (actionList.top().getDriver() == ACT_SEND && actionList.top().getMessage()->getDestEntity() == SODE_ALOPOINT){
					// send message to ALO

					DEBOUT("SL_CO::call action is send to ALO", actionList.top().getMessage()->getLine(0) << " ** " << actionList.top().getMessage()->getExtendedInternalCID())
					call_oset->getALO()->p_w(actionList.top().getMessage());

				}
				if (actionList.top().getDriver() == ACT_SEND && actionList.top().getMessage()->getDestEntity() == SODE_APOINT){
					ATRANSMIT(actionList.top().getMessage())
//					DEBOUT("SL_CO::call action is send to APOINT, string:", actionList.top().getMessage()->getIncBuffer().c_str())
//					sendto(actionList.top().getMessage()->getSock(),
//							actionList.top().getMessage()->getIncBuffer().c_str(),
//							actionList.top().getMessage()->getIncBuffer().length() , 0, (struct sockaddr *) &(actionList.top().getMessage()->getAddress()),
//							sizeof(actionList.top().getMessage()->getAddress()));

					//Purge message
					PURGEMESSAGE(actionList.top().getMessage(), "PURGE MESSAGE")
				} else {
					//TODO
					DEBOUT("SL_CO::call action is ???", "")
				}

				actionList.pop();

			}
		}
		else {
			DEBOUT("SL_CO::event", "action is null nothing, event ignored")
			PURGEMESSAGE(_message,"SL_SM_SV::delete message")
			return;
		}
	}
	else if (_message->getDestEntity() == SODE_SMCLPOINT){

		DEBOUT("********************************************************************","")
		DEBOUT("*******************to client state machine**************************","")

		string callidy = _message->getExtendedInternalCID();

		DEBOUT("Message to CL machine callidy", callidy)

		SL_SM_CL* sl_sm_cl = call_oset->getSL_SM_CL(callidy);

		if (sl_sm_cl == 0x0){

			//Client state machine does not exists
			//create and put in comap

			DEBOUT("Creating CL machine callidy", callidy)
			sl_sm_cl = new SL_SM_CL(call_oset->getENGINE(), _message);
			call_oset->addSL_SM_CL(callidy, sl_sm_cl);

			DEBOUT("Associating", callidy << " and " << call_oset->getCallIdX())
			SL_CC* tmp_sl_cc = (SL_CC*)call_oset->getENGINE();
			tmp_sl_cc->getCOMAP()->setY2XCallId(callidy,call_oset->getCallIdX());
		}

		ACTION* action = sl_sm_cl->event(_message);

		if (action != 0x0){

			// now read actions

			stack<SingleAction> actionList;
			actionList = action->getActionList();
			while (!actionList.empty()){

				DEBOUT("SL_CO::reading action stack, message:", actionList.top().getMessage()->getIncBuffer())

				if (actionList.top().getDriver() == ACT_SEND && actionList.top().getMessage()->getDestEntity() == SODE_BPOINT){

					DEBOUT("SL_CO::call action is send to B", actionList.top().getMessage()->getLine(0) << " ** " << actionList.top().getMessage()->getExtendedInternalCID())

					struct sockaddr_in si_bpart;
					memset((char *) &si_bpart, 0, sizeof(si_bpart));
					si_bpart.sin_family = AF_INET;
					//TODO ???
					// port nel TO o nella request
//					DEBOUT("create addess", actionList.top().getMessage()->getHeadTo().getContent())
//					DEBOUT("create addess 2", actionList.top().getMessage()->getHeadTo().getC_AttSipUri().getS_AttHostPort().getHostName());
//					DEBOUT("create addess 3", actionList.top().getMessage()->getHeadTo().getC_AttSipUri().getS_AttHostPort().getPort());

					si_bpart.sin_port = htons(actionList.top().getMessage()->getHeadTo().getC_AttSipUri().getS_AttHostPort().getPort());
					if (inet_aton(actionList.top().getMessage()->getHeadTo().getC_AttSipUri().getChangeS_AttHostPort().getHostName().c_str(), &si_bpart.sin_addr)==0){
						DEBASSERT("can't create b address")
					} else{

						BTRANSMIT(actionList.top().getMessage())

//						if (sendto(actionList.top().getMessage()->getSock(),
//							actionList.top().getMessage()->getIncBuffer().c_str(),
//							actionList.top().getMessage()->getIncBuffer().length() , 0, (struct sockaddr *)  &si_bpart,
//							sizeof(si_bpart)) == -1) {
//							DEBASSERT("not sent")}
					}
					//This INVITE belongs to ALO cannot be deleted here
					//but it can be a retransission. and so it's a copy.
					//DELETE INVITE HERE...
					PURGEMESSAGE(actionList.top().getMessage(), "PURGE INVITE")

				} else if (actionList.top().getDriver() == ACT_SEND && actionList.top().getMessage()->getDestEntity() == SODE_APOINT) {
					ATRANSMIT(actionList.top().getMessage())
//					DEBOUT("SL_CO::call action is send to APOINT, string:", actionList.top().getMessage()->getIncBuffer().c_str())
//					sendto(actionList.top().getMessage()->getSock(),
//							actionList.top().getMessage()->getIncBuffer().c_str(),
//							actionList.top().getMessage()->getIncBuffer().length() , 0, (struct sockaddr *) &(actionList.top().getMessage()->getAddress()),
//							sizeof(actionList.top().getMessage()->getAddress()));

					//Purge message
					PURGEMESSAGE(actionList.top().getMessage(), "PURGE MESSAGE")
				} else if (actionList.top().getDriver() == ACT_TIMERON){ // to alarm
					DEBOUT("SL_CO::call action is send to ALARM", actionList.top().getMessage()->getLine(0) << " ** " << actionList.top().getMessage()->getExtendedInternalCID())
					SysTime st1 = actionList.top().getMessage()->getFireTime();
					SysTime st2;
					st2.tv.tv_sec = 0;
					st2.tv.tv_usec = 0;
					actionList.top().getMessage()->setFireTime(st2);
					call_oset->getENGINE()->getSUDP()->getAlmgr()->insertAlarm(actionList.top().getMessage(),st1);

				} else if (actionList.top().getDriver() == ACT_TIMEROFF){
					DEBOUT("SL_CO::call action is clear ALARM", actionList.top().getMessage()->getLine(0) << " ** " << actionList.top().getMessage()->getExtendedInternalCID())
					string callid = actionList.top().getMessage()->getExtendedInternalCID();
					DEBOUT("SL_CO::cancel alarm, callid", callid)
					call_oset->getENGINE()->getSUDP()->getAlmgr()->cancelAlarm(callid);
				} else {
					//TODO
					DEBOUT("SL_CO::call action is ???", "")
				}
				actionList.pop();
			}
		}
		else {
			DEBOUT("SL_CO::event", "action is null nothing, event ignored")
			PURGEMESSAGE(_message, "SL_SM_SV::delete message")
			return;
		}
	}

	DEBOUT("SL_CO::call ended","")
	if (action != 0x0){
		delete action;
	}
}
//**********************************************************************************
//**********************************************************************************
SL_SM::SL_SM(ENGINE* _engine, MESSAGE* _message){
	sl_cc = _engine;
	messageGenerator = _message;
}
ENGINE* SL_SM::getSL_CC(void){
	return sl_cc;
}
MESSAGE* SL_SM::getGenerator(void){
	return messageGenerator;
}
SL_SM_SV::SL_SM_SV(ENGINE* _engine, MESSAGE* _message):
	SL_SM(_engine, _message){

	DEBOUT("SL_SM_SV::state","0")

	State = 0;

}

//**********************************************************************************
//**********************************************************************************
ACTION* SL_SM_SV::event(MESSAGE* _message){

	bool purgeMessage = false;

	DEBOUT("SL_SM_SV::event call id", _message->getHeadCallId().getContent())

	//TODO
	//Invert state and event for more readibility...

	if (_message->getReqRepType() == REQSUPP) {
		DEBOUT("SL_SM_SV::event REQSUPP", _message->getHeadSipRequest().getContent())

		if (State == 0){
			DEBOUT("SL_SM_SV::event" , "state 0")
			if ((_message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST)
				&& _message->getDestEntity() == SODE_SMSVPOINT
				&& _message->getGenEntity() ==  SODE_APOINT) {

				DEBOUT("SL_SM_SV::event move to state 1", _message->getHeadSipRequest().getContent())

				ACTION* action = new ACTION();

				//_message changes its dest and gen
				_message->setDestEntity(SODE_ALOPOINT);
				_message->setGenEntity(SODE_SMSVPOINT);
				SingleAction sa_1 = SingleAction(_message, ACT_SEND);

				//etry is filled later by SL_CO (see design)
				CREATEMESSAGE(etry, _message, SODE_SMSVPOINT)

				//TODO qui fare etry...
				DEBOUT("ETRY","SIP/2.0 100 Trying")
				etry->setHeadSipReply("SIP/2.0 100 Trying");
				DEBOUT("ETRY","Purge sdp")
				etry->purgeSDP();
				DEBOUT("ETRY","delete User-Agent:")
				etry->dropHeader("User-Agent:");
				DEBOUT("ETRY","delete Max-Forwards:")
				etry->removeMaxForwards();
				DEBOUT("ETRY","delete Content-Type:")
				etry->dropHeader("Content-Type:");
				DEBOUT("ETRY","delete Allow:")
				etry->dropHeader("Allow:");
				DEBOUT("ETRY","delete Route:")
				etry->dropHeader("Route:");
				DEBOUT("ETRY","delete Date:")
				etry->dropHeader("Date:");

//				DEBOUT("ETRY","delete Content-Length:")
//				etry->dropHeader("Content-Length:");
				etry->setGenericHeader("Content-Length:","0");
				//crash here...



				//via add rport
				C_HeadVia* viatmp = (C_HeadVia*) etry->getSTKHeadVia().top();
				DEBOUT("via1", viatmp->getC_AttVia().getContent())
				DEBOUT("via2", viatmp->getC_AttVia().getViaParms().findRvalue("rport"))
				//TODO 124??
				viatmp->getChangeC_AttVia().getChangeViaParms().replaceRvalue("rport", "124");
				//viatmp->getC_AttVia().getViaParms().compileTupleVector();
				DEBOUT("via3", viatmp->getC_AttVia().getViaParms().findRvalue("rport"))

				DEBOUT("via4", viatmp->getC_AttVia().getContent())
				etry->popSTKHeadVia();
				etry->pushHeadVia(viatmp->getC_AttVia().getContent());



				DEBOUT("ETRY","setDestEntity")
				etry->setDestEntity(SODE_APOINT);

				etry->compileMessage();
				etry->dumpVector();

				SingleAction sa_2 = SingleAction(etry, ACT_SEND);

				action->addSingleAction(sa_1);
				action->addSingleAction(sa_2);

				DEBOUT("SL_SM_SV::actions set", _message->getHeadSipRequest().getContent())

				State = 1;
				return action;
			}else {
				DEBOUT("SL_SM_SV::event State 0 unexpected message ignored", _message->getHeadCallId().getContent())
			}
		}else if (State == 1){
			DEBOUT("SL_SM_SV::event State 1 probable retransmission","")
		}
		return 0x0;

	}
	if (_message->getReqRepType() == REPSUPP) {
		DEBOUT("SL_SM_SV::event to be implemented", _message->getHeadSipReply().getContent())
		return 0x0;
	}

	//State = 0;

}
//**********************************************************************************
//**********************************************************************************
SL_SM_CL::SL_SM_CL(ENGINE* _engine, MESSAGE* _messgenerator):
	SL_SM(_engine, _messgenerator){

	DEBOUT("SL_SM_CL::state","0")

	resend_invite = 0;

	State = 0;

}
//**********************************************************************************
//**********************************************************************************
ACTION* SL_SM_CL::event(MESSAGE* _message){

	bool purgeMessage = false;

	//TODO qui
	// gestire quando arriva dal timer
	//

	DEBOUT("SL_SM_CL::event", _message->getHeadCallId().getContent())

	if (State == 0){
		if (_message->getReqRepType() == REQSUPP) {

			DEBOUT("SL_SM_CL::event", _message->getHeadSipRequest().getContent())
			DEBOUT("SL_SM_CL::event" , "state 0")

			if (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST
				&& _message->getDestEntity() == SODE_SMCLPOINT
				&& _message->getGenEntity() ==  SODE_ALOPOINT
				&& resend_invite < MAX_INVITE_RESEND) {

				DEBOUT("SL_SM_CL::event move to state 1", _message->getHeadSipRequest().getContent())

				ACTION* action = new ACTION();

				// create two actions:
				// send message
				// send message + timer
				// move to state 1

				// this is to be sent immediately
				_message->setDestEntity(SODE_BPOINT);
				_message->setGenEntity(SODE_SMCLPOINT);
				SysTime nowT;
				nowT.tv.tv_sec = 0;
				nowT.tv.tv_usec = 0;
				_message->setFireTime(nowT);

				SingleAction sa_1 = SingleAction(_message, ACT_SEND);

				//careful with source message.
				DUPLICATEMESSAGE(__message, _message, SODE_SMCLPOINT)

				//This is to be sent later after timer expires
				//the generating is to be set to ALO
				//so when sl_cc receives from timer it will resend it to
				//client state machine which
				//TODO review states and interaction...
				__message->setDestEntity(SODE_BPOINT);
				__message->setGenEntity(SODE_ALOPOINT);


				SysTime afterT;
				GETTIME(afterT);
				afterT.tv.tv_sec = afterT.tv.tv_sec + TIMER_1_sc*(resend_invite+1);
				afterT.tv.tv_usec = afterT.tv.tv_usec + TIMER_1_mc*(resend_invite+1);
				__message->setFireTime(afterT);
				SingleAction sa_2 = SingleAction(__message, ACT_TIMERON);

				action->addSingleAction(sa_1);
				action->addSingleAction(sa_2);

				DEBOUT("SL_SM_CL::actions set", _message->getHeadSipRequest().getContent())

				State = 1;
				resend_invite++;
				return action;

			}
		} else {
			//TODO purge state machine(?)
			return 0x0;
		}
	}
	if (State == 1){
		DEBOUT("SL_SM_CL::event" , "state 1")

		if (_message->getReqRepType() == REQSUPP) {

			if (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST
			&& _message->getDestEntity() == SODE_SMCLPOINT
			&& _message->getGenEntity() ==  SODE_ALOPOINT
			&& resend_invite < MAX_INVITE_RESEND) {

				DEBOUT("SL_SM_CL::resend" , resend_invite)

				DEBOUT("SL_SM_CL::event keep state 1", _message->getHeadSipRequest().getContent())

				ACTION* action = new ACTION();

				// create two actions:
				// send message
				// send message + timer
				// move to state 1

				// this is to be sent immediately
				_message->setDestEntity(SODE_BPOINT);
				_message->setGenEntity(SODE_SMCLPOINT);
				SysTime nowT;
				nowT.tv.tv_sec = 0;
				nowT.tv.tv_usec = 0;
				_message->setFireTime(nowT);

				SingleAction sa_1 = SingleAction(_message, ACT_SEND);

				//careful with source message.
				DUPLICATEMESSAGE(__message, _message, SODE_SMCLPOINT)

				//This is to be sent later after timer expires
				//the generating is to be set to ALO
				//so when sl_cc receives from timer it will resend it to
				//client state machine which
				//TODO review states and interaction...
				__message->setDestEntity(SODE_BPOINT);
				__message->setGenEntity(SODE_ALOPOINT);


				SysTime afterT;
				GETTIME(afterT);
				//TODO check if mc is overflowed
				afterT.tv.tv_sec = afterT.tv.tv_sec + TIMER_1_sc*(resend_invite+1);
				afterT.tv.tv_usec = afterT.tv.tv_usec + TIMER_1_mc*(resend_invite+1);
				__message->setFireTime(afterT);
				SingleAction sa_2 = SingleAction(__message, ACT_TIMERON);

				action->addSingleAction(sa_1);
				action->addSingleAction(sa_2);

				DEBOUT("SL_SM_CL::actions set", _message->getHeadSipRequest().getContent())

				State = 1;
				resend_invite++;
				return action;
			}
			else {
				return 0x0;
			}

		}
		if (_message->getReqRepType() == REPSUPP) {
			if (_message->getHeadSipReply().getReply().getCode() == TRYING_100
			&& _message->getDestEntity() == SODE_SMCLPOINT
			&& _message->getGenEntity() ==  SODE_BPOINT) {

				DEBOUT("SL_SM_CL::event state 1 try",  _message->getHeadSipReply().getReply().getCode() )

				// TODO clear timer ad create new timer for the ringing

				ACTION* action = new ACTION();
				SingleAction sa_1 = SingleAction(_message, ACT_TIMEROFF);
				action->addSingleAction(sa_1);

//				//TODO timer for ring


				State = 2;
				return action;
			}
		}
		else {
			return 0x0;
		}
	}
	if (State == 2){
		if (_message->getReqRepType() == REPSUPP) {
			if (_message->getHeadSipReply().getReply().getCode() == DIALOGE_101
			&& _message->getDestEntity() == SODE_SMCLPOINT
			&& _message->getGenEntity() ==  SODE_BPOINT) {

				DEBOUT("SL_SM_CL::event state 2 dialog est",  _message->getHeadSipReply().getReply().getCode() )

				ACTION* action = new ACTION();

				// TODO clear timer ad create new timer for the ringing

				// Dialog establish must derive from incoming invite
				// get incoming invite
				MESSAGE* __message = getGenerator();
				DEBOUT("MESSAGE GENERATOR", __message)
				CREATEMESSAGE(dialoge_x, __message, SODE_SMCLPOINT)
				dialoge_x->setDestEntity(SODE_APOINT);

				//TODO qui fare dialoge_x...
				DEBOUT("dialoge_x","SIP/2.0 101 Dialog Establishement")
				dialoge_x->setHeadSipReply("SIP/2.0 101 Dialog Establishement");
				DEBOUT("dialoge_x","Purge sdp")
				dialoge_x->purgeSDP();
				DEBOUT("dialoge_x","delete User-Agent:")
				dialoge_x->dropHeader("User-Agent:");
				DEBOUT("dialoge_x","delete Max-Forwards:")
				dialoge_x->removeMaxForwards();
				DEBOUT("dialoge_x","delete Allow:")
				dialoge_x->dropHeader("Allow:");
				DEBOUT("dialoge_x","delete Route:")
				dialoge_x->dropHeader("Route:");
				DEBOUT("dialoge_x","delete Date:")
				dialoge_x->dropHeader("Date:");

				dialoge_x->setGenericHeader("Content-Length:","0");
				//crash here...

				//via add rport
				DEBY
				C_HeadVia* viatmp = (C_HeadVia*) dialoge_x->getSTKHeadVia().top();
				DEBOUT("via1", viatmp->getC_AttVia().getContent())
				DEBOUT("via2", viatmp->getC_AttVia().getViaParms().findRvalue("rport"))
				//TODO 124??
				viatmp->getChangeC_AttVia().getChangeViaParms().replaceRvalue("rport", "124");
				//viatmp->getC_AttVia().getViaParms().compileTupleVector();
				DEBOUT("via3", viatmp->getC_AttVia().getViaParms().findRvalue("rport"))

				DEBOUT("via4", viatmp->getC_AttVia().getContent())
				dialoge_x->popSTKHeadVia();
				dialoge_x->pushHeadVia(viatmp->getC_AttVia().getContent());

				dialoge_x->compileMessage();
				dialoge_x->dumpVector();

				SingleAction sa_1 = SingleAction(dialoge_x, ACT_SEND);

				action->addSingleAction(sa_1);

				State = 2;
				return action;
			}
			else {
				DEBOUT("SL_SM_CL::event state 2 reply not implemented",  _message->getHeadSipReply().getReply().getCode() )
				State = 4;
				return (ACTION*)0x0;
			}
		}
	}
	return 0x0;
}
