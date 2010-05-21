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
#ifndef SIPUTIL_H
#include "SIPUTIL.h"
#endif

static SIPUTIL SipUtil;

//**********************************************************************************
//**********************************************************************************
// CALL_OSET
//**********************************************************************************
//**********************************************************************************
CALL_OSET::CALL_OSET(ENGINE* _engine){

	engine = _engine;

}
//MESSAGE* CALL_OSET::getInviteA(void){
//	return inviteA;
//}

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
SL_SM_CL* CALL_OSET::getSL_SM_CL(void){
	return sl_sm_cl;
}
//v4
void CALL_OSET::setSL_SM_CL(SL_SM_CL* _sl_sm_cl){
	DEBOUT("CALL_OSET::setSL_SM_SV_v4 store ", _sl_sm_cl)
	sl_sm_cl = _sl_sm_cl;
}
//v4
void CALL_OSET::setCall_IdY_v4(string _cally){
	DEBOUT("CALL_OSET::setCall_IdY_v4 store ", _cally)
	callId_Y_v4 = _cally;
}
//v4
string CALL_OSET::getCallId_Y_v4(void){
	return callId_Y_v4;
}

//void CALL_OSET::setInviteB(MESSAGE* _message){
//	DEBOUT("CALL_OSET::setInviteB store ", _message)
//	inviteB = _message;
//}
////get the final invite
//MESSAGE* CALL_OSET::getInviteB(void){
//	return inviteB;
//}
//clear all the other non confirmed invites
void CALL_OSET::purgeGenMess_CL_v4(void){
	DEBASSERT("empty purgeGenMess_CL_v4")
}

//**********************************************************************************
//**********************************************************************************
string CALL_OSET::getCallIdX(void){
	return callId_X;
}
////**********************************************************************************
////**********************************************************************************
//int CALL_OSET::getCounterSequence(string _request){
//
//	//counterSequenceMap.insert(pair<int, TRNSCT*>(cs, tmp));
//
//	map<string,int>::iterator tt;
//	tt = counterSequenceMap.find(_request);
//
//	if ( tt == counterSequenceMap.end() )
//		return -1;
//	else
//		return tt->second;
//}
////**********************************************************************************
////**********************************************************************************
//int CALL_OSET::getNextCounterSequence(string _request){
//
//	map<string,int>::iterator tt;
//	tt = counterSequenceMap.find(_request);
//
//	if ( tt == counterSequenceMap.end() ){
//		counterSequenceMap.insert(pair<string,int>(_request, 1));
//		return 1;
//	}
//	else {
//		int i = tt->second ++;
//		counterSequenceMap.insert(pair<string,int>(_request, i));
//		return i;
//	}
//}
////**********************************************************************************
////**********************************************************************************
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

	DEBOUT("SL_CO::call incoming", _message->getHeadSipRequest().getContent())

    ACTION* action = 0x0;

