//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Core And Service Layer
// Copyright (C) 2011 Guglielmo Incisa di Camerana
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
#include <algorithm>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <iostream>
#include <map>
#include <math.h>
#include <memory>
#include <pthread.h>
#include <signal.h>
#include <sstream>
#include <stack>
#include <stdio.h>
#include <stdlib.h>     /* for atoi() and exit() */
#include <string>
#include <string.h>
#include <sys/socket.h> /* for socket() and bind() */
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <vector>
#include <unordered_map>

#include "UTIL.h"


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
#ifndef TRNSPRT_H
#include "TRNSPRT.h"
#endif


static SIPUTIL SipUtil;

//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
TRNSCT_SM::TRNSCT_SM(int _requestType, MESSAGE* _matrixMess, MESSAGE* _a_Matrix, ENGINE* _sl_cc, SL_CO_P* _sl_co):
	SM_V6(_sl_cc, _sl_co){
	DEBINF("TRNSCT_SM::TRNSCT_SM(int _requestType, MESSAGE* _matrixMess, MESSAGE* _a_Matrix, ENGINE* _sl_cc, SL_CO* _sl_co):SM(_sl_cc, _sl_co)",
			this<<"]["<<_requestType<<"]["<<_matrixMess<<"]["<<_a_Matrix<<"]["<<_sl_cc<<"]["<<_sl_co)

	requestType = _requestType;

	if (_matrixMess == MainMessage || _matrixMess == 0x0 ){
		DEBASSERT("TRNSCT_SM::TRNSCT_SM invalid Matrix ["<<_matrixMess<<"]")
	}
	if (_a_Matrix == MainMessage || _a_Matrix == 0x0 ){
		DEBASSERT("TRNSCT_SM::TRNSCT_SM invalid A_Matrix ["<<_matrixMess<<"]["<<_a_Matrix)
	}

	if (_a_Matrix == _matrixMess){
		Matrix = _matrixMess;
		A_Matrix = _a_Matrix;
		Matrix->setLock(sl_co->call_oset);
	}
	else{
		Matrix = _matrixMess;
		A_Matrix = _a_Matrix;
#ifdef CHECKDOA
		if ( Comap->getDoaState(sl_co->call_oset,_matrixMess->getModulus()) == DOA_DELETED){
			DEBASSERT("CHECKDOA DOA_DELETED call_oset [" <<this<<"]["<<sl_co->call_oset<<" matrix["<<Matrix<<"]")
		}
#endif
		Matrix->setLock(sl_co->call_oset);
	}
}
TRNSCT_SM::~TRNSCT_SM(void){
	DEBINF("TRNSCT_SM::~TRNSCT_SM(void)",this)
    //A_Matrix belongs to another SM
    Matrix->unSetLock(sl_co->call_oset);
    PURGEMESSAGE(Matrix)
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
	DEBINF("SingleAction TRNSCT_SM::generateTimerS(int genPoint)",this<<"]["<<genPoint)

    CREATEMESSAGE(timer_s, getMatrixMessage(), genPoint,genPoint)
    SysTime afterT;
    GETTIME(afterT);
    lli firetime = ((lli) afterT.tv.tv_sec)*1000000+(lli)afterT.tv.tv_usec + TIMER_S;
    timer_s->setFireTime(firetime);
    timer_s->setTypeOfInternal(TYPE_OP);
    timer_s->setTypeOfOperation(TYPE_OP_TIMER_ON);
    timer_s->setOrderOfOperation("TIMER_S");


    timer_s->setSourceMessage(getA_Matrix());
    DEBY
    return(SingleAction(timer_s));

}
SingleAction TRNSCT_SM::clearTimerS(int genPoint){
	DEBINF("SingleAction TRNSCT_SM::clearTimerS(int genPoint)",this<<"]["<<genPoint)

    CREATEMESSAGE(timer_s, getMatrixMessage(), genPoint,genPoint)
    timer_s->setTypeOfInternal(TYPE_OP);
    timer_s->setTypeOfOperation(TYPE_OP_TIMER_OFF);
    timer_s->setOrderOfOperation("TIMER_S");
    return(SingleAction(timer_s));

}

//**********************************************************************************
//ACTION* SM::event(MESSAGE* _event){
//    TIMEDEF
//    SETNOW
//
//	DEBINF("ACTION* SM::event(MESSAGE* _event)",this<<"]["<<_event)
//    PREDICATE_ACTION* tmp;
//
//    ACTION* act=0x0;
//
//    DEBDEV("SM::event Look for state", State)
//    pair<multimap<const int,PREDICATE_ACTION*>::iterator,multimap<const int,PREDICATE_ACTION*>::iterator> ret;
//    multimap<const int,PREDICATE_ACTION*>::iterator iter;
//    ret = move_sm.equal_range(State);
//
//    for (iter=ret.first; iter!=ret.second; ++iter){
//        tmp  = iter->second;
//        if (tmp->predicate(this, _event)){
//            act = tmp->action(this, _event);
//            PRINTDIFF("ACTION* SM::event(MESSAGE* _event) end")
//            return act;
//        }
//    }
//
//#ifdef DEBCODE
//    if(act != 0x0){
//		stack<SingleAction> actionList = act->getActionList();
//		while (!actionList.empty()){
//			MESSAGE* _tmpMessage = actionList.top().getMessage();
//			if (_tmpMessage == _event){
//				DEBASSERT("DID not duplicate event message")
//			}
//			actionList.pop();
//		}
//    }
//#endif
//    PRINTDIFF("ACTION* SM::event(MESSAGE* _event) end")
//    return(act);
//}
//**********************************************************************************
//**********************************************************************************
//void SM::insert_move(int _i, PREDICATE_ACTION* _pa){
//
//    move_sm.insert(pair<const int, PREDICATE_ACTION*>(_i, _pa));
//
//}
SM_V6::SM_V6(ENGINE* _eng, SL_CO_P* _sl_co){

    sl_cc = _eng;
    sl_co = _sl_co;
    State = 0;

//	controlSequence = 1;
}
ENGINE* SM_V6::getSL_CC(void){
    return sl_cc;
}
SL_CO_P* SM_V6::getSL_CO_P(void){
    return sl_co;
}
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//PREDICATE_ACTION::PREDICATE_ACTION(SM* _sm){
//    machine = _sm;
//}
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************

