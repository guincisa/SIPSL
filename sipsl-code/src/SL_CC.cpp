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
//
//**********************************************************************************
// SL_CC Core and Service Layer Call Control
// Message Router
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
#ifndef TRNSPRT_H
#include "TRNSPRT.h"
#endif
#ifndef DAO_H
#include "DAO.h"
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

	DEBUGSL_CC_3("void SL_CC::parse(void* __mess, int _mmod)", this<<"]["<<__mess<<"]["<<_mmod)
    RELLOCK(&(sb[_mmod]->condvarmutex),"sb"<<_mmod<<".condvarmutex");

    PROFILE("SL_CC::parse() start")
    TIMEDEF
    SETNOW

	MESSAGE* _mess = (MESSAGE*)__mess;

    DEBUGSL_CC("SL_CC::parse", _mess)
    DEBMESSAGE("SL_CC::parse", _mess)

    if (_mess == MainMessage){
        DEBASSERT("_mess == MainMessage")
    }

    if (_mess->getGenEntity() == SODE_NTWPOINT){

    	DEBUGSL_CC("SL_CC::parse incoming message from network ", _mess->getGenEntity())

        CALL_OSET* call_oset = 0x0;

        string callids = _mess->getHeadCallId();
        int modulus = _mess->getModulus();


        DEBUGSL_CC("SL_CC::parse CALLOSET normal ID",callids<<"]["<<modulus)

        GETLOCK(&(comap->unique_exx[modulus]),"unique_exx"<<modulus,24);

		//if deleted?!?!?!?!?
		call_oset = comap->getCALL_OSET_XMain(callids, modulus);

        //First try to get the Call object using x side parameters
        if (call_oset != 0x0 ) {
        	DEBUGSL_CC("SL_CC::parse", "A SIDE call_oset exists ["<<call_oset)

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
            	DEBUGSL_CC("SL_CC::parse rejected by COMAP", callids)
                if(!_mess->getLock()){
                    PURGEMESSAGE(_mess)
                }
                else {
                	DEBUGSL_CC("Put this message into the locked messages table",_mess)
                    DEBASSERT("")
                }
            }
            else {
                DEBY
            }
            //Message has been worked by SL_CO
            PRINTDIFF("SL_CC::parse() end - x found")
            CALCPERF("SL_CC::parse() end",3)
            return;
        }
        // Then try to get call object using y side params
        else {
            //if found then is locked
            call_oset = comap->getCALL_OSET_YDerived(callids,modulus);
            if (call_oset != 0x0){
            	DEBUGSL_CC("SL_CC::parse", "B SIDE call_oset exists ["<< call_oset)

                //to SV if Request to CL if Reply
                if (_mess->getReqRepType() == REQSUPP) {
                    _mess->setDestEntity(SODE_TRNSCT_SV);
                    _mess->setRequestDirection(SODE_BKWD);
                }
                else if (_mess->getReqRepType() == REPSUPP){
                    _mess->setDestEntity(SODE_TRNSCT_CL);
                }
                if (comap->use_CALL_OSET_SL_CO_call(call_oset, _mess,modulus) == -1 ){
                	DEBUGSL_CC("SL_CC::parse rejected by COMAP", callids)
                }
                PRINTDIFF("SL_CC::parse() end - y found")
                CALCPERF("SL_CC::parse() end",3)
                return;
            }
        }
        // call_oset == 0x0 always true here
        // COMAP[mod] still locked!
        // Call does not exists on any side

        if (call_oset == 0x0 && _mess->getReqRepType() == REQSUPP && _mess->getHeadSipRequestCode() == INVITE_REQUEST) {
            //new call Server (originating) side
        	DEBUGSL_CC("SL_CC::parse new call CALL_OSET creation X side, message", _mess)

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
            call_oset = comap->setCALL_OSET(callids, modulus, this, transport, _mess, getSUDP()->getDomain(),TYPE_SL_CO);
            //End
            //////////////////////////////

            DEBUGSL_CC("SL_CC::parse CALL_OSET created by x side", callids << "] [" <<call_oset)
            if (comap->use_CALL_OSET_SL_CO_call(call_oset, _mess,modulus) == -1 ){
            	DEBUGSL_CC("SL_CC::parse rejected by COMAP", callids)
                if(!_mess->getLock()){
                    PURGEMESSAGE(_mess)
                }
                else {
                	DEBUGSL_CC("Put this message into the locked messages table",_mess)
                    DEBASSERT("")
                }
            }else {
            	DEBUGSL_CC("","")
            }
            PRINTDIFF("SL_CC::parse() end - new calloset")
            CALCPERF("SL_CC::parse() end",3)
            return;
        }
        else if (call_oset == 0x0 && _mess->getReqRepType() == REQSUPP && _mess->getHeadSipRequestCode() == REGISTER_REQUEST) {

        //Register
#ifdef VODAFONEBB
//        //Using VIA
//        DEBOUT("username@domain",_mess->getFromUser())
//        DEBOUT("REGISTER getViaUriHost",_mess->getViaUriHost())
//		DEBOUT("REGISTER port",_mess->getViaUriPort())
//		DEBOUT("REGISTER port using default","5060")
//
//		stringstream _xx;
//		//_xx << _mess->getViaUriHost() << ":"<<_mess->getViaUriPort() ;
//		_xx << _mess->getViaUriHost() << ":5060";

        DEBUGSL_CC("REGISTER username",_mess->getContactName())
		DEBUGSL_CC("REGISTER address ",_mess->getContactAddress())
		DEBUGSL_CC("REGISTER port using default 5060, not ",_mess->getContactPort())

		stringstream _xx;
		//_xx << _mess->getViaUriHost() << ":"<<_mess->getViaUriPort() ;
		_xx << _mess->getContactAddress() << ":" << _mess->getContactPort();

		dao->putData(TBL_REGISTER,make_pair(_mess->getContactName(),_xx.str()));

#else
        	//Inserting regiter data
		//into dao and reply 200 OK
			DEBUGSL_CC("username@domain",_mess->getFromUser())
			DEBUGSL_CC("REGISTER port",ntohs(_mess->getEchoClntAddr().sin_port))
			DEBUGSL_CC("REGISTER address",inet_ntoa(_mess->getEchoClntAddr().sin_addr))
			stringstream _xx;
			_xx << inet_ntoa(_mess->getEchoClntAddr().sin_addr) << ":" << ntohs((_mess->getEchoClntAddr()).sin_port);
			// TBL nat never put
			//dao->putData(TBL_NAT,make_pair(_mess->getFromUser(),_xx.str()));
			dao->putData(TBL_ROUTE,make_pair(_mess->getFromUser(),_xx.str()));

#endif

			CREATEMESSAGE(OKregister, _mess, SODE_TRNSCT_SV,SODE_NTWPOINT)
			SipUtil.genASideReplyFromRequest(_mess,OKregister);
			OKregister->setHeadSipReply("200 OK");
        	OKregister->dropHeader("Contact:");
        	OKregister->compileMessage();
        	transport->p_w(OKregister);
        	RELLOCK(&(comap->unique_exx[modulus]),"unique_exx"<<modulus);

        }
        else if (call_oset == 0x0 && _mess->getReqRepType() == REQSUPP && _mess->getHeadSipRequestCode() == MESSAGE_REQUEST) {

            call_oset = comap->getCALL_OSET_XMain(_mess->getFromUser(), modulus);

            if (call_oset != 0x0) {
            	DEBUGSL_CC("SL_CC::parse", "A SIDE call_oset exists ["<<call_oset)
            }
            else{
            	//in messaging only X side exists
            	//create
                call_oset = comap->setCALL_OSET(callids, modulus, this, transport, _mess, getSUDP()->getDomain(),TYPE_SL_MO);
            }
            if (comap->use_CALL_OSET_SL_CO_call(call_oset, _mess,modulus) == -1 ){
            	DEBUGSL_CC("SL_CC::parse rejected by COMAP", callids)
                if(!_mess->getLock()){
                    PURGEMESSAGE(_mess)
                }
                else {
                	DEBUGSL_CC("Put this message into the locked messages table",_mess)
                    DEBASSERT("")
                }
            }else {
            	DEBUGSL_CC("","")
            }
        }else if (call_oset == 0x0 && _mess->getReqRepType() == REPSUPP && SUDP::getRealm() == MESSAGE_REALM){

            call_oset = comap->getCALL_OSET_XMain(_mess->getFromUser(), modulus);


        }

        else {
        	DEBUGSL_CC("Unexpected message ignored", _mess)
            if(!_mess->getLock()){
                 PURGEMESSAGE(_mess)
            }else {
                 DEBASSERT ("Unexpected message ignored found locked")
            }
        	RELLOCK(&(comap->unique_exx[modulus]),"unique_exx"<<modulus);
            PRINTDIFF("SL_CC::parse() end - unexpected message")
            CALCPERF("SL_CC::parse() end",3)
            return;
        }
    }
    //Message coming from inside
    else if (_mess->getGenEntity() == SODE_ALOPOINT || _mess->getGenEntity() == SODE_TRNSCT_CL || _mess->getGenEntity() == SODE_TRNSCT_SV){

    	DEBUGSL_CC("SL_CC::parse entity from SODE_ALOPOINT (3) or SODE_TRNSCT_CL (4) or SODE_TRNSCT_SV (5)", _mess->getGenEntity() )

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

        DEBUGSL_CC("SL_CC::parse Message from ALO/TRNSCT_CL generating call object", callids)

        CALL_OSET* call_oset = 0x0;

        //COMAP locked here
        GETLOCK(&(comap->unique_exx[modulus]) , "unique_exx" << modulus ,25)
        call_oset = comap->getCALL_OSET_XMain(callids,modulus);

        if (call_oset == 0x0) {
            call_oset = comap->getCALL_OSET_YDerived(callids,modulus);
            if (call_oset != 0x0){
            	DEBUGSL_CC("SL_CC::parse", "B SIDE call_oset exists ["<<call_oset)
                if (comap->use_CALL_OSET_SL_CO_call(call_oset, _mess,modulus) == -1 ){
                	DEBUGSL_CC("SL_CC::parse rejected by COMAP", callids)
                    if(!_mess->getLock()){
                        PURGEMESSAGE(_mess)
                    }
                    else {
                    	DEBUGSL_CC("locked message", _mess)
                        DEBASSERT("locked message")
                    }
                } else{
                	DEBUGSL_CC("","")
                }
                PRINTDIFF("SL_CC::parse() end - reply (y) sm found")
                CALCPERF("SL_CC::parse() end",3)
                return;
            }else{
                //Not existent or deleted
            	//From alarm
                if(!_mess->getLock()){
                    PURGEMESSAGE(_mess)
                }
                else {
                	DEBUGSL_CC("Unexpected locked message",_mess)
                	DEBASSERT("Unexpected locked message")
                }
            	RELLOCK(&(comap->unique_exx[modulus]),"unique_exx"<<modulus);
            }
        }
        else {//!=0x0
            if (comap->use_CALL_OSET_SL_CO_call(call_oset, _mess,modulus) == -1 ){
            	DEBUGSL_CC("SL_CC::parse rejected by COMAP", callids)
                if(!_mess->getLock()){
                    PURGEMESSAGE(_mess)
                }
                else {
                	DEBUGSL_CC("Unexpected locked message",_mess)
					// it may be locked because it may have created an SM and stored
                }
            }else{
            	DEBUGSL_CC("","")
            }
            PRINTDIFF("SL_CC::parse() end - reply (x) sm found")
            CALCPERF("SL_CC::parse() end",3)
            return;
        }
    } else {
    	DEBUGSL_CC("Unexpected source of the message", _mess->getGenEntity())
        DEBASSERT("")
    }
    PRINTDIFF("SL_CC::parse() end - end")
    CALCPERF("SL_CC::parse() end",3)
    return;
}
