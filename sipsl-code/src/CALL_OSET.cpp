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
    DEBOUT("SL_CO::call", _message->getDialogExtendedCID())

    ACTION* action = 0x0;



	if (_message->getDestEntity() == SODE_SMSVPOINT) {
		DEBOUT("********************************************************************","")
		DEBOUT("*******************to server state machine**************************","")


		SL_SM_SV* sl_sm_sv = call_oset->getSL_SM_SV();

		action = sl_sm_sv->event(_message);

		if (action != 0x0){

			// now get actions
			stack<SingleAction> actionList = action->getActionList();

			while (!actionList.empty()){

				MESSAGE* _tmpMessage = actionList.top().getMessage();

				DEBOUT("SL_CO::reading action stack server, message:", _tmpMessage->getIncBuffer())

				//V3
				if (_tmpMessage->typeOfInternal == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_SMCLPOINT){

					DEBASSERT("should not happen")
					DEBOUT("SL_CO::call action is send to CL", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getDialogExtendedCID())
					call_oset->getENGINE()->p_w(_tmpMessage);

				}
				else if (_tmpMessage->typeOfInternal == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_ALOPOINT){
					// send message to ALO

					DEBOUT("SL_CO::call action is send to ALO", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getDialogExtendedCID())
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

		string callidy = _message->getDialogExtendedCID();

		DEBOUT("Message to CL machine callidy", callidy)

		SL_SM_CL* sl_sm_cl = call_oset->getSL_SM_CL(callidy);

		if (sl_sm_cl == 0x0){

			//Client state machine does not exists
			//create and put in comap

			DEBOUT("Creating CL machine callidy", callidy)
			sl_sm_cl = new SL_SM_CL(call_oset->getENGINE(), this);
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

				//V3
				if (_tmpMessage->typeOfInternal == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_ALOPOINT){
					// send message to ALO
					DEBOUT("SL_CO::call action is send to ALO", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getDialogExtendedCID())
					call_oset->getALO()->p_w(_tmpMessage);
				}
				//V3
				else if (_tmpMessage->typeOfInternal == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_BPOINT){

					DEBOUT("SL_CO::call action is send to B", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getDialogExtendedCID())

					BTRANSMIT(_tmpMessage)

					//DELETE INVITE HERE...
					PURGEMESSAGE(_tmpMessage, "PURGE INVITE")

				}
				//V3
				else if (_tmpMessage->typeOfInternal == TYPE_OP){ // to alarm

					DEBOUT("SL_CO:: TYPE_OP","")

					if ( _tmpMessage->typeOfOperation == TYPE_OP_TIMER_ON){
						DEBOUT("SL_CO::call action is send to ALARM", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getDialogExtendedCID())
						SysTime st1 = _tmpMessage->getFireTime();
						SysTime st2;
						st2.tv.tv_sec = 0;
						st2.tv.tv_usec = 0;
						_tmpMessage->setFireTime(st2);
						call_oset->getENGINE()->getSUDP()->getAlmgr()->insertAlarm(_tmpMessage, st2);

					} else if (_tmpMessage->typeOfOperation == TYPE_OP_TIMER_OFF){

						DEBOUT("SL_CO::call action is clear ALARM", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getDialogExtendedCID())
						string callid = _tmpMessage->getDialogExtendedCID();
						DEBOUT("SL_CO::cancel alarm, callid", callid)
						call_oset->getENGINE()->getSUDP()->getAlmgr()->cancelAlarm(callid);
					}
					else {
						DEBASSERT("SL_CO client side inconsistency")
					}

				}
				//V3
				else if (_tmpMessage->typeOfInternal == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_SMSVPOINT) {
					DEBOUT("CLIENT SM send to Server SM", _tmpMessage->getLine(0))
					DEBOUT("CLIENT SM send to Server SM 2",  _tmpMessage->getDialogExtendedCID())
					((SL_CC*)call_oset->getENGINE())->p_w(_tmpMessage);

				}

				else if (_tmpMessage->typeOfInternal == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_ALOPOINT){
					DEBOUT("SL_CO::call action is send to ALO", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getDialogExtendedCID())
					call_oset->getALO()->p_w(_tmpMessage);

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
		DEBOUT("SL_CO::call delete action","")
		delete action;
	}
}
//**********************************************************************************
//**********************************************************************************
PREDICATE_ACTION::PREDICATE_ACTION(SL_SM* _sm){
	machine = _sm;
}
//**********************************************************************************
//**********************************************************************************
ACTION* SL_SM::event(MESSAGE* _event){

	PREDICATE_ACTION* tmp;

	ACTION* act=0x0;

	DEBOUT("SM_SL::event Look for state", State)

	pair<multimap<int,PREDICATE_ACTION*>::iterator,multimap<int,PREDICATE_ACTION*>::iterator> ret;

	multimap<int,PREDICATE_ACTION*>::iterator iter;

	ret = move_sm.equal_range(State);

	pthread_mutex_lock(&mutex);

    for (iter=ret.first; iter!=ret.second; ++iter){
		tmp  = iter->second;

		if (tmp->predicate(this, _event)){
			act = tmp->action(this, _event);
			pthread_mutex_unlock(&mutex);
			return act;
		}
	}
	pthread_mutex_unlock(&mutex);
	return(act);

	DEBOUT("SM_SL::exec_it unexpected event", _event)
}
//**********************************************************************************
//**********************************************************************************
void SL_SM::insert_move(int _i, PREDICATE_ACTION* _pa){

	DEBOUT("SM_SL::insert_move", _i << " " << _pa )
	move_sm.insert(pair<int, PREDICATE_ACTION*>(_i, _pa));

}
SL_SM::SL_SM(ENGINE* _eng, SL_CO* _sl_co){

	sl_cc = _eng;
    sl_co = _sl_co;

    pthread_mutex_init(&mutex, NULL);
	State = 0;
}
ENGINE* SL_SM::getSL_CC(void){
	return sl_cc;
}
SL_CO* SL_SM::getSL_CO(void){
	return sl_co;
}

//**********************************************************************************
//**********************************************************************************
//bool predicate_p0_cl(SL_SM* _sm, MESSAGE* _message){
//
//	//if (State == 0){
//		if (_message->getReqRepType() == REQSUPP && _message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST
//				&& _message->getDestEntity() == SODE_SMCLPOINT
//				&& _message->getGenEntity() ==  SODE_ALOPOINT
//				&& ((SL_SM_CL*)_sm)->resend_invite < MAX_INVITE_RESEND) {
//			DEBOUT("predicate_p0_cl", "true")
//			return true;
//		}
//		else {
//			DEBOUT("predicate_p0_cl", "false")
//			return false;
//		}
//}
//ACTION* action_p0_cl(SL_SM* _sm, MESSAGE* _message) {
//
//	DEBOUT("SL_SM_CL::event move to state 1", _message->getHeadSipRequest().getContent())
//
//	ACTION* action = new ACTION();
//
//	// create two actions:
//	// send message
//	// send message + timer
//	// move to state 1
//
//	// this is to be sent immediately
//	_message->setDestEntity(SODE_BPOINT);
//	_message->setGenEntity(SODE_SMCLPOINT);
//	SysTime nowT;
//	nowT.tv.tv_sec = 0;
//	nowT.tv.tv_usec = 0;
//	_message->setFireTime(nowT);
//	_message->typeOfInternal = TYPE_MESS;
//
//	SingleAction sa_1 = SingleAction(_message);
//
//	//careful with source message.
//	DUPLICATEMESSAGE(__message, _message, SODE_SMCLPOINT)
//
//	//This is to be sent later after timer expires
//	//the generating is to be set to ALO
//	//so when sl_cc receives from timer it will resend it to
//	//client state machine which
//	//TODO review states and interaction...
//	__message->setDestEntity(SODE_BPOINT);
//	__message->setGenEntity(SODE_SMCLPOINT);
//
//
//	SysTime afterT;
//	GETTIME(afterT);
//	afterT.tv.tv_sec = afterT.tv.tv_sec + TIMER_1_sc*(((SL_SM_CL*)_sm)->resend_invite+1);
//	afterT.tv.tv_usec = afterT.tv.tv_usec + TIMER_1_mc*(((SL_SM_CL*)_sm)->resend_invite+1);
//	__message->setFireTime(afterT);
//	__message->typeOfInternal = TYPE_OP;
//	__message->typeOfOperation = TYPE_OP_TIMER_ON;
//	SingleAction sa_2 = SingleAction(__message);
//
//	action->addSingleAction(sa_1);
//	action->addSingleAction(sa_2);
//
//
//	DEBOUT("SL_SM_CL::actions set", _message->getHeadSipRequest().getContent())
//
//	_sm->State = 1;
//	((SL_SM_CL*)_sm)->resend_invite++;
//	return action;
//}
//bool predicate_p1a_cl(SL_SM* _sm, MESSAGE* _message){
//
//	//if (State == 1){
//	if (_message->getReqRepType() == REQSUPP
//		&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST
//		&& _message->getDestEntity() == SODE_SMCLPOINT
//		&& _message->getGenEntity() ==  SODE_ALOPOINT
//		&& ((SL_SM_CL*)_sm)->resend_invite < MAX_INVITE_RESEND) {
//		return true;
//	} else
//		return false;
//}
//ACTION* action_p1a_cl(SL_SM* _sm, MESSAGE* _message) {
//
//	DEBOUT("SL_SM_CL::resend" , ((SL_SM_CL*)_sm)->resend_invite)
//
//	DEBOUT("SL_SM_CL::event keep state 1", _message->getHeadSipRequest().getContent())
//
//	ACTION* action = new ACTION();
//
//	// create two actions:
//	// send message
//	// send message + timer
//	// move to state 1
//
//	// this is to be sent immediately
//	_message->setDestEntity(SODE_BPOINT);
//	_message->setGenEntity(SODE_SMCLPOINT);
//	SysTime nowT;
//	nowT.tv.tv_sec = 0;
//	nowT.tv.tv_usec = 0;
//	_message->setFireTime(nowT);
//	_message->typeOfInternal = TYPE_MESS;
//
//	SingleAction sa_1 = SingleAction(_message);
//
//	//careful with source message.
//	DUPLICATEMESSAGE(__message, _message, SODE_SMCLPOINT)
//
//	//This is to be sent later after timer expires
//	//the generating is to be set to ALO
//	//so when sl_cc receives from timer it will resend it to
//	//client state machine which
//	//TODO review states and interaction...
//	__message->setDestEntity(SODE_BPOINT);
//	__message->setGenEntity(SODE_SMCLPOINT);
//
//	SysTime afterT;
//	GETTIME(afterT);
//	//TODO check if mc is overflowed
//	afterT.tv.tv_sec = afterT.tv.tv_sec + TIMER_1_sc*(((SL_SM_CL*)_sm)->resend_invite+1);
//	afterT.tv.tv_usec = afterT.tv.tv_usec + TIMER_1_mc*(((SL_SM_CL*)_sm)->resend_invite+1);
//	__message->setFireTime(afterT);
//	__message->typeOfInternal = TYPE_OP;
//	__message->typeOfOperation = TYPE_OP_TIMER_ON;
//	SingleAction sa_2 = SingleAction(__message);
//
//	action->addSingleAction(sa_1);
//	action->addSingleAction(sa_2);
//
//	DEBOUT("SL_SM_CL::actions set", _message->getHeadSipRequest().getContent())
//
//	_sm->State = 1;
//	((SL_SM_CL*)_sm)->resend_invite++;
//	return action;
//
//}
//bool predicate_p1b_cl(SL_SM* _sm, MESSAGE* _message){
//
//	if (_message->getReqRepType() == REPSUPP
//		&&_message->getHeadSipReply().getReply().getCode() == TRYING_100
//		&& _message->getDestEntity() == SODE_SMCLPOINT
//		&& _message->getGenEntity() ==  SODE_BPOINT) {
//			return true;
//		}
//		else
//			return false;
//}
//ACTION* action_p1b_cl(SL_SM* _sm, MESSAGE* _message) {
//	DEBOUT("SL_SM_CL::event state 1 try",  _message->getHeadSipReply().getReply().getCode() )
//
//	// TODO clear timer ad create new timer for the ringing
//
//	ACTION* action = new ACTION();
//	_message->typeOfInternal = TYPE_OP;
//	_message->typeOfOperation = TYPE_OP_TIMER_OFF;
//	SingleAction sa_1 = SingleAction(_message);
//	action->addSingleAction(sa_1);
//
////				//TODO timer for ring
//
//	_sm->State = 2;
//	return action;
//}
//bool predicate_p2a_cl(SL_SM* _sm, MESSAGE* _message){
//	//else if (State == 2){
//	if (_message->getReqRepType() == REPSUPP
//		&& _message->getHeadSipReply().getReply().getCode() == DIALOGE_101
//		&& _message->getDestEntity() == SODE_SMCLPOINT
//		&& _message->getGenEntity() ==  SODE_BPOINT) {
//		return true;
//	} else
//		return false;
//
//}
//ACTION* action_p2a_cl(SL_SM* _sm, MESSAGE* _message) {
//
//	DEBOUT("SL_SM_CL::event state 2 dialog est",  _message->getHeadSipReply().getReply().getCode() )
//
//	ACTION* action = new ACTION();
//
//	// TODO clear timer ad create new timer for the ringing
//
//	// Dialog establish must derive from incoming invite
//	// get incoming invite
//	MESSAGE* __message = _sm->getSL_CO()->call_oset->getGenMessage();
//	DEBOUT("MESSAGE GENERATOR", __message)
//	CREATEMESSAGE(dialoge_x, __message, SODE_SMCLPOINT)
//	//CREATENEWMESSAGE(dialoge_x, __message->getIncBuffer(),__message->getSock(), __message->getAddress(), SODE_SMCLPOINT)
//	dialoge_x->setDestEntity(SODE_SMSVPOINT);
//	dialoge_x->setGenEntity(SODE_SMCLPOINT);
//	dialoge_x->typeOfInternal = TYPE_MESS;
//
//	//TODO qui fare dialoge_x...
//	DEBOUT("dialoge_x","SIP/2.0 101 Dialog Establishement")
//	dialoge_x->setHeadSipReply("SIP/2.0 101 Dialog Establishement");
//	DEBOUT("dialoge_x","Purge sdp")
//	dialoge_x->purgeSDP();
//	DEBOUT("dialoge_x","delete User-Agent:")
//	dialoge_x->dropHeader("User-Agent:");
//	DEBOUT("dialoge_x","delete Max-Forwards:")
//	dialoge_x->removeMaxForwards();
//	DEBOUT("dialoge_x","delete Allow:")
//	dialoge_x->dropHeader("Allow:");
//	DEBOUT("dialoge_x","delete Route:")
//	dialoge_x->dropHeader("Route:");
//	DEBOUT("dialoge_x","delete Date:")
//	dialoge_x->dropHeader("Date:");
//	DEBOUT("dialoge_x","delete Content-Type:")
//	dialoge_x->dropHeader("Content-Type:");
//
//
//	dialoge_x->setGenericHeader("Content-Length:","0");
//	//crash here...
//
//	//via add rport
//	DEBY
//	C_HeadVia* viatmp = (C_HeadVia*) dialoge_x->getSTKHeadVia().top();
//	//TODO 124??
//	DEBOUT("viatmp->getContent", viatmp->getContent())
//	viatmp->getChangeC_AttVia().getChangeViaParms().replaceRvalue("rport", "124");
//	dialoge_x->popSTKHeadVia();
//	dialoge_x->pushHeadVia("Via: "+viatmp->getC_AttVia().getContent());
//
//	dialoge_x->compileMessage();
//	dialoge_x->dumpVector();
//
//	C_HeadVia* viatmp2 = (C_HeadVia*) dialoge_x->getSTKHeadVia().top();
//
//	SingleAction sa_1 = SingleAction(dialoge_x);
//
//	action->addSingleAction(sa_1);
//
//	_sm->State = 2;
//	return action;
//}
//bool predicate_p2b_cl(SL_SM* _sm, MESSAGE* _message){
//	if (_message->getReqRepType() == REPSUPP
//			&& _message->getHeadSipReply().getReply().getCode() == RINGING_180
//			&& _message->getDestEntity() == SODE_SMCLPOINT
//			&& _message->getGenEntity() ==  SODE_BPOINT) {
//		return true;
//	}
//	else
//		return false;
//
//}
//ACTION* action_p2b_cl(SL_SM* _sm, MESSAGE* _message) {
//
//	DEBOUT("SL_SM_CL::event state 2 ringing",  _message->getHeadSipReply().getReply().getCode() )
//	ACTION* action = new ACTION();
//
//	// TODO clear timer ad create new timer for the ringing
//
//	// Dialog establish must derive from incoming invite
//	// get incoming invite
//	MESSAGE* __message = _sm->getSL_CO()->call_oset->getGenMessage();
//	DEBOUT("MESSAGE GENERATOR", __message)
//	CREATEMESSAGE(ring_x, __message, SODE_SMCLPOINT)
//	ring_x->setDestEntity(SODE_SMSVPOINT);
//	ring_x->setGenEntity(SODE_SMCLPOINT);
//	ring_x->typeOfInternal = TYPE_MESS;
//
//	//TODO qui fare dialoge_x...
//	DEBOUT("ring_x","SIP/2.0 180 Ringnig")
//	ring_x->setHeadSipReply("SIP/2.0 180 Ringing");
//	DEBOUT("ring_x","Purge sdp")
//	ring_x->purgeSDP();
//	DEBOUT("ring_x","delete User-Agent:")
//	ring_x->dropHeader("User-Agent:");
//	DEBOUT("ring_x","delete Max-Forwards:")
//	ring_x->removeMaxForwards();
//	DEBOUT("ring_x","delete Allow:")
//	ring_x->dropHeader("Allow:");
//	DEBOUT("ring_x","delete Route:")
//	ring_x->dropHeader("Route:");
//	DEBOUT("ring_x","delete Date:")
//	ring_x->dropHeader("Date:");
//	DEBOUT("ring_x","delete Content-Type:")
//	ring_x->dropHeader("Content-Type:");
//
//	ring_x->setGenericHeader("Content-Length:","0");
//	//crash here...
//
//	//via add rport
//	DEBY
//	C_HeadVia* viatmp = (C_HeadVia*) ring_x->getSTKHeadVia().top();
//	//TODO 124??
//	DEBOUT("viatmp->getContent", viatmp->getContent())
//	viatmp->getChangeC_AttVia().getChangeViaParms().replaceRvalue("rport", "124");
//	ring_x->popSTKHeadVia();
//	ring_x->pushHeadVia("Via: "+viatmp->getC_AttVia().getContent());
//
//	ring_x->compileMessage();
//	ring_x->dumpVector();
//
//	C_HeadVia* viatmp2 = (C_HeadVia*) ring_x->getSTKHeadVia().top();
//
//	SingleAction sa_1 = SingleAction(ring_x);
//
//	action->addSingleAction(sa_1);
//
//	_sm->State = 4;
//	return action;
//
//}
//bool predicate_p4_cl(SL_SM* _sm, MESSAGE* _message){
//
//	if (_message->getReqRepType() == REPSUPP
//		&& _message->getHeadSipReply().getReply().getCode() == OK_200
//		&& _message->getDestEntity() == SODE_SMCLPOINT
//		&& _message->getGenEntity() ==  SODE_BPOINT) {
//			return true;
//		} else
//			return false;
//
//}
//ACTION* action_p4_cl(SL_SM* _sm, MESSAGE* _message) {
//
//	DEBOUT("SL_SM_CL::event state 2 ringing",  _message->getHeadSipReply().getReply().getCode() )
//	ACTION* action = new ACTION();
//
//	// TODO clear timer ad create new timer for the ringing
//
//	// Dialog establish must derive from incoming invite
//	// get incoming invite
//	//200 ok shoudl be sent to ALO
//	MESSAGE* __message = _sm->getSL_CO()->call_oset->getGenMessage();
//	DEBOUT("MESSAGE GENERATOR", __message)
//	CREATEMESSAGE(ok_x, __message, SODE_SMCLPOINT)
//	ok_x->setDestEntity(SODE_ALOPOINT);
//	ok_x->setGenEntity(SODE_SMCLPOINT);
//	ok_x->typeOfInternal = TYPE_MESS;
//
//	//TODO qui fare dialoge_x...
//	DEBOUT("ok_x","SIP/2.0 200 OK")
//	ok_x->setHeadSipReply("SIP/2.0 200 OK");
//
//	DEBOUT("ok_x","delete User-Agent:")
//	ok_x->dropHeader("User-Agent:");
//	DEBOUT("ok_x","delete Max-Forwards:")
//	ok_x->removeMaxForwards();
//	DEBOUT("ok_x","delete Allow:")
//	ok_x->dropHeader("Allow:");
//	DEBOUT("ok_x","delete Route:")
//	ok_x->dropHeader("Route:");
//	DEBOUT("ok_x","delete Date:")
//	ok_x->dropHeader("Date:");
//
//
//	//via add rport
//	DEBY
//	C_HeadVia* viatmp = (C_HeadVia*) ok_x->getSTKHeadVia().top();
//	//TODO 124??
//	DEBOUT("viatmp->getContent", viatmp->getContent())
//	viatmp->getChangeC_AttVia().getChangeViaParms().replaceRvalue("rport", "124");
//	ok_x->popSTKHeadVia();
//	ok_x->pushHeadVia("Via: "+viatmp->getC_AttVia().getContent());
//
//	ok_x->setGenericHeader("Content-Length:","0");
//
//	if (ok_x->getSDPSize() != 0 ){
//		//SDP must copy the SDP from incoming OK and put here
//		vector<string> __sdp = _message->getSDP();
//		ok_x->purgeSDP();
//		DEBOUT("PURGED SDP","")
//		ok_x->dumpVector();
//		ok_x->importSDP(__sdp);
//		ok_x->dumpVector();
//	}
//
//
//	ok_x->compileMessage();
//	ok_x->dumpVector();
//
//	//C_HeadVia* viatmp2 = (C_HeadVia*) ok_x->getSTKHeadVia().top();
//
//	SingleAction sa_1 = SingleAction(ok_x);
//
//	action->addSingleAction(sa_1);
//
//	_sm->State = 4;
//	return action;
//
//}
//bool predicate_p4b_cl(SL_SM* _sm, MESSAGE* _message){
//
//	//if (State == 0){
//		if (_message->getReqRepType() == REQSUPP && _message->getHeadSipRequest().getS_AttMethod().getMethodID() == ACK_REQUEST
//				&& _message->getDestEntity() == SODE_SMCLPOINT
//				&& _message->getGenEntity() ==  SODE_ALOPOINT) {
//			DEBOUT("predicate_p4b_cl", "true")
//			return true;
//		}
//		else {
//			DEBOUT("predicate_p4b_cl", "false")
//			return false;
//		}
//}
//ACTION* action_p4b_cl(SL_SM* _sm, MESSAGE* _message) {
//
//	DEBOUT("SL_SM_CL::event move to state 1", _message->getHeadSipRequest().getContent())
//
//	ACTION* action = new ACTION();
//
//	_message->setDestEntity(SODE_BPOINT);
//	_message->setGenEntity(SODE_SMCLPOINT);
//	SysTime nowT;
//	nowT.tv.tv_sec = 0;
//	nowT.tv.tv_usec = 0;
//	_message->setFireTime(nowT);
//	_message->typeOfInternal = TYPE_MESS;
//
//	SingleAction sa_1 = SingleAction(_message);
//	action->addSingleAction(sa_1);
//
//	_sm->State = 5;
//	return action;
//}
//SL_SM_CL::SL_SM_CL(ENGINE* _eng, SL_CO* _sl_co):
//		SL_SM(_eng, _sl_co),
//		P0_CL((SL_SM*)this),
//		P1a_CL((SL_SM*)this),
//		P1b_CL((SL_SM*)this),
//		P2a_CL((SL_SM*)this),
//		P2b_CL((SL_SM*)this),
//		P4_CL((SL_SM*)this),
//		P4b_CL((SL_SM*)this){
//
//
//	resend_invite = 0;
//
//	P0_CL.action = &action_p0_cl;
//	P0_CL.predicate = &predicate_p0_cl;
//
//	P1a_CL.action = &action_p1a_cl;
//	P1a_CL.predicate = &predicate_p1a_cl;
//
//	P1b_CL.action = &action_p1b_cl;
//	P1b_CL.predicate = &predicate_p1b_cl;
//
//	P2a_CL.action = &action_p2a_cl;
//	P2a_CL.predicate = &predicate_p2a_cl;
//
//	P2b_CL.action = &action_p2b_cl;
//	P2b_CL.predicate = &predicate_p2b_cl;
//
//	P4_CL.action = &action_p4_cl;
//	P4_CL.predicate = &predicate_p4_cl;
//
//	P4b_CL.action = &action_p4b_cl;
//	P4b_CL.predicate = &predicate_p4b_cl;
//
//
//	insert_move(0,&P0_CL);
//	insert_move(1,&P1a_CL);
//	insert_move(1,&P1b_CL);
//	insert_move(2,&P2a_CL);
//	insert_move(2,&P2b_CL);
//	insert_move(4,&P4_CL);
//	insert_move(4,&P4b_CL);
//
//
//}
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
////begin State machine V2
//bool predicate_p0_sv(SL_SM* _sm, MESSAGE* _message){
//
//	if (_message->getReqRepType() == REQSUPP
//			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST
//			&& _message->getDestEntity() == SODE_SMSVPOINT
//			&& _message->getGenEntity() ==  SODE_APOINT) {
//		return true;
//	}
//	else
//		return false;
//}
//ACTION* action_p0_sv(SL_SM* _sm, MESSAGE* _message) {
//
//	DEBOUT("SL_SM_SV::event move to state 1", _message->getHeadSipRequest().getContent())
//
//	ACTION* action = new ACTION();
//
//	//_message changes its dest and gen
//	_message->setDestEntity(SODE_ALOPOINT);
//	_message->setGenEntity(SODE_SMSVPOINT);
//	_message->typeOfInternal = TYPE_MESS;
//	SingleAction sa_1 = SingleAction(_message);
//
//	//etry is filled later by SL_CO (see design)
//	CREATEMESSAGE(etry, _message, SODE_SMSVPOINT)
//
//	//TODO qui fare etry...
//	DEBOUT("ETRY","SIP/2.0 100 Trying")
//	etry->setHeadSipReply("SIP/2.0 100 Trying");
//	DEBOUT("ETRY","Purge sdp")
//	etry->purgeSDP();
//	DEBOUT("ETRY","delete User-Agent:")
//	etry->dropHeader("User-Agent:");
//	DEBOUT("ETRY","delete Max-Forwards:")
//	etry->removeMaxForwards();
//	DEBOUT("ETRY","delete Content-Type:")
//	etry->dropHeader("Content-Type:");
//	DEBOUT("ETRY","delete Allow:")
//	etry->dropHeader("Allow:");
//	DEBOUT("ETRY","delete Route:")
//	etry->dropHeader("Route:");
//	DEBOUT("ETRY","delete Date:")
//	etry->dropHeader("Date:");
//
////				DEBOUT("ETRY","delete Content-Length:")
////				etry->dropHeader("Content-Length:");
//	etry->setGenericHeader("Content-Length:","0");
//	//crash here...
//
//
//
//	//via add rport
//	C_HeadVia* viatmp = (C_HeadVia*) etry->getSTKHeadVia().top();
//	DEBOUT("via1", viatmp->getC_AttVia().getContent())
//	DEBOUT("via2", viatmp->getC_AttVia().getViaParms().findRvalue("rport"))
//	//TODO 124??
//	viatmp->getChangeC_AttVia().getChangeViaParms().replaceRvalue("rport", "124");
//	//viatmp->getC_AttVia().getViaParms().compileTupleVector();
//	DEBOUT("via3", viatmp->getC_AttVia().getViaParms().findRvalue("rport"))
//
//	DEBOUT("via4", viatmp->getC_AttVia().getContent())
//	etry->popSTKHeadVia();
//	etry->pushHeadVia("Via: " + viatmp->getC_AttVia().getContent());
//
//
//
//	DEBOUT("ETRY","setDestEntity")
//	etry->setDestEntity(SODE_APOINT);
//
//	etry->compileMessage();
//	etry->dumpVector();
//	etry->typeOfInternal = TYPE_MESS;
//
//	SingleAction sa_2 = SingleAction(etry);
//
//	action->addSingleAction(sa_1);
//	action->addSingleAction(sa_2);
//
//	DEBOUT("SL_SM_SV::actions set", _message->getHeadSipRequest().getContent())
//
//	_sm->State = 1;
//
//	return action;
//
//}
//
//bool predicate_p1a_sv(SL_SM* _sm, MESSAGE* _message){
//
//	if (_message->getReqRepType() == REQSUPP
//			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST
//			&& _message->getDestEntity() == SODE_SMSVPOINT
//			&& _message->getGenEntity() ==  SODE_APOINT) {
//		return true;
//	}
//	else
//		return false;
//}
//ACTION* action_p1a_sv(SL_SM* _sm, MESSAGE* _message) {
//
//	DEBOUT("SL_SM_SV::event State 1 probable retransmission","")
//	ACTION* action = new ACTION();
//
//	CREATEMESSAGE(etry, _message, SODE_SMSVPOINT)
//
//	//TODO qui fare etry...
//	DEBOUT("ETRY","SIP/2.0 100 Trying")
//	etry->setHeadSipReply("SIP/2.0 100 Trying");
//	DEBOUT("ETRY","Purge sdp")
//	etry->purgeSDP();
//	DEBOUT("ETRY","delete User-Agent:")
//	etry->dropHeader("User-Agent:");
//	DEBOUT("ETRY","delete Max-Forwards:")
//	etry->removeMaxForwards();
//	DEBOUT("ETRY","delete Content-Type:")
//	etry->dropHeader("Content-Type:");
//	DEBOUT("ETRY","delete Allow:")
//	etry->dropHeader("Allow:");
//	DEBOUT("ETRY","delete Route:")
//	etry->dropHeader("Route:");
//	DEBOUT("ETRY","delete Date:")
//	etry->dropHeader("Date:");
//
//	etry->setGenericHeader("Content-Length:","0");
//
//	//via add rport
//	C_HeadVia* viatmp = (C_HeadVia*) etry->getSTKHeadVia().top();
//	DEBOUT("via1", viatmp->getC_AttVia().getContent())
//	DEBOUT("via2", viatmp->getC_AttVia().getViaParms().findRvalue("rport"))
//	//TODO 124??
//	viatmp->getChangeC_AttVia().getChangeViaParms().replaceRvalue("rport", "124");
//	//viatmp->getC_AttVia().getViaParms().compileTupleVector();
//	DEBOUT("via3", viatmp->getC_AttVia().getViaParms().findRvalue("rport"))
//
//	DEBOUT("via4", viatmp->getC_AttVia().getContent())
//	etry->popSTKHeadVia();
//	etry->pushHeadVia("Via: " + viatmp->getC_AttVia().getContent());
//
//
//
//	DEBOUT("ETRY","setDestEntity")
//	etry->setDestEntity(SODE_APOINT);
//
//	etry->compileMessage();
//	etry->dumpVector();
//	etry->typeOfInternal = TYPE_MESS;
//
//	SingleAction sa_1 = SingleAction(etry);
//
//	action->addSingleAction(sa_1);
//	return action;
//
//}
//bool predicate_p1b_sv(SL_SM* _sm, MESSAGE* _message){
//
//	if (_message->getReqRepType() == REPSUPP
//			&& (_message->getHeadSipReply().getReply().getCode() == DIALOGE_101
//			|| _message->getHeadSipReply().getReply().getCode() == RINGING_180
//			|| _message->getHeadSipReply().getReply().getCode() == OK_200)) {
//		return true;
//	} else
//		return false;
//}
//
//ACTION* action_p1b_sv(SL_SM* _sm, MESSAGE* _message) {
//
//	DEBOUT("SL_SM_SV::REPSUPP in state 1, DIALOGE or RINGING or OK arrived", _message->getLine(0))
//	ACTION* action = new ACTION();
//	_message->setDestEntity(SODE_APOINT);
//	_message->setGenEntity(SODE_SMCLPOINT);
//	_message->typeOfInternal = TYPE_MESS;
//
//	SingleAction sa_1 = SingleAction(_message);
//	action->addSingleAction(sa_1);
//	return action;
//
//}
//bool predicate_p1c_sv(SL_SM* _sm, MESSAGE* _message){
//
//	if (_message->getReqRepType() == REQSUPP
//			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == ACK_REQUEST
//			&& _message->getDestEntity() == SODE_SMSVPOINT
//			&& _message->getGenEntity() ==  SODE_APOINT) {
//		return true;
//	}
//	else
//		return false;
//}
//ACTION* action_p1c_sv(SL_SM* _sm, MESSAGE* _message) {
//
//	DEBOUT("SL_SM_SV::event move to state 1", _message->getHeadSipRequest().getContent())
//
//	ACTION* action = new ACTION();
//
//	//_message changes its dest and gen
//	_message->setDestEntity(SODE_ALOPOINT);
//	_message->setGenEntity(SODE_SMSVPOINT);
//	_message->typeOfInternal = TYPE_MESS;
//	SingleAction sa_1 = SingleAction(_message);
//
//	action->addSingleAction(sa_1);
//
//	DEBOUT("SL_SM_SV::actions set", _message->getHeadSipRequest().getContent())
//
//	_sm->State = 2;
//
//	return action;
//
//}
//SL_SM_SV::SL_SM_SV(ENGINE* _eng, SL_CO* _sl_co):
//		SL_SM(_eng, _sl_co),
//		P0_SV((SL_SM*)this),
//		P1a_SV((SL_SM*)this),
//		P1b_SV((SL_SM*)this),
//		P1c_SV((SL_SM*)this){
//
//	// 0 (INVITE) ----> 1
//	// 1 (ACK)    ----> 2
//	P0_SV.action = &action_p0_sv;
//	P0_SV.predicate = &predicate_p0_sv;
//
//	P1a_SV.action = &action_p1a_sv;
//	P1a_SV.predicate = &predicate_p1a_sv;
//
//	P1b_SV.action = &action_p1b_sv;
//	P1b_SV.predicate = &predicate_p1b_sv;
//
//	P1c_SV.action = &action_p1c_sv;
//	P1c_SV.predicate = &predicate_p1c_sv;
//
//	insert_move(0,&P0_SV);
//	insert_move(1,&P1a_SV);
//	insert_move(1,&P1b_SV);
//	insert_move(1,&P1c_SV);
//
//}
//V3
bool pre_0_1_sv(SL_SM* _sm, MESSAGE* _message){

	DEBOUT("SM_SV pre_0_1_sv","called")
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST
			&& _message->getDestEntity() == SODE_SMSVPOINT
			&& _message->getGenEntity() ==  SODE_APOINT) {
		DEBOUT("SM_SV pre_0_1_sv","true")
		return true;
	}
	else {
		DEBOUT("SM_SV pre_0_1_sv","false")
		return false;
	}
}
ACTION* act_0_1_sv(SL_SM* _sm, MESSAGE* _message) {

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

	_sm->State = 1;

	return action;

}
//V3
//will do pre_2_2_sv also
bool pre_1_2_sv(SL_SM* _sm, MESSAGE* _message){

	DEBOUT("SM_SV pre_1_2_sv","")

	if (_message->getReqRepType() == REPSUPP
		&& (_message->getHeadSipReply().getReply().getCode() == DIALOGE_101
				|| _message->getHeadSipReply().getReply().getCode() == RINGING_180)
		&& _message->getDestEntity() == SODE_SMSVPOINT
		&& _message->getGenEntity() ==  SODE_SMCLPOINT) {
			return true;
			DEBOUT("SM_SV pre_1_2_sv","true")
		}
		else {
			DEBOUT("SM_SV pre_1_2_sv","false")
			return false;
		}
}
ACTION* act_1_2_sv(SL_SM* _sm, MESSAGE* _message) {

	DEBOUT("SL_SM_SV::act_1_2_sv",  _message->getHeadSipReply().getReply().getCode() )
	ACTION* action = new ACTION();

	_message->setDestEntity(SODE_APOINT);
	_message->setGenEntity(SODE_SMSVPOINT);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	_sm->State = 2;

	return action;
}
//V3
//same for 2->3
bool pre_1_3_sv(SL_SM* _sm, MESSAGE* _message){

	DEBOUT("SM_SV pre_1_3_sv","")

	if (_message->getReqRepType() == REPSUPP
		&& _message->getHeadSipReply().getReply().getCode() == OK_200
		&& _message->getDestEntity() == SODE_SMSVPOINT
		&& _message->getGenEntity() ==  SODE_ALOPOINT) {
			DEBOUT("SM_SV pre_2_3_sv","true")
			return true;
		}
		else {
			DEBOUT("SM_SV pre_2_3_sv","false")
			return false;
		}
}
ACTION* act_1_3_sv(SL_SM* _sm, MESSAGE* _message) {

	DEBOUT("SM_SV act_2_3_sv","")

	ACTION* action = new ACTION();

	_message->setDestEntity(SODE_APOINT);
	_message->setGenEntity(SODE_SMSVPOINT);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	//TODO
	// start here timer for resend 200OK until ACK
	// arrives


	_sm->State = 3;

	return action;

}

SL_SM_SV::SL_SM_SV(ENGINE* _eng, SL_CO* _sl_co):
		SL_SM(_eng, _sl_co),
		P0_1SV((SL_SM*)this),
		P1_2SV((SL_SM*)this),
		P2_2SV((SL_SM*)this),
		P2_3SV((SL_SM*)this),
		P1_3SV((SL_SM*)this){

	P0_1SV.action = &act_0_1_sv;
	P0_1SV.predicate = &pre_0_1_sv;

	P1_2SV.action = &act_1_2_sv;
	P1_2SV.predicate = &pre_1_2_sv;

	P1_3SV.action = &act_1_3_sv;
	P1_3SV.predicate = &pre_1_3_sv;

	P2_2SV.action = &act_1_2_sv;
	P2_2SV.predicate = &pre_1_2_sv;


	P2_3SV.action = &act_1_3_sv;
	P2_3SV.predicate = &pre_1_3_sv;


	insert_move(0,&P0_1SV);
	insert_move(1,&P1_2SV);
	insert_move(1,&P1_3SV);
	insert_move(2,&P2_2SV);
	insert_move(2,&P2_3SV);
}
//*****************************************************************
// client sm
//*****************************************************************
bool pre_0_1_cl(SL_SM* _sm, MESSAGE* _message){

	DEBOUT("SM_CL pre_0_1_cl","")
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST
			&& _message->getDestEntity() == SODE_SMCLPOINT
			&& _message->getGenEntity() ==  SODE_ALOPOINT) {
		DEBOUT("SM_CL pre_0_1_cl","true")
		return true;
	}
	else {
		DEBOUT("SM_CL pre_0_1_cl","false")
		return false;
	}
}
ACTION* act_0_1_cl(SL_SM* _sm, MESSAGE* _message) {

	DEBOUT("SM_CL act_0_1_cl","")

	ACTION* action = new ACTION();

	_message->setDestEntity(SODE_BPOINT);
	_message->setGenEntity(SODE_SMCLPOINT);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

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
	afterT.tv.tv_sec = afterT.tv.tv_sec + TIMER_1_sc*(((SL_SM_CL*)_sm)->resend_invite+1);
	afterT.tv.tv_usec = afterT.tv.tv_usec + TIMER_1_mc*(((SL_SM_CL*)_sm)->resend_invite+1);
	__message->setFireTime(afterT);
	__message->typeOfInternal = TYPE_OP;
	__message->typeOfOperation = TYPE_OP_TIMER_ON;
	SingleAction sa_2 = SingleAction(__message);

	action->addSingleAction(sa_2);

	((SL_SM_CL*)_sm)->resend_invite++;

	_sm->State = 1;

	return action;

}
bool pre_1_2_cl(SL_SM* _sm, MESSAGE* _message){

	DEBOUT("SM_CL pre_1_2_cl","")

	if (_message->getReqRepType() == REPSUPP
		&&_message->getHeadSipReply().getReply().getCode() == TRYING_100
		&& _message->getDestEntity() == SODE_SMCLPOINT
		&& _message->getGenEntity() ==  SODE_BPOINT) {
			DEBOUT("SM_CL pre_1_2_cl","true")
			return true;
		}
		else {
			DEBOUT("SM_CL pre_1_2_cl","false")
			return false;
		}
}
ACTION* act_1_2_cl(SL_SM* _sm, MESSAGE* _message) {

	DEBOUT("SM_CL act_1_2_cl",_message->getHeadSipReply().getReply().getCode())

	// TODO clear timer ad create new timer for the ringing ?

	ACTION* action = new ACTION();
	_message->typeOfInternal = TYPE_OP;
	_message->typeOfOperation = TYPE_OP_TIMER_OFF;
	SingleAction sa_1 = SingleAction(_message);
	action->addSingleAction(sa_1);

	_sm->State = 2;
	return action;
}
//same for 2->3
bool pre_1_3_cl(SL_SM* _sm, MESSAGE* _message){

	DEBOUT("SM_CL pre_1_3_cl","")

	if (_message->getReqRepType() == REPSUPP
		&& (_message->getHeadSipReply().getReply().getCode() == DIALOGE_101
				|| _message->getHeadSipReply().getReply().getCode() == RINGING_180)
		&& _message->getDestEntity() == SODE_SMCLPOINT
		&& _message->getGenEntity() ==  SODE_BPOINT) {
			DEBOUT("SM_CL pre_1_3_cl","true")
			return true;
		}
		else {
			DEBOUT("SM_CL pre_1_3_cl","false")
			return false;
		}
}
ACTION* act_1_3_cl(SL_SM* _sm, MESSAGE* _message) {

	DEBOUT("SM_CL act_1_3_cl","")

	ACTION* action = new ACTION();


	MESSAGE* __message = _sm->getSL_CO()->call_oset->getGenMessage();
	DEBOUT("MESSAGE GENERATOR", __message)
	CREATEMESSAGE(reply_x, __message, SODE_SMSVPOINT)
	reply_x->setDestEntity(SODE_SMSVPOINT);
	reply_x->setGenEntity(SODE_SMCLPOINT);
	reply_x->typeOfInternal = TYPE_MESS;

	//TODO qui fare dialoge_x...
	DEBOUT("reply_x","SIP/2.0 " << _message->getHeadSipReply().getContent())
	reply_x->setHeadSipReply(_message->getHeadSipReply().getContent());
	DEBOUT("reply_x","Purge sdp")
	reply_x->purgeSDP();
	DEBOUT("reply_x","delete User-Agent:")
	reply_x->dropHeader("User-Agent:");
	DEBOUT("reply_x","delete Max-Forwards:")
	reply_x->removeMaxForwards();
	DEBOUT("reply_x","delete Allow:")
	reply_x->dropHeader("Allow:");
	DEBOUT("reply_x","delete Route:")
	reply_x->dropHeader("Route:");
	DEBOUT("reply_x","delete Date:")
	reply_x->dropHeader("Date:");
	DEBOUT("reply_x","delete Content-Type:")
	reply_x->dropHeader("Content-Type:");

	reply_x->setGenericHeader("Content-Length:","0");
	//crash here...

	//via add rport
	DEBY
	C_HeadVia* viatmp = (C_HeadVia*) reply_x->getSTKHeadVia().top();
	//TODO 124??
	DEBOUT("viatmp->getContent", viatmp->getContent())
	viatmp->getChangeC_AttVia().getChangeViaParms().replaceRvalue("rport", "124");
	reply_x->popSTKHeadVia();
	reply_x->pushHeadVia("Via: "+viatmp->getC_AttVia().getContent());

	reply_x->compileMessage();
	reply_x->dumpVector();

	C_HeadVia* viatmp2 = (C_HeadVia*) reply_x->getSTKHeadVia().top();

	SingleAction sa_1 = SingleAction(reply_x);

	action->addSingleAction(sa_1);

	_sm->State = 3;
	return action;


}
//ok for 2->4 and 3->4
bool pre_1_4_cl(SL_SM* _sm, MESSAGE* _message){

	DEBOUT("SM_CL pre_1_4_cl","")

	if (_message->getReqRepType() == REPSUPP
		&&_message->getHeadSipReply().getReply().getCode() == OK_200
		&& _message->getDestEntity() == SODE_SMCLPOINT
		&& _message->getGenEntity() ==  SODE_BPOINT) {
			DEBOUT("SM_CL pre_1_4_cl","true")
			return true;
		}
		else {
			DEBOUT("SM_CL pre_1_4_cl","false")
			return false;
		}
}
ACTION* act_1_4_cl(SL_SM* _sm, MESSAGE* _message) {

	DEBOUT("SM_CL act_1_4_cl","")

	ACTION* action = new ACTION();

	_message->setDestEntity(SODE_ALOPOINT);
	_message->setGenEntity(SODE_SMCLPOINT);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	DUPLICATEMESSAGE(__message, _message, SODE_SMCLPOINT)
	__message->typeOfInternal = TYPE_OP;
	__message->typeOfOperation = TYPE_OP_TIMER_OFF;
	SingleAction sa_2 = SingleAction(_message);
	action->addSingleAction(sa_2);

	_sm->State = 4;
	return action;
}
SL_SM_CL::SL_SM_CL(ENGINE* _eng, SL_CO* _sl_co):
		SL_SM(_eng, _sl_co),
		P0_1CL((SL_SM*)this),
		P1_2CL((SL_SM*)this),
		P1_3CL((SL_SM*)this),
		P1_4CL((SL_SM*)this),
		P3_3CL((SL_SM*)this),
		P3_4CL((SL_SM*)this),
		P2_3CL((SL_SM*)this),
		P2_4CL((SL_SM*)this){

	resend_invite = 0;

	P0_1CL.action = &act_0_1_cl;
	P0_1CL.predicate = &pre_0_1_cl;

	P1_2CL.action = &act_1_2_cl;
	P1_2CL.predicate = &pre_1_2_cl;

	P1_3CL.action = &act_1_3_cl;
	P1_3CL.predicate = &pre_1_3_cl;

	P1_4CL.action = &act_1_4_cl;
	P1_4CL.predicate = &pre_1_4_cl;

	P3_3CL.action = &act_1_3_cl;
	P3_3CL.predicate = &pre_1_3_cl;

	P3_4CL.action = &act_1_4_cl;
	P3_4CL.predicate = &pre_1_4_cl;

	P2_3CL.action = &act_1_3_cl;
	P2_3CL.predicate = &pre_1_3_cl;

	P2_4CL.action = &act_1_4_cl;
	P2_4CL.predicate = &pre_1_4_cl;


	insert_move(0,&P0_1CL);
	insert_move(1,&P1_2CL);
	insert_move(1,&P1_3CL);
	insert_move(1,&P1_4CL);
	insert_move(3,&P3_3CL);
	insert_move(3,&P3_4CL);
	insert_move(2,&P2_3CL);
	insert_move(2,&P2_4CL);


}

//end State machine V3