//	// now create the transaction state machine A
//	TRNSCT_SM* trnsctSM = new TRNSCT_SM(_mess->getHeadSipRequest().getS_AttMethod().getMethodID(), _mess, this, sl_co);
//	char t_key[32];
//	spritf(t_key, "%d#A#%d", _mess->getHeadSipRequest().getS_AttMethod().getMethodID(), _mess->getHeadCSeq().getSequence());
//	call_oset->addTrnsctSm(t_key, trnsctSM);


	if (_message->getDestEntity() == SODE_SMSVPOINT) {

	    DEBOUT("SL_CO::search for transaction state machine", _message->getDialogExtendedCID())

		char t_key[32];
		sprintf(t_key, "%d#A#%d", _message->getHeadSipRequest().getS_AttMethod().getMethodID(), _message->getHeadCSeq().getSequence());
		TRNSCT_SM* trnsctSM = 0x0;
		trnsctSM = call_oset->addTrnsctSm(t_key);
//		if (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST){
//			//cast to correct SM
//			TRNSCT_SM_INVITE* trnsctSM_I = (TRNSCT_SM_INVITE*)trnsctSM;
//		}

		if (trnsctSM == 0x0){
			if (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST){
				trnsctSM = new TRNSCT_SM_INVITE(_message->getHeadSipRequest().getS_AttMethod().getMethodID(), _message, call_oset->getENGINE(), this);
			}
			call_oset->addTrnsctSm(t_key);
		}

		action = trnsctSM->event(_message);

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

		string callidy = _message->getDialogExtendedCID();

	    DEBOUT("SL_CO::call client state machine", callidy)

		//v4
		SL_SM_CL* sl_sm_cl = call_oset->getSL_SM_CL();
		//SL_SM_CL* sl_sm_cl = call_oset->getSL_SM_CL(callidy);

		//v4
		if (sl_sm_cl == 0x0){

			//Client state machine does not exists
			//create and put in comap

			DEBOUT("Creating CL machine callidy", callidy)
			sl_sm_cl = new SL_SM_CL(call_oset->getENGINE(), this);
			//v4
			DEBOUT("Associating", callidy << " and " << call_oset->getCallIdX())
			call_oset->setSL_SM_CL(sl_sm_cl);
			call_oset->setCall_IdY_v4(callidy);
//			call_oset->createTransactionY(_message);
			SL_CC* tmp_sl_cc = (SL_CC*)call_oset->getENGINE();
			tmp_sl_cc->getCOMAP()->setY2XCallId(callidy,call_oset->getCallIdX());
		}
		//v4
//		if (call_oset->findGenMess_CL_v4(callidy) == 0x0){
//
//			DEBOUT("Associating", callidy << " and " << call_oset->getCallIdX())
//			call_oset->addGenMess_CL_v4(callidy, _message);
//			SL_CC* tmp_sl_cc = (SL_CC*)call_oset->getENGINE();
//			tmp_sl_cc->getCOMAP()->setY2XCallId(callidy,call_oset->getCallIdX());
//		}

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
					if (!_tmpMessage->getLock()){
						PURGEMESSAGE(_tmpMessage, "PURGE INVITE")
					}
					else {
						DEBOUT("PURGE INVITE", "locked")
					}

				}//V3
				else if (_tmpMessage->typeOfInternal == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_SMSVPOINT) {
					DEBOUT("CLIENT SM send to Server SM", _tmpMessage->getLine(0))
					DEBOUT("CLIENT SM send to Server SM 2",  _tmpMessage->getDialogExtendedCID())
					((SL_CC*)call_oset->getENGINE())->p_w(_tmpMessage);

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
				else {
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
////**********************************************************************************
////**********************************************************************************
//int TRNSCT::getControlSequence(void){
//	return controlSequence;
//}
//void TRNSCT::setControlSequence(int _s){
//	controlSequence = _s;
//}
//
//void TRNSCT::setMatrixMessage(MESSAGE* _message){
//
//	Matrix = _message;
//}
//
//MESSAGE* TRNSCT::getMatrixMessage(void){
//
//	return Matrix;
//
//}
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

	DEBOUT("SL_SM::SL_SM", "")
	sl_cc = _eng;
    sl_co = _sl_co;

    pthread_mutex_init(&mutex, NULL);
	State = 0;

//	controlSequence = 1;
}
ENGINE* SL_SM::getSL_CC(void){
	return sl_cc;
}
SL_CO* SL_SM::getSL_CO(void){
	return sl_co;
}
//void SL_SM::setControlSequence(int _s){
//	controlSequence = _s;
//}
//int SL_SM::getControlSequence(void){
//	return controlSequence;
//}

//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************

bool pre_0_1_sv(SL_SM* _sm, MESSAGE* _message){

	DEBOUT("SM_SV pre_0_1_sv called","")
	if (_message->getReqRepType() == REQSUPP
			&& (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST
					|| _message->getHeadSipRequest().getS_AttMethod().getMethodID() == ACK_REQUEST)
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

	DEBOUT("SL_SM_SV::act_0_1_sv", _message->getHeadSipRequest().getContent())

	DEBOUT("SL_SM_SV::act_0_1_sv CSeq", _message->getHeadCSeq().getContent())
	DEBOUT("SL_SM_SV::act_0_1_sv CSeq", _message->getHeadCSeq().getSequence())

    //_sm->setControlSequence(_message->getHeadCSeq().getSequence());

	ACTION* action = new ACTION();

	//_message changes its dest and gen
	_message->setDestEntity(SODE_ALOPOINT);
	_message->setGenEntity(SODE_SMSVPOINT);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	//etry is filled later by SL_CO (see design)
	CREATEMESSAGE(etry, _message, SODE_SMSVPOINT)

	DEBOUT("ETRY","SIP/2.0 100 Trying")
	etry->setHeadSipReply("SIP/2.0 100 Trying");

	etry->dropHeader("Contact:");

	SipUtil.genASideReplyFromRequest(_message, etry);
	etry->compileMessage();
	etry->dumpVector();
	etry->typeOfInternal = TYPE_MESS;

	SingleAction sa_2 = SingleAction(etry);

	action->addSingleAction(sa_1);
	action->addSingleAction(sa_2);

	DEBOUT("SL_SM_SV::actions set", _message->getHeadSipRequest().getContent())

	DEBOUT("SL_SM_SV::act_0_1_sv move to state 1","")
	_sm->State = 1;

	return action;

}
//V3
//will do pre_2_2_sv also
bool pre_1_2_sv(SL_SM* _sm, MESSAGE* _message){

	DEBOUT("SM_SV pre_1_2_sv called","")

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

	DEBOUT("SL_SM_SV::act_1_2_sv move to state 2","")
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

	DEBOUT("SM_SV act_2_3_sv called","")

	ACTION* action = new ACTION();

	_message->setDestEntity(SODE_APOINT);
	_message->setGenEntity(SODE_SMSVPOINT);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	//TODO
	// start here timer for resend 200OK until ACK
	// arrives


	DEBOUT("SM_SV act_2_3_sv move to state 3","")
	_sm->State = 3;

	return action;

}
//predicate use: 0->1
ACTION* act_3_4_sv(SL_SM* _sm, MESSAGE* _message) {

	DEBOUT("SM_SV act_3_4_sv called","")

	DEBOUT("SL_SM_SV::event move to state 1", _message->getHeadSipRequest().getContent())

	ACTION* action = new ACTION();

	//_message changes its dest and gen
	_message->setDestEntity(SODE_ALOPOINT);
	_message->setGenEntity(SODE_SMSVPOINT);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	DEBOUT("SM_SV act_3_4_sv move to state 4","")
	_sm->State = 4;

	return action;

}
bool pre_4_6_sv(SL_SM* _sm, MESSAGE* _message){

	DEBOUT("SM_SV pre_4_6_sv called","")
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == BYE_REQUEST
			&& _message->getDestEntity() == SODE_SMSVPOINT
			&& _message->getGenEntity() ==  SODE_APOINT) {
		DEBOUT("SM_SV pre_4_6_sv","true")
		return true;
	}
	else {
		DEBOUT("SM_SV pre_4_6_sv","false")
		return false;
	}
}
ACTION* act_4_6_sv(SL_SM* _sm, MESSAGE* _message) {

	DEBOUT("SL_SM_SV::act_4_6_sv", _message->getHeadSipRequest().getContent())

	ACTION* action = new ACTION();

	_message->setDestEntity(SODE_ALOPOINT);
	_message->setGenEntity(SODE_SMSVPOINT);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	DEBOUT("SM_SV act_4_6_sv move to state 6","")

	_sm->State = 6;

	return action;

}
SL_SM_SV::SL_SM_SV(ENGINE* _eng, SL_CO* _sl_co):
		SL_SM(_eng, _sl_co),
		P0_1SV((SL_SM*)this),
		P1_2SV((SL_SM*)this),
		P2_2SV((SL_SM*)this),
		P2_3SV((SL_SM*)this),
		P1_3SV((SL_SM*)this),
		P3_4SV((SL_SM*)this),
		P4_6SV((SL_SM*)this){

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

	P3_4SV.action = &act_3_4_sv;
	P3_4SV.predicate = &pre_0_1_sv;

	P4_6SV.action = &act_4_6_sv;
	P4_6SV.predicate = &pre_4_6_sv;


	insert_move(0,&P0_1SV);
	insert_move(1,&P1_2SV);
	insert_move(1,&P1_3SV);
	insert_move(2,&P2_2SV);
	insert_move(2,&P2_3SV);
	insert_move(3,&P3_4SV);
	insert_move(4,&P4_6SV);


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

	//This is to be sent later, after timer expires
	//Preconfigure message entity points, the alarm manager cannot do this
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

	DEBOUT("SM_CL act_0_1_cl to state 1","")
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

	// the message contains the to tag that we must save
	// or store it in valo during 200ok

//	MESSAGE* __message = _sm->getSL_CO()->call_oset->getInviteA();
//	MESSAGE* __message = _sm->getSL_CO()->call_oset->getTransactionY(_message->getHeadCSeq().getSequence())->getMatrixMessage();

	DEBOUT("MESSAGE GENERATOR", __message)


	CREATEMESSAGE(reply_x, __message, SODE_SMSVPOINT)
	reply_x->setDestEntity(SODE_SMSVPOINT);
	reply_x->setGenEntity(SODE_SMCLPOINT);
	reply_x->typeOfInternal = TYPE_MESS;


	DEBOUT("CONTACT", reply_x->getHeadContact().getContent())

	reply_x->replaceHeadContact("<sip:sipsl@grog:5060>");

	DEBOUT("NEW CONTACT", reply_x->getHeadContact().getContent())

	DEBOUT("Purge SDP","")
	reply_x->purgeSDP();

	SipUtil.genASideReplyFromBReply(_message, __message, reply_x);
	reply_x->compileMessage();
	reply_x->dumpVector();

	//C_HeadVia* viatmp2 = (C_HeadVia*) reply_x->getSTKHeadVia().top();

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
	SingleAction sa_2 = SingleAction(__message);
	action->addSingleAction(sa_2);

	_sm->State = 4;
	return action;
}
bool pre_4_5_cl(SL_SM* _sm, MESSAGE* _message){

	DEBOUT("SM_CL pre_4_5_cl","")
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == ACK_REQUEST
			&& _message->getDestEntity() == SODE_SMCLPOINT
			&& _message->getGenEntity() ==  SODE_ALOPOINT) {
		DEBOUT("SM_CL pre_4_5_cl","true")
		return true;
	}
	else {
		DEBOUT("SM_CL pre_4_5_cl","false")
		return false;
	}
}
ACTION* act_4_5_cl(SL_SM* _sm, MESSAGE* _message) {

	DEBOUT("SM_CL act_4_5_cl","")

	ACTION* action = new ACTION();

	_message->setDestEntity(SODE_BPOINT);
	_message->setGenEntity(SODE_SMCLPOINT);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	_sm->State = 5;

	return action;

}
bool pre_5_7_cl(SL_SM* _sm, MESSAGE* _message){

	DEBOUT("SM_CL pre_5_7_cl","")
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == BYE_REQUEST
			&& _message->getDestEntity() == SODE_SMCLPOINT
			&& _message->getGenEntity() ==  SODE_ALOPOINT) {
		DEBOUT("SM_CL pre_5_7_cl","true")
		return true;
	}
	else {
		DEBOUT("SM_CL pre_5_7_cl","false")
		return false;
	}
}
ACTION* act_5_7_cl(SL_SM* _sm, MESSAGE* _message) {

	DEBOUT("SM_CL act_5_7_cl","")

	ACTION* action = new ACTION();

	_message->setDestEntity(SODE_BPOINT);
	_message->setGenEntity(SODE_SMCLPOINT);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	_sm->State = 7;

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
		P2_4CL((SL_SM*)this),
		P4_5CL((SL_SM*)this),
		P5_7CL((SL_SM*)this),
		P7_8CL((SL_SM*)this){

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

	P4_5CL.action = &act_4_5_cl;
	P4_5CL.predicate = &pre_4_5_cl;

	P5_7CL.action = &act_5_7_cl;
	P5_7CL.predicate = &pre_5_7_cl;

	P7_8CL.action = &act_1_4_cl;
	P7_8CL.predicate = &pre_1_4_cl;

	insert_move(0,&P0_1CL);
	insert_move(1,&P1_2CL);
	insert_move(1,&P1_3CL);
	insert_move(1,&P1_4CL);
	insert_move(3,&P3_3CL);
	insert_move(3,&P3_4CL);
	insert_move(2,&P2_3CL);
	insert_move(2,&P2_4CL);
	insert_move(4,&P4_5CL);
	insert_move(5,&P5_7CL);
	insert_move(7,&P7_8CL);


}

//V5
//**********************************************************************************
//**********************************************************************************
ACTION* SM_V5::event(MESSAGE* _event){

	PREDICATE_ACTION_V5* tmp;

	ACTION* act=0x0;

	DEBOUT("SM_SL::event Look for state", State)

	pair<multimap<int,PREDICATE_ACTION_V5*>::iterator,multimap<int,PREDICATE_ACTION_V5*>::iterator> ret;

	multimap<int,PREDICATE_ACTION_V5*>::iterator iter;

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
void SM_V5::insert_move(int _i, PREDICATE_ACTION_V5* _pa){

	DEBOUT("SL_SM_V5::insert_move", _i << " " << _pa )
	move_sm.insert(pair<int, PREDICATE_ACTION_V5*>(_i, _pa));

}
SM_V5::SM_V5(ENGINE* _eng, SL_CO* _sl_co){

	DEBOUT("SM_V5::SM_V5", "")
	sl_cc = _eng;
    sl_co = _sl_co;

    pthread_mutex_init(&mutex, NULL);
	State = 0;

//	controlSequence = 1;
}
ENGINE* SM_V5::getSL_CC(void){
	return sl_cc;
}
SL_CO* SM_V5::getSL_CO(void){
	return sl_co;
}
//**********************************************************************************
//**********************************************************************************
PREDICATE_ACTION_V5::PREDICATE_ACTION_V5(SM_V5* _sm){
	machine = _sm;
}
