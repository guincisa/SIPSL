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
#include <math.h>

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
#ifndef SIPUTIL_H
#include "SIPUTIL.h"
#endif
#ifndef TRNSCT_SM_H
#include "TRNSCT_SM.h"
#endif


static SIPUTIL SipUtil;

//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
TRNSCT_SM::TRNSCT_SM(int _requestType, MESSAGE* _matrixMess, MESSAGE* _a_Matrix, ENGINE* _sl_cc, SL_CO* _sl_co):
	SM(_sl_cc, _sl_co){

	requestType = _requestType;
	Matrix = _matrixMess;

	//TODO insert into lock table???
	Matrix->setLock();

	//This one is locked elswere
	A_Matrix = _a_Matrix;
	if (_a_Matrix == 0x0){
		DEBASSERT("NO")
	}
}
TRNSCT_SM::~TRNSCT_SM(void){

	DEBOUT("TRNSCT_SM::~TRNSCT_SM ",this)
	if (Matrix == A_Matrix){
		PURGEMESSAGE(Matrix)
	}else{
		PURGEMESSAGE(Matrix)
		PURGEMESSAGE(A_Matrix)
	}

	DEBOUT("TRNSCT_SM::~TRNSCT_SM done",this)

}


MESSAGE* TRNSCT_SM::getMatrixMessage(void){
	if (Matrix == 0x0){
		DEBASSERT("NO")
	}

	return Matrix;
}
MESSAGE* TRNSCT_SM::getA_Matrix(void){
	if (A_Matrix == 0x0){
		DEBASSERT("NO")
	}
	return A_Matrix;
}

//**********************************************************************************

ACTION* SM::event(MESSAGE* _event){

	PREDICATE_ACTION* tmp;

	ACTION* act=0x0;

	DEBOUT("SM::event Look for state", State)

	pair<multimap<const int,PREDICATE_ACTION*>::iterator,multimap<const int,PREDICATE_ACTION*>::iterator> ret;

	multimap<const int,PREDICATE_ACTION*>::iterator iter;

	ret = move_sm.equal_range(State);

	GETLOCK(&mutex,"mutex");

    for (iter=ret.first; iter!=ret.second; ++iter){
		tmp  = iter->second;

		if (tmp->predicate(this, _event)){
			act = tmp->action(this, _event);
			RELLOCK(&mutex,"mutex");
			DEBY
			return act;
		}
	}
	RELLOCK(&mutex,"mutex");
	DEBY
	return(act);
}
//**********************************************************************************
//**********************************************************************************
void SM::insert_move(int _i, PREDICATE_ACTION* _pa){

	DEBOUT("SM::insert_move", _i << " " << _pa )
	move_sm.insert(pair<const int, PREDICATE_ACTION*>(_i, _pa));

}
SM::SM(ENGINE* _eng, SL_CO* _sl_co){

	DEBOUT("SM::SM", "")
	sl_cc = _eng;
    sl_co = _sl_co;

    pthread_mutex_init(&mutex, NULL);
	State = 0;

//	controlSequence = 1;
}
ENGINE* SM::getSL_CC(void){
	return sl_cc;
}
SL_CO* SM::getSL_CO(void){
	return sl_co;
}
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
PREDICATE_ACTION::PREDICATE_ACTION(SM* _sm){
	machine = _sm;
}
//**********************************************************************************
//**********************************************************************************

