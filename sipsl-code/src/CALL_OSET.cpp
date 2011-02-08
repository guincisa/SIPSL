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
#ifndef MESSAGE_H
#include "MESSAGE.h"
#endif
#ifndef SPIN_H
#include "SPIN.h"
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
#ifndef ACTION_H
#include "ACTION.h"
#endif
#ifndef DOA_H
#include "DOA.h"
#endif
#ifndef SUDP_H
#include "SUDP.h"
#endif
#ifndef CALL_OSET_H
#include "CALL_OSET.h"
#endif
#ifndef COMAP_H
#include "COMAP.h"
#endif
#ifndef ALO_H
#include "ALO.h"
#endif
#ifndef VALO_H
#include "VALO.h"
#endif
#ifndef SIP_PROPERTIES_H
#include "SIP_PROPERTIES.h"
#endif
#ifndef ALARM_H
#include "ALARM.h"
#endif
#ifndef TRNSCT_SM_H
#include "TRNSCT_SM.h"
#endif
#ifndef SIPUTIL_H
#include "SIPUTIL.h"
#endif
#ifndef TRNSPRT_H
#include "TRNSPRT.h"
#endif

//static SIPUTIL SipUtil;

//**********************************************************************************
//**********************************************************************************
// CALL_OSET
//**********************************************************************************
CALL_OSET::CALL_OSET(ENGINE* _engine, TRNSPRT* _transport, string _call){

    pthread_mutex_init(&mutex, NULL);
	GETLOCK(&(mutex),"CALL_OSET::mutex");


	engine = _engine;
	sequenceMap.insert(pair<string, int>("ACK_B",0));
	sequenceMap.insert(pair<string, int>("INVITE_B",0));
	sequenceMap.insert(pair<string, int>("BYE_B",0));

	sl_co = 0x0;
	alo = 0x0;
	lastTRNSCT_SM_ACK_CL = 0x0;

	NEWPTR2(sl_co, SL_CO(this), "SL_CO(this)")
	NEWPTR2(alo, VALO(_engine, this), "VALO(_engine, this)")
	alo->linkSUDP(_engine->getSUDP());
	callId_X = _call;
	callId_Y = "";

	transport = _transport;

	DEBOUT("CALL_OSET sequenceMap", &sequenceMap)
	DEBOUT("CALL_OSET trnsctSmMap", &trnsctSmMap)

	RELLOCK(&(mutex),"SL_CO::mutex");

}
CALL_OSET::~CALL_OSET(void){

	//Need to lock here!
	GETLOCK(&(mutex),"CALL_OSET::mutex");


	DEBOUT("CALL_OSET ACCESS CALL_OSET::~CALL_OSET begin", this)
	if (sl_co != 0x0){
		DEBY
		//purge states machines
		//must delete specific SM!
		map<string, TRNSCT_SM*> ::iterator p;
		for (p = trnsctSmMap.begin() ; p != trnsctSmMap.end() ; p++) {
			DEBOUT("(p->first)", (p->first))
			if ( (p->first).substr(0,8).compare("INVITE#4") == 0){
				DELPTR((TRNSCT_SM_INVITE_CL*)p->second,"(TRNSCT_SM_INVITE_CL*)p->second");
				continue;
			}
			if ( (p->first).substr(0,8).compare("INVITE#5") == 0){
				DELPTR((TRNSCT_SM_INVITE_SV*)p->second,"(TRNSCT_SM_INVITE_SV*)p->second");
				continue;
			}
			if ( (p->first).substr(0,5).compare("ACK#4") == 0){
				DELPTR((TRNSCT_SM_ACK_CL*)p->second,"(TRNSCT_SM_ACK_CL*)p->second");
				continue;
			}
			if ( (p->first).substr(0,5).compare("ACK#5") == 0){
				DELPTR((TRNSCT_SM_ACK_SV*)p->second,"(TRNSCT_SM_ACK_SV*)p->second");
				continue;
			}
			if ( (p->first).substr(0,5).compare("BYE#4") == 0){
				DELPTR((TRNSCT_SM_BYE_CL*)p->second,"(TRNSCT_SM_BYE_CL*)p->second");
				continue;
			}
			if ( (p->first).substr(0,5).compare("BYE#5") == 0){
				DELPTR((TRNSCT_SM_BYE_SV*)p->second,"(TRNSCT_SM_BYE_SV*)p->second");
				continue;
			}

		}
		DEBY
	}
	if (alo != 0x0){
		DELPTR((VALO*)alo, "VALO");
	}

	//TODO if the message is a source message for a message which in ALARM
	//and this one triggers it, it will crash?
	MESSAGE* m = getNextLockedMessage();
	while (m != MainMessage){

		DEBOUT("MESSAGE to be deleted", m)
		if (!m->getLock()){
			DEBASSERT("CALL_OSET::~CALL_OSET message in lockedmessage tabelk found unlocked ["<< m <<"]")
		}

		//TODO DEBCODE
		//checks if this message is still in global table

#ifdef DEBCODE
		map<const MESSAGE*, MESSAGE*>::iterator p;
		int ixx = getModulus(m);
		pthread_mutex_lock(&messTableMtx[ixx]);
		p = globalMessTable[ixx].find(m);
		if (p ==globalMessTable[ixx].end()){
			DEBWARNING("Message already deleted",m)
		}
		pthread_mutex_unlock(&messTableMtx[ixx]);
#endif

		DEBMESSAGESHORT("DOA locked message", m)
		if (m->getTypeOfInternal() == TYPE_OP){
			string callid_alarm = m->getHeadCallId().getContent() +  ((C_HeadVia*) m->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch") + "#" + m->getOrderOfOperation()+ "#";
			DEBOUT("CALL_OSET::~CALL_OSET::cancel alarm, callid", callid_alarm)
			getENGINE()->getSUDP()->getAlmgr()->cancelAlarm(callid_alarm);
		}
		//TODO what if the message is being triggered now?
		m->unSetLock(this);
		PURGEMESSAGE(m);
		DEBY
		m = getNextLockedMessage();
		DEBOUT("Message to be deleted", m)
	}
	DELPTR(sl_co, "SL_CO")
	DEBY
	RELLOCK(&(mutex),"SL_CO::mutex");

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
	map<string, int> ::iterator p;
	p = sequenceMap.find(_method);
	if (p != sequenceMap.end()){
		DEBOUT("CALL_OSET::insertSequence exists", _method <<"]["<<_i)
		sequenceMap.erase(p);
	}

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
TRNSPRT* CALL_OSET::getTRNSPRT(void){
	return transport;
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

void CALL_OSET::insertLockedMessage(MESSAGE* _message){
	DEBMESSAGESHORT("Insert locked message", _message)
	map<MESSAGE*,int>::iterator i;
	i = lockedMessages.find(_message);
	if (i !=lockedMessages.end()){
		return;
	}else {
		lockedMessages.insert(pair<MESSAGE*,int>(_message,0));
	}
	return;
}
MESSAGE* CALL_OSET::getNextLockedMessage(void){

	DEBY
	map<MESSAGE*,int>::iterator p;
	p=lockedMessages.begin();
	if (p!=lockedMessages.end()){
		MESSAGE* t = (MESSAGE*)p->first;
		lockedMessages.erase(p);
		DEBY
		return t;
	}
	DEBY
	return MainMessage;

}
void CALL_OSET::removeLockedMessage(MESSAGE* _message){

	map<MESSAGE*,int>::iterator p;
	p=lockedMessages.find(_message);
	if (p!=lockedMessages.end()){
		DEBOUT("CALL_OSET::removeLockedMessage found", _message)
		//MESSAGE* t = (MESSAGE*)p->first;
		lockedMessages.erase(p);
	}
	else {
		DEBOUT("CALL_OSET::removeLockedMessage not found", _message)
	}

}
bool CALL_OSET::isLockedMessage(MESSAGE* _message){
	map<MESSAGE*,int>::iterator p;
	p=lockedMessages.find(_message);

	return ( p!=lockedMessages.end() );

}
void CALL_OSET::dumpTrnsctSm(void){

	map<string, TRNSCT_SM*> ::iterator p;
	p = trnsctSmMap.begin();
	for ( p = trnsctSmMap.begin(); p != trnsctSmMap.end() ; p++){
		DEBOUT("TRNSCT MAP", p->first << "]["<<(TRNSCT_SM*)p->second << "][" << ((TRNSCT_SM*)p->second)->getId())
	}
}
//**********************************************************************************
TRNSCT_SM* CALL_OSET::getTrnsctSm(string _method, int _sode, string _branch){

	DEBOUT_UTIL("CALL_OSET::getTrnsctSm",_method <<"#"<< _sode <<"#"<<_branch << "] call_oset ["<<this)
	char t_key[512];
	sprintf(t_key, "%s#%d#%s", _method.c_str(), _sode,_branch.c_str());

	string stmp = t_key;

	DEBOUT_UTIL("CALL_OSET::getTrnsctSm", stmp)

	map<string, TRNSCT_SM*> ::iterator p;
	p = trnsctSmMap.find(stmp);
	if (p != trnsctSmMap.end()){
		DEBOUT_UTIL("CALL_OSET::getTrnsctSm found",_method <<"#"<< _sode <<"#"<<_branch << "["<<(TRNSCT_SM*)p->second<<"]")
		return ((TRNSCT_SM*)p->second);
	}else {
		return 0x0;
	}
}
//**********************************************************************************
void CALL_OSET::addTrnsctSm(string _method, int _sode, string _branch, TRNSCT_SM* _trnsctSm){

	DEBOUT_UTIL("CALL_OSET::addTrnsctSm",_method <<"#"<< _sode <<"#"<<_branch << "] ["<<_trnsctSm<<"] call_oset ["<<this)
	char t_key[512];
	sprintf(t_key, "%s#%d#%s", _method.c_str(), _sode, _branch.c_str());

	string stmp = t_key;
	DEBOUT_UTIL("CALL_OSET::addTrnsctSm", stmp)

	_trnsctSm->setId(stmp);

	map<string, TRNSCT_SM*>::iterator it;
	it = trnsctSmMap.find(stmp);
	if (it != trnsctSmMap.end()){
		DEBOUT("CALL_OSET::addTrnsctSm adding a exsiting sm", stmp <<"]["<<_trnsctSm)
		DEBASSERT("CALL_OSET::addTrnsctSm")
	}
	trnsctSmMap.insert(pair<string, TRNSCT_SM*>(stmp, _trnsctSm));

	// special for client sm Ack
	if (_method.substr(0,3).compare("ACK") == 0 && _sode == SODE_TRNSCT_CL ){
		DEBOUT_UTIL("CALL_OSET::addTrnsctSm special Ack sm cl pointer",_method <<"#"<< _sode <<"#"<<_branch <<"#"<<_trnsctSm)
		lastTRNSCT_SM_ACK_CL = _trnsctSm;
	}	return;
}
void CALL_OSET::call(MESSAGE* _message){
	GETLOCK(&(mutex),"CALL_OSET::mutex");
	sl_co->call(_message);
	RELLOCK(&(mutex),"CALL_OSET::mutex");
}
int CALL_OSET::getOverallState_CL(void){
	return sl_co->OverallState_CL;
}
int CALL_OSET::getOverallState_SV(void){
	return sl_co->OverallState_SV;
}

//**********************************************************************************
//**********************************************************************************
// SL_CO
//**********************************************************************************
SL_CO::SL_CO(CALL_OSET* _call_oset){
	call_oset = _call_oset;
    OverallState_SV = OS_INIT;
    OverallState_CL = OS_INIT;

}
//**********************************************************************************
//**********************************************************************************
void SL_CO::call(MESSAGE* _message){


	PRTIME
	DEBMESSAGE("SL_CO::call incoming", _message)

    ACTION* action = 0x0;

	//Rule: message from Alarm must be TYPE_OP
	//Rule: message from Alarm must be locked and unlocked here
	if(_message->getTypeOfInternal() == TYPE_OP && 	_message->getTypeOfOperation() !=  TYPE_OP_SMCOMMAND){
		if (_message->getLock()){
			_message->unSetLock(call_oset);

			//TIMER_S
			//_message->setTypeOfInternal(TYPE_MESS);

		}else {
			DEBASSERT("Break rule: a message from alarm is found unlocked")
		}
	}


	//Message is going to Server SM
	int dest = _message->getDestEntity();

	if (dest == SODE_TRNSCT_SV) {

		DEBOUT("SL_CO::search for transaction state machine", _message->getHeadCallId().getContent())

		TRNSCT_SM* trnsctSM = 0x0;

		//First look for an existing SM using METHOD+SM_SV+branch
		DEBDEV("((C_HeadVia*)_message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue(\"branch\")",((C_HeadVia*)_message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch"))
    	trnsctSM = call_oset->getTrnsctSm(_message->getHeadCSeq().getMethod().getContent(), SODE_TRNSCT_SV, ((C_HeadVia*)_message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch"));
    	DEBY
    	//There are no sm, create it
		if (trnsctSM == 0x0 ){
			if (_message->getReqRepType() == REQSUPP && _message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST && OverallState_SV == OS_INIT){
				call_oset->insertSequence("INVITE_A", _message->getHeadCSeq().getSequence());
				NEWPTR2(trnsctSM, TRNSCT_SM_INVITE_SV(_message->getHeadSipRequest().getS_AttMethod().getMethodID(), _message, call_oset->getENGINE(), this),"TRNSCT_SM_INVITE_SV")
			}
			else if (_message->getReqRepType() == REQSUPP && _message->getHeadSipRequest().getS_AttMethod().getMethodID() == ACK_REQUEST && OverallState_SV == OS_COMPLETED){
				NEWPTR2(trnsctSM, TRNSCT_SM_ACK_SV(_message->getHeadSipRequest().getS_AttMethod().getMethodID(), _message, call_oset->getENGINE(), this),"TRNSCT_SM_ACK_SV")
			}
			else if (_message->getRequestDirection() == SODE_FWD && _message->getReqRepType() == REQSUPP && _message->getHeadSipRequest().getS_AttMethod().getMethodID() == BYE_REQUEST && OverallState_SV == OS_CONFIRMED){
				NEWPTR2(trnsctSM, TRNSCT_SM_BYE_SV(_message->getHeadSipRequest().getS_AttMethod().getMethodID(), _message, call_oset->getENGINE(), this),"TRNSCT_SM_BYE_SV")
			}
			//TODO To test
			else if (_message->getRequestDirection() == SODE_BKWD && _message->getReqRepType() == REQSUPP && _message->getHeadSipRequest().getS_AttMethod().getMethodID() == BYE_REQUEST && OverallState_CL == OS_COMPLETED){
				NEWPTR2(trnsctSM, TRNSCT_SM_BYE_SV(_message->getHeadSipRequest().getS_AttMethod().getMethodID(), _message, call_oset->getENGINE(), this),"TRNSCT_SM_BYE_SV")
			}
			else if (_message->getReqRepType() == REPSUPP ){
				// but the call object has been recognized!!!
				DEBOUT("A unrecognized reply directed to server has reached the call object","")
				DEBASSERT("Unrecognized Reply message sent to SV machine")
			}
			else {
				// SL_CO not in correct state
				DEBOUT("Unexpected message ignored", _message)
				if(_message->getLock()){
					DEBASSERT("Unexpected message to be ignored found locked")
				}
				int t = _message->getModulus();
				PURGEMESSAGE(_message)
				((SL_CC*)call_oset->getENGINE())->getCOMAP()->setDoaRequested(call_oset, t);
				return;
			}
			DEBOUT("call_oset->addTrnsctSm", _message->getHeadCSeq().getMethod().getContent() << " " << ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch"))
			call_oset->addTrnsctSm(_message->getHeadCSeq().getMethod().getContent(), SODE_TRNSCT_SV, ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch"), trnsctSM);
			DEBOUT("call_oset->addTrnsctSm","done")

		}
    	if (trnsctSM != 0x0){

			//send the message to sm
			action = trnsctSM->event(_message);

			if (action != 0x0){

				actionCall_SV(action);

			}
			else {

				//_message must be deleted using an action and sode_kill
				DEBASSERT("Action cannot be null anymore")

				DEBOUT("SL_CO::event", "action is null nothing, event ignored")
				//Delete only if not locked
				if (!_message->getLock()){
					PURGEMESSAGE(_message)
				} else {
					//TODO
					//The message is locked for some reason but did not trigger any action...
					//can be the 200 ok A
					DEBMESSAGESHORT("Breaks rule: the message is locked for some reason but did not trigger any action...",_message)
					DEBASSERT("Check this case out")
				}

			}
		}
    	else {
    		DEBMESSAGE("SL_CO::call the message has no SM clients *****", _message)
			DEBASSERT("Check this case out")
    	}
	}
	//Message is going to Client SM
	else if (dest == SODE_TRNSCT_CL){

            string callidys = _message->getHeadCallId().getContent();
	    DEBOUT("SL_CO::call client state machine", callidys)


		TRNSCT_SM* trnsct_cl = 0x0;

		//Get into the ack cl state machine
		if (_message->getTypeOfInternal() == TYPE_OP && _message->getTypeOfOperation() == TYPE_OP_SMCOMMAND){
			DEBOUT("lastTRNSCT_SM_ACK_CL", call_oset->lastTRNSCT_SM_ACK_CL)
			trnsct_cl = call_oset->lastTRNSCT_SM_ACK_CL;
			if ( trnsct_cl == 0x0){
				DEBASSERT("call_oset->lastTRNSCT_SM_ACK_CL NULL")
			}

		}else {
			DEBY
			//Replies are recognized here
			trnsct_cl = call_oset->getTrnsctSm(_message->getHeadCSeq().getMethod().getContent(), SODE_TRNSCT_CL, ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch"));
		}

		if (trnsct_cl == 0x0){

			DEBOUT("Creating Trnsct Client machine callidy", callidys)
			// All those request are generated by ALO
			if(_message->getReqRepType() == REQSUPP){
				if (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST){
					NEWPTR2(trnsct_cl, TRNSCT_SM_INVITE_CL(_message->getHeadSipRequest().getS_AttMethod().getMethodID(), _message, _message->getSourceMessage(), call_oset->getENGINE(), this),"TRNSCT_SM_INVITE_CL")

				}
				else if (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == ACK_REQUEST){
					NEWPTR2(trnsct_cl, TRNSCT_SM_ACK_CL(_message->getHeadSipRequest().getS_AttMethod().getMethodID(), _message, _message->getSourceMessage(), call_oset->getENGINE(), this),"TRNSCT_SM_ACK_CL")
					if (call_oset->lastTRNSCT_SM_ACK_CL != 0x0){
						DEBASSERT("An ACK CL SM already exists")
					}
					call_oset->lastTRNSCT_SM_ACK_CL = trnsct_cl;
				}
				else if (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == BYE_REQUEST){
					NEWPTR2(trnsct_cl, TRNSCT_SM_BYE_CL(_message->getHeadSipRequest().getS_AttMethod().getMethodID(), _message, _message->getSourceMessage(), call_oset->getENGINE(), this),"TRNSCT_SM_BYE_CL")
				}

				call_oset->addTrnsctSm(_message->getHeadCSeq().getMethod().getContent(), SODE_TRNSCT_CL, ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch"), trnsct_cl);

				//This is needed when a new request is coming from ALO
				//B side call ID created by ALO has to be associated to the main call_id (from Aside)
				SL_CC* tmp_sl_cc = (SL_CC*)call_oset->getENGINE();
				tmp_sl_cc->getCOMAP()->setY2XCallId(callidys,call_oset->getCallId_X(), _message->getModulus());

			}else{
				// but the call object has been recognized!!!
				// the sm may have been deleted
				DEBWARNING("An unexpected reply directed to client has reached the call object", _message)
				call_oset->dumpTrnsctSm();
				action = 0x0;
				DEBASSERT("An unexpected reply directed to client has reached the call object")
			}
		}

		action = trnsct_cl->event(_message);

		if (action != 0x0){

			actionCall_CL(action);

		}
		else {
			//_message must be deleted using an action and sode_kill

			// we may receive the message from above: An unexpected reply directed to client has reached the call object

			//TODO we mayn receive an alarm that is expired
			//so the sm has ignored it
			DEBOUT("SL_CO::event", "action is null nothing, event ignored")
			if (!_message->getLock()){
				//TODO why I do this?
				_message->unSetLock(call_oset);
				PURGEMESSAGE(_message)
			}
			else {
				DEBASSERT("Rule break: a message with no associated actions has been found locked")
			}
		}
	}

	DEBOUT("SL_CO::call ended","")
	if (action != 0x0){
		DEBOUT("SL_CO::call delete action","")
		DELPTR(action,"ACTION");
	}
	//RELLOCK(&mutex,"mutex");
	return;
}

void SL_CO::actionCall_SV(ACTION* action){
	// now get actions one by one
	stack<SingleAction> actionList = action->getActionList();

	while (!actionList.empty()){

		MESSAGE* _tmpMessage = actionList.top().getMessage();
		DEBMESSAGE("SL_CO::reading action stack server, message:", _tmpMessage)

		if (_tmpMessage->getDestEntity() == SODE_KILL){
			if (_tmpMessage->getLock()){
				DEBASSERT("Locked message directed to SODE_KILL")
			}
			else{
				PURGEMESSAGE(_tmpMessage)
			}
		}

		else if (_tmpMessage->getDestEntity() == SODE_KILLDOA){
			if (_tmpMessage->getLock()){
				DEBASSERT("Locked message directed to SODE_KILLDOA")
			}
			else{
				DEBOUT("SL_CO::actionCall_SV setDoaRequested",call_oset)
				((SL_CC*)(call_oset->getENGINE()))->getCOMAP()->setDoaRequested(call_oset, _tmpMessage->getModulus());
				PURGEMESSAGE(_tmpMessage)
			}
		}

		//TODO this leaks ?


		else if (_tmpMessage->getTypeOfInternal() == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_ALOPOINT){
			//To ALO
			DEBOUT("SL_CO::call action is send to ALO", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getDialogExtendedCID())
			call_oset->getALO()->call(_tmpMessage);
			if(!_tmpMessage->getLock()){
				DEBOUT("Message coming back form ALO not locked, deleted", _tmpMessage)
				PURGEMESSAGE(_tmpMessage)
			}
		}
		else if (_tmpMessage->getTypeOfInternal() == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_TRNSCT_CL){
			//server sm sending to client sm should not happen
			DEBOUT("SL_CO::call action SV to CL directly","")
			DEBASSERT("Server sm sending to client sm should not happen ")
		}
		else if (_tmpMessage->getTypeOfInternal() == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_NTWPOINT){
			//To network
			DEBDEV("Send to transport", _tmpMessage)
			call_oset->getTRNSPRT()->parse(_tmpMessage);

		} else if (_tmpMessage->getTypeOfInternal() == TYPE_OP){

			if ( _tmpMessage->getTypeOfOperation() == TYPE_OP_TIMER_ON){

				DEBOUT("SL_CO::call action is send to ALARM on", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getHeadCallId().getContent() << ((C_HeadVia*) _tmpMessage->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch")+ "#" + _tmpMessage->getOrderOfOperation()+ "#");
				// Rule : timer on must be locked
				if (!_tmpMessage->getLock()){
					DEBOUT("Message to alarm found unlocked", _tmpMessage)
					DEBASSERT("Message to alarm found unlocked")
				}

				call_oset->getENGINE()->getSUDP()->getAlmgr()->insertAlarm(_tmpMessage, _tmpMessage->getFireTime());

			} else if (_tmpMessage->getTypeOfOperation() == TYPE_OP_TIMER_OFF){

				DEBOUT("SL_CO::call action is clear ALARM off", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getHeadCallId().getContent() << ((C_HeadVia*) _tmpMessage->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch")+ "#" + _tmpMessage->getOrderOfOperation()+ "#")
				string callid_alarm = _tmpMessage->getHeadCallId().getContent() +  ((C_HeadVia*) _tmpMessage->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch") + "#" + _tmpMessage->getOrderOfOperation()+ "#";
				DEBOUT("SL_CO::cancel alarm, callid", callid_alarm)
				call_oset->getENGINE()->getSUDP()->getAlmgr()->cancelAlarm(callid_alarm);
				if(!_tmpMessage->getLock()){
					PURGEMESSAGE(_tmpMessage)
				}
				else{
					DEBASSERT("Rule break: a timer off message found locked")
				}
			}
		}
		else {
			DEBMESSAGE("SL_CO::call action is unexpected",_tmpMessage)
			DEBOUT("SL_CO::call action is unexpected - type of internal",_tmpMessage->getTypeOfInternal())
			DEBOUT("SL_CO::call action is unexpected - destination",_tmpMessage->getDestEntity())
			DEBASSERT("SL_CO::call action is unexpected")
		}
		DEBOUT("pop action","")
		actionList.pop();
	}
}
void SL_CO::actionCall_CL(ACTION* action){

	// now read actions

	stack<SingleAction> actionList;
	actionList = action->getActionList();
	DEBY
	while (!actionList.empty()){

		MESSAGE* _tmpMessage = actionList.top().getMessage();

		DEBMESSAGE("SL_CO::reading action stack client, message:", _tmpMessage)

		if (_tmpMessage->getDestEntity() == SODE_KILL){
			if (_tmpMessage->getLock()){
				DEBASSERT("Locked message directed to SODE_KILL")
			}
			else{
				PURGEMESSAGE(_tmpMessage)
			}
		}
		else if (_tmpMessage->getDestEntity() == SODE_KILLDOA){
			if (_tmpMessage->getLock()){
				DEBASSERT("Locked message directed to SODE_KILL")
			}
			else{
				DEBOUT("SL_CO::actionCall_CL setDoaRequested",call_oset)
				((SL_CC*)(call_oset->getENGINE()))->getCOMAP()->setDoaRequested(call_oset, _tmpMessage->getModulus());
				PURGEMESSAGE(_tmpMessage)
			}
		}

		else if (_tmpMessage->getTypeOfInternal() == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_ALOPOINT){
			// send message to ALO
			// 200OK B side
			DEBOUT("SL_CO::call action is send to ALO", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getHeadCallId().getContent())
			call_oset->getALO()->call(_tmpMessage);
			if(!_tmpMessage->getLock()){
				DEBOUT("Message coming back form ALO not locked, deleted", _tmpMessage)
				PURGEMESSAGE(_tmpMessage)
			}
//			actionList.pop();
//			continue;
		}
		else if (_tmpMessage->getTypeOfInternal() == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_NTWPOINT){

			DEBOUT("SL_CO::call action is send to B", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getHeadCallId().getContent())
			//To network
			DEBDEV("Send to trnasport", _tmpMessage)
			call_oset->getTRNSPRT()->parse(_tmpMessage);

//			actionList.pop();
//			continue;
		}
		else if (_tmpMessage->getTypeOfInternal() == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_SMSVPOINT) {
			DEBOUT("CLIENT SM send to Server SM", _tmpMessage->getLine(0))
			DEBOUT("CLIENT SM send to Server SM 2",  _tmpMessage->getHeadCallId().getContent())
			bool ret = ((SL_CC*)call_oset->getENGINE())->p_w(_tmpMessage);
			DEBOUT("bool ret = sl_cc->p_w(_tmpMess);", ret)
			if(!ret){
				DEBOUT("SL_CO::actionCall_CL p_w message rejected, put in rejection queue",_tmpMessage)
				bool ret2 = ((SL_CC*)call_oset->getENGINE())->p_w_s(_tmpMessage);
				if (!ret2){
					if (!_tmpMessage->getLock()){
						PURGEMESSAGE(_tmpMessage)
					}
				}

			}

//
//			actionList.pop();
//			continue;
		}
		else if (_tmpMessage->getTypeOfInternal() == TYPE_OP ){

			DEBOUT("SL_CO:: TYPE_OP","")

			if ( _tmpMessage->getTypeOfOperation() == TYPE_OP_TIMER_ON){
				DEBOUT("SL_CO::call action is send to ALARM on", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getHeadCallId().getContent() << ((C_HeadVia*) _tmpMessage->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch")+ "#" + _tmpMessage->getOrderOfOperation()+ "#");

				if (!_tmpMessage->getLock()){
					DEBOUT("Message to alarm found unlocked", _tmpMessage)
					DEBASSERT("Message to alarm found unlocked")
				}
				call_oset->getENGINE()->getSUDP()->getAlmgr()->insertAlarm(_tmpMessage, _tmpMessage->getFireTime());
				DEBY

			} else if (_tmpMessage->getTypeOfOperation() == TYPE_OP_TIMER_OFF){

				DEBOUT("SL_CO::call action is clear ALARM off", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getHeadCallId().getContent() << ((C_HeadVia*) _tmpMessage->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch")+ "#" + _tmpMessage->getOrderOfOperation()+ "#")
				string callid_alarm = _tmpMessage->getHeadCallId().getContent() +  ((C_HeadVia*) _tmpMessage->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch") + "#" + _tmpMessage->getOrderOfOperation()+ "#";
				DEBOUT("SL_CO::cancel alarm, callid", callid_alarm)
				call_oset->getENGINE()->getSUDP()->getAlmgr()->cancelAlarm(callid_alarm);

				if(!_tmpMessage->getLock()){
					PURGEMESSAGE(_tmpMessage)
				}else {
					DEBOUT("Rule break timer off message found locked",_tmpMessage)
					DEBASSERT("Rule break timer off message found locked")
				}

			}
			else if (_tmpMessage->getTypeOfOperation() == TYPE_OP_SMCOMMAND){
				DEBMESSAGESHORT("SL_CO::call action is internal send to some SM", _tmpMessage )
				bool ret = ((SL_CC*)call_oset->getENGINE())->p_w(_tmpMessage);
				DEBOUT("bool ret = sl_cc->p_w(_tmpMess);", ret)
				if(!ret){
					DEBOUT("SL_CO::actionCall_CL p_w message rejected, put in rejection queue",_tmpMessage)
					bool ret2 = ((SL_CC*)call_oset->getENGINE())->p_w_s(_tmpMessage);
					if (!ret2){
						if (!_tmpMessage->getLock()){
							PURGEMESSAGE(_tmpMessage)
						}
					}

				}


			}
			else {
				DEBASSERT("SL_CO client operation type inconsistency")
			}
//			actionList.pop();
//			continue;
		}
		else if (_tmpMessage->getDestEntity() == SODE_NOPOINT){
			DEBOUT("SL_CO::TEST, message is abandoned",_tmpMessage->getLine(0))
		}
		else {
			//TODO
			DEBOUT("SL_CO::call action is unexpected", "")
			DEBASSERT("")
		}
		actionList.pop();
	}
}


