//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPCSL Sip Core And Service Layer
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
#ifndef TRNSCT_SM_H
#include "TRNSCT_SM.h"
#endif
#ifndef DAO_H
#include "DAO.h"
#endif

SIPUTIL SipUtil;

VALO::VALO(ENGINE* _e, CALL_OSET* _co):ALO(_e, _co){
	DEBINF("VALO::VALO(ENGINE* _e, CALL_OSET* _co):ALO(_e, _co)", this<<"]["<<_e<<"]["<<_co)
}
VALO::~VALO(void){
	DEBINF("VALO::~VALO(void)",this)

	//purge pointers on map except messages
	map<string, void*> ::iterator p;

	p = ctxt_store.find("tohead_200ok_b");
	if (p != ctxt_store.end()){
		DELPTR((string*)p->second,"tohead_200ok_b");
		ctxt_store.erase(p);
	}

	p = ctxt_store.find("tohead_200ok_a");
	if (p != ctxt_store.end()){
		DELPTR((string*)p->second,"tohead_200ok_a");
		ctxt_store.erase(p);
	}

	p = ctxt_store.find("CSeqB2BINVITE");
	if (p != ctxt_store.end()){
		DELPTR((int*)p->second,"CSeqB2BINVITE");
		ctxt_store.erase(p);
	}

	p = ctxt_store.find("totag_200ok_b");
	if (p != ctxt_store.end()){
		DELPTR((string*)p->second,"totag_200ok_b");
		ctxt_store.erase(p);
	}

	p = ctxt_store.find("allvia_200ok_b");
	if (p != ctxt_store.end()){
		DELPTR((string*)p->second,"allvia_200ok_b");
		ctxt_store.erase(p);
	}

	p = ctxt_store.find("callid_200ok_b");
	if (p != ctxt_store.end()){
		DELPTR((string*)p->second,"callid_200ok_b");
		ctxt_store.erase(p);
	}

	p = ctxt_store.find("fromhead_200ok_b");
	if (p != ctxt_store.end()){
		DELPTR((string*)p->second,"fromhead_200ok_b");
		ctxt_store.erase(p);
	}

	p = ctxt_store.find("fromhead_200ok_a");
	if (p != ctxt_store.end()){
		DELPTR((string*)p->second,"fromhead_200ok_a");
		ctxt_store.erase(p);
	}
	p = ctxt_store.find("RouteIp");
	if (p != ctxt_store.end()){
		DELPTR((string*)p->second,"RouteIp");
		ctxt_store.erase(p);
	}
	p = ctxt_store.find("RoutePort");
	if (p != ctxt_store.end()){
		DELPTR((string*)p->second,"RoutePort");
		ctxt_store.erase(p);
	}
	p = ctxt_store.find("ContactBAddress");
	if (p != ctxt_store.end()){
		DELPTR((string*)p->second,"ContactBAddress");
		ctxt_store.erase(p);
	}
	p = ctxt_store.find("ContactBPort");
	if (p != ctxt_store.end()){
		DELPTR((string*)p->second,"ContactBPort");
		ctxt_store.erase(p);
	}
	p = ctxt_store.find("ContactBName");
	if (p != ctxt_store.end()){
		DELPTR((string*)p->second,"ContactBName");
		ctxt_store.erase(p);
	}

}
void VALO::onInvite(MESSAGE* _message){
	DEBINF("void VALO::onInvite(MESSAGE* _message)",this<<"]["<<_message)

    TIMEDEF
    SETNOW
    PROFILE("VALO::onInvite")

	CREATEMESSAGE(message, _message, SODE_ALOPOINT, SODE_TRNSCT_CL)

    //MLF2
	message->setSourceMessage(_message->getSourceMessage());

    //Deleted in action_SV
    //PURGEMESSAGE(_message)

	try {
		DEBALO("VALO","remove route")
		message->dropHeader("Route:");
	}
	catch(HeaderException e){
		DEBALO("Exception ", e.getMessage())
	}

	DEBY
	pair<string,string> tp = _message->getRequestUriProtocol();
	DEBOUT("indirizzo request", tp.first << "] ["<<tp.second)
	char css[GENSTRINGLEN];
	if (tp.second.compare("5060") != 0){
		sprintf(css,"%s:%s",tp.first.c_str(),tp.second.c_str());
	}else{
		sprintf(css,"%s",tp.first.c_str());
	}
	DEBY
#ifdef VODAFONEBB
	string tmpR = ((SL_CC*)sl_cc)->getDAO()->getData(TBL_REGISTER,css);
#else
	string tmpR = ((SL_CC*)sl_cc)->getDAO()->getData(TBL_ROUTE,css);
#endif
	DEBOUT("getDAO()->getData tmpR", tmpR)
	if (tmpR.length() != 0){
		DEBY
		size_t found = tmpR.find(":");
		if ( found!=string::npos){
			NEWPTR(string*, ss, string(tmpR.substr(0,found-1)),"RouteIp")
			NEWPTR(string*, ss2, string(tmpR.substr(found+1)),"RoutePort")
			DEBOUT("message->setRoute",tmpR.substr(0,found-1) <<"]["<<tmpR.substr(found+1))
			ctxt_store.insert(pair<string, void*>("RouteIp", (void*) ss ));
			ctxt_store.insert(pair<string, void*>("RoutePort", (void*) ss2 ));

			message->setRoute(tmpR.substr(0,found-1) , tmpR.substr(found+1));
		}else{
			NEWPTR(string*, ss, string(tmpR.substr(0,found-1)),"RouteIp")
			NEWPTR(string*, ss2, string("5060"),"RoutePort")
			ctxt_store.insert(pair<string, void*>("RouteIp", (void*) ss ));
			ctxt_store.insert(pair<string, void*>("RoutePort", (void*) ss2 ));
			message->setRoute(tmpR,"5060");
		}
	}


//	stringstream tmps ;
//	tmps << "INVITE sip:"<< *ss <<" SIP/2.0";
//	message->setHeadSipRequest(tmps.str());

	//message->setHeadSipRequest("INVITE sip:GUGLISIPSL@bphone.gugli.com:5062 SIP/2.0");

	char cs[GENSTRINGLEN];
	int csnext = call_oset->getNextSequence("INVITE_B");
	sprintf(cs,"%d INVITE",csnext);
	message->setGenericHeader("CSeq:",cs);

	//Standard changes
	string newCallid = call_oset->getCallId_Y();
	SipUtil.genBInvitefromAInvite(_message->getSourceMessage(), message, getSUDP(), newCallid);
	//no good contact

	message->compileMessage();


	DEBMESSAGE("New outgoing b2b message", message)

	//TODO ???
	NEWPTR(int*, CSeqB2BINVITE, int(csnext),"CSeqB2BINVITE")
	ctxt_store.insert(pair<string, void*>("CSeqB2BINVITE", (void*) CSeqB2BINVITE ));


	call_oset->setCallId_Y(newCallid);

	//store this invites
	//only possible because both invites are locked during the creation of their SM
	ctxt_store.insert(pair<string, void*>("invite_a", (void*) _message->getSourceMessage() ));
	ctxt_store.insert(pair<string, void*>("invite_b", (void*) message ));


#ifdef NONESTEDPW
	int r;
	call_oset->getSL_CO()->call(message,r);
//	//do not delete here delete in "call"
//	if (!message->getLock()){
//		PURGEMESSAGE(message)
//	}

#else
	sl_cc->p_w(message);
#endif

     PRINTDIFF("VALO::onInvite")

}
void VALO::onAck(MESSAGE* _message){
	DEBINF("void VALO::onAck(MESSAGE* _message)",this<<"]["<<_message)
    TIMEDEF
    SETNOW
    PROFILE("VALO::onAck")

	/*
	SIP/2.0 200 OK
	Via: SIP/2.0/UDP sipsl.gugli.com:5060;branch=z9hG4bK1c28881306836789394442;rport
	From: sipp <sip:sipp@10.21.99.79:5061>;tag=1c7c281306836789409645
	To: sut <sip:service@10.21.99.79:5062>;tag=15115SIPpTag012
	Call-ID: CoMap051c28881306836789394442@sipsl.gugli.com
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
	*/
	/*
	[ACK sip:service@10.21.99.79:5062 SIP/2.0
	Via: SIP/2.0/UDP 10.21.99.79:5061;branch=z9hG4bK-22189-1-5
	From: sipp <sip:sipp@10.21.99.79:5061>;tag=22189SIPpTag001
	To: sut <sip:service@10.21.99.79:5062>
	Call-ID: 1-22189@10.21.99.79
	CSeq: 1 ACK
	Contact: sip:sipp@10.21.99.79:5061
	Max-Forwards: 70
	Subject: Performance Test
	Content-Length: 0

	]

	[ACK sip:GUGLISIPSL@10.21.99.79:5062 SIP/2.0
	From: sipp <sip:sipp@10.21.99.79:5061>;tag=1c7c281306836789409645
	To: sut <sip:service@10.21.99.79:5062>;tag=15115SIPpTag012
	Call-ID: CoMap051c28881306836789394442@sipsl.gugli.com
	CSeq: 1 ACK
	Contact: sip:sipp@10.21.99.79:5061
	Max-Forwards: 70
	Subject: Performance Test
	Content-Length: 0
	Via: SIP/2.0/UDP sipsl.gugli.com:5060;branch=z9hG4bK1d0ad81306836789440734;rport

	]*/

	//V5 trying to build the ACK using the ACK_A
	CREATEMESSAGE(newack, _message, SODE_ALOPOINT, SODE_SMCLPOINT)

	//Remove route
	try {
		DEBALO("VALO","remove route")
		newack->dropHeader("Route:");
	}
	catch(HeaderException e){
		DEBALO("Exception ", e.getMessage())
	}

//	string sss = newack->getUri("REQUEST").first;
//	int ppp = newack->getUri("REQUEST").second;
//	DEBOUT("indirizzo request", sss << "] ["<<ppp)
//	stringstream css;
//	css << sss;
//	css << ":";
//	css <<ppp;

	map<string, void*> ::iterator p,p1,p2,p3;

	p = ctxt_store.find("RouteIp");
	if (p != ctxt_store.end()){
		string* ss1 = (string*)p->second;
		p = ctxt_store.find("RoutePort");
		string* ss2 = (string*)p->second;
		newack->setRoute(*ss1,*ss2);
	}

	p1 = ctxt_store.find("ContactBName");
	p2 = ctxt_store.find("ContactBAddress");
	p3 = ctxt_store.find("ContactBPort");
	
	char tmps[GENSTRINGLEN];
	sprintf(tmps,"ACK sip:%s@%s:%s SIP/2.0",((string*)p1->second)->c_str(),((string*)p2->second)->c_str(),((string*)p3->second)->c_str());
	newack->setHeadSipRequest(tmps);

	//Purge SDP
	newack->purgeSDP();

	//FROM and TO copied from B 200 OK
	p = ctxt_store.find("tohead_200ok_b");
	string tohead_200ok_b = *((string*)p->second);
	p = ctxt_store.find("fromhead_200ok_b");
	string fromhead_200ok_b = *((string*)p->second);
	newack->setGenericHeader("To:",tohead_200ok_b);
	newack->setGenericHeader("From:",fromhead_200ok_b);


	//CallId
	newack->setGenericHeader("Call-ID:", call_oset->getCallId_Y());

	//CSEQ
	p = ctxt_store.find("CSeqB2BINVITE");
	char cst[GENSTRINGLEN];
	sprintf(cst,"%d %s",*((int*)p->second),_message->getHeadCSeqMethod().c_str());
	newack->setGenericHeader("CSeq:", cst);

	//Via
	char viatmpS[GENSTRINGLEN];
	sprintf(viatmpS,"SIP/2.0/UDP %s:%i;branch=z9hG4bK%s;rport",getSUDP()->getDomain().c_str(),getSUDP()->getPort(),newack->getKey().c_str());
	newack->popVia();
	newack->pushNewVia(viatmpS);
	DEBALO("newack->pushHeadVia(viatmpS);", viatmpS)

	newack->compileMessage();


	DEBMESSAGE("New outgoing b2b message", newack)
#ifdef NONESTEDPW
	int r;
	call_oset->getSL_CO()->call(newack,r);
//	//do not delete here delete in "call"
//	if (!newack->getLock()){
//		PURGEMESSAGE(newack)
//	}

#else
	sl_cc->p_w(newack);
#endif


    PRINTDIFF("VALO::onAck")


}
void VALO::onAckNoTrnsct(MESSAGE* _message){
	DEBINF("void VALO::onAckNoTrnsct(MESSAGE* _message)",this<<"]["<<_message)
	DEBASSERT("VALO::onAckNoTrnsct")

//	DEBALO("VALO::onAckNoTrnsct",_message->getHeadSipRequest().getContent())
//
//	//V4
//	//get invite sent to b
////	DEBALO("VALO onAck", call_oset->getInviteB()->getIncBuffer())
////	CREATEMESSAGE(message, call_oset->getInviteB(), SODE_ALOPOINT)
////	//CREATEMESSAGE(message, _message, SODE_ALOPOINT)
//
//	//V5 trying to build the ACK using the ACK_A instead of INVITE b
//	CREATEMESSAGE(newack, _message, SODE_ALOPOINT)
//	//set as source the original ack, needed to identify call_oset_x when back to call control
//	newack->setSourceMessage(_message);
//	newack->setDestEntity(SODE_SMCLPOINT);
//
//	//Remove route
//	try {
//		DEBALO("VALO message->getHeadRoute().getRoute().getHostName()",newack->getHeadRoute().getRoute().getHostName())
//		DEBALO("VALO message->getHeadRoute().getRoute().getPort()",newack->getHeadRoute().getRoute().getPort())
//		DEBALO("VALO","remove route")
//		newack->removeHeadRoute();
//	}
//	catch(HeaderException e){
//		DEBALO("Exception ", e.getMessage())
//	}
//
//	//change request
//	DEBALO("VALO ", newack->getHeadSipRequest().getContent())
//	newack->setHeadSipRequest("ACK sip:GUGLISIPSL@bphone.gugli.com:5062 SIP/2.0");
//
//	//don't change CSEQ
////	char buff[64];
////	sprintf(buff, "%d ACK", call_oset->getNextSequence("ACK"));
////	newack->replaceHeadCSeq(buff);
////	DEBALO("VALO","Cseq")
//
//	//Purge SDP
//	newack->purgeSDP();
//	newack->addGenericHeader("Content-Length:","0");
//
////
//	//TOTAG
//	char toTmp[512];
//	map<string, void*> ::iterator p;
//	p = ctxt_store.find("totag");
//	string toTagB = *((string*)p->second);
//	sprintf(toTmp, "%s %s;tag=%s",newack->getHeadTo().getNameUri().c_str(), newack->getHeadTo().getC_AttSipUri().getContent().c_str(),toTagB.c_str());
//	string toTmpS(toTmp);
//	DEBALO("******** TO new" , toTmpS)
//	newack->replaceHeadTo(toTmpS);
//	DEBALO("TO",newack->getHeadTo().getContent())
//	DEBALO("TO",newack->getHeadTo().getC_AttSipUri().getContent())
//	DEBALO("TO",newack->getHeadTo().getNameUri())
//	DEBALO("TO",newack->getHeadTo().getC_AttUriParms().getContent())
//
//	DEBALO("NEW ACK via","")
//	map<string, void*> ::iterator p2;
//	p2 = ctxt_store.find("allvia");
//	string allVia = *((string*)p2->second);
//	newack->purgeSTKHeadVia();
//	newack->pushHeadVia(allVia);
//	DEBALO("NEW ACK via",allVia.c_str())
//
//	newack->compileMessage();
//	newack->dumpVector();
//
//	// THIS ACK needs the B_INVITE call id and
//	// 200 ok from B from and to headers!!!
//	DEBALO("CHECK THIS CALLID +++++++++++", call_oset->getCallId_Y());
//
//	p = ctxt_store.find("tohead_200ok_b");
//	string tohead_200ok_b = *((string*)p->second);
//	p = ctxt_store.find("fromhead_200ok_b");
//	string fromhead_200ok_b = *((string*)p->second);
//	p = ctxt_store.find("callid_200ok_b");
//	string callid_200ok_b = *((string*)p->second);
//	p = ctxt_store.find("CSeqB2BINIVTE");
//	int CSeqB2BINIVTE = *((int*)p->second);
//
//	DEBALO("CHECK THIS TO HEAD +++++++++++", tohead_200ok_b);
//	DEBALO("CHECK THIS FROM HEAD +++++++++++", fromhead_200ok_b);
//	DEBALO("CHECK THIS CSeq  +++++++++++", CSeqB2BINIVTE);
//
//	DEBALO("CHECK THIS CALL ID from context map +++++++++++", callid_200ok_b);
//	DEBALO("CHECK THIS CALL ID from call_oset +++++++++++", call_oset->getCallId_Y());
//
////	!!!!
////	To: To: <sip:gugli_linphone@172.21.160.181:5061>;tag=9f7bc830
////	From: From: <sip:gugli_twinkle@guglicorp.com>;tag=9d448d81276175425117411
//
//	newack->replaceHeadTo(tohead_200ok_b);
//	newack->replaceHeadFrom(fromhead_200ok_b);
//	newack->setGenericHeader("Call-ID:", call_oset->getCallId_Y());
//	newack->getHeadCSeq().setSequence(CSeqB2BINIVTE);
//
//	newack->compileMessage();
//	newack->dumpVector();
//
//
//	DEBMESSAGE("New outgoing b2b message", newack->getIncBuffer())
//	sl_cc->p_w(newack);
//
//
}
void VALO::onBye(MESSAGE* _message){
	DEBINF("void VALO::onBye(MESSAGE* _message)",this<<"]["<<_message)
    TIMEDEF
    SETNOW
    PROFILE("VALO::onBye")

	DEBMESSAGE("VALO::onBye", _message)

//	map<string, void*> ::iterator p;
//	p = ctxt_store.find("invite_b");
//	MESSAGE* invite_b = ((MESSAGE*)p->second);
//
//	CREATEMESSAGE(message, invite_b, SODE_ALOPOINT)

	CREATEMESSAGE(message, _message, SODE_ALOPOINT, SODE_TRNSCT_CL)
	//wrong!?
	//bye cannot have sourcemessage
	message->setSourceMessage(_message->getSourceMessage());
	//message->setSourceMessage(_message);


	DEBALO("BYE DIRECTION",_message->getRequestDirection() )
	if (_message->getRequestDirection() == 0){
		DEBASSERT("BYE DIRECTION missing")
	}

	if (_message->getRequestDirection() == SODE_FWD ) {
		DEBALO("VALO::onBye SODE_FWD",this<<"]["<<_message)

		//message->setDestEntity(SODE_TRNSCT_CL);

//		stringstream tmps;
//		tmps << "BYE sip:GUGLISIPSL@"<<BPHONE<<":5062 SIP/2.0";
//		message->setHeadSipRequest(tmps.str());

//		string sss = _message->getUri("REQUEST").first;
//		int ppp = _message->getUri("REQUEST").second;
//		DEBOUT("indirizzo request", sss << "] ["<<ppp)
//		stringstream css;
//		css << sss;
//		css << ":";
//		css <<ppp;
//		string ss = ((SL_CC*)sl_cc)->getDAO()->getData(css.str());

		map<string, void*> ::iterator p,p1,p2,p3;
		p = ctxt_store.find("RouteIp");
		if (p != ctxt_store.end()){
			string* ss1 = (string*)p->second;
			p = ctxt_store.find("RoutePort");
			string* ss2 = (string*)p->second;
			message->setRoute(*ss1,*ss2);
		}
		p1 = ctxt_store.find("ContactBName");
		p2 = ctxt_store.find("ContactBAddress");
		p3 = ctxt_store.find("ContactBPort");
		
		char tmps[GENSTRINGLEN] ;
		sprintf(tmps,"BYE sip:%s@%s:%s SIP/2.0",((string*)p1->second)->c_str(),((string*)p2->second)->c_str(),((string*)p3->second)->c_str());
		message->setHeadSipRequest(tmps);

		char viatmp[GENSTRINGLEN];
		sprintf(viatmp,"SIP/2.0/UDP %s:%d;branch=z9hG4bK%s;rport",getSUDP()->getDomain().c_str(),getSUDP()->getPort(),message->getKey().c_str());
		message->popVia();
		message->pushNewVia(viatmp);

		char cst[GENSTRINGLEN];
		sprintf(cst,"%d BYE",call_oset->getNextSequence("INVITE_B"));
		message->setGenericHeader("CSeq:", cst);


		p = ctxt_store.find("tohead_200ok_b");
		string tohead_200ok_b = *((string*)p->second);
		p = ctxt_store.find("fromhead_200ok_b");
		string fromhead_200ok_b = *((string*)p->second);
		p = ctxt_store.find("callid_200ok_b");
		string callid_200ok_b = *((string*)p->second);
		message->setGenericHeader("To:",tohead_200ok_b);
		message->setGenericHeader("From:",fromhead_200ok_b);
		message->setGenericHeader("Call-ID:", call_oset->getCallId_Y());

		message->compileMessage();
		//message->dumpVector();
		//done in the client sm
		//message->setLock();
#ifdef NONESTEDPW
		int r;
		call_oset->getSL_CO()->call(message,r);
//		//do not delete here delete in "call"
//		if (!message->getLock()){
//			PURGEMESSAGE(message)
//		}

#else
		sl_cc->p_w(message);
#endif

	}
	else if (_message->getRequestDirection() == SODE_BKWD ) {


		DEBALO("VALO::onBye SODE_BKWD",this<<"]["<<_message)

		//DEBASSERT("implement nat!!!!")

		map<string, void*> ::iterator p;
		p = ctxt_store.find("invite_a");
		MESSAGE* __message = (MESSAGE*)p->second;

		DEBALO("Search for INVITE A sequence", call_oset->getCurrentSequence("INVITE_A"));

		//BYE Auser@domain
		//to A
		//from B
		stringstream _xx;
		_xx << ntohs((__message->getEchoClntAddr()).sin_port);

		message->setNatTraversal(inet_ntoa(__message->getEchoClntAddr().sin_addr),_xx.str());

		//need to send to NAT address....
		stringstream _ss;
		//sbagliato.....
		//corregere solo questo
		//mettere il contact del INVITE da A
		_ss << "BYE sip:ceppadim@" << __message->getViaUriHost() << ":" << __message->getViaUriPort() << " SIP/2.0";
		message->setHeadSipRequest(_ss.str());

		//
		message->popVia();
		stringstream viatmp;
		viatmp << "SIP/2.0/UDP "<<getSUDP()->getDomain()<<":"<<getSUDP()->getPort()<<";branch=z9hG4bK"<<message->getKey()<<";rport";
		message->pushNewVia(viatmp.str());

		stringstream css;
		css << call_oset->getCurrentSequence("INVITE_A");
		css << " BYE";
		message->setGenericHeader("CSeq:",css.str());

		p = ctxt_store.find("tohead_200ok_a");
		string tohead_200ok_a = *((string*)p->second);
		p = ctxt_store.find("fromhead_200ok_a");
		string fromhead_200ok_a = *((string*)p->second);
		// are inverted
		message->setGenericHeader("To:",fromhead_200ok_a);
		message->setGenericHeader("From:",tohead_200ok_a);
		message->setGenericHeader("Call-ID:", call_oset->getCallId_X());


		message->compileMessage();

#ifdef NONESTEDPW
		int r;
		call_oset->getSL_CO()->call(message,r);
//		if (!message->getLock()){
//			PURGEMESSAGE(message)
//		}

#else
		sl_cc->p_w(message);
#endif

	}
        PRINTDIFF("VALO::onBye")

}
void VALO::on200Ok(MESSAGE* _message){
	DEBINF("void VALO::on200Ok(MESSAGE* _message)", this<<"]["<<_message)
    TIMEDEF
    SETNOW
    PROFILE("VALO::on200Ok")

	TRNSCT_SM* trnsct_cl = call_oset->getTrnsctSm(_message->getHeadCSeqMethod(), SODE_TRNSCT_CL, _message->getViaBranch());
	if (trnsct_cl == 0x0){
		DEBASSERT("TRNSCT_SM* trnsct_cl == 0x0")
	}
	MESSAGE* __message = ((TRNSCT_SM*)trnsct_cl)->getA_Matrix();

	//The 200OK is called twice so we are leaking
	if (ctxt_store.find("totag_200ok_b") == ctxt_store.end()){
		NEWPTR(string*, totag, string(_message->getToTag()),"totag_200ok_b")
		DEBALO("STORE totag", totag)
		TRYCATCH(ctxt_store.insert(pair<string, void*>("totag_200ok_b", (void*) totag )))
	}

	if (ctxt_store.find("allvia_200ok_b") == ctxt_store.end()){
		NEWPTR(string*, allvia, string(_message->getViaLine()),"allvia_200ok_b")
		DEBALO("STORE totag", allvia)
		TRYCATCH(ctxt_store.insert(pair<string, void*>("allvia_200ok_b", (void*) allvia )))
	}

	// Need to store the FROM and TO
	// To create the ACK
	if (ctxt_store.find("tohead_200ok_b") == ctxt_store.end()){
		NEWPTR(string*, tohead,  string(_message->getGenericHeader("To:")),"tohead_200ok_b")
		DEBALO("STORE TO HEAD ok 200 ok from B", tohead)
		TRYCATCH(ctxt_store.insert(pair<string, void*>("tohead_200ok_b", (void*) tohead )))
	}
	if (ctxt_store.find("fromhead_200ok_b") == ctxt_store.end()){
		NEWPTR(string*, fromhead, string(_message->getGenericHeader("From:")),"fromhead_200ok_b")
		DEBALO("STORE FROM HEAD of 200 ok", fromhead)
		TRYCATCH(ctxt_store.insert(pair<string, void*>("fromhead_200ok_b", (void*) fromhead )))
	}

	//this shoudl go into call_oset call_idy
	if (ctxt_store.find("callid_200ok_b") == ctxt_store.end()){
		NEWPTR(string*, callid_200ok_b, string(_message->getGenericHeader("Call-ID:")),"callid_200ok_b")
		DEBALO("STORE CALL ID of 200 ok", callid_200ok_b)
		TRYCATCH(ctxt_store.insert(pair<string, void*>("callid_200ok_b", (void*) callid_200ok_b )))
	}

	//contact
	if (ctxt_store.find("ContactBName") == ctxt_store.end()){
		NEWPTR(string*, ContactBName, string(_message->getContactName()),"ContactBName")
		DEBALO("STORE ContactBName 200 ok", ContactBName)
		TRYCATCH(ctxt_store.insert(pair<string, void*>("ContactBName", (void*) ContactBName )))

		NEWPTR(string*, ContactBPort, string(_message->getContactPort()),"ContactBPort")
		DEBALO("STORE ContactBPort 200 ok", ContactBPort)
		TRYCATCH(ctxt_store.insert(pair<string, void*>("ContactBPort", (void*) ContactBPort )))

		NEWPTR(string*, ContactBAddress, string(_message->getContactAddress()),"ContactBAddress")
		DEBALO("STORE ContactBAddress 200 ok", ContactBName)
		TRYCATCH(ctxt_store.insert(pair<string, void*>("ContactBAddress", (void*) ContactBAddress )))
	}

	DEBALO("on200Ok MESSAGE GENERATOR", __message)
	CREATEMESSAGE(ok_x, __message, SODE_ALOPOINT , SODE_TRNSCT_SV)

	//ok_x->setTypeOfInternal(TYPE_MESS);

	DEBALO("ok_x","SIP/2.0 200 OK")

	if (ok_x->hasSDP()){
		//SDP must copy the SDP from incoming OK and put here
		vector< pair<char*, bool> > sdps  = _message->getSDP();
		ok_x->purgeSDP();

		ok_x->setSDP(sdps);
	}

	ok_x->setGenericHeader("Contact:","<sip:sipsl@grog:5060>");

	SipUtil.genASideReplyFromBReply(_message, __message, ok_x, getSUDP());
	ok_x->compileMessage();

	if (ctxt_store.find("tohead_200ok_a") == ctxt_store.end()){
		NEWPTR(string*, tohead_a, string(ok_x->getGenericHeader("To:")),"tohead_200ok_a")
		DEBALO("STORE tags of 200 OK to A",*tohead_a)
		TRYCATCH(ctxt_store.insert(pair<string, void*>("tohead_200ok_a", (void*) tohead_a )))
	}
	if (ctxt_store.find("fromhead_200ok_a") == ctxt_store.end()){
		NEWPTR(string*, fromhead_a, string(ok_x->getGenericHeader("From:")),"fromhead_200ok_a")
		DEBALO("STORE FROM HEAD of 200 ok", *fromhead_a)
		TRYCATCH(ctxt_store.insert(pair<string, void*>("fromhead_200ok_a", (void*) fromhead_a )))
	}

//	//TODO the 200 OK B for now is deleted
//	PURGEMESSAGE(_message)

	//ok_x->dumpVector();
#ifdef NONESTEDPW
		int r;
		call_oset->getSL_CO()->call(ok_x,r);
//		//do not delete here delete in "call"
//		if (!ok_x->getLock()){
//			PURGEMESSAGE(ok_x)
//		}
#else
		sl_cc->p_w(ok_x);
#endif

    PRINTDIFF("VALO::on200Ok")


}