//*****************************************************************
// INVITE_A from network
//*****************************************************************
bool pre_invite_from_a(SM_V6* _sm, MESSAGE* _message){

    DEBDEV("TRNSCT_INV_SV pre_invite_from_a called",_message)
    if (_message->getReqRepType() == REQSUPP
            && (_message->getHeadSipRequestCode() == INVITE_REQUEST)
            && _message->getDestEntity() == SODE_TRNSCT_SV
            && _message->getGenEntity() ==  SODE_NTWPOINT){
        DEBDEV("TRNSCT_INV_SV pre_invite_from_a","true")
        return true;
    }
    else {
        DEBDEV("TRNSCT_INV_SV pre_invite_from_a","false")
        return false;
    }
}
//*****************************************************************
ACTION* act_invite_to_alo(SM_V6* _sm, MESSAGE* _message) {

	DEBDEV("TRSNCT_INV_SV::act_invite_to_alo", _message)
	//DEBDEV("TRSNCT_INV_SV::act_invite_to_alo", _message->getHeadSipRequest().getContent())

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: INVITE A is duplcated and sent to ALO
	CREATEMESSAGE(m2alo, _message, SODE_TRNSCT_CL, SODE_ALOPOINT)
	m2alo->setSourceMessage(_message);
	SingleAction sa_1 = SingleAction(m2alo);
	action->addSingleAction(sa_1);


	//**************************************
	//Action 3: TIMER_S
	// A timer must be always sent locked
	action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_SV));
	//action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_SV));

	//**************************************
	//Action 2: 100 TRY is created and sent to NTW
	//This message is stored so it is meant for more thing so I have to lock it
	CREATEMESSAGE(etry, _message, SODE_TRNSCT_SV,SODE_NTWPOINT)
	SipUtil.genTryFromInvite(_message, etry);
	((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_1 = etry;

#ifdef CHECKDOA
		if ( Comap->getDoaState(_sm->getSL_CO_P()->call_oset,_message->getModulus()) == DOA_DELETED){
			DEBASSERT("CHECKDOA DOA_DELETED call_oset" <<_sm->getSL_CO_P()->call_oset)
		}
#endif
	((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_1->setLock(_sm->getSL_CO_P()->call_oset);

	#ifdef USEFASTSEND
#ifndef QUICKTRY //if defined, already sent
	_sm->getSL_CO()->call_oset->getTRNSPRT()->p_w(etry);
#endif

#else
	SingleAction sa_2 = SingleAction(etry);
	action->addSingleAction(sa_2);
#endif


	//**************************************
	//Local state 1
	DEBDEV("TRSNCT_INV_SV::act_invite_to_alo move to state 1","")
	_sm->State = 1;

	//**************************************
	//OverallState_SV = OS_PROCEEDING
	DEBDEV("TRSNCT_INV_SV::act_invite_to_alo move OverallState_SV to ","OS_PROCEEDING")
	_sm->getSL_CO_P()->OverallState_SV = OS_PROCEEDING;

	//OVERALL
	//Send to ALARM: message for maximum time in calling state (64*T1)
	//TODO


	//SipUtil.genQuickReplyFromInvite(_message, timer_s, "SIP/2.0 503 Server Unavailable");

	return action;

}
//*****************************************************************
ACTION* act_resend_try_to_a(SM_V6* _sm, MESSAGE* _message) {


	//***********************************************************
	//RETRANSMIT THE TRY
	//***********************************************************
	DEBDEV("TRSNCT_INV_SV::act_resend_try_to_a", _message)
	//DEBDEV("TRSNCT_INV_SV::act_resend_try_to_a", _message->getHeadSipRequest().getContent())

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: No need to duplicate the 100 TRY
	// because the destination is already NTW
	SingleAction sa_1 = SingleAction(((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_1);
	action->addSingleAction(sa_1);


	//**************************************
	//Action 3: TIMER_S
	action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_SV));
	//action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_SV));


	return action;

}
//*****************************************************************
ACTION* act_send_servunav_to_a(SM_V6* _sm, MESSAGE* _message) {

	//send back
	//"503"  ;  Service Unavailable

	//***********************************************************
	//Trnsmit 503
	//***********************************************************
	DEBDEV("TRSNCT_INV_SV::act_send_servunav_to_a", _message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//non so funziona mandare un mess e un kill
	//deve pero chiudere anche lato b
	CREATEMESSAGE(killd,_message, SODE_TRNSCT_SV,SODE_KILLDOA)
	SingleAction sa_1 = SingleAction(killd);
	action->addSingleAction(sa_1);

	CREATEMESSAGE(servunav, ((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_1, SODE_TRNSCT_SV,SODE_NTWPOINT)
	servunav->setHeadSipReply("503 Server Unavailable");
	servunav->compileMessage();
	SingleAction sa_2 = SingleAction(servunav);
	action->addSingleAction(sa_2);

	DEBDEV("SM act_send_servunav_to_a move to state 5","")
	_sm->State = 5;

	DEBDEV("SM act_send_servunav_to_a move OverallState_SV","OS_TERMINATED")
	_sm->getSL_CO_P()->OverallState_SV = OS_TERMINATED;

	return action;

}

//*****************************************************************
// 101 DE or 180 RING from SM_CL
//*****************************************************************
 bool pre_provrep_from_cl(SM_V6* _sm, MESSAGE* _message){

	DEBDEV("TRSNCT_INV_SV pre_provreply_from_b called",_message)

	if (_message->getReqRepType() == REPSUPP
		&& (_message->getHeadSipReplyCode() == DIALOGE_101
				|| _message->getHeadSipReplyCode() == RINGING_180)
		&& _message->getDestEntity() == SODE_TRNSCT_SV
		&& _message->getGenEntity() ==  SODE_TRNSCT_CL) {
			DEBDEV("SM_SV pre_provreply_from_b","true")
			return true;
		}
		else {
			DEBDEV("SM_SV pre_provreply_from_b","false")
			return false;
		}
}
//*****************************************************************
ACTION* act_provreply_to_a(SM_V6* _sm, MESSAGE* _message) {

	//_message riciclato

	DEBDEV("TRSNCT_INV_SV::act_provreply_to_a",  _message)
	//DEBDEV("TRSNCT_INV_SV::act_provreply_to_a",  _message->getHeadSipReply().getContent() )

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: Duplicate and Forward the Reply
	// The message ha been prepared by client so it's ready to be sent back
	CREATEMESSAGE(m2netw, _message, SODE_TRNSCT_SV, SODE_NTWPOINT)
	//**************************************
	//Store the message to use it for retransmission
	//cannot store the _message since I can't change destination
	((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_2 = m2netw;
#ifdef CHECKDOA
		if ( Comap->getDoaState(_sm->getSL_CO_P()->call_oset,_message->getModulus()) == DOA_DELETED){
			DEBASSERT("CHECKDOA DOA_DELETED call_oset" <<_sm->getSL_CO_P()->call_oset)
		}
#endif
	((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_2->setLock(_sm->getSL_CO_P()->call_oset);
#ifdef USEFASTSEND
	_sm->getSL_CO_P()->call_oset->getTRNSPRT()->p_w(m2netw);
#else
	SingleAction sa_1 = SingleAction(m2netw);
	action->addSingleAction(sa_1);
#endif


	//**************************************
	//Action TIMER_S
	// bug timer s action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_SV));
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_SV));


	//**************************************
	//Local state 2
	DEBDEV("TRSNCT_INV_SV::act_provreply_to_a move to state 2","")
	_sm->State = 2;

	//**************************************
	//OverallState_SV OS_PROCEEDING
	DEBDEV("TRSNCT_INV_SV::act_provreply_to_a move OverallState_SV","OS_PROCEEDING")
	_sm->getSL_CO_P()->OverallState_SV = OS_PROCEEDING;

	return action;
}
//*****************************************************************
ACTION* act_resend_provreply_to_a(SM_V6* _sm, MESSAGE* _message) {

	DEBDEV("TRSNCT_INV_SV::act_resend_provreply_to_a",  _message )

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: Resend stored message
	//no need to duplicate since dest is already the ntw
	SingleAction sa_1 = SingleAction(((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_2);
	action->addSingleAction(sa_1);

	//**************************************
	//Action TIMER_S
	// bug timer s action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_SV));
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_SV));

	return action;

}
//*****************************************************************
// 200 OK from SM_CL
//*****************************************************************
bool pre_200ok_from_alo(SM_V6* _sm, MESSAGE* _message){

	DEBDEV("TRSNCT_INV_SV pre_200ok_from_alo",_message)

	if (_message->getReqRepType() == REPSUPP
		&& _message->getHeadSipReplyCode() == OK_200
		&& _message->getDestEntity() == SODE_TRNSCT_SV
		&& _message->getGenEntity() ==  SODE_ALOPOINT) {
			DEBDEV("SM_V5 pre_200ok_from_alo","true")
			return true;
	}
	else {
		DEBDEV("SM pre_200ok_from_alo","false")
		return false;
	}
}
//*****************************************************************
ACTION* act_200ok_fwdto_a(SM_V6* _sm, MESSAGE* _message) {

	DEBDEV("TRSNCT_INV_SV act_200ok_fwdto_a called",_message)

	//200ok from ALO
	//send to A
	//create alarm

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

#ifdef CHECKDOA
		if ( Comap->getDoaState(_sm->getSL_CO_P()->call_oset,_message->getModulus()) == DOA_DELETED){
			DEBASSERT("CHECKDOA DOA_DELETED call_oset" <<_sm->getSL_CO_P()->call_oset)
		}
#endif
	//**************************************
	//Action 1: Forward 200 OK to A
	if( ((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3 == MainMessage && _message->getReqRepType() == REPSUPP){
		((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3 = _message;
		DEBDEV("STORED_MESSAGE_1_3", ((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3)
		((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3->setLock(_sm->getSL_CO_P()->call_oset);
	}else {
		DEBASSERT("Don't know what to do")
	}
	//MLF2
	CREATEMESSAGE(m2netw, _message, SODE_TRNSCT_SV, SODE_NTWPOINT)
#ifdef USEFASTSEND
	_sm->getSL_CO_P()->call_oset->getTRNSPRT()->p_w(m2netw);
#else
	SingleAction sa_1 = SingleAction(m2netw);
	action->addSingleAction(sa_1);
#endif

	//**************************************
	//Action 2: copy the 200 OK and send to ALARM
	//This will be sent to A which will resend the ACK
	//TODO This leaks
	CREATEMESSAGE(ack_timer, ((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3, SODE_TRNSCT_SV, SODE_TRNSCT_SV)
	SysTime afterT;
	GETTIME(afterT);
	// T1 and not 2+T1
	lli firetime = ((lli) afterT.tv.tv_sec)*1000000+(lli)afterT.tv.tv_usec + TIMER_1;
	ack_timer->setFireTime(firetime);
	ack_timer->setTypeOfInternal(TYPE_OP);
	ack_timer->setTypeOfOperation(TYPE_OP_TIMER_ON);
	ack_timer->setOrderOfOperation("TIMER_G");
	//ack_timer->setLock(_sm->getSL_CO()->call_oset);
	SingleAction sa_2 = SingleAction(ack_timer);
	action->addSingleAction(sa_2);
	//**************************************
	//**************************************

	//**************************************
	//Action TIMER_S
	// bug timer s action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_SV));
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_SV));

	DEBDEV("SM act_200ok_to_a move to state 3","")
	_sm->State = 3;

	DEBDEV("SM act_200ok_to_a move OverallState_SV","OS_COMPLETED")
	_sm->getSL_CO_P()->OverallState_SV = OS_COMPLETED;

	return action;

	//OVERALL
	//TODO
	//cancel 64*T1 timer


}
ACTION* act_200ok_refwdto_a(SM_V6* _sm, MESSAGE* _message) {

	DEBDEV("TRSNCT_INV_SV act_200ok_refwdto_a called",_message)

	//200ok from ALARM
	//send to a
	//retrigger alarm

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: Forward stored 200 OK to A
	if( ((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3 == MainMessage){
		DEBASSERT("OK from alarm but STOREMESS_1_3 empty")
	}
	CREATEMESSAGE(mnetw, ((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3, SODE_TRNSCT_SV, SODE_NTWPOINT)

#ifdef CHECKDOA
		if ( Comap->getDoaState(_sm->getSL_CO_P()->call_oset,_message->getModulus()) == DOA_DELETED){
			DEBASSERT("CHECKDOA DOA_DELETED call_oset" <<_sm->getSL_CO_P()->call_oset)
		}
#endif

#ifdef USEFASTSEND
	_sm->getSL_CO_P()->call_oset->getTRNSPRT()->p_w(mnetw);
#else
	SingleAction sa_1 = SingleAction(mnetw);
	action->addSingleAction(sa_1);
#endif

	//**************************************
	//Action 2: copy the 200 OK and send to ALARM
	//This will be sent to A which will resend the ACK
	//TODO this message leaks
	CREATEMESSAGE(ack_timer, ((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3, SODE_TRNSCT_SV, SODE_TRNSCT_SV)
	ack_timer->setSourceMessage( ((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3 );
	SysTime afterT;
	GETTIME(afterT);
	// T1 and not 2+T1
	lli firetime = ((lli) afterT.tv.tv_sec)*1000000+(lli)afterT.tv.tv_usec + TIMER_1;
	ack_timer->setFireTime(firetime);
	ack_timer->setTypeOfInternal(TYPE_OP);
	ack_timer->setTypeOfOperation(TYPE_OP_TIMER_ON);
	ack_timer->setOrderOfOperation("TIMER_G");
	SingleAction sa_2 = SingleAction(ack_timer);
	action->addSingleAction(sa_2);

	((TRNSCT_SM_INVITE_SV*)_sm)->resend_200ok ++;

	//**************************************
	//**************************************


	//**************************************
	//Action TIMER_S
	// bug timer s action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_SV));
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_SV));


	return action;

	//OVERALL
	//TODO
	//cancel 64*T1 timer


}
ACTION* act_200ok_resendto_a(SM_V6* _sm, MESSAGE* _message) {

	DEBDEV("TRSNCT_INV_SV act_200ok_resendto_a called",_message)


	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//INVITE from A
	//resend stored 200ok
	//reset alarm

	//**************************************
	//Action 1: Forward stored 200 OK to A
	if( ((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3 == MainMessage){
		DEBASSERT("OK from alarm but STOREMESS_1_3 empty")
	}
	CREATEMESSAGE(mnetw, ((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3, SODE_TRNSCT_SV, SODE_NTWPOINT)

#ifdef CHECKDOA
		if ( Comap->getDoaState(_sm->getSL_CO_P()->call_oset,_message->getModulus()) == DOA_DELETED){
			DEBASSERT("CHECKDOA DOA_DELETED call_oset" <<_sm->getSL_CO_P()->call_oset)
		}
#endif

#ifdef USEFASTSEND
	_sm->getSL_CO_P()->call_oset->getTRNSPRT()->p_w(mnetw);
#else
	SingleAction sa_1 = SingleAction(mnetw);
	action->addSingleAction(sa_1);
#endif
	//**************************************
	//Action 2: copy the 200 OK and send to ALARM
	//This will be sent to A which will resend the ACK
	CREATEMESSAGE(ack_timer, ((TRNSCT_SM_INVITE_SV*)_sm)->STOREMESS_1_3, SODE_TRNSCT_SV, SODE_TRNSCT_SV)
	SysTime afterT;
	GETTIME(afterT);
	// T1 and not 2+T1
	lli firetime = ((lli) afterT.tv.tv_sec)*1000000+(lli)afterT.tv.tv_usec + TIMER_1;
	ack_timer->setFireTime(firetime);
	ack_timer->setTypeOfInternal(TYPE_OP);
	ack_timer->setTypeOfOperation(TYPE_OP_TIMER_ON);
	ack_timer->setOrderOfOperation("TIMER_G");
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
	//Action TIMER_S
	// bug timer s action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_SV));
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_SV));

	return action;

	//OVERALL
	//TODO
	//cancel 64*T1 timer


}

//*****************************************************************
// 200 OK from Alarm and resend < max and overall state not confirmed
//*****************************************************************
bool pre_200ok_from_alarm(SM_V6* _sm, MESSAGE* _message){

	DEBDEV("TRSNCT_INV_SV pre_200ok_from_alarm",_message)

	if (_message->getReqRepType() == REPSUPP
		&& _message->getHeadSipReplyCode() == OK_200
		&& _message->getDestEntity() == SODE_TRNSCT_SV
		&& _message->getGenEntity() ==  SODE_TRNSCT_SV
		&&  ((TRNSCT_SM_INVITE_SV*)_sm)->resend_200ok < MAX_INVITE_RESEND
		&& _sm->getSL_CO_P()->OverallState_SV != OS_CONFIRMED) {
			DEBDEV("SM_V5 pre_200ok_from_alarm","true")
			return true;
	}
	else {
		DEBDEV("SM pre_200ok_from_alarm","false")
		return false;
	}
}
//*****************************************************************
// 200 OK from Alarm and resend >= max and overall state not confirmed
//*****************************************************************
bool pre_200ok_from_alarm_maxreach(SM_V6* _sm, MESSAGE* _message){

	DEBDEV("TRSNCT_INV_SV pre_200ok_from_alarm_maxreach",_message)

	if (_message->getReqRepType() == REPSUPP
		&& _message->getHeadSipReplyCode() == OK_200
		&& _message->getDestEntity() == SODE_TRNSCT_SV
		&& _message->getGenEntity() ==  SODE_TRNSCT_SV
		&&  ((TRNSCT_SM_INVITE_SV*)_sm)->resend_200ok >= MAX_INVITE_RESEND
		&& _sm->getSL_CO_P()->OverallState_SV != OS_CONFIRMED) {
			DEBDEV("SM_V5 pre_200ok_from_alarm_maxreach","true")
			return true;
	}
	else {
		DEBDEV("pre_200ok_from_alarm_maxreach","false")
		return false;
	}
}
//*****************************************************************
ACTION* act_terminate_sv(SM_V6* _sm, MESSAGE* _message) {

	DEBDEV("TRSNCT_INV_SV act_terminate_sv ",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")


	CREATEMESSAGE(killd,_message, SODE_TRNSCT_SV,SODE_KILLDOA)
	SingleAction sa_1 = SingleAction(killd);
	action->addSingleAction(sa_1);

	//TODO CLEAR CALL ACK not arriving

	//**************************************
	//Action TIMER_S
	// bug timer s action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_SV));


	DEBDEV("SM act_terminate_sv move to state 5","")
	_sm->State = 5;

	DEBDEV("SM act_terminate_sv move OverallState_SV","OS_TERMINATED")
	_sm->getSL_CO_P()->OverallState_SV = OS_TERMINATED;

	return (action);
}
//*****************************************************************
// 200 OK from Alarm overall state confirmed
//*****************************************************************
bool pre_200ok_from_alarm_confirm(SM_V6* _sm, MESSAGE* _message){

	DEBDEV("TRSNCT_INV_SV pre_200ok_from_alarm_confirm",_message)

	if (_message->getReqRepType() == REPSUPP
		&& _message->getHeadSipReplyCode() == OK_200
		&& _message->getDestEntity() == SODE_TRNSCT_SV
		&& _message->getGenEntity() ==  SODE_TRNSCT_SV
		&&  _sm->getSL_CO_P()->OverallState_SV == OS_CONFIRMED) {
			DEBDEV("SM_V5 pre_200ok_from_alarm_confirm","true")
			return true;
	}
	else {
		DEBDEV("pre_200ok_from_alarm_confirm","false")
		return false;
	}
}
//*****************************************************************
ACTION* act_null_sv(SM_V6* _sm, MESSAGE* _message) {

	DEBDEV("TRSNCT_INV_SV act_null_sv",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//TODO CLEAR CALL ACK not arriving

	//**************************************
	//Action TIMER_S
	// bug timer s action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_SV));
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_SV));

	return (action);

}
bool pre_N_99_inv_sv(SM_V6* _sm, MESSAGE* _message){

	DEBDEV("TRSNCT_INV_SV pre_N_99_inv_sv",_message)

	if (_message->getReqRepType() == REPSUPP
		&& _message->getHeadSipReplyCode() == SU_503
		&& _message->getDestEntity() == SODE_TRNSCT_SV
		&& _message->getGenEntity() ==  SODE_TRNSCT_SV
		&& _sm->getSL_CO_P()->OverallState_SV != OS_CONFIRMED) {
			DEBDEV("SM_V5 pre_N_99_inv_sv","true")
			return true;
	}
	else {
		DEBDEV("SM pre_N_99_inv_sv","false")
		return false;
	}
}
ACTION* act_N_99_inv_sv(SM_V6* _sm, MESSAGE* _message) {

	//Send temination signal to client machine
	DEBDEV("TRSNCT_INV_SV act_N_99_inv_sv",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")


	CREATEMESSAGE(killd,_message, SODE_TRNSCT_SV,SODE_KILLDOA)
	SingleAction sa_1 = SingleAction(killd);
	action->addSingleAction(sa_1);

	//**************************************
	//Action TIMER_S
	// bug timer s action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_SV));


	DEBDEV("SM act_N_99_inv_sv move to state 99","")
	_sm->State = 99;

	DEBDEV("SM act_N_99_inv_sv move OverallState_SV","OS_TERMINATED")
	_sm->getSL_CO_P()->OverallState_SV = OS_TERMINATED;

	//TODO finish here
	return(action);

}
//*****************************************************************
// TIMER_S
//*****************************************************************
bool pre_timer_s_sv(SM_V6* _sm, MESSAGE* _message){

	DEBDEV("TRNSCT_INV_SV pre_timer_s",_message)
	if (_message->getTypeOfInternal() == TYPE_OP
			&& _message->getOrderOfOperation().compare("TIMER_S") == 0
			&& _message->getDestEntity() == SODE_TRNSCT_SV
			&& _message->getGenEntity() ==  SODE_TRNSCT_SV
			&& _sm->getSL_CO_P()->OverallState_SV != OS_CONFIRMED){
		DEBDEV("TRNSCT_INV_SV pre_timer_s","true")
		return true;
	}
	else {
		DEBDEV("TRNSCT_INV_SV pre_timer_s","false")
		return false;
	}
}
ACTION* act_timer_s_sv(SM_V6* _sm, MESSAGE* _message) {

	//Send temination signal to client machine
	DEBDEV("TRSNCT_INV_SV act_timer_s",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	CREATEMESSAGE(killd,_message, SODE_TRNSCT_SV,SODE_KILLDOA)
	SingleAction sa_1 = SingleAction(killd);
	action->addSingleAction(sa_1);

	DEBDEV("SM act_timer_s move to state 99","")
	_sm->State = 99;

	DEBDEV("SM act_timer_s move OverallState_SV","OS_TERMINATED")
	_sm->getSL_CO_P()->OverallState_SV = OS_TERMINATED;
	DEBDEV("SM act_timer_s killed by timer s",_sm->getSL_CO_P())

	return(action);

}
//**********************************************************************************
TRNSCT_SM_INVITE_SV::TRNSCT_SM_INVITE_SV(int _requestType, MESSAGE* _matrixMess, ENGINE* _sl_cc, SL_CO* _sl_co):
		TRNSCT_SM(_requestType, _matrixMess, _matrixMess, _sl_cc, _sl_co){

	resend_200ok = 0;

	STOREMESS_1_1 = MainMessage;
	STOREMESS_1_2 = MainMessage;
	STOREMESS_1_3 = MainMessage;
}
ACTION* TRNSCT_SM_INVITE_SV::event(MESSAGE* _message){


	if (State == 0){
		//insert_move(0,&PA_INV_0_1SV);
		if (pre_invite_from_a((SM_V6*)this, _message)){
			return act_invite_to_alo((SM_V6*)this, _message);
		}
	}
	else if (State == 1){
		//insert_move(1,&PA_INV_1_1SV);
		if (pre_invite_from_a((SM_V6*)this, _message)){
			//return act_resend_try_to_a((SM_V6*)this, _message);
			return act_send_servunav_to_a((SM_V6*)this, _message);
		}
		//insert_move(1,&PA_INV_1_2SV);
		else if (pre_provrep_from_cl((SM_V6*)this, _message)){
			return act_provreply_to_a((SM_V6*)this, _message);
		}
		//insert_move(1,&PA_INV_1_3SV);
		else if (pre_200ok_from_alo((SM_V6*)this, _message)){
			return act_200ok_fwdto_a((SM_V6*)this, _message);
		}
		//insert_move(1,&PA_INV_S_SV);
		else if (pre_timer_s_sv((SM_V6*)this, _message)){
			return act_timer_s_sv((SM_V6*)this, _message);
		}

	}
	else if (State == 2){
		//insert_move(2,&PA_INV_2_2SV);
		if (pre_invite_from_a((SM_V6*)this, _message)){
			//return act_resend_provreply_to_a((SM_V6*)this, _message);
			return act_send_servunav_to_a((SM_V6*)this, _message);
		}
		//insert_move(2,&PA_INV_3_3eSV);
		else if (pre_provrep_from_cl((SM_V6*)this, _message)){
			return act_null_sv((SM_V6*)this, _message);
		}
		//insert_move(2,&PA_INV_1_3SV);
		else if (pre_200ok_from_alo((SM_V6*)this, _message)){
			return act_200ok_fwdto_a((SM_V6*)this, _message);
		}
		//insert_move(2,&PA_INV_S_SV);
		else if (pre_timer_s_sv((SM_V6*)this, _message)){
			return act_timer_s_sv((SM_V6*)this, _message);
		}
	}else if (State == 3){
		//insert_move(3,&PA_INV_3_3aSV);
		if (pre_invite_from_a((SM_V6*)this, _message)){
			return act_200ok_resendto_a((SM_V6*)this, _message);
		}
		//insert_move(3,&PA_INV_3_3bSV);
		else if (pre_200ok_from_alarm((SM_V6*)this, _message)){
			return act_200ok_refwdto_a((SM_V6*)this, _message);
		}
		//insert_move(3,&PA_INV_3_5SV);
		else if (pre_200ok_from_alarm_maxreach((SM_V6*)this, _message)){
			return act_terminate_sv((SM_V6*)this, _message);
		}
		//insert_move(3,&PA_INV_3_3dSV);
		else if (pre_200ok_from_alarm_confirm((SM_V6*)this, _message)){
			return act_null_sv((SM_V6*)this, _message);
		}
		//insert_move(3,&PA_INV_3_3eSV);
		else if (pre_provrep_from_cl((SM_V6*)this, _message)){
			return act_null_sv((SM_V6*)this, _message);
		}
		//insert_move(3,&PA_INV_S_SV);
		else if (pre_timer_s_sv((SM_V6*)this, _message)){
			return act_timer_s_sv((SM_V6*)this, _message);
		}
	}
	return 0x0;
//	//First Invite_A
//	//Try_A is sent
//	//Invite_A is sent to ALO
//	PA_INV_0_1SV.predicate = &pre_invite_from_a;
//	PA_INV_0_1SV.action = &act_invite_to_alo;
//
//	//Second Invite_A
//	//Try_a lost
//	PA_INV_1_1SV.predicate = &pre_invite_from_a;
//	PA_INV_1_1SV.action = &act_resend_try_to_a;
//
//	PA_INV_1_2SV.predicate = &pre_provrep_from_cl;
//	PA_INV_1_2SV.action = &act_provreply_to_a;
//
//	PA_INV_2_2SV.predicate = &pre_invite_from_a;
//	PA_INV_2_2SV.action = &act_resend_provreply_to_a;
//
//	PA_INV_1_3SV.predicate = &pre_200ok_from_alo;
//	PA_INV_1_3SV.action = &act_200ok_fwdto_a;
//
//	PA_INV_3_3aSV.predicate = &pre_invite_from_a;
//	PA_INV_3_3aSV.action = &act_200ok_resendto_a;
//
//	PA_INV_3_3bSV.predicate = &pre_200ok_from_alarm;
//	PA_INV_3_3bSV.action = &act_200ok_refwdto_a;
//
//	PA_INV_3_5SV.predicate = &pre_200ok_from_alarm_maxreach;
//	PA_INV_3_5SV.action = &act_terminate_sv;
//
//	PA_INV_3_3dSV.predicate = &pre_200ok_from_alarm_confirm;
//	PA_INV_3_3dSV.action = &act_null_sv;
//
//	//Why some 180 are not sent?
//	//Probably beause tehy arrive when the sm is already in state 2
//	PA_INV_3_3eSV.predicate = &pre_provrep_from_cl;
//	PA_INV_3_3eSV.action = &act_null_sv;
//
//	PA_INV_S_SV.predicate = &pre_timer_s_sv;
//	PA_INV_S_SV.action = &act_timer_s_sv;

}
//*****************************************************************
// client transaction
//*****************************************************************
// INVITE B from ALO
//*****************************************************************
bool pre_invite_from_sv(SM_V6* _sm, MESSAGE* _message){

	DEBDEV("TRNSCT_INV_CL pre_invite_from_sv",_message)
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequestCode() == INVITE_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_ALOPOINT) {
		DEBDEV("TRNSCT_INV_CL pre_invite_from_sv","true")
		return true;
	}
	else {
		DEBDEV("TRNSCT_INV_CL pre_invite_from_sv","false")
		return false;
	}
}
//*****************************************************************
// INVITE B from ALARM
//*****************************************************************
//*****************************************************************
// careful pre_1_1_inv_cl and pre_1_99_inv_cl are different
// This invite could be deleted, currently is inserted into insertLockedMessage table
bool pre_invite_from_alarm(SM_V6* _sm, MESSAGE* _message){

	DEBDEV("TRNSCT_INV_CL pre_invite_from_alarm",_message)
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequestCode() == INVITE_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_TRNSCT_CL
			&& ((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite < MAX_INVITE_RESEND) {
		DEBDEV("TRNSCT_INV_CL pre_invite_from_alarm","true")
		return true;
	}
	else {
		DEBDEV("TRNSCT_INV_CL pre_invite_from_alarm","false")
		return false;
	}
}
//*****************************************************************
//act_1_1_inv_cl
ACTION* act_invite_to_b(SM_V6* _sm, MESSAGE* _message) {

	DEBDEV("TRNSCT_INV_CL act_invite_to_b",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: send message to NTW
	CREATEMESSAGE(m2netw, _message, SODE_TRNSCT_CL, SODE_NTWPOINT)

#ifdef CHECKDOA
		if ( Comap->getDoaState(_sm->getSL_CO_P()->call_oset,_message->getModulus()) == DOA_DELETED){
			DEBASSERT("CHECKDOA DOA_DELETED call_oset" <<_sm->getSL_CO_P()->call_oset)
		}
#endif

#ifdef USEFASTSEND
	_sm->getSL_CO_P()->call_oset->getTRNSPRT()->p_w(m2netw);
#else
	SingleAction sa_1 = SingleAction(m2netw);
	action->addSingleAction(sa_1);
#endif

	//**************************************
	//Action 2: send message to ALARM
	CREATEMESSAGE(__timedmessage, _message, SODE_TRNSCT_CL, SODE_TRNSCT_CL)
	__timedmessage->setSourceMessage(((TRNSCT_SM*)_sm)->getA_Matrix());
	SysTime afterT;
	GETTIME(afterT);
	lli firetime = ((lli) afterT.tv.tv_sec)*1000000+(lli)afterT.tv.tv_usec + TIMER_1 * pow(2,((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite);
	DEBDEV("TRNSCT_INV_CL act_invite_to_b creating alarm ", pow(2,((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite) << " " << firetime)
	__timedmessage->setFireTime(firetime);
	__timedmessage->setTypeOfInternal(TYPE_OP);
	__timedmessage->setTypeOfOperation(TYPE_OP_TIMER_ON);
	__timedmessage->setOrderOfOperation("TIMER_A");
	SingleAction sa_2 = SingleAction(__timedmessage);
	action->addSingleAction(sa_2);

	//**************************************
	//Action TIMER_S
	// bug timer s action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_CL));
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_CL));


	((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite++;

	DEBDEV("TRNSCT_SM_INVITE_CL act_invite_to_b resend value", ((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite)

	DEBDEV("SM act_invite_to_b move to state","1")
	_sm->State = 1;
	DEBDEV("SM act_invite_to_b move OverallState_CL","OS_CALLING")
	_sm->getSL_CO_P()->OverallState_CL = OS_CALLING;

	return action;
}
//*****************************************************************
// INVITE B from ALARM max resend reached
//*****************************************************************
// pre_1_99_inv_cl
bool pre_invite_from_alarm_maxreach(SM_V6* _sm, MESSAGE* _message){

	DEBDEV("TRNSCT_INV_CL pre_invite_from_alarm_maxreach",_message)
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequestCode() == INVITE_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_TRNSCT_CL
			&& ((TRNSCT_SM_INVITE_CL*)_sm)->resend_invite >= MAX_INVITE_RESEND) {
		DEBDEV("TRNSCT_INV_CL pre_invite_from_alarm_maxreach","true")
		return true;
	}
	else {
		DEBDEV("TRNSCT_INV_CL pre_invite_from_alarm_maxreach","false")
		return false;
	}
}
//act_1_99_inv_cl
ACTION* act_terminate_cl(SM_V6* _sm, MESSAGE* _message) {

	DEBDEV("TRNSCT_INV_CL act_terminate_cl *** incomplete *** ",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	CREATEMESSAGE(killd,_message, SODE_TRNSCT_CL,SODE_KILLDOA)
	SingleAction sa_1 = SingleAction(killd);
	action->addSingleAction(sa_1);


	//**************************************
	//Action TIMER_S
	// bug timer s action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_CL));


	DEBDEV("SM act_terminate_cl move OverallState_CL to","OS_TERMINATED")
	_sm->getSL_CO_P()->OverallState_CL = OS_TERMINATED;
	DEBDEV("SM act_terminate_cl move to",99)
	_sm->State = 99;

	return (action);

}
//*****************************************************************
// 100 Try B
//*****************************************************************
//pre_1_2_inv_cl
bool pre_try_from_b(SM_V6* _sm, MESSAGE* _message){

	DEBDEV("TRNSCT_INV_CL pre_try_from_b",_message)

	if (_message->getReqRepType() == REPSUPP
		&&_message->getHeadSipReplyCode() == TRYING_100
		&& _message->getDestEntity() == SODE_TRNSCT_CL
		&& _message->getGenEntity() ==  SODE_NTWPOINT) {
			DEBDEV("TRNSCT_INV_CL pre_try_from_b","true")
			return true;
		}
		else {
			DEBDEV("TRNSCT_INV_CL pre_try_from_b","false")
			return false;
		}
}
//*****************************************************************
//act_1_2_inv_cl
ACTION* act_clear_invite_alarm(SM_V6* _sm, MESSAGE* _message) {

	DEBDEV("TRNSCT_INV_CL act_clear_invite_alarm",_message)

	//DEBDEV("TRNSCT_INV_CL act_clear_invite_alarm",_message->getHeadSipReply().getReply().getCode())

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: clear INVITE B alarm
	CREATEMESSAGE(clearalm, _message, SODE_TRNSCT_CL, SODE_TRNSCT_CL)
	clearalm->setTypeOfInternal(TYPE_OP);
	clearalm->setTypeOfOperation(TYPE_OP_TIMER_OFF);
	clearalm->setOrderOfOperation("TIMER_A");
	SingleAction sa_1 = SingleAction(clearalm);
	action->addSingleAction(sa_1);

	//**************************************
	//Action TIMER_S
	// bug timer s action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_CL));
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_CL));


	DEBDEV("SM act_clear_invite_alarm move to state","2")
	_sm->State = 2;
	DEBDEV("SM act_clear_invite_alarm move OverallState_CL to ","OS_PROCEEDING")
	_sm->getSL_CO_P()->OverallState_CL = OS_PROCEEDING;

	return action;
}
//*****************************************************************
// 101 DE B - 180 RING B
//*****************************************************************
//pre_1_3_inv_cl
bool pre_provrep_from_b(SM_V6* _sm, MESSAGE* _message){

	DEBDEV("SM_CL pre_provrep_from",_message)

	if (_message->getReqRepType() == REPSUPP
		&& (_message->getHeadSipReplyCode() == DIALOGE_101
				|| _message->getHeadSipReplyCode() == RINGING_180)
		&& _message->getDestEntity() == SODE_TRNSCT_CL
		&& _message->getGenEntity() ==  SODE_NTWPOINT) {
			DEBDEV("SM_CL pre_provrep_from","true")
			return true;
		}
		else {
			DEBDEV("SM_CL pre_provrep_from","false")
			return false;
		}
}
//*****************************************************************
//act_1_3_inv_cl
ACTION* act_provrep_to_sv(SM_V6* _sm, MESSAGE* _message) {

	DEBDEV("SM_CL act_provrep_to_sv",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: create reply using A_Matrix
	// the message contains the to tag that we must save
	// or store it in valo during 200ok
	MESSAGE* __message = ((TRNSCT_SM*)_sm)->getA_Matrix();
	DEBDEV("MESSAGE GENERATOR", __message)
	CREATEMESSAGE(reply_x, __message, SODE_TRNSCT_CL, SODE_TRNSCT_SV)
	SipUtil.genASideReplyFromBReply(_message, __message, reply_x,_sm->getSL_CC()->getSUDP());
	reply_x->purgeSDP();
	reply_x->compileMessage();
	//DEBDEV("CONTACT", reply_x->getHeadContact()->getContent())
	SingleAction sa_1 = SingleAction(reply_x);

	action->addSingleAction(sa_1);



	//**************************************
	//Action 2: clear INVITE B alarm
	CREATEMESSAGE(clearalm, _message, SODE_TRNSCT_CL, SODE_TRNSCT_CL)
	clearalm->setTypeOfInternal(TYPE_OP);
	clearalm->setTypeOfOperation(TYPE_OP_TIMER_OFF);
	clearalm->setOrderOfOperation("TIMER_A");
	SingleAction sa_2 = SingleAction(clearalm);
	action->addSingleAction(sa_2);

	//**************************************
	//Action TIMER_S
	// bug timer s action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_CL));
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_CL));


	DEBDEV("SM act_provrep_to_sv move to state","3")
	_sm->State = 3;

	DEBDEV("SM act_provrep_to_sv move OverallState_CL to","OS_PROCEEDING")
	_sm->getSL_CO_P()->OverallState_CL = OS_PROCEEDING;


	return action;


}
//*****************************************************************
// 200 OK B
//*****************************************************************
//pre_1_4_inv_cl
bool pre_200ok_from_b(SM_V6* _sm, MESSAGE* _message){

	DEBDEV("SM pre_200ok_from_b",_message)

	if (_message->getReqRepType() == REPSUPP
		&&_message->getHeadSipReplyCode() == OK_200
		&& _message->getDestEntity() == SODE_TRNSCT_CL
		&& _message->getGenEntity() ==  SODE_NTWPOINT) {
			DEBDEV("SM pre_200ok_from_b","true")
			return true;
		}
		else {
			DEBDEV("SM pre_200ok_from_b","false")
			return false;
		}
}
//*****************************************************************
//act_1_4_inv_cl
ACTION* act_200ok_inv_to_alo(SM_V6* _sm, MESSAGE* _message) {

	DEBDEV("SM act_200ok_inv_to_alo",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: Clear alarm here in case the b did not send any trying
	CREATEMESSAGE(___message, _message, SODE_TRNSCT_CL, SODE_TRNSCT_CL)
	___message->setTypeOfInternal(TYPE_OP);
	___message->setTypeOfOperation(TYPE_OP_TIMER_OFF);
	___message->setOrderOfOperation("TIMER_A");
	SingleAction sa_1 = SingleAction(___message);
	action->addSingleAction(sa_1);

	//**************************************
	//Action 2: 200 OK B to ALO
	//Alo will delete it
	CREATEMESSAGE(_toalo, _message, SODE_TRNSCT_CL, SODE_ALOPOINT)
	_toalo->setSourceMessage(((TRNSCT_SM*)_sm)->getMatrixMessage());
	SingleAction sa_2 = SingleAction(_toalo);
	action->addSingleAction(sa_2);

	//**************************************
	//Action TIMER_S
	// bug timer s action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_CL));
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_CL));


	DEBDEV("SM act_200ok_inv_to_alo move to state","4")
	_sm->State = 4;
	DEBDEV("SM act_200ok_inv_to_alo move OverallState_CL to","OS_PROCEEDING")
	_sm->getSL_CO_P()->OverallState_CL = OS_PROCEEDING;


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
bool pre_200ok_from_b_proceeding(SM_V6* _sm, MESSAGE* _message){

	DEBDEV("SM_CL pre_200ok_from_b_proceeding",_message)

	if (_message->getReqRepType() == REPSUPP
		&&_message->getHeadSipReplyCode() == OK_200
		&& _message->getDestEntity() == SODE_TRNSCT_CL
		&& _message->getGenEntity() ==  SODE_NTWPOINT
		&& _sm->getSL_CO_P()->OverallState_CL == OS_PROCEEDING){
			DEBDEV("SM pre_200ok_from_b_proceeding","true")
			return true;
		}
		else {
			DEBDEV("SM pre_200ok_from_b_proceeding","false")
			return false;
		}
}
////*****************************************************************
//
//ACTION* act_null(SM* _sm, MESSAGE* _message) {
//
//
//	DEBDEV("SM act_4_4a_inv_cl",_message)
//	return 0x0;
//
//}

//*****************************************************************
// 200 OK B in OS_COMPLETED
// Need to resend the ACK
//*****************************************************************
//pre_4_4b_inv_cl
bool pre_200ok_from_b_completed(SM_V6* _sm, MESSAGE* _message){

	DEBDEV("SM_CL pre_200ok_from_b_completed",_message)

	if (_message->getReqRepType() == REPSUPP
		&&_message->getHeadSipReplyCode() == OK_200
		&& _message->getDestEntity() == SODE_TRNSCT_CL
		&& _message->getGenEntity() ==  SODE_NTWPOINT
		&& _sm->getSL_CO_P()->OverallState_CL == OS_COMPLETED){
			DEBDEV("SM pre_200ok_from_b_completed","true")
			return true;
		}
		else {
			DEBDEV("SM pre_200ok_from_b_completed","false")
			return false;
		}
}
//act_4_4b_inv_cl
ACTION* act_resend_ack(SM_V6* _sm, MESSAGE* _message) {


	DEBDEV("SM act_resend_ack",_message)
	//Need to resend ACK

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: take the 200OK change to send it to ack_cl
//	CREATEMESSAGE(reack, _message, SODE_TRNSCT_CL, SODE_TRNSCT_CL)
//	reack->setTypeOfInternal(TYPE_OP);
//	reack->setTypeOfOperation(TYPE_OP_SMCOMMAND);
//	//TODO need the branch!!!
//	reack->setGenericHeader("CSeq:","1 ACK");
//	reack->compileMessage();
//	reack->setSourceHeadCallId(((TRNSCT_SM*)_sm)->getMatrixMessage()->getHeadCallId());
//	reack->setSourceModulus(((TRNSCT_SM*)_sm)->getMatrixMessage()->getModulus());
//	SingleAction sa_1 = SingleAction(reack);
//	action->addSingleAction(sa_1);

	//Faster ACk resend
	//Will not resend an event to access ACK_CL but it will dip it from it directly
	//Look for ACKTOB and send it

#ifdef CHECKDOA
		if ( Comap->getDoaState(_sm->getSL_CO_P()->call_oset,_message->getModulus()) == DOA_DELETED){
			DEBASSERT("CHECKDOA DOA_DELETED call_oset" <<_sm->getSL_CO_P()->call_oset)
		}
#endif
	TRNSCT_SM_ACK_CL* ackcli = (TRNSCT_SM_ACK_CL*)_sm->getSL_CO_P()->call_oset->lastTRNSCT_SM_ACK_CL;

    if (ackcli == 0x0){
    	DEBY
    	return action;
    }
    else {
    	MESSAGE* temp = ackcli->ACKTOB;
    	DEBOUT("IT WORKS HERE",temp)
        if(temp ==MainMessage){
        	DEBASSERT("totally wrong")
        }
    	SingleAction sa_1 = SingleAction(temp);
    	action->addSingleAction(sa_1);

    }

	//**************************************
	//Action TIMER_S
	// bug timer s action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_CL));

	//DEBASSERT("ACTION* act_4_4b_inv_cl need to send this to ACK-CL")
	return action;

}
//*****************************************************************
ACTION* act_null_cl(SM_V6* _sm, MESSAGE* _message) {

	DEBDEV("TRSNCT_INV_CL act_null_cl",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//TODO CLEAR CALL ACK not arriving

	//**************************************
	//Action TIMER_S
	// bug timer s action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_CL));
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_CL));

	return (action);

}


//*****************************************************************
// TIMER_S
//*****************************************************************
bool pre_timer_s_cl(SM_V6* _sm, MESSAGE* _message){

	DEBDEV("TRNSCT_INV_CL pre_timer_s_cl",_message)
	if (_message->getTypeOfInternal() == TYPE_OP
			&& _message->getOrderOfOperation().compare("TIMER_S") == 0
			&& _message->getDestEntity() == SODE_TRNSCT_SV
			&& _message->getGenEntity() ==  SODE_TRNSCT_SV
			&& _sm->getSL_CO_P()->OverallState_CL != OS_COMPLETED){
		DEBDEV("TRNSCT_INV_CL pre_timer_s_cl","true")
		return true;
	}
	else {
		DEBDEV("TRNSCT_INV_CL pre_timer_s_cl","false")
		return false;
	}
}
ACTION* act_timer_s_cl(SM_V6* _sm, MESSAGE* _message) {

	//Send temination signal to client machine
	DEBDEV("TRSNCT_INV_CL act_timer_s_cl",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")


	CREATEMESSAGE(killd,_message, SODE_TRNSCT_CL,SODE_KILLDOA)
	SingleAction sa_1 = SingleAction(killd);
	action->addSingleAction(sa_1);

	DEBDEV("SM act_timer_s move to state 99","")
	_sm->State = 99;

	DEBDEV("SM act_timer_s move OverallState_CL","OS_TERMINATED")
	_sm->getSL_CO_P()->OverallState_CL = OS_TERMINATED;
	DEBDEV("SM act_timer_s killed by timer s",_sm->getSL_CO_P())

	return(action);

}

//**********************************************************************************
TRNSCT_SM_INVITE_CL::TRNSCT_SM_INVITE_CL(int _requestType, MESSAGE* _matrixMess, MESSAGE* _A_Matrix, ENGINE* _sl_cc, SL_CO* _sl_co):
		TRNSCT_SM(_requestType, _matrixMess, _A_Matrix, _sl_cc, _sl_co){

	resend_invite = 1;
}
ACTION* TRNSCT_SM_INVITE_CL::event(MESSAGE* _message){

	if(State == 0){
		//insert_move(0,&PA_INV_0_1CL);
		if (pre_invite_from_sv((SM_V6*)this, _message)){
			return act_invite_to_b((SM_V6*)this, _message);
		}
	}else if (State == 1){
		//insert_move(1,&PA_INV_1_1CL);
		if (pre_invite_from_sv((SM_V6*)this, _message)){
			return act_invite_to_b((SM_V6*)this, _message);
		}
		//insert_move(1,&PA_INV_1_2CL);
		else if (pre_try_from_b((SM_V6*)this, _message)){
			return act_clear_invite_alarm((SM_V6*)this, _message);
		}
		//insert_move(1,&PA_INV_1_3CL);
		else if (pre_provrep_from_b((SM_V6*)this, _message)){
			return act_provrep_to_sv((SM_V6*)this, _message);
		}
		//insert_move(1,&PA_INV_S_CL);
		else if (pre_timer_s_cl((SM_V6*)this, _message)){
			return act_timer_s_cl((SM_V6*)this, _message);
		}
		//insert_move(1,&PA_INV_1_99CL);
		else if (pre_invite_from_alarm_maxreach((SM_V6*)this, _message)){
			return act_terminate_cl((SM_V6*)this, _message);
		}
		//insert_move(1,&PA_INV_1_4CL);
		else if (pre_200ok_from_b((SM_V6*)this, _message)){
			return act_200ok_inv_to_alo((SM_V6*)this, _message);
		}

	} else if (State == 2){
		//insert_move(2,&PA_INV_1_3CL);
		if (pre_provrep_from_b((SM_V6*)this, _message)){
			return act_provrep_to_sv((SM_V6*)this, _message);
		}
		//insert_move(2,&PA_INV_S_CL);
		else if (pre_timer_s_cl((SM_V6*)this, _message)){
			return act_timer_s_cl((SM_V6*)this, _message);
		}
		//insert_move(2,&PA_INV_1_4CL);
		else if (pre_200ok_from_b((SM_V6*)this, _message)){
			return act_200ok_inv_to_alo((SM_V6*)this, _message);
		}
	} else if (State == 3){
		//insert_move(3,&PA_INV_1_4CL);
		if (pre_200ok_from_b((SM_V6*)this, _message)){
			return act_200ok_inv_to_alo((SM_V6*)this, _message);
		}
		//insert_move(3,&PA_INV_S_CL);
		else if (pre_timer_s_cl((SM_V6*)this, _message)){
			return act_timer_s_cl((SM_V6*)this, _message);
		}

	} else if (State == 4){
		//insert_move(4,&PA_INV_4_4bCL);
		if (pre_200ok_from_b_completed((SM_V6*)this, _message)){
			return act_resend_ack((SM_V6*)this, _message);
		}
		//new resend ack also in proceeding
//		if (pre_200ok_from_b((SM_V6*)this, _message)){
//			return act_resend_ack((SM_V6*)this, _message);
//		}
		//insert_move(4,&PA_INV_4_4aCL);
		else if (pre_200ok_from_b_proceeding((SM_V6*)this, _message)){
			return act_null_cl((SM_V6*)this, _message);
		}
		//insert_move(4,&PA_INV_4_4cCL);
		else if (pre_provrep_from_b((SM_V6*)this, _message)){
			return act_null_cl((SM_V6*)this, _message);
		}

	}
	return 0x0;

	//from 1 for 2 or 3
//	PA_INV_1_4CL.predicate = &pre_200ok_from_b;
//	PA_INV_1_4CL.action = &act_200ok_inv_to_alo;
//
//	PA_INV_0_1CL.predicate = &pre_invite_from_sv;
//	PA_INV_0_1CL.action = &act_invite_to_b;
//
//	PA_INV_1_1CL.predicate = &pre_invite_from_alarm;
//	PA_INV_1_1CL.action = &act_invite_to_b;
//
//	PA_INV_1_99CL.predicate = &pre_invite_from_alarm_maxreach;
//	PA_INV_1_99CL.action = &act_terminate_cl;
//
//	PA_INV_1_2CL.predicate = &pre_try_from_b;
//	PA_INV_1_2CL.action = &act_clear_invite_alarm;
//
//	//from 1 or from 2
//	PA_INV_1_3CL.predicate = &pre_provrep_from_b;
//	PA_INV_1_3CL.action = &act_provrep_to_sv;
//
//
//	PA_INV_4_4aCL.predicate = &pre_200ok_from_b_proceeding;
//	PA_INV_4_4aCL.action = &act_null_cl;
//
//	PA_INV_4_4bCL.predicate = &pre_200ok_from_b_completed;
//	PA_INV_4_4bCL.action = &act_resend_ack;
//
//	//Not all 180 are sent back
//	PA_INV_4_4cCL.predicate = &pre_provrep_from_b;
//	PA_INV_4_4cCL.action = &act_null_cl;
//
//	PA_INV_S_CL.predicate = &pre_timer_s_cl;
//	PA_INV_S_CL.action = &act_timer_s_cl;




}
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//pre_0_1_ack_sv
bool pre_ack_from_a(SM_V6* _sm, MESSAGE* _message){

	DEBDEV("SM pre_ack_from_a called",_message)
	if (_message->getReqRepType() == REQSUPP
			&& (_message->getHeadSipRequestCode() == ACK_REQUEST)
			&& _message->getDestEntity() == SODE_TRNSCT_SV
			&& _message->getGenEntity() ==  SODE_NTWPOINT) {
		DEBDEV("SM pre_ack_from_a","true")
		return true;
	}
	else {
		DEBDEV("SM pre_ack_from_a","false")
		return false;
	}
}
//act_0_1_ack_sv
ACTION* act_ack_to_alo(SM_V6* _sm, MESSAGE* _message) {

	DEBDEV("SM act_ack_to_alo called",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: ack is to ALO
	CREATEMESSAGE(ack,_message, SODE_TRNSCT_SV,SODE_ALOPOINT)
	ack->setType_trnsct(TYPE_TRNSCT);
	SingleAction sa_1 = SingleAction(ack);
	action->addSingleAction(sa_1);

	DEBDEV("SM act_ack_to_alo move to state 1","")
	_sm->State = 1;
	DEBDEV("SM act_ack_to_alo move OverallState_SV to 1","OS_CONFIRMED")
	_sm->getSL_CO_P()->OverallState_SV = OS_CONFIRMED;

	//TODO
	// clear 200 ok alarm sent by invite sv
	//move to confirmed

	return action;

}
//act_null
//ACTION* act_1_1_ack_sv(SM* _sm, MESSAGE* _message) {
//
//	DEBDEV("SM act_1_1_ack_sv called",_message)
//
//	//**************************************
//	//Action 1: ACK has been resent by A : ignore
//
//	return 0x0;
//}

//**********************************************************************************
TRNSCT_SM_ACK_SV::TRNSCT_SM_ACK_SV(int _requestType, MESSAGE* _matrixMess, ENGINE* _sl_cc, SL_CO* _sl_co):
		TRNSCT_SM(_requestType, _matrixMess, _matrixMess, _sl_cc, _sl_co){
}
ACTION* TRNSCT_SM_ACK_SV::event(MESSAGE* _message){

	if (State == 0){
		//insert_move(0,&PA_ACK_0_1SV);
		if (pre_ack_from_a((SM_V6*)this, _message)){
			return act_ack_to_alo((SM_V6*)this, _message);
		}
	}else if (State == 1){
		//insert_move(1,&PA_ACK_1_1SV);
		if (pre_ack_from_a((SM_V6*)this, _message)){
			return act_null_sv((SM_V6*)this, _message);
		}
	}
	return 0x0;
//	PA_ACK_0_1SV.predicate = &pre_ack_from_a;
//	PA_ACK_0_1SV.action = &act_ack_to_alo;
//
//	PA_ACK_1_1SV.predicate = &pre_ack_from_a;
//	PA_ACK_1_1SV.action = &act_null_sv;
}
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//pre_0_1_ack_cl
bool pre_ack_from_alo(SM_V6* _sm, MESSAGE* _message){

	DEBDEV("SM pre_ack_from_alo",_message)
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequestCode() == ACK_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_ALOPOINT) {
		DEBDEV("SM pre_ack_from_alo","true")
		return true;
	}
	else {
		DEBDEV("SM pre_ack_from_alo","false")
		return false;
	}
}
//act_0_1_ack_cl
ACTION* act_ack_to_b(SM_V6* _sm, MESSAGE* _message) {

	DEBDEV("SM act_ack_to_b",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	// to test put this:
	//_message->setDestEntity(SODE_NOPOINT);

	DEBMESSAGE("FIRST ACK B", _message)

	//**************************************
	//Action 1: ack is from ALO
	CREATEMESSAGE(ack,_message, SODE_TRNSCT_CL,SODE_NTWPOINT)
	((TRNSCT_SM_ACK_CL*)_sm)->ACKTOB = ack;

#ifdef CHECKDOA
		if ( Comap->getDoaState(_sm->getSL_CO_P()->call_oset,_message->getModulus()) == DOA_DELETED){
			DEBASSERT("CHECKDOA DOA_DELETED call_oset" <<_sm->getSL_CO_P()->call_oset)
		}
#endif

	((TRNSCT_SM_ACK_CL*)_sm)->ACKTOB->setLock(_sm->getSL_CO_P()->call_oset);

#ifdef USEFASTSEND
	_sm->getSL_CO_P()->call_oset->getTRNSPRT()->p_w(ack);
#else
	SingleAction sa_1 = SingleAction(ack);
	action->addSingleAction(sa_1);
#endif

	DEBDEV("SM act_ack_to_b move to state","1")
	_sm->State = 1;
	DEBDEV("SM act_ack_to_b move OverallState_CL to","OS_COMPLETED")
	_sm->getSL_CO_P()->OverallState_CL = OS_COMPLETED;


	//
	//clear timer s???


	return action;

}
//pre_1_1_ack_cl
bool pre_200ok_from_inv_cl(SM_V6* _sm, MESSAGE* _message){

	DEBDEV("SM pre_200ok_from_inv_cl",_message)

	if (_message->getReqRepType() == REPSUPP
			&&_message->getHeadSipRequestCode() == OK_200
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_TRNSCT_CL) {
		DEBDEV("SM pre_200ok_from_inv_cl","true")
		return true;
	}
	else {
		DEBDEV("SM pre_200ok_from_inv_cl","false")
		return false;
	}
}
//act_1_1_ack_cl
ACTION* act_resend_ack_to_b(SM_V6* _sm, MESSAGE* _message) {

	DEBDEV("SM act_resend_ack_to_b","resend ACK [" << _message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: send ack to B
	CREATEMESSAGE(__message, ((TRNSCT_SM*)_sm)->getMatrixMessage(), SODE_TRNSCT_CL,SODE_NTWPOINT)

#ifdef CHECKDOA
		if ( Comap->getDoaState(_sm->getSL_CO_P()->call_oset,_message->getModulus()) == DOA_DELETED){
			DEBASSERT("CHECKDOA DOA_DELETED call_oset" <<_sm->getSL_CO_P()->call_oset)
		}
#endif

	_sm->getSL_CO_P()->call_oset->getTRNSPRT()->p_w(__message);
//	SingleAction sa_1 = SingleAction(__message);
//	action->addSingleAction(sa_1);
	DEBMESSAGE("SECOND ACK B", __message)

	return action;

}

//**********************************************************************************
TRNSCT_SM_ACK_CL::TRNSCT_SM_ACK_CL(int _requestType, MESSAGE* _matrixMess, MESSAGE* _A_Matrix, ENGINE* _sl_cc, SL_CO* _sl_co):
		TRNSCT_SM(_requestType, _matrixMess, _A_Matrix, _sl_cc, _sl_co){
	ACKTOB = MainMessage;
}
ACTION* TRNSCT_SM_ACK_CL::event(MESSAGE* _message){

	if(State == 0){
		//insert_move(0,&PA_ACK_0_1CL);
		if(pre_ack_from_alo((SM_V6*)this,_message)){
			return act_ack_to_b((SM_V6*)this,_message);
		}
	}else if(State == 0){
		//insert_move(1,&PA_ACK_1_1CL);
		if(pre_200ok_from_inv_cl((SM_V6*)this,_message)){
			return act_resend_ack_to_b((SM_V6*)this,_message);
		}
	}
	return 0x0;
//	PA_ACK_0_1CL.predicate = &pre_ack_from_alo;
//	PA_ACK_0_1CL.action = &act_ack_to_b;
//
//	PA_ACK_1_1CL.predicate = &pre_200ok_from_inv_cl;
//	PA_ACK_1_1CL.action = &act_resend_ack_to_b;

}

//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
//pre_0_1_bye_sv
bool pre_bye_from_a(SM_V6* _sm, MESSAGE* _message){

	DEBDEV("TRNSCT_INV_SV pre_bye_from_a called",_message)
	if (_message->getReqRepType() == REQSUPP
			&& (_message->getHeadSipRequestCode() == BYE_REQUEST)
			&& _message->getDestEntity() == SODE_TRNSCT_SV
			&& ( _message->getGenEntity() ==  SODE_NTWPOINT)) {
		DEBDEV("TRNSCT_INV_SV pre_bye_from_a","true")
		return true;
	}
	else {
		DEBDEV("TRNSCT_INV_SV pre_bye_from_a","false")
		return false;
	}
}
//act_0_1_bye_sv
ACTION* act_bye_to_alo(SM_V6* _sm, MESSAGE* _message) {

	DEBDEV("SM act_bye_to_alo called",_message)

//	//BYE V2
	//new code
	//**************************************
	//Action 2: 100 TRY is created and sent to NTW
	//This message is stored so it is meant for more thing so I have to lock it
	CREATEMESSAGE(a200ok, _message, SODE_TRNSCT_SV,SODE_NTWPOINT)
	SipUtil.genQuickReplyFromInvite(_message, a200ok,"200 OK");
	((TRNSCT_SM_BYE_SV*)_sm)->STORED_MESSAGE = a200ok;
	DEBDEV("STORED_MESSAGE", ((TRNSCT_SM_BYE_SV*)_sm)->STORED_MESSAGE)

#ifdef CHECKDOA
		if ( Comap->getDoaState(_sm->getSL_CO_P()->call_oset,_message->getModulus()) == DOA_DELETED){
			DEBASSERT("CHECKDOA DOA_DELETED call_oset" <<_sm->getSL_CO_P()->call_oset)
		}
#endif

	((TRNSCT_SM_BYE_SV*)_sm)->STORED_MESSAGE->setLock(_sm->getSL_CO_P()->call_oset);
#ifdef USEFASTSEND
	_sm->getSL_CO_P()->call_oset->getTRNSPRT()->p_w(a200ok);
#else
	NEWPTR(ACTION*, action, ACTION(),"ACTION")
	SingleAction sa_2 = SingleAction(a200ok);
	action->addSingleAction(sa_2);
#endif


	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1:
	//_message changes its dest and gen
	// remember initial generation  is used for backward messages like bye coming from B
	CREATEMESSAGE(by, _message, SODE_TRNSCT_SV,SODE_ALOPOINT)
	by->setSourceMessage(_message);
	SingleAction sa_1 = SingleAction(by);
	action->addSingleAction(sa_1);

	//16 luglio
	//???
//	CREATEMESSAGE(___message, ((TRNSCT_SM_BYE_SV*)_sm)->STORED_MESSAGE, SODE_TRNSCT_SV, SODE_KILLDOA)
//	SingleAction sa_3 = SingleAction(___message);
//	action->addSingleAction(sa_3);


	//**************************************
	//Action TIMER_S
	// bug timer s action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_SV));
	action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_SV));


	//BYE V2
	//DEBDEV("TRSNCT_INV_SV::act_bye_to_alo move to state 1","")
	//_sm->State = 1;
	DEBDEV("TRSNCT_INV_SV::act_bye_to_alo move to state 2","")
	DEBDEV("SM act_200ok_bye_to_a move OverallState_SV to","OS_TERMINATED")
	_sm->getSL_CO_P()->OverallState_SV = OS_TERMINATED;
	DEBDEV("SM act_200ok_bye_to_a move to state 2","")
	_sm->State = 2;

	return action;

}
ACTION* act_200ok_bye_to_a(SM_V6* _sm, MESSAGE* _message) {

	DEBDEV("SM act_200ok_bye_to_a called",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//BYE V2
	//will never be triggered
//	//Store this 200 OK for retransmission
//
//	((TRNSCT_SM_BYE_SV*)_sm)->STORED_MESSAGE = _message;
//	DEBDEV("STORED_MESSAGE", ((TRNSCT_SM_BYE_SV*)_sm)->STORED_MESSAGE)
//	((TRNSCT_SM_BYE_SV*)_sm)->STORED_MESSAGE->setLock(_sm->getSL_CO()->call_oset);
//
//	//**************************************
//	//Action 1:
//	CREATEMESSAGE(mess, _message, SODE_TRNSCT_SV,SODE_NTWPOINT)
//	SingleAction sa_1 = SingleAction(mess);
//	action->addSingleAction(sa_1);

	//**************************************
	//Action TIMER_S
	// bug timer s action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_SV));

	//BYE V2
//	CREATEMESSAGE(___message, _message, SODE_TRNSCT_SV, SODE_KILLDOA)
//	SingleAction sa_3 = SingleAction(___message);
//	action->addSingleAction(sa_3);
//
//
//	DEBDEV("SM act_200ok_bye_to_a move OverallState_SV to","OS_TERMINATED")
//	_sm->getSL_CO()->OverallState_SV = OS_TERMINATED;
//
//	DEBDEV("SM act_200ok_bye_to_a move to state 2","")
//	_sm->State = 2;

	//((SL_CC*)(_sm->getSL_CC()))->getCOMAP()->setDoaRequested(_sm->getSL_CO()->call_oset, _message->getModulus());

	return action;

}
ACTION* act_resend_200ok_to_a(SM_V6* _sm, MESSAGE* _message) {

	DEBDEV("SM act_resend_200ok_to_a called",_message)


	NEWPTR(ACTION*, action, ACTION(),"ACTION")
	//**************************************
	//Action 1: resend 200 ok
	//BYE V2
//	CREATEMESSAGE(___message, ((TRNSCT_SM_BYE_SV*)_sm)->STORED_MESSAGE, SODE_TRNSCT_SV, SODE_KILLDOA)
//	SingleAction sa_1 = SingleAction(___message);
//	action->addSingleAction(sa_1);


	//BYE V2
	SingleAction sa_2 = SingleAction(((TRNSCT_SM_BYE_SV*)_sm)->STORED_MESSAGE);
	action->addSingleAction(sa_2);


	//**************************************
	//Action TIMER_S
	// bug timer s action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_SV));

//	SingleAction sa_3 = SingleAction(_message);
//	action->addSingleAction(sa_3);

	//((SL_CC*)(_sm->getSL_CC()))->getCOMAP()->setDoaRequested(_sm->getSL_CO()->call_oset, _message->getModulus());

	return action;

}
//*****************************************************************
// TIMER_S
//*****************************************************************
bool pre_timer_s_sv_bye(SM_V6* _sm, MESSAGE* _message){

	DEBDEV("TRNSCT_BYE_SV pre_timer_s_sv_bye",_message)
	if (_message->getTypeOfInternal() == TYPE_OP
			&& _message->getOrderOfOperation().compare("TIMER_S") == 0
			&& _message->getDestEntity() == SODE_TRNSCT_SV
			&& _message->getGenEntity() ==  SODE_TRNSCT_SV
			&& _sm->getSL_CO_P()->OverallState_SV != OS_TERMINATED){
		DEBDEV("TRNSCT_BYE_SV pre_timer_s_sv_bye","true")
		return true;
	}
	else {
		DEBDEV("TRNSCT_BYE_SV pre_timer_s_sv_bye","false")
		return false;
	}
}

//**********************************************************************************
TRNSCT_SM_BYE_SV::TRNSCT_SM_BYE_SV(int _requestType, MESSAGE* _matrixMess, ENGINE* _sl_cc, SL_CO* _sl_co):
		TRNSCT_SM(_requestType, _matrixMess, _matrixMess, _sl_cc, _sl_co){

	STORED_MESSAGE = MainMessage;
}
ACTION* TRNSCT_SM_BYE_SV::event(MESSAGE* _message){

	if (State == 0){
		//insert_move(0,&PA_BYE_0_1SV);
		if ( pre_bye_from_a((SM_V6*)this, _message)){
			return act_bye_to_alo((SM_V6*)this, _message);
		}
		//insert_move(0,&PA_BYE_S_SV);
		else if ( pre_timer_s_sv_bye((SM_V6*)this, _message)){
			return act_timer_s_sv((SM_V6*)this, _message);
		}
	}else if(State == 1){
		//insert_move(1,&PA_BYE_1_2SV);
		if ( pre_200ok_from_alo((SM_V6*)this, _message)){
			return act_200ok_bye_to_a((SM_V6*)this, _message);
		}
		//insert_move(1,&PA_BYE_1_1SV);
		else if ( pre_bye_from_a((SM_V6*)this, _message)){
			//return act_null_sv((SM_V6*)this, _message);
			return act_resend_200ok_to_a((SM_V6*)this, _message);
		}
		//insert_move(1,&PA_BYE_S_SV);
		else if ( pre_timer_s_sv_bye((SM_V6*)this, _message)){
			return act_timer_s_sv((SM_V6*)this, _message);
		}
	}else if(State == 2){
		//insert_move(2,&PA_BYE_2_2SV);
		if ( pre_bye_from_a((SM_V6*)this, _message)){
			return act_resend_200ok_to_a((SM_V6*)this, _message);
		}
	}
	return 0x0;

//	//BYE V2 PA have wrong names
//	PA_BYE_0_1SV.predicate = &pre_bye_from_a;
//	PA_BYE_0_1SV.action = &act_bye_to_alo;
//
//	PA_BYE_1_2SV.predicate = &pre_200ok_from_alo;
//	PA_BYE_1_2SV.action = &act_200ok_bye_to_a;
//
//	//Bye when in state 1:
//	//A send a bye because OK did not arrive in time
//	//but 200 ok did not arrive from CLSV so ignore
//
//	PA_BYE_1_1SV.predicate = &pre_bye_from_a;
//	PA_BYE_1_1SV.action = &act_null_sv;
//
//
//	//Bye when in state 2
//	//resend the 200OK
//
//	PA_BYE_2_2SV.predicate = &pre_bye_from_a;
//	PA_BYE_2_2SV.action = &act_resend_200ok_to_a;
//
//	PA_BYE_S_SV.predicate = &pre_timer_s_sv_bye;
//	PA_BYE_S_SV.action = &act_timer_s_sv;





}
//**********************************************************************************
//**********************************************************************************
//pre_0_1_bye_cl
bool pre_bye_from_alo(SM_V6* _sm, MESSAGE* _message){

	DEBDEV("TRNSCT_SM_BYE_CL pre_bye_from_alo",_message)
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequestCode() == BYE_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_ALOPOINT) {
		DEBDEV("TRNSCT_SM_BYE_CL pre_bye_from_alo","true")
		return true;
	}
	else {
		DEBDEV("TRNSCT_SM_BYE_CL pre_bye_from_alo","false")
		return false;
	}

}
bool pre_bye_from_alarm(SM_V6* _sm, MESSAGE* _message){

	DEBDEV("TRNSCT_SM_BYE_CL pre_bye_from_alarm",_message)
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequestCode() == BYE_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_TRNSCT_CL
			&& ((TRNSCT_SM_BYE_CL*)_sm)->resend_bye < MAX_INVITE_RESEND) {
		DEBDEV("TRNSCT_SM_BYE_CL pre_bye_from_alarm","true")
		return true;
	}
	else {
		DEBDEV("TRNSCT_SM_BYE_CL pre_bye_from_alarm","false")
		return false;
	}
}
//act_1_1_bye_cl
ACTION* act_bye_to_b(SM_V6* _sm, MESSAGE* _message) {

    DEBDEV("TRNSCT_SM_BYE_CL act_bye_to_b",_message)

    NEWPTR(ACTION*, action, ACTION(),"ACTION")
    //**************************************
    //Action 1: send to NTW
    CREATEMESSAGE(_m,_message,SODE_TRNSCT_CL,SODE_NTWPOINT)
    _m->setSourceMessage(_message);

#ifdef CHECKDOA
		if ( Comap->getDoaState(_sm->getSL_CO_P()->call_oset,_message->getModulus()) == DOA_DELETED){
			DEBASSERT("CHECKDOA DOA_DELETED call_oset" <<_sm->getSL_CO_P()->call_oset)
		}
#endif

#ifdef USEFASTSEND
	_sm->getSL_CO_P()->call_oset->getTRNSPRT()->p_w(_m);
#else
	SingleAction sa_1 = SingleAction(_m);
    action->addSingleAction(sa_1);
#endif

    //**************************************
    //Action 2: send to alarm
    //careful with source message.
    CREATEMESSAGE(__timedmessage, _message, SODE_TRNSCT_CL, SODE_TRNSCT_CL)
    __timedmessage->setSourceMessage(((TRNSCT_SM*)_sm)->getA_Matrix());
    //This is to be sent later, after timer expires
    //Preconfigure message entity points, the alarm manager cannot do this
    SysTime afterT;
    GETTIME(afterT);
    lli firetime = ((lli) afterT.tv.tv_sec)*1000000+(lli)afterT.tv.tv_usec + TIMER_1 * pow(2,((TRNSCT_SM_BYE_CL*)_sm)->resend_bye);
    DEBDEV("TRNSCT_SM_BYE_CL act_bye_to_b creating alarm ", TIMER_1 * pow(2,((TRNSCT_SM_BYE_CL*)_sm)->resend_bye) << " " << firetime)
    __timedmessage->setFireTime(firetime);
    __timedmessage->setTypeOfInternal(TYPE_OP);
    __timedmessage->setTypeOfOperation(TYPE_OP_TIMER_ON);
    __timedmessage->setOrderOfOperation("TIMER_A");
    SingleAction sa_2 = SingleAction(__timedmessage);
    action->addSingleAction(sa_2);

    //**************************************
    //Action TIMER_S
    // bug timer s action->addSingleAction(((TRNSCT_SM*)_sm)->generateTimerS(SODE_TRNSCT_CL));
    action->addSingleAction(((TRNSCT_SM*)_sm)->clearTimerS(SODE_TRNSCT_CL));

    DEBDEV("TRNSCT_SM_BYE_CL act_bye_to_b move to state 1","")
    _sm->State = 1;

    ((TRNSCT_SM_BYE_CL*)_sm)->resend_bye++;


    return action;
}
ACTION* act_200ok_bye_to_alo(SM_V6* _sm, MESSAGE* _message) {

	DEBDEV("TRNSCT_SM_BYE_CL act_200ok_bye_to_alo",_message)

	NEWPTR(ACTION*, action, ACTION(),"ACTION")

	//**************************************
	//Action 1: send to ALO
	//16 luglio
//	CREATEMESSAGE(_m, _message, SODE_TRNSCT_CL, SODE_ALOPOINT)
//	SingleAction sa_1 = SingleAction(_m);
//	action->addSingleAction(sa_1);
	//16 luglio
	//???
	CREATEMESSAGE(___message, _message, SODE_TRNSCT_CL, SODE_KILLDOA)
	SingleAction sa_3 = SingleAction(___message);
	action->addSingleAction(sa_3);


	//**************************************
	//Action 2: clear alarm
	CREATEMESSAGE(__message, _message, SODE_TRNSCT_CL, SODE_TRNSCT_CL)
	__message->setTypeOfInternal(TYPE_OP);
	__message->setTypeOfOperation(TYPE_OP_TIMER_OFF);
	__message->setOrderOfOperation("TIMER_A");
	SingleAction sa_2 = SingleAction(__message);
	action->addSingleAction(sa_2);


	DEBDEV("SM act_200ok_bye_to_alo move OverallState_CL to","OS_TERMINATED")
	_sm->getSL_CO_P()->OverallState_CL = OS_TERMINATED;

	DEBDEV("TRNSCT_SM_BYE_CL act_200ok_bye_to_alo move to state 2","")
	_sm->State = 2;

	return action;
}
//*****************************************************************
//pre_1_99_bye_cl
bool pre_bye_from_alarm_maxreach(SM_V6* _sm, MESSAGE* _message){

	DEBDEV("TRNSCT_BYE_CL pre_bye_from_alarm",_message)
	if (_message->getReqRepType() == REQSUPP
			&& _message->getHeadSipRequestCode() == BYE_REQUEST
			&& _message->getDestEntity() == SODE_TRNSCT_CL
			&& _message->getGenEntity() ==  SODE_TRNSCT_CL
			&& ((TRNSCT_SM_BYE_CL*)_sm)->resend_bye >= MAX_INVITE_RESEND) {
		DEBDEV("TRNSCT_BYE_CL pre_bye_from_alarm","true")
		return true;
	}
	else {
		DEBDEV("TRNSCT_INV_CL pre_bye_from_alarm","false")
		return false;
	}
}
//**********************************************************************************
TRNSCT_SM_BYE_CL::TRNSCT_SM_BYE_CL(int _requestType, MESSAGE* _matrixMess, MESSAGE* _A_Matrix, ENGINE* _sl_cc, SL_CO* _sl_co):
		TRNSCT_SM(_requestType, _matrixMess, _A_Matrix, _sl_cc, _sl_co){
	resend_bye = 1;
}
ACTION* TRNSCT_SM_BYE_CL::event(MESSAGE* _message){

	if (State == 0){
		//insert_move(0,&PA_BYE_0_1CL);
		if ( pre_bye_from_alo((SM_V6*)this, _message)){
			return act_bye_to_b((SM_V6*)this, _message);
		}
		//insert_move(0,&PA_BYE_S_CL);
		else if ( pre_timer_s_cl((SM_V6*)this, _message)){
			return act_timer_s_cl((SM_V6*)this, _message);
		}
	}else if (State == 1){
		//insert_move(1,&PA_BYE_1_1CL);
		if ( pre_bye_from_alarm((SM_V6*)this, _message)){
			return act_bye_to_b((SM_V6*)this, _message);
		}
		//insert_move(1,&PA_BYE_S_CL);
		else if ( pre_timer_s_cl((SM_V6*)this, _message)){
			return act_timer_s_cl((SM_V6*)this, _message);
		}
		//insert_move(1,&PA_BYE_1_99CL);
		else if ( pre_bye_from_alarm_maxreach((SM_V6*)this, _message)){
			return act_terminate_cl((SM_V6*)this, _message);
		}
		//insert_move(1,&PA_BYE_1_2CL);
		else if ( pre_200ok_from_b((SM_V6*)this, _message)){
			return act_200ok_bye_to_alo((SM_V6*)this, _message);
		}
	}else if (State == 2){
		//insert_move(2,&PA_BYE_2_2CL);
		if ( pre_200ok_from_b((SM_V6*)this, _message)){
			return act_200ok_bye_to_alo((SM_V6*)this, _message);
		}

	}
	return 0x0;

//	PA_BYE_0_1CL.predicate = &pre_bye_from_alo;
//	PA_BYE_0_1CL.action = &act_bye_to_b;
//
//	PA_BYE_1_1CL.predicate = &pre_bye_from_alarm;
//	PA_BYE_1_1CL.action = &act_bye_to_b;
//
//	PA_BYE_1_99CL.predicate = &pre_bye_from_alarm_maxreach;
//	PA_BYE_1_99CL.action = &act_terminate_cl;
//
//	PA_BYE_1_2CL.predicate = &pre_200ok_from_b;
//	PA_BYE_1_2CL.action = &act_200ok_bye_to_alo;
//
//	PA_BYE_S_CL.predicate = &pre_timer_s_cl;
//	PA_BYE_S_CL.action = &act_timer_s_cl;
//
//	//New sm rework
//	PA_BYE_2_2CL.predicate = &pre_200ok_from_b;
//	PA_BYE_2_2CL.action = &act_200ok_bye_to_alo;

}
//**********************************************************************************

