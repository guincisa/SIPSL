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

	if (_matrixMess == MainMessage || _matrixMess == 0x0 ){
		DEBASSERT("TRNSCT_SM::TRNSCT_SM invalid Matrix ["<<_matrixMess<<"]")
	}
	if (_a_Matrix == MainMessage || _a_Matrix == 0x0 ){
		DEBASSERT("TRNSCT_SM::TRNSCT_SM invalid A_Matrix ["<<_matrixMess<<"]")
	}

	if (_a_Matrix == _matrixMess){
		Matrix = _matrixMess;
		A_Matrix = _a_Matrix;
		Matrix->setLock(sl_co->call_oset);
	}
	else{
		Matrix = _matrixMess;
		A_Matrix = _a_Matrix;
		Matrix->setLock(sl_co->call_oset);
	}
}
TRNSCT_SM::~TRNSCT_SM(void){

	DEBOUT("TRNSCT_SM::~TRNSCT_SM ",this << "id [" <<id<<"]")

	//A_Matrix belongs to another SM
	Matrix->unSetLock(sl_co->call_oset);
	PURGEMESSAGE(Matrix)
	DEBOUT("TRNSCT_SM::~TRNSCT_SM done",this)

}


MESSAGE* TRNSCT_SM::getMatrixMessage(void){
	if (Matrix == 0x0 || Matrix == MainMessage){
		DEBASSERT("NO")
	}

	return Matrix;
}
MESSAGE* TRNSCT_SM::getA_Matrix(void){
	if (A_Matrix == 0x0 || A_Matrix == MainMessage){
		DEBASSERT("NO")
	}
	return A_Matrix;
}
void TRNSCT_SM::setId(string _id){
	id = _id;
}
string TRNSCT_SM::getId(void){
	return id;
}
SingleAction TRNSCT_SM::generateTimerS(int genPoint){

	DEBOUT("TRNSCT_SM::generateTimerS genpoint",genPoint )

	CREATEMESSAGE(timer_s, getMatrixMessage(), genPoint,genPoint)
	SysTime afterT;
	GETTIME(afterT);
	unsigned long long int firetime = ((unsigned long long int) afterT.tv.tv_sec)*1000000+(unsigned long long int)afterT.tv.tv_usec + TIMER_S;
	timer_s->setFireTime(firetime);
	timer_s->setTypeOfInternal(TYPE_OP);
	timer_s->setTypeOfOperation(TYPE_OP_TIMER_ON);
	timer_s->setOrderOfOperation("TIMER_S");
	timer_s->setLock(sl_co->call_oset);
	return(SingleAction(timer_s));

}
SingleAction TRNSCT_SM::clearTimerS(int genPoint){

	DEBOUT("TRNSCT_SM::generateTimerS genpoint",genPoint )

	CREATEMESSAGE(timer_s, getMatrixMessage(), genPoint,genPoint)
	timer_s->setTypeOfInternal(TYPE_OP);
	timer_s->setTypeOfOperation(TYPE_OP_TIMER_OFF);
	timer_s->setOrderOfOperation("TIMER_S");
	return(SingleAction(timer_s));

}

//**********************************************************************************

