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

	DEBOUT("SL_CO::call", _message->getHeadSipRequest().getContent())
    DEBOUT("SL_CO::call", _message->getHeadCallId().getContent())


	if (_message->getDestEntity() == SODE_SMSVPOINT) {

		SL_SM_SV* sl_sm_sv = call_oset->getSL_SM_SV();

		ACTION* action = sl_sm_sv->event(_message);

		if (action != 0x0){

			// now act
			stack<SingleAction> actionList;
			actionList = action->getActionList();
			while (!actionList.empty()){
				DEBOUT("SL_CO::reading action stack", "")
				if (actionList.top().getMessage()->getDestEntity() == SODE_ALOPOINT){
					// send message to ALO

					DEBOUT("SL_CO::call action is ALO", actionList.top().getMessage()->getHeadSipRequest().getContent())
					call_oset->getALO()->p_w(actionList.top().getMessage());
				}
				if (actionList.top().getMessage()->getDestEntity() == SODE_APOINT){
					DEBOUT("ETRY ", actionList.top().getMessage()->getIncBuffer().c_str())
					sendto(actionList.top().getMessage()->getSock(),
							actionList.top().getMessage()->getIncBuffer().c_str(),
							actionList.top().getMessage()->getIncBuffer().length() , 0, (struct sockaddr *) &(actionList.top().getMessage()->getAddress()),
							sizeof(actionList.top().getMessage()->getAddress()));

					//Purge Etry
					PURGEMESSAGE(actionList.top().getMessage(), "PURGE TRY")
				}
				// TODO else...

				actionList.pop();
			}
		}
		else {
			DEBOUT("SL_CO::event", "action is null nothing, event ignored")
			//if (purgeMessage){
			PURGEMESSAGE(_message,"SL_SM_SV::delete message")
//				string key = _message->getKey();
//				pthread_mutex_lock(&messTableMtx);
//				DEBOUT("SL_SM_SV::delete message",key)
//				globalMessTable.erase(key);
//				delete _message;
//				pthread_mutex_unlock(&messTableMtx);
			//}
			return;
		}
	}
	else if (_message->getDestEntity() == SODE_SMCLPOINT){
		DEBOUT("********************************************************************","")
		DEBOUT("*******************to client state machine**************************","")

		//get new idy
		DEBOUT("_message->getHeadCallId().getNormCallId()", _message->getHeadCallId().getNormCallId())
		// fails
		DEBOUT("_message->getSTKHeadVia().top()", _message->getSTKHeadVia().top())
		DEBOUT("_message->getSTKHeadVia().top()->getC_AttVia().getViaParms().findRvalue(\"branch\")", _message->getSTKHeadVia().top()->getC_AttVia().getViaParms().findRvalue("branch"))

		string callidy = _message->getHeadCallId().getNormCallId() +
				_message->getSTKHeadVia().top()->getC_AttVia().getViaParms().findRvalue("branch");

		DEBOUT("Message to CL machine callidy", callidy)

		SL_SM_CL* sl_sm_cl = call_oset->getSL_SM_CL(callidy);

		if (sl_sm_cl == 0x0){
			DEBOUT("Creating CL machine callidy", callidy)
			sl_sm_cl = new SL_SM_CL(call_oset->getENGINE());
			call_oset->addSL_SM_CL(callidy, sl_sm_cl);
			DEBOUT("Associating", callidy << " and " << call_oset->getCallIdX())
			SL_CC* tmp_sl_cc = (SL_CC*)call_oset->getENGINE();
			tmp_sl_cc->getCOMAP()->setY2XCallId(callidy,call_oset->getCallIdX());
		}
		ACTION* action = sl_sm_cl->event(_message);

		if (action != 0x0){

			// now act
			stack<SingleAction> actionList;
			actionList = action->getActionList();
			while (!actionList.empty()){
				DEBOUT("SL_CO::reading action stack", "")
				if (actionList.top().getMessage()->getDestEntity() == SODE_BPOINT){


					if (actionList.top().getMessage()->getFireTime().tv.tv_sec == 0 &&
							actionList.top().getMessage()->getFireTime().tv.tv_usec == 0) {
						DEBOUT("SL_CO::destination is b", actionList.top().getMessage()->getHeadSipRequest().getContent())

						struct sockaddr_in si_bpart;
						memset((char *) &si_bpart, 0, sizeof(si_bpart));
						si_bpart.sin_family = AF_INET;
						//TODO ???
						// port nel TO o nella request
						DEBOUT("create addess", actionList.top().getMessage()->getHeadTo().getContent())
						DEBOUT("create addess 2", actionList.top().getMessage()->getHeadTo().getC_AttSipUri().getS_AttHostPort().getHostName());
						DEBOUT("create addess 3", actionList.top().getMessage()->getHeadTo().getC_AttSipUri().getS_AttHostPort().getPort());

						si_bpart.sin_port = htons(actionList.top().getMessage()->getHeadTo().getC_AttSipUri().getS_AttHostPort().getPort());
						if (inet_aton(actionList.top().getMessage()->getHeadTo().getC_AttSipUri().getChangeS_AttHostPort().getHostName().c_str(), &si_bpart.sin_addr)==0){
							DEBASSERT("can't create b address")
						} else{

							DEBOUT("sending", actionList.top().getMessage()->getIncBuffer())

							if (sendto(actionList.top().getMessage()->getSock(),
								actionList.top().getMessage()->getIncBuffer().c_str(),
								actionList.top().getMessage()->getIncBuffer().length() , 0, (struct sockaddr *)  &si_bpart,
								sizeof(si_bpart)) == -1) {
								DEBASSERT("not sent")
							}
							//DELETE INVITE HERE...
							PURGEMESSAGE(actionList.top().getMessage(), "PURGE INVITE")

						}
					} else { // to alarm
						DEBOUT("SL_CO::destination is ALARM", actionList.top().getMessage()->getHeadSipRequest().getContent())
						SysTime st1 = actionList.top().getMessage()->getFireTime();
						SysTime st2;
						st2.tv.tv_sec = 0;
						st2.tv.tv_usec = 0;
						actionList.top().getMessage()->setFireTime(st2);
						call_oset->getENGINE()->getSUDP()->getAlmgr()->insertAlarm(actionList.top().getMessage(),st1);
					}

				}
				// TODO else...
				actionList.pop();
			}
		}
		else {
			DEBOUT("SL_CO::event", "action is null nothing, event ignored")
			//if (purgeMessage){
			PURGEMESSAGE(_message, "SL_SM_SV::delete message")
//				string key = _message->getKey();
//				pthread_mutex_lock(&messTableMtx);
//				DEBOUT("SL_SM_SV::delete message",key)
//				globalMessTable.erase(key);
//				delete _message;
//				pthread_mutex_unlock(&messTableMtx);
			//}
			return;
		}
	}
	//do something with action
}
//**********************************************************************************
//**********************************************************************************
//ACTION SL_SM::event(MESSAGE* _message){
//	DEBOUT("SL_SM::event", _message->getIncBuffer())
//}