//		[BYE sip:sipsl@127.0.1.1:5060 SIP/2.0
//		Route: <sip:192.168.0.101:5060;lr>
//		CSeq: 2 BYE
//		Via: SIP/2.0/UDP 192.168.0.101:5061;branch=z9hG4bKe2af05fd-4605-e111-9967-0024d74c54f8;rport
//		From: <sip:guic@192.168.0.101:5061>;tag=7c5dbbe6-4605-e111-9967-0024d74c54f8
//		Call-ID: CoMap0003c01f6d01320406587444034@grog.sipsl.org
//		To: <sip:majo@192.168.0.101>;tag=233c8a01320406587451325
//		Contact: <sip:guic@192.168.0.101:5061>
//		Content-Length: 0
//		Max-Forwards: 70
//
//		]
//		Session Initiation Protocol
//		    Request-Line: INVITE sip:guic@10.10.10.248:5061 SIP/2.0
//		        Method: INVITE
//		        Request-URI: sip:guic@10.10.10.248:5061
//		            Request-URI User Part: guic
//		            Request-URI Host Part: 10.10.10.248
//		            Request-URI Host Port: 5061
//		        [Resent Packet: False]
//		    Message Header
//		        Via: SIP/2.0/UDP 10.10.10.248:5062;rport;branch=z9hG4bK2018640705
//		            Transport: UDP
//		            Sent-by Address: 10.10.10.248
//		            Sent-by port: 5062
//		            RPort: rport
//		            Branch: z9hG4bK2018640705
//		        From: "majo" <sip:majo@10.10.10.248:5062>;tag=833511048
//		            SIP Display info: "majo"
//		            SIP from address: sip:majo@10.10.10.248:5062
//		                SIP from address User Part: majo
//		                SIP from address Host Part: 10.10.10.248
//		                SIP from address Host Port: 5062
//		            SIP tag: 833511048
//		        To: <sip:guic@10.10.10.248:5061>
//		            SIP to address: sip:guic@10.10.10.248:5061
//		                SIP to address User Part: guic
//		                SIP to address Host Part: 10.10.10.248
//		                SIP to address Host Port: 5061
//		        Call-ID: 134096379
//		        CSeq: 20 INVITE
//		            Sequence Number: 20
//		            Method: INVITE
//		        Contact: "majo" <sip:majo@10.10.10.248>
//		            SIP Display info: "majo"
//		            Contact-URI: sip:majo@10.10.10.248
//		                Contactt-URI User Part: majo
//		                Contact-URI Host Part: 10.10.10.248
//		        Content-Type: application/sdp
//		        Allow: INVITE, ACK, CANCEL, OPTIONS, BYE, REFER, NOTIFY, MESSAGE, SUBSCRIBE, INFO
//		        Max-Forwards: 70
//		        User-Agent: Linphone/3.3.2 (eXosip2/3.3.0)
//		        Subject: Phone call
//		        Content-Length:   404
//		Session Initiation Protocol
//		    Status-Line: SIP/2.0 200 OK
//		        Status-Code: 200
//		        [Resent Packet: False]
//		        [Request Frame: 235]
//		        [Response Time (ms): 1615]
//		    Message Header
//		        CSeq: 20 INVITE
//		            Sequence Number: 20
//		            Method: INVITE
//		        Via: SIP/2.0/UDP 10.10.10.248:5062;rport;branch=z9hG4bK2018640705
//		            Transport: UDP
//		            Sent-by Address: 10.10.10.248
//		            Sent-by port: 5062
//		            RPort: rport
//		            Branch: z9hG4bK2018640705
//		        User-Agent: Ekiga/3.2.7
//		        From: "majo" <sip:majo@10.10.10.248:5062>;tag=833511048
//		            SIP Display info: "majo"
//		            SIP from address: sip:majo@10.10.10.248:5062
//		                SIP from address User Part: majo
//		                SIP from address Host Part: 10.10.10.248
//		                SIP from address Host Port: 5062
//		            SIP tag: 833511048
//		        Call-ID: 134096379
//		        To: <sip:guic@10.10.10.248:5061>;tag=d45d67c1-82fe-e011-880f-68b599e80ba8
//		            SIP to address: sip:guic@10.10.10.248:5061
//		                SIP to address User Part: guic
//		                SIP to address Host Part: 10.10.10.248
//		                SIP to address Host Port: 5061
//		            SIP tag: d45d67c1-82fe-e011-880f-68b599e80ba8
//		        Contact: <sip:guic@10.10.10.248:5061>
//		            Contact-URI: sip:guic@10.10.10.248:5061
//		                Contactt-URI User Part: guic
//		                Contact-URI Host Part: 10.10.10.248
//		                Contact-URI Host Port: 5061
//		        Allow: INVITE,ACK,OPTIONS,BYE,CANCEL,SUBSCRIBE,NOTIFY,REFER,MESSAGE,INFO,PING
//		        Content-Type: application/sdp
//		        Content-Length: 251
//		Session Initiation Protocol
//		    Request-Line: BYE sip:majo@10.10.10.248 SIP/2.0
//		        Method: BYE
//		        Request-URI: sip:majo@10.10.10.248
//		            Request-URI User Part: majo
//		            Request-URI Host Part: 10.10.10.248
//		        [Resent Packet: False]
//		    Message Header
//		        CSeq: 2 BYE
//		            Sequence Number: 2
//		            Method: BYE
//		        Via: SIP/2.0/UDP 10.10.10.248:5061;branch=z9hG4bK46dcecc3-82fe-e011-880f-68b599e80ba8;rport
//		            Transport: UDP
//		            Sent-by Address: 10.10.10.248
//		            Sent-by port: 5061
//		            Branch: z9hG4bK46dcecc3-82fe-e011-880f-68b599e80ba8
//		            RPort: rport
//		        From: <sip:guic@10.10.10.248:5061>;tag=d45d67c1-82fe-e011-880f-68b599e80ba8
//		            SIP from address: sip:guic@10.10.10.248:5061
//		                SIP from address User Part: guic
//		                SIP from address Host Part: 10.10.10.248
//		                SIP from address Host Port: 5061
//		            SIP tag: d45d67c1-82fe-e011-880f-68b599e80ba8
//		        Call-ID: 134096379
//		        To: "majo" <sip:majo@10.10.10.248:5062>;tag=833511048
//		            SIP Display info: "majo"
//		            SIP to address: sip:majo@10.10.10.248:5062
//		                SIP to address User Part: majo
//		                SIP to address Host Part: 10.10.10.248
//		                SIP to address Host Port: 5062
//		            SIP tag: 833511048
//		        Contact: <sip:guic@10.10.10.248:5061>
//		            Contact-URI: sip:guic@10.10.10.248:5061
//		                Contactt-URI User Part: guic
//		                Contact-URI Host Part: 10.10.10.248
//		                Contact-URI Host Port: 5061
//		        Content-Length: 0
//		        Max-Forwards: 70
