//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Core And Service Layer
// Copyright (C) 2010 Guglielmo Incisa di Camerana
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
#ifndef ENGINE_H
#include "ENGINE.h"
#endif
#ifndef DOA_H
#include "DOA.h"
#endif

//**********************************************************************************
//**********************************************************************************
DOA::DOA(int _i):ENGINE(_i) {
}

void DOA::setComap(COMAP* _comap){
	comap = _comap;
}

void DOA::parse(MESSAGE* _mess) {

	DEBMESSAGE("DOA::parse", _mess)

	pthread_mutex_unlock(&(sb.condvarmutex));

	CALL_OSET* call_oset = 0x0;
	DEBY
	call_oset = comap->getCALL_OSET_XMain(_mess->getHeadCallId().getContent());
	DEBY
	if (call_oset == 0x0){
		call_oset = comap->getCALL_OSET_YDerived(_mess->getHeadCallId().getContent());
		while (comap->getCALL_OSETStatus(call_oset) > 0){
			DEBY
			WAITTIME
		}
		comap->deleteYCALL_OSET(_mess->getHeadCallId().getContent());
		DEBY
	}else{
		while (comap->getCALL_OSETStatus(call_oset) > 0){
			DEBY
			WAITTIME
		}
	}
	DEBY
	call_oset->setDoa();

	string key;
	static multimap<const string, MESSAGE *> ::iterator p;
	while(!call_oset->messageKeys.empty()){
		key = (string)(call_oset->messageKeys.top());
		p = globalMessTable.find(key);
		if (p != globalMessTable.end()){
			delete ((MESSAGE*)p->second);
			pthread_mutex_lock(&messTableMtx);
			globalMessTable.erase(p);
			pthread_mutex_unlock(&messTableMtx);
		}
		call_oset->messageKeys.pop();
	}
	if (call_oset != 0x0){
		DEBY
		delete call_oset;
	}

	// delete message in the stack
}
