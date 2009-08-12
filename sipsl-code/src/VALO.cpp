//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPCSL Sip Core And Service Layer
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


#include <stdio.h>


#ifndef MESSAGE_H
#include "MESSAGE.h"
#endif
#ifndef VALO_H
#include "VALO.h"
#endif

VALO::VALO(ENGINE* _e):ALO(_e){}

void VALO::onInvite(MESSAGE* _message){

	//	// do business logic...
	//	// create b2b invite related message & so on...
	//	//TODO clean this
		char bu[512];
		SysTime inTime;
		GETTIME(inTime);
		MESSAGE* message;
		message = new MESSAGE(_message->getIncBuffer().c_str(), SODE_SMSVPOINT, inTime, _message->getSock(), _message->getSocket());
		sprintf(bu, "%x#%lld",message,inTime.tv.tv_sec*1000000+inTime.tv.tv_usec);
		string key(bu);
		message->setKey(key);
		pthread_mutex_lock(&messTableMtx);
		globalMessTable.insert(pair<string, MESSAGE*>(key, message));
		pthread_mutex_unlock(&messTableMtx);

		DEBOUT("VALO","1")
		// TODO
		int tl = message->getTotLines();
		DEBOUT("VALO::parse tot lines",tl)
		sl_cc->p_w(message);
		DEBOUT("VALO","2")

}
