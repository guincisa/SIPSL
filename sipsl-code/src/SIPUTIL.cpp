//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Service Layer
// Copyright (C) 2011 Guglielmo Incisa di Camerana
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

#include <algorithm>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <iostream>
#include <map>
#include <math.h>
#include <memory>
#include <pthread.h>
#include <signal.h>
#include <sstream>
#include <stack>
#include <stdio.h>
#include <stdlib.h>     /* for atoi() and exit() */
#include <string>
#include <string.h>
#include <sys/socket.h> /* for socket() and bind() */
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <vector>
#include <unordered_map>

#include "UTIL.h"
#
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


void SIPUTIL::genASideReplyFromBReply(MESSAGE* _gtor, MESSAGE* __gtor, MESSAGE* _gted, SUDP* _sudp){
	DEBUGSIPUTIL_3("void SIPUTIL::genASideReplyFromBReply(MESSAGE* _gtor, MESSAGE* __gtor, MESSAGE* _gted)",_gtor<<"]["<<__gtor<<"]["<<_gted)

	TIMEDEF
	SETNOW
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

	string totag = __gtor->getToTag();
	DEBUGSIPUTIL("SIPUTIL::genASideReplyFromBReply __gtor->getToTag()", totag)
	if( totag.length()!= 0 ){
		_gted->setProperty("To:", "tag", totag.c_str());
	}


	char cons[GENSTRINGLEN];
	sprintf(cons, "<sip:sipsl@%s:%s>",_sudp->getLocalIp().c_str(),_sudp->getLocalPort().c_str());

	_gted->setGenericHeader("Contact:",cons);
	_gted->setHeadSipReply(_gtor->getHeadSipReply());
	_gted->dropHeader("User-Agent:");
	_gted->dropHeader("Max-Forwards:");
	_gted->dropHeader("Allow:");
	_gted->dropHeader("Route:");
	_gted->dropHeader("Date:");

	PRINTDIFF("SIPUTIL::genASideReplyFromBReply")
//	_gted->setProperty("Via:","received",inet_ntoa(_gted->getEchoClntAddr().sin_addr));
//	stringstream xx;
//	xx << ntohs((_gted->getEchoClntAddr()).sin_port);
//	_gted->setProperty("Via:","rport",xx.str());

}

void SIPUTIL::genASideReplyFromRequest(MESSAGE* _gtor, MESSAGE* _gted){
	DEBUGSIPUTIL_3("void SIPUTIL::genASideReplyFromRequest(MESSAGE* _gtor, MESSAGE* _gted)",_gtor<<"]["<<_gted)

	TIMEDEF
	SETNOW
	_gted->purgeSDP();
	_gted->dropHeader("User-Agent:");
	_gted->dropHeader("Max-Forwards:");
	_gted->dropHeader("Content-Type:");
	_gted->dropHeader("Allow:");
	_gted->dropHeader("Route:");
	_gted->dropHeader("Date:");
	_gted->setGenericHeader("Content-Length:","0");

	PRINTDIFF("SIPUTIL::genASideReplyFromRequest")
	//now check if Via has the rport property and is empty
	//fill it in with inet_ntoa(echoClntAddr.sin_addr):echoClntAddr.sin_port
	//received=192.0.2.1;rport=9988
	//vanno nello strato di trasporto!!!!
//	_gted->setProperty("Via:","received",inet_ntoa(_gted->getEchoClntAddr().sin_addr));
//	stringstream xx;
//	xx << ntohs((_gted->getEchoClntAddr()).sin_port);
//	_gted->setProperty("Via:","rport",xx.str());


}

void SIPUTIL::genBInvitefromAInvite(MESSAGE* _gtor, MESSAGE* _gted, SUDP* sudp, string _callidy){
	DEBUGSIPUTIL_3("void SIPUTIL::genBInvitefromAInvite(MESSAGE* _gtor, MESSAGE* _gted, SUDP* sudp, string _callidy)",_gtor<<"]["<<_gted<<"]["<<sudp<<"]["<<_callidy)

	TIMEDEF
	SETNOW
	//Via Via: SIP/2.0/TCP 127.0.0.1:5060;branch=z9hG4bKYesTAZxWOfNDtT97ie51tw
	//set new Via, is used by the b part to send replies
	_gted->popVia();
	char viatmp[GENSTRINGLEN];
	sprintf(viatmp,"SIP/2.0/UDP %s:%s;branch=z9hG4bK%s;rport",sudp->getLocalIp().c_str(),sudp->getLocalPort().c_str(),_gtor->getKey().c_str());
	_gted->pushNewVia(viatmp);

	_gted->setGenericHeader("Call-ID:", _callidy);
	char cons[GENSTRINGLEN];

	sprintf(cons,"<sip:sipsl@%s:%s>",sudp->getLocalIp().c_str(),sudp->getLocalPort().c_str());

	_gted->setGenericHeader("Contact:", cons);

	_gted->setProperty("From:", "tag", _gted->getKey());

	if (_gtor->hasSDP()){
		//SDP must copy the SDP from incoming OK and put here
		_gted->purgeSDP();
		_gted->setSDP(_gtor->getSDP());
	}
	PRINTDIFF("SIPUTIL::genBInvitefromAInvite")

}

void SIPUTIL::genTryFromInvite(MESSAGE* _invite, MESSAGE* _etry){
	DEBUGSIPUTIL_3("void SIPUTIL::genTryFromInvite(MESSAGE* _invite, MESSAGE* _etry)",_invite<<"]["<<_etry)

	TIMEDEF
	SETNOW

	_etry->setHeadSipReply("100 Trying");

	_etry->dropHeader("Contact:");

	genASideReplyFromRequest(_invite, _etry);
	_etry->_forceHeadSipReply(100);
	_etry->compileMessage();
	PRINTDIFF("SIPUTIL::genTryFromInvite")

}
void SIPUTIL::genQuickReplyFromInvite(MESSAGE* _invite, MESSAGE* _qrep, string _header){
	DEBUGSIPUTIL_3("void SIPUTIL::genQuickReplyFromInvite(MESSAGE* _invite, MESSAGE* _qrep, string _header)",_invite<<"]["<<_qrep<<"]["<<_header)
	//DEBOUT("Reply",_header)
	TIMEDEF
	SETNOW
	_qrep->setHeadSipReply(_header);

	_qrep->dropHeader("Contact:");

	genASideReplyFromRequest(_invite, _qrep);

	_qrep->compileMessage();
	PRINTDIFF("SIPUTIL::genQuickReplyFromInvite")

#ifdef LOGSIP
	_qrep->dumpVector();
#endif
}

