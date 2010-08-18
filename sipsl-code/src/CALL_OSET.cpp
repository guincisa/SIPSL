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

static SIPUTIL SipUtil;

//**********************************************************************************
//**********************************************************************************
// CALL_OSET
//**********************************************************************************
CALL_OSET::CALL_OSET(ENGINE* _engine, string _call){

	engine = _engine;
	sequenceMap.insert(pair<string, int>("ACK_B",0));
	sequenceMap.insert(pair<string, int>("INVITE_B",0));
	sequenceMap.insert(pair<string, int>("BYE_B",0));

	sl_co = 0x0;
	alo = 0x0;

	sl_co = new SL_CO(this);
	alo = new VALO(_engine, this);
	alo->linkSUDP(_engine->getSUDP());
	callId_X = _call;
	callId_Y = "";

	DEBOUT("CALL_OSET sequenceMap", &sequenceMap)
	DEBOUT("CALL_OSET trnsctSmMap", &trnsctSmMap)

}
CALL_OSET::~CALL_OSET(void){

	DEBOUT("CALL_OSET::~CALL_OSET begin", this)
	if (sl_co != 0x0){
		DEBY
		//purge states machines
		map<string, TRNSCT_SM*> ::iterator p;
		for (p = trnsctSmMap.begin() ; p != trnsctSmMap.end() ; p++) {
			DEBY
			DELPTR((TRNSCT_SM*)p->second,"(TRNSCT_SM*)p->second");
		}
		DEBY
	}
	if (alo != 0x0){
		DEBY
		delete alo;
	}
	MESSAGE* m = getNextLockedMessage();
	while (m != 0x0){
		DEBY
		DEBMESSAGESHORT("DOA locked message", m)
		PURGEMESSAGE(m);
		DEBY
		m = getNextLockedMessage();
		DEBOUT("Message to be deleted", m)
	}
	DEBY

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

void CALL_OSET::insertLockedMessage(MESSAGE* _message){
	DEBMESSAGESHORT("Insert locked message", _message)
	lockedMessages.insert(pair<MESSAGE*,int>(_message,0));
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
	return 0x0;

}
void CALL_OSET::removeLockedMessage(MESSAGE* _message){

	map<MESSAGE*,int>::iterator p;
	p=lockedMessages.find(_message);
	if (p!=lockedMessages.end()){
		DEBY
		MESSAGE* t = (MESSAGE*)p->first;
		lockedMessages.erase(p);
	}

}


//**********************************************************************************
TRNSCT_SM* CALL_OSET::getTrnsctSm(string _method, int _sode, string _branch){

	DEBOUT_UTIL("CALL_OSET::getTrnsctSm",_method <<"#"<< _sode <<"#"<<_branch)
	char t_key[512];
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

	DEBOUT_UTIL("CALL_OSET::addTrnsctSm",_method <<"#"<< _sode <<"#"<<_branch)
	char t_key[512];
	if (_sode == SODE_TRNSCT_CL){
		sprintf(t_key, "%s#SODE_TRNSCT_CL#%s", _method.c_str(), _branch.c_str());
	}
	else if (_sode == SODE_TRNSCT_SV){
		sprintf(t_key, "%s#SODE_TRNSCT_SV#%s", _method.c_str(), _branch.c_str());
	}
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
    pthread_mutex_init(&mutex, NULL);

}
//**********************************************************************************
//**********************************************************************************
void SL_CO::call(MESSAGE* _message){

	//TODO if we use this mutex we can remove mutexes in state machines
	GETLOCK(&mutex,"mutex");
	DEBMESSAGE("SL_CO::call incoming", _message)

    ACTION* action = 0x0;

	//Message is going to Server SM
	if (_message->getDestEntity() == SODE_TRNSCT_SV) {

		DEBOUT("SL_CO::search for transaction state machine", _message->getHeadCallId().getContent())

		TRNSCT_SM* trnsctSM = 0x0;

		//First look for an existing SM using METHOD+SM_SV+branch
    	trnsctSM = call_oset->getTrnsctSm(_message->getHeadCSeq().getMethod().getContent(), SODE_TRNSCT_SV, ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch"));

    	//There are no sm, create it
		if (trnsctSM == 0x0){
			if (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST){
				call_oset->insertSequence("INVITE_A", _message->getHeadCSeq().getSequence());
				NEWPTR2(trnsctSM, TRNSCT_SM_INVITE_SV(_message->getHeadSipRequest().getS_AttMethod().getMethodID(), _message, call_oset->getENGINE(), this),"TRNSCT_SM_INVITE_SV")
			}
			else if (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == ACK_REQUEST){
				NEWPTR2(trnsctSM, TRNSCT_SM_ACK_SV(_message->getHeadSipRequest().getS_AttMethod().getMethodID(), _message, call_oset->getENGINE(), this),"TRNSCT_SM_ACK_SV")
			}
			else if (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == BYE_REQUEST){
				NEWPTR2(trnsctSM, TRNSCT_SM_BYE_SV(_message->getHeadSipRequest().getS_AttMethod().getMethodID(), _message, call_oset->getENGINE(), this),"TRNSCT_SM_BYE_SV")
			}
			//Add the sm to the map
			DEBOUT("call_oset->addTrnsctSm", _message->getHeadCSeq().getMethod().getContent() << " " << ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch"))
			call_oset->addTrnsctSm(_message->getHeadCSeq().getMethod().getContent(), SODE_TRNSCT_SV, ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch"), trnsctSM);
			DEBOUT("call_oset->addTrnsctSm","done")
		}

		//send the message to sm
		action = trnsctSM->event(_message);

		if (action != 0x0){

			// now get actions one by one
			stack<SingleAction> actionList = action->getActionList();

			while (!actionList.empty()){

				MESSAGE* _tmpMessage = actionList.top().getMessage();
				DEBMESSAGE("SL_CO::reading action stack server, message:", _tmpMessage)

				if (_tmpMessage->typeOfInternal == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_ALOPOINT){
					//To ALO
					DEBOUT("SL_CO::call action is send to ALO", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getDialogExtendedCID())
					call_oset->getALO()->call(_tmpMessage);
				}
				else if (_tmpMessage->typeOfInternal == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_TRNSCT_CL){
					//server sm sending to client sm should not happen
					DEBASSERT("Server sm sending to client sm should not happen should not happen")
				}
				else if (_tmpMessage->typeOfInternal == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_NTWPOINT){
					//To network
					if (_tmpMessage->getReqRepType() == REPSUPP) {
						//TODO Check if there is a ROUTE header
						call_oset->getENGINE()->getSUDP()->sendReply(_tmpMessage);
					}
					else {
						DEBASSERT("Unexpected SM_SV sending a Request to network")
					}

				} else {
					//TODO
					DEBASSERT("SL_CO::call action is unexpected")
				}
				DEBOUT("pop action","")
				actionList.pop();
			}
		}
		else {
			DEBOUT("SL_CO::event", "action is null nothing, event ignored")
			//Delete only if not locked
			if (!_message->getLock()){
				PURGEMESSAGE(_message)
			} else {
				//TODO
				//The message is locked for some reason but did not trigger any action...
				DEBASSERT("Check this case out")
			}

		}
	}
	//Message is going to Client SM
	else if (_message->getDestEntity() == SODE_TRNSCT_CL){

		string callidys = _message->getHeadCallId().getContent();
	    DEBOUT("SL_CO::call client state machine", callidys)

		TRNSCT_SM* trnsct_cl = call_oset->getTrnsctSm(_message->getHeadCSeq().getMethod().getContent(), SODE_TRNSCT_CL, ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch"));

		if (trnsct_cl == 0x0){

			DEBOUT("Creating Trnsct Client machine callidy", callidys)
			if(_message->getReqRepType() == REQSUPP){
				if (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST){
					NEWPTR2(trnsct_cl, TRNSCT_SM_INVITE_CL(_message->getHeadSipRequest().getS_AttMethod().getMethodID(), _message, _message->getSourceMessage(), call_oset->getENGINE(), this),"TRNSCT_SM_INVITE_CL")
				}
				else if (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == ACK_REQUEST){
					NEWPTR2(trnsct_cl, TRNSCT_SM_ACK_CL(_message->getHeadSipRequest().getS_AttMethod().getMethodID(), _message, _message->getSourceMessage(), call_oset->getENGINE(), this),"TRNSCT_SM_ACK_CL")
				}
				else if (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == BYE_REQUEST){
					NEWPTR2(trnsct_cl, TRNSCT_SM_BYE_CL(_message->getHeadSipRequest().getS_AttMethod().getMethodID(), _message, _message->getSourceMessage(), call_oset->getENGINE(), this),"TRNSCT_SM_BYE_CL")
				}
			}else{
				DEBOUT("A unexpected reply directed to client has reached the call object","")
				DEBASSERT("Check this")
			}

			call_oset->addTrnsctSm(_message->getHeadCSeq().getMethod().getContent(), SODE_TRNSCT_CL, ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch"), trnsct_cl);

			//TODO?
			//This is needed when a new request is coming from ALO
			//B side call ID created by ALO has to be associated to the main call_id (from Aside)
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

				DEBMESSAGE("SL_CO::reading action stack client, message:", _tmpMessage)

				if (_tmpMessage->typeOfInternal == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_ALOPOINT){
					// send message to ALO
					// 200OK B side
					DEBOUT("SL_CO::call action is send to ALO", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getHeadCallId().getContent())
					call_oset->getALO()->call(_tmpMessage);
					actionList.pop();
					continue;
				}
				else if (_tmpMessage->typeOfInternal == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_NTWPOINT){

					DEBOUT("SL_CO::call action is send to B", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getHeadCallId().getContent())

					if (_tmpMessage->getReqRepType() == REQSUPP) {
						call_oset->getENGINE()->getSUDP()->sendRequest(_tmpMessage);
					}
					else {
						DEBASSERT("Unexpected SM_CL sending a Reply to network")
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
						DEBOUT("SL_CO::call action is send to ALARM on", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getHeadCallId().getContent() << ((C_HeadVia*) _tmpMessage->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch"));
						SysTime st1 = _tmpMessage->getFireTime();
						call_oset->getENGINE()->getSUDP()->getAlmgr()->insertAlarm(_tmpMessage, st1);

					} else if (_tmpMessage->typeOfOperation == TYPE_OP_TIMER_OFF){

						DEBOUT("SL_CO::call action is clear ALARM off", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getHeadCallId().getContent() << ((C_HeadVia*) _tmpMessage->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch"))
						string callid_alarm = _tmpMessage->getHeadCallId().getContent() +  ((C_HeadVia*) _tmpMessage->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch");
						DEBOUT("SL_CO::cancel alarm, callid", callid_alarm)
						call_oset->getENGINE()->getSUDP()->getAlmgr()->cancelAlarm(callid_alarm);
					}
					else {
						DEBASSERT("SL_CO client operation type inconsistency")
					}
					actionList.pop();
					continue;
				}
				else {
					//TODO
					DEBOUT("SL_CO::call action is unexpected", "")
					DEBASSERT("")
				}
				actionList.pop();
			}
		}
		else {
			//TODO we mayn receive an alarm that is expired
			//so the sm has ignored it
			DEBOUT("SL_CO::event", "action is null nothing, event ignored")
			if (!_message->getLock()){
				PURGEMESSAGE(_message)
			}
		}
	}

	DEBOUT("SL_CO::call ended","")
	if (action != 0x0){
		DEBOUT("SL_CO::call delete action","")
		delete action;
	}
	RELLOCK(&mutex,"mutex");
}


