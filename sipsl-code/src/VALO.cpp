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
		message = new MESSAGE(_message, SODE_ALOPOINT);
		//set by SL_CC message->setDestEntity(SODE_SMCLPOINT);
		sprintf(bu, "%x#%lld",message,inTime.tv.tv_sec*1000000+inTime.tv.tv_usec);
		string key(bu);
		message->setKey(key);
		DEBOUT("NEW MESSAGE",message->getIncBuffer());
		DEBOUT("NEW MESSAGE"," " + message->getTotLines());
		pthread_mutex_lock(&messTableMtx);
		globalMessTable.insert(pair<string, MESSAGE*>(key, message));
		pthread_mutex_unlock(&messTableMtx);

		// REQUEST
		// maybe changed or unchanged
		// ---- unchanged
		// Route
		// remove it
		DEBOUT("VALO message->getHeadRoute().getRoute().getHostName()",message->getHeadRoute().getRoute().getHostName())
		DEBOUT("VALO message->getHeadRoute().getRoute().getPort()",message->getHeadRoute().getRoute().getPort())
		message->removeHeadRoute();

		DEBOUT("VALO","remove route")

		//change request
		// INVITE INVITE sip:guic2@127.0.0.1:5061 SIP/2.0
		DEBOUT("VALO ", message->getHeadSipRequest().getContent())
		message->setHeadSipRequest("INVITE sip:sipslguic@127.0.0.1:5061 SIP/2.0");

		//Cseq new to 1
		message->replaceHeadCSeq("999 INVITE");
		DEBOUT("VALO","Cseq")

		//Via Via: SIP/2.0/TCP 127.0.0.1:5060;branch=z9hG4bKYesTAZxWOfNDtT97ie51tw

		char viatmp[512];
		sprintf(viatmp, "SIP/2.0/UDP %s:%d;branch=z9hG4bK%s",getSUDP()->getDomain().c_str(),getSUDP()->getPort(),message->getKey().c_str());
		string viatmpS(viatmp);
		message->purgeSTKHeadVia();
		message->pushHeadVia(viatmpS);

		//From changes
		// in From: <sip:guic@172.21.160.184>;tag=0ac37672-6a86-de11-992a-001d7206fe48
		// out From: <sip:guic@172.21.160.184>;tag=YKcAvQ
		DEBOUT("FROM",message->getHeadFrom().getContent())
		DEBOUT("FROM",message->getHeadFrom().getC_AttSipUri().getContent())
		DEBOUT("FROM",message->getHeadFrom().getNameUri())
		DEBOUT("FROM",message->getHeadFrom().getC_AttUriParms().getContent())
		// change tag
		char fromtmp[512];
		sprintf(fromtmp, "%s;tag=%s",message->getHeadFrom().getC_AttSipUri().getContent().c_str(),message->getKey().c_str());
		string fromtmpS(fromtmp);
		DEBOUT("******** FROM new" , fromtmpS)
		message->replaceHeadFrom(fromtmpS);
		DEBOUT("FROM",message->getHeadFrom().getContent())
		DEBOUT("FROM",message->getHeadFrom().getC_AttSipUri().getContent())
		DEBOUT("FROM",message->getHeadFrom().getNameUri())
		DEBOUT("FROM",message->getHeadFrom().getC_AttUriParms().getContent())


		DEBOUT("CONTACT", message->getHeadContact().getContent())
		message->replaceHeadContact("<sip:sipsl.gugli.com:5060>");
		DEBOUT("NEW CONTACT", message->getHeadContact().getContent())

		// Compile the message
		message->compileMessage();
		message->dumpVector();
		DEBOUT("New outoging b2b message", message->getIncBuffer())

		sl_cc->p_w(message);

}
