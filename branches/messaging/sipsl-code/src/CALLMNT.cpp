//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Service Layer 
// Copyright (C) 2007 Guglielmo Incisa di Camerana
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
//**********************************************************************************

#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <map>
#include <time.h>

#include "COMPAR.h"
#include "COMM.h"
#include "ENGINE.h"
#include "CALLMNT.h"


//**********************************************************************************
//**********************************************************************************
// STATE MACHINE
//**********************************************************************************
//**********************************************************************************
SL_SM::SL_SM() {
    DEBOUT("SL_SM::SL_SM","SM created at inital state START")
    //State = S_START;
}
//**********************************************************************************
int SL_SM::getState(void) {
    return State;
}
//**********************************************************************************
MESSAGE * SL_SM::event(MESSAGE& m) {
    return NULL;
}
//**********************************************************************************
SL_SM_CL::SL_SM_CL(){
    State = 0;
}
//**********************************************************************************
SL_SM_SV::SL_SM_SV(){
    State = 0;
}
//**********************************************************************************
MESSAGE ** SL_SM_SV::event(MESSAGE& m) {

    // returns:
    // pointer to the inout message if the message passes
    // point to a new array of messages if this is to send (100TRY in answer to an INVITE)
    // NULL if error
    
    if ( State == 0) {
        if (m._ReqLine.getMethodId() == INVITE_METHOD &&
        m.source == APOINT &&
        m.destination == SMSVPOINT) {

            m.source = SMSVPOINT;
            m.destination = ALOPOINT;

            MESSAGE ** mt = new MESSAGE*[2];;
			MESSAGE messTry = new MESSAGE();
			messTry.iscomplete = false;
			messTry._ReqLine.setMethodId(3);
			mt[0] = &messTry;
			mt[1] = NULL;
            State = 1;
            return mt;
        }
    }
    if ( State == 1) {
        if (m._ReqLine.getMethodId() == RINGING_METHOD &&
        m.source == SMCLPOINT &&
        m.destination == SMSVPOINT) {
            m.source = SMSVPOINT;
            m.destination = SMCLPOINT;
            mt = &m;
            State = 2;
            return mt;
        }
    }
    if ( State == 2) {
        if (m._ReqLine.getMethodId() == OK_METHOD &&
        m.source == ALOPOINT &&
        m.destination == SMSVPOINT) {
            m.source = SMSVPOINT;
            m.destination = APOINT;
            mt = &m;
            State = 3;
            return mt;
        }
        else if (m._ReqLine.getMethodId() == OK_METHOD &&
        m.source == SMCLPOINT &&
        m.destination == SMSVPOINT) {
            m.source = SMSVPOINT;
            m.destination = STOPPOINT;
            // retrieve INVITEx and generate 200 ok from it
            MESSAGE ** mt = new MESSAGE*[2];;
			MESSAGE messEmptyOK = new MESSAGE();
			mt[0] = &messEmptyOk;
			mt[2] = NULL;
            State = 3;
            return mt;
        }
            
    }
    return mt; // NULL, error...
}
//**********************************************************************************
MESSAGE * SL_SM_CL::event(MESSAGE& m) {

    // returns:
    // pointer to the inout message if the message passes
    // point to a new message if this is to send (100TRY in answer to an INVITE)
    // NULL if error
    
    MESSAGE * mt = NULL;
    if ( State == 0) {
        if (m._ReqLine.getMethodId() == INVITE_METHOD &&
        m.source == ALOPOINT &&
        m.destination == SMCLPOINT) {
            m.source = SMCLPOINT;
            m.destination = BPOINT;
            State = 1;
            return mt;
        }
    }
    if ( State == 1) {
        if (m._ReqLine.getMethodId() == RINGING_METHOD &&
        m.source == BPOINT &&
        m.destination == SMCLPOINT) {
            m.source = SMCLPOINT;
            m.destination = SMSVPOINT;
            mt = &m;
            State = 2;
            return mt;
        }
    }
    if ( State == 2) {
        if (m._ReqLine.getMethodId() == OK_METHOD &&
        m.source == BPOINT &&
        m.destination == SMCLPOINT) {
            m.source = SMCLPOINT;
            m.destination = SMSVPOINT;
            mt = &m;
            State = 3;
            return mt;
        }
    }
    return mt; // NULL, error...
}
//**********************************************************************************
//**********************************************************************************
// SL_CO
//**********************************************************************************
//**********************************************************************************
SL_CO::SL_CO(ENGINE *cc, MESSAGE m){
    DEBOUT("SL_CO::SL_CO","invoked")
    pthread_mutex_init(&mutex, NULL);
    alo = new ALO;
    sm_sv = new SL_SM_SV;
    sl_cc = cc;
	mastermessage = m;
    return;
}
//**********************************************************************************
SL_CO::SL_CO(){
    DEBOUT("SL_CO::SL_CO","invoked")
}
//**********************************************************************************
void SL_CO::call(MESSAGE m) {
    DEBOUT("SL_CO::call","invoked")
    pthread_mutex_lock(&mutex);
    MESSAGE * mt;
    if (m.destination == SMSVPOINT) {
        mt = sm_sv.event(m);
        if (mt == NULL) {
            //error in state machine
            DEBOUT("Unexpected message in server sm")
            return;
        }
        // else 
        if (m.destination == APOINT) {
            // send to network
        }
        if (m.destination == BPOINT) {
            //  does not happens
        }
        if (m.destination == ALOPOINT) {
            // send to ALO
            alo.p&w(m);
        }
        if (m.destination == SMCLPOINT) {
            // now to SM_CC or directly to SM_CL???
            // OR BELOW...???
        }
        if (m.destination == STOPPOINT){
            // schedule to delete
        }
        if ( mt != &m ) {
            // we have an empty message to fill
            // fill it and send to destination like above if's
        }
    }
	if (m.destination == SMCLPOINT) {
        // for client state machine	
    }
	if (m.destination == 3) {
        // for alo
    }
}
    
