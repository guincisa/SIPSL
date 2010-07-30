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
CALL_OSET::CALL_OSET(ENGINE* _engine){
	engine = _engine;
	sequenceMap.insert(pair<string, int>("ACK_B",0));
	sequenceMap.insert(pair<string, int>("INVITE_B",0));
	sequenceMap.insert(pair<string, int>("BYE_B",0));
}
int CALL_OSET::getNextSequence(string _method){

	map<string, int> ::iterator p;
	p = sequenceMap.find(_method);
	if (p != sequenceMap.end()){
		int i = (int)p->second + 1;
		sequenceMap.erase(p);
		sequenceMap.insert(pair<string, int>(_method,i));
		return (i);
	}else {
		sequenceMap.insert(pair<string, int>(_method,1));
		return 1;
	}
}
void CALL_OSET::insertSequence(string _method, int _i){
	sequenceMap.insert(pair<string, int>(_method,_i));
}
int CALL_OSET::getCurrentSequence(string _method){

	map<string, int> ::iterator p;
	p = sequenceMap.find(_method);
	if (p != sequenceMap.end()){
		return (int)p->second;
	}
	else {
		return 0;
	}

}
//**********************************************************************************
ENGINE* CALL_OSET::getENGINE(void){
	return engine;
}
//**********************************************************************************
ALO* CALL_OSET::getALO(void){
	return alo;
}
//**********************************************************************************
void CALL_OSET::setALO(ALO* _alo){
	alo = _alo;
}
//**********************************************************************************
SL_CO* CALL_OSET::getSL_CO(void){
	return sl_co;
}
void CALL_OSET::setSL_CO(SL_CO* _sl_co){
	sl_co = _sl_co;
}
//**********************************************************************************
//v4
void CALL_OSET::setCallId_Y(string _cally){
	DEBOUT("CALL_OSET::setCall_IdY store ", _cally)
	callId_Y = _cally;
}
//**********************************************************************************
//v4
string CALL_OSET::getCallId_Y(void){
	return callId_Y;
}
//**********************************************************************************
//v4
void CALL_OSET::setCallId_X(string _callId_X){

	callId_X = _callId_X;
}
//**********************************************************************************
//v4
string CALL_OSET::getCallId_X(void){
	return callId_X;
}
//**********************************************************************************
TRNSCT_SM* CALL_OSET::getTrnsctSm(string _method, int _sode, string _branch){

	DEBOUT_UTIL("CALL_OSET::getTrnsctSm", _method <<" "<<_branch)
	char t_key[264];
	if (_sode == SODE_TRNSCT_CL)
		sprintf(t_key, "%s#SODE_TRNSCT_CL#%s", _method.c_str(), _branch.c_str());
	else if (_sode == SODE_TRNSCT_SV)
		sprintf(t_key, "%s#SODE_TRNSCT_SV#%s", _method.c_str(), _branch.c_str());

	map<string, TRNSCT_SM*> ::iterator p;
	p = trnsctSmMap.find(t_key);
	if (p != trnsctSmMap.end()){
		return ((TRNSCT_SM*)p->second);
	}else {
		return 0x0;
	}
}
//**********************************************************************************
void CALL_OSET::addTrnsctSm(string _method, int _sode, string _branch, TRNSCT_SM* _trnsctSm){

	DEBOUT_UTIL("CALL_OSET::addTrnsctSm", _method <<" "<<_branch)
	char t_key[64];
	if (_sode == SODE_TRNSCT_CL)
		sprintf(t_key, "%s#SODE_TRNSCT_CL#%s", _method.c_str(), _branch.c_str());
	else if (_sode == SODE_TRNSCT_SV)
		sprintf(t_key, "%s#SODE_TRNSCT_SV#%s", _method.c_str(), _branch.c_str());
	else {
		DEBOUT("CALL_OSET::addTrnsctSm NOT INSERTED", _method << _branch)
		return;
	}

	trnsctSmMap.insert(pair<string, TRNSCT_SM*>(t_key, _trnsctSm));
	return;
}
//**********************************************************************************
//**********************************************************************************
// SL_CO
//**********************************************************************************
SL_CO::SL_CO(CALL_OSET* _call_oset){
	call_oset = _call_oset;
    //pthread_mutex_init(&mutex, NULL);

}
//**********************************************************************************
//**********************************************************************************
void SL_CO::call(MESSAGE* _message){

	//pthread_mutex_lock(&mutex);
	DEBMESSAGE("SL_CO::call incoming", _message->getIncBuffer())

    ACTION* action = 0x0;

	if (_message->getDestEntity() == SODE_TRNSCT_SV) {

		DEBOUT("SL_CO::search for transaction state machine", _message->getHeadCallId().getContent())

		TRNSCT_SM* trnsctSM = 0x0;

	    //ACK treatment:
	    //if the Request is an ACK then check the CSEQ and look for "CSEQ INVITE" state machine
	    //if the state machine is in a 2xx reply state then the ACK is acknowledging the INVITE
	    //so the message must go to the INVITE state machine
//	    if (_message->getHeadCSeq().getMethod().getMethodID() == ACK_REQUEST) {
//	    	//Problem ACK come with a new branch
//			trnsctSM = call_oset->getTrnsctSm("INVITE", SODE_TRNSCT_SV, ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch"));
//			if (trnsctSM != 0x0 && trnsctSM->State == 3){
//				//run into the INVITE state machine
//			}
//			//else the ACK is a new transaction
//			else {
//				DEBASSERT("ACK is a new transaction not supported now")
//			}
//	    }
//	    else {
	    	trnsctSM = call_oset->getTrnsctSm(_message->getHeadCSeq().getMethod().getContent(), SODE_TRNSCT_SV, ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch"));
//	    }

		if (trnsctSM == 0x0){
			if (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST){
				call_oset->insertSequence("INVITE_A", _message->getHeadCSeq().getSequence());
				NEWPTR2(trnsctSM, TRNSCT_SM_INVITE_SV(_message->getHeadSipRequest().getS_AttMethod().getMethodID(), _message, call_oset->getENGINE(), this))
			}
			else if (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == ACK_REQUEST){
				NEWPTR2(trnsctSM, TRNSCT_SM_ACK_SV(_message->getHeadSipRequest().getS_AttMethod().getMethodID(), _message, call_oset->getENGINE(), this))
			}
			else if (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == BYE_REQUEST){
				NEWPTR2(trnsctSM, TRNSCT_SM_BYE_SV(_message->getHeadSipRequest().getS_AttMethod().getMethodID(), _message, call_oset->getENGINE(), this))
			}
			DEBOUT("call_oset->addTrnsctSm", _message->getHeadCSeq().getMethod().getContent() << " " << ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch"))
			call_oset->addTrnsctSm(_message->getHeadCSeq().getMethod().getContent(), SODE_TRNSCT_SV, ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch"), trnsctSM);
			DEBOUT("call_oset->addTrnsctSm","done")
		}

		action = trnsctSM->event(_message);

		if (action != 0x0){

			// now get actions
			stack<SingleAction> actionList = action->getActionList();

			while (!actionList.empty()){

				MESSAGE* _tmpMessage = actionList.top().getMessage();
				DEBMESSAGE("SL_CO::reading action stack server, message:", _tmpMessage->getIncBuffer())

				//V5
				if (_tmpMessage->typeOfInternal == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_ALOPOINT){
					// send message to ALO
					DEBOUT("SL_CO::call action is send to ALO", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getDialogExtendedCID())
					call_oset->getALO()->p_w(_tmpMessage);
				}
				else if (_tmpMessage->typeOfInternal == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_TRNSCT_CL){
					//server sm sending to client sm shoud not happen
					DEBASSERT("should not happen")
					//DEBOUT("SL_CO::call action is send to CL", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getDialogExtendedCID())
					//call_oset->getENGINE()->p_w(_tmpMessage);
				}
				else if (_tmpMessage->typeOfInternal == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_NTWPOINT){
					if (_tmpMessage->getReqRepType() == REPSUPP) {
						//Check if there is a ROUTE header
						call_oset->getENGINE()->getSUDP()->sendReply(_tmpMessage);
						//PURGEMESSAGE(_tmpMessage, "PURGE MESSAGE")
					}
					else {
						DEBASSERT("???")
					}

				} else {
					//TODO
					DEBASSERT("SL_CO::call action is ???")
				}
				DEBOUT("pop action","")
				actionList.pop();
			}
		}
		else {
			DEBOUT("SL_CO::event", "action is null nothing, event ignored")
			PURGEMESSAGE(_message,"SL_CO::delete message")
			//return;
		}
	}
	else if (_message->getDestEntity() == SODE_TRNSCT_CL){

		string callidys = _message->getHeadCallId().getContent();
	    DEBOUT("SL_CO::call client state machine", callidys)

		TRNSCT_SM* trnsct_cl = call_oset->getTrnsctSm(_message->getHeadCSeq().getMethod().getContent(), SODE_TRNSCT_CL, ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch"));

		if (trnsct_cl == 0x0){

			DEBOUT("Creating Trnsct Client machine callidy", callidys)
			if(_message->getReqRepType() == REQSUPP){
				if (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST){
					NEWPTR2(trnsct_cl, TRNSCT_SM_INVITE_CL(_message->getHeadSipRequest().getS_AttMethod().getMethodID(), _message, _message->getSourceMessage(), call_oset->getENGINE(), this))
				}
				else if (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == ACK_REQUEST){
					NEWPTR2(trnsct_cl, TRNSCT_SM_ACK_CL(_message->getHeadSipRequest().getS_AttMethod().getMethodID(), _message, _message->getSourceMessage(), call_oset->getENGINE(), this))
				}
				else if (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == BYE_REQUEST){
					NEWPTR2(trnsct_cl, TRNSCT_SM_BYE_CL(_message->getHeadSipRequest().getS_AttMethod().getMethodID(), _message, _message->getSourceMessage(), call_oset->getENGINE(), this))
				}
			}else{
				DEBOUT("****************************errore","")
				DEBASSERT("fare qui")
			}

			call_oset->addTrnsctSm(_message->getHeadCSeq().getMethod().getContent(), SODE_TRNSCT_CL, ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch"), trnsct_cl);

			//TODO a che serve?
			//serve solo la prima volta con l'invite...
			SL_CC* tmp_sl_cc = (SL_CC*)call_oset->getENGINE();
			tmp_sl_cc->getCOMAP()->setY2XCallId(callidys,call_oset->getCallId_X());
		}

		ACTION* action = trnsct_cl->event(_message);

		if (action != 0x0){

			// now read actions

			stack<SingleAction> actionList;
			actionList = action->getActionList();

			while (!actionList.empty()){

				MESSAGE* _tmpMessage = actionList.top().getMessage();

				DEBMESSAGE("SL_CO::reading action stack client, message:", _tmpMessage->getIncBuffer())

				if (_tmpMessage->typeOfInternal == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_ALOPOINT){
					// send message to ALO
					// should never happen
					DEBOUT("SL_CO::call action is send to ALO", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getHeadCallId().getContent())
					call_oset->getALO()->p_w(_tmpMessage);
					actionList.pop();
					continue;
				}
				else if (_tmpMessage->typeOfInternal == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_NTWPOINT){

					DEBOUT("SL_CO::call action is send to B", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getHeadCallId().getContent())

					if (_tmpMessage->getReqRepType() == REQSUPP) {
						call_oset->getENGINE()->getSUDP()->sendRequest(_tmpMessage);
//						if (!_tmpMessage->getLock()){
//							PURGEMESSAGE(_tmpMessage, "PURGE INVITE")
//						}
					}
					else {
						DEBOUT("++++++++++++++++++++ assert ","")
						DEBASSERT("???")
					}
					actionList.pop();
					continue;
				}
				else if (_tmpMessage->typeOfInternal == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_SMSVPOINT) {
					DEBOUT("CLIENT SM send to Server SM", _tmpMessage->getLine(0))
					DEBOUT("CLIENT SM send to Server SM 2",  _tmpMessage->getHeadCallId().getContent())
					((SL_CC*)call_oset->getENGINE())->p_w(_tmpMessage);
					actionList.pop();
					continue;
				}
				else if (_tmpMessage->typeOfInternal == TYPE_OP){ // to alarm

					DEBOUT("SL_CO:: TYPE_OP","")

					if ( _tmpMessage->typeOfOperation == TYPE_OP_TIMER_ON){
						DEBOUT("SL_CO::call action is send to ALARM", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getHeadCallId().getContent())
						SysTime st1 = _tmpMessage->getFireTime();
						call_oset->getENGINE()->getSUDP()->getAlmgr()->insertAlarm(_tmpMessage, st1);

					} else if (_tmpMessage->typeOfOperation == TYPE_OP_TIMER_OFF){

						DEBOUT("SL_CO::call action is clear ALARM", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getHeadCallId().getContent())
						string callid_alarm = _message->getHeadCSeq().getContent() +  _message->getHeadCallId().getNormCallId();
						DEBOUT("SL_CO::cancel alarm, callid", callid_alarm)
						call_oset->getENGINE()->getSUDP()->getAlmgr()->cancelAlarm(callid_alarm);
						PURGEMESSAGE(_tmpMessage, "Delete message used to clear alarm")
					}
					else {
						DEBASSERT("SL_CO client side inconsistency")
					}
					actionList.pop();
					continue;
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
		}
	}

	DEBOUT("SL_CO::call ended","")
	if (action != 0x0){
		DEBOUT("SL_CO::call delete action","")
		delete action;
	}
	//pthread_mutex_unlock(&mutex);
}
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
TRNSCT_SM::TRNSCT_SM(int _requestType, MESSAGE* _matrixMess, MESSAGE* _a_Matrix, ENGINE* _sl_cc, SL_CO* _sl_co):
	SM_V5(_sl_cc, _sl_co){

	requestType = _requestType;
	Matrix = _matrixMess;
	A_Matrix = _a_Matrix;
	if (_a_Matrix == 0x0){
		DEBY
		DEBASSERT("NO")
	}
}

MESSAGE* TRNSCT_SM::getMatrixMessage(void){
	if (Matrix == 0x0){
		DEBY
		DEBASSERT("NO")
	}

	return Matrix;
}
MESSAGE* TRNSCT_SM::getA_Matrix(void){
	if (A_Matrix == 0x0){
		DEBY
		DEBASSERT("NO")
	}
	return A_Matrix;
}

//**********************************************************************************

ACTION* SM_V5::event(MESSAGE* _event){

	PREDICATE_ACTION_V5* tmp;

	ACTION* act=0x0;

	DEBOUT("SM_V5::event Look for state", State)

	pair<multimap<const int,PREDICATE_ACTION_V5*>::iterator,multimap<const int,PREDICATE_ACTION_V5*>::iterator> ret;

	multimap<const int,PREDICATE_ACTION_V5*>::iterator iter;

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

	DEBOUT("SM_V5::exec_it unexpected event", _event)
}
//**********************************************************************************
//**********************************************************************************
void SM_V5::insert_move(int _i, PREDICATE_ACTION_V5* _pa){

	DEBOUT("SM_V5::insert_move", _i << " " << _pa )
	move_sm.insert(pair<const int, PREDICATE_ACTION_V5*>(_i, _pa));

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
//**********************************************************************************
//**********************************************************************************
PREDICATE_ACTION_V5::PREDICATE_ACTION_V5(SM_V5* _sm){
	machine = _sm;
}
//**********************************************************************************
//**********************************************************************************
//*****************************************************************
// V5
// server transaction
//*****************************************************************
bool pre_0_1_inv_sv(SM_V5* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_INV_SV pre_0_1_inv_sv called","")
	if (_message->getReqRepType() == REQSUPP
			&& (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST)
			&& _message->getDestEntity() == SODE_TRNSCT_SV
			&& _message->getGenEntity() ==  SODE_NTWPOINT) {
		DEBOUT("TRNSCT_INV_SV pre_0_1_inv_sv","true")
		return true;
	}
	else {
		DEBOUT("TRNSCT_INV_SV pre_0_1_inv_sv","false")
		return false;
	}
}
ACTION* act_0_1_inv_sv(SM_V5* _sm, MESSAGE* _message) {

	DEBOUT("TRSNCT_INV_SV::act_0_1_inv_sv", _message->getHeadSipRequest().getContent())

	DEBOUT("TRSNCT_INV_SV::act_0_1_inv_sv CSeq", _message->getHeadCSeq().getContent())
	DEBOUT("TRSNCT_INV_SV::act_0_1_inv_sv CSeq", _message->getHeadCSeq().getSequence())

    //_sm->setControlSequence(_message->getHeadCSeq().getSequence());

	NEWPTR(ACTION*, action, ACTION())

	//_message changes its dest and gen
	_message->setDestEntity(SODE_ALOPOINT);
	_message->setGenEntity(SODE_TRNSCT_SV);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	//etry is filled later by SL_CO (see design)
	CREATEMESSAGE(etry, _message, SODE_TRNSCT_SV)

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

	DEBOUT("TRSNCT_INV_SV::act_0_1_inv_sv set", _message->getHeadSipRequest().getContent())

	DEBOUT("TRSNCT_INV_SV::act_0_1_inv_sv move to state 1","")
	_sm->State = 1;

	return action;

}
bool pre_1_2_inv_sv(SM_V5* _sm, MESSAGE* _message){

	DEBOUT("SM_SV pre_1_2_inv_sv called","")

	if (_message->getReqRepType() == REPSUPP
		&& (_message->getHeadSipReply().getReply().getCode() == DIALOGE_101
				|| _message->getHeadSipReply().getReply().getCode() == RINGING_180)
		&& _message->getDestEntity() == SODE_TRNSCT_SV
		&& _message->getGenEntity() ==  SODE_TRNSCT_CL) {
			return true;
			DEBOUT("SM_SV pre_1_2_inv_sv","true")
		}
		else {
			DEBOUT("SM_SV pre_1_2_inv_sv","false")
			return false;
		}
}
ACTION* act_1_2_inv_sv(SM_V5* _sm, MESSAGE* _message) {

	DEBOUT("act_1_2_inv_sv::act_1_2_sv",  _message->getHeadSipReply().getReply().getCode() )

	NEWPTR(ACTION*, action, ACTION())

	// The message ha been prepared by client so it's ready to be sent back
	_message->setDestEntity(SODE_NTWPOINT);
	_message->setGenEntity(SODE_TRNSCT_SV);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	DEBOUT("SL_SM_SV::act_1_2_inv_sv move to state 2","")
	_sm->State = 2;

	return action;
}
bool pre_1_3_inv_sv(SM_V5* _sm, MESSAGE* _message){

	DEBOUT("SM_V5 pre_1_3_inv_sv","")

	if (_message->getReqRepType() == REPSUPP
		&& _message->getHeadSipReply().getReply().getCode() == OK_200
		&& _message->getDestEntity() == SODE_TRNSCT_SV
		&& _message->getGenEntity() ==  SODE_ALOPOINT) {
			DEBOUT("SM_V5 pre_1_3_inv_sv","true")
			return true;
		}
		else {
			DEBOUT("SM_V5 pre_1_3_inv_sv","false")
			return false;
		}
}
ACTION* act_1_3_inv_sv(SM_V5* _sm, MESSAGE* _message) {

	DEBOUT("SM_V5 act_1_3_inv_sv called","")

	NEWPTR(ACTION*, action, ACTION())

	_message->setDestEntity(SODE_NTWPOINT);
	_message->setGenEntity(SODE_TRNSCT_SV);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	//TODO
	// start here timer for resend 200OK until ACK
	// arrives


	DEBOUT("SM_V5 act_1_3_inv_sv move to state 3","")
	_sm->State = 3;

	return action;

}
bool pre_3_4_inv_sv(SM_V5* _sm, MESSAGE* _message){

	DEBOUT("SM_V5 pre_3_4_inv_sv called","")
	if (_message->getReqRepType() == REQSUPP
			&& (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == ACK_REQUEST)
			&& _message->getDestEntity() == SODE_TRNSCT_SV
			&& _message->getGenEntity() ==  SODE_NTWPOINT) {
		DEBOUT("SM_V5 pre_3_4_inv_sv","true")
		return true;
	}
	else {
		DEBOUT("SM_V5 pre_3_4_inv_sv","false")
		return false;
	}
}

ACTION* act_3_4_inv_sv(SM_V5* _sm, MESSAGE* _message) {

	DEBOUT("SM_V5 act_3_4_inv_sv called","")

	DEBOUT("SM_V5::event move to state 4", _message->getHeadSipRequest().getContent())

	NEWPTR(ACTION*, action, ACTION())

	//_message changes its dest and gen
	_message->setDestEntity(SODE_ALOPOINT);
	_message->setGenEntity(SODE_TRNSCT_SV);
	_message->typeOfInternal = TYPE_MESS;
	_message->type_trnsct = TYPE_NNTRNSCT;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	DEBOUT("SM_V5 act_3_4_inv_sv move to state 4","")
	_sm->State = 4;

	return action;

}

//**********************************************************************************
TRNSCT_SM_INVITE_SV::TRNSCT_SM_INVITE_SV(int _requestType, MESSAGE* _matrixMess, ENGINE* _sl_cc, SL_CO* _sl_co):
		TRNSCT_SM(_requestType, _matrixMess, _matrixMess, _sl_cc, _sl_co),
		PA_INV_0_1SV((SM_V5*)this),
		PA_INV_1_2SV((SM_V5*)this),
		PA_INV_1_3SV((SM_V5*)this),
		PA_INV_3_4SV((SM_V5*)this){

	PA_INV_0_1SV.action = &act_0_1_inv_sv;
	PA_INV_0_1SV.predicate = &pre_0_1_inv_sv;

	PA_INV_1_2SV.action = &act_1_2_inv_sv;
	PA_INV_1_2SV.predicate = &pre_1_2_inv_sv;

	PA_INV_1_3SV.action = &act_1_3_inv_sv;
	PA_INV_1_3SV.predicate = &pre_1_3_inv_sv;

	PA_INV_3_4SV.action = &act_3_4_inv_sv;
	PA_INV_3_4SV.predicate = &pre_3_4_inv_sv;


	insert_move(0,&PA_INV_0_1SV);
	insert_move(1,&PA_INV_1_2SV);
	insert_move(1,&PA_INV_1_3SV);
	insert_move(2,&PA_INV_1_3SV);
	insert_move(3,&PA_INV_3_4SV);

}
//*****************************************************************
// V5
// client transaction
//*****************************************************************
bool pre_0_1_inv_cl(SM_V5* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_INV_CL pre_0_1_inv_cl","")
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_ALOPOINT) {
		DEBOUT("TRNSCT_INV_CL pre_0_1_inv_cl","true")
		return true;
	}
	else {
		DEBOUT("TRNSCT_INV_CL pre_0_1_inv_cl","false")
		return false;
	}
}
ACTION* act_0_1_inv_cl(SM_V5* _sm, MESSAGE* _message) {

	DEBOUT("TRNSCT_INV_CL act_0_1_inv_cl","")

	NEWPTR(ACTION*, action, ACTION())

	//Message has to be sent
	_message->setDestEntity(SODE_NTWPOINT);
	_message->setGenEntity(SODE_TRNSCT_CL);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	//careful with source message.
	//Prepare message for Alarm
	DUPLICATEMESSAGE(__timedmessage, _message, SODE_TRNSCT_CL)

	//This is to be sent later, after timer expires
	//Preconfigure message entity points, the alarm manager cannot do this
	__timedmessage->setDestEntity(SODE_TRNSCT_CL);
	__timedmessage->setGenEntity(SODE_TRNSCT_CL);

	SysTime afterT;
	GETTIME(afterT);
	afterT.tv.tv_sec = afterT.tv.tv_sec + TIMER_1_sc*(((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite+1);
	afterT.tv.tv_usec = afterT.tv.tv_usec + TIMER_1_mc*(((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite+1);

	__timedmessage->setFireTime(afterT);
	__timedmessage->typeOfInternal = TYPE_OP;
	__timedmessage->typeOfOperation = TYPE_OP_TIMER_ON;
	SingleAction sa_2 = SingleAction(__timedmessage);

	action->addSingleAction(sa_2);

	((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite++;

	DEBOUT("TRNSCT_INV_CL act_0_1_inv_cl resend value", ((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite)

	_sm->State = 1;

	return action;

}
bool pre_1_1_inv_cl(SM_V5* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_INV_CL pre_1_1_inv_cl","")
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_TRNSCT_CL
			&& ((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite <= MAX_INVITE_RESEND) {
		DEBOUT("TRNSCT_INV_CL pre_1_1_inv_cl","true")
		return true;
	}
	else {
		DEBOUT("TRNSCT_INV_CL pre_1_1_inv_cl","false")
		return false;
	}
}
bool pre_1_99_inv_cl(SM_V5* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_INV_CL pre_1_99_inv_cl","")
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_TRNSCT_CL
			&& ((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite > MAX_INVITE_RESEND) {
		DEBOUT("TRNSCT_INV_CL pre_1_99_inv_cl","true")
		return true;
	}
	else {
		DEBOUT("TRNSCT_INV_CL pre_1_99_inv_cl","false")
		return false;
	}
}
ACTION* act_1_99_inv_cl(SM_V5* _sm, MESSAGE* _message) {

	DEBASSERT("TRNSCT_INV_CL act_1_99_inv_cl please do something")

	_sm->State = 99;

	return 0x0;

}
bool pre_1_1b_inv_cl(SM_V5* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_INV_CL pre_1_1b_inv_cl","")

	if (_message->getReqRepType() == REPSUPP
		&&_message->getHeadSipReply().getReply().getCode() == TRYING_100
		&& _message->getDestEntity() == SODE_TRNSCT_CL
		&& _message->getGenEntity() ==  SODE_NTWPOINT) {
			DEBOUT("TRNSCT_INV_CL pre_1_1b_inv_cl","true")
			return true;
		}
		else {
			DEBOUT("TRNSCT_INV_CL pre_1_1b_inv_cl","false")
			return false;
		}
}
ACTION* act_1_1b_inv_cl(SM_V5* _sm, MESSAGE* _message) {

	DEBOUT("TRNSCT_INV_CL act_1_1b_inv_cl",_message->getHeadSipReply().getReply().getCode())

	// TODO clear timer ad create new timer for the ringing ?

	NEWPTR(ACTION*, action, ACTION())
	_message->typeOfInternal = TYPE_OP;
	_message->typeOfOperation = TYPE_OP_TIMER_OFF;
	SingleAction sa_1 = SingleAction(_message);
	action->addSingleAction(sa_1);

	_sm->State = 1;
	return action;
}
bool pre_1_3_inv_cl(SM_V5* _sm, MESSAGE* _message){

	DEBOUT("SM_CL pre_1_3_inv_cl","")

	if (_message->getReqRepType() == REPSUPP
		&& (_message->getHeadSipReply().getReply().getCode() == DIALOGE_101
				|| _message->getHeadSipReply().getReply().getCode() == RINGING_180)
		&& _message->getDestEntity() == SODE_TRNSCT_CL
		&& _message->getGenEntity() ==  SODE_NTWPOINT) {
			DEBOUT("SM_CL pre_1_3_inv_cl","true")
			return true;
		}
		else {
			DEBOUT("SM_CL pre_1_3_inv_cl","false")
			return false;
		}
}
ACTION* act_1_3_inv_cl(SM_V5* _sm, MESSAGE* _message) {

	DEBOUT("SM_CL act_1_3_inv_cl","")

	NEWPTR(ACTION*, action, ACTION())

	// the message contains the to tag that we must save
	// or store it in valo during 200ok

	MESSAGE* __message = ((TRNSCT_SM*)_sm)->getA_Matrix();
	DEBOUT("MESSAGE GENERATOR", __message)

	CREATEMESSAGE(reply_x, __message, SODE_TRNSCT_SV)
	reply_x->setDestEntity(SODE_TRNSCT_SV);
	reply_x->setGenEntity(SODE_TRNSCT_CL);
	reply_x->typeOfInternal = TYPE_MESS;

	DEBOUT("CONTACT", reply_x->getHeadContact().getContent())

	reply_x->replaceHeadContact("<sip:sipsl@grog:5060>");

	DEBOUT("NEW CONTACT", reply_x->getHeadContact().getContent())

	DEBOUT("Purge SDP","")
	reply_x->purgeSDP();

	SipUtil.genASideReplyFromBReply(_message, __message, reply_x);
	reply_x->compileMessage();
	reply_x->dumpVector();

	SingleAction sa_1 = SingleAction(reply_x);

	action->addSingleAction(sa_1);


	//Clear alam here in case the b did not send any trying
	DUPLICATEMESSAGE(___message, _message, SODE_TRNSCT_CL)
	___message->typeOfInternal = TYPE_OP;
	___message->typeOfOperation = TYPE_OP_TIMER_OFF;
	SingleAction sa_2 = SingleAction(___message);
	action->addSingleAction(sa_2);

	_sm->State = 3;
	return action;


}
bool pre_1_4_inv_cl(SM_V5* _sm, MESSAGE* _message){

	DEBOUT("SM_V5 pre_1_4_inv_cl","")

	if (_message->getReqRepType() == REPSUPP
		&&_message->getHeadSipReply().getReply().getCode() == OK_200
		&& _message->getDestEntity() == SODE_TRNSCT_CL
		&& _message->getGenEntity() ==  SODE_NTWPOINT) {
			DEBOUT("SM_V5 pre_1_4_inv_cl","true")
			return true;
		}
		else {
			DEBOUT("SM_V5 pre_1_4_inv_cl","false")
			return false;
		}
}
ACTION* act_1_4_inv_cl(SM_V5* _sm, MESSAGE* _message) {

	DEBOUT("SM_V5 act_1_4_inv_cl","")

	NEWPTR(ACTION*, action, ACTION())

	_message->setDestEntity(SODE_ALOPOINT);
	_message->setGenEntity(SODE_TRNSCT_CL);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	//Clear alam here in case the b did not send any trying
	DUPLICATEMESSAGE(___message, _message, SODE_TRNSCT_CL)
	___message->typeOfInternal = TYPE_OP;
	___message->typeOfOperation = TYPE_OP_TIMER_OFF;
	SingleAction sa_2 = SingleAction(___message);
	action->addSingleAction(sa_2);

	_sm->State = 4;
	return action;
}
bool pre_4_5_inv_cl(SM_V5* _sm, MESSAGE* _message){

	DEBOUT("SM_V5 pre_4_5_inv_cl","")
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == ACK_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_ALOPOINT) {
		DEBOUT("SM_V5 pre_4_5_inv_cl","true")
		return true;
	}
	else {
		DEBOUT("SM_V5 pre_4_5_inv_cl","false")
		return false;
	}
}
ACTION* act_4_5_inv_cl(SM_V5* _sm, MESSAGE* _message) {

	DEBOUT("SM_V5 act_4_5_inv_cl","")

	NEWPTR(ACTION*, action, ACTION())

	_message->setDestEntity(SODE_NTWPOINT);
	_message->setGenEntity(SODE_TRNSCT_CL);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	_sm->State = 5;

	return action;

}
//**********************************************************************************
TRNSCT_SM_INVITE_CL::TRNSCT_SM_INVITE_CL(int _requestType, MESSAGE* _matrixMess, MESSAGE* _A_Matrix, ENGINE* _sl_cc, SL_CO* _sl_co):
		TRNSCT_SM(_requestType, _matrixMess, _A_Matrix, _sl_cc, _sl_co),
		PA_INV_0_1CL((SM_V5*)this),
		PA_INV_1_1CL((SM_V5*)this),
		PA_INV_1_1bCL((SM_V5*)this),
		PA_INV_1_3CL((SM_V5*)this),
		PA_INV_1_4CL((SM_V5*)this),
		PA_INV_4_5CL((SM_V5*)this),
		PA_INV_1_99CL((SM_V5*)this){

	PA_INV_0_1CL.action = &act_0_1_inv_cl;
	PA_INV_0_1CL.predicate = &pre_0_1_inv_cl;

	PA_INV_1_1CL.action = &act_0_1_inv_cl;
	PA_INV_1_1CL.predicate = &pre_1_1_inv_cl;

	PA_INV_1_99CL.action = &act_1_99_inv_cl;
	PA_INV_1_99CL.predicate = &pre_1_99_inv_cl;

	PA_INV_1_1bCL.action = &act_1_1b_inv_cl;
	PA_INV_1_1bCL.predicate = &pre_1_1b_inv_cl;

	PA_INV_1_3CL.action = &act_1_3_inv_cl;
	PA_INV_1_3CL.predicate = &pre_1_3_inv_cl;

	PA_INV_1_4CL.action = &act_1_4_inv_cl;
	PA_INV_1_4CL.predicate = &pre_1_4_inv_cl;

	PA_INV_4_5CL.action = &act_4_5_inv_cl;
	PA_INV_4_5CL.predicate = &pre_4_5_inv_cl;


	resend_invite = 0;

	insert_move(0,&PA_INV_0_1CL);
	insert_move(1,&PA_INV_1_1CL);
	insert_move(1,&PA_INV_1_1bCL);
	insert_move(1,&PA_INV_1_3CL);
	insert_move(1,&PA_INV_1_4CL);
	insert_move(3,&PA_INV_1_4CL);
	insert_move(4,&PA_INV_4_5CL);

	//manca 3 a 3 con dialoge


	insert_move(1,&PA_INV_1_99CL);


}
bool pre_0_1_ack_sv(SM_V5* _sm, MESSAGE* _message){

	DEBOUT("SM_V5 pre_0_1_ack_sv called","")
	if (_message->getReqRepType() == REQSUPP
			&& (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == ACK_REQUEST)
			&& _message->getDestEntity() == SODE_TRNSCT_SV
			&& _message->getGenEntity() ==  SODE_NTWPOINT) {
		DEBOUT("SM_V5 pre_0_1_ack_sv","true")
		return true;
	}
	else {
		DEBOUT("SM_V5 pre_0_1_ack_sv","false")
		return false;
	}
}
ACTION* act_0_1_ack_sv(SM_V5* _sm, MESSAGE* _message) {

	DEBOUT("SM_V5 act_0_1_ack_sv called","")

	DEBOUT("SM_V5::event move to state 1", _message->getHeadSipRequest().getContent())

	NEWPTR(ACTION*, action, ACTION())

	//_message changes its dest and gen
	_message->setDestEntity(SODE_ALOPOINT);
	_message->setGenEntity(SODE_TRNSCT_SV);
	_message->typeOfInternal = TYPE_MESS;
	_message->type_trnsct = TYPE_TRNSCT;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	DEBOUT("SM_V5 act_0_1_ack_sv move to state 1","")
	_sm->State = 1;

	return action;

}
//**********************************************************************************
TRNSCT_SM_ACK_SV::TRNSCT_SM_ACK_SV(int _requestType, MESSAGE* _matrixMess, ENGINE* _sl_cc, SL_CO* _sl_co):
		TRNSCT_SM(_requestType, _matrixMess, _matrixMess, _sl_cc, _sl_co),
		PA_ACK_0_1SV((SM_V5*)this){

	PA_ACK_0_1SV.action = &act_0_1_ack_sv;
	PA_ACK_0_1SV.predicate = &pre_0_1_ack_sv;

	insert_move(0,&PA_ACK_0_1SV);
}
bool pre_0_1_ack_cl(SM_V5* _sm, MESSAGE* _message){

	DEBOUT("SM_V5 pre_0_1_ack_cl","")
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == ACK_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_ALOPOINT) {
		DEBOUT("SM_V5 pre_0_1_ack_cl","true")
		return true;
	}
	else {
		DEBOUT("SM_V5 pre_0_1_ack_cl","false")
		return false;
	}
}
ACTION* act_0_1_ack_cl(SM_V5* _sm, MESSAGE* _message) {

	DEBOUT("SM_V5 act_0_1_ack_cl","")

	NEWPTR(ACTION*, action, ACTION())

	_message->setDestEntity(SODE_NTWPOINT);
	_message->setGenEntity(SODE_TRNSCT_CL);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	_sm->State = 1;

	return action;

}
//**********************************************************************************
TRNSCT_SM_ACK_CL::TRNSCT_SM_ACK_CL(int _requestType, MESSAGE* _matrixMess, MESSAGE* _A_Matrix, ENGINE* _sl_cc, SL_CO* _sl_co):
		TRNSCT_SM(_requestType, _matrixMess, _A_Matrix, _sl_cc, _sl_co),
		PA_ACK_0_1CL((SM_V5*)this){

	PA_ACK_0_1CL.action = &act_0_1_ack_cl;
	PA_ACK_0_1CL.predicate = &pre_0_1_ack_cl;

	insert_move(0,&PA_ACK_0_1CL);
}
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
bool pre_0_1_bye_sv(SM_V5* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_INV_SV pre_0_1_bye_sv called","")
	if (_message->getReqRepType() == REQSUPP
			&& (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == BYE_REQUEST)
			&& _message->getDestEntity() == SODE_TRNSCT_SV
			&& ( _message->getGenEntity() ==  SODE_NTWPOINT)) {
		DEBOUT("TRNSCT_INV_SV pre_0_1_bye_sv","true")
		return true;
	}
	else {
		DEBOUT("TRNSCT_INV_SV pre_0_1_bye_sv","false")
		return false;
	}
}
ACTION* act_0_1_bye_sv(SM_V5* _sm, MESSAGE* _message) {

	DEBOUT("TRSNCT_INV_SV::act_0_1_bye_sv", _message->getHeadSipRequest().getContent())

	DEBOUT("TRSNCT_INV_SV::act_0_1_bye_sv CSeq", _message->getHeadCSeq().getContent())
	DEBOUT("TRSNCT_INV_SV::act_0_1_bye_sv CSeq", _message->getHeadCSeq().getSequence())

    //_sm->setControlSequence(_message->getHeadCSeq().getSequence());

	NEWPTR(ACTION*, action, ACTION())

	//_message changes its dest and gen
	// remember initial generation  is used for backward messagges like bye coming from B
	_message->setDestEntity(SODE_ALOPOINT);
	_message->setGenEntity(SODE_TRNSCT_SV);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	DEBOUT("TRSNCT_INV_SV::act_0_1_bye_sv set", _message->getHeadSipRequest().getContent())

	DEBOUT("TRSNCT_INV_SV::act_0_1_bye_sv move to state 1","")
	_sm->State = 1;

	return action;

}
bool pre_1_2_bye_sv(SM_V5* _sm, MESSAGE* _message){

	DEBOUT("SM_V5 pre_1_2_bye_sv","")

	if (_message->getReqRepType() == REPSUPP
		&& _message->getHeadSipReply().getReply().getCode() == OK_200
		&& _message->getDestEntity() == SODE_TRNSCT_SV
		&& _message->getGenEntity() ==  SODE_ALOPOINT) {
			DEBOUT("SM_V5 pre_1_2_bye_sv","true")
			return true;
		}
		else {
			DEBOUT("SM_V5 pre_1_2_bye_sv","false")
			return false;
		}
}
ACTION* act_1_2_bye_sv(SM_V5* _sm, MESSAGE* _message) {

	DEBOUT("SM_V5 act_1_2_bye_sv called","")

	NEWPTR(ACTION*, action, ACTION())

	_message->setDestEntity(SODE_NTWPOINT);
	_message->setGenEntity(SODE_TRNSCT_SV);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);


	DEBOUT("SM_V5 act_1_2_bye_sv move to state 2","")
	_sm->State = 2;

	return action;

}
//**********************************************************************************
TRNSCT_SM_BYE_SV::TRNSCT_SM_BYE_SV(int _requestType, MESSAGE* _matrixMess, ENGINE* _sl_cc, SL_CO* _sl_co):
		TRNSCT_SM(_requestType, _matrixMess, _matrixMess, _sl_cc, _sl_co),
		PA_BYE_0_1SV((SM_V5*)this),
		PA_BYE_1_2SV((SM_V5*)this){

	PA_BYE_0_1SV.action = &act_0_1_bye_sv;
	PA_BYE_0_1SV.predicate = &pre_0_1_bye_sv;

	PA_BYE_1_2SV.action = &act_1_2_bye_sv;
	PA_BYE_1_2SV.predicate = &pre_1_2_bye_sv;


	insert_move(0,&PA_BYE_0_1SV);
	insert_move(1,&PA_BYE_1_2SV);

}
bool pre_0_1_bye_cl(SM_V5* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_SM_BYE_CL pre_0_1_bye_cl","")
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == BYE_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_ALOPOINT) {
		DEBOUT("TRNSCT_SM_BYE_CL pre_0_1_bye_cl","true")
		return true;
	}
	else {
		DEBOUT("TRNSCT_SM_BYE_CL pre_0_1_bye_cl","false")
		return false;
	}

}
ACTION* act_0_1_bye_cl(SM_V5* _sm, MESSAGE* _message) {

	DEBOUT("TRNSCT_SM_BYE_CL act_0_1_bye_cl","")

	NEWPTR(ACTION*, action, ACTION())

	_message->setDestEntity(SODE_NTWPOINT);
	_message->setGenEntity(SODE_TRNSCT_CL);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	//careful with source message.
	DUPLICATEMESSAGE(__timedmessage, _message, SODE_TRNSCT_CL)

	//This is to be sent later, after timer expires
	//Preconfigure message entity points, the alarm manager cannot do this

	//V5?????
	//???????
	__timedmessage->setDestEntity(SODE_TRNSCT_CL);
	__timedmessage->setGenEntity(SODE_TRNSCT_CL);

	SysTime afterT;
	GETTIME(afterT);
	//TODO check if mc is overflowed
	//V5 non funziona!!!
	//DEBOUT("current time ", afterT.tv.tv_sec << "] [" <<afterT.tv.tv_usec)
	afterT.tv.tv_sec = afterT.tv.tv_sec + TIMER_1_sc*(((TRNSCT_SM_BYE_CL*)_sm)->resend_bye+1);
	afterT.tv.tv_usec = afterT.tv.tv_usec + TIMER_1_mc*(((TRNSCT_SM_BYE_CL*)_sm)->resend_bye+1);
	//PRINTTIMESHORT("afterT",afterT)

	__timedmessage->setFireTime(afterT);
	__timedmessage->typeOfInternal = TYPE_OP;
	__timedmessage->typeOfOperation = TYPE_OP_TIMER_ON;
	SingleAction sa_2 = SingleAction(__timedmessage);

	action->addSingleAction(sa_2);

	//V5 TODO
	((TRNSCT_SM_BYE_CL*)_sm)->resend_bye++;

	DEBOUT("TRNSCT_SM_BYE_CL act_0_1_bye_cl","")
	_sm->State = 1;

	return action;
}
bool pre_1_1_bye_cl(SM_V5* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_SM_BYE_CL pre_1_1_inv_cl","")
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == BYE_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_TRNSCT_CL
			&& ((TRNSCT_SM_BYE_CL*)_sm)->resend_bye <= MAX_INVITE_RESEND) {
		DEBOUT("TRNSCT_SM_BYE_CL pre_1_1_bye_cl","true")
		return true;
	}
	else {
		DEBOUT("TRNSCT_SM_BYE_CL pre_1_1_bye_cl","false")
		return false;
	}
}

bool pre_1_2_bye_cl(SM_V5* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_SM_BYE_CL pre_1_4_bye_cl","")

	if (_message->getReqRepType() == REPSUPP
		&&_message->getHeadSipReply().getReply().getCode() == OK_200
		&& _message->getDestEntity() == SODE_TRNSCT_CL
		&& _message->getGenEntity() ==  SODE_NTWPOINT) {
			DEBOUT("TRNSCT_SM_BYE_CL pre_1_4_bye_cl","true")
			return true;
		}
		else {
			DEBOUT("TRNSCT_SM_BYE_CL pre_1_4_bye_cl","false")
			return false;
		}
}
ACTION* act_1_2_bye_cl(SM_V5* _sm, MESSAGE* _message) {

	DEBOUT("TRNSCT_SM_BYE_CL act_3_4_inv_cl","")

	ACTION* action = new ACTION();

	_message->setDestEntity(SODE_ALOPOINT);
	_message->setGenEntity(SODE_TRNSCT_CL);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	DUPLICATEMESSAGE(__message, _message, SODE_TRNSCT_CL)
	__message->typeOfInternal = TYPE_OP;
	__message->typeOfOperation = TYPE_OP_TIMER_OFF;
	SingleAction sa_2 = SingleAction(__message);
	action->addSingleAction(sa_2);

	_sm->State = 2;
	return action;
}
//**********************************************************************************
TRNSCT_SM_BYE_CL::TRNSCT_SM_BYE_CL(int _requestType, MESSAGE* _matrixMess, MESSAGE* _A_Matrix, ENGINE* _sl_cc, SL_CO* _sl_co):
		TRNSCT_SM(_requestType, _matrixMess, _A_Matrix, _sl_cc, _sl_co),
		PA_BYE_0_1CL((SM_V5*)this),
		PA_BYE_1_1CL((SM_V5*)this),
		PA_BYE_1_2CL((SM_V5*)this){

	PA_BYE_0_1CL.action = &act_0_1_bye_cl;
	PA_BYE_0_1CL.predicate = &pre_0_1_bye_cl;

	PA_BYE_1_1CL.action = &act_0_1_bye_cl;
	PA_BYE_1_1CL.predicate = &pre_1_1_bye_cl;

	PA_BYE_1_2CL.action = &act_1_2_bye_cl;
	PA_BYE_1_2CL.predicate = &pre_1_2_bye_cl;

	insert_move(0,&PA_BYE_0_1CL);
	insert_move(1,&PA_BYE_1_1CL);
	insert_move(1,&PA_BYE_1_2CL);

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
////**********************************************************************************
////**********************************************************************************
//PREDICATE_ACTION::PREDICATE_ACTION(SL_SM* _sm){
//	machine = _sm;
//}
////**********************************************************************************
////**********************************************************************************
//ACTION* SL_SM::event(MESSAGE* _event){
//
//	PREDICATE_ACTION* tmp;
//
//	ACTION* act=0x0;
//
//	DEBOUT("SM_SL::event Look for state", State)
//
//	pair<multimap<int,PREDICATE_ACTION*>::iterator,multimap<int,PREDICATE_ACTION*>::iterator> ret;
//
//	multimap<int,PREDICATE_ACTION*>::iterator iter;
//
//	ret = move_sm.equal_range(State);
//
//	pthread_mutex_lock(&mutex);
//
//    for (iter=ret.first; iter!=ret.second; ++iter){
//		tmp  = iter->second;
//
//		if (tmp->predicate(this, _event)){
//			act = tmp->action(this, _event);
//			pthread_mutex_unlock(&mutex);
//			return act;
//		}
//	}
//	pthread_mutex_unlock(&mutex);
//	return(act);
//
//	DEBOUT("SM_SL::exec_it unexpected event", _event)
//}
////**********************************************************************************
////**********************************************************************************
//void SL_SM::insert_move(int _i, PREDICATE_ACTION* _pa){
//
//	DEBOUT("SM_SL::insert_move", _i << " " << _pa )
//	move_sm.insert(pair<int, PREDICATE_ACTION*>(_i, _pa));
//
//}
//SL_SM::SL_SM(ENGINE* _eng, SL_CO* _sl_co){
//
//	DEBOUT("SL_SM::SL_SM", "")
//	sl_cc = _eng;
//    sl_co = _sl_co;
//
//    pthread_mutex_init(&mutex, NULL);
//	State = 0;
//
////	controlSequence = 1;
//}
//ENGINE* SL_SM::getSL_CC(void){
//	return sl_cc;
//}
//SL_CO* SL_SM::getSL_CO(void){
//	return sl_co;
//}
////void SL_SM::setControlSequence(int _s){
////	controlSequence = _s;
////}
////int SL_SM::getControlSequence(void){
////	return controlSequence;
////}
//
////**********************************************************************************
////**********************************************************************************
////**********************************************************************************
////**********************************************************************************
////**********************************************************************************
////**********************************************************************************
//
//bool pre_0_1_sv(SL_SM* _sm, MESSAGE* _message){
//
//	DEBOUT("SM_SV pre_0_1_sv called","")
//	if (_message->getReqRepType() == REQSUPP
//			&& (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST
//					|| _message->getHeadSipRequest().getS_AttMethod().getMethodID() == ACK_REQUEST)
//			&& _message->getDestEntity() == SODE_SMSVPOINT
//			&& _message->getGenEntity() ==  SODE_APOINT) {
//		DEBOUT("SM_SV pre_0_1_sv","true")
//		return true;
//	}
//	else {
//		DEBOUT("SM_SV pre_0_1_sv","false")
//		return false;
//	}
//}
//ACTION* act_0_1_sv(SL_SM* _sm, MESSAGE* _message) {
//
//	DEBOUT("SL_SM_SV::act_0_1_sv", _message->getHeadSipRequest().getContent())
//
//	DEBOUT("SL_SM_SV::act_0_1_sv CSeq", _message->getHeadCSeq().getContent())
//	DEBOUT("SL_SM_SV::act_0_1_sv CSeq", _message->getHeadCSeq().getSequence())
//
//    //_sm->setControlSequence(_message->getHeadCSeq().getSequence());
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
//	DEBOUT("ETRY","SIP/2.0 100 Trying")
//	etry->setHeadSipReply("SIP/2.0 100 Trying");
//
//	etry->dropHeader("Contact:");
//
//	SipUtil.genASideReplyFromRequest(_message, etry);
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
//	DEBOUT("SL_SM_SV::act_0_1_sv move to state 1","")
//	_sm->State = 1;
//
//	return action;
//
//}
////V3
////will do pre_2_2_sv also
//bool pre_1_2_sv(SL_SM* _sm, MESSAGE* _message){
//
//	DEBOUT("SM_SV pre_1_2_sv called","")
//
//	if (_message->getReqRepType() == REPSUPP
//		&& (_message->getHeadSipReply().getReply().getCode() == DIALOGE_101
//				|| _message->getHeadSipReply().getReply().getCode() == RINGING_180)
//		&& _message->getDestEntity() == SODE_SMSVPOINT
//		&& _message->getGenEntity() ==  SODE_SMCLPOINT) {
//			return true;
//			DEBOUT("SM_SV pre_1_2_sv","true")
//		}
//		else {
//			DEBOUT("SM_SV pre_1_2_sv","false")
//			return false;
//		}
//}
//ACTION* act_1_2_sv(SL_SM* _sm, MESSAGE* _message) {
//
//	DEBOUT("SL_SM_SV::act_1_2_sv",  _message->getHeadSipReply().getReply().getCode() )
//	ACTION* action = new ACTION();
//
//	_message->setDestEntity(SODE_APOINT);
//	_message->setGenEntity(SODE_SMSVPOINT);
//	_message->typeOfInternal = TYPE_MESS;
//	SingleAction sa_1 = SingleAction(_message);
//
//	action->addSingleAction(sa_1);
//
//	DEBOUT("SL_SM_SV::act_1_2_sv move to state 2","")
//	_sm->State = 2;
//
//	return action;
//}
////V3
////same for 2->3
//bool pre_1_3_sv(SL_SM* _sm, MESSAGE* _message){
//
//	DEBOUT("SM_SV pre_1_3_sv","")
//
//	if (_message->getReqRepType() == REPSUPP
//		&& _message->getHeadSipReply().getReply().getCode() == OK_200
//		&& _message->getDestEntity() == SODE_SMSVPOINT
//		&& _message->getGenEntity() ==  SODE_ALOPOINT) {
//			DEBOUT("SM_SV pre_2_3_sv","true")
//			return true;
//		}
//		else {
//			DEBOUT("SM_SV pre_2_3_sv","false")
//			return false;
//		}
//}
//ACTION* act_1_3_sv(SL_SM* _sm, MESSAGE* _message) {
//
//	DEBOUT("SM_SV act_2_3_sv called","")
//
//	ACTION* action = new ACTION();
//
//	_message->setDestEntity(SODE_APOINT);
//	_message->setGenEntity(SODE_SMSVPOINT);
//	_message->typeOfInternal = TYPE_MESS;
//	SingleAction sa_1 = SingleAction(_message);
//
//	action->addSingleAction(sa_1);
//
//	//TODO
//	// start here timer for resend 200OK until ACK
//	// arrives
//
//
//	DEBOUT("SM_SV act_2_3_sv move to state 3","")
//	_sm->State = 3;
//
//	return action;
//
//}
////predicate use: 0->1
//ACTION* act_3_4_sv(SL_SM* _sm, MESSAGE* _message) {
//
//	DEBOUT("SM_SV act_3_4_sv called","")
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
//	DEBOUT("SM_SV act_3_4_sv move to state 4","")
//	_sm->State = 4;
//
//	return action;
//
//}
//bool pre_4_6_sv(SL_SM* _sm, MESSAGE* _message){
//
//	DEBOUT("SM_SV pre_4_6_sv called","")
//	if (_message->getReqRepType() == REQSUPP
//			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == BYE_REQUEST
//			&& _message->getDestEntity() == SODE_SMSVPOINT
//			&& _message->getGenEntity() ==  SODE_APOINT) {
//		DEBOUT("SM_SV pre_4_6_sv","true")
//		return true;
//	}
//	else {
//		DEBOUT("SM_SV pre_4_6_sv","false")
//		return false;
//	}
//}
//ACTION* act_4_6_sv(SL_SM* _sm, MESSAGE* _message) {
//
//	DEBOUT("SL_SM_SV::act_4_6_sv", _message->getHeadSipRequest().getContent())
//
//	ACTION* action = new ACTION();
//
//	_message->setDestEntity(SODE_ALOPOINT);
//	_message->setGenEntity(SODE_SMSVPOINT);
//	_message->typeOfInternal = TYPE_MESS;
//	SingleAction sa_1 = SingleAction(_message);
//
//	action->addSingleAction(sa_1);
//
//	DEBOUT("SM_SV act_4_6_sv move to state 6","")
//
//	_sm->State = 6;
//
//	return action;
//
//}
//SL_SM_SV::SL_SM_SV(ENGINE* _eng, SL_CO* _sl_co):
//		SL_SM(_eng, _sl_co),
//		P0_1SV((SL_SM*)this),
//		P1_2SV((SL_SM*)this),
//		P2_2SV((SL_SM*)this),
//		P2_3SV((SL_SM*)this),
//		P1_3SV((SL_SM*)this),
//		P3_4SV((SL_SM*)this),
//		P4_6SV((SL_SM*)this){
//
//	P0_1SV.action = &act_0_1_sv;
//	P0_1SV.predicate = &pre_0_1_sv;
//
//	P1_2SV.action = &act_1_2_sv;
//	P1_2SV.predicate = &pre_1_2_sv;
//
//	P1_3SV.action = &act_1_3_sv;
//	P1_3SV.predicate = &pre_1_3_sv;
//
//	P2_2SV.action = &act_1_2_sv;
//	P2_2SV.predicate = &pre_1_2_sv;
//
//
//	P2_3SV.action = &act_1_3_sv;
//	P2_3SV.predicate = &pre_1_3_sv;
//
//	P3_4SV.action = &act_3_4_sv;
//	P3_4SV.predicate = &pre_0_1_sv;
//
//	P4_6SV.action = &act_4_6_sv;
//	P4_6SV.predicate = &pre_4_6_sv;
//
//
//	insert_move(0,&P0_1SV);
//	insert_move(1,&P1_2SV);
//	insert_move(1,&P1_3SV);
//	insert_move(2,&P2_2SV);
//	insert_move(2,&P2_3SV);
//	insert_move(3,&P3_4SV);
//	insert_move(4,&P4_6SV);
//
//
//}
////*****************************************************************
//// client sm
////*****************************************************************
//bool pre_0_1_cl(SL_SM* _sm, MESSAGE* _message){
//
//	DEBOUT("SM_CL pre_0_1_cl","")
//	if (_message->getReqRepType() == REQSUPP
//			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST
//			&& _message->getDestEntity() == SODE_SMCLPOINT
//			&& _message->getGenEntity() ==  SODE_ALOPOINT) {
//		DEBOUT("SM_CL pre_0_1_cl","true")
//		return true;
//	}
//	else {
//		DEBOUT("SM_CL pre_0_1_cl","false")
//		return false;
//	}
//}
//ACTION* act_0_1_cl(SL_SM* _sm, MESSAGE* _message) {
//
//	DEBOUT("SM_CL act_0_1_cl","")
//
//	ACTION* action = new ACTION();
//
//	_message->setDestEntity(SODE_BPOINT);
//	_message->setGenEntity(SODE_SMCLPOINT);
//	_message->typeOfInternal = TYPE_MESS;
//	SingleAction sa_1 = SingleAction(_message);
//
//	action->addSingleAction(sa_1);
//
//	//careful with source message.
//	DUPLICATEMESSAGE(__message, _message, SODE_SMCLPOINT)
//
//	//This is to be sent later, after timer expires
//	//Preconfigure message entity points, the alarm manager cannot do this
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
//	action->addSingleAction(sa_2);
//
//	((SL_SM_CL*)_sm)->resend_invite++;
//
//	DEBOUT("SM_CL act_0_1_cl to state 1","")
//	_sm->State = 1;
//
//	return action;
//
//}
//bool pre_1_2_cl(SL_SM* _sm, MESSAGE* _message){
//
//	DEBOUT("SM_CL pre_1_2_cl","")
//
//	if (_message->getReqRepType() == REPSUPP
//		&&_message->getHeadSipReply().getReply().getCode() == TRYING_100
//		&& _message->getDestEntity() == SODE_SMCLPOINT
//		&& _message->getGenEntity() ==  SODE_BPOINT) {
//			DEBOUT("SM_CL pre_1_2_cl","true")
//			return true;
//		}
//		else {
//			DEBOUT("SM_CL pre_1_2_cl","false")
//			return false;
//		}
//}
//ACTION* act_1_2_cl(SL_SM* _sm, MESSAGE* _message) {
//
//	DEBOUT("SM_CL act_1_2_cl",_message->getHeadSipReply().getReply().getCode())
//
//	// TODO clear timer ad create new timer for the ringing ?
//
//	ACTION* action = new ACTION();
//	_message->typeOfInternal = TYPE_OP;
//	_message->typeOfOperation = TYPE_OP_TIMER_OFF;
//	SingleAction sa_1 = SingleAction(_message);
//	action->addSingleAction(sa_1);
//
//	_sm->State = 2;
//	return action;
//}
////same for 2->3
//bool pre_1_3_cl(SL_SM* _sm, MESSAGE* _message){
//
//	DEBOUT("SM_CL pre_1_3_cl","")
//
//	if (_message->getReqRepType() == REPSUPP
//		&& (_message->getHeadSipReply().getReply().getCode() == DIALOGE_101
//				|| _message->getHeadSipReply().getReply().getCode() == RINGING_180)
//		&& _message->getDestEntity() == SODE_SMCLPOINT
//		&& _message->getGenEntity() ==  SODE_BPOINT) {
//			DEBOUT("SM_CL pre_1_3_cl","true")
//			return true;
//		}
//		else {
//			DEBOUT("SM_CL pre_1_3_cl","false")
//			return false;
//		}
//}
//ACTION* act_1_3_cl(SL_SM* _sm, MESSAGE* _message) {
//
//	DEBOUT("SM_CL act_1_3_cl","")
//
//	ACTION* action = new ACTION();
//
//	// the message contains the to tag that we must save
//	// or store it in valo during 200ok
//
////	MESSAGE* __message = _sm->getSL_CO()->call_oset->getInviteA();
////	MESSAGE* __message = _sm->getSL_CO()->call_oset->getTransactionY(_message->getHeadCSeq().getSequence())->getMatrixMessage();
//
//	DEBOUT("MESSAGE GENERATOR", __message)
//
//
//	CREATEMESSAGE(reply_x, __message, SODE_SMSVPOINT)
//	reply_x->setDestEntity(SODE_SMSVPOINT);
//	reply_x->setGenEntity(SODE_SMCLPOINT);
//	reply_x->typeOfInternal = TYPE_MESS;
//
//
//	DEBOUT("CONTACT", reply_x->getHeadContact().getContent())
//
//	reply_x->replaceHeadContact("<sip:sipsl@grog:5060>");
//
//	DEBOUT("NEW CONTACT", reply_x->getHeadContact().getContent())
//
//	DEBOUT("Purge SDP","")
//	reply_x->purgeSDP();
//
//	SipUtil.genASideReplyFromBReply(_message, __message, reply_x);
//	reply_x->compileMessage();
//	reply_x->dumpVector();
//
//	//C_HeadVia* viatmp2 = (C_HeadVia*) reply_x->getSTKHeadVia().top();
//
//	SingleAction sa_1 = SingleAction(reply_x);
//
//	action->addSingleAction(sa_1);
//
//	_sm->State = 3;
//	return action;
//
//
//}
////ok for 2->4 and 3->4
//bool pre_1_4_cl(SL_SM* _sm, MESSAGE* _message){
//
//	DEBOUT("SM_CL pre_1_4_cl","")
//
//	if (_message->getReqRepType() == REPSUPP
//		&&_message->getHeadSipReply().getReply().getCode() == OK_200
//		&& _message->getDestEntity() == SODE_SMCLPOINT
//		&& _message->getGenEntity() ==  SODE_BPOINT) {
//			DEBOUT("SM_CL pre_1_4_cl","true")
//			return true;
//		}
//		else {
//			DEBOUT("SM_CL pre_1_4_cl","false")
//			return false;
//		}
//}
//ACTION* act_1_4_cl(SL_SM* _sm, MESSAGE* _message) {
//
//	DEBOUT("SM_CL act_1_4_cl","")
//
//	ACTION* action = new ACTION();
//
//	_message->setDestEntity(SODE_ALOPOINT);
//	_message->setGenEntity(SODE_SMCLPOINT);
//	_message->typeOfInternal = TYPE_MESS;
//	SingleAction sa_1 = SingleAction(_message);
//
//	action->addSingleAction(sa_1);
//
//	DUPLICATEMESSAGE(__message, _message, SODE_SMCLPOINT)
//	__message->typeOfInternal = TYPE_OP;
//	__message->typeOfOperation = TYPE_OP_TIMER_OFF;
//	SingleAction sa_2 = SingleAction(__message);
//	action->addSingleAction(sa_2);
//
//	_sm->State = 4;
//	return action;
//}
//bool pre_4_5_cl(SL_SM* _sm, MESSAGE* _message){
//
//	DEBOUT("SM_CL pre_4_5_cl","")
//	if (_message->getReqRepType() == REQSUPP
//			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == ACK_REQUEST
//			&& _message->getDestEntity() == SODE_SMCLPOINT
//			&& _message->getGenEntity() ==  SODE_ALOPOINT) {
//		DEBOUT("SM_CL pre_4_5_cl","true")
//		return true;
//	}
//	else {
//		DEBOUT("SM_CL pre_4_5_cl","false")
//		return false;
//	}
//}
//ACTION* act_4_5_cl(SL_SM* _sm, MESSAGE* _message) {
//
//	DEBOUT("SM_CL act_4_5_cl","")
//
//	ACTION* action = new ACTION();
//
//	_message->setDestEntity(SODE_BPOINT);
//	_message->setGenEntity(SODE_SMCLPOINT);
//	_message->typeOfInternal = TYPE_MESS;
//	SingleAction sa_1 = SingleAction(_message);
//
//	action->addSingleAction(sa_1);
//
//	_sm->State = 5;
//
//	return action;
//
//}
//bool pre_5_7_cl(SL_SM* _sm, MESSAGE* _message){
//
//	DEBOUT("SM_CL pre_5_7_cl","")
//	if (_message->getReqRepType() == REQSUPP
//			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == BYE_REQUEST
//			&& _message->getDestEntity() == SODE_SMCLPOINT
//			&& _message->getGenEntity() ==  SODE_ALOPOINT) {
//		DEBOUT("SM_CL pre_5_7_cl","true")
//		return true;
//	}
//	else {
//		DEBOUT("SM_CL pre_5_7_cl","false")
//		return false;
//	}
//}
//ACTION* act_5_7_cl(SL_SM* _sm, MESSAGE* _message) {
//
//	DEBOUT("SM_CL act_5_7_cl","")
//
//	ACTION* action = new ACTION();
//
//	_message->setDestEntity(SODE_BPOINT);
//	_message->setGenEntity(SODE_SMCLPOINT);
//	_message->typeOfInternal = TYPE_MESS;
//	SingleAction sa_1 = SingleAction(_message);
//
//	action->addSingleAction(sa_1);
//
//	_sm->State = 7;
//
//	return action;
//
//}
//SL_SM_CL::SL_SM_CL(ENGINE* _eng, SL_CO* _sl_co):
//		SL_SM(_eng, _sl_co),
//		P0_1CL((SL_SM*)this),
//		P1_2CL((SL_SM*)this),
//		P1_3CL((SL_SM*)this),
//		P1_4CL((SL_SM*)this),
//		P3_3CL((SL_SM*)this),
//		P3_4CL((SL_SM*)this),
//		P2_3CL((SL_SM*)this),
//		P2_4CL((SL_SM*)this),
//		P4_5CL((SL_SM*)this),
//		P5_7CL((SL_SM*)this),
//		P7_8CL((SL_SM*)this){
//
//	resend_invite = 0;
//
//	P0_1CL.action = &act_0_1_cl;
//	P0_1CL.predicate = &pre_0_1_cl;
//
//	P1_2CL.action = &act_1_2_cl;
//	P1_2CL.predicate = &pre_1_2_cl;
//
//	P1_3CL.action = &act_1_3_cl;
//	P1_3CL.predicate = &pre_1_3_cl;
//
//	P1_4CL.action = &act_1_4_cl;
//	P1_4CL.predicate = &pre_1_4_cl;
//
//	P3_3CL.action = &act_1_3_cl;
//	P3_3CL.predicate = &pre_1_3_cl;
//
//	P3_4CL.action = &act_1_4_cl;
//	P3_4CL.predicate = &pre_1_4_cl;
//
//	P2_3CL.action = &act_1_3_cl;
//	P2_3CL.predicate = &pre_1_3_cl;
//
//	P2_4CL.action = &act_1_4_cl;
//	P2_4CL.predicate = &pre_1_4_cl;
//
//	P4_5CL.action = &act_4_5_cl;
//	P4_5CL.predicate = &pre_4_5_cl;
//
//	P5_7CL.action = &act_5_7_cl;
//	P5_7CL.predicate = &pre_5_7_cl;
//
//	P7_8CL.action = &act_1_4_cl;
//	P7_8CL.predicate = &pre_1_4_cl;
//
//	insert_move(0,&P0_1CL);
//	insert_move(1,&P1_2CL);
//	insert_move(1,&P1_3CL);
//	insert_move(1,&P1_4CL);
//	insert_move(3,&P3_3CL);
//	insert_move(3,&P3_4CL);
//	insert_move(2,&P2_3CL);
//	insert_move(2,&P2_4CL);
//	insert_move(4,&P4_5CL);
//	insert_move(5,&P5_7CL);
//	insert_move(7,&P7_8CL);
//
//
//}

