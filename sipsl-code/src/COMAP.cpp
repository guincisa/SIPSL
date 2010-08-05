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
COMAP::COMAP(void){
	pthread_mutex_init(&cos_mutex, NULL);
}
COMAP::~COMAP(void){
}
int COMAP::getCALL_OSETStatus(CALL_OSET* _call_oset){

	pthread_mutex_lock(&cos_mutex);

	map<CALL_OSET*, int>::iterator p;
	p = call_oset_status.find(_call_oset);
	if (p != call_oset_status.end()){
		int i = (int)p->second;
		pthread_mutex_unlock(&cos_mutex);
		return i;
	}else{
		//status has not been set so by default is busy
		pthread_mutex_unlock(&cos_mutex);
		return 9999;
	}
}
void COMAP::setCALL_OSETStatus(CALL_OSET* _call_oset, int _status){

	pthread_mutex_lock(&cos_mutex);

	map<CALL_OSET*, int>::iterator p;
	p = call_oset_status.find(_call_oset);
	if (p != call_oset_status.end()){
		call_oset_status.erase(p);
	}
	call_oset_status.insert(pair<CALL_OSET*, int >(_call_oset, _status));
	pthread_mutex_unlock(&cos_mutex);
}
void COMAP::useCALL_OSET_SL_CO_call(CALL_OSET* _call_oset, MESSAGE* _message){

	//check if not doa
	//if doa return()

	pthread_mutex_lock(&cos_mutex);
	//busy call_oset + 1
	pthread_mutex_unlock(&cos_mutex);
	//else
	_call_oset->getSL_CO()->call(_message);
	pthread_mutex_lock(&cos_mutex);
	//busy call_oset + 1
	pthread_mutex_unlock(&cos_mutex);



}
//**********************************************************************************
//**********************************************************************************
CALL_OSET* COMAP::getCALL_OSET_XMain(string _callId_X){

	DEBOUT_UTIL("COMAP::getCALL_OSET_XMain retrieving using call id", _callId_X)

	CALL_OSET* tmp = 0x0;
	map<string, CALL_OSET*>::iterator p;
	p = comap_mm.find(_callId_X);
	if (p != comap_mm.end()){
			tmp = (CALL_OSET*)p->second;
			DEBOUT_UTIL("COMAP::getCALL_OSET found ", tmp)
	}else {
		DEBOUT_UTIL("COMAP::getCALL_OSET not found", "")
	}
	//else return 0x0
	return tmp;
}
//**********************************************************************************
//**********************************************************************************
CALL_OSET* COMAP::getCALL_OSET_YDerived(string _callId_Y){

	DEBOUT("COMAP::getCALL_OSET_YSecond retrieving using derived", _callId_Y)

	CALL_OSET* tmp = 0x0;
	string tmp2 = "";
	map<string, CALL_OSET*>::iterator p;
	map<string, string>::iterator p2;
	p2 = call_id_y2x.find(_callId_Y);

	if (p2 != call_id_y2x.end()){
		tmp2 = (string)p2->second;
		DEBOUT("COMAP::getCALL_OSET Y-X found ", tmp2)
		p = comap_mm.find(tmp2);
		if (p != comap_mm.end()){
				tmp = (CALL_OSET*)p->second;
				DEBOUT("COMAP::getCALL_OSET Y-X found ", tmp)

				return tmp;
		}
	}
	DEBOUT("COMAP::getCALL_OSET Y-X not found", "")
	return tmp; //0x0

}
//**********************************************************************************
//**********************************************************************************
void COMAP::setCALL_OSET(string _callId_X, CALL_OSET* _call_oset){

	DEBOUT("COMAP::setCALL_OSET inserting ", _callId_X)

	pthread_mutex_lock(&cos_mutex);

	call_oset_status.insert(pair<CALL_OSET*, int >(_call_oset, 9999));

	pthread_mutex_unlock(&cos_mutex);

	comap_mm.insert(pair<string, CALL_OSET* >(_callId_X, _call_oset));

}
//**********************************************************************************
//**********************************************************************************
void COMAP::setY2XCallId(string _callId_Y, string _callId_X){

	DEBOUT("COMAP::setY2XCallId inserting ", _callId_Y + " " + _callId_X)

	call_id_y2x.insert(pair<string, string>(_callId_Y, _callId_X));

}
//**********************************************************************************
//**********************************************************************************
void COMAP::deleteCALL_OSET(string _callId_X){

	DEBOUT("COMAP::deleteCALL_OSET remove ", _callId_X)

	CALL_OSET* tmp = 0x0;
	map<string, CALL_OSET*>::iterator p;
	p = comap_mm.find(_callId_X);
	if (p != comap_mm.end()){
			comap_mm.erase(p);
	}
}
//**********************************************************************************
//**********************************************************************************
void COMAP::deleteYCALL_OSET(string _callId_Y){

	DEBOUT("COMAP::delete/CALL_OSET remove ", _callId_Y)

	map<string, CALL_OSET*>::iterator p;
	map<string, string>::iterator p2;
	string tmp2 = "";

	p2 = call_id_y2x.find(_callId_Y);

	if (p2 != call_id_y2x.end()){
		tmp2 = (string)p2->second;
		DEBOUT("COMAP::getCALL_OSET Y-X found ", tmp2)
		p = comap_mm.find(tmp2);
		if (p != comap_mm.end()){
			comap_mm.erase(p);
			call_id_y2x.erase(p2);
		}
	}
}

