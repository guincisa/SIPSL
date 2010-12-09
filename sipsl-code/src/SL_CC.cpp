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


//**********************************************************************************
//**********************************************************************************
SL_CC::SL_CC(int _i):ENGINE(_i) {

	NEWPTR2(comap, COMAP(),"COMAP()")
}
COMAP* SL_CC::getCOMAP(void){
	return comap;
}
void SL_CC::linkTransport(TRNSPRT* _transport){
	transport = _transport;
}
void SL_CC::linkSipEngine(ENGINE* _sipengine){
	sipengine = _sipengine;
}
//**********************************************************************************
//**********************************************************************************
void SL_CC::parse_s(MESSAGE* _mess) {

	DEBOUT("SL_CC::parse_s", _mess)
	RELLOCK(&(rej.condvarmutex),"rej.condvarmutex");

	//Unlock it at the end
	sipengine->lockBuffer();

	//Trick because the parse below does an unlock...
	GETLOCK(&(sb.condvarmutex),"sb.condvarmutex");
	parse(_mess);
	sipengine->unLockBuffer();
	return;
}
//**********************************************************************************
//**********************************************************************************
void SL_CC::parse(MESSAGE* _mess) {

	DEBMESSAGESHORT("SL_CC::parse", _mess)

	RELLOCK(&(sb.condvarmutex),"sb.condvarmutex");

	//Maybe it has been delete by DOA when outside the call_oset
	//TODO needed???
	if (_mess == MainMessage){
		DEBASSERT("_mess == MainMessage")
	}

	if (_mess->getGenEntity() == SODE_NTWPOINT){

		DEBSIP("SL_CC::parse incoming message from network ", _mess->getGenEntity())

		CALL_OSET* call_oset = 0x0;

		string callids = _mess->getHeadCallId().getContent();
		int modulus = _mess->getModulus();

		DEBSIP("SL_CC::parse CALLOSET normal ID",callids)

		//If found it will be locked here
		call_oset = comap->getCALL_OSET_XMain(callids, modulus);

		//First try to get the Call object using x side parameters
		if (call_oset != 0x0) {
			DEBINF("SL_CC::parse", "A SIDE call_oset exists")

			//to SV if Request to CL if Reply
			if (_mess->getReqRepType() == REQSUPP) {
				_mess->setDestEntity(SODE_TRNSCT_SV);
				_mess->setRequestDirection(SODE_FWD);
			}
			else if (_mess->getReqRepType() == REPSUPP){
				_mess->setDestEntity(SODE_TRNSCT_CL);
			}

			//Main entrance to sl_co
			if (comap->use_CALL_OSET_SL_CO_call(call_oset, _mess, modulus) == -1 ){
				DEBINF("SL_CC::parse rejected by COMAP", callids)
				if(!_mess->getLock()){
					PURGEMESSAGE(_mess)
				}
				else {
					DEBINF("Put this message into the locked messages table",_mess)
					DEBASSERT("")
				}
			}
			//MEssage has been worked by SL_CO
			return;
		}
		// Then try to get call object using y side params
		else {
			//if found then is locked
			call_oset = comap->getCALL_OSET_YDerived(callids,modulus);
			if (call_oset != 0x0){
				DEBINF("SL_CC::parse", "B SIDE call_oset exists "<< call_oset)

				//to SV if Request to CL if Reply
				if (_mess->getReqRepType() == REQSUPP) {
					_mess->setDestEntity(SODE_TRNSCT_SV);
					_mess->setRequestDirection(SODE_BKWD);
				}
				else if (_mess->getReqRepType() == REPSUPP){
					_mess->setDestEntity(SODE_TRNSCT_CL);
				}
				if (comap->use_CALL_OSET_SL_CO_call(call_oset, _mess,modulus) == -1 ){
					DEBINF("SL_CC::parse rejected by COMAP", callids)
					//DEST = SODE_KILL !
					if(!_mess->getLock()){
						PURGEMESSAGE(_mess)
					}
					else {
						DEBINF("Put this message into the locked messages table",_mess)
						DEBASSERT("")
					}
				}
				return;
			}
		}
		// Does not exists on any side
		if (call_oset == 0x0 && _mess->getReqRepType() == REQSUPP) {
			//new call Server (originating) side
			DEBINF("SL_CC::parse new call CALL_OSET creation X side, message", _mess)

			//If new then it is always SODE_APOINT
			_mess->setDestEntity(SODE_TRNSCT_SV);

			//////////////////////////////
			//Start - Initialization block
			NEWPTR2(call_oset, CALL_OSET(this, transport, callids),"CALL_OSET ACCESS CALL_OSET(this, callids)")
			comap->setCALL_OSET(callids, call_oset, modulus);
			//End
			//////////////////////////////

			DEBINF("SL_CC::parse CALL_OSET created by x side", callids << "] [" <<call_oset)
			if (comap->use_CALL_OSET_SL_CO_call(call_oset, _mess,modulus) == -1 ){
				DEBINF("SL_CC::parse rejected by COMAP", callids)
				//TODO delete message
			}
			return;
		}else {
			DEBMESSAGE("Unexpected message ignored", _mess)
			if(!_mess->getLock()){
				PURGEMESSAGE(_mess)
			}else {
				DEBASSERT ("Unexpected message ignored found locked")
			}
			return;
		}
	}
	else if (_mess->getGenEntity() == SODE_ALOPOINT || _mess->getGenEntity() == SODE_TRNSCT_CL || _mess->getGenEntity() == SODE_TRNSCT_SV){

		DEBINF("SL_CC::parse entity from SODE_ALOPOINT (3) or SODE_TRNSCT_CL (4) or SODE_TRNSCT_SV (5)", _mess->getGenEntity() )

		//Careful with source message
		//Ok if coming from server: its the retransmission of 200ok for A
		DEBY
		DEBOUT("SL_CC::parse _mess->getSourceMessage()", _mess->getSourceMessage())
		if (_mess->getSourceMessage() == MainMessage){
			DEBOUT("_mess", _mess)
			DEBOUT("_mess->getGenEntity", _mess->getGenEntity())
			DEBOUT("_mess->getDestEntity", _mess->getDestEntity())
			DEBASSERT("_mess->getSourceMessage() == MainMessage")
		}
		string callids = _mess->getSourceMessage()->getHeadCallId().getContent();
		int modulus = _mess->getSourceMessage()->getModulus();

		DEBCODE(
		if (modulus != _mess->getModulus()){
			DEBASSERT("Modulus inconsistency")
		})

		DEBSIP("SL_CC::parse Message from ALO/TRNSCT_CL generating call object", callids)

		CALL_OSET* call_oset = 0x0;

		call_oset = comap->getCALL_OSET_XMain(callids,modulus);
		//TODO may be deleted here?
		if (call_oset == 0x0) {
			call_oset = comap->getCALL_OSET_YDerived(callids,modulus);
			if (call_oset != 0x0){
				DEBINF("SL_CC::parse", "B SIDE call_oset exists")
				if (comap->use_CALL_OSET_SL_CO_call(call_oset, _mess,modulus) == -1 ){
					DEBINF("SL_CC::parse rejected by COMAP", callids)
					if(!_mess->getLock()){
						PURGEMESSAGE(_mess)
					}
					else {
						DEBINF("Put this message into the locked messages table",_mess)
					}
				}
				return;
			}else{
				//Not existent or deleted
				if(!_mess->getLock()){
					PURGEMESSAGE(_mess)
				}
				else {
					DEBINF("Put this message into the locked messages table",_mess)
				}
			}

		}
		else {
			if (comap->use_CALL_OSET_SL_CO_call(call_oset, _mess,modulus) == -1 ){
				DEBINF("SL_CC::parse rejected by COMAP", callids)
				if(!_mess->getLock()){
					PURGEMESSAGE(_mess)
				}
				else {
					DEBINF("Put this message into the locked messages table",_mess)
				}
			}
			return;
		}
	} else {
		DEBINF("Unexpected source of the message", _mess->getGenEntity())
		DEBASSERT("")
	}
    return;
}