//**********************************************************************************
//**********************************************************************************
SL_SM::SL_SM(ENGINE* _engine){
	sl_cc = _engine;
}
ENGINE* SL_SM::getSL_CC(void){
	return sl_cc;
}
SL_SM_SV::SL_SM_SV(ENGINE* _engine):
	SL_SM(_engine){

	DEBOUT("SL_SM_SV::state","0")

	State = 0;

}
//**********************************************************************************
//**********************************************************************************
ACTION* SL_SM_SV::event(MESSAGE* _message){

	bool purgeMessage = false;

	DEBOUT("SL_SM_SV::event call id", _message->getHeadCallId().getContent())

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
				SingleAction sa_1 = SingleAction(_message);

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

				SingleAction sa_2 = SingleAction(etry);

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
		DEBOUT("SL_SM_SV::event", _message->getHeadSipReply().getContent())
	}

	State = 0;

}
//**********************************************************************************
//**********************************************************************************
SL_SM_CL::SL_SM_CL(ENGINE* _engine):
	SL_SM(_engine){

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

	if (_message->getReqRepType() == REQSUPP) {
		DEBOUT("SL_SM_CL::event", _message->getHeadSipRequest().getContent())
		if (State == 0){
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

				SingleAction sa_1 = SingleAction(_message);

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
				afterT.tv.tv_sec = afterT.tv.tv_sec + TIMER_1_sc;
				afterT.tv.tv_usec = afterT.tv.tv_usec + TIMER_1_mc;
				__message->setFireTime(afterT);
				SingleAction sa_2 = SingleAction(__message);

				action->addSingleAction(sa_1);
				action->addSingleAction(sa_2);

				DEBOUT("SL_SM_CL::actions set", _message->getHeadSipRequest().getContent())

				State = 1;
				resend_invite++;
				return action;

			}
		}
		if (State == 1){
			DEBOUT("SL_SM_CL::event" , "state 1")

			DEBOUT("SL_SM_CL::dest" , _message->getDestEntity())
			DEBOUT("SL_SM_CL::gen" , _message->getGenEntity())
			DEBOUT("SL_SM_CL::resend" , resend_invite)


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

				SingleAction sa_1 = SingleAction(_message);

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
				afterT.tv.tv_sec = afterT.tv.tv_sec + TIMER_1_sc;
				afterT.tv.tv_usec = afterT.tv.tv_usec + TIMER_1_mc;
				__message->setFireTime(afterT);
				SingleAction sa_2 = SingleAction(__message);

				action->addSingleAction(sa_1);
				action->addSingleAction(sa_2);

				DEBOUT("SL_SM_CL::actions set", _message->getHeadSipRequest().getContent())

				State = 1;
				resend_invite++;
				return action;

			}

		}


	}

}
