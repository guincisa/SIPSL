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

//**********************************************************************************
//**********************************************************************************
COMAP::COMAP(void){
	pthread_mutex_init(&doa_mutex, NULL);
	pthread_mutex_init(&comap_mutex, NULL);
	pthread_mutex_init(&call_y2x_mutex, NULL);
	pthread_mutex_init(&co_msgcnt_mutex, NULL);

	DEBDEV("comap comap_mm", &comap_mm)
	DEBDEV("comap call_id_y2x",&call_id_y2x)
	DEBDEV("comap call_oset_doa_state", &call_oset_doa_state)
	DEBDEV("comap call_oset_msg_cnt", &call_oset_msg_cnt)

}
COMAP::~COMAP(void){
}
//**********************************************************************************
//**********************************************************************************
CALL_OSET* COMAP::getCALL_OSET_XMain(string _callId_X){

	DEBINF("COMAP::getCALL_OSET_XMain retrieving using call id", _callId_X)

	CALL_OSET* tmp = 0x0;
	map<string, CALL_OSET*>::iterator p;

	GETLOCK(&comap_mutex,"comap_mutex");
	p = comap_mm.find(_callId_X);
	if (p != comap_mm.end()){
			tmp = (CALL_OSET*)p->second;

			DEBINF("COMAP::getCALL_OSET found ", tmp)
	}else {
		DEBINF("COMAP::getCALL_OSET not found", "")
	}
	RELLOCK(&comap_mutex,"comap_mutex");
	return tmp;
}
//**********************************************************************************
//**********************************************************************************
CALL_OSET* COMAP::getCALL_OSET_YDerived(string _callId_Y){

	DEBINF("COMAP::getCALL_OSET_YSecond retrieving using derived", _callId_Y)

	CALL_OSET* tmp = 0x0;
	string tmp2 = "";
	map<string, CALL_OSET*>::iterator p;
	map<string, string>::iterator p2;

	GETLOCK(&call_y2x_mutex,"call_y2x_mutex");
	p2 = call_id_y2x.find(_callId_Y);

	if (p2 != call_id_y2x.end()){
		tmp2 = (string)p2->second;
		DEBINF("COMAP::getCALL_OSET Y-X found ", tmp2)
		GETLOCK(&comap_mutex,"comap_mutex");
		p = comap_mm.find(tmp2);
		if (p != comap_mm.end()){
				tmp = (CALL_OSET*)p->second;
				RELLOCK(&comap_mutex,"comap_mutex");
				RELLOCK(&call_y2x_mutex,"call_y2x_mutex");
				DEBINF("COMAP::getCALL_OSET Y-X found ", tmp)
				return tmp;
		}
		RELLOCK(&comap_mutex,"comap_mutex");
	}
	RELLOCK(&call_y2x_mutex,"call_y2x_mutex");
	DEBINF("COMAP::getCALL_OSET Y-X not found", _callId_Y)
	return tmp; //0x0

}
//**********************************************************************************
//**********************************************************************************
void COMAP::setCALL_OSET(string _callId_X, CALL_OSET* _call_oset){

	DEBINF("COMAP::setCALL_OSET inserting ", _callId_X << "] [" << _call_oset)

	//need to look for call_oset in call_oset_doa_state
	//need to look for call_id_x in comap_mm
	//need to look for call_id_x in call_id_y2x
	//need to look for call_oset in call_oset_msg_cnt
	//in case it finds any in any of the four maps and the call_oset is not in the DOA_DELETED
	//then send assert...

	map<CALL_OSET*,int>::iterator p_doamap;
	map<string, CALL_OSET*>::iterator p_comap_mm;
	map<string,string>::iterator p_cally2x;
	map<CALL_OSET*,int>::iterator p_msgcnt;

	//doa
	int doa_state;
	GETLOCK(&doa_mutex,"doa_mutex");
	p_doamap = call_oset_doa_state.find(_call_oset);
	if (p_doamap != call_oset_doa_state.end()){
		if ((int)(p_doamap->second) != DOA_DELETED) {
			RELLOCK(&doa_mutex,"doa_mutex");
			DEBY
			DEBASSERT("COMAP::setCALL_OSET invalid call_oset")
			return;
		}else {
			DEBY
			call_oset_doa_state.erase(p_doamap);
		}
	}
	call_oset_doa_state.insert(pair<CALL_OSET*, int>(_call_oset, NOT_DOA));
	RELLOCK(&doa_mutex,"doa_mutex");

	//comap
	GETLOCK(&comap_mutex,"comap_mutex");
	p_comap_mm = comap_mm.find(_callId_X);
	if (p_comap_mm != comap_mm.end()){
		DEBY
		comap_mm.erase(p_comap_mm);
	}
	comap_mm.insert(pair<string, CALL_OSET*>(_callId_X, _call_oset));
	RELLOCK(&comap_mutex,"comap_mutex");

	//call_y2x just delete it
	GETLOCK(&call_y2x_mutex,"call_y2x_mutex");
	p_cally2x = call_id_y2x.find(_callId_X);
	if (p_cally2x != call_id_y2x.end()){
		DEBY
		call_id_y2x.erase(p_cally2x);
	}
	RELLOCK(&call_y2x_mutex,"call_y2x_mutex");

	//call_oset message counter
	GETLOCK(&co_msgcnt_mutex,"co_msgcnt_mutex");
	p_msgcnt = call_oset_msg_cnt.find(_call_oset);
	if (p_msgcnt != call_oset_msg_cnt.end()){
		DEBY
		call_oset_msg_cnt.erase(p_msgcnt);
	}
	call_oset_msg_cnt.insert(pair<CALL_OSET*, int>(_call_oset, 0));
	RELLOCK(&co_msgcnt_mutex,"co_msgcnt_mutex");

	DEBY
	return;
}
//**********************************************************************************
//**********************************************************************************
void COMAP::setY2XCallId(string _callId_Y, string _callId_X){

	DEBINF("COMAP::setY2XCallId inserting ", _callId_Y + " " + _callId_X)
	map<string,string>::iterator p_cally2x;

	GETLOCK(&call_y2x_mutex,"call_y2x_mutex");
	p_cally2x = call_id_y2x.find(_callId_Y);
	if (p_cally2x != call_id_y2x.end()){
		DEBY
		call_id_y2x.erase(p_cally2x);
	}
	call_id_y2x.insert(pair<string, string>(_callId_Y, _callId_X));
	RELLOCK(&call_y2x_mutex,"call_y2x_mutex");

}
//**********************************************************************************
//**********************************************************************************
void COMAP::setDoa(CALL_OSET* _call_oset, int _doa){

	DEBY
	map<CALL_OSET*, int>::iterator p;
	GETLOCK(&doa_mutex,"doa_mutex");
	p = call_oset_doa_state.find(_call_oset);
	if (p != call_oset_doa_state.end()){
		call_oset_doa_state.erase(p);
		call_oset_doa_state.insert(pair<CALL_OSET*, int>(_call_oset,_doa));
	}else {
		call_oset_doa_state.insert(pair<CALL_OSET*, int>(_call_oset,_doa));
	}
	RELLOCK(&doa_mutex,"doa_mutex");

}
int COMAP::getDoa(CALL_OSET* _call_oset){

	map<CALL_OSET*, int>::iterator p;
	int tmp;
	GETLOCK(&doa_mutex,"doa_mutex");
	p = call_oset_doa_state.find(_call_oset);
	if (p!=	call_oset_doa_state.end()){
		tmp = (int)p->second;
	}else {
		tmp = DOA_DELETED;
	}
	RELLOCK(&doa_mutex,"doa_mutex");
	return tmp;
}
//**********************************************************************************
//**********************************************************************************
int COMAP::getCALL_OSET_MsgCnt(CALL_OSET* _call_oset){

	//call_oset message counter
	int i;
	map<CALL_OSET*,int>::iterator p_msgcnt;

	GETLOCK(&co_msgcnt_mutex,"co_msgcnt_mutex");
	p_msgcnt = call_oset_msg_cnt.find(_call_oset);
	if (p_msgcnt != call_oset_msg_cnt.end()){
		DEBY
		i = (int)p_msgcnt->second;
	}else {
		DEBY
		i = -1;
	}
	RELLOCK(&co_msgcnt_mutex,"co_msgcnt_mutex");
	return i;

}
//**********************************************************************************
//**********************************************************************************
void COMAP::incCALL_OSET_MsgCnt(CALL_OSET* _call_oset){

	//call_oset message counter
	int i;
	map<CALL_OSET*,int>::iterator p_msgcnt;

	GETLOCK(&co_msgcnt_mutex,"co_msgcnt_mutex");
	p_msgcnt = call_oset_msg_cnt.find(_call_oset);
	if (p_msgcnt != call_oset_msg_cnt.end()){
		DEBY
		i = (int)p_msgcnt->second;
		i++;
		call_oset_msg_cnt.erase(p_msgcnt);
		call_oset_msg_cnt.insert(pair<CALL_OSET*, int>(_call_oset, i));
		DEBY
	}else {
		DEBY
	}
	RELLOCK(&co_msgcnt_mutex,"co_msgcnt_mutex");
	return;

}
//**********************************************************************************
//**********************************************************************************
void COMAP::decCALL_OSET_MsgCnt(CALL_OSET* _call_oset){

	DEBOUT("COMAP::decCALL_OSET_MsgCnt", _call_oset)

	//call_oset message counter
	int i;
	map<CALL_OSET*,int>::iterator p_msgcnt;

	GETLOCK(&comap_mutex,"comap_mutex");

	GETLOCK(&co_msgcnt_mutex,"co_msgcnt_mutex");
	p_msgcnt = call_oset_msg_cnt.find(_call_oset);
	if (p_msgcnt != call_oset_msg_cnt.end()){
		DEBY
		i = (int)p_msgcnt->second;
		i--;
		DEBOUT("COMAP::decCALL_OSET_MsgCnt new value", _call_oset << "] [" << i)
		call_oset_msg_cnt.erase(p_msgcnt);
		call_oset_msg_cnt.insert(pair<CALL_OSET*, int>(_call_oset, i));
		DEBY
	}else {
		DEBY
	}

	//if no messages inside and in doa_requested then switch to doa_confirmed
	if (i == 0 && getDoa(_call_oset)==DOA_REQUESTED){
		DEBOUT("COMAP::decCALL_OSET_MsgCnt DOA_CONFIRMED", _call_oset )
		setDoa(_call_oset, DOA_CONFIRMED);

		RELLOCK(&co_msgcnt_mutex,"co_msgcnt_mutex");

		DEBOUT("COMAP::decCALL_OSET_MsgCnt deleting here!", _call_oset)
		setDoa(_call_oset, DOA_DELETED);
		DEBY

		GETLOCK(&doa_mutex,"doa_mutex");
		DEBY
		call_oset_doa_state.erase(_call_oset);
		RELLOCK(&doa_mutex,"doa_mutex");

		GETLOCK(&call_y2x_mutex,"call_y2x_mutex");
		DEBY
		call_id_y2x.erase(_call_oset->getCallId_Y());
		RELLOCK(&call_y2x_mutex,"call_y2x_mutex");

		GETLOCK(&co_msgcnt_mutex,"co_msgcnt_mutex");
		DEBY
		call_oset_msg_cnt.erase(_call_oset);
		RELLOCK(&co_msgcnt_mutex,"co_msgcnt_mutex");

		comap_mm.erase(_call_oset->callId_X);
		DELPTR(_call_oset,"call_oset");
		RELLOCK(&comap_mutex,"comap_mutex");

		return;

	}

	RELLOCK(&co_msgcnt_mutex,"co_msgcnt_mutex");
	RELLOCK(&comap_mutex,"comap_mutex");

	return;

}
//**********************************************************************************
//**********************************************************************************
int COMAP::use_CALL_OSET_SL_CO_call(CALL_OSET* _call_oset, MESSAGE* _message){

	DEBOUT("COMAP::use_CALL_OSET_SL_CO_call", _call_oset << "] [" << _message->getKey())

	//Check the call_oset doa
	if (getDoa(_call_oset) == DOA_DELETED || getDoa(_call_oset) == DOA_CONFIRMED) {
		DEBOUT("COMAP::use_CALL_OSET_SL_CO_call rejected call_oset doa deleted", _call_oset )
		_message->setDestEntity(SODE_KILL);
		return -1;
	}
	if (getDoa(_call_oset) == DOA_REQUESTED && _message->getGenEntity() == SODE_NTWPOINT) {
		DEBOUT("COMAP::use_CALL_OSET_SL_CO_call rejected call_oset doa_requested", _call_oset )
		_message->setDestEntity(SODE_KILL);
		return -1;
	}
	if (getDoa(_call_oset) == NOT_DOA){
		DEBOUT("COMAP::use_CALL_OSET_SL_CO_call accepted", _call_oset )
		incCALL_OSET_MsgCnt(_call_oset);
		DEBY
		_call_oset->getSL_CO()->call(_message);
		DEBY
		decCALL_OSET_MsgCnt(_call_oset);
		DEBY
		return 0;
	}
	return -1;
}
//**********************************************************************************
//**********************************************************************************
void COMAP::setDoaRequested(CALL_OSET* _call_oset) {

	DEBOUT("COMAP::setDoaRequested", _call_oset)
	if (getDoa(_call_oset) == DOA_DELETED || getDoa(_call_oset) == DOA_CONFIRMED) {
		DEBY
	}else {
		setDoa(_call_oset, DOA_REQUESTED);
	}

}
void COMAP::purgeDOA(void){

	map<string, CALL_OSET*>::iterator p_comap_mm;
	CALL_OSET* call_oset;
	stack<string> todel_cx;

	//TODO
	// not good... this blocks the comap for too long!
	GETLOCK(&comap_mutex,"comap_mutex");

	for( p_comap_mm = comap_mm.begin(); p_comap_mm != comap_mm.end(); ++p_comap_mm){
		DEBY
		call_oset = (CALL_OSET*)p_comap_mm->second;
		DEBOUT("COMAP::purgeDOA", call_oset)
		if ( getDoa(call_oset) == DOA_CONFIRMED){
			DEBOUT("COMAP::purgeDOA deleted", call_oset)
			setDoa(call_oset, DOA_DELETED);
			DEBY

			todel_cx.push((string)p_comap_mm->first);

			GETLOCK(&doa_mutex,"doa_mutex");
			DEBY
			call_oset_doa_state.erase(call_oset);
			RELLOCK(&doa_mutex,"doa_mutex");

			GETLOCK(&call_y2x_mutex,"call_y2x_mutex");
			DEBY
			call_id_y2x.erase(call_oset->getCallId_Y());
			RELLOCK(&call_y2x_mutex,"call_y2x_mutex");

			GETLOCK(&co_msgcnt_mutex,"co_msgcnt_mutex");
			DEBY
			call_oset_msg_cnt.erase(call_oset);
			RELLOCK(&co_msgcnt_mutex,"co_msgcnt_mutex");

			DELPTR(call_oset,"call_oset");
		}else{
			DEBY
		}
	}
	DEBY
	string tops;
	while (!todel_cx.empty()){
		tops = todel_cx.top();
		DEBOUT("COMAP::purgeDOA todel_cx", tops)
		comap_mm.erase(tops);
		todel_cx.pop();
	}
	RELLOCK(&comap_mutex,"comap_mutex");
	DEBY

}



