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

		//change request
		// INVITE INVITE sip:guic2@127.0.0.1:5061 SIP/2.0
		DEBOUT("VALO ", message->getHeadSipRequest().getContent())
		message->setHeadSipRequest("sipslguic@127.0.0.1:5061", SODE_ALOPOINT);

		//Cseq new to 1
		message->setHeadCSeq("1 INVITE", SODE_ALOPOINT);


		//Via Via: SIP/2.0/TCP 127.0.0.1:5060;branch=z9hG4bKYesTAZxWOfNDtT97ie51tw
		DEBOUT("1","1")

		char viatmp[512];
		sprintf(viatmp, "SIP/2.0/UDP %s:%d;branch=z9hG4bK%s",getSUDP()->getDomain().c_str(),getSUDP()->getPort(),message->getKey().c_str());
		string viatmpS(viatmp);
		message->purgeSTKHeadVia();
		message->pushHeadVia(viatmpS, SODE_ALOPOINT, 0);
		DEBOUT("via",message->getSTKHeadVia().top()->getContent() )

		//From changes
		// in From: <sip:guic@172.21.160.184>;tag=0ac37672-6a86-de11-992a-001d7206fe48
		// out From: <sip:guic@172.21.160.184>;tag=YKcAvQ
		DEBOUT("FROM",message->getHeadFrom().getContent())
		DEBOUT("FROM",message->getHeadFrom().getC_AttSipUri().getContent())
		DEBOUT("FROM",message->getHeadFrom().getNameUri())
		DEBOUT("FROM",message->getHeadFrom().getC_AttUriParms().getContent())
		// change tag
		char fromtmp[512];
		sprintf(fromtmp, "%s;tag=merde",message->getHeadFrom().getC_AttSipUri().getContent().c_str());
		string fromtmpS(viatmp);
		message->setHeadFrom("<sip:cacchio>;tag=ceppen", SODE_ALOPOINT);
		DEBOUT("FROM",message->getHeadFrom().getContent())
		DEBOUT("FROM",message->getHeadFrom().getC_AttSipUri().getContent())
		DEBOUT("FROM",message->getHeadFrom().getNameUri())
		DEBOUT("FROM",message->getHeadFrom().getC_AttUriParms().getContent())



		//Contact: <sip:guic2@172.21.160.184:5062>
		//Contact: <sip:127.0.0.1:5060;transport=tcp>

		DEBOUT("VALO","1")
		// TODO
		int tl = message->getTotLines();
		DEBOUT("VALO::parse tot lines",tl)
		//sl_cc->p_w(message);
		DEBOUT("VALO","2")

}
