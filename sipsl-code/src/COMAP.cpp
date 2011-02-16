//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Service Layer
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

//**********************************************************************************
//**********************************************************************************
COMAP::COMAP(void){
    for(int i = 0; i< COMAPS; i++){
        pthread_mutex_init(&unique_exx[i], NULL);
        DEBDEV("comap unique_exx", &unique_exx)
    }

    for (int i = 0; i < COMAPS; i++){
        loktry[i] = 0;
    }
}
COMAP::~COMAP(void){
}
//**********************************************************************************
//**********************************************************************************
CALL_OSET* COMAP::getCALL_OSET_XMain(string _callId_X, int _mod){

    PROFILE("COMAP::getCALL_OSET_XMain")
    TIMEDEF
    SETNOW

    DEBINF("COMAP::getCALL_OSET_XMain retrieving using call id", _callId_X <<" comap [" << _mod)

    if (_mod >= COMAPS){
            DEBASSERT("invalid comap index")
    }

    CALL_OSET* tmp = 0x0;
    map<string, CALL_OSET*>::iterator p;

    GETLOCK(&unique_exx[_mod],"unique_exx"<<_mod);

    p = comap_mm[_mod].find(_callId_X);
    if (p != comap_mm[_mod].end()){
            tmp = (CALL_OSET*)p->second;

        DEBINF("COMAP::getCALL_OSET found ", tmp)

        if (getDoa(tmp,_mod)== DOA_REQUESTED){
            resetDoaRequestTimer(tmp,_mod);
        }
        if (getDoa(tmp,_mod)== DOA_DELETED){
                tmp = 0x0;
        }else{
            // all other cases the call:oset will be used, lock it now
            DEBINF("CALL_OSET ACCESS COMAP::getCALL_OSET_XMain", tmp)
            RELLOCK(&unique_exx[_mod],"unique_exx"<<_mod);
            PRINTDIFF("COMAP::getCALL_OSET_XMain end")
            return tmp;
        }

    }else {
        DEBINF("COMAP::getCALL_OSET not found", _callId_X << "]["<<_mod)
    }
    RELLOCK(&unique_exx[_mod],"unique_exx"<<_mod);
    PRINTDIFF("COMAP::getCALL_OSET_XMain end")
    return tmp;
}
//**********************************************************************************
//**********************************************************************************
CALL_OSET* COMAP::getCALL_OSET_YDerived(string _callId_Y, int _mod){

    PROFILE("COMAP::getCALL_OSET_YDerived")
    TIMEDEF
    SETNOW

    DEBINF("COMAP::getCALL_OSET_YSecond retrieving using derived", _callId_Y << " comap ["<<_mod)

    if (_mod >= COMAPS){
        DEBASSERT("invalid comap index")
    }

    CALL_OSET* tmp = 0x0;
    string tmp2 = "";
    map<string, CALL_OSET*>::iterator p;
    map<string, string>::iterator p2;

    GETLOCK(&unique_exx[_mod],"unique_exx"<<_mod);

    p2 = call_id_y2x[_mod].find(_callId_Y);

    if (p2 != call_id_y2x[_mod].end()){
        tmp2 = (string)p2->second;
        DEBINF("COMAP::getCALL_OSET Y-X found ", tmp2)
        p = comap_mm[_mod].find(tmp2);
        if (p != comap_mm[_mod].end()){
            tmp = (CALL_OSET*)p->second;

            DEBINF("COMAP::getCALL_OSET found ", tmp)

            int tmpDoa = getDoa(tmp,_mod);
            
            if ( tmpDoa == DOA_REQUESTED){
                resetDoaRequestTimer(tmp,_mod);
            }
            if ( tmpDoa == DOA_DELETED){
                DEBINF("COMAP::getCALL_OSET found but in DOA_DELETED", tmp)
                tmp = 0x0;
            }else{
                // all other cases the call:oset will be used, lock it now
                DEBINF("CALL_OSET ACCESS COMAP::getCALL_OSET_YDerived", tmp)
            }

        }else {
            DEBINF("COMAP::getCALL_OSET Y-X not found", _callId_Y)
        }
    }

    //Consistency check
    if (tmp != 0x0){
        map<string, string>::iterator p3;
        p3 = call_id_x2y[_mod].find(tmp2);
        if (p3 ==call_id_x2y[_mod].end()){
            DEBOUT("COMAP::getCALL_OSET_YDerived inconsystency in call_id_x2y",tmp2)
            DEBASSERT("COMAP::getCALL_OSET_YDerived inconsystency in call_id_x2y")
        }
    }

    RELLOCK(&unique_exx[_mod],"unique_exx"<<_mod);
    DEBINF("COMAP::getCALL_OSET_YSecond retrieved using derived", _callId_Y << " comap ["<<_mod <<"][" << tmp)
    PRINTDIFF("COMAP::getCALL_OSET_YDerived end")
    return tmp; //0x0

}
//**********************************************************************************
//**********************************************************************************
void COMAP::setCALL_OSET(string _callId_X, CALL_OSET* _call_oset, int _mod){

    PROFILE("COMAP::setCALL_OSET")
    TIMEDEF
    SETNOW

    DEBINF("COMAP::setCALL_OSET inserting ", _callId_X << "] [" << _call_oset << " comap [" << _mod)

    //need to look for call_oset in call_oset_doa_state
    //need to look for call_id_x in comap_mm
    //need to look for call_id_x in call_id_y2x
    //need to look for call_oset in call_oset_msg_cnt
    //in case it finds any in any of the four maps and the call_oset is not in the DOA_DELETED
    //then send assert...

    if (_mod >= COMAPS){
        DEBASSERT("invalid comap index")
    }

    GETLOCK(&unique_exx[_mod],"unique_exx"<<_mod);

    map<CALL_OSET*,int>::iterator p_doamap;
    map<string, CALL_OSET*>::iterator p_comap_mm;
    map<string,string>::iterator p_cally2x;
    map<string,string>::iterator p_callx2y;
    map<CALL_OSET*, lli>::iterator p_ttl, p_ttldel;

    //doa
    p_doamap = call_oset_doa_state[_mod].find(_call_oset);
    p_ttl = call_oset_ttl[_mod].find(_call_oset);
    p_ttldel = call_oset_ttl_delete[_mod].find(_call_oset);

    if (p_doamap != call_oset_doa_state[_mod].end()){
        if ((int)(p_doamap->second) != DOA_DELETED) {
            //???
            RELLOCK(&unique_exx[_mod],"unique_exx"<<_mod);
            DEBASSERT("COMAP::setCALL_OSET invalid call_oset")
            PRINTDIFF("COMAP::setCALL_OSET end")
            return;
        }else {
            call_oset_doa_state[_mod].erase(p_doamap);
            call_oset_ttl[_mod].erase(p_ttl);
            call_oset_ttl_delete[_mod].erase(p_ttldel);

        }
    }
    call_oset_doa_state[_mod].insert(pair<CALL_OSET*, int>(_call_oset, NOT_DOA));
    call_oset_ttl[_mod].insert(pair<CALL_OSET*, lli>(_call_oset, 0));
    call_oset_ttl_delete[_mod].insert(pair<CALL_OSET*, lli>(_call_oset, 0));

    //comap
    p_comap_mm = comap_mm[_mod].find(_callId_X);
    if (p_comap_mm != comap_mm[_mod].end()){
        comap_mm[_mod].erase(p_comap_mm);
    }
    comap_mm[_mod].insert(pair<string, CALL_OSET*>(_callId_X, _call_oset));


    //Look for any y associated
    p_callx2y = call_id_x2y[_mod].find(_callId_X);
    if (p_callx2y != call_id_x2y[_mod].end()){
        call_id_y2x[_mod].erase(p_cally2x->second);
        call_id_x2y[_mod].erase(p_cally2x);
    }


    RELLOCK(&unique_exx[_mod],"unique_exx"<<_mod);
    PRINTDIFF("COMAP::setCALL_OSET end")
    return;
}
//**********************************************************************************
//**********************************************************************************
void COMAP::setY2XCallId(string _callId_Y, string _callId_X, int _mod){

    PROFILE("COMAP::setY2XCallId")
    TIMEDEF
    SETNOW

    DEBINF("COMAP::setY2XCallId inserting ", _callId_Y << " " << _callId_X << " " <<_mod)
    if (_mod >= COMAPS){
            DEBASSERT("invalid comap index")
    }

    map<string,string>::iterator p_cally2x;
    map<string,string>::iterator p_callx2y;

    GETLOCK(&unique_exx[_mod],"unique_exx"<<_mod);
    p_cally2x = call_id_y2x[_mod].find(_callId_Y);
    if (p_cally2x != call_id_y2x[_mod].end()){
        call_id_y2x[_mod].erase(p_cally2x);
    }
    call_id_y2x[_mod].insert(pair<string, string>(_callId_Y, _callId_X));

    p_callx2y = call_id_x2y[_mod].find(_callId_X);
    if (p_callx2y != call_id_x2y[_mod].end()){
        call_id_x2y[_mod].erase(p_callx2y);
    }
    call_id_x2y[_mod].insert(pair<string, string>(_callId_X, _callId_Y));


    RELLOCK(&unique_exx[_mod],"unique_exx"<<_mod);

    PRINTDIFF("COMAP::setY2XCallId end")

}
//**********************************************************************************
//**********************************************************************************
void COMAP::setDoa(CALL_OSET* _call_oset, int _doa, int _mod){

    PROFILE("COMAP::setDoa")
    TIMEDEF
    SETNOW

    DEBINF("COMAP::setDoa",_call_oset << "] ["<<_doa )

    if (_mod >= COMAPS){
            DEBASSERT("invalid comap index "<<_mod)
    }

    map<CALL_OSET*, int>::iterator p;

    p = call_oset_doa_state[_mod].find(_call_oset);
    if (p != call_oset_doa_state[_mod].end()){
        call_oset_doa_state[_mod].erase(p);
        call_oset_doa_state[_mod].insert(pair<CALL_OSET*, int>(_call_oset,_doa));
    }else {
        call_oset_doa_state[_mod].insert(pair<CALL_OSET*, int>(_call_oset,_doa));
    }
    if (_doa == DOA_REQUESTED) {
        resetDoaRequestTimer(_call_oset, _mod);
    }

    PRINTDIFF("COMAP::setDoa end")
}
int COMAP::getDoa(CALL_OSET* _call_oset, int _mod){

    PROFILE("COMAP::getDoa")
    TIMEDEF
    SETNOW

    DEBINF("COMAP::getDoa",_call_oset << "] ["<<_mod )

    if (_mod >= COMAPS){
        DEBASSERT("invalid comap index "<<_mod)
    }

    map<CALL_OSET*, int>::iterator p;
    int tmp;
    p = call_oset_doa_state[_mod].find(_call_oset);
    if (p!=	call_oset_doa_state[_mod].end()){
        tmp = (int)p->second;
    }else {
        tmp = DOA_DELETED;
    }
    DEBINF("COMAP::getDoa returns",_call_oset << "] ["<<_mod <<"][" << tmp )
    PRINTDIFF("COMAP::getDoa end")
    return tmp;
}
//**********************************************************************************
//**********************************************************************************
int COMAP::use_CALL_OSET_SL_CO_call(CALL_OSET* _call_oset, MESSAGE* _message, int _mod){

    PROFILE("COMAP::use_CALL_OSET_SL_CO_call")
    TIMEDEF
    SETNOW

    DEBINF("COMAP::use_CALL_OSET_SL_CO_call", _call_oset << "] [" << _message->getKey() << "] comap [" << _mod)

    if (_mod >= COMAPS){
        DEBASSERT("invalid comap index "<<_mod)
    }

    GETLOCK(&unique_exx[_mod],"unique_exx"<<_mod);

    int tmpDoa = getDoa(_call_oset, _mod);
    //Check the call_oset doa
    if ( tmpDoa == DOA_DELETED) {
        DEBINF("COMAP::use_CALL_OSET_SL_CO_call rejected call_oset doa deleted", _call_oset )
        RELLOCK(&unique_exx[_mod],"unique_exxx"<<_mod);
        PRINTDIFF("COMAP::use_CALL_OSET_SL_CO_call end")
        return -1;
    }
    //if (getDoa(_call_oset,_mod) == DOA_REQUESTED  && _message->getGenEntity() == SODE_NTWPOINT) {
    if ( tmpDoa == DOA_REQUESTED) {
        resetDoaRequestTimer(_call_oset,_mod);
    }

    DEBINF("COMAP::use_CALL_OSET_SL_CO_call accepted", _call_oset )

    RELLOCK(&unique_exx[_mod],"unique_exx"<<_mod);
    DEBINF("COMAP::use_CALL_OSET_SL_CO_call invoking call for message", _message)
    _call_oset->call(_message);
    PRINTDIFF("COMAP::use_CALL_OSET_SL_CO_call end")
    return 0;
}
//**********************************************************************************
//**********************************************************************************
void COMAP::setDoaRequested(CALL_OSET* _call_oset, int _mod) {

    PROFILE("COMAP::setDoaRequested")
    TIMEDEF
    SETNOW

    DEBINF("COMAP::setDoaRequested", _call_oset << " comap ["<< _mod)

    if (_mod >= COMAPS){
        DEBASSERT("invalid comap index "<<_mod)
    }

    GETLOCK(&unique_exx[_mod],"unique_ex"<<_mod);

    resetDoaRequestTimer(_call_oset,_mod);

    if (getDoa(_call_oset,_mod) == DOA_DELETED || getDoa(_call_oset,_mod) == DOA_REQUESTED) {
        DEBINF("COMAP::setDoaRequested already deleted (-1) or confirmed (2)", getDoa(_call_oset,_mod))
        //Reset delete timer

        RELLOCK(&unique_exx[_mod],"unique_exx"<<_mod)
        PRINTDIFF("COMAP::setDoaRequested end")
        return;

    }else {
        DEBINF("COMAP::setDoaRequested to DOA_REQUESTED (1)", DOA_REQUESTED)
        setDoa(_call_oset, DOA_REQUESTED, _mod);
    }
    RELLOCK(&unique_exx[_mod],"unique_exx"<<_mod)
    PRINTDIFF("COMAP::setDoaRequested end")
    return;

}
void COMAP::purgeDOA(void){


    PROFILE("COMAP::purgeDOA")
    TIMEDEF
    SETNOW

    DEBINF("COMAP::purgeDOA started",0)

    int mod;

    for ( mod=0; mod< COMAPS; mod++){
        map<string, CALL_OSET*>::iterator p_comap_mm;
        CALL_OSET* call_oset;
        stack<string> todel_cx;

        if (mod >= COMAPS){
            DEBASSERT("invalid comap index "<<mod)
        }
        int trylok;
        TRYLOCK(&unique_exx[mod]," purgeDOA unique_exx"<<mod, trylok)
        if(trylok != 0){
            DEBINF("COMAP::purgeDOA trylock failed", mod)
            loktry[mod]++;
            if (loktry[mod] > 10){
                DEBINF("COMAP::purgeDOA trylock failed more than 10 times", mod)
                DEBASSERT("COMAP::purgeDOA trylock failed")
            }else{
                continue;
            }
        }else {
            loktry[mod] = 0;
        }

        DEBMEM("COMAP::purgeDOA comap entries",comap_mm[mod].size())
        for( p_comap_mm = comap_mm[mod].begin(); p_comap_mm != comap_mm[mod].end() ; ++p_comap_mm){
            call_oset = (CALL_OSET*)p_comap_mm->second;

            DEBINF("purgeDOA CALL_OSET::SL_CO::mutex getting", call_oset)

            string tmps;
            if (call_oset->getOverallState_CL() == OS_COMPLETED && call_oset->getOverallState_SV()==OS_CONFIRMED){
                tmps = "CALL ESTABLISHED";
            }else if (call_oset->getOverallState_CL() == OS_TERMINATED && call_oset->getOverallState_SV()==OS_TERMINATED){
                tmps = "CLOSED CALL";
            }else {
                tmps = "CALL TEMPORARY STATE";
            }

            DEBINF("COMAP::purgeDOA", call_oset << "] DOA state ["<<getDoa(call_oset,mod) <<"]["<<tmps)
            if ( getDoa(call_oset,mod) == DOA_REQUESTED){

                //check time
                SysTime afterT;
                GETTIME(afterT);
                lli now = ((lli) afterT.tv.tv_sec)*1000000+(lli)afterT.tv.tv_usec;
                map<CALL_OSET*, lli>::iterator p_ttl;
                p_ttl = call_oset_ttl[mod].find(call_oset);
                if (p_ttl == call_oset_ttl[mod].end()){
                        DEBASSERT("call_oset_ttl inexistent")
                }
                if (p_ttl->second < now){

                    //TODO check here the timer if expired delete

                    DEBINF("COMAP::purgeDOA time expired", call_oset)
                    setDoa(call_oset, DOA_DELETED,mod);
                    lli killtime = now + TIMER_DOA / 2;
                    DEBINF("COMAP::setDoa ttl will be removed", TIMER_DOA /2 << " " << killtime)
                    call_oset_ttl[mod].erase(p_ttl);
                    call_oset_ttl_delete[mod].insert(pair<CALL_OSET*, lli>(call_oset,killtime));

                } else{
                    DEBINF("COMAP::purgeDOA not time to delete ", call_oset)
                }
            }
            if ( getDoa(call_oset,mod) == DOA_DELETED ){

                //check time
                SysTime afterT;
                GETTIME(afterT);
                lli now = ((lli) afterT.tv.tv_sec)*1000000+(lli)afterT.tv.tv_usec;
                map<CALL_OSET*, lli>::iterator p_ttl_del;
                p_ttl_del = call_oset_ttl_delete[mod].find(call_oset);
                if (p_ttl_del == call_oset_ttl_delete[mod].end()){
                        DEBASSERT("call_oset_ttl insistent")
                }
                if (p_ttl_del->second < now){

                    //check if the CALL_OSET::SL_CO is locked

                    todel_cx.push((string)p_comap_mm->first);

                    call_oset_doa_state[mod].erase(call_oset);

                    call_id_y2x[mod].erase(call_oset->getCallId_Y());
                    call_id_x2y[mod].erase(call_oset->getCallId_X());

//					RELLOCK(&(call_oset->getSL_CO()->mutex),"purgeDOA CALL_OSET::SL_CO::mutex")
                    DEBOUT("purgeDOA CALL_OSET::SL_CO::mutex releasing", call_oset)

                    DELPTR(call_oset,"CALL_OSET");
                    call_oset = 0x0;
                    //cant relase here... would core

                }
            }
            else{
                //Nothing
            }
            if (call_oset != 0x0){
            }
        }
        string tops;
        while (!todel_cx.empty()){
            tops = todel_cx.top();
            DEBINF("COMAP::purgeDOA todel_cx", tops)
            comap_mm[mod].erase(tops);
            todel_cx.pop();
        }
        RELLOCK(&unique_exx[mod]," purgeDOA unique_exx"<<mod);
    }
    PRINTDIFF("COMAP::purgeDOA end")
}

void COMAP::resetDoaRequestTimer(CALL_OSET* _call_oset,int _modulus){

    PROFILE("COMAP::resetDoaRequestTimer")
    TIMEDEF
    SETNOW

    //Reset delete timer
    map<CALL_OSET*, lli>::iterator p_ttl;
    p_ttl = call_oset_ttl[_modulus].find(_call_oset);
    if (p_ttl == call_oset_ttl[_modulus].end()){
        DEBASSERT("call_oset_ttl insistent")
    }
    SysTime afterT;
    GETTIME(afterT);
    lli killtime = ((lli) afterT.tv.tv_sec)*1000000+(lli)afterT.tv.tv_usec + TIMER_DOA;
    DEBINF("COMAP::setDoa ttl ", TIMER_DOA  << " " << killtime)
    call_oset_ttl[_modulus].erase(p_ttl);
    call_oset_ttl[_modulus].insert(pair<CALL_OSET*, lli>(_call_oset,killtime));
    PRINTDIFF("COMAP::resetDoaRequestTimer")
}