/*
 * CO::CO(REG_AC *rg, PROXY_AC *pr){
    //cout << "New CO " << this << endl << flush;
    pthread_mutex_init(&mutex, NULL);
    //ac = _ac;
    alo = new ALO;
    sm = new SM;
    reg_ac = rg;
    proxy_ac = pr;
    return;
}
*/
//**********************************************************************************
SL_CO::~SL_CO(void) {
    DEBOUT("SL_CO::~SL_CO","invoked")
    pthread_mutex_destroy(&mutex);
    delete alo;
    delete sm_cl;
}
//**********************************************************************************
//void SL_CO::reply(MESSAGE mess, RATYPE * ac){
    //try {
        //sm->stateChange(4);
    //} 
    //catch (ParseEx e) {
        ////cout << "Exception in CO " << e.error << endl << flush;
        //mess.sendNAck(-3, "Error in state change");
        //return;
        ////TODO something???
    //}
    //csl_cc->sendReply(mess);
//}
//**********************************************************************************
//SM * CO::getSM(void) {
    //return sm;
//}
    
//**********************************************************************************
// ALO
//**********************************************************************************
//void ALO::call(MESSAGE& acm, SL_CO * co, SL_SM * sm) {
        //return;
//}
//**********************************************************************************
// COMAP
//**********************************************************************************
COMAP::COMAP() {
    DEBOUT("COMAP::COMAP","invoked")
    int i;
    for(i = 0 ; i < MAXCOMAPLOCKS ; i++) {
        pthread_mutex_init(&(mutex[i]), NULL);
    }
}
//**********************************************************************************
string COMAP::store(SL_CO* _co, string _cid){

    char mdc[1];
    int mdi,modi;

    string tmp2;

    while (_cid.length() < 32) {
        string tmp = _cid;

        _cid = tmp + tmp;
    }

    //modulo

    tmp2 = _cid.substr(0,32);
    DEBOUT("normalized call id", tmp2);

    strcpy(mdc, &((tmp2.c_str())[31]));
    mdi = mdc[0];
    modi = mdi % MAXCOMAPLOCKS;
    DEBOUT("normalized call id modulo", modi);

    pthread_mutex_lock(&(mutex[modi]));
    OnCall.insert(CMTvalue(tmp2, _co));
    pthread_mutex_unlock(&(mutex[modi]));

    DEBOUT("Call Object", _co);

    return tmp2;
}
//**********************************************************************************
void COMAP::remove(string _cid){

    //TODO implement locking mechanism
    OnCall.erase(_cid);
    //cout << "delete " << _cid << endl;
}
//**********************************************************************************
SL_CO* COMAP::find(string _cid){

    char mdc[1];
    int mdi,modi;
    string tmp2;

    while (_cid.length() < 32) {
        string tmp = _cid;

        _cid = tmp + tmp;
    }
    tmp2 = _cid.substr(0,32);
    DEBOUT("normalized call id", tmp2);

    COMAPTYPE::iterator p;
    strcpy(mdc, &((tmp2.c_str())[31]));
    mdi = mdc[0];
    modi = mdi % MAXCOMAPLOCKS;
    DEBOUT("normalized call id modulo", modi);

    pthread_mutex_lock(&(mutex[modi]));
    p = OnCall.find(tmp2);


    if (p == OnCall.end()) {
        return (SL_CO*)NULL;
        pthread_mutex_unlock(&(mutex[modi]));
    }
    else {
        return((SL_CO*)(p->second));
        pthread_mutex_unlock(&(mutex[modi]));
    }
}
//void SM::stateChange(int evt) throw(ParseEx) {
//            DEBERROR("SM::stateChange of base class invoked")
//}
/* del b2bua
**********************************************************************************
// PROXY_SM
**********************************************************************************
void PROXY_SM::stateChange(int evt) {
    DEBOUT("","PROXY_SM::stateChange")
}
**********************************************************************************
// REG_SM
**********************************************************************************
void REG_SM::stateChange(int evt) {
    //TODO must call statechange with messagetype
    DEBOUT("","REG_SM::stateChange")
    ParseEx e = ParseEx("Wrong State Transition");
    switch (State) {
        case S_START :
            switch (evt) {
                case E_REGINNEW :
                  State = S_RIN_T;
                  return;
                  break;
                default:
                  DEBOUT("S_START","unexpected event")
                  throw e;
                  //error
            }
            break;
        case S_RIN_T :
            switch (evt) {
                case E_200OK:
                    State = S_REG;
                    return;
                    break;
		case E_REM200OK:
		    State = S_REM_T;
		    return;
		    break;
                default:
                    DEBOUT("S_RIN_T","unexpected event")
                    throw e;
                    //error
            }
            break;
        case S_REG :
            switch (evt) {
                case E_REGINADDUPREM:
                    State = S_RIN_T;
                    return;
                    break;
                case E_INTERNALRM:
                    State = S_REM_T;
                    return;
                    break;
                default:
                    DEBOUT("S_REG","unexpected event")
                    throw e;
                    //error
            }
            break;
        case S_REM_T :
            switch (evt) {
                case E_INTERNALDEL:
                    State = S_START;
                    //cout << "Call ended must remove objects scheduled" << endl << flush;
                    // actually cannot be removed now because the method
                    // invocation will need to return up to CO and then to ALO
                    return;
                    break;
                 default:
                    DEBOUT("S_REM_T","unexpected event")
                    throw e;
            }
            return;
            break;
        default:
            throw e;
    }
	return;
}
*/
/* del b2bua
**********************************************************************************
// PROXY_CO
**********************************************************************************
void PROXY_CO::call(MESSAGE& m) {
            DEBOUT("PROXY_CO::call invoked","")
}

**********************************************************************************
// REG_CO
**********************************************************************************
void REG_CO::call(MESSAGE& m) {
    DEBOUT("REG_CO::call invoked","")
    // mutex on
    
    pthread_mutex_lock(&mutex);
    int finalevent = -1;

    try {
        // REGISTER IN NEW if not in REGMAP
        bool newreg;
        string stoco = m._To.getToEruri().getEruriRuri().getSipUri().getUserInfo();
        DEBOUT("REGMAP search key",stoco)
	// Check if the REGISTER is OK
	// --> if Expire no present check expire into contact
	// if expire is 0 deregister
        regmapstruct rmsf = reg_ac->registerMap.getRegMap(stoco);
        if ( rmsf.expire == -1) { // not found in regmap
            sm->stateChange(E_REGINNEW);
            DEBOUT("Registration creation",stoco)
            regmapstruct rms;
            rms.contact = m._Contact.getContEruri().getValue();
            rms.callid =  m._CallID.getCallId();

            if (!m._Expires.isPresent()) {
                DEBOUT("Expire header not found","")
                string exps = m._Contact.getContEruri().getEruriParms().getParmValue("expires");
                DEBOUT("Look for expire param in Contact",exps)
                if (exps.compare("") == 0) {
                    // invalid register
                    DEBOUT("REG_CO::call invalid register", "no expire")
// do something
                }
                rms.expire =atoi(exps.c_str());
            }
            else {
                rms.expire = m._Expires.getExpires();
            }

	    // if 0 deregister...
// incompleto
            DEBOUT("RMS contact",rms.contact)
            DEBOUT("RMS callid",rms.callid)
            DEBOUT("RMS expire",rms.expire)
            DEBOUT("TO",m._To.getToEruri().getEruriRuri().getSipUri().getUserInfo());

            // create reg map
            //string xtoco = m._To.getToEruri().getEruriRuri().getSipUri().getUserInfo();
            reg_ac->registerMap.createReg(rms,stoco); 
            // check
            regmapstruct tmprms = reg_ac->registerMap.getRegMap(stoco); 
            DEBOUT("register map check search key to",stoco)
            DEBOUT("RMS contact",tmprms.contact)
            DEBOUT("RMS callid",tmprms.callid)
            DEBOUT("RMS expire",tmprms.expire)
            finalevent = E_200OK;
        }
        // Registration exists
        else if (rmsf.expire != 0) {
            DEBOUT("Registration update/update",stoco)
            sm->stateChange(E_REGINADDUPREM);
            DEBOUT("RMS contact",rmsf.contact)
            DEBOUT("RMS callid",rmsf.callid)
            DEBOUT("RMS expire",rmsf.expire)
            DEBOUT("TO",m._To.getToEruri().getEruriRuri().getSipUri().getUserInfo());
            if ( m._Expires.getExpires() == 0) {
                //delete register
                DEBOUT("Registration delete",stoco)
                reg_ac->registerMap.removeReg(stoco);
                finalevent = E_REM200OK;
///// shall remove CO
            }
            else if ( m._Expires.getExpires() > 0 ) {
                DEBOUT("Registration update",stoco)
                reg_ac->registerMap.updateReg(stoco,m._Expires.getExpires());
                finalevent =  E_200OK;
            }
        }
    }
    catch (ParseEx e) {
        //cout << "Exception in CO " << e.error << endl << flush;
        //acm.sendNAck(-3, "Error in state change");
        //r.message = "Exception in CO";
        // create error reply ...
        DEBOUT ("Exception","")
    }
    // create REPLY tamplate
    //acm.createReplyTemplate("Payload placeholder");
    //acm.sendReply(1,"");
    
    //else create 200ok
    
    //Check state is REGISTER IN must send 200OK
    if ( sm->getState() == S_RIN_T) {

        DEBOUT("AC create 200OK","")
        //
        //TODO orri
        string myviaTemp = "SIP/2.0/UDP " + m._Via[15].getViaHostPort() + ";received=" + inet_ntoa(m.echoClntAddr.sin_addr) + m._Via[15].getViaParms().getValue();
        string myvia[16];
        for(int i = 0; i< 16 ; i++) {
            myvia[i] = m._Via[i].value;
        }
        myvia[15] = myviaTemp;

        REGISTER200OK *pp  = m.p_create200ok();
        auto_ptr<REGISTER200OK> pr ;
        pr = auto_ptr<REGISTER200OK>(pp);
        pr->setHeaders(m._To.value + ";tag=4714f77c-1d9aa6ce1953c",m._From.value,m._CallID.value,m._CSeq.value,myvia,m._Contact.value);
        pr->setSock(m.sock);
         contact port where sip signalling goes 
        // TODO
        m.echoClntAddr.sin_port = htons(m._Contact.getContEruri().getEruriRuri().getSipUri().getPort());    
        pr->setSockAddr(m.echoClntAddr);

        //alo->call(acm, this, sm);
        DEBOUT("before pr->send()","")
        pr->send();
        DEBOUT("after pr->send()","")

        //First move state and then send 200ok
        try {
            DEBOUT("before sm->stateChange(finalevent)","")
            sm->stateChange(finalevent);
            DEBOUT("after sm->stateChange(finalevent)","")
        }
        catch (ParseEx e) {
            DEBOUT("Parse ex sm->stateChange(E_200OK)",e.error)
            //cout << "Exception in CO " << e.error << endl << flush;
            //acm.sendNAck(-3, "Error in state change");
            //r.message = "Exception in CO";
            // create error reply ...
        }
    }

    DEBOUT("before pthread_mutex_unlock(&mutex);","")
    pthread_mutex_unlock(&mutex);
    DEBOUT("after pthread_mutex_unlock(&mutex);","")
    
    return; 
    

}
*/
/*del b2bua
**********************************************************************************
REG_CO::REG_CO(REG_AC *rg, PROXY_AC *pr){
    DEBOUT("REG_CO::REG_CO","invoked")
    //cout << "New CO " << this << endl << flush;
    pthread_mutex_init(&mutex, NULL);
    //ac = _ac;
    alo = new ALO;
    sm = new REG_SM;
    reg_ac = rg;
    proxy_ac = pr;
    return;
}*/

