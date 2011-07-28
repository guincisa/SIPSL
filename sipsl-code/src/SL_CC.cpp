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
// Message Router
//**********************************************************************************

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
#ifndef TRNSPRT_H
#include "TRNSPRT.h"
#endif

static SIPUTIL SipUtil;
extern COMAP* Comap;

//**********************************************************************************
//**********************************************************************************
SL_CC::SL_CC(int _i, int _m, string _s):ENGINE(_i, _m, _s) {
    NEWPTR2(comap, COMAP(),"COMAP()")
	Comap = comap;
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
void SL_CC::setDAO(DAO* _dao) {
    dao = _dao;
}
DAO* SL_CC::getDAO(void) {
    return dao;
}
//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
void SL_CC::parse(void* __mess, int _mmod){

	DEBINF("void SL_CC::parse(void* __mess, int _mmod)", this<<"]["<<__mess<<"]["<<_mmod)
    RELLOCK(&(sb[_mmod]->condvarmutex),"sb"<<_mmod<<".condvarmutex");

    PROFILE("SL_CC::parse() start")
    TIMEDEF
    SETNOW

	MESSAGE* _mess = (MESSAGE*)__mess;

    DEBDEV("SL_CC::parse", _mess)
    DEBMESSAGESHORT("SL_CC::parse", _mess)

    if (_mess == MainMessage){
        DEBASSERT("_mess == MainMessage")
    }

    if (_mess->getGenEntity() == SODE_NTWPOINT){

        DEBSIP("SL_CC::parse incoming message from network ", _mess->getGenEntity())

        CALL_OSET* call_oset = 0x0;

        string callids = _mess->getHeadCallId();
        int modulus = _mess->getModulus();

        DEBSIP("SL_CC::parse CALLOSET normal ID",callids<<"]["<<modulus)

        GETLOCK(&(comap->unique_exx[modulus]),"unique_exx"<<modulus,24);

        //if deleted?!?!?!?!?
        call_oset = comap->getCALL_OSET_XMain(callids, modulus);

        //First try to get the Call object using x side parameters
        if (call_oset != 0x0) {
            DEBINF("SL_CC::parse", "A SIDE call_oset exists ["<<call_oset)

            //to SV if Request to CL if Reply
            if (_mess->getReqRepType() == REQSUPP) {
                _mess->setDestEntity(SODE_TRNSCT_SV);
                _mess->setRequestDirection(SODE_FWD);
            }
            else if (_mess->getReqRepType() == REPSUPP){
                _mess->setDestEntity(SODE_TRNSCT_CL);
            }

            //Main entrance to sl_co through comap
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
            else {
                DEBY
            }
            //Message has been worked by SL_CO
            //PRINTDIFF("SL_CC::parse() end")
            CALCPERF("SL_CC::parse() end",3)
            return;
        }
        // Then try to get call object using y side params
        else {
            //if found then is locked
            call_oset = comap->getCALL_OSET_YDerived(callids,modulus);
            if (call_oset != 0x0){
                DEBINF("SL_CC::parse", "B SIDE call_oset exists ["<< call_oset)

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
                }
                //PRINTDIFF("SL_CC::parse() end")
                CALCPERF("SL_CC::parse() end",3)
                return;
            }
        }
        // call_oset == 0x0 always true here
        // COMAP[mod] still locked!
        // Call does not exists on any side

        if (call_oset == 0x0 && _mess->getReqRepType() == REQSUPP && _mess->getHeadSipRequestCode() == INVITE_REQUEST) {
            //new call Server (originating) side
            DEBINF("SL_CC::parse new call CALL_OSET creation X side, message", _mess)

            //If new then it is always SODE_APOINT
            _mess->setDestEntity(SODE_TRNSCT_SV);

#ifdef QUICKTRY
            //Quicktry here
            // this will send the try now instead of waiting allocation of CALL_OSET
            // it should avoid retransmissions from client when SIPSL is overloaded
        	CREATEMESSAGE(etry, _mess, SODE_TRNSCT_SV,SODE_NTWPOINT)
        	SipUtil.genTryFromInvite(_mess, etry);
        	transport->p_w(etry);
#endif

            //////////////////////////////
            //Start - Initialization block
            CALL_OSET* call_oset = 0x0;
            call_oset = comap->setCALL_OSET(callids, modulus, this, transport, _mess, getSUDP()->getDomain());
            //End
            //////////////////////////////

            DEBINF("SL_CC::parse CALL_OSET created by x side", callids << "] [" <<call_oset)
            if (comap->use_CALL_OSET_SL_CO_call(call_oset, _mess,modulus) == -1 ){
                DEBINF("SL_CC::parse rejected by COMAP", callids)
                if(!_mess->getLock()){
                    PURGEMESSAGE(_mess)
                }
                else {
                    DEBINF("Put this message into the locked messages table",_mess)
                    DEBASSERT("")
                }
            }else {
                DEBY
            }
            //PRINTDIFF("SL_CC::parse() end")
            CALCPERF("SL_CC::parse() end",3)
            return;
        }
        else {
            DEBMESSAGE("Unexpected message ignored", _mess)
            if(!_mess->getLock()){
                 PURGEMESSAGE(_mess)
            }else {
                 DEBASSERT ("Unexpected message ignored found locked")
            }
        	RELLOCK(&(comap->unique_exx[modulus]),"unique_exx"<<modulus);
            //PRINTDIFF("SL_CC::parse() end")
            CALCPERF("SL_CC::parse() end",3)
            return;
        }
    }
    //Message coming from inside
    else if (_mess->getGenEntity() == SODE_ALOPOINT || _mess->getGenEntity() == SODE_TRNSCT_CL || _mess->getGenEntity() == SODE_TRNSCT_SV){

        DEBINF("SL_CC::parse entity from SODE_ALOPOINT (3) or SODE_TRNSCT_CL (4) or SODE_TRNSCT_SV (5)", _mess->getGenEntity() )

        //Careful with source message
        //Ok if coming from server: its the retransmission of 200ok for A

        string callids = _mess->getSourceMessageCallId();
        int modulus = _mess->getSourceModulus();

        if (callids.length() == 0){
            DEBMESSAGE("No source call id in the incoming message",_mess)
            DEBASSERT("No source call id in the incoming message")
        }

#ifdef DEBCODE
        if (modulus != _mess->getModulus()){
            DEBASSERT("Modulus inconsistency")
        }
#endif

        DEBSIP("SL_CC::parse Message from ALO/TRNSCT_CL generating call object", callids)

        CALL_OSET* call_oset = 0x0;

        //COMAP locked here
        GETLOCK(&(comap->unique_exx[modulus]) , "unique_exx" << modulus ,25)
        call_oset = comap->getCALL_OSET_XMain(callids,modulus);

        if (call_oset == 0x0) {
            call_oset = comap->getCALL_OSET_YDerived(callids,modulus);
            if (call_oset != 0x0){
                DEBINF("SL_CC::parse", "B SIDE call_oset exists ["<<call_oset)
                if (comap->use_CALL_OSET_SL_CO_call(call_oset, _mess,modulus) == -1 ){
                    DEBINF("SL_CC::parse rejected by COMAP", callids)
                    if(!_mess->getLock()){
                        PURGEMESSAGE(_mess)
                    }
                    else {
                    	DEBOUT("locked message", _mess)
                        DEBASSERT("locked message")
                    }
                } else{
                    DEBY
                }
                //PRINTDIFF("SL_CC::parse() end")
                CALCPERF("SL_CC::parse() end",3)
                return;
            }else{
                //Not existent or deleted
            	//From alarm
                if(!_mess->getLock()){
                    PURGEMESSAGE(_mess)
                }
                else {
                    DEBOUT("Unexpected locked message",_mess)
                	DEBASSERT("Unexpected locked message")
                }
            	RELLOCK(&(comap->unique_exx[modulus]),"unique_exx"<<modulus);
            }
        }
        else {//!=0x0
            if (comap->use_CALL_OSET_SL_CO_call(call_oset, _mess,modulus) == -1 ){
                DEBINF("SL_CC::parse rejected by COMAP", callids)
                if(!_mess->getLock()){
                    PURGEMESSAGE(_mess)
                }
                else {
                    DEBOUT("Unexpected locked message",_mess)
					// it may be locked because it may have created an SM and stored
                }
            }else{
                DEBY
            }
            //PRINTDIFF("SL_CC::parse() end")
            CALCPERF("SL_CC::parse() end",3)
            return;
        }
    } else {
        DEBINF("Unexpected source of the message", _mess->getGenEntity())
        DEBASSERT("")
    }
    //PRINTDIFF("SL_CC::parse() end")
    CALCPERF("SL_CC::parse() end",3)
    return;
}
