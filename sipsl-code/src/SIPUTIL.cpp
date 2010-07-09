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

// SIPUTIL is a static class to be used for creating messages from other messages

#include <stdio.h>
#include <assert.h>

#ifndef SIPUTIL_H
#include "SIPUTIL.h"
#endif

#ifndef MESSAGE_H
#include "MESSAGE.h"
#endif
#ifndef SUDP_H
#include "SUDP.h"
#endif


void SIPUTIL::genASideReplyFromBReply(MESSAGE* _gtor, MESSAGE* __gtor, MESSAGE* _gted){

	//__gtor is the B reply
	// _gtor is the A Invite
	// _getd reply for A

	//Must define here the to tag
	//only if empty...
	if ( _gted->getHeadTo().getC_AttUriParms().getContent().length() == 0){
		DEBOUT("_gted->getHeadTo().getC_AttUriParms()).getContent()",_gted->getHeadTo().getC_AttUriParms().getContent())
		char totmp[512];
		if (__gtor->getHeadTo().getNameUri().length()!= 0){
			sprintf(totmp, "%s %s;tag=%x",__gtor->getHeadTo().getNameUri().c_str(), __gtor->getHeadTo().getC_AttSipUri().getContent().c_str(),__gtor);
		}else {
			sprintf(totmp, "%s;tag=%x",__gtor->getHeadTo().getC_AttSipUri().getContent().c_str(),__gtor);
		}
		string totmpS(totmp);
		DEBOUT("******** TO new" , totmpS)
		_gted->replaceHeadTo(totmpS);
		DEBOUT("TO",_gted->getHeadTo().getContent())
		DEBOUT("TO",_gted->getHeadTo().getC_AttSipUri().getContent())
		DEBOUT("TO",_gted->getHeadTo().getNameUri())
		DEBOUT("TO",_gted->getHeadTo().getC_AttUriParms().getContent())
	}


	//TODO qui fare dialoge_x...
	DEBOUT("_gted","SIP/2.0 " << _gtor->getHeadSipReply().getContent())
	_gted->setHeadSipReply(_gtor->getHeadSipReply().getContent());
//	DEBOUT("reply_x","Purge sdp")
//	_gted->purgeSDP();
	DEBOUT("reply_x","delete User-Agent:")
	_gted->dropHeader("User-Agent:");
	DEBOUT("reply_x","delete Max-Forwards:")
	_gted->removeMaxForwards();
	DEBOUT("reply_x","delete Allow:")
	_gted->dropHeader("Allow:");
	DEBOUT("reply_x","delete Route:")
	_gted->dropHeader("Route:");
	DEBOUT("reply_x","delete Date:")
	_gted->dropHeader("Date:");
//	DEBOUT("reply_x","delete Content-Type:")
//	_gted->dropHeader("Content-Type:");

	//_gted->setGenericHeader("Content-Length:","0");
	//crash here...

	//via add rport
	DEBY
	C_HeadVia* viatmp = (C_HeadVia*) _gted->getSTKHeadVia().top();
	//TODO 124??
	DEBOUT("viatmp->getContent", viatmp->getContent())
	viatmp->getChangeC_AttVia().getChangeViaParms().replaceRvalue("rport", "124");
	_gted->popSTKHeadVia();
	_gted->pushHeadVia("Via: "+viatmp->getC_AttVia().getContent());


}

void SIPUTIL::genASideReplyFromRequest(MESSAGE* _gtor, MESSAGE* _gted){

	//TODO qui fare etry...
	DEBOUT("GTED","Purge sdp")
	_gted->purgeSDP();
	DEBOUT("GTED","delete User-Agent:")
	_gted->dropHeader("User-Agent:");
	DEBOUT("GTED","delete Max-Forwards:")
	_gted->removeMaxForwards();
	DEBOUT("GTED","delete Content-Type:")
	_gted->dropHeader("Content-Type:");
	DEBOUT("GTED","delete Allow:")
	_gted->dropHeader("Allow:");
	DEBOUT("GTED","delete Route:")
	_gted->dropHeader("Route:");
	DEBOUT("GTED","delete Date:")
	_gted->dropHeader("Date:");

	_gted->setGenericHeader("Content-Length:","0");

	//via add rport
	C_HeadVia* viatmp = (C_HeadVia*) _gted->getSTKHeadVia().top();
	DEBY
	//TODO 124??
	viatmp->getChangeC_AttVia().getChangeViaParms().replaceRvalue("rport", "124");
	DEBY

	DEBOUT("GTED via", viatmp->getC_AttVia().getContent())
	_gted->popSTKHeadVia();
	_gted->pushHeadVia("Via: " + viatmp->getC_AttVia().getContent());

	DEBOUT("GTED","setDestEntity")
	_gted->setDestEntity(SODE_APOINT);
}