//*****************************************************************
// INVITE_A from network
//*****************************************************************
bool pre_0_1_inv_sv(SM* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_INV_SV pre_0_1_inv_sv called","")
	if (_message->getReqRepType() == REQSUPP
			&& (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST)
			&& _message->getDestEntity() == SODE_TRNSCT_SV
			&& _message->getGenEntity() ==  SODE_NTWPOINT){
		DEBOUT("TRNSCT_INV_SV pre_0_1_inv_sv","true")
		return true;
	}
	else {
		DEBOUT("TRNSCT_INV_SV pre_0_1_inv_sv","false")
		return false;
	}
}
ACTION* act_0_1_inv_sv(SM* _sm, MESSAGE* _message) {

	DEBOUT("TRSNCT_INV_SV::act_0_1_inv_sv", _message->getHeadSipRequest().getContent())

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//_message changes its dest and gen
	_message->setDestEntity(SODE_ALOPOINT);
	_message->setGenEntity(SODE_TRNSCT_SV);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	CREATEMESSAGE(etry, _message, SODE_TRNSCT_SV,SODE_NTWPOINT)
	SipUtil.genTryFromInvite(_message, etry);
	etry->typeOfInternal = TYPE_MESS;

	SingleAction sa_2 = SingleAction(etry);

	action->addSingleAction(sa_1);
	action->addSingleAction(sa_2);

	DEBOUT("TRSNCT_INV_SV::act_0_1_inv_sv move to state 1","")

	_sm->State = 1;

	//OVERALL
	//Send to ALARM: message for maximum time in calling state (64*T1)
	//TODO
	_sm->getSL_CO()->OverallState_SV = OS_PROCEEDING;

	return action;

}
//***********************************************************
//RETRANSMIT THE TRY
ACTION* act_1_1_inv_sv(SM* _sm, MESSAGE* _message) {

	DEBOUT("TRSNCT_INV_SV::act_1_1_inv_sv", _message->getHeadSipRequest().getContent())

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	CREATEMESSAGE(etry, _message, SODE_TRNSCT_SV,SODE_NTWPOINT)
	SipUtil.genTryFromInvite(_message, etry);
	etry->typeOfInternal = TYPE_MESS;

	SingleAction sa_1 = SingleAction(etry);

	action->addSingleAction(sa_1);

	DEBOUT("TRSNCT_INV_SV::act_1_1_inv_sv move to state 1","")

	_sm->State = 1;

	return action;

}
//*****************************************************************
bool pre_1_2_inv_sv(SM* _sm, MESSAGE* _message){

	DEBOUT("SM_SV pre_1_2_inv_sv called","")

	if (_message->getReqRepType() == REPSUPP
		&& (_message->getHeadSipReply().getReply().getCode() == DIALOGE_101
				|| _message->getHeadSipReply().getReply().getCode() == RINGING_180)
		&& _message->getDestEntity() == SODE_TRNSCT_SV
		&& _message->getGenEntity() ==  SODE_TRNSCT_CL) {
			DEBOUT("SM_SV pre_1_2_inv_sv","true")
			return true;
		}
		else {
			DEBOUT("SM_SV pre_1_2_inv_sv","false")
			return false;
		}
}
ACTION* act_1_2_inv_sv(SM* _sm, MESSAGE* _message) {

	DEBOUT("TRSNCT_INV_SV::act_1_2_inv_sv",  _message->getHeadSipReply().getContent() )

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	// The message ha been prepared by client so it's ready to be sent back
	_message->setDestEntity(SODE_NTWPOINT);
	_message->setGenEntity(SODE_TRNSCT_SV);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	DEBOUT("TRSNCT_INV_SV::act_1_2_inv_sv move to state 2","")

	//Store the message to use it for retransmission
	((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_2 = _message;
	((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_2->setLock();
	_sm->getSL_CO()->call_oset->insertLockedMessage(((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_2);

	_sm->State = 2;

	//OVERALL
	//TODO
	//cancel 64*T1 timer
	_sm->getSL_CO()->OverallState_SV = OS_PROCEEDING;

	return action;
}
//*****************************************************************
ACTION* act_2_2_inv_sv(SM* _sm, MESSAGE* _message) {

	DEBOUT("TRSNCT_INV_SV::act_2_2_inv_sv",  _message->getHeadSipReply().getContent() )

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//Resend stored message
	SingleAction sa_1 = SingleAction(((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_2);

	action->addSingleAction(sa_1);

	DEBOUT("TRSNCT_INV_SV::act_2_2_inv_sv move to state 2","")

	//OVERALL
	//TODO
	//cancel 64*T1 timer

	return action;

}
//*****************************************************************
bool pre_1_3_inv_sv(SM* _sm, MESSAGE* _message){

	DEBOUT("SM_V5 pre_1_3_inv_sv","")

	if (_message->getReqRepType() == REPSUPP
		&& _message->getHeadSipReply().getReply().getCode() == OK_200
		&& _message->getDestEntity() == SODE_TRNSCT_SV
		&& _message->getGenEntity() ==  SODE_ALOPOINT) {
			DEBOUT("SM_V5 pre_1_3_inv_sv","true")
			return true;
	}
	else {
		DEBOUT("SM pre_1_3_inv_sv","false")
		return false;
	}
}
ACTION* act_1_3_inv_sv(SM* _sm, MESSAGE* _message) {

	DEBOUT("SM act_1_3_inv_sv called","")

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: Forward 200 OK to A
	_message->setDestEntity(SODE_NTWPOINT);
	_message->setGenEntity(SODE_TRNSCT_SV);
	_message->typeOfInternal = TYPE_MESS;
	//replace with 200ok A
	((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3 = _message;
	((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3->setLock();
	_sm->getSL_CO()->call_oset->insertLockedMessage(((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3);
	SingleAction sa_1 = SingleAction(_message);
	action->addSingleAction(sa_1);
	//**************************************
	//**************************************


	//**************************************
	//Action 2: copy the 200 OK and send to ALARM
	//This will be sent to A which will resend the ACK
	CREATEMESSAGE(ack_timer, _message, SODE_TRNSCT_SV, SODE_TRNSCT_SV)
	SysTime afterT;
	GETTIME(afterT);
	// T1 and not 2+T1
	unsigned long long int firetime = ((unsigned long long int) afterT.tv.tv_sec)*1000000+(unsigned long long int)afterT.tv.tv_usec + TIMER_1;
	ack_timer->setFireTime(firetime);
	ack_timer->typeOfInternal = TYPE_OP;
	ack_timer->typeOfOperation = TYPE_OP_TIMER_ON;
	ack_timer->orderOfOperation = "TIMER_G";
	ack_timer->setLock();
	_sm->getSL_CO()->call_oset->insertLockedMessage(ack_timer);
	SingleAction sa_2 = SingleAction(ack_timer);
	action->addSingleAction(sa_2);
	//**************************************
	//**************************************

	DEBOUT("SM act_1_3_inv_sv move to state 3","")
	_sm->State = 3;

	_sm->getSL_CO()->OverallState_SV = OS_COMPLETED;

	return action;

	//TODO
	// resend 200 ok until ack arrived which is state = OS_CONFIRMED

	//OVERALL
	//TODO
	//cancel 64*T1 timer


}
ACTION* act_3_3a_inv_sv(SM* _sm, MESSAGE* _message) {

	//Invite from A coming again

	DEBOUT("SM act_3_3a_inv_sv called","")

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//Invite from A can be purged
	PURGEMESSAGE(_message)

	//**************************************
	//Action 1: Send stored 200 OK to A
	((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3->setDestEntity(SODE_NTWPOINT);
	((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3->setGenEntity(SODE_TRNSCT_SV);
	((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3);
	action->addSingleAction(sa_1);
	//**************************************
	//**************************************


	//**************************************
	//Action 2: alarm for resending the 200 OK to A
	CREATEMESSAGE(ack_timer, ((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3, SODE_TRNSCT_SV,SODE_TRNSCT_SV)
	SysTime afterT;
	GETTIME(afterT);
	unsigned long long int firetime = ((unsigned long long int) afterT.tv.tv_sec)*1000000+(unsigned long long int)afterT.tv.tv_usec + TIMER_1;
	ack_timer->setFireTime(firetime);
	ack_timer->typeOfInternal = TYPE_OP;
	ack_timer->typeOfOperation = TYPE_OP_TIMER_ON;
	ack_timer->orderOfOperation = "TIMER_G";
	ack_timer->setLock();
	_sm->getSL_CO()->call_oset->insertLockedMessage(ack_timer);
	SingleAction sa_2 = SingleAction(ack_timer);
	action->addSingleAction(sa_2);
	//**************************************
	//**************************************


	//**************************************
	//Action 3: Clear alarm for 200 OK to A (created in 1_3)
	CREATEMESSAGE(ack_timer_clear, ((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3, SODE_TRNSCT_SV,SODE_TRNSCT_SV)
	ack_timer_clear->typeOfInternal = TYPE_OP;
	ack_timer_clear->typeOfOperation = TYPE_OP_TIMER_OFF;
	ack_timer_clear->orderOfOperation = "TIMER_G";
	SingleAction sa_3 = SingleAction(ack_timer_clear);
	action->addSingleAction(sa_3);
	//**************************************
	//**************************************

	return action;

}
bool pre_3b_3b_inv_sv(SM* _sm, MESSAGE* _message){

	DEBOUT("SM_V5 pre_3b_3b_inv_sv","")

	if (_message->getReqRepType() == REPSUPP
		&& _message->getHeadSipReply().getReply().getCode() == OK_200
		&& _message->getDestEntity() == SODE_TRNSCT_SV
		&& _message->getGenEntity() ==  SODE_TRNSCT_SV
		&&  ((TRNSCT_SM_INVITE_SV*)_sm)->resend_200ok < MAX_INVITE_RESEND
		&& _sm->getSL_CO()->OverallState_SV == OS_PROCEEDING) {
			DEBOUT("SM_V5 pre_3b_3b_inv_sv","true")
			return true;
	}
	else {
		DEBOUT("SM pre_3b_3b_inv_sv","false")
		return false;
	}
}
ACTION* act_3_3b_inv_sv(SM* _sm, MESSAGE* _message) {

	DEBOUT("SM act_3_3b_inv_sv called","")

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: 200ok from alarm send it
	_message->setDestEntity(SODE_NTWPOINT);
	_message->setGenEntity(SODE_TRNSCT_SV);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);
	action->addSingleAction(sa_1);
	//**************************************
	//**************************************


	//**************************************
	//Action 2: 200ok from alarm retrigger
	((TRNSCT_SM_INVITE_SV*)_sm)->resend_200ok++;
	CREATEMESSAGE(ack_timer, _message, SODE_TRNSCT_SV,SODE_TRNSCT_SV)
	SysTime afterT;
	GETTIME(afterT);
	//Shoudl be T1 and not 2xT1
	unsigned long long int firetime = ((unsigned long long int) afterT.tv.tv_sec)*1000000+(unsigned long long int)afterT.tv.tv_usec + TIMER_1 * pow(2,((TRNSCT_SM_INVITE_SV*)_sm)->resend_200ok);
	ack_timer->setFireTime(firetime);
	ack_timer->typeOfInternal = TYPE_OP;
	ack_timer->typeOfOperation = TYPE_OP_TIMER_ON;
	ack_timer->orderOfOperation = "TIMER_G";
	ack_timer->setLock();
	_sm->getSL_CO()->call_oset->insertLockedMessage(ack_timer);
	SingleAction sa_2 = SingleAction(ack_timer);
	action->addSingleAction(sa_2);
	//**************************************
	//**************************************


	DEBOUT("SM act_3_3b_inv_sv move to state 3","")
	return action;

}
bool pre_3_3c_inv_sv(SM* _sm, MESSAGE* _message){

	DEBOUT("SM_V5 pre_3b_3b_inv_sv","")

	if (_message->getReqRepType() == REPSUPP
		&& _message->getHeadSipReply().getReply().getCode() == OK_200
		&& _message->getDestEntity() == SODE_TRNSCT_SV
		&& _message->getGenEntity() ==  SODE_TRNSCT_SV
		&&  (((TRNSCT_SM_INVITE_SV*)_sm)->resend_200ok >= MAX_INVITE_RESEND
		|| _sm->getSL_CO()->OverallState_SV == OS_CONFIRMED)) {
			DEBOUT("SM_V5 pre_3b_3c_inv_sv","true")
			return true;
	}
	else {
		DEBOUT("pre_3b_3c_inv_sv","false")
		return false;
	}
}
ACTION* act_3_3c_inv_sv(SM* _sm, MESSAGE* _message) {

	DEBOUT("SM act_3_3c_inv_sv called no actions","")

	//**************************************
	//Purge it
	_message->unSetLock();
	_sm->getSL_CO()->call_oset->removeLockedMessage(_message);

	//**************************************
	//Purge also STOREDMESS?
	return ((ACTION*) 0x0);
}

//bool pre_3b_5_inv_sv(SM* _sm, MESSAGE* _message){
//
//	DEBOUT("SM_V5 pre_3b_3b_inv_sv","")
//
//	if (_message->getReqRepType() == REPSUPP
//		&& _message->getHeadSipReply().getReply().getCode() == OK_200
//		&& _message->getDestEntity() == SODE_TRNSCT_SV
//		&& _message->getGenEntity() ==  SODE_TRNSCT_SV
//		&&  ((TRNSCT_SM_INVITE_SV*)_sm)->resend_200ok >= MAX_INVITE_RESEND) {
//			DEBOUT("SM_V5 pre_3b_3b_inv_sv","true")
//			return true;
//	}
//	else {
//		DEBOUT("SM pre_3b_3b_inv_sv","false")
//		return false;
//	}
//}

bool pre_3_4_inv_sv(SM* _sm, MESSAGE* _message){

	DEBOUT("SM_V5 pre_3_4_inv_sv called","")
	if (_message->getReqRepType() == REQSUPP
			&& (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == ACK_REQUEST)
			&& _message->getDestEntity() == SODE_TRNSCT_SV
			&& _message->getGenEntity() ==  SODE_NTWPOINT) {
		DEBOUT("SM pre_3_4_inv_sv","true")
		return true;
	}
	else {
		DEBOUT("SM pre_3_4_inv_sv","false")
		return false;
	}
}

ACTION* act_3_4_inv_sv(SM* _sm, MESSAGE* _message) {

	DEBOUT("SM act_3_4_inv_sv called","")

	DEBOUT("SM::event move to state 4", _message->getHeadSipRequest().getContent())

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//_message changes its dest and gen
	_message->setDestEntity(SODE_ALOPOINT);
	_message->setGenEntity(SODE_TRNSCT_SV);
	_message->typeOfInternal = TYPE_MESS;
	_message->type_trnsct = TYPE_NNTRNSCT;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	DEBOUT("SM act_3_4_inv_sv move to state 4","")
	_sm->State = 4;

	return action;

}

//**********************************************************************************
TRNSCT_SM_INVITE_SV::TRNSCT_SM_INVITE_SV(int _requestType, MESSAGE* _matrixMess, ENGINE* _sl_cc, SL_CO* _sl_co):
		TRNSCT_SM(_requestType, _matrixMess, _matrixMess, _sl_cc, _sl_co),
		PA_INV_0_1SV((SM*)this),
		PA_INV_1_2SV((SM*)this),
		PA_INV_2_2SV((SM*)this),
		PA_INV_1_3SV((SM*)this),
		PA_INV_3_4SV((SM*)this),
		PA_INV_1_1SV((SM*)this),
		PA_INV_3_3aSV((SM*)this),
		PA_INV_3_3bSV((SM*)this),
		PA_INV_3_3cSV((SM*)this){

	resend_200ok = 0;

	STOREMESS_1_2 = 0x0;
	STOREMESS_1_3 = 0x0;


	PA_INV_0_1SV.action = &act_0_1_inv_sv;
	PA_INV_0_1SV.predicate = &pre_0_1_inv_sv;

	PA_INV_1_1SV.action = &act_1_1_inv_sv;
	PA_INV_1_1SV.predicate = &pre_0_1_inv_sv;

	PA_INV_1_2SV.action = &act_1_2_inv_sv;
	PA_INV_1_2SV.predicate = &pre_1_2_inv_sv;

	PA_INV_2_2SV.predicate = &pre_0_1_inv_sv;
	PA_INV_2_2SV.action = &act_2_2_inv_sv;

	PA_INV_1_3SV.action = &act_1_3_inv_sv;
	PA_INV_1_3SV.predicate = &pre_1_3_inv_sv;

	PA_INV_3_4SV.action = &act_3_4_inv_sv;
	PA_INV_3_4SV.predicate = &pre_3_4_inv_sv;

	PA_INV_3_3aSV.action = &act_3_3a_inv_sv;
	PA_INV_3_3aSV.predicate = &pre_0_1_inv_sv;

	PA_INV_3_3bSV.action = &act_3_3b_inv_sv;
	PA_INV_3_3bSV.predicate = &pre_3b_3b_inv_sv;

	PA_INV_3_3cSV.action = &act_3_3c_inv_sv;
	PA_INV_3_3cSV.predicate = &pre_3_3c_inv_sv;

	insert_move(0,&PA_INV_0_1SV);
	insert_move(1,&PA_INV_1_1SV);
	insert_move(1,&PA_INV_1_2SV);
	insert_move(2,&PA_INV_2_2SV);
	insert_move(1,&PA_INV_1_3SV);
	insert_move(2,&PA_INV_1_3SV);
	insert_move(3,&PA_INV_3_4SV);
	insert_move(3,&PA_INV_3_3aSV);
	insert_move(3,&PA_INV_3_3bSV);
	insert_move(3,&PA_INV_3_3cSV);




}
//*****************************************************************
// client transaction
//*****************************************************************
// INVITE B
//*****************************************************************
bool pre_0_1_inv_cl(SM* _sm, MESSAGE* _message){

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
ACTION* act_0_1_inv_cl(SM* _sm, MESSAGE* _message) {

	DEBOUT("TRNSCT_INV_CL act_0_1_inv_cl","")

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//Message has to be sent
	_message->setDestEntity(SODE_NTWPOINT);
	_message->setGenEntity(SODE_TRNSCT_CL);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	//careful with source message.
	//Prepare message for Alarm
	CREATEMESSAGE(__timedmessage, _message, SODE_TRNSCT_CL,SODE_TRNSCT_CL)
	__timedmessage->setSourceMessage(_message->getSourceMessage());

	SysTime afterT;
	GETTIME(afterT);
	unsigned long long int firetime = ((unsigned long long int) afterT.tv.tv_sec)*1000000+(unsigned long long int)afterT.tv.tv_usec + TIMER_1;

	DEBOUT("TRNSCT_INV_CL act_0_1_inv_cl creating alarm ", TIMER_1 << " " << firetime)

	__timedmessage->setFireTime(firetime);
	__timedmessage->typeOfInternal = TYPE_OP;
	__timedmessage->typeOfOperation = TYPE_OP_TIMER_ON;
	__timedmessage->orderOfOperation = "TIMER_A";
	__timedmessage->setLock();
	_sm->getSL_CO()->call_oset->insertLockedMessage(__timedmessage);
	SingleAction sa_2 = SingleAction(__timedmessage);

	action->addSingleAction(sa_2);

	DEBOUT("TRNSCT_INV_CL act_0_1_inv_cl resend value", ((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite)

	_sm->State = 1;

	//OVERALL
	//TODO start timer 64*T1
	//Order of operation = 1
	//__timedmessage_2->orderOfOperation = "TIMER_B";
	_sm->getSL_CO()->OverallState_CL = OS_CALLING;


	return action;

}
//*****************************************************************
// INVITE B from ALARM
//*****************************************************************
//*****************************************************************
// careful pre_1_1_inv_cl and pre_1_99_inv_cl are different
// This invite could be deleted, currently is inserted into insertLockedMessage table
bool pre_1_1_inv_cl(SM* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_INV_CL pre_1_1_inv_cl","")
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_TRNSCT_CL
			&& ((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite < MAX_INVITE_RESEND) {
		DEBOUT("TRNSCT_INV_CL pre_1_1_inv_cl","true")
		return true;
	}
	else {
		DEBOUT("TRNSCT_INV_CL pre_1_1_inv_cl","false")
		return false;
	}
}
ACTION* act_1_1_inv_cl(SM* _sm, MESSAGE* _message) {

	DEBOUT("TRNSCT_INV_CL act_1_1_inv_cl","")

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//Message has to be sent
	_message->setDestEntity(SODE_NTWPOINT);
	_message->setGenEntity(SODE_TRNSCT_CL);
	_message->typeOfInternal = TYPE_MESS;
	//This will delete the message in SUDP
	_message->unSetLock();
	_sm->getSL_CO()->call_oset->removeLockedMessage(_message);

	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	//careful with source message.
	//Prepare message for Alarm
	CREATEMESSAGE(__timedmessage, _message, SODE_TRNSCT_CL, SODE_TRNSCT_CL)
	__timedmessage->setSourceMessage(_message->getSourceMessage());

	SysTime afterT;
	GETTIME(afterT);
	//TODO this operation is wrong

	((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite++;


	unsigned long long int firetime = ((unsigned long long int) afterT.tv.tv_sec)*1000000+(unsigned long long int)afterT.tv.tv_usec + TIMER_1 * pow(2,((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite);
//	afterT.tv.tv_sec = afterT.tv.tv_sec + TIMER_1_sc*(((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite+1);
//	afterT.tv.tv_usec = afterT.tv.tv_usec + TIMER_1_mc*(((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite+1);

	DEBOUT("TRNSCT_INV_CL act_1_1_inv_cl creating alarm ", pow(2,((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite) << " " << firetime)

	__timedmessage->setFireTime(firetime);
	__timedmessage->typeOfInternal = TYPE_OP;
	__timedmessage->typeOfOperation = TYPE_OP_TIMER_ON;
	__timedmessage->orderOfOperation = "TIMER_A";
	__timedmessage->setLock();
	_sm->getSL_CO()->call_oset->insertLockedMessage(__timedmessage);
	SingleAction sa_2 = SingleAction(__timedmessage);

	action->addSingleAction(sa_2);

	DEBOUT("TRNSCT_INV_CL act_1_1_inv_cl resend value", ((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite)

	_sm->State = 1;

	//OVERALL
	//TODO start timer 64*T1
	//Order of operation = 1
	//__timedmessage_2->orderOfOperation = "TIMER_B";
	_sm->getSL_CO()->OverallState_CL = OS_CALLING;


	return action;
}

//*****************************************************************
bool pre_1_99_inv_cl(SM* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_INV_CL pre_1_99_inv_cl","")
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_TRNSCT_CL
			&& ((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite >= MAX_INVITE_RESEND) {
		DEBOUT("TRNSCT_INV_CL pre_1_99_inv_cl","true")
		return true;
	}
	else {
		DEBOUT("TRNSCT_INV_CL pre_1_99_inv_cl","false")
		return false;
	}
}
ACTION* act_1_99_inv_cl(SM* _sm, MESSAGE* _message) {

	DEBOUT("TRNSCT_INV_CL act_1_99_inv_cl *** incomplete *** ","")

	_message->unSetLock();
	_sm->getSL_CO()->call_oset->removeLockedMessage(_message);

	((SL_CC*)(_sm->getSL_CC()))->getCOMAP()->setDoaRequested(_sm->getSL_CO()->call_oset, _message->getModulus());


	_sm->State = 99;

	return 0x0;

}
//*****************************************************************
bool pre_1_2_inv_cl(SM* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_INV_CL pre_1_1b_inv_cl","")

	if (_message->getReqRepType() == REPSUPP
		&&_message->getHeadSipReply().getReply().getCode() == TRYING_100
		&& _message->getDestEntity() == SODE_TRNSCT_CL
		&& _message->getGenEntity() ==  SODE_NTWPOINT) {
			DEBOUT("TRNSCT_INV_CL pre_1_2_inv_cl","true")
			return true;
		}
		else {
			DEBOUT("TRNSCT_INV_CL pre_1_2_inv_cl","false")
			return false;
		}
}
ACTION* act_1_2_inv_cl(SM* _sm, MESSAGE* _message) {

	DEBOUT("TRNSCT_INV_CL act_1_2_inv_cl",_message->getHeadSipReply().getReply().getCode())

	// TODO clear timer ad create new timer for the ringing ?

	NEWPTR(ACTION*, action, ACTION(),"ACTION")
	_message->typeOfInternal = TYPE_OP;
	_message->typeOfOperation = TYPE_OP_TIMER_OFF;
	_message->orderOfOperation = "TIMER_A";

	SingleAction sa_1 = SingleAction(_message);
	action->addSingleAction(sa_1);

	//TODO the message has no esplicit destination because type is OP
	_message->setGenEntity(SODE_TRNSCT_CL);
	//_message->setGenEntity(SODE_TIMERPOINT);

	_sm->State = 2;

	//OVERALL
	//TODO  clear 64*T1
	//__timedmessage_2->orderOfOperation = "TIMER_B";
	_sm->getSL_CO()->OverallState_CL = OS_PROCEEDING;


	return action;
}
bool pre_1_3_inv_cl(SM* _sm, MESSAGE* _message){

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
ACTION* act_1_3_inv_cl(SM* _sm, MESSAGE* _message) {

	DEBOUT("SM_CL act_1_3_inv_cl","")

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	// the message contains the to tag that we must save
	// or store it in valo during 200ok

	MESSAGE* __message = ((TRNSCT_SM*)_sm)->getA_Matrix();
	DEBOUT("MESSAGE GENERATOR", __message)

	CREATEMESSAGE(reply_x, __message, SODE_TRNSCT_CL, SODE_TRNSCT_SV)
	SipUtil.genASideReplyFromBReply(_message, __message, reply_x);
	reply_x->purgeSDP();
	reply_x->compileMessage();
	reply_x->dumpVector();
	reply_x->typeOfInternal = TYPE_MESS;

	DEBOUT("CONTACT", reply_x->getHeadContact()->getContent())

	SingleAction sa_1 = SingleAction(reply_x);

	action->addSingleAction(sa_1);


//	//Clear alarm here in case the b did not send any trying
//	CREATEMESSAGE(___message, _message, SODE_TRNSCT_CL)
//	___message->typeOfInternal = TYPE_OP;
//	___message->typeOfOperation = TYPE_OP_TIMER_OFF;
//	___message->setLock();
//	_sm->getSL_CO()->call_oset->insertLockedMessage(___message);
//
//	SingleAction sa_2 = SingleAction(___message);
//	action->addSingleAction(sa_2);
//
//	PURGEMESSAGE(_message);

	//Clear alarm here in case the b did not send any trying
	_message->typeOfInternal = TYPE_OP;
	_message->typeOfOperation = TYPE_OP_TIMER_OFF;
	_message->orderOfOperation = "TIMER_A";
	SingleAction sa_2 = SingleAction(_message);
	action->addSingleAction(sa_2);

	_sm->State = 3;

	//OVERALL
	//TODO  clear 64*T1
	//__timedmessage_2->orderOfOperation = "TIMER_B";
	_sm->getSL_CO()->OverallState_CL = OS_PROCEEDING;

	return action;


}
bool pre_1_4_inv_cl(SM* _sm, MESSAGE* _message){

	DEBOUT("SM pre_1_4_inv_cl","")

	if (_message->getReqRepType() == REPSUPP
		&&_message->getHeadSipReply().getReply().getCode() == OK_200
		&& _message->getDestEntity() == SODE_TRNSCT_CL
		&& _message->getGenEntity() ==  SODE_NTWPOINT) {
			DEBOUT("SM pre_1_4_inv_cl","true")
			return true;
		}
		else {
			DEBOUT("SM pre_1_4_inv_cl","false")
			return false;
		}
}
ACTION* act_1_4_inv_cl(SM* _sm, MESSAGE* _message) {

	DEBOUT("SM act_1_4_inv_cl","")

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	_message->setDestEntity(SODE_ALOPOINT);
	_message->setGenEntity(SODE_TRNSCT_CL);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

//	//Clear alam here in case the b did not send any trying
//	___message->typeOfInternal = TYPE_OP;
//	___message->typeOfOperation = TYPE_OP_TIMER_OFF;
//	___message->setLock();
//	_sm->getSL_CO()->call_oset->insertLockedMessage(___message);
//
//	_message->setLock();
//	_sm->getSL_CO()->call_oset->insertLockedMessage(_message);

	//Clear alam here in case the b did not send any trying
	CREATEMESSAGE(___message, _message, SODE_TRNSCT_CL, SODE_TRNSCT_CL)
	___message->typeOfInternal = TYPE_OP;
	___message->typeOfOperation = TYPE_OP_TIMER_OFF;
	___message->orderOfOperation = "TIMER_A";
	___message->setLock();
	_sm->getSL_CO()->call_oset->insertLockedMessage(___message);

	SingleAction sa_2 = SingleAction(___message);
	action->addSingleAction(sa_2);

	_sm->State = 4;

	//OVERALL
	//TODO  clear 64*T1
	//__timedmessage_2->orderOfOperation = "TIMER_B";
	_sm->getSL_CO()->OverallState_CL = OS_PROCEEDING;
	//resend ACK in case:
	//create a fake client ACK and sent it to client transqaction
	//machine to retrigger the ack towards b

	return action;
}
bool pre_4_4_inv_cl(SM* _sm, MESSAGE* _message){

	DEBOUT("SM pre_4_4_inv_cl message type:",_message->getReqRepType() <<"] reply type code:["<<_message->getHeadSipReply().getReply().getCode() <<"] destination:["<<_message->getDestEntity() <<"] Overall:["<<_sm->getSL_CO()->OverallState_CL)

	if (_message->getReqRepType() == REPSUPP
		&&_message->getHeadSipReply().getReply().getCode() == OK_200
		&& _message->getDestEntity() == SODE_TRNSCT_CL
		&& _message->getGenEntity() ==  SODE_NTWPOINT
		&& _sm->getSL_CO()->OverallState_CL == OS_COMPLETED){
			DEBOUT("SM pre_4_4_inv_cl","true")
			return true;
		}
		else {
			DEBOUT("SM pre_4_4_inv_cl","false")
			return false;
		}
}
ACTION* act_4_4_inv_cl(SM* _sm, MESSAGE* _message) {

	//take the 200OK change to send it to ack_cl

	DEBOUT("SM act_4_4_inv_cl","")

	NEWPTR(ACTION*, action, ACTION(),"ACTION")
	_message->setDestEntity(SODE_TRNSCT_CL);
	_message->setGenEntity(SODE_TRNSCT_CL);
	_message->typeOfInternal = TYPE_OP;
	_message->typeOfOperation = TYPE_OP_SMCOMMAND;

	_message->replaceHeadCSeq(1,"ACK");

	_message->compileMessage();
	_message->setSourceMessage(((TRNSCT_SM*)_sm)->getMatrixMessage());
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	return action;

}

bool pre_4_5_inv_cl(SM* _sm, MESSAGE* _message){

	DEBOUT("SM pre_4_5_inv_cl","")
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == ACK_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_ALOPOINT) {
		DEBOUT("SM pre_4_5_inv_cl","true")
		return true;
	}
	else {
		DEBOUT("SM pre_4_5_inv_cl","false")
		return false;
	}
}
ACTION* act_4_5_inv_cl(SM* _sm, MESSAGE* _message) {

	DEBOUT("SM act_4_5_inv_cl","")

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

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
		PA_INV_0_1CL((SM*)this),
		PA_INV_1_1CL((SM*)this),
		PA_INV_1_2CL((SM*)this),
		PA_INV_1_3CL((SM*)this),
		PA_INV_1_4CL((SM*)this),
		PA_INV_4_5CL((SM*)this),
		PA_INV_4_4CL((SM*)this),
		PA_INV_1_99CL((SM*)this){

	PA_INV_0_1CL.action = &act_0_1_inv_cl;
	PA_INV_0_1CL.predicate = &pre_0_1_inv_cl;

	PA_INV_1_1CL.action = &act_1_1_inv_cl;
	PA_INV_1_1CL.predicate = &pre_1_1_inv_cl;

	PA_INV_1_99CL.action = &act_1_99_inv_cl;
	PA_INV_1_99CL.predicate = &pre_1_99_inv_cl;

	PA_INV_1_2CL.action = &act_1_2_inv_cl;
	PA_INV_1_2CL.predicate = &pre_1_2_inv_cl;

	//from 1 or from 2
	PA_INV_1_3CL.action = &act_1_3_inv_cl;
	PA_INV_1_3CL.predicate = &pre_1_3_inv_cl;

	//from 1 for 2 or 3
	PA_INV_1_4CL.action = &act_1_4_inv_cl;
	PA_INV_1_4CL.predicate = &pre_1_4_inv_cl;

	PA_INV_4_5CL.action = &act_4_5_inv_cl;
	PA_INV_4_5CL.predicate = &pre_4_5_inv_cl;

	PA_INV_4_4CL.action = &act_4_4_inv_cl;
	PA_INV_4_4CL.predicate = &pre_4_4_inv_cl;


	resend_invite = 0;

	//INVITE in
	insert_move(0,&PA_INV_0_1CL);
	//INVITE in from alarm
	insert_move(1,&PA_INV_1_1CL);
	//100 TRY
	insert_move(1,&PA_INV_1_2CL);
	//180 RING or 101 DIALOG EST
	insert_move(1,&PA_INV_1_3CL);
	insert_move(2,&PA_INV_1_3CL);

	//200 OK
	insert_move(1,&PA_INV_1_4CL);
	insert_move(2,&PA_INV_1_4CL);
	insert_move(3,&PA_INV_1_4CL);

	//ACK will be sent in the other SM.
	insert_move(4,&PA_INV_4_5CL);

	insert_move(4,&PA_INV_4_4CL);
	insert_move(5,&PA_INV_4_4CL);

	//manca 3 a 3 con dialoge

	//Resend max reached
	insert_move(1,&PA_INV_1_99CL);


}
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
bool pre_0_1_ack_sv(SM* _sm, MESSAGE* _message){

	DEBOUT("SM pre_0_1_ack_sv called","")
	if (_message->getReqRepType() == REQSUPP
			&& (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == ACK_REQUEST)
			&& _message->getDestEntity() == SODE_TRNSCT_SV
			&& _message->getGenEntity() ==  SODE_NTWPOINT) {
		DEBOUT("SM pre_0_1_ack_sv","true")
		return true;
	}
	else {
		DEBOUT("SM pre_0_1_ack_sv","false")
		return false;
	}
}
ACTION* act_0_1_ack_sv(SM* _sm, MESSAGE* _message) {

	DEBOUT("SM act_0_1_ack_sv called","")

	DEBOUT("SM::event move to state 1", _message->getHeadSipRequest().getContent())

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//_message changes its dest and gen
	_message->setDestEntity(SODE_ALOPOINT);
	_message->setGenEntity(SODE_TRNSCT_SV);
	_message->typeOfInternal = TYPE_MESS;
	_message->type_trnsct = TYPE_TRNSCT;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	DEBOUT("SM act_0_1_ack_sv move to state 1","")
	_sm->State = 1;

	//TODO
	// clear 200 ok alarm sent by invite sv
	//move to confirmed
	_sm->getSL_CO()->OverallState_SV = OS_CONFIRMED;

	return action;

}
//**********************************************************************************
TRNSCT_SM_ACK_SV::TRNSCT_SM_ACK_SV(int _requestType, MESSAGE* _matrixMess, ENGINE* _sl_cc, SL_CO* _sl_co):
		TRNSCT_SM(_requestType, _matrixMess, _matrixMess, _sl_cc, _sl_co),
		PA_ACK_0_1SV((SM*)this){

	PA_ACK_0_1SV.action = &act_0_1_ack_sv;
	PA_ACK_0_1SV.predicate = &pre_0_1_ack_sv;

	insert_move(0,&PA_ACK_0_1SV);
}
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
bool pre_0_1_ack_cl(SM* _sm, MESSAGE* _message){

	DEBOUT("SM pre_0_1_ack_cl","")
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == ACK_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_ALOPOINT) {
		DEBOUT("SM pre_0_1_ack_cl","true")
		return true;
	}
	else {
		DEBOUT("SM pre_0_1_ack_cl","false")
		return false;
	}
}
ACTION* act_0_1_ack_cl(SM* _sm, MESSAGE* _message) {

	DEBOUT("SM act_0_1_ack_cl","")

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	// to test put this:
	//_message->setDestEntity(SODE_NOPOINT);

	_message->setDestEntity(SODE_NTWPOINT);
	_message->setGenEntity(SODE_TRNSCT_CL);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	_sm->State = 1;

	//TODO
	// store this ack and retransmit is 200 ok arrives again
	_sm->getSL_CO()->OverallState_CL = OS_COMPLETED;

	DEBMESSAGE("FIRST ACK B", _message)

	return action;

}
bool pre_1_1_ack_cl(SM* _sm, MESSAGE* _message){

	DEBOUT("SM pre_1_1_ack_cl", _message->getReqRepType() << "]["<<_message->getHeadSipReply().getReply().getCode()<< "]["<<_message->getDestEntity()<< "]["<<_message->getGenEntity())
	if (_message->getReqRepType() == REPSUPP
			&&_message->getHeadSipReply().getReply().getCode() == OK_200
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_TRNSCT_CL) {
		DEBOUT("SM pre_1_1_ack_cl","true")
		return true;
	}
	else {
		DEBOUT("SM pre_1_1_ack_cl","false")
		return false;
	}
}
ACTION* act_1_1_ack_cl(SM* _sm, MESSAGE* _message) {

	DEBOUT("SM act_1_1_ack_cl","resend ACK")

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	CREATEMESSAGE(__message, ((TRNSCT_SM*)_sm)->getMatrixMessage(), SODE_TRNSCT_CL,SODE_NTWPOINT)

	__message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(__message);

	action->addSingleAction(sa_1);

	DEBMESSAGE("SECOND ACK B", __message)

	PURGEMESSAGE(_message)

	return action;

}

//**********************************************************************************
TRNSCT_SM_ACK_CL::TRNSCT_SM_ACK_CL(int _requestType, MESSAGE* _matrixMess, MESSAGE* _A_Matrix, ENGINE* _sl_cc, SL_CO* _sl_co):
		TRNSCT_SM(_requestType, _matrixMess, _A_Matrix, _sl_cc, _sl_co),
		PA_ACK_0_1CL((SM*)this),
		PA_ACK_1_1CL((SM*)this){

	PA_ACK_0_1CL.action = &act_0_1_ack_cl;
	PA_ACK_0_1CL.predicate = &pre_0_1_ack_cl;

	PA_ACK_1_1CL.action = &act_1_1_ack_cl;
	PA_ACK_1_1CL.predicate = &pre_1_1_ack_cl;

	insert_move(0,&PA_ACK_0_1CL);
	insert_move(1,&PA_ACK_1_1CL);

}

//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
bool pre_0_1_bye_sv(SM* _sm, MESSAGE* _message){

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
ACTION* act_0_1_bye_sv(SM* _sm, MESSAGE* _message) {

	DEBOUT("TRSNCT_INV_SV::act_0_1_bye_sv", _message->getHeadSipRequest().getContent())

	DEBOUT("TRSNCT_INV_SV::act_0_1_bye_sv CSeq", _message->getHeadCSeq().getContent())
	DEBOUT("TRSNCT_INV_SV::act_0_1_bye_sv CSeq", _message->getHeadCSeq().getSequence())

    //_sm->setControlSequence(_message->getHeadCSeq().getSequence());

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

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
bool pre_1_2_bye_sv(SM* _sm, MESSAGE* _message){

	DEBOUT("SM pre_1_2_bye_sv","")

	if (_message->getReqRepType() == REPSUPP
		&& _message->getHeadSipReply().getReply().getCode() == OK_200
		&& _message->getDestEntity() == SODE_TRNSCT_SV
		&& _message->getGenEntity() ==  SODE_ALOPOINT) {
			DEBOUT("SM pre_1_2_bye_sv","true")
			return true;
		}
		else {
			DEBOUT("SM pre_1_2_bye_sv","false")
			return false;
		}
}
ACTION* act_1_2_bye_sv(SM* _sm, MESSAGE* _message) {

	DEBOUT("SM act_1_2_bye_sv called","")

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	_message->setDestEntity(SODE_NTWPOINT);
	_message->setGenEntity(SODE_TRNSCT_SV);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	DEBOUT("SM act_1_2_bye_sv move to state 2","")
	_sm->State = 2;

	((SL_CC*)(_sm->getSL_CC()))->getCOMAP()->setDoaRequested(_sm->getSL_CO()->call_oset, _message->getModulus());

	return action;

}
//**********************************************************************************
TRNSCT_SM_BYE_SV::TRNSCT_SM_BYE_SV(int _requestType, MESSAGE* _matrixMess, ENGINE* _sl_cc, SL_CO* _sl_co):
		TRNSCT_SM(_requestType, _matrixMess, _matrixMess, _sl_cc, _sl_co),
		PA_BYE_0_1SV((SM*)this),
		PA_BYE_1_2SV((SM*)this){

	PA_BYE_0_1SV.action = &act_0_1_bye_sv;
	PA_BYE_0_1SV.predicate = &pre_0_1_bye_sv;

	PA_BYE_1_2SV.action = &act_1_2_bye_sv;
	PA_BYE_1_2SV.predicate = &pre_1_2_bye_sv;


	insert_move(0,&PA_BYE_0_1SV);
	insert_move(1,&PA_BYE_1_2SV);

}
bool pre_0_1_bye_cl(SM* _sm, MESSAGE* _message){

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
ACTION* act_0_1_bye_cl(SM* _sm, MESSAGE* _message) {

	DEBOUT("TRNSCT_SM_BYE_CL act_0_1_bye_cl","")

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	_message->setDestEntity(SODE_NTWPOINT);
	_message->setGenEntity(SODE_TRNSCT_CL);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	//careful with source message.
	CREATEMESSAGE(__timedmessage, _message, SODE_TRNSCT_CL, SODE_TRNSCT_CL)
	__timedmessage->setSourceMessage(_message->getSourceMessage());
	//This is to be sent later, after timer expires
	//Preconfigure message entity points, the alarm manager cannot do this

	SysTime afterT;
	GETTIME(afterT);
	unsigned long long int firetime = ((unsigned long long int) afterT.tv.tv_sec)*1000000+(unsigned long long int)afterT.tv.tv_usec + TIMER_1;

	DEBOUT("TRNSCT_SM_BYE_CL act_0_1_bye_cl creating alarm ", TIMER_1  << " " << firetime)
	__timedmessage->setFireTime(firetime);
	__timedmessage->typeOfInternal = TYPE_OP;
	__timedmessage->typeOfOperation = TYPE_OP_TIMER_ON;
	__timedmessage->setLock();
	_sm->getSL_CO()->call_oset->insertLockedMessage(__timedmessage);
	SingleAction sa_2 = SingleAction(__timedmessage);

	action->addSingleAction(sa_2);

	DEBOUT("TRNSCT_SM_BYE_CL act_0_1_bye_cl","")
	_sm->State = 1;

	return action;
}
bool pre_1_1_bye_cl(SM* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_SM_BYE_CL pre_1_1_bye_cl","")
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == BYE_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_TRNSCT_CL
			&& ((TRNSCT_SM_BYE_CL*)_sm)->resend_bye < MAX_INVITE_RESEND) {
		DEBOUT("TRNSCT_SM_BYE_CL pre_1_1_bye_cl","true")
		return true;
	}
	else {
		DEBOUT("TRNSCT_SM_BYE_CL pre_1_1_bye_cl","false")
		return false;
	}
}
ACTION* act_1_1_bye_cl(SM* _sm, MESSAGE* _message) {

	DEBOUT("TRNSCT_SM_BYE_CL act_1_1_bye_cl","")

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	_message->setDestEntity(SODE_NTWPOINT);
	_message->setGenEntity(SODE_TRNSCT_CL);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	//careful with source message.
	CREATEMESSAGE(__timedmessage, _message, SODE_TRNSCT_CL, SODE_TRNSCT_CL)
	__timedmessage->setSourceMessage(_message->getSourceMessage());
	//This is to be sent later, after timer expires
	//Preconfigure message entity points, the alarm manager cannot do this


	SysTime afterT;
	GETTIME(afterT);

	((TRNSCT_SM_BYE_CL*)_sm)->resend_bye++;

	unsigned long long int firetime = ((unsigned long long int) afterT.tv.tv_sec)*1000000+(unsigned long long int)afterT.tv.tv_usec + TIMER_1 * pow(2,((TRNSCT_SM_BYE_CL*)_sm)->resend_bye);

	DEBOUT("TRNSCT_SM_BYE_CL act_1_1_bye_cl creating alarm ", TIMER_1 * pow(2,((TRNSCT_SM_BYE_CL*)_sm)->resend_bye) << " " << firetime)
	__timedmessage->setFireTime(firetime);
	__timedmessage->typeOfInternal = TYPE_OP;
	__timedmessage->typeOfOperation = TYPE_OP_TIMER_ON;
	__timedmessage->setLock();
	_sm->getSL_CO()->call_oset->insertLockedMessage(__timedmessage);
	SingleAction sa_2 = SingleAction(__timedmessage);

	action->addSingleAction(sa_2);

	DEBOUT("TRNSCT_SM_BYE_CL act_1_1_bye_cl","")
	_sm->State = 1;

	return action;
}
bool pre_1_2_bye_cl(SM* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_SM_BYE_CL pre_1_2_bye_cl","")

	if (_message->getReqRepType() == REPSUPP
		&&_message->getHeadSipReply().getReply().getCode() == OK_200
		&& _message->getDestEntity() == SODE_TRNSCT_CL
		&& _message->getGenEntity() ==  SODE_NTWPOINT) {
			DEBOUT("TRNSCT_SM_BYE_CL pre_1_2_bye_cl","true")
			return true;
		}
		else {
			DEBOUT("TRNSCT_SM_BYE_CL pre_1_2_bye_cl","false")
			return false;
		}
}
ACTION* act_1_2_bye_cl(SM* _sm, MESSAGE* _message) {

	DEBOUT("TRNSCT_SM_BYE_CL act_1_2_bye_cl","")

	NEWPTR(ACTION*, action, ACTION(),"ACTION")
	_message->setDestEntity(SODE_ALOPOINT);
	_message->setGenEntity(SODE_TRNSCT_CL);
	_message->typeOfInternal = TYPE_MESS;
	SingleAction sa_1 = SingleAction(_message);

	action->addSingleAction(sa_1);

	CREATEMESSAGE(__message, _message, SODE_TRNSCT_CL, SODE_TRNSCT_CL)
	__message->typeOfInternal = TYPE_OP;
	__message->typeOfOperation = TYPE_OP_TIMER_OFF;
	__message->setLock();
	_sm->getSL_CO()->call_oset->insertLockedMessage(__message);

	SingleAction sa_2 = SingleAction(__message);
	action->addSingleAction(sa_2);

	_sm->State = 2;

	return action;
}
//*****************************************************************
bool pre_1_99_bye_cl(SM* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_BYE_CL pre_1_99_bye_cl","")
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == BYE_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_TRNSCT_CL
			&& ((TRNSCT_SM_BYE_CL*)_sm)->resend_bye >= MAX_INVITE_RESEND) {
		DEBOUT("TRNSCT_BYE_CL pre_1_99_bye_cl","true")
		return true;
	}
	else {
		DEBOUT("TRNSCT_INV_CL pre_1_99_bye_cl","false")
		return false;
	}
}
ACTION* act_1_99_bye_cl(SM* _sm, MESSAGE* _message) {

	DEBOUT("TRNSCT_INV_CL act_1_99_bye_cl *** incomplete *** ","")

	_message->unSetLock();
	_sm->getSL_CO()->call_oset->removeLockedMessage(_message);

	((SL_CC*)(_sm->getSL_CC()))->getCOMAP()->setDoaRequested(_sm->getSL_CO()->call_oset, _message->getModulus());


	_sm->State = 99;

	return 0x0;

}

//**********************************************************************************
TRNSCT_SM_BYE_CL::TRNSCT_SM_BYE_CL(int _requestType, MESSAGE* _matrixMess, MESSAGE* _A_Matrix, ENGINE* _sl_cc, SL_CO* _sl_co):
		TRNSCT_SM(_requestType, _matrixMess, _A_Matrix, _sl_cc, _sl_co),
		PA_BYE_0_1CL((SM*)this),
		PA_BYE_1_1CL((SM*)this),
		PA_BYE_1_99CL((SM*)this),
		PA_BYE_1_2CL((SM*)this){

	PA_BYE_0_1CL.action = &act_0_1_bye_cl;
	PA_BYE_0_1CL.predicate = &pre_0_1_bye_cl;

	PA_BYE_1_1CL.action = &act_1_1_bye_cl;
	PA_BYE_1_1CL.predicate = &pre_1_1_bye_cl;

	PA_BYE_1_99CL.action = &act_1_99_bye_cl;
	PA_BYE_1_99CL.predicate = &pre_1_99_bye_cl;


	PA_BYE_1_2CL.action = &act_1_2_bye_cl;
	PA_BYE_1_2CL.predicate = &pre_1_2_bye_cl;

	insert_move(0,&PA_BYE_0_1CL);
	insert_move(1,&PA_BYE_1_1CL);
	insert_move(1,&PA_BYE_1_99CL);
	insert_move(1,&PA_BYE_1_2CL);

	resend_bye = 0;


}
