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
//
//**********************************************************************************
// SL_CC Core and Service Layer Call Control
// Implements the basic call state machine
//**********************************************************************************
//#include <pthread.h>
//#include <unistd.h>
//#include <iostream>
//#include <stdio.h>
//#include <string>
//#include <sys/socket.h> /* for socket() and bind() */
//#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
//#include <stdlib.h>     /* for atoi() and exit() */

#include <vector>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <map>

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

//**********************************************************************************
//**********************************************************************************
SL_CC::SL_CC(void) {

	comap = new COMAP();
}
COMAP* SL_CC::getCOMAP(void){
	return comap;
}

//**********************************************************************************
//**********************************************************************************
void SL_CC::parse(MESSAGE* _mess) {

	//rifare tutta perche qui se entro col messaggio destinato alla SV ok
	//ma se entro con un messaggio destinato alla CL devo prendere ls SV
	//e creare la CL

	// if the message comes from A then it is a new message that will trigger a new CALL OBJECT

	DEBOUT("Incoming message generating entity", _mess->getGenEntity())

	if (_mess->getGenEntity() == SODE_NTWPOINT){

		DEBOUT("SL_CC::parse", _mess->getIncBuffer())

		CALL_OSET* call_oset = 0x0;

		string callidx = _mess->getHeadCallId().getNormCallId() +
				_mess->getSTKHeadVia().top()->getC_AttVia().getViaParms().findRvalue("branch");

		DEBOUT("SL_CC::parse CALLOSET ID",callidx)

		call_oset = comap->getCALL_OSET_SV(callidx);

		if (call_oset != 0x0) {
			DEBOUT("SL_CC::parse", "A SIDE call_oset existing")
			_mess->setDestEntity(SODE_SMSVPOINT);
			_mess->setGenEntity(SODE_APOINT);
			call_oset->getSL_CO()->call(_mess);
		}
		else {
			call_oset = comap->getCALL_OSET_CL(callidx);
			if (call_oset != 0x0){
				DEBOUT("SL_CC::parse", "B SIDE call_oset existing")
				_mess->setGenEntity(SODE_BPOINT);
				_mess->setDestEntity(SODE_SMCLPOINT);
				call_oset->getSL_CO()->call(_mess);
			}
		}
		if (call_oset == 0x0) {
			//new call X SIDE
			DEBOUT("SL_CC::parse new call", "CALL_OSET creation x side")

			_mess->setGenEntity(SODE_APOINT);
			_mess->setDestEntity(SODE_SMSVPOINT);


			call_oset = new CALL_OSET(this);
			SL_CO* sl_co = new SL_CO(call_oset);
			SL_SM_SV* sl_sm_sv = new SL_SM_SV(this);
			//NEED USER DEFINED CLASS
			VALO* alo = new VALO(this);
			alo->linkSUDP(getSUDP());

			call_oset->setSL_X(callidx, sl_co, sl_sm_sv, alo);
			DEBOUT("SL_CC::parse", "CALL_OSET created x side")

			comap->setCALL_OSET(callidx, call_oset);

			//_mess->setDestEntity(SODE_SMSVPOINT);
			sl_co->call(_mess);
			//END.
		}
	}

//	if (_mess->getGenEntity() == SODE_APOINT){
//
//		DEBOUT("SL_CC::parse", _mess->getHeadSipRequest().getContent())
//		DEBOUT("SL_CC::parse", _mess)
//
//		CALL_OSET* call_oset = 0x0;
//
//		string callidx = _mess->getHeadCallId().getNormCallId() +
//				_mess->getSTKHeadVia().top()->getC_AttVia().getViaParms().findRvalue("branch");
//		DEBOUT("CALLOSET ID",callidx)
//
//		call_oset = comap->getCALL_OSET_SV(callidx);
//
//		if (call_oset == 0x0) {
//			//new call
//			DEBOUT("SL_CC::parse new call", "CALL_OSET creation x side")
//
//			call_oset = new CALL_OSET(this);
//			SL_CO* sl_co = new SL_CO(call_oset);
//			SL_SM_SV* sl_sm_sv = new SL_SM_SV(this);
//			//NEED USER DEFINED CLASS
//			VALO* alo = new VALO(this);
//			alo->linkSUDP(getSUDP());
//
//			call_oset->setSL_X(callidx, sl_co, sl_sm_sv, alo);
//			DEBOUT("SL_CC::parse", "CALL_OSET created x side")
//
//			comap->setCALL_OSET(callidx, call_oset);
//
//			//_mess->setDestEntity(SODE_SMSVPOINT);
//			sl_co->call(_mess);
//			//END.
//
//		} else {
//			//CALL Exists
//			DEBOUT("SL_CC::parse existing call", "")
//			//_mess->setDestEntity(SODE_SMSVPOINT);
//			call_oset->getSL_CO()->call(_mess);
//		}
//	}
	else if (_mess->getGenEntity() == SODE_ALOPOINT){
		DEBOUT("SL_CC::parse gen entity","SODE_ALOPOINT")

		_mess->setDestEntity(SODE_SMCLPOINT);

		//get original idx
		string callidx = _mess->getSourceMessage()->getHeadCallId().getNormCallId() +
				_mess->getSourceMessage()->getSTKHeadVia().top()->getC_AttVia().getViaParms().findRvalue("branch");

		DEBOUT("Message from ALO generating SV machine callidx", callidx)

		CALL_OSET* call_oset = 0x0;

		call_oset = comap->getCALL_OSET_SV(callidx);

		if (call_oset == 0x0) {
			DEBOUT("SL_CC::parse","Orphan Invite")
			//TODO kill the invite
		}
		else {
				call_oset->getSL_CO()->call(_mess);
			}
		}

//    if ( m._ReqLine.getMethodId() == INTERNALS_METHOD) {
//        DEBOUT("INTERNALS_METHOD","SL_CC::parse")
//        return;
//    }
//    // DOMAIN
//    //
//    DEBOUT("=============================================================================","")
//    DEBOUT("=============================================================================","")
//    DEBOUT("=============================================================================","")
//    DEBOUT("=============================================================================","")
//    DEBOUT("=============================================================================","")
//    DEBOUT("=============================================================================","")
//
//    DEBOUT ("check Domain", sipStack->getDomain())
//    DEBOUT ("Incoming request domain",  m._ReqLine.getURI().getSipUri().getHost())
//    if (sipStack->getDomain() != m._ReqLine.getURI().getSipUri().getHost()) {
//        DEBOUT("not allowed","")
//        return;
//    }
//    else {
//        DEBOUT ("allowed","")
//    }
//
//    //////////////////////////////////////////////////////////////////////////
//    // begin here mutex on call ID.
//    // -> access/creation of call map must be mutexed
//    // -> access/creation of regmap must be mutexed
//    // -> access to state machine must be mutexed
//    // -> access to call object (CO) in general must be mutexed
//    // mutex can be :
//    // - per call id -> generate a lock for every new call id
//    // - per modulus call id -> reduce the number of locks
//    //
//
//    // look for call ID
//    // access to call map not in mutex but probably it should...
//    DEBOUT("LOOK call id", m._CallID.getCallId())
//    SL_CO *_cotmp = callMap->find(m._CallID.getCallId());
//    if ( _cotmp == NULL) {
//        DEBOUT("call id not found in call map","")
//        DEBOUT("check contact","")
//        DEBOUT("check TO","")
//
//        // NEW CALL
//        SL_CO * co = new SL_CO(this, m);
//        DEBOUT("Call Object Created","")
//        m._CallID32 = callMap->store(co,m._CallID.getCallId());
//        DEBOUT("Internal call id", m._CallID32)
//        _cotmp = co;
//    }
//    else {
//        DEBOUT("CALL id exists", m._CallID.getCallId())
//    }
//    if (m.source == 1 && m.destination == 0) {
//        m.destination = 1;
//    }
//    else if (m.source == 2 && m.destination == 0){
//        m.destination = 2;
//    }
//    _cotmp->call(m);
//    DEBOUT("after _cotmp->call()","")
//
///* b2bua non so
//        if (_cotmp->getSM()->getState() == S_REM_T ) {
//            DEBOUT("Deleting Call Object",m._CallID.getCallId())
//            callMap.removeCall(m._CallID.getCallId());
//            delete _cotmp;
//        }
//*/
//    DEBOUT("SL_CC:parse end","")
    return;
}
////**********************************************************************************
////**********************************************************************************
//void SL_CC::associateSIPSTACK(SUDP * _s) {
//    sipStack = _s;
//}
////**********************************************************************************
////**********************************************************************************
//void SL_CC::sendReply(MESSAGE me) {
//    //cout << "AC::sendReply" << endl << flush;
//    me.sendReply("");
//
//    // check if CO has to be deleted by checking in state = 3 E_END
//    if (co->getSM()->getState() == S_END) {
//        //cout << "delete" << endl;
//        delete co;
//        // remove record
//        //callMap.removeCall(me.I_I);
//    }
//*/
//    return;
//}
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SL_CC_SM_SV
//**********************************************************************************
//void SL_CC_SM_SV::stateChange(int evt) {
//    DEBOUT("","SL_CC_SM_SV::stateChange")
//}
//**********************************************************************************
// SL_CC_SM_CL
//**********************************************************************************
//void SL_CC_SM_CL::stateChange(int evt) {
//    DEBOUT("","SL_CC_SM_CL::stateChange")
//}
//**********************************************************************************
// SL_CC_CO
//**********************************************************************************
//void SL_CC_CO::call(MESSAGE& m) {
//            DEBOUT("SL_CC_CO::call invoked","")
//}