ACTION* SM::event(MESSAGE* _event){

	PREDICATE_ACTION* tmp;

	ACTION* act=0x0;

	DEBOUT("SM::event Look for state", State)
	pair<multimap<const int,PREDICATE_ACTION*>::iterator,multimap<const int,PREDICATE_ACTION*>::iterator> ret;
	multimap<const int,PREDICATE_ACTION*>::iterator iter;
	ret = move_sm.equal_range(State);

    for (iter=ret.first; iter!=ret.second; ++iter){
		tmp  = iter->second;
		DEBOUT("SM::event tmp  = iter->second;", iter->second)
		if (tmp->predicate(this, _event)){
			act = tmp->action(this, _event);
			return act;
		}
	}

	//Default event is delete the message
	//TODO not nice
	if(act == 0x0){
		NEWPTR2(act, ACTION(),"ACTION")
			_event->setDestEntity(SODE_KILL);
			_event->setGenEntity(SODE_TRNSCT_SV);
			SingleAction sa_1 = SingleAction(_event);
			act->addSingleAction(sa_1);
	}

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
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************

//*****************************************************************
// INVITE_A from network
//*****************************************************************
bool pre_invite_from_a(SM* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_INV_SV pre_invite_from_a called",_message)
	if (_message->getReqRepType() == REQSUPP
			&& (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST)
			&& _message->getDestEntity() == SODE_TRNSCT_SV
			&& _message->getGenEntity() ==  SODE_NTWPOINT){
		DEBOUT("TRNSCT_INV_SV pre_invite_from_a","true")
		return true;
	}
	else {
		DEBOUT("TRNSCT_INV_SV pre_invite_from_a","false")
		return false;
	}
}
//*****************************************************************
ACTION* act_invite_to_alo(SM* _sm, MESSAGE* _message) {

	//_message riciclato

	DEBOUT("TRSNCT_INV_SV::act_invite_to_alo", _message)
	DEBOUT("TRSNCT_INV_SV::act_invite_to_alo", _message->getHeadSipRequest().getContent())

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: INVITE A is forwarded to ALO
	//_message changes its dest and gen
	//To ALO is a single action so I don't lock it here
	_message->setDestEntity(SODE_ALOPOINT);
	_message->setGenEntity(SODE_TRNSCT_SV);
	_message->setTypeOfInternal(TYPE_MESS);
	SingleAction sa_1 = SingleAction(_message);
	action->addSingleAction(sa_1);

	//**************************************
	//Action 2: 100 TRY is created and sent to NTW
	//This message is stored so it is meant for more thing so I have to lock it
	CREATEMESSAGE(etry, _message, SODE_TRNSCT_SV,SODE_NTWPOINT)
	SipUtil.genTryFromInvite(_message, etry);
	etry->setTypeOfInternal(TYPE_MESS);
	((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_1 = etry;
	((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_1->setLock(_sm->getSL_CO()->call_oset);
	SingleAction sa_2 = SingleAction(etry);
	action->addSingleAction(sa_2);

	//**************************************
	//Action 3: TIMER_S
	// A timer must be always sent locked
	action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_SV));


	//**************************************
	//Local state 1
	DEBOUT("TRSNCT_INV_SV::act_invite_to_alo move to state 1","")
	_sm->State = 1;

	//**************************************
	//OverallState_SV = OS_PROCEEDING
	DEBOUT("TRSNCT_INV_SV::act_invite_to_alo move OverallState_SV to ","OS_PROCEEDING")
	_sm->getSL_CO()->OverallState_SV = OS_PROCEEDING;

	//OVERALL
	//Send to ALARM: message for maximum time in calling state (64*T1)
	//TODO


	//SipUtil.genQuickReplyFromInvite(_message, timer_s, "SIP/2.0 503 Server Unavailable");

	return action;

}
//*****************************************************************
ACTION* act_resend_try_to_a(SM* _sm, MESSAGE* _message) {


	//***********************************************************
	//RETRANSMIT THE TRY
	//***********************************************************
	DEBOUT("TRSNCT_INV_SV::act_resend_try_to_a", _message)
	DEBOUT("TRSNCT_INV_SV::act_resend_try_to_a", _message->getHeadSipRequest().getContent())

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//TODO
	//Store the 100 TRY and use here a copy

	//**************************************
	//Action 1: Retransmit the 100 TRY
	SingleAction sa_1 = SingleAction(((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_1);
	action->addSingleAction(sa_1);

	//**************************************
	//Action 2: Send this message to gc
	_message->setDestEntity(SODE_KILL);
	_message->setGenEntity(SODE_TRNSCT_SV);
	SingleAction sa_2 = SingleAction(_message);
	action->addSingleAction(sa_2);

	//**************************************
	//Action 3: TIMER_S
	action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_SV));
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_SV));


	return action;

}
//*****************************************************************
// 101 DE or 180 RING from SM_CL
//*****************************************************************
 bool pre_provrep_from_cl(SM* _sm, MESSAGE* _message){

	DEBOUT("TRSNCT_INV_SV pre_provreply_from_b called",_message)

	if (_message->getReqRepType() == REPSUPP
		&& (_message->getHeadSipReply().getReply().getCode() == DIALOGE_101
				|| _message->getHeadSipReply().getReply().getCode() == RINGING_180)
		&& _message->getDestEntity() == SODE_TRNSCT_SV
		&& _message->getGenEntity() ==  SODE_TRNSCT_CL) {
			DEBOUT("SM_SV pre_provreply_from_b","true")
			return true;
		}
		else {
			DEBOUT("SM_SV pre_provreply_from_b","false")
			return false;
		}
}
//*****************************************************************
ACTION* act_provreply_to_a(SM* _sm, MESSAGE* _message) {

	//_message riciclato

	DEBOUT("TRSNCT_INV_SV::act_provreply_to_a",  _message)
	DEBOUT("TRSNCT_INV_SV::act_provreply_to_a",  _message->getHeadSipReply().getContent() )

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: Forward the Reply
	// The message ha been prepared by client so it's ready to be sent back
	_message->setDestEntity(SODE_NTWPOINT);
	_message->setGenEntity(SODE_TRNSCT_SV);
	_message->setTypeOfInternal(TYPE_MESS);
	SingleAction sa_1 = SingleAction(_message);
	action->addSingleAction(sa_1);

	//**************************************
	//Action TIMER_S
	action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_SV));
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_SV));


	//**************************************
	//Store the message to use it for retransmission
	((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_2 = _message;
	((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_2->setLock(_sm->getSL_CO()->call_oset);

	//**************************************
	//Local state 2
	DEBOUT("TRSNCT_INV_SV::act_provreply_to_a move to state 2","")
	_sm->State = 2;

	//**************************************
	//OverallState_SV OS_PROCEEDING
	DEBOUT("TRSNCT_INV_SV::act_provreply_to_a move OverallState_SV","OS_PROCEEDING")
	_sm->getSL_CO()->OverallState_SV = OS_PROCEEDING;

	return action;
}
//*****************************************************************
ACTION* act_resend_provreply_to_a(SM* _sm, MESSAGE* _message) {

	DEBOUT("TRSNCT_INV_SV::act_resend_provreply_to_a",  _message )

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: Resend stored message
	SingleAction sa_1 = SingleAction(((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_2);
	action->addSingleAction(sa_1);

	//**************************************
	//Action 2: Delete this one
	_message->setDestEntity(SODE_KILL);
	_message->setGenEntity(SODE_TRNSCT_SV);
	SingleAction sa_2 = SingleAction(_message);
	action->addSingleAction(sa_2);

	//**************************************
	//Action TIMER_S
	action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_SV));
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_SV));

	return action;

}
//*****************************************************************
// 200 OK from SM_CL
//*****************************************************************
bool pre_200ok_from_alo(SM* _sm, MESSAGE* _message){

	DEBOUT("TRSNCT_INV_SV pre_200ok_from_alo",_message)

	if (_message->getReqRepType() == REPSUPP
		&& _message->getHeadSipReply().getReply().getCode() == OK_200
		&& _message->getDestEntity() == SODE_TRNSCT_SV
		&& _message->getGenEntity() ==  SODE_ALOPOINT) {
			DEBOUT("SM_V5 pre_200ok_from_alo","true")
			return true;
	}
	else {
		DEBOUT("SM pre_200ok_from_alo","false")
		return false;
	}
}
//*****************************************************************
ACTION* act_200ok_fwdto_a(SM* _sm, MESSAGE* _message) {

	DEBOUT("TRSNCT_INV_SV act_200ok_to_a called",_message)

	//200ok from ALO
	//send to A
	//create alarm

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: Forward 200 OK to A
	if( ((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3 == MainMessage && _message->getReqRepType() == REPSUPP){
		_message->setDestEntity(SODE_NTWPOINT);
		_message->setGenEntity(SODE_TRNSCT_SV);
		_message->setTypeOfInternal(TYPE_MESS);
		((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3 = _message;
		DEBOUT("STORED_MESSAGE_1_3", ((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3)
		((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3->setLock(_sm->getSL_CO()->call_oset);
	}else {
		DEBASSERT("Don't know what to do")
	}

	SingleAction sa_1 = SingleAction(((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3);
	action->addSingleAction(sa_1);

	//**************************************
	//Action 2: copy the 200 OK and send to ALARM
	//This will be sent to A which will resend the ACK
	//TODO This leaks
	CREATEMESSAGE(ack_timer, ((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3, SODE_TRNSCT_SV, SODE_TRNSCT_SV)
	SysTime afterT;
	GETTIME(afterT);
	// T1 and not 2+T1
	unsigned long long int firetime = ((unsigned long long int) afterT.tv.tv_sec)*1000000+(unsigned long long int)afterT.tv.tv_usec + TIMER_1;
	ack_timer->setFireTime(firetime);
	ack_timer->setTypeOfInternal(TYPE_OP);
	ack_timer->setTypeOfOperation(TYPE_OP_TIMER_ON);
	ack_timer->setOrderOfOperation("TIMER_G");
	ack_timer->setLock(_sm->getSL_CO()->call_oset);
	SingleAction sa_2 = SingleAction(ack_timer);
	action->addSingleAction(sa_2);
	//**************************************
	//**************************************

	//**************************************
	//Action TIMER_S
	action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_SV));
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_SV));

	DEBOUT("SM act_200ok_to_a move to state 3","")
	_sm->State = 3;

	DEBOUT("SM act_200ok_to_a move OverallState_SV","OS_COMPLETED")
	_sm->getSL_CO()->OverallState_SV = OS_COMPLETED;

	return action;

	//OVERALL
	//TODO
	//cancel 64*T1 timer


}
ACTION* act_200ok_refwdto_a(SM* _sm, MESSAGE* _message) {

	DEBOUT("TRSNCT_INV_SV act_200ok_to_a called",_message)

	//200ok from ALARM
	//send to a
	//retrigger alarm

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: Forward stored 200 OK to A
	if( ((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3 == MainMessage){
		DEBASSERT("OK from alarm but STOREMESS_1_3 empty")
	}
	SingleAction sa_1 = SingleAction(((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3);
	action->addSingleAction(sa_1);

	//**************************************
	//Action 2: copy the 200 OK and send to ALARM
	//This will be sent to A which will resend the ACK
	//TODO this message leaks
	CREATEMESSAGE(ack_timer, ((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3, SODE_TRNSCT_SV, SODE_TRNSCT_SV)
	//source is correct
	//ack_timer->setSourceMessage(_message->getSourceMessage());
	SysTime afterT;
	GETTIME(afterT);
	// T1 and not 2+T1
	unsigned long long int firetime = ((unsigned long long int) afterT.tv.tv_sec)*1000000+(unsigned long long int)afterT.tv.tv_usec + TIMER_1;
	ack_timer->setFireTime(firetime);
	ack_timer->setTypeOfInternal(TYPE_OP);
	ack_timer->setTypeOfOperation(TYPE_OP_TIMER_ON);
	ack_timer->setOrderOfOperation("TIMER_G");
	ack_timer->setLock(_sm->getSL_CO()->call_oset);
	SingleAction sa_2 = SingleAction(ack_timer);
	action->addSingleAction(sa_2);

	((TRNSCT_SM_INVITE_SV*)_sm)->resend_200ok ++;

	//**************************************
	//**************************************

//	//**************************************
//	//Action 3: Clear alarm for 200 OK to A (created in 1_3)
//	CREATEMESSAGE(ack_timer_clear, ((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3, SODE_TRNSCT_SV,SODE_TRNSCT_SV)
//	ack_timer_clear->setTypeOfInternal(TYPE_OP);
//	ack_timer_clear->setTypeOfOperation(TYPE_OP_TIMER_OFF);
//	ack_timer_clear->setOrderOfOperation("TIMER_G");
//	SingleAction sa_3 = SingleAction(ack_timer_clear);
//	action->addSingleAction(sa_3);


	//**************************************
	//Action 4: Delete this one
	_message->setDestEntity(SODE_KILL);
	_message->setGenEntity(SODE_TRNSCT_SV);
	SingleAction sa_4 = SingleAction(_message);
	action->addSingleAction(sa_4);

	//**************************************
	//Action TIMER_S
	action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_SV));
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_SV));


	return action;

	//OVERALL
	//TODO
	//cancel 64*T1 timer


}
ACTION* act_200ok_resendto_a(SM* _sm, MESSAGE* _message) {

	DEBOUT("TRSNCT_INV_SV act_200ok_resendto_a called",_message)


	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//INVITE from A
	//resend stored 200ok
	//reset alarm

	//**************************************
	//Action 1: Forward stored 200 OK to A
	if( ((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3 == MainMessage){
		DEBASSERT("OK from alarm but STOREMESS_1_3 empty")
	}
	SingleAction sa_1 = SingleAction(((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3);
	action->addSingleAction(sa_1);

	//**************************************
	//Action 2: copy the 200 OK and send to ALARM
	//This will be sent to A which will resend the ACK
	CREATEMESSAGE(ack_timer, ((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3, SODE_TRNSCT_SV, SODE_TRNSCT_SV)
	//source is correct
	//ack_timer->setSourceMessage(_message->getSourceMessage());
	SysTime afterT;
	GETTIME(afterT);
	// T1 and not 2+T1
	unsigned long long int firetime = ((unsigned long long int) afterT.tv.tv_sec)*1000000+(unsigned long long int)afterT.tv.tv_usec + TIMER_1;
	ack_timer->setFireTime(firetime);
	ack_timer->setTypeOfInternal(TYPE_OP);
	ack_timer->setTypeOfOperation(TYPE_OP_TIMER_ON);
	ack_timer->setOrderOfOperation("TIMER_G");
	ack_timer->setLock(_sm->getSL_CO()->call_oset);
	SingleAction sa_2 = SingleAction(ack_timer);
	action->addSingleAction(sa_2);
	//**************************************
	//**************************************

	//**************************************
	//Action 3: Clear alarm for 200 OK to A (created in 1_3)
	CREATEMESSAGE(ack_timer_clear, ((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3, SODE_TRNSCT_SV,SODE_TRNSCT_SV)
	ack_timer_clear->setTypeOfInternal(TYPE_OP);
	ack_timer_clear->setTypeOfOperation(TYPE_OP_TIMER_OFF);
	ack_timer_clear->setOrderOfOperation("TIMER_G");
	SingleAction sa_3 = SingleAction(ack_timer_clear);
	action->addSingleAction(sa_3);

	//**************************************
	//Action 4: Delete this one
	_message->setDestEntity(SODE_KILL);
	_message->setGenEntity(SODE_TRNSCT_SV);
	SingleAction sa_4 = SingleAction(_message);
	action->addSingleAction(sa_4);

	//**************************************
	//Action TIMER_S
	action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_SV));
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_SV));

	return action;

	//OVERALL
	//TODO
	//cancel 64*T1 timer


}

//*****************************************************************
// 200 OK from Alarm and resend < max and overall state not confirmed
//*****************************************************************
bool pre_200ok_from_alarm(SM* _sm, MESSAGE* _message){

	DEBOUT("TRSNCT_INV_SV pre_200ok_from_alarm",_message)

	if (_message->getReqRepType() == REPSUPP
		&& _message->getHeadSipReply().getReply().getCode() == OK_200
		&& _message->getDestEntity() == SODE_TRNSCT_SV
		&& _message->getGenEntity() ==  SODE_TRNSCT_SV
		&&  ((TRNSCT_SM_INVITE_SV*)_sm)->resend_200ok < MAX_INVITE_RESEND
		&& _sm->getSL_CO()->OverallState_SV != OS_CONFIRMED) {
			DEBOUT("SM_V5 pre_200ok_from_alarm","true")
			return true;
	}
	else {
		DEBOUT("SM pre_200ok_from_alarm","false")
		return false;
	}
}
//*****************************************************************
// 200 OK from Alarm and resend >= max and overall state not confirmed
//*****************************************************************
bool pre_200ok_from_alarm_maxreach(SM* _sm, MESSAGE* _message){

	DEBOUT("TRSNCT_INV_SV pre_200ok_from_alarm_maxreach",_message)

	if (_message->getReqRepType() == REPSUPP
		&& _message->getHeadSipReply().getReply().getCode() == OK_200
		&& _message->getDestEntity() == SODE_TRNSCT_SV
		&& _message->getGenEntity() ==  SODE_TRNSCT_SV
		&&  ((TRNSCT_SM_INVITE_SV*)_sm)->resend_200ok >= MAX_INVITE_RESEND
		&& _sm->getSL_CO()->OverallState_SV != OS_CONFIRMED) {
			DEBOUT("SM_V5 pre_200ok_from_alarm_maxreach","true")
			return true;
	}
	else {
		DEBOUT("pre_200ok_from_alarm_maxreach","false")
		return false;
	}
}
//*****************************************************************
ACTION* act_terminate_sv(SM* _sm, MESSAGE* _message) {

	DEBOUT("TRSNCT_INV_SV act_terminate_sv ",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//TODO CLEAR CALL ACK not arriving

	//Kill this message
	_message->setDestEntity(SODE_KILLDOA);
	_message->setGenEntity(SODE_TRNSCT_SV);
	SingleAction sa_1 = SingleAction(_message);
	action->addSingleAction(sa_1);

	//**************************************
	//Action TIMER_S
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_SV));


	DEBOUT("SM act_200ok_to_a move to state 5","")
	_sm->State = 5;

	DEBOUT("SM act_200ok_to_a move OverallState_SV","OS_TERMINATED")
	_sm->getSL_CO()->OverallState_SV = OS_TERMINATED;

	//TODO
	//((SL_CC*)(_sm->getSL_CC()))->getCOMAP()->setDoaRequested(_sm->getSL_CO()->call_oset, _message->getModulus());


	return (action);
}
//*****************************************************************
// 200 OK from Alarm overall state confirmed
//*****************************************************************
bool pre_200ok_from_alarm_confirm(SM* _sm, MESSAGE* _message){

	DEBOUT("TRSNCT_INV_SV pre_200ok_from_alarm_confirm",_message)

	if (_message->getReqRepType() == REPSUPP
		&& _message->getHeadSipReply().getReply().getCode() == OK_200
		&& _message->getDestEntity() == SODE_TRNSCT_SV
		&& _message->getGenEntity() ==  SODE_TRNSCT_SV
		&&  _sm->getSL_CO()->OverallState_SV == OS_CONFIRMED) {
			DEBOUT("SM_V5 pre_200ok_from_alarm_confirm","true")
			return true;
	}
	else {
		DEBOUT("pre_200ok_from_alarm_confirm","false")
		return false;
	}
}
//*****************************************************************
ACTION* act_null_sv(SM* _sm, MESSAGE* _message) {

	DEBOUT("TRSNCT_INV_SV act_null_sv",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//TODO CLEAR CALL ACK not arriving

	//Kill this message
	_message->setDestEntity(SODE_KILL);
	_message->setGenEntity(SODE_TRNSCT_SV);
	SingleAction sa_1 = SingleAction(_message);
	action->addSingleAction(sa_1);

	//**************************************
	//Action TIMER_S
	action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_SV));
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_SV));

	return (action);

}
bool pre_N_99_inv_sv(SM* _sm, MESSAGE* _message){

	DEBOUT("TRSNCT_INV_SV pre_N_99_inv_sv",_message)

	if (_message->getReqRepType() == REPSUPP
		&& _message->getHeadSipReply().getReply().getCode() == SU_503
		&& _message->getDestEntity() == SODE_TRNSCT_SV
		&& _message->getGenEntity() ==  SODE_TRNSCT_SV
		&& _sm->getSL_CO()->OverallState_SV != OS_CONFIRMED) {
			DEBOUT("SM_V5 pre_N_99_inv_sv","true")
			return true;
	}
	else {
		DEBOUT("SM pre_N_99_inv_sv","false")
		return false;
	}
}
ACTION* act_N_99_inv_sv(SM* _sm, MESSAGE* _message) {

	//Send temination signal to client machine
	DEBOUT("TRSNCT_INV_SV act_N_99_inv_sv",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")


	_message->setDestEntity(SODE_KILLDOA);
	_message->setGenEntity(SODE_TRNSCT_SV);
	SingleAction sa_1 = SingleAction(_message);
	action->addSingleAction(sa_1);

	//**************************************
	//Action TIMER_S
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_SV));


	DEBOUT("SM act_N_99_inv_sv move to state 99","")
	_sm->State = 99;

	DEBOUT("SM act_N_99_inv_sv move OverallState_SV","OS_TERMINATED")
	_sm->getSL_CO()->OverallState_SV = OS_TERMINATED;

	//TODO finish here
	return(action);

}
//*****************************************************************
// TIMER_S
//*****************************************************************
bool pre_timer_s_sv(SM* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_INV_SV pre_timer_s",_message)
	if (_message->getTypeOfInternal() == TYPE_OP
			&& _message->getOrderOfOperation().compare("TIMER_S") == 0
			&& _message->getDestEntity() == SODE_TRNSCT_SV
			&& _message->getGenEntity() ==  SODE_TRNSCT_SV
			&& _sm->getSL_CO()->OverallState_SV != OS_CONFIRMED){
		DEBOUT("TRNSCT_INV_SV pre_timer_s","true")
		return true;
	}
	else {
		DEBOUT("TRNSCT_INV_SV pre_timer_s","false")
		return false;
	}
}
ACTION* act_timer_s_sv(SM* _sm, MESSAGE* _message) {

	//Send temination signal to client machine
	DEBOUT("TRSNCT_INV_SV act_timer_s",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")


	_message->setDestEntity(SODE_KILLDOA);
	_message->setGenEntity(SODE_TRNSCT_SV);
	SingleAction sa_1 = SingleAction(_message);
	action->addSingleAction(sa_1);

	DEBOUT("SM act_timer_s move to state 99","")
	_sm->State = 99;

	DEBOUT("SM act_timer_s move OverallState_SV","OS_TERMINATED")
	_sm->getSL_CO()->OverallState_SV = OS_TERMINATED;
	DEBOUT("SM act_timer_s killed by timer s",_sm->getSL_CO())

	return(action);

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
		PA_INV_3_5SV((SM*)this),
		PA_INV_3_3dSV((SM*)this),
		PA_INV_3_3eSV((SM*)this),
		PA_INV_S_SV((SM*)this){

	resend_200ok = 0;

	STOREMESS_1_1 = MainMessage;
	STOREMESS_1_2 = MainMessage;
	STOREMESS_1_3 = MainMessage;


	//First Invite_A
	//Try_A is sent
	//Invite_A is sent to ALO
	PA_INV_0_1SV.predicate = &pre_invite_from_a;
	PA_INV_0_1SV.action = &act_invite_to_alo;

	//Second Invite_A
	//Try_a lost
	PA_INV_1_1SV.predicate = &pre_invite_from_a;
	PA_INV_1_1SV.action = &act_resend_try_to_a;

	PA_INV_1_2SV.predicate = &pre_provrep_from_cl;
	PA_INV_1_2SV.action = &act_provreply_to_a;

	PA_INV_2_2SV.predicate = &pre_invite_from_a;
	PA_INV_2_2SV.action = &act_resend_provreply_to_a;

	PA_INV_1_3SV.predicate = &pre_200ok_from_alo;
	PA_INV_1_3SV.action = &act_200ok_fwdto_a;

	PA_INV_3_3aSV.predicate = &pre_invite_from_a;
	PA_INV_3_3aSV.action = &act_200ok_resendto_a;

	PA_INV_3_3bSV.predicate = &pre_200ok_from_alarm;
	PA_INV_3_3bSV.action = &act_200ok_refwdto_a;

	PA_INV_3_5SV.predicate = &pre_200ok_from_alarm_maxreach;
	PA_INV_3_5SV.action = &act_terminate_sv;

	PA_INV_3_3dSV.predicate = &pre_200ok_from_alarm_confirm;
	PA_INV_3_3dSV.action = &act_null_sv;

	//Why some 180 are not sent?
	//Probably beause tehy arrive when the sm is already in state 2
	PA_INV_3_3eSV.predicate = &pre_provrep_from_cl;
	PA_INV_3_3eSV.action = &act_null_sv;

	PA_INV_S_SV.predicate = &pre_timer_s_sv;
	PA_INV_S_SV.action = &act_timer_s_sv;

	insert_move(0,&PA_INV_0_1SV);

	insert_move(1,&PA_INV_1_1SV);
	insert_move(1,&PA_INV_1_2SV);
	insert_move(1,&PA_INV_1_3SV);
	insert_move(1,&PA_INV_S_SV);

	insert_move(2,&PA_INV_2_2SV);
	insert_move(2,&PA_INV_3_3eSV);
	insert_move(2,&PA_INV_1_3SV);
	insert_move(2,&PA_INV_S_SV);

	insert_move(3,&PA_INV_3_3aSV);
	insert_move(3,&PA_INV_3_3bSV);
	insert_move(3,&PA_INV_3_5SV);
	insert_move(3,&PA_INV_3_3dSV);
	insert_move(3,&PA_INV_3_3eSV);
	insert_move(3,&PA_INV_S_SV);


}
//*****************************************************************
// client transaction
//*****************************************************************
// INVITE B from ALO
//*****************************************************************
bool pre_invite_from_sv(SM* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_INV_CL pre_invite_from_sv",_message)
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_ALOPOINT) {
		DEBOUT("TRNSCT_INV_CL pre_invite_from_sv","true")
		return true;
	}
	else {
		DEBOUT("TRNSCT_INV_CL pre_invite_from_sv","false")
		return false;
	}
}
//*****************************************************************
// INVITE B from ALARM
//*****************************************************************
//*****************************************************************
// careful pre_1_1_inv_cl and pre_1_99_inv_cl are different
// This invite could be deleted, currently is inserted into insertLockedMessage table
bool pre_invite_from_alarm(SM* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_INV_CL pre_invite_from_alarm",_message)
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_TRNSCT_CL
			&& ((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite < MAX_INVITE_RESEND) {
		DEBOUT("TRNSCT_INV_CL pre_invite_from_alarm","true")
		return true;
	}
	else {
		DEBOUT("TRNSCT_INV_CL pre_invite_from_alarm","false")
		return false;
	}
}
//*****************************************************************
//act_1_1_inv_cl
ACTION* act_invite_to_b(SM* _sm, MESSAGE* _message) {

	DEBOUT("TRNSCT_INV_CL act_invite_to_b",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: send message to NTW
	_message->setDestEntity(SODE_NTWPOINT);
	_message->setGenEntity(SODE_TRNSCT_CL);
	_message->setTypeOfInternal(TYPE_MESS);
	SingleAction sa_1 = SingleAction(_message);
	action->addSingleAction(sa_1);

	//**************************************
	//Action 2: send message to ALARM
	CREATEMESSAGE(__timedmessage, _message, SODE_TRNSCT_CL, SODE_TRNSCT_CL)
	__timedmessage->setSourceMessage(_message->getSourceMessage());
	SysTime afterT;
	GETTIME(afterT);
	unsigned long long int firetime = ((unsigned long long int) afterT.tv.tv_sec)*1000000+(unsigned long long int)afterT.tv.tv_usec + TIMER_1 * pow(2,((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite);
	DEBOUT("TRNSCT_INV_CL act_invite_to_b creating alarm ", pow(2,((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite) << " " << firetime)
	__timedmessage->setFireTime(firetime);
	__timedmessage->setTypeOfInternal(TYPE_OP);
	__timedmessage->setTypeOfOperation(TYPE_OP_TIMER_ON);
	__timedmessage->setOrderOfOperation("TIMER_A");
	__timedmessage->setLock(_sm->getSL_CO()->call_oset);
	SingleAction sa_2 = SingleAction(__timedmessage);
	action->addSingleAction(sa_2);

	//**************************************
	//Action TIMER_S
	action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_CL));


	((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite++;

	DEBOUT("TRNSCT_SM_INVITE_CL act_invite_to_b resend value", ((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite)

	DEBOUT("SM act_invite_to_b move to state","1")
	_sm->State = 1;
	DEBOUT("SM act_invite_to_b move OverallState_CL","OS_CALLING")
	_sm->getSL_CO()->OverallState_CL = OS_CALLING;

	return action;
}
//*****************************************************************
// INVITE B from ALARM max resend reached
//*****************************************************************
// pre_1_99_inv_cl
bool pre_invite_from_alarm_maxreach(SM* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_INV_CL pre_invite_from_alarm_maxreach",_message)
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == INVITE_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_TRNSCT_CL
			&& ((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite >= MAX_INVITE_RESEND) {
		DEBOUT("TRNSCT_INV_CL pre_invite_from_alarm_maxreach","true")
		return true;
	}
	else {
		DEBOUT("TRNSCT_INV_CL pre_invite_from_alarm_maxreach","false")
		return false;
	}
}
//act_1_99_inv_cl
ACTION* act_terminate_cl(SM* _sm, MESSAGE* _message) {

	DEBOUT("TRNSCT_INV_CL act_terminate_cl *** incomplete *** ",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	_message->setDestEntity(SODE_KILLDOA);
	_message->setGenEntity(SODE_TRNSCT_CL);
	SingleAction sa_1 = SingleAction(_message);
	action->addSingleAction(sa_1);

	//**************************************
	//Action TIMER_S
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_CL));


	DEBOUT("SM act_terminate_cl move OverallState_SV to","OS_TERMINATED")
	_sm->getSL_CO()->OverallState_CL = OS_TERMINATED;
	DEBOUT("SM act_terminate_cl move to",99)
	_sm->State = 99;

	return (action);

}
//*****************************************************************
// 100 Try B
//*****************************************************************
//pre_1_2_inv_cl
bool pre_try_from_b(SM* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_INV_CL pre_try_from_b",_message)

	if (_message->getReqRepType() == REPSUPP
		&&_message->getHeadSipReply().getReply().getCode() == TRYING_100
		&& _message->getDestEntity() == SODE_TRNSCT_CL
		&& _message->getGenEntity() ==  SODE_NTWPOINT) {
			DEBOUT("TRNSCT_INV_CL pre_try_from_b","true")
			return true;
		}
		else {
			DEBOUT("TRNSCT_INV_CL pre_try_from_b","false")
			return false;
		}
}
//*****************************************************************
//act_1_2_inv_cl
ACTION* act_clear_invite_alarm(SM* _sm, MESSAGE* _message) {

	DEBOUT("TRNSCT_INV_CL act_clear_invite_alarm",_message)

	DEBOUT("TRNSCT_INV_CL act_clear_invite_alarm",_message->getHeadSipReply().getReply().getCode())

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: clear INVITE B alarm
	_message->setTypeOfInternal(TYPE_OP);
	_message->setTypeOfOperation(TYPE_OP_TIMER_OFF);
	_message->setOrderOfOperation("TIMER_A");
	_message->setGenEntity(SODE_TRNSCT_CL);
	SingleAction sa_1 = SingleAction(_message);
	action->addSingleAction(sa_1);
	//The message has no esplicit destination because type is OP

	//**************************************
	//Action TIMER_S
	action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_CL));
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_CL));


	DEBOUT("SM act_clear_invite_alarm move to state","2")
	_sm->State = 2;
	DEBOUT("SM act_clear_invite_alarm move OverallState_CL to ","OS_PROCEEDING")
	_sm->getSL_CO()->OverallState_CL = OS_PROCEEDING;

	return action;
}
//*****************************************************************
// 101 DE B - 180 RING B
//*****************************************************************
//pre_1_3_inv_cl
bool pre_provrep_from_b(SM* _sm, MESSAGE* _message){

	DEBOUT("SM_CL pre_provrep_from",_message)

	if (_message->getReqRepType() == REPSUPP
		&& (_message->getHeadSipReply().getReply().getCode() == DIALOGE_101
				|| _message->getHeadSipReply().getReply().getCode() == RINGING_180)
		&& _message->getDestEntity() == SODE_TRNSCT_CL
		&& _message->getGenEntity() ==  SODE_NTWPOINT) {
			DEBOUT("SM_CL pre_provrep_from","true")
			return true;
		}
		else {
			DEBOUT("SM_CL pre_provrep_from","false")
			return false;
		}
}
//*****************************************************************
//act_1_3_inv_cl
ACTION* act_provrep_to_sv(SM* _sm, MESSAGE* _message) {

	DEBOUT("SM_CL act_provrep_to_sv",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: create reply using A_Matrix
	// the message contains the to tag that we must save
	// or store it in valo during 200ok
	MESSAGE* __message = ((TRNSCT_SM*)_sm)->getA_Matrix();
	DEBOUT("MESSAGE GENERATOR", __message)
	CREATEMESSAGE(reply_x, __message, SODE_TRNSCT_CL, SODE_TRNSCT_SV)
	SipUtil.genASideReplyFromBReply(_message, __message, reply_x);
	reply_x->purgeSDP();
	reply_x->compileMessage();
	reply_x->dumpVector();
	reply_x->setTypeOfInternal(TYPE_MESS);
	DEBOUT("CONTACT", reply_x->getHeadContact()->getContent())
	SingleAction sa_1 = SingleAction(reply_x);
	action->addSingleAction(sa_1);


	//**************************************
	//Action 2: clear INVITE B alarm
	_message->setTypeOfInternal(TYPE_OP);
	_message->setTypeOfOperation(TYPE_OP_TIMER_OFF);
	_message->setOrderOfOperation("TIMER_A");
	SingleAction sa_2 = SingleAction(_message);
	action->addSingleAction(sa_2);

	//**************************************
	//Action TIMER_S
	action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_CL));
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_CL));


	DEBOUT("SM act_provrep_to_sv move to state","3")
	_sm->State = 3;

	DEBOUT("SM act_provrep_to_sv move OverallState_CL to","OS_PROCEEDING")
	_sm->getSL_CO()->OverallState_CL = OS_PROCEEDING;


	return action;


}
//*****************************************************************
// 200 OK B
//*****************************************************************
//pre_1_4_inv_cl
bool pre_200ok_from_b(SM* _sm, MESSAGE* _message){

	DEBOUT("SM pre_200ok_from_b",_message)

	if (_message->getReqRepType() == REPSUPP
		&&_message->getHeadSipReply().getReply().getCode() == OK_200
		&& _message->getDestEntity() == SODE_TRNSCT_CL
		&& _message->getGenEntity() ==  SODE_NTWPOINT) {
			DEBOUT("SM pre_200ok_from_b","true")
			return true;
		}
		else {
			DEBOUT("SM pre_200ok_from_b","false")
			return false;
		}
}
//*****************************************************************
//act_1_4_inv_cl
ACTION* act_200ok_inv_to_alo(SM* _sm, MESSAGE* _message) {

	DEBOUT("SM act_200ok_inv_to_alo",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: Clear alarm here in case the b did not send any trying
	CREATEMESSAGE(___message, _message, SODE_TRNSCT_CL, SODE_TRNSCT_CL)
	___message->setTypeOfInternal(TYPE_OP);
	___message->setTypeOfOperation(TYPE_OP_TIMER_OFF);
	___message->setOrderOfOperation("TIMER_A");
//	___message->setLock();
//	_sm->getSL_CO()->call_oset->insertLockedMessage(___message);
	SingleAction sa_1 = SingleAction(___message);
	action->addSingleAction(sa_1);

	//**************************************
	//Action 2: 200 OK B to ALO
	//Alo will delete it
	_message->setDestEntity(SODE_ALOPOINT);
	_message->setGenEntity(SODE_TRNSCT_CL);
	_message->setTypeOfInternal(TYPE_MESS);
	_message->setSourceMessage(((TRNSCT_SM*)_sm)->getMatrixMessage());
//	_message->setLock();
//	_sm->getSL_CO()->call_oset->insertLockedMessage(_message);
	SingleAction sa_2 = SingleAction(_message);
	action->addSingleAction(sa_2);

//	//Clear alam here in case the b did not send any trying
//	___message->typeOfInternal = TYPE_OP;
//	___message->typeOfOperation = TYPE_OP_TIMER_OFF;
//	___message->setLock();
//	_sm->getSL_CO()->call_oset->insertLockedMessage(___message);
//
//	_message->setLock();
//	_sm->getSL_CO()->call_oset->insertLockedMessage(_message);

	//**************************************
	//Action TIMER_S
	action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_CL));
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_CL));


	DEBOUT("SM act_200ok_inv_to_alo move to state","4")
	_sm->State = 4;
	DEBOUT("SM act_200ok_inv_to_alo move OverallState_CL to","OS_PROCEEDING")
	_sm->getSL_CO()->OverallState_CL = OS_PROCEEDING;


	//OVERALL
	//TODO  clear 64*T1
	//__timedmessage_2->orderOfOperation = "TIMER_B";
	//resend ACK in case:
	//create a fake client ACK and sent it to client transqaction
	//machine to retrigger the ack towards b

	return action;
}
//*****************************************************************
// 200 OK B in OS_PROCEEDING
// No Need to resend the ACK (not ready)
//*****************************************************************
//pre_4_4a_inv_cl
bool pre_200ok_from_b_proceeding(SM* _sm, MESSAGE* _message){

	DEBOUT("SM_CL pre_200ok_from_b_proceeding",_message)

	//DEBOUT("SM pre_4_4a_inv_cl message type:",_message->getReqRepType() <<"] reply type code:["<<_message->getHeadSipReply().getReply().getCode() <<"] destination:["<<_message->getDestEntity() <<"] Overall:["<<_sm->getSL_CO()->OverallState_CL)

	if (_message->getReqRepType() == REPSUPP
		&&_message->getHeadSipReply().getReply().getCode() == OK_200
		&& _message->getDestEntity() == SODE_TRNSCT_CL
		&& _message->getGenEntity() ==  SODE_NTWPOINT
		&& _sm->getSL_CO()->OverallState_CL == OS_PROCEEDING){
			DEBOUT("SM pre_200ok_from_b_proceeding","true")
			return true;
		}
		else {
			DEBOUT("SM pre_200ok_from_b_proceeding","false")
			return false;
		}
}
////*****************************************************************
//
//ACTION* act_null(SM* _sm, MESSAGE* _message) {
//
//
//	DEBOUT("SM act_4_4a_inv_cl",_message)
//	return 0x0;
//
//}

//*****************************************************************
// 200 OK B in OS_COMPLETED
// Need to resend the ACK
//*****************************************************************
//pre_4_4b_inv_cl
bool pre_200ok_from_b_completed(SM* _sm, MESSAGE* _message){

	DEBOUT("SM_CL pre_200ok_from_b_completed",_message)
	//DEBOUT("SM pre_4_4b_inv_cl message type:",_message->getReqRepType() <<"] reply type code:["<<_message->getHeadSipReply().getReply().getCode() <<"] destination:["<<_message->getDestEntity() <<"] Overall:["<<_sm->getSL_CO()->OverallState_CL)

	if (_message->getReqRepType() == REPSUPP
		&&_message->getHeadSipReply().getReply().getCode() == OK_200
		&& _message->getDestEntity() == SODE_TRNSCT_CL
		&& _message->getGenEntity() ==  SODE_NTWPOINT
		&& _sm->getSL_CO()->OverallState_CL == OS_COMPLETED){
			DEBOUT("SM pre_200ok_from_b_completed","true")
			return true;
		}
		else {
			DEBOUT("SM pre_200ok_from_b_completed","false")
			return false;
		}
}
//act_4_4b_inv_cl
ACTION* act_resend_ack(SM* _sm, MESSAGE* _message) {


	DEBOUT("SM act_resend_ack",_message)
	//Need to resend ACK

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: take the 200OK change to send it to ack_cl
	_message->setDestEntity(SODE_TRNSCT_CL);
	_message->setGenEntity(SODE_TRNSCT_CL);
	_message->setTypeOfInternal(TYPE_OP);
	_message->setTypeOfOperation(TYPE_OP_SMCOMMAND);
	//TODO need the branch!!!
	_message->replaceHeadCSeq(1,"ACK");
	_message->compileMessage();
	//_message->setSourceMessage(((TRNSCT_SM*)_sm)->getMatrixMessage());
        _message->setSourceHeadCallId(((TRNSCT_SM*)_sm)->getMatrixMessage()->getHeadCallId().getContent());
        _message->setSourceModulus(((TRNSCT_SM*)_sm)->getMatrixMessage()->getModulus());
	SingleAction sa_1 = SingleAction(_message);
	action->addSingleAction(sa_1);

	//**************************************
	//Action TIMER_S
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_CL));

	//DEBASSERT("ACTION* act_4_4b_inv_cl need to send this to ACK-CL")
	return action;

}
//*****************************************************************
ACTION* act_null_cl(SM* _sm, MESSAGE* _message) {

	DEBOUT("TRSNCT_INV_CL act_null_cl",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//TODO CLEAR CALL ACK not arriving

	//Kill this message
	_message->setDestEntity(SODE_KILL);
	_message->setGenEntity(SODE_TRNSCT_CL);
	SingleAction sa_1 = SingleAction(_message);
	action->addSingleAction(sa_1);

	//**************************************
	//Action TIMER_S
	action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_CL));
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_CL));

	return (action);

}


//*****************************************************************
// TIMER_S
//*****************************************************************
bool pre_timer_s_cl(SM* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_INV_CL pre_timer_s_cl",_message)
	if (_message->getTypeOfInternal() == TYPE_OP
			&& _message->getOrderOfOperation().compare("TIMER_S") == 0
			&& _message->getDestEntity() == SODE_TRNSCT_SV
			&& _message->getGenEntity() ==  SODE_TRNSCT_SV
			&& _sm->getSL_CO()->OverallState_CL != OS_COMPLETED){
		DEBOUT("TRNSCT_INV_CL pre_timer_s_cl","true")
		return true;
	}
	else {
		DEBOUT("TRNSCT_INV_CL pre_timer_s_cl","false")
		return false;
	}
}
ACTION* act_timer_s_cl(SM* _sm, MESSAGE* _message) {

	//Send temination signal to client machine
	DEBOUT("TRSNCT_INV_CL act_timer_s_cl",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")


	_message->setDestEntity(SODE_KILLDOA);
	_message->setGenEntity(SODE_TRNSCT_SV);
	SingleAction sa_1 = SingleAction(_message);
	action->addSingleAction(sa_1);

	DEBOUT("SM act_timer_s move to state 99","")
	_sm->State = 99;

	DEBOUT("SM act_timer_s move OverallState_SV","OS_TERMINATED")
	_sm->getSL_CO()->OverallState_CL = OS_TERMINATED;
	DEBOUT("SM act_timer_s killed by timer s",_sm->getSL_CO())

	return(action);

}

//**********************************************************************************
TRNSCT_SM_INVITE_CL::TRNSCT_SM_INVITE_CL(int _requestType, MESSAGE* _matrixMess, MESSAGE* _A_Matrix, ENGINE* _sl_cc, SL_CO* _sl_co):
		TRNSCT_SM(_requestType, _matrixMess, _A_Matrix, _sl_cc, _sl_co),
		PA_INV_0_1CL((SM*)this),
		PA_INV_1_1CL((SM*)this),
		PA_INV_1_2CL((SM*)this),
		PA_INV_1_3CL((SM*)this),
		PA_INV_1_4CL((SM*)this),
		PA_INV_4_4aCL((SM*)this),
		PA_INV_4_4bCL((SM*)this),
		PA_INV_4_4cCL((SM*)this),
		PA_INV_1_99CL((SM*)this),
		PA_INV_S_CL((SM*)this){

	PA_INV_0_1CL.predicate = &pre_invite_from_sv;
	PA_INV_0_1CL.action = &act_invite_to_b;

	PA_INV_1_1CL.predicate = &pre_invite_from_alarm;
	PA_INV_1_1CL.action = &act_invite_to_b;

	PA_INV_1_99CL.predicate = &pre_invite_from_alarm_maxreach;
	PA_INV_1_99CL.action = &act_terminate_cl;

	PA_INV_1_2CL.predicate = &pre_try_from_b;
	PA_INV_1_2CL.action = &act_clear_invite_alarm;

	//from 1 or from 2
	PA_INV_1_3CL.predicate = &pre_provrep_from_b;
	PA_INV_1_3CL.action = &act_provrep_to_sv;

	//from 1 for 2 or 3
	PA_INV_1_4CL.predicate = &pre_200ok_from_b;
	PA_INV_1_4CL.action = &act_200ok_inv_to_alo;

	PA_INV_4_4aCL.predicate = &pre_200ok_from_b_proceeding;
	PA_INV_4_4aCL.action = &act_null_cl;

	PA_INV_4_4bCL.predicate = &pre_200ok_from_b_completed;
	PA_INV_4_4bCL.action = &act_resend_ack;

	//Not all 180 are sent back
	PA_INV_4_4cCL.predicate = &pre_provrep_from_b;
	PA_INV_4_4cCL.action = &act_null_cl;

	PA_INV_S_CL.predicate = &pre_timer_s_cl;
	PA_INV_S_CL.action = &act_timer_s_cl;

	resend_invite = 1;

	//INVITE in
	insert_move(0,&PA_INV_0_1CL);

	//INVITE in from alarm
	insert_move(1,&PA_INV_1_1CL);
	//100 TRY
	insert_move(1,&PA_INV_1_2CL);
	//180 RING or 101 DIALOG EST
	insert_move(1,&PA_INV_1_3CL);

	insert_move(1,&PA_INV_S_CL);

	insert_move(2,&PA_INV_1_3CL);
	insert_move(2,&PA_INV_S_CL);

	//Resend max reached
	insert_move(1,&PA_INV_1_99CL);

	//200 OK
	insert_move(1,&PA_INV_1_4CL);
	insert_move(2,&PA_INV_1_4CL);
	insert_move(3,&PA_INV_1_4CL);
	insert_move(3,&PA_INV_S_CL);


	//ACK will be sent in the other SM.

	insert_move(4,&PA_INV_4_4bCL);
	insert_move(4,&PA_INV_4_4aCL);
	insert_move(4,&PA_INV_4_4cCL);

}
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//pre_0_1_ack_sv
bool pre_ack_from_a(SM* _sm, MESSAGE* _message){

	DEBOUT("SM pre_ack_from_a called",_message)
	if (_message->getReqRepType() == REQSUPP
			&& (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == ACK_REQUEST)
			&& _message->getDestEntity() == SODE_TRNSCT_SV
			&& _message->getGenEntity() ==  SODE_NTWPOINT) {
		DEBOUT("SM pre_ack_from_a","true")
		return true;
	}
	else {
		DEBOUT("SM pre_ack_from_a","false")
		return false;
	}
}
//act_0_1_ack_sv
ACTION* act_ack_to_alo(SM* _sm, MESSAGE* _message) {

	DEBOUT("SM act_ack_to_alo called",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: ack is to ALO
	_message->setDestEntity(SODE_ALOPOINT);
	_message->setGenEntity(SODE_TRNSCT_SV);
	_message->setTypeOfInternal(TYPE_MESS);
	_message->setType_trnsct(TYPE_TRNSCT);
	SingleAction sa_1 = SingleAction(_message);
	action->addSingleAction(sa_1);

	DEBOUT("SM act_ack_to_alo move to state 1","")
	_sm->State = 1;
	DEBOUT("SM act_ack_to_alo move OverallState_SV to 1","OS_CONFIRMED")
	_sm->getSL_CO()->OverallState_SV = OS_CONFIRMED;

	//TODO
	// clear 200 ok alarm sent by invite sv
	//move to confirmed

	return action;

}
//act_null
//ACTION* act_1_1_ack_sv(SM* _sm, MESSAGE* _message) {
//
//	DEBOUT("SM act_1_1_ack_sv called",_message)
//
//	//**************************************
//	//Action 1: ACK has been resent by A : ignore
//
//	return 0x0;
//}

//**********************************************************************************
TRNSCT_SM_ACK_SV::TRNSCT_SM_ACK_SV(int _requestType, MESSAGE* _matrixMess, ENGINE* _sl_cc, SL_CO* _sl_co):
		TRNSCT_SM(_requestType, _matrixMess, _matrixMess, _sl_cc, _sl_co),
		PA_ACK_0_1SV((SM*)this),
		PA_ACK_1_1SV((SM*)this){

	PA_ACK_0_1SV.predicate = &pre_ack_from_a;
	PA_ACK_0_1SV.action = &act_ack_to_alo;

	PA_ACK_1_1SV.predicate = &pre_ack_from_a;
	PA_ACK_1_1SV.action = &act_null_sv;


	insert_move(0,&PA_ACK_0_1SV);
	insert_move(1,&PA_ACK_1_1SV);

}
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//pre_0_1_ack_cl
bool pre_ack_from_alo(SM* _sm, MESSAGE* _message){

	DEBOUT("SM pre_ack_from_alo",_message)
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == ACK_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_ALOPOINT) {
		DEBOUT("SM pre_ack_from_alo","true")
		return true;
	}
	else {
		DEBOUT("SM pre_ack_from_alo","false")
		return false;
	}
}
//act_0_1_ack_cl
ACTION* act_ack_to_b(SM* _sm, MESSAGE* _message) {

	DEBOUT("SM act_ack_to_b",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	// to test put this:
	//_message->setDestEntity(SODE_NOPOINT);

	DEBMESSAGE("FIRST ACK B", _message)

	//**************************************
	//Action 1: ack is from ALO
	_message->setDestEntity(SODE_NTWPOINT);
	_message->setGenEntity(SODE_TRNSCT_CL);
	_message->setTypeOfInternal(TYPE_MESS);
	SingleAction sa_1 = SingleAction(_message);
	action->addSingleAction(sa_1);

	DEBOUT("SM act_ack_to_b move to state","1")
	_sm->State = 1;
	DEBOUT("SM act_ack_to_b move OverallState_CL to","OS_COMPLETED")
	_sm->getSL_CO()->OverallState_CL = OS_COMPLETED;

	return action;

}
//pre_1_1_ack_cl
bool pre_200ok_from_inv_cl(SM* _sm, MESSAGE* _message){

	DEBOUT("SM pre_200ok_from_inv_cl",_message)

	if (_message->getReqRepType() == REPSUPP
			&&_message->getHeadSipReply().getReply().getCode() == OK_200
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_TRNSCT_CL) {
		DEBOUT("SM pre_200ok_from_inv_cl","true")
		return true;
	}
	else {
		DEBOUT("SM pre_200ok_from_inv_cl","false")
		return false;
	}
}
//act_1_1_ack_cl
ACTION* act_resend_ack_to_b(SM* _sm, MESSAGE* _message) {

	DEBOUT("SM act_resend_ack_to_b","resend ACK [" << _message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: send ack to B
	CREATEMESSAGE(__message, ((TRNSCT_SM*)_sm)->getMatrixMessage(), SODE_TRNSCT_CL,SODE_NTWPOINT)
	__message->setTypeOfInternal(TYPE_MESS);
	SingleAction sa_1 = SingleAction(__message);
	action->addSingleAction(sa_1);
	DEBMESSAGE("SECOND ACK B", __message)

	_message->setDestEntity(SODE_KILL);
	_message->setGenEntity(SODE_TRNSCT_CL);
	SingleAction sa_2 = SingleAction(_message);
	action->addSingleAction(sa_2);

//	if(!_message->getLock()){
//		_message->setLock();
//		_sm->getSL_CO()->call_oset->insertLockedMessage(_message);
//	}

	return action;

}

//**********************************************************************************
TRNSCT_SM_ACK_CL::TRNSCT_SM_ACK_CL(int _requestType, MESSAGE* _matrixMess, MESSAGE* _A_Matrix, ENGINE* _sl_cc, SL_CO* _sl_co):
		TRNSCT_SM(_requestType, _matrixMess, _A_Matrix, _sl_cc, _sl_co),
		PA_ACK_0_1CL((SM*)this),
		PA_ACK_1_1CL((SM*)this){

	PA_ACK_0_1CL.predicate = &pre_ack_from_alo;
	PA_ACK_0_1CL.action = &act_ack_to_b;

	PA_ACK_1_1CL.predicate = &pre_200ok_from_inv_cl;
	PA_ACK_1_1CL.action = &act_resend_ack_to_b;

	insert_move(0,&PA_ACK_0_1CL);
	insert_move(1,&PA_ACK_1_1CL);

}

//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//pre_0_1_bye_sv
bool pre_bye_from_a(SM* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_INV_SV pre_bye_from_a called",_message)
	if (_message->getReqRepType() == REQSUPP
			&& (_message->getHeadSipRequest().getS_AttMethod().getMethodID() == BYE_REQUEST)
			&& _message->getDestEntity() == SODE_TRNSCT_SV
			&& ( _message->getGenEntity() ==  SODE_NTWPOINT)) {
		DEBOUT("TRNSCT_INV_SV pre_bye_from_a","true")
		return true;
	}
	else {
		DEBOUT("TRNSCT_INV_SV pre_bye_from_a","false")
		return false;
	}
}
//act_0_1_bye_sv
ACTION* act_bye_to_alo(SM* _sm, MESSAGE* _message) {

	DEBOUT("SM act_bye_to_alo called",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1:
	//_message changes its dest and gen
	// remember initial generation  is used for backward messages like bye coming from B
	_message->setDestEntity(SODE_ALOPOINT);
	_message->setGenEntity(SODE_TRNSCT_SV);
	_message->setTypeOfInternal(TYPE_MESS);
	SingleAction sa_1 = SingleAction(_message);
	action->addSingleAction(sa_1);


	//**************************************
	//Action TIMER_S
	action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_SV));

	DEBOUT("TRSNCT_INV_SV::act_bye_to_alo move to state 1","")
	_sm->State = 1;

	return action;

}
ACTION* act_200ok_bye_to_a(SM* _sm, MESSAGE* _message) {

	DEBOUT("SM act_200ok_bye_to_a called",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//Store this 200 OK for retransmission

	((TRNSCT_SM_BYE_SV*)_sm)->STORED_MESSAGE = _message;
	DEBOUT("STORED_MESSAGE", ((TRNSCT_SM_BYE_SV*)_sm)->STORED_MESSAGE)
	((TRNSCT_SM_BYE_SV*)_sm)->STORED_MESSAGE->setLock(_sm->getSL_CO()->call_oset);

	//**************************************
	//Action 1:
	_message->setDestEntity(SODE_NTWPOINT);
	_message->setGenEntity(SODE_TRNSCT_SV);
	_message->setTypeOfInternal(TYPE_MESS);
	SingleAction sa_1 = SingleAction(_message);
	action->addSingleAction(sa_1);

	//**************************************
	//Action TIMER_S
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_SV));

	CREATEMESSAGE(___message, _message, SODE_TRNSCT_SV, SODE_KILLDOA)
	SingleAction sa_3 = SingleAction(___message);
	action->addSingleAction(sa_3);


	DEBOUT("SM act_200ok_bye_to_a move OverallState_SV to","OS_TERMINATED")
	_sm->getSL_CO()->OverallState_SV = OS_TERMINATED;

	DEBOUT("SM act_200ok_bye_to_a move to state 2","")
	_sm->State = 2;

	//((SL_CC*)(_sm->getSL_CC()))->getCOMAP()->setDoaRequested(_sm->getSL_CO()->call_oset, _message->getModulus());

	return action;

}
ACTION* act_resend_200ok_to_a(SM* _sm, MESSAGE* _message) {

	DEBOUT("SM act_resend_200ok_to_a called",_message)


	NEWPTR(ACTION*, action, ACTION(),"ACTION")
	//**************************************
	//Action 1: resend 200 ok
	SingleAction sa_1 = SingleAction(((TRNSCT_SM_BYE_SV*)_sm)->STORED_MESSAGE);
	action->addSingleAction(sa_1);

	//**************************************
	//Action TIMER_S
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_SV));

//	SingleAction sa_3 = SingleAction(_message);
//	action->addSingleAction(sa_3);

	//((SL_CC*)(_sm->getSL_CC()))->getCOMAP()->setDoaRequested(_sm->getSL_CO()->call_oset, _message->getModulus());

	return action;

}
//*****************************************************************
// TIMER_S
//*****************************************************************
bool pre_timer_s_sv_bye(SM* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_BYE_SV pre_timer_s_sv_bye",_message)
	if (_message->getTypeOfInternal() == TYPE_OP
			&& _message->getOrderOfOperation().compare("TIMER_S") == 0
			&& _message->getDestEntity() == SODE_TRNSCT_SV
			&& _message->getGenEntity() ==  SODE_TRNSCT_SV
			&& _sm->getSL_CO()->OverallState_SV != OS_TERMINATED){
		DEBOUT("TRNSCT_BYE_SV pre_timer_s_sv_bye","true")
		return true;
	}
	else {
		DEBOUT("TRNSCT_BYE_SV pre_timer_s_sv_bye","false")
		return false;
	}
}

//**********************************************************************************
TRNSCT_SM_BYE_SV::TRNSCT_SM_BYE_SV(int _requestType, MESSAGE* _matrixMess, ENGINE* _sl_cc, SL_CO* _sl_co):
		TRNSCT_SM(_requestType, _matrixMess, _matrixMess, _sl_cc, _sl_co),
		PA_BYE_0_1SV((SM*)this),
		PA_BYE_1_2SV((SM*)this),
		PA_BYE_1_1SV((SM*)this),
		PA_BYE_2_2SV((SM*)this),
		PA_BYE_S_SV((SM*)this){

	STORED_MESSAGE = MainMessage;

	PA_BYE_0_1SV.predicate = &pre_bye_from_a;
	PA_BYE_0_1SV.action = &act_bye_to_alo;

	PA_BYE_1_2SV.predicate = &pre_200ok_from_alo;
	PA_BYE_1_2SV.action = &act_200ok_bye_to_a;

	//Bye when in state 1:
	//A send a bye because OK did not arrive in time
	//but 200 ok did not arrive from CLSV so ignore

	PA_BYE_1_1SV.predicate = &pre_bye_from_a;
	PA_BYE_1_1SV.action = &act_null_sv;


	//Bye when in state 2
	//resend the 200OK

	PA_BYE_2_2SV.predicate = &pre_bye_from_a;
	PA_BYE_2_2SV.action = &act_resend_200ok_to_a;

	PA_BYE_S_SV.predicate = &pre_timer_s_sv_bye;
	PA_BYE_S_SV.action = &act_timer_s_sv;



	insert_move(0,&PA_BYE_0_1SV);
	insert_move(0,&PA_BYE_S_SV);

	insert_move(1,&PA_BYE_1_2SV);
	insert_move(1,&PA_BYE_1_1SV);
	insert_move(1,&PA_BYE_S_SV);

	insert_move(2,&PA_BYE_2_2SV);


}
//**********************************************************************************
//**********************************************************************************
//pre_0_1_bye_cl
bool pre_bye_from_alo(SM* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_SM_BYE_CL pre_bye_from_alo",_message)
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == BYE_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_ALOPOINT) {
		DEBOUT("TRNSCT_SM_BYE_CL pre_bye_from_alo","true")
		return true;
	}
	else {
		DEBOUT("TRNSCT_SM_BYE_CL pre_bye_from_alo","false")
		return false;
	}

}
bool pre_bye_from_alarm(SM* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_SM_BYE_CL pre_bye_from_alarm",_message)
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == BYE_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_TRNSCT_CL
			&& ((TRNSCT_SM_BYE_CL*)_sm)->resend_bye < MAX_INVITE_RESEND) {
		DEBOUT("TRNSCT_SM_BYE_CL pre_bye_from_alarm","true")
		return true;
	}
	else {
		DEBOUT("TRNSCT_SM_BYE_CL pre_bye_from_alarm","false")
		return false;
	}
}
//act_1_1_bye_cl
ACTION* act_bye_to_b(SM* _sm, MESSAGE* _message) {

	DEBOUT("TRNSCT_SM_BYE_CL act_bye_to_b",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")
	//**************************************
	//Action 1: send to NTW
	_message->setDestEntity(SODE_NTWPOINT);
	_message->setGenEntity(SODE_TRNSCT_CL);
	_message->setTypeOfInternal(TYPE_MESS);
	SingleAction sa_1 = SingleAction(_message);
	action->addSingleAction(sa_1);

	//**************************************
	//Action 2: send to alarm
	//careful with source message.
	CREATEMESSAGE(__timedmessage, _message, SODE_TRNSCT_CL, SODE_TRNSCT_CL)
	__timedmessage->setSourceMessage(_message->getSourceMessage());
	//This is to be sent later, after timer expires
	//Preconfigure message entity points, the alarm manager cannot do this
	SysTime afterT;
	GETTIME(afterT);
	unsigned long long int firetime = ((unsigned long long int) afterT.tv.tv_sec)*1000000+(unsigned long long int)afterT.tv.tv_usec + TIMER_1 * pow(2,((TRNSCT_SM_BYE_CL*)_sm)->resend_bye);
	DEBOUT("TRNSCT_SM_BYE_CL act_bye_to_b creating alarm ", TIMER_1 * pow(2,((TRNSCT_SM_BYE_CL*)_sm)->resend_bye) << " " << firetime)
	__timedmessage->setFireTime(firetime);
	__timedmessage->setTypeOfInternal(TYPE_OP);
	__timedmessage->setTypeOfOperation(TYPE_OP_TIMER_ON);
	__timedmessage->setOrderOfOperation("TIMER_A");
	__timedmessage->setLock(_sm->getSL_CO()->call_oset);
	SingleAction sa_2 = SingleAction(__timedmessage);
	action->addSingleAction(sa_2);

	//**************************************
	//Action TIMER_S
	action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_CL));
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_CL));

	DEBOUT("TRNSCT_SM_BYE_CL act_bye_to_b move to state 1","")
	_sm->State = 1;

	((TRNSCT_SM_BYE_CL*)_sm)->resend_bye++;


	return action;
}
ACTION* act_200ok_bye_to_alo(SM* _sm, MESSAGE* _message) {

	DEBOUT("TRNSCT_SM_BYE_CL act_200ok_bye_to_alo",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: send to ALO
	_message->setDestEntity(SODE_ALOPOINT);
	_message->setGenEntity(SODE_TRNSCT_CL);
	_message->setTypeOfInternal(TYPE_MESS);
//	_message->setLock();
//	_sm->getSL_CO()->call_oset->insertLockedMessage(_message);
	SingleAction sa_1 = SingleAction(_message);
	action->addSingleAction(sa_1);

	//**************************************
	//Action 2: clear alarm
	CREATEMESSAGE(__message, _message, SODE_TRNSCT_CL, SODE_TRNSCT_CL)
	__message->setTypeOfInternal(TYPE_OP);
	__message->setTypeOfOperation(TYPE_OP_TIMER_OFF);
	__message->setOrderOfOperation("TIMER_A");
	SingleAction sa_2 = SingleAction(__message);
	action->addSingleAction(sa_2);


	DEBOUT("SM act_200ok_bye_to_alo move OverallState_SV to","OS_TERMINATED")
	_sm->getSL_CO()->OverallState_CL = OS_TERMINATED;

	DEBOUT("TRNSCT_SM_BYE_CL act_200ok_bye_to_alo move to state 2","")
	_sm->State = 2;

	return action;
}
//*****************************************************************
//pre_1_99_bye_cl
bool pre_bye_from_alarm_maxreach(SM* _sm, MESSAGE* _message){

	DEBOUT("TRNSCT_BYE_CL pre_bye_from_alarm",_message)
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequest().getS_AttMethod().getMethodID() == BYE_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_TRNSCT_CL
			&& ((TRNSCT_SM_BYE_CL*)_sm)->resend_bye >= MAX_INVITE_RESEND) {
		DEBOUT("TRNSCT_BYE_CL pre_bye_from_alarm","true")
		return true;
	}
	else {
		DEBOUT("TRNSCT_INV_CL pre_bye_from_alarm","false")
		return false;
	}
}
//**********************************************************************************
TRNSCT_SM_BYE_CL::TRNSCT_SM_BYE_CL(int _requestType, MESSAGE* _matrixMess, MESSAGE* _A_Matrix, ENGINE* _sl_cc, SL_CO* _sl_co):
		TRNSCT_SM(_requestType, _matrixMess, _A_Matrix, _sl_cc, _sl_co),
		PA_BYE_0_1CL((SM*)this),
		PA_BYE_1_1CL((SM*)this),
		PA_BYE_1_99CL((SM*)this),
		PA_BYE_1_2CL((SM*)this),
		PA_BYE_S_CL((SM*)this){

	PA_BYE_0_1CL.predicate = &pre_bye_from_alo;
	PA_BYE_0_1CL.action = &act_bye_to_b;

	PA_BYE_1_1CL.predicate = &pre_bye_from_alarm;
	PA_BYE_1_1CL.action = &act_bye_to_b;

	PA_BYE_1_99CL.predicate = &pre_bye_from_alarm_maxreach;
	PA_BYE_1_99CL.action = &act_terminate_cl;

	PA_BYE_1_2CL.predicate = &pre_200ok_from_b;
	PA_BYE_1_2CL.action = &act_200ok_bye_to_alo;

	PA_BYE_S_CL.predicate = &pre_timer_s_cl;
	PA_BYE_S_CL.action = &act_timer_s_cl;


	insert_move(0,&PA_BYE_0_1CL);
	insert_move(0,&PA_BYE_S_CL);

	insert_move(1,&PA_BYE_1_1CL);
	insert_move(1,&PA_BYE_S_CL);

	insert_move(1,&PA_BYE_1_99CL);

	insert_move(1,&PA_BYE_1_2CL);

	resend_bye = 1;


}
//**********************************************************************************

