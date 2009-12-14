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
CALL_OSET::CALL_OSET(ENGINE* _engine, MESSAGE* _genMessage){

	engine = _engine;
	genMessage = _genMessage;

}
MESSAGE* CALL_OSET::getGenMessage(void){
	return genMessage;
}

//**********************************************************************************
//**********************************************************************************
ENGINE* CALL_OSET::getENGINE(void){
	return engine;
}
//**********************************************************************************
//**********************************************************************************

void CALL_OSET::setSL_X(string _callId_X, SL_CO* _sl_co, SL_SM_SV2* _sl_sm_sv, ALO* _alo){

	callId_X = _callId_X;
	sl_co = _sl_co;
	sl_sm_sv = _sl_sm_sv;
	alo = _alo;

}
//**********************************************************************************
//**********************************************************************************
SL_SM_SV2* CALL_OSET::getSL_SM_SV(void){

	return sl_sm_sv;
}
//**********************************************************************************
//**********************************************************************************
SL_SM_CL2* CALL_OSET::getSL_SM_CL(string _callidy){

	map<string, SL_SM_CL2*>::iterator iter = mm_sl_sm_cl.find(_callidy);
    if( iter != mm_sl_sm_cl.end() ) {
		return iter->second;
    }
    else {
    	return 0x0;
    }
}
//**********************************************************************************
//**********************************************************************************
void CALL_OSET::addSL_SM_CL(string _callId_Y, SL_SM_CL2* _sl_cl){

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


		SL_SM_SV2* sl_sm_sv = call_oset->getSL_SM_SV();

		action = sl_sm_sv->event(_message);

		if (action != 0x0){

			// now get actions
			stack<SingleAction> actionList = action->getActionList();

			while (!actionList.empty()){

				MESSAGE* _tmpMessage = actionList.top().getMessage();

				DEBOUT("SL_CO::reading action stack server, message:", _tmpMessage->getIncBuffer())

				if (_tmpMessage->typeOfInternal == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_ALOPOINT){
					// send message to ALO

					DEBOUT("SL_CO::call action is send to ALO", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getExtendedInternalCID())
					call_oset->getALO()->p_w(_tmpMessage);

				}
				else if (_tmpMessage->typeOfInternal == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_APOINT){

					DEBY

					ATRANSMIT(_tmpMessage)
					//Purge message
					PURGEMESSAGE(_tmpMessage, "PURGE MESSAGE")

				} else {
					//TODO
					DEBASSERT("SL_CO::call action is ???")
				}

				actionList.pop();

			}
		}
		else {
			DEBOUT("SL_CO::event", "action is null nothing, event ignored")
			PURGEMESSAGE(_message,"SL_CO::delete message")
			//return;
		}
	}
	else if (_message->getDestEntity() == SODE_SMCLPOINT){

		DEBOUT("********************************************************************","")
		DEBOUT("*******************to client state machine**************************","")

		string callidy = _message->getExtendedInternalCID();

		DEBOUT("Message to CL machine callidy", callidy)

		SL_SM_CL2* sl_sm_cl = call_oset->getSL_SM_CL(callidy);

		if (sl_sm_cl == 0x0){

			//Client state machine does not exists
			//create and put in comap

			DEBOUT("Creating CL machine callidy", callidy)
			sl_sm_cl = new SL_SM_CL2(call_oset->getENGINE(), this);
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

				MESSAGE* _tmpMessage = actionList.top().getMessage();

				DEBOUT("SL_CO::reading action stack client, message:", _tmpMessage->getIncBuffer())

				if (_tmpMessage->typeOfInternal == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_BPOINT){

					DEBOUT("SL_CO::call action is send to B", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getExtendedInternalCID())

					BTRANSMIT(_tmpMessage)

					//DELETE INVITE HERE...
					PURGEMESSAGE(_tmpMessage, "PURGE INVITE")

				} else if (_tmpMessage->typeOfInternal == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_SMSVPOINT) {
					DEBOUT("CLIENT SM send to Server SM", _tmpMessage->getLine(0))
					DEBOUT("CLIENT SM send to Server SM 2",  _tmpMessage->getExtendedInternalCID())
					((SL_CC*)call_oset->getENGINE())->p_w(_tmpMessage);

				} else if (_tmpMessage->typeOfInternal == TYPE_OP){ // to alarm

					DEBOUT("SL_CO:: TYPE_OP","")

					if ( _tmpMessage->typeOfOperation == TYPE_OP_TIMER_ON){
						DEBOUT("SL_CO::call action is send to ALARM", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getExtendedInternalCID())
						SysTime st1 = _tmpMessage->getFireTime();
						SysTime st2;
						st2.tv.tv_sec = 0;
						st2.tv.tv_usec = 0;
						_tmpMessage->setFireTime(st2);
						call_oset->getENGINE()->getSUDP()->getAlmgr()->insertAlarm(_tmpMessage, st2);

					} else if (_tmpMessage->typeOfOperation == TYPE_OP_TIMER_OFF){

						DEBOUT("SL_CO::call action is clear ALARM", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getExtendedInternalCID())
						string callid = _tmpMessage->getExtendedInternalCID();
						DEBOUT("SL_CO::cancel alarm, callid", callid)
						call_oset->getENGINE()->getSUDP()->getAlmgr()->cancelAlarm(callid);
					}
					else {
						DEBASSERT("SL_CO client side inconsistency")
					}

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
			//return;
		}
	}

	DEBOUT("SL_CO::call ended","")
	if (action != 0x0){
		delete action;
	}
}
//**********************************************************************************
//**********************************************************************************
SL_SM2::SL_SM2(ENGINE* _engine, SL_CO* _sl_co){

	sl_co = _sl_co;
	sl_cc = _engine;

    pthread_mutex_init(&mutex, NULL);

}
ENGINE* SL_SM2::getSL_CC(void){
	return sl_cc;
}
SL_CO* SL_SM2::getSL_CO(void){
	return sl_co;
}
SL_SM_SV2::SL_SM_SV2(ENGINE* _engine,SL_CO* _sl_co):
	SL_SM2(_engine, _sl_co){

	DEBOUT("SL_SM_SV::state","0")

	State = 0;

}

