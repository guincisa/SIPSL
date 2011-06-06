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


void SIPUTIL::genASideReplyFromBReply(MESSAGE* _gtor, MESSAGE* __gtor, MESSAGE* _gted){
	DEBINF("void SIPUTIL::genASideReplyFromBReply(MESSAGE* _gtor, MESSAGE* __gtor, MESSAGE* _gted)",_gtor<<"]["<<__gtor<<"]["<<_gted)

	//__gtor is the B reply
	// _gtor is the A Invite
	// _getd reply for A

	//Must define here the to tag
	//only if empty...


	/*
	SIP/2.0 200 OK
	Via: SIP/2.0/UDP sipsl.gugli.com:5060;branch=z9hG4bK1a21801306247463264072;rport
	From: sipp <sip:sipp@10.21.99.79:5061>;tag=1a78b81306247463273504
	To: sut <sip:service@10.21.99.79:5062>;tag=25297SIPpTag011
	Call-ID: CoMap121a21801306247463264072@sipsl.gugli.com
	CSeq: 1 INVITE
	Contact: <sip:10.21.99.79:5062;transport=UDP>
	Content-Type: application/sdp
	Content-Length:   133

	v=0
	o=user1 53655765 2353687637 IN IP4 10.21.99.79
	s=-
	c=IN IP4 10.21.99.79
	t=0 0
	m=audio 6000 RTP/AVP 0
	a=rtpmap:0 PCMU/8000

	SIP/2.0 200 OK
	Via: SIP/2.0/UDP 10.21.99.79:5061;branch=z9hG4bK-29057-1-0
	From: sipp <sip:sipp@10.21.99.79:5061>;tag=29057SIPpTag001
	To: sut <sip:service@10.21.99.79:5062>
	Call-ID: 1-29057@10.21.99.79
	CSeq: 1 INVITE
	Contact: <sip:sipsl@grog:5060>
	Subject: Performance Test
	Content-Type: application/sdp
	Content-Length: 133

	v=0
	o=user1 53655765 2353687637 IN IP4 10.21.99.79
	s=-
	c=IN IP4 10.21.99.79
	t=0 0
	m=audio 6000 RTP/AVP 0
	a=rtpmap:0 PCMU/8000
	 */


//	if ( _gted->getHeadTo()->getC_AttUriParms().getContent().length() == 0){
//		char totmp[512];
//		if (__gtor->getHeadTo()->getNameUri().length()!= 0){
//			sprintf(totmp, "%s %s;tag=%x",__gtor->getHeadTo()->getNameUri().c_str(), __gtor->getHeadTo()->getC_AttSipUri().getContent().c_str(),(unsigned int)__gtor);
//		}else {
//			sprintf(totmp, "%s;tag=%x",__gtor->getHeadTo()->getC_AttSipUri().getContent().c_str(),(unsigned int)__gtor);
//		}
//		string totmpS(totmp);
//		_gted->replaceHeadTo(totmpS);
//	}
//	if (_gted->getHeadToParams().length() == 0){
//		string totmp ("");
//		if (__gtor->getHeadToName().length()!=0){
//			totmp += __gtor->getHeadToName();
//			totmp += " ";
//		}
//		totmp += __gtor->getHeadToUri();
//		totmp += ";tag=";
//		totmp += (unsigned int)__gtor;
//	}

	_gted->setGenericHeader("Contact:","<sip:sipsl@grog:5060>");

	string ttt(""); ttt += (unsigned int) __gtor;
	_gted->setProperty("To:", "tag", ttt);
	_gted->setHeadSipReply(_gtor->getHeadSipReply());
	_gted->dropHeader("User-Agent:");
	_gted->dropHeader("Max-Forwards:");
	_gted->dropHeader("Allow:");
	_gted->dropHeader("Route:");
	_gted->dropHeader("Date:");
	_gted->setProperty("Via:","rport","1234");

}

void SIPUTIL::genASideReplyFromRequest(MESSAGE* _gtor, MESSAGE* _gted){
	DEBINF("void SIPUTIL::genASideReplyFromRequest(MESSAGE* _gtor, MESSAGE* _gted)",_gtor<<"]["<<_gted)

	_gted->purgeSDP();
	_gted->dropHeader("User-Agent:");
	_gted->dropHeader("Max-Forwards:");
	_gted->dropHeader("Content-Type:");
	_gted->dropHeader("Allow:");
	_gted->dropHeader("Route:");
	_gted->dropHeader("Date:");
	_gted->setGenericHeader("Content-Length:","0");

	//_gted->setProperty("Via:","rport","124");

}

void SIPUTIL::genBInvitefromAInvite(MESSAGE* _gtor, MESSAGE* _gted, SUDP* sudp, string _callidy){
	DEBINF("void SIPUTIL::genBInvitefromAInvite(MESSAGE* _gtor, MESSAGE* _gted, SUDP* sudp, string _callidy)",_gtor<<"]["<<_gted<<"]["<<sudp<<"]["<<_callidy)

	//Via Via: SIP/2.0/TCP 127.0.0.1:5060;branch=z9hG4bKYesTAZxWOfNDtT97ie51tw
	//set new Via, is used by the b part to send replies
	_gted->popVia();
	stringstream viatmp;
	viatmp << ("SIP/2.0/UDP ") << sudp->getDomain() << ":" << sudp->getPort() << ";branch=z9hG4bK" <<_gtor->getKey() << ";rport";
	_gted->pushNewVia(viatmp.str());

	_gted->setGenericHeader("Call-ID:", _callidy);

	_gted->setProperty("From:", "tag", _gted->getKey());

	if (_gtor->hasSDP()){
		//SDP must copy the SDP from incoming OK and put here
		_gted->purgeSDP();
		_gted->setSDP(_gtor->getSDP());
	}

}

void SIPUTIL::genTryFromInvite(MESSAGE* _invite, MESSAGE* _etry){
	DEBINF("void SIPUTIL::genTryFromInvite(MESSAGE* _invite, MESSAGE* _etry)",_invite<<"]["<<_etry)

	//DEBOUT("ETRY","SIP/2.0 100 Trying")
	_etry->setHeadSipReply("100 Trying");

	_etry->dropHeader("Contact:");

	genASideReplyFromRequest(_invite, _etry);
	_etry->compileMessage();

}
void SIPUTIL::genQuickReplyFromInvite(MESSAGE* _invite, MESSAGE* _qrep, string _header){
	DEBINF("void SIPUTIL::genQuickReplyFromInvite(MESSAGE* _invite, MESSAGE* _qrep, string _header)",_invite<<"]["<<_qrep<<"]["<<_header)
	//DEBOUT("Reply",_header)
	_qrep->setHeadSipReply(_header);

	_qrep->dropHeader("Contact:");

	genASideReplyFromRequest(_invite, _qrep);
	_qrep->compileMessage();
#ifdef LOGSIP
	_qrep->dumpVector();
#endif
}

