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
// CALL_OSET call objects set
// object needed to run the call: state machines, application object (valo)
#include <vector>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <map>
#include <string.h>
#include <errno.h>

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
CALL_OSET::CALL_OSET(ENGINE* _engine, TRNSPRT* _transport, string _call, int _modulus){
	DEBINFCALLOSET("CALL_OSET::CALL_OSET(ENGINE* _engine, TRNSPRT* _transport, string _call, int _modulus)", this<<"]["<<_engine<<"]["<<_transport<<"]["<<_call<<"]["<<_modulus)

#ifdef SV_CL_MUTEX
	pthread_mutex_init(&mutex_sv, NULL);
    pthread_mutex_init(&mutex_cl, NULL);
	GETLOCK(&(mutex_cl),"CALL_OSET::mutex_cl");
	GETLOCK(&(mutex_sv),"CALL_OSET::mutex_sv");
#else
	pthread_mutex_init(&mutex, NULL);
    GETLOCK(&(mutex),"CALL_OSET::mutex");
#endif

	modulus = _modulus;

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

	DEBINFCALLOSET("CALL_OSET sequenceMap trnsctSmMap", &sequenceMap<<"]["<<&trnsctSmMap)

#ifdef SV_CL_MUTEX
	RELLOCK(&(mutex_cl),"SL_CO::mutex_cl");
	RELLOCK(&(mutex_sv),"SL_CO::mutex_sv");
#else
	RELLOCK(&(mutex),"SL_CO::mutex");
#endif
}
CALL_OSET::~CALL_OSET(void){
	DEBINFCALLOSET("CALL_OSET::~CALL_OSET(void)",this)

#ifdef SV_CL_MUTEX
	GETLOCK(&(mutex_cl),"CALL_OSET::mutex_cl");
	GETLOCK(&(mutex_sv),"CALL_OSET::mutex_sv");
#else
    GETLOCK(&(mutex),"CALL_OSET::mutex");
#endif

	DEBINFCALLOSET("CALL_OSET ACCESS CALL_OSET::~CALL_OSET begin", this)
	if (sl_co != 0x0){
		DEBY
		//purge states machines
		//must delete specific SM!
		map<string, TRNSCT_SM*> ::iterator p;
		for (p = trnsctSmMap.begin() ; p != trnsctSmMap.end() ; p++) {
			DEBINFCALLOSET("(p->first)", (p->first))
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
	//Messages could be anywhere
	MESSAGE* m = getNextLockedMessage();
	while (m != MainMessage){

		DEBINFCALLOSET("MESSAGE to be deleted", m)
		if (!m->getLock() || m->getTypeOfInternal()==  TYPE_OP){
			DEBINFCALLOSET("CALL_OSET::~CALL_OSET invalid message", m)
			DEBASSERT("CALL_OSET::~CALL_OSET message invalid["<< m <<"]")
		}

		//TODO DEBCODE
		//checks if this message is still in global table

//#ifdef DEBCODE
//		map<const MESSAGE*, MESSAGE*>::iterator p;
//		int ixx = getModulus(m);
//		pthread_mutex_lock(&messTableMtx[ixx]);
//		p = globalMessTable[ixx].find(m);
//		if (p ==globalMessTable[ixx].end()){
//			DEBWARNING("Message already deleted",m)
//		}
//		pthread_mutex_unlock(&messTableMtx[ixx]);
//#endif
//
//		//I messaggi cancellati qui potrebbero trovarsi ovunque....
//		DEBMESSAGESHORT("DOA locked message", m)
//		if (m->getTypeOfInternal() == TYPE_OP){
//			string callid_alarm = m->getHeadCallId().getContent() +  ((C_HeadVia*) m->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch") + "#" + m->getOrderOfOperation()+ "#";
//			DEBINFCALLOSET("CALL_OSET::~CALL_OSET::cancel alarm, callid", callid_alarm)
//			getENGINE()->getSUDP()->getAlmgr()->cancelAlarm(callid_alarm);
//			//Do not delete alarmed messages
//		}
//		//TODO what if the message is being triggered now?
		m->unSetLock(this);
		PURGEMESSAGE(m);
		DEBY
		DEBINFCALLOSET("Message to be deleted", m)
		m = getNextLockedMessage();
	}
#ifdef SV_CL_MUTEX
	RELLOCK(&(mutex_cl),"SL_CO::mutex_cl");
	RELLOCK(&(mutex_sv),"SL_CO::mutex_sv");
	int rr = pthread_mutex_destroy(&mutex_cl);
	if (rr == EBUSY){
		DEBASSERT("CALL_OSET::~CALL_OSET mutex_cl locked["<< sl_co <<"]")
	}
	rr = pthread_mutex_destroy(&mutex_sv);
	if (rr == EBUSY){
		DEBASSERT("CALL_OSET::~CALL_OSET mutex_sv locked["<< sl_co <<"]")
	}
#else
	RELLOCK(&(mutex),"SL_CO::mutex");
	int rr = pthread_mutex_destroy(&mutex);
	if (rr == EBUSY){
		DEBASSERT("CALL_OSET::~CALL_OSET mutex locked["<< sl_co <<"]")

	}
#endif

	DELPTR(sl_co, "SL_CO")
	DEBY

	//remove from mosulusMap
#ifdef USEMAPMODUL
	map<const string, int>::iterator itm;
    GETLOCK(&modulusMapMtx,"modulusMapMtx");
	itm = modulusMap.find(callId_X);
	if(itm != modulusMap.end()){
		modulusMap.erase(itm);
	}
	RELLOCK(&modulusMapMtx,"modulusMapMtx")
#endif


}
int CALL_OSET::getNextSequence(string _method){
	DEBINFCALLOSET("int CALL_OSET::getNextSequence(string _method)", this<<"]["<<_method)

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
	DEBINFCALLOSET("void CALL_OSET::insertSequence(string _method, int _i)", this<<"]["<<_method<<"]["<<_i)

	DEBINFCALLOSET("int CALL_OSET::getNextSequence(string _method)", _method)
	map<string, int> ::iterator p;
	p = sequenceMap.find(_method);
	if (p != sequenceMap.end()){
		DEBINFCALLOSET("CALL_OSET::insertSequence exists", _method <<"]["<<_i)
		sequenceMap.erase(p);
	}

	sequenceMap.insert(pair<string, int>(_method,_i));
}
int CALL_OSET::getCurrentSequence(string _method){
	DEBINFCALLOSET("int CALL_OSET::getCurrentSequence(string _method)", this<<"]["<<_method)

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
	DEBINFCALLOSET("void CALL_OSET::setCallId_Y(string _cally)", this<<"]["<<_cally)
	callId_Y = _cally;
}
//**********************************************************************************
//v4
string CALL_OSET::getCallId_Y(void){
	return callId_Y;
}
string CALL_OSET::getCallId_X(void){
	return callId_X;
}
//**********************************************************************************
//v4
void CALL_OSET::setCallId_X(string _callId_X){
	callId_X = _callId_X;
}
//**********************************************************************************
//v4

void CALL_OSET::insertLockedMessage(MESSAGE* _message){
	DEBINFCALLOSET("void CALL_OSET::insertLockedMessage(MESSAGE* _message)",this<<"]["<<_message)
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
	DEBINFCALLOSET("MESSAGE* CALL_OSET::getNextLockedMessage(void)",this)
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
	DEBINFCALLOSET("void CALL_OSET::removeLockedMessage(MESSAGE* _message)",this<<"]["<<_message)

	map<MESSAGE*,int>::iterator p;
	p=lockedMessages.find(_message);
	if (p!=lockedMessages.end()){
		DEBINFCALLOSET("CALL_OSET::removeLockedMessage found", _message)
		//MESSAGE* t = (MESSAGE*)p->first;
		lockedMessages.erase(p);
	}
	else {
		DEBINFCALLOSET("CALL_OSET::removeLockedMessage not found", _message)
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
		DEBINFCALLOSET("TRNSCT MAP", p->first << "]["<<(TRNSCT_SM*)p->second << "][" << ((TRNSCT_SM*)p->second)->getId())
	}
}
//**********************************************************************************
TRNSCT_SM* CALL_OSET::getTrnsctSm(string _method, int _sode, string _branch){
	DEBINFCALLOSET("TRNSCT_SM* CALL_OSET::getTrnsctSm(string _method, int _sode, string _branch)",this<<"]["<<_method <<"]["<<_sode <<"]["<<_branch)
	char t_key[512];
	sprintf(t_key, "%s#%d#%s", _method.c_str(), _sode,_branch.c_str());

	string stmp = t_key;

	DEBINFCALLOSET("CALL_OSET::getTrnsctSm", stmp)

	map<string, TRNSCT_SM*> ::iterator p;
	p = trnsctSmMap.find(stmp);
	if (p != trnsctSmMap.end()){
		DEBINFCALLOSET("CALL_OSET::getTrnsctSm found",_method <<"#"<< _sode <<"#"<<_branch << "["<<(TRNSCT_SM*)p->second<<"]")
		return ((TRNSCT_SM*)p->second);
	}else {
		DEBINFCALLOSET("CALL_OSET::getTrnsctSm not found",_method <<"#"<< _sode <<"#"<<_branch)
		return 0x0;
	}
}
//**********************************************************************************
void CALL_OSET::addTrnsctSm(string _method, int _sode, string _branch, TRNSCT_SM* _trnsctSm){
	DEBINFCALLOSET("void CALL_OSET::addTrnsctSm(string _method, int _sode, string _branch, TRNSCT_SM* _trnsctSm)",this<<"]["<<_method<<"]["<<_sode<<"]["<<_branch<<"]["<<_trnsctSm)

	DEBINFCALLOSET("CALL_OSET::addTrnsctSm",_method <<"#"<< _sode <<"#"<<_branch << "] ["<<_trnsctSm<<"] call_oset ["<<this)
	char t_key[512];
	sprintf(t_key, "%s#%d#%s", _method.c_str(), _sode, _branch.c_str());

	string stmp = t_key;

	_trnsctSm->setId(stmp);

	pair<map<string, TRNSCT_SM*>::iterator, bool> ret;
	ret = trnsctSmMap.insert(pair<string, TRNSCT_SM*>(stmp, _trnsctSm));
	if(!ret.second){
		DEBINFCALLOSET("CALL_OSET::addTrnsctSm adding a exsiting sm", stmp <<"]["<<_trnsctSm)
		DEBASSERT("CALL_OSET::addTrnsctSm")
	}
	DEBINFCALLOSET("CALL_OSET::addTrnsctSm", stmp<<"]["<<_trnsctSm)

	// special for client sm Ack
	if (_sode == SODE_TRNSCT_CL && _method.substr(0,3).compare("ACK") == 0 ){
		DEBINFCALLOSET("CALL_OSET::addTrnsctSm special Ack sm cl pointer",_method <<"#"<< _sode <<"#"<<_branch <<"#"<<_trnsctSm)
		lastTRNSCT_SM_ACK_CL = _trnsctSm;
	}	return;
}

void CALL_OSET::call(MESSAGE* _message){
	DEBINFCALLOSET("void CALL_OSET::call(MESSAGE* _message)",this<<"]["<<_message)
    PROFILE("CALL_OSET::call() begin")
    TIMEDEF
    SETNOW

	int modulus = 0;

#ifdef SV_CL_MUTEX
	int dest = _message->getDestEntity();
#endif

	int op = sl_co->call(_message, modulus);

#ifdef SV_CL_MUTEX
	if (dest == SODE_TRNSCT_SV){
		RELLOCK(&(mutex_sv),"CALL_OSET::mutex_sv "<<mutex_sv);
	}
	else if(dest == SODE_TRNSCT_CL){
		RELLOCK(&(mutex_cl),"CALL_OSET::mutex_cl "<<mutex_cl);
	}
#else
	RELLOCK(&(mutex),"CALL_OSET::mutex "<<mutex);
#endif



	//The call oset may have been deleted here
	if (op == 1){
		((SL_CC*)getENGINE())->getCOMAP()->setDoaRequested(this, modulus);
	}

    PRINTDIFF("CALL_OSET::call() end")

	//setdoareq must be asynchronous
	//then release the mutex
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
int SL_CO::call(MESSAGE* _message, int& _r_modulus){
	DEBINFCALLOSET("int SL_CO::call(MESSAGE* _message, int& _r_modulus)",this<<"]["<<_message<<"]["<<_r_modulus)

    TIMEDEF
    SETNOW
    PROFILE("SL_CO::call begin")
    DEBMESSAGE("SL_CO::call incoming", _message)

    ACTION* action = 0x0;

    int oper = 0;

    if(_message->getTypeOfInternal() == TYPE_OP && _message->getTypeOfOperation() !=  TYPE_OP_SMCOMMAND){
		if (_message->getLock()){
			DEBINFCALLOSET("Break rule: a message from alarm must not be locked", _message)
			DEBASSERT("Break rule: a message from alarm must not be locked")
		}
    }

    //Message is going to Server SM
    int dest = _message->getDestEntity();

    if (dest == SODE_TRNSCT_SV) {

        TRNSCT_SM* trnsctSM = 0x0;
        //First look for an existing SM using METHOD+SM_SV+branch
        //DEBINFCALLOSET("((C_HeadVia*)_message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue(\"branch\")",((C_HeadVia*)_message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch"))
        trnsctSM = call_oset->getTrnsctSm(_message->getHeadCSeqMethod(), SODE_TRNSCT_SV, _message->getViaBranch());
        //There are no sm, create it
        //OVERALLSTATE lock usage start here

        //Quicktry :
        //create the INVITE_SV during creation of call_oset
        //and here set only the callID
        if (trnsctSM == 0x0 ){
            if (OverallState_SV == OS_INIT && _message->getReqRepType() == REQSUPP && _message->getHeadSipRequestCode() == INVITE_REQUEST ){
                call_oset->insertSequence("INVITE_A", _message->getHeadCSeq());
                NEWPTR2(trnsctSM, TRNSCT_SM_INVITE_SV(_message->getHeadSipRequestCode(), _message, call_oset->getENGINE(), this),"TRNSCT_SM_INVITE_SV")
            }
            else if (OverallState_SV == OS_COMPLETED && _message->getReqRepType() == REQSUPP && _message->getHeadSipRequestCode() == ACK_REQUEST){
                NEWPTR2(trnsctSM, TRNSCT_SM_ACK_SV(_message->getHeadSipRequestCode(), _message, call_oset->getENGINE(), this),"TRNSCT_SM_ACK_SV")
            }
            else if ( OverallState_SV == OS_CONFIRMED && _message->getRequestDirection() == SODE_FWD && _message->getReqRepType() == REQSUPP && _message->getHeadSipRequestCode() == BYE_REQUEST ){
                NEWPTR2(trnsctSM, TRNSCT_SM_BYE_SV(_message->getHeadSipRequestCode(), _message, call_oset->getENGINE(), this),"TRNSCT_SM_BYE_SV")
            }
            //TODO To test
            else if ( OverallState_CL == OS_COMPLETED && _message->getRequestDirection() == SODE_BKWD && _message->getReqRepType() == REQSUPP && _message->getHeadSipRequestCode() == BYE_REQUEST ){
                NEWPTR2(trnsctSM, TRNSCT_SM_BYE_SV(_message->getHeadSipRequestCode(), _message, call_oset->getENGINE(), this),"TRNSCT_SM_BYE_SV")
            }
            else if (_message->getReqRepType() != REPSUPP ){
                // but the call object has been recognized!!!
                // SL_CO not in correct state
                DEBINFCALLOSET("Unexpected message ignored", _message)
                if(_message->getLock()){
                    DEBASSERT("Unexpected message to be ignored found locked")
                }
                oper = 1;
                _r_modulus = _message->getModulus();
                PRINTDIFF("SL_CO::call() end")

                //DELETE _message here!
    			if(!_message->getLock()){
    				PURGEMESSAGE(_message)
    			}
                return oper;
            }
            else {
                DEBINFCALLOSET("A unrecognized reply directed to server has reached the call object","")
                DEBASSERT("Unrecognized Reply message sent to SV machine")
            }
            //DEBINFCALLOSET("call_oset->addTrnsctSm", _message->getHeadCSeq().getMethod().getContent() << " " << ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch"))
            //call_oset->addTrnsctSm(_message->getHeadCSeq().getMethod().getContent(), SODE_TRNSCT_SV, ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch"), trnsctSM);
            call_oset->addTrnsctSm(_message->getHeadCSeqMethod(), SODE_TRNSCT_SV, _message->getViaBranch(), trnsctSM);
            DEBINFCALLOSET("call_oset->addTrnsctSm","done")

        }

        //OVERALLSTATE lock ends here?

    	if (trnsctSM != 0x0){

            //send the message to sm
            action = trnsctSM->event(_message);

            if (action != 0x0){
                PRINTDIFF("Before actionCall_SV")
                SETNOW
                oper = actionCall_SV(action, _r_modulus);
                PRINTDIFF("After actionCall_SV")

            }
            else {

            }
        }
    	else {
            DEBMESSAGE("SL_CO::call the message has no SM clients *****", _message)
            DEBASSERT("Check this case out")
    	}
    }
    //Message is going to Client SM
    else if (dest == SODE_TRNSCT_CL){

        string callidys = _message->getHeadCallId();
        DEBINFCALLOSET("SL_CO::call client state machine", callidys)

        TRNSCT_SM* trnsct_cl = 0x0;

        //Get into the ack cl state machine
        if (_message->getTypeOfInternal() == TYPE_OP && _message->getTypeOfOperation() == TYPE_OP_SMCOMMAND){
            DEBINFCALLOSET("lastTRNSCT_SM_ACK_CL", call_oset->lastTRNSCT_SM_ACK_CL)
            trnsct_cl = call_oset->lastTRNSCT_SM_ACK_CL;
            if ( trnsct_cl == 0x0){
            	//Still not created
            	//do nothing
            	//the message is deleted in comap

            	//DELETE _message here!
    			if(!_message->getLock()){
    				PURGEMESSAGE(_message)
    			}
            	return oper;
            }
        }
        else if (_message->getReqRepType() == REPSUPP ){
            //Only Replies are recognized here
            string smid1 = _message->getHeadCSeqMethod();
            string smid2 = _message->getViaBranch();
            DEBINFCALLOSET("call_oset->getTrnsctSm",smid1 <<"#"<< SODE_TRNSCT_CL <<"#"<<smid2 )
            trnsct_cl = call_oset->getTrnsctSm(smid1,SODE_TRNSCT_CL,smid2);
        }else {
            //DEBINFCALLOSET("((C_HeadVia*)_message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue(\"branch\")",((C_HeadVia*)_message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch"))
        	trnsct_cl = call_oset->getTrnsctSm(_message->getHeadCSeqMethod(), SODE_TRNSCT_CL, _message->getViaBranch());
        }

        //OVERALLSTATE lock begin ?

        if (trnsct_cl == 0x0){

            DEBINFCALLOSET("Creating Trnsct Client machine callidy", callidys)
            // All those request are generated by ALO

            DEBINFCALLOSET("_message->getReqRepType() == REQSUPP",_message->getReqRepType())
            DEBINFCALLOSET("_message->getHeadSipRequestCode() == INVITE_REQUEST",_message->getHeadSipRequestCode())

            if(_message->getReqRepType() == REQSUPP){
                if (_message->getHeadSipRequestCode() == INVITE_REQUEST){
                    NEWPTR2(trnsct_cl, TRNSCT_SM_INVITE_CL(_message->getHeadSipRequestCode(), _message, _message->getSourceMessage(), call_oset->getENGINE(), this),"TRNSCT_SM_INVITE_CL")
                	SL_CC* tmp_sl_cc = (SL_CC*)call_oset->getENGINE();
                }
                else if (_message->getHeadSipRequestCode() == ACK_REQUEST){
                    NEWPTR2(trnsct_cl, TRNSCT_SM_ACK_CL(_message->getHeadSipRequestCode(), _message, _message->getSourceMessage(), call_oset->getENGINE(), this),"TRNSCT_SM_ACK_CL")
                    if (call_oset->lastTRNSCT_SM_ACK_CL != 0x0){
                            DEBASSERT("An ACK CL SM already exists")
                    }
                    call_oset->lastTRNSCT_SM_ACK_CL = trnsct_cl;
                }
                else if (_message->getHeadSipRequestCode() == BYE_REQUEST){
                    NEWPTR2(trnsct_cl, TRNSCT_SM_BYE_CL(_message->getHeadSipRequestCode(), _message, _message->getSourceMessage(), call_oset->getENGINE(), this),"TRNSCT_SM_BYE_CL")
                }
                DEBINFCALLOSET("_message->getHeadCSeqMethod()",_message->getHeadCSeqMethod())
                DEBINFCALLOSET("_message->getViaBranch()",_message->getViaBranch())
                DEBINFCALLOSET("trnsct_cl",trnsct_cl)
                call_oset->addTrnsctSm(_message->getHeadCSeqMethod(), SODE_TRNSCT_CL, _message->getViaBranch(), trnsct_cl);
            	DEBY
            }else{
                // but the call object has been recognized!!!
                // the sm may have been deleted
                DEBWARNING("An unexpected reply directed to client has reached the call object", _message)
                DEBMESSAGE("An unexpected reply directed to client has reached the call object", _message)
                DEBINFCALLOSET("An unexpected reply directed to client has reached the call object", _message <<"]["<<_message->getTypeOfInternal())
                call_oset->dumpTrnsctSm();
                action = 0x0;
                DEBASSERT("An unexpected reply directed to client has reached the call object")
            }
        }
        DEBY

        //OVERALLSTATE end here?

        action = trnsct_cl->event(_message);

        if (action != 0x0){
            PRINTDIFF("Before actionCall_CL")
            SETNOW
            oper = actionCall_CL(action, _r_modulus);
            PRINTDIFF("After actionCall_CL")
        }
        else {
            //_message must be deleted using an action and sode_kill

            // we may receive the message from above: An unexpected reply directed to client has reached the call object

            //TODO we mayn receive an alarm that is expired
            //so the sm has ignored it
            DEBINFCALLOSET("SL_CO::event", "action is null nothing, event ignored")
        }
    }

    DEBINFCALLOSET("SL_CO::call ended","")
    if (action != 0x0){
        DEBINFCALLOSET("SL_CO::call delete action","")
        DELPTR(action,"ACTION");
    }
    //RELLOCK(&mutex,"mutex");
    PRINTDIFF("SL_CO::call() ")

    //DELETE _message here!
	if(!_message->getLock()){
		PURGEMESSAGE(_message)
	}
    return oper;
}

int SL_CO::actionCall_SV(ACTION* action, int& _r_modulus){
	DEBINFCALLOSET("int SL_CO::actionCall_SV(ACTION* action, int& _r_modulus)",this<<"]["<<action<<"]["<<_r_modulus)
    TIMEDEF
    SETNOW
    PROFILE("SL_CO::actionCall_SV begin")

    // now get actions one by one
    stack<SingleAction> actionList = action->getActionList();

    int oper = 0;

    while (!actionList.empty()){

        MESSAGE* _tmpMessage = actionList.top().getMessage();
        DEBMESSAGE("SL_CO::reading action stack server, message:", _tmpMessage)

        if (_tmpMessage->getTypeOfInternal() == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_ALOPOINT){
            //To ALO
            DEBINFCALLOSET("SL_CO::call action is send to ALO", _tmpMessage->getFirstLine() << " ** " << _tmpMessage->getDialogExtendedCID())
            call_oset->getALO()->call(_tmpMessage);
            if(!_tmpMessage->getLock()){
            	DEBINFCALLOSET("Message coming back form ALO not locked, deleted", _tmpMessage)
				PURGEMESSAGE(_tmpMessage)
            }

            //MLF2
            else {
            	DEBINFCALLOSET("Message coming back form ALO locked", _tmpMessage)
                DEBASSERT("Message coming back form ALO locked")
            }

        }
        else if (_tmpMessage->getTypeOfInternal() == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_NTWPOINT){
            //To network
            DEBINFCALLOSET("Send to transport", _tmpMessage)
            //WHY PARSE
            call_oset->getTRNSPRT()->p_w(_tmpMessage);
        }
        else if (_tmpMessage->getTypeOfInternal() == TYPE_OP){
             if ( _tmpMessage->getTypeOfOperation() == TYPE_OP_TIMER_ON){

                //DEBINFCALLOSET("SL_CO::call action is send to ALARM on", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getHeadCallId().getContent() << ((C_HeadVia*) _tmpMessage->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch")+ "#" + _tmpMessage->getOrderOfOperation()+ "#");
                // Rule : timer on must be unlocked
                if (_tmpMessage->getLock()){
                	DEBINFCALLOSET("Message to alarm found locked", _tmpMessage)
                    DEBASSERT("Message to alarm found locked")
                }

                 call_oset->getENGINE()->getSUDP()->getAlmgr()->insertAlarm(_tmpMessage, _tmpMessage->getFireTime());

            }
            else if (_tmpMessage->getTypeOfOperation() == TYPE_OP_TIMER_OFF){

                //DEBINFCALLOSET("SL_CO::call action is clear ALARM off", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getHeadCallId().getContent() << ((C_HeadVia*) _tmpMessage->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch")+ "#" + _tmpMessage->getOrderOfOperation()+ "#")
#ifdef ALARMENGINE
				call_oset->getENGINE()->getSUDP()->getAlmgr()->cancelAlarm(_tmpMessage);
#else
				string callid_alarm = _tmpMessage->getHeadCallId().getContent() +  ((C_HeadVia*) _tmpMessage->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch") + "#" + _tmpMessage->getOrderOfOperation()+ "#";
				DEBINFCALLOSET("SL_CO::cancel alarm, callid", callid_alarm)

				call_oset->getENGINE()->getSUDP()->getAlmgr()->cancelAlarm(callid_alarm,_tmpMessage->getModulus());
				if(!_tmpMessage->getLock()){
						PURGEMESSAGE(_tmpMessage)
				}
				else{
						DEBASSERT("Rule break: a timer off message found locked")
				}
#endif

            }
        }
        else if (_tmpMessage->getDestEntity() == SODE_KILLDOA){
            if (_tmpMessage->getLock()){
                DEBASSERT("Locked message directed to SODE_KILLDOA")
            }
            else{
                DEBINFCALLOSET("SL_CO::actionCall_SV setDoaRequested",call_oset)
                //not here
                //((SL_CC*)(call_oset->getENGINE()))->getCOMAP()->setDoaRequested(call_oset, _tmpMessage->getModulus());
				_r_modulus = _tmpMessage->getModulus();
                PURGEMESSAGE(_tmpMessage)
                oper = 1;
            }
        }
        else if (_tmpMessage->getTypeOfInternal() == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_TRNSCT_CL){
            //server sm sending to client sm should not happen
            DEBINFCALLOSET("SL_CO::call action SV to CL directly","")
            DEBASSERT("Server sm sending to client sm should not happen ")
        }
        else {
            DEBMESSAGE("SL_CO::call action is unexpected",_tmpMessage)
            DEBINFCALLOSET("SL_CO::call action is unexpected - type of internal",_tmpMessage->getTypeOfInternal())
            DEBINFCALLOSET("SL_CO::call action is unexpected - destination",_tmpMessage->getDestEntity())
            DEBASSERT("SL_CO::call action is unexpected")
        }
        DEBINFCALLOSET("pop action","")
        actionList.pop();
    }
    PRINTDIFF("SL_CO::actionCall_SV() ")
    return oper;
}
int SL_CO::actionCall_CL(ACTION* action, int& _r_modulus){
	DEBINFCALLOSET("int SL_CO::actionCall_CL(ACTION* action, int& _r_modulus)",this<<"]["<<action<<"]["<<_r_modulus)

	// now read actions
    TIMEDEF
    SETNOW
    PROFILE("SL_CO::actionCall_CL begin")

	stack<SingleAction> actionList;
	actionList = action->getActionList();

	int oper = 0;

	while (!actionList.empty()){

		MESSAGE* _tmpMessage = actionList.top().getMessage();

		DEBMESSAGE("SL_CO::reading action stack client, message:", _tmpMessage)

		if (_tmpMessage->getTypeOfInternal() == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_ALOPOINT){
			// send message to ALO
			// 200OK B side
			//DEBINFCALLOSET("SL_CO::call action is send to ALO", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getHeadCallId().getContent())
			call_oset->getALO()->call(_tmpMessage);
			if(!_tmpMessage->getLock()){
				DEBINFCALLOSET("Message coming back form ALO not locked, deleted", _tmpMessage)
				PURGEMESSAGE(_tmpMessage)
			}
		}
		else if (_tmpMessage->getTypeOfInternal() == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_NTWPOINT){

			//DEBINFCALLOSET("SL_CO::call action is send to B", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getHeadCallId().getContent())
			//To network
			call_oset->getTRNSPRT()->p_w(_tmpMessage);

		}
		else if (_tmpMessage->getTypeOfInternal() == TYPE_MESS && _tmpMessage->getDestEntity() == SODE_SMSVPOINT) {
			//DEBINFCALLOSET("CLIENT SM send to Server SM", _tmpMessage->getLine(0) << "]["<<_tmpMessage->getHeadCallId().getContent())
#ifdef NONESTEDPW
			call_oset->getSL_CO()->call(_tmpMessage,_r_modulus);
//			//do not delete here delete in "call"
//			if(!_tmpMessage->getLock()){
//				PURGEMESSAGE(_tmpMessage)
//			}
#else
			((SL_CC*)call_oset->getENGINE())->p_w(_tmpMessage);
#endif
		}
		else if (_tmpMessage->getTypeOfInternal() == TYPE_OP ){

			DEBINFCALLOSET("SL_CO:: TYPE_OP","")

			if ( _tmpMessage->getTypeOfOperation() == TYPE_OP_TIMER_ON){
				//DEBINFCALLOSET("SL_CO::call action is send to ALARM on", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getHeadCallId().getContent() << ((C_HeadVia*) _tmpMessage->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch")+ "#" + _tmpMessage->getOrderOfOperation()+ "#");

				//MLF2
				if (_tmpMessage->getLock()){
					DEBINFCALLOSET("Message to alarm found locked", _tmpMessage)
					DEBASSERT("Message to alarm found locked")
				}
				call_oset->getENGINE()->getSUDP()->getAlmgr()->insertAlarm(_tmpMessage, _tmpMessage->getFireTime());

			} else if (_tmpMessage->getTypeOfOperation() == TYPE_OP_TIMER_OFF){

#ifdef ALARMENGINE
				call_oset->getENGINE()->getSUDP()->getAlmgr()->cancelAlarm(_tmpMessage);
#else
				DEBINFCALLOSET("SL_CO::call action is clear ALARM off", _tmpMessage->getLine(0) << " ** " << _tmpMessage->getHeadCallId().getContent() << ((C_HeadVia*) _tmpMessage->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch")+ "#" + _tmpMessage->getOrderOfOperation()+ "#")
				string callid_alarm = _tmpMessage->getHeadCallId().getContent() +  ((C_HeadVia*) _tmpMessage->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch") + "#" + _tmpMessage->getOrderOfOperation()+ "#";
				DEBINFCALLOSET("SL_CO::cancel alarm, callid", callid_alarm)
				//
				//TODO
				// here is synch!!!
				call_oset->getENGINE()->getSUDP()->getAlmgr()->cancelAlarm(callid_alarm,_tmpMessage->getModulus());

				if(!_tmpMessage->getLock()){
					PURGEMESSAGE(_tmpMessage)
				}else {
					DEBINFCALLOSET("Rule break timer off message found locked",_tmpMessage)
					DEBASSERT("Rule break timer off message found locked")
				}
#endif



			}
			else if (_tmpMessage->getTypeOfOperation() == TYPE_OP_SMCOMMAND){
				DEBMESSAGESHORT("SL_CO::call action is internal send to some SM", _tmpMessage )
#ifdef NONESTEDPW
				call_oset->getSL_CO()->call(_tmpMessage,_r_modulus);
#else
				((SL_CC*)call_oset->getENGINE())->p_w(_tmpMessage);
#endif
			}
			else {
				DEBASSERT("SL_CO client operation type inconsistency")
			}
		}
		else if (_tmpMessage->getDestEntity() == SODE_KILLDOA){
			if (_tmpMessage->getLock()){
				DEBASSERT("Locked message directed to SODE_KILL")
			}
			else{
				DEBINFCALLOSET("SL_CO::actionCall_CL setDoaRequested",call_oset)
				_r_modulus =  _tmpMessage->getModulus();
				PURGEMESSAGE(_tmpMessage)
				oper = 1;
			}
		}
		else if (_tmpMessage->getDestEntity() == SODE_NOPOINT){
			//DEBINFCALLOSET("SL_CO::TEST, message is abandoned",_tmpMessage->getLine(0))
		}
		else {
			//TODO
			DEBINFCALLOSET("SL_CO::call action is unexpected", "")
			DEBASSERT("")
		}
		actionList.pop();
	}
    PRINTDIFF("SL_CO::actionCall_CL() ")
	return oper;
}