//void SIPUTIL::genBRequestfromARequest(MESSAGE* _gtor, MESSAGE* _gted, SUDP* sudp){
//
//		//Via Via: SIP/2.0/TCP 127.0.0.1:5060;branch=z9hG4bKYesTAZxWOfNDtT97ie51tw
//
//		char viatmp[512];
//		sprintf(viatmp, "SIP/2.0/UDP %s:%d;branch=z9hG4bK%s;rport",sudp->getDomain().c_str(),sudp->getPort(),_gtor->getKey().c_str());
//		string viatmpS(viatmp);
//		_gted->purgeSTKHeadVia();
//		_gted->pushHeadVia("Via: " + viatmpS);
//
//		_gted->replaceHeadContact("<sip:sipsl@grog:5060>");
//		DEBOUT("NEW CONTACT", _gted->getHeadContact().getContent())
//
//		// Compile the message
//		_gted->compileMessage();
//
//	}
void SIPUTIL::genBInvitefromAInvite(MESSAGE* _gtor, MESSAGE* _gted, SUDP* sudp){

	//Via Via: SIP/2.0/TCP 127.0.0.1:5060;branch=z9hG4bKYesTAZxWOfNDtT97ie51tw

	char viatmp[512];
	sprintf(viatmp, "SIP/2.0/UDP %s:%d;branch=z9hG4bK%s;rport",sudp->getDomain().c_str(),sudp->getPort(),_gtor->getKey().c_str());
	string viatmpS(viatmp);
	_gted->purgeSTKHeadVia();
	_gted->pushHeadVia("Via: " + viatmpS);

	//Create new call id
	char callIdtmp[512];
	sprintf(callIdtmp, "%s@%s", _gtor->getKey().c_str(), sudp->getDomain().c_str());
	string callIdtmpS(callIdtmp);
	_gted->setGenericHeader("Call-ID:", callIdtmpS);

	//From changes
	// in From: <sip:guic@172.21.160.184>;tag=0ac37672-6a86-de11-992a-001d7206fe48
	// out From: <sip:guic@172.21.160.184>;tag=YKcAvQ
	DEBOUT("FROM",_gted->getHeadFrom().getContent())
	DEBOUT("FROM",_gted->getHeadFrom().getC_AttSipUri().getContent())
	DEBOUT("FROM",_gted->getHeadFrom().getNameUri())
	DEBOUT("FROM",_gted->getHeadFrom().getC_AttUriParms().getContent())
	// change tag
	char fromtmp[512];
	if (_gted->getHeadFrom().getNameUri().length() == 0)
		sprintf(fromtmp, "%s;tag=%s",_gted->getHeadFrom().getC_AttSipUri().getContent().c_str(),_gted->getKey().c_str());
	else
		sprintf(fromtmp, "%s %s;tag=%s",_gted->getHeadFrom().getNameUri().c_str(), _gted->getHeadFrom().getC_AttSipUri().getContent().c_str(),_gted->getKey().c_str());
	string fromtmpS(fromtmp);
	DEBOUT("******** FROM new" , fromtmpS)
	_gted->replaceHeadFrom(fromtmpS);
	DEBOUT("FROM",_gted->getHeadFrom().getContent())
	DEBOUT("FROM",_gted->getHeadFrom().getC_AttSipUri().getContent())
	DEBOUT("FROM",_gted->getHeadFrom().getNameUri())
	DEBOUT("FROM",_gted->getHeadFrom().getC_AttUriParms().getContent())


	DEBOUT("CONTACT", _gted->getHeadContact().getContent())

	_gted->replaceHeadContact("<sip:sipsl@grog:5060>");
	DEBOUT("NEW CONTACT", _gted->getHeadContact().getContent())


}