//**********************************************************************************
//**********************************************************************************
ACTION* SL_SM_SV2::event(MESSAGE* _message){


	DEBOUT("SL_SM_SV::event call id", _message->getHeadCallId().getContent())

	//TODO
	//Invert state and event for more readibility...

	//Mutex this method
	pthread_mutex_lock(&mutex);


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
				_message->typeOfInternal = TYPE_MESS;
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
				etry->pushHeadVia("Via: " + viatmp->getC_AttVia().getContent());



				DEBOUT("ETRY","setDestEntity")
				etry->setDestEntity(SODE_APOINT);

				etry->compileMessage();
				etry->dumpVector();
				etry->typeOfInternal = TYPE_MESS;

				SingleAction sa_2 = SingleAction(etry);

				action->addSingleAction(sa_1);
				action->addSingleAction(sa_2);

				DEBOUT("SL_SM_SV::actions set", _message->getHeadSipRequest().getContent())

				State = 1;

				pthread_mutex_unlock(&mutex);
				return action;
			}else {
				DEBOUT("SL_SM_SV::event State 0 unexpected message ignored", _message->getHeadCallId().getContent())
			}
		}else if (State == 1){
			DEBOUT("SL_SM_SV::event State 1 probable retransmission","")
			ACTION* action = new ACTION();

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

			etry->setGenericHeader("Content-Length:","0");

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
			etry->pushHeadVia("Via: " + viatmp->getC_AttVia().getContent());



			DEBOUT("ETRY","setDestEntity")
			etry->setDestEntity(SODE_APOINT);

			etry->compileMessage();
			etry->dumpVector();
			etry->typeOfInternal = TYPE_MESS;

			SingleAction sa_1 = SingleAction(etry);

			action->addSingleAction(sa_1);
			pthread_mutex_unlock(&mutex);
			return action;

		}
		pthread_mutex_unlock(&mutex);
		return 0x0;
	}
	if (_message->getReqRepType() == REPSUPP) {
		if (State == 0){
			DEBOUT("SL_SM_SV::REPSUPP in state 0, ignore", _message->getLine(0))

			pthread_mutex_unlock(&mutex);
			return 0x0;

		} else if (State == 1) {

			if (_message->getHeadSipReply().getReply().getCode() == DIALOGE_101
					|| _message->getHeadSipReply().getReply().getCode() == RINGING_180
					|| _message->getHeadSipReply().getReply().getCode() == OK_200) {
				DEBOUT("SL_SM_SV::REPSUPP in state 1, DIALOGE or RINGING or OK arrived", _message->getLine(0))
				ACTION* action = new ACTION();
				_message->setDestEntity(SODE_APOINT);
				_message->setGenEntity(SODE_SMCLPOINT);
				_message->typeOfInternal = TYPE_MESS;

				SingleAction sa_1 = SingleAction(_message);
				action->addSingleAction(sa_1);
				pthread_mutex_unlock(&mutex);
				return action;
			}
			DEBOUT("SL_SM_SV::event to be implemented", _message->getHeadSipReply().getContent())
		}
		pthread_mutex_unlock(&mutex);
		return 0x0;
	}

	//State = 0;

}
//**********************************************************************************
//**********************************************************************************
SL_SM_CL2::SL_SM_CL2(ENGINE* _engine, SL_CO* _sl_co):
	SL_SM2(_engine, _sl_co){

	DEBOUT("SL_SM_CL::state","0")

	resend_invite = 0;

	State = 0;

}
//**********************************************************************************
//**********************************************************************************
ACTION* SL_SM_CL2::event(MESSAGE* _message){


	DEBOUT("SL_SM_CL::event ", _message->getLine(0)<< " ** " << _message->getHeadCallId().getContent())


	//Mutex this method
	pthread_mutex_lock(&mutex);

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
				_message->typeOfInternal = TYPE_MESS;

				SingleAction sa_1 = SingleAction(_message);

				//careful with source message.
				DUPLICATEMESSAGE(__message, _message, SODE_SMCLPOINT)

				//This is to be sent later after timer expires
				//the generating is to be set to ALO
				//so when sl_cc receives from timer it will resend it to
				//client state machine which
				//TODO review states and interaction...
				__message->setDestEntity(SODE_BPOINT);
				__message->setGenEntity(SODE_SMCLPOINT);


				SysTime afterT;
				GETTIME(afterT);
				afterT.tv.tv_sec = afterT.tv.tv_sec + TIMER_1_sc*(resend_invite+1);
				afterT.tv.tv_usec = afterT.tv.tv_usec + TIMER_1_mc*(resend_invite+1);
				__message->setFireTime(afterT);
				__message->typeOfInternal = TYPE_OP;
				__message->typeOfOperation = TYPE_OP_TIMER_ON;
				SingleAction sa_2 = SingleAction(__message);

				action->addSingleAction(sa_1);
				action->addSingleAction(sa_2);


				DEBOUT("SL_SM_CL::actions set", _message->getHeadSipRequest().getContent())

				State = 1;
				resend_invite++;
				pthread_mutex_unlock(&mutex);
				return action;

			}
		} else {
			//TODO purge state machine(?)
			pthread_mutex_unlock(&mutex);
			return 0x0;
		}
	}
	else if (State == 1){
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
				_message->typeOfInternal = TYPE_MESS;

				SingleAction sa_1 = SingleAction(_message);

				//careful with source message.
				DUPLICATEMESSAGE(__message, _message, SODE_SMCLPOINT)

				//This is to be sent later after timer expires
				//the generating is to be set to ALO
				//so when sl_cc receives from timer it will resend it to
				//client state machine which
				//TODO review states and interaction...
				__message->setDestEntity(SODE_BPOINT);
				__message->setGenEntity(SODE_SMCLPOINT);



				SysTime afterT;
				GETTIME(afterT);
				//TODO check if mc is overflowed
				afterT.tv.tv_sec = afterT.tv.tv_sec + TIMER_1_sc*(resend_invite+1);
				afterT.tv.tv_usec = afterT.tv.tv_usec + TIMER_1_mc*(resend_invite+1);
				__message->setFireTime(afterT);
				__message->typeOfInternal = TYPE_OP;
				__message->typeOfOperation = TYPE_OP_TIMER_ON;
				SingleAction sa_2 = SingleAction(__message);

				action->addSingleAction(sa_1);
				action->addSingleAction(sa_2);

				DEBOUT("SL_SM_CL::actions set", _message->getHeadSipRequest().getContent())

				State = 1;
				resend_invite++;
				pthread_mutex_unlock(&mutex);
				return action;
			}
			else {
				pthread_mutex_unlock(&mutex);
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
				_message->typeOfInternal = TYPE_OP;
				_message->typeOfOperation = TYPE_OP_TIMER_OFF;
				SingleAction sa_1 = SingleAction(_message);
				action->addSingleAction(sa_1);

//				//TODO timer for ring


				State = 2;
				pthread_mutex_unlock(&mutex);
				return action;
			}
		}
		else {
			pthread_mutex_unlock(&mutex);
			return 0x0;
		}
	}
	else if (State == 2){
		if (_message->getReqRepType() == REPSUPP) {
			if (_message->getHeadSipReply().getReply().getCode() == DIALOGE_101
			&& _message->getDestEntity() == SODE_SMCLPOINT
			&& _message->getGenEntity() ==  SODE_BPOINT) {

				DEBOUT("SL_SM_CL::event state 2 dialog est",  _message->getHeadSipReply().getReply().getCode() )

				ACTION* action = new ACTION();

				// TODO clear timer ad create new timer for the ringing

				// Dialog establish must derive from incoming invite
				// get incoming invite
				MESSAGE* __message = getSL_CO()->call_oset->getGenMessage();
				DEBOUT("MESSAGE GENERATOR", __message)
				CREATEMESSAGE(dialoge_x, __message, SODE_SMCLPOINT)
				//CREATENEWMESSAGE(dialoge_x, __message->getIncBuffer(),__message->getSock(), __message->getAddress(), SODE_SMCLPOINT)
				dialoge_x->setDestEntity(SODE_SMSVPOINT);
				dialoge_x->setGenEntity(SODE_SMCLPOINT);
				dialoge_x->typeOfInternal = TYPE_MESS;

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
				//TODO 124??
				DEBOUT("viatmp->getContent", viatmp->getContent())
				viatmp->getChangeC_AttVia().getChangeViaParms().replaceRvalue("rport", "124");
				dialoge_x->popSTKHeadVia();
				dialoge_x->pushHeadVia("Via: "+viatmp->getC_AttVia().getContent());

				dialoge_x->compileMessage();
				dialoge_x->dumpVector();

				C_HeadVia* viatmp2 = (C_HeadVia*) dialoge_x->getSTKHeadVia().top();

				SingleAction sa_1 = SingleAction(dialoge_x);

				action->addSingleAction(sa_1);

				State = 2;
				pthread_mutex_unlock(&mutex);
				return action;
			}
			else if (_message->getHeadSipReply().getReply().getCode() == RINGING_180
					&& _message->getDestEntity() == SODE_SMCLPOINT
					&& _message->getGenEntity() ==  SODE_BPOINT) {

				DEBOUT("SL_SM_CL::event state 2 ringing",  _message->getHeadSipReply().getReply().getCode() )
				ACTION* action = new ACTION();

				// TODO clear timer ad create new timer for the ringing

				// Dialog establish must derive from incoming invite
				// get incoming invite
				MESSAGE* __message = getSL_CO()->call_oset->getGenMessage();
				DEBOUT("MESSAGE GENERATOR", __message)
				CREATEMESSAGE(ring_x, __message, SODE_SMCLPOINT)
				ring_x->setDestEntity(SODE_SMSVPOINT);
				ring_x->setGenEntity(SODE_SMCLPOINT);
				ring_x->typeOfInternal = TYPE_MESS;

				//TODO qui fare dialoge_x...
				DEBOUT("ring_x","SIP/2.0 180 Ringnig")
				ring_x->setHeadSipReply("SIP/2.0 180 Ringing");
				DEBOUT("ring_x","Purge sdp")
				ring_x->purgeSDP();
				DEBOUT("ring_x","delete User-Agent:")
				ring_x->dropHeader("User-Agent:");
				DEBOUT("ring_x","delete Max-Forwards:")
				ring_x->removeMaxForwards();
				DEBOUT("ring_x","delete Allow:")
				ring_x->dropHeader("Allow:");
				DEBOUT("ring_x","delete Route:")
				ring_x->dropHeader("Route:");
				DEBOUT("ring_x","delete Date:")
				ring_x->dropHeader("Date:");

				ring_x->setGenericHeader("Content-Length:","0");
				//crash here...

				//via add rport
				DEBY
				C_HeadVia* viatmp = (C_HeadVia*) ring_x->getSTKHeadVia().top();
				//TODO 124??
				DEBOUT("viatmp->getContent", viatmp->getContent())
				viatmp->getChangeC_AttVia().getChangeViaParms().replaceRvalue("rport", "124");
				ring_x->popSTKHeadVia();
				ring_x->pushHeadVia("Via: "+viatmp->getC_AttVia().getContent());

				ring_x->compileMessage();
				ring_x->dumpVector();

				C_HeadVia* viatmp2 = (C_HeadVia*) ring_x->getSTKHeadVia().top();

				SingleAction sa_1 = SingleAction(ring_x);

				action->addSingleAction(sa_1);

				State = 4;
				pthread_mutex_unlock(&mutex);
				return action;

			}else {
				DEBOUT("SL_SM_CL::event state 2 reply not implemented",  _message->getHeadSipReply().getReply().getCode() )
				State = 4;
				pthread_mutex_unlock(&mutex);
				return (ACTION*)0x0;
			}
		}
	}
	else if (State == 4){
		if (_message->getReqRepType() == REPSUPP) {
			if (_message->getHeadSipReply().getReply().getCode() == OK_200
			&& _message->getDestEntity() == SODE_SMCLPOINT
			&& _message->getGenEntity() ==  SODE_BPOINT) {

				DEBOUT("SL_SM_CL::event state 2 ringing",  _message->getHeadSipReply().getReply().getCode() )
				ACTION* action = new ACTION();

				// TODO clear timer ad create new timer for the ringing

				// Dialog establish must derive from incoming invite
				// get incoming invite
				MESSAGE* __message = getSL_CO()->call_oset->getGenMessage();
				DEBOUT("MESSAGE GENERATOR", __message)
				CREATEMESSAGE(ok_x, __message, SODE_SMCLPOINT)
				ok_x->setDestEntity(SODE_SMSVPOINT);
				ok_x->setGenEntity(SODE_SMCLPOINT);
				ok_x->typeOfInternal = TYPE_MESS;

				//TODO qui fare dialoge_x...
				DEBOUT("ok_x","SIP/2.0 200 OK")
				ok_x->setHeadSipReply("SIP/2.0 200 OK");

				DEBOUT("ok_x","delete User-Agent:")
				ok_x->dropHeader("User-Agent:");
				DEBOUT("ok_x","delete Max-Forwards:")
				ok_x->removeMaxForwards();
				DEBOUT("ok_x","delete Allow:")
				ok_x->dropHeader("Allow:");
				DEBOUT("ok_x","delete Route:")
				ok_x->dropHeader("Route:");
				DEBOUT("ok_x","delete Date:")
				ok_x->dropHeader("Date:");

				ok_x->setGenericHeader("Content-Length:","0");
				//crash here...

				if (ok_x->getSDPSize() != 0 ){
					//SDP must copy the SDP from incoming OK and put here
					vector<string> __sdp = _message->getSDP();
					ok_x->purgeSDP();
					ok_x->importSDP(__sdp);
					char aaa[10];
					sprintf(aaa,"%d", _message->getSDPSize());
					ok_x->setGenericHeader("Content-Length:", aaa );
				}

				//via add rport
				DEBY
				C_HeadVia* viatmp = (C_HeadVia*) ok_x->getSTKHeadVia().top();
				//TODO 124??
				DEBOUT("viatmp->getContent", viatmp->getContent())
				viatmp->getChangeC_AttVia().getChangeViaParms().replaceRvalue("rport", "124");
				ok_x->popSTKHeadVia();
				ok_x->pushHeadVia("Via: "+viatmp->getC_AttVia().getContent());

				ok_x->compileMessage();
				ok_x->dumpVector();

				C_HeadVia* viatmp2 = (C_HeadVia*) ok_x->getSTKHeadVia().top();

				SingleAction sa_1 = SingleAction(ok_x);

				action->addSingleAction(sa_1);

				State = 4;
				pthread_mutex_unlock(&mutex);
				return action;

			}
		}
	}
	pthread_mutex_unlock(&mutex);
	return 0x0;
}
//**********************************************************************************
//**********************************************************************************
PREDICATE_ACTION::PREDICATE_ACTION(SL_SM* _sm){
	machine = _sm;
}
SL_SM::SL_SM(ENGINE* _eng, SL_CO* _sl_co){
    pthread_mutex_init(&mutex, NULL);
	State = 0;
}
//**********************************************************************************
//**********************************************************************************
void SL_SM::exec_it(MESSAGE* _event){

	PREDICATE_ACTION* tmp;

	DEBOUT("SM_SL::exec_it Look for state", State)

	pair<multimap<int,PREDICATE_ACTION*>::iterator,multimap<int,PREDICATE_ACTION*>::iterator> ret;

	multimap<int,PREDICATE_ACTION*>::iterator iter;

	ret = move_sm.equal_range(State);

	pthread_mutex_lock(&mutex);

    for (iter=ret.first; iter!=ret.second; ++iter){
		tmp  = iter->second;

		if (tmp->predicate(_event)){
			tmp->action(this);
			pthread_mutex_lock(&mutex);
			return;
		}
	}
	pthread_mutex_lock(&mutex);

	DEBOUT("SM_SL::exec_it unexpected event", _event)
}
//**********************************************************************************
//**********************************************************************************
void SL_SM::insert_move(int _i, PREDICATE_ACTION* _pa){

	DEBOUT("SM_SL::insert_move", _i << " " << _pa )
	move_sm.insert(pair<int, PREDICATE_ACTION*>(_i, _pa));

}
bool predicate_p1(MESSAGE* _s){
	if (true)
		return true;
	else
		return false;
};
void action_p1(SL_SM* _sm) {
	cout << "EVENT is aaa input, move to 1" <<endl;
	_sm->State = 1;
};
SL_SM_CL::SL_SM_CL(ENGINE* _eng, SL_CO* _sl_co):
		SL_SM(_eng, _sl_co),
		P1((SL_SM*)this){

	P1.action = &action_p1;
	P1.predicate = &predicate_p1;
}


