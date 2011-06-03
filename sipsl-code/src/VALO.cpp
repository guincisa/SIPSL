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
#include <assert.h>
#include <string.h>



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
#ifndef TRNSCT_SM_H
#include "TRNSCT_SM.h"
#endif
SIPUTIL SipUtil;

VALO::VALO(ENGINE* _e, CALL_OSET* _co):ALO(_e, _co){}
VALO::~VALO(void){

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
}

void VALO::onInvite(MESSAGE* _message){

    TIMEDEF
    SETNOW
    PROFILE("VALO::onInvite")

	DEBALO("VALO::onInvite", _message->getHeadSipRequest().getContent())

	CREATEMESSAGE(message, _message, SODE_ALOPOINT, SODE_TRNSCT_CL)

    //MLF2
	message->setSourceMessage(_message->getSourceMessage());

    //Deleted in action_SV
    //PURGEMESSAGE(_message)

	try {
		DEBALO("VALO message->getHeadRoute().getRoute().getHostName()",message->getHeadRoute()->getRoute().getHostName())
		DEBALO("VALO message->getHeadRoute().getRoute().getPort()",message->getHeadRoute()->getRoute().getPort())
		DEBALO("VALO","remove route")
		message->dropHeader("Route:");
	}
	catch(HeaderException e){
		DEBALO("Exception ", e.getMessage())
	}

	//TODO use Registrar
	stringstream tmps ;
	tmps << "INVITE sip:GUGLISIPSL@"<<BPHONE<<":5062 SIP/2.0";
	message->setHeadSipRequest(tmps.str());

	//message->setHeadSipRequest("INVITE sip:GUGLISIPSL@bphone.gugli.com:5062 SIP/2.0");

	stringstream cs;
	cs << call_oset->getNextSequence("INVITE_B");
	cs << " INVITE";
	message->setGenericHeader("CSeq:",cs.str());

	//Standard changes
	SipUtil.genBInvitefromAInvite(_message->getSourceMessage(), message, getSUDP(), call_oset->getCallId_Y());
	message->setGenericHeader("Contact:", "<sip:sipsl@grog:5060>");

	message->dumpMessageBuffer();

	message->compileMessage();


	DEBMESSAGE("New outgoing b2b message", message)

	//TODO ???
	DEBALO("STORE CSeq sequence number for ack", message->getHeadCSeq().getSequence())
	NEWPTR(int*, CSeqB2BINVITE, int(message->getHeadCSeq()),"CSeqB2BINVITE")
	ctxt_store.insert(pair<string, void*>("CSeqB2BINVITE", (void*) CSeqB2BINVITE ));


	DEBALO("STORING now call id", message->getHeadCallId())
	call_oset->setCallId_Y(message->getHeadCallId());

	//store this invites
	//only possible because both invites are locked during the creation of their SM
	ctxt_store.insert(pair<string, void*>("invite_a", (void*) _message->getSourceMessage() ));
	ctxt_store.insert(pair<string, void*>("invite_b", (void*) message ));

#ifdef NONESTEDPW
	int r;
	call_oset->getSL_CO()->call(message,r);
#else
	sl_cc->p_w(message);
#endif

     PRINTDIFF("VALO::onInvite")

}
void VALO::onAck(MESSAGE* _message){

    TIMEDEF
    SETNOW
    PROFILE("VALO::onAck")

	DEBALO("VALO::onAck",_message->getHeadSipRequest().getContent())

	//V5 trying to build the ACK using the ACK_A instead of INVITE b
	CREATEMESSAGE(newack, _message, SODE_ALOPOINT, SODE_SMCLPOINT)
	//set as source the original ack, needed to identify call_oset_x when back to call control
	newack->setSourceMessage(_message);

	//Remove route
	try {
		DEBALO("VALO message->getHeadRoute().getRoute().getHostName()",newack->getHeadRoute()->getRoute().getHostName())
		DEBALO("VALO message->getHeadRoute().getRoute().getPort()",newack->getHeadRoute()->getRoute().getPort())
		DEBALO("VALO","remove route")
		newack->dropHeader("Route:");
	}
	catch(HeaderException e){
		DEBALO("Exception ", e.getMessage())
	}

	//change request
	DEBALO("VALO ", newack->getHeadSipRequest().getContent())
	stringstream tmps ;
	tmps << "ACK sip:GUGLISIPSL@"<<BPHONE<<":5062 SIP/2.0";
	newack->setHeadSipRequest(tmps.str());

	//newack->setHeadSipRequest("ACK sip:GUGLISIPSL@bphone.gugli.com:5062 SIP/2.0");

	//don't change CSEQ
//	char buff[64];
//	sprintf(buff, "%d ACK", call_oset->getNextSequence("ACK"));
//	newack->replaceHeadCSeq(buff);
//	DEBALO("VALO","Cseq")

	//Purge SDP
	newack->purgeSDP();
	newack->addGenericHeader("Content-Length:","0");

	//TOTAG
	char toTmp[512];
	map<string, void*> ::iterator p;
	p = ctxt_store.find("totag_200ok_b");
	string toTagB = *((string*)p->second);
//	sprintf(toTmp, "%s %s;tag=%s",newack->getHeadTo()->getNameUri().c_str(), newack->getHeadTo()->getC_AttSipUri().getContent().c_str(),toTagB.c_str());
//	string toTmpS(toTmp);
	string tmpto = newack->getGenericHeader("To:");
	tmpto += toTagB.c_str();
	DEBALO("******** TO new" , tmpto)

	newack->setGenericHeader("To:",tmpto);
//	DEBALO("TO",newack->getHeadTo()->getContent())
//	DEBALO("TO",newack->getHeadTo()->getC_AttSipUri().getContent())
//	DEBALO("TO",newack->getHeadTo()->getNameUri())
//	DEBALO("TO",newack->getHeadTo()->getC_AttUriParms().getContent())

	DEBALO("New ACK via","")
	map<string, void*> ::iterator p2;
	p2 = ctxt_store.find("allvia_200ok_b");
	string allVia = *((string*)p2->second);
	newack->popVia();
	newack->pushNewVia(allVia);
	DEBALO("New ACK via",allVia.c_str())

	// THIS ACK needs the B_INVITE call id and
	// 200 ok from B from and to headers!!!
	DEBALO("CHECK THIS CALLID +++++++++++", call_oset->getCallId_Y());

	p = ctxt_store.find("tohead_200ok_b");
	string tohead_200ok_b = *((string*)p->second);
	p = ctxt_store.find("fromhead_200ok_b");
	string fromhead_200ok_b = *((string*)p->second);
	p = ctxt_store.find("callid_200ok_b");
	string callid_200ok_b = *((string*)p->second);
	p = ctxt_store.find("CSeqB2BINVITE");

	//TODO cseq for ACK
	int CSeqB2BINVITE = *((int*)p->second);

	DEBALO("CHECK THIS TO HEAD +++++++++++", tohead_200ok_b);
	DEBALO("CHECK THIS FROM HEAD +++++++++++", fromhead_200ok_b);
	DEBALO("CHECK THIS CSeq  +++++++++++", CSeqB2BINVITE);

	DEBALO("CHECK THIS CALL ID from context map +++++++++++", callid_200ok_b);
	DEBALO("CHECK THIS CALL ID from call_oset +++++++++++", call_oset->getCallId_Y());

//	!!!!
//	To: To: <sip:gugli_linphone@172.21.160.181:5061>;tag=9f7bc830
//	From: From: <sip:gugli_twinkle@guglicorp.com>;tag=9d448d81276175425117411

	newack->setGenericHeader("To:", tohead_200ok_b);
	newack->setGenericHeader("From:", fromhead_200ok_b);
	newack->setGenericHeader("Call-ID:", call_oset->getCallId_Y());
	string cst;
	cst += CSeqB2BINVITE;
	cst += " ";
	cst += newack->getHeadCSeqMethod();
	newack->setGenericHeader("CSeq:", cst);

	char viatmp[512];
	sprintf(viatmp, "SIP/2.0/UDP %s:%d;branch=z9hG4bK%s;rport",getSUDP()->getDomain().c_str(),getSUDP()->getPort(),newack->getKey().c_str());
	string viatmpS(viatmp);
	string tmpv = "SIP/2.0/UDP ";
	tmpv += getSUDP()->getDomain();
	newack->popVia();
	newack->pushNewVia(viatmpS);
	DEBALO("newack->pushHeadVia(viatmpS);", viatmpS)

	newack->compileMessage();
	//newack->dumpVector();

	//done in state machine client
//	newack->setLock();
//	call_oset->insertLockedMessage(newack);


	DEBMESSAGE("New outgoing b2b message", newack)
#ifdef NONESTEDPW
	int r;
	call_oset->getSL_CO()->call(newack,r);
#else
	sl_cc->p_w(newack);
#endif


    PRINTDIFF("VALO::onAck")


}
void VALO::onAckNoTrnsct(MESSAGE* _message){

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
	message->setSourceMessage(_message->getSourceMessage());


	DEBALO("BYE DIRECTION",_message->getHeadCSeq().getContent() << " " << _message->getRequestDirection())

	if (_message->getRequestDirection() == SODE_FWD ) {

		//message->setDestEntity(SODE_TRNSCT_CL);

		stringstream tmps;
		tmps << "BYE sip:GUGLISIPSL@"<<BPHONE<<":5062 SIP/2.0";
		message->setHeadSipRequest(tmps.str());

		stringstream viatmp;
		viatmp << "SIP/2.0/UDP "<<getSUDP()->getDomain().c_str()<<":"<<getSUDP()->getPort()<<";branch=z9hG4bK"<<message->getKey()<<";rport";
		message->popVia();
		message->pushNewVia(viatmp.str());

		string cst;
		cst += call_oset->getNextSequence("INVITE_B");
		cst += " BYE";
		message->setGenericHeader("CSeq:", cst);


		map<string, void*> ::iterator p;
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
#else
		sl_cc->p_w(message);
#endif

	}
	else if (_message->getRequestDirection() == SODE_BKWD ) {

		map<string, void*> ::iterator p;
		p = ctxt_store.find("invite_a");
		MESSAGE* __message = (MESSAGE*)p->second;

		DEBALO("Search for INVITE A sequence", call_oset->getCurrentSequence("INVITE_A"));
		//message->setDestEntity(SODE_TRNSCT_CL);

		//Request has to be made using INVITE_A via address
		string viatmps = __message->getViaLine();
		DEBY
		stringstream _ss;
		_ss << "BYE sip:ceppadim@" << __message->getViaUriHost() << ":" << __message->getViaUriPort() << " SIP/2.0";
		message->setHeadSipRequest(_ss.str());

		//
		message->popVia();
		stringstream viatmp;
		viatmp << "SIP/2.0/UDP "<<getSUDP()->getDomain()<<":"<<getSUDP()->getPort()<<";branch=z9hG4bK"<<message->getKey()<<";rport";
		message->pushNewVia(viatmp.str());

		message->setGenericHeader("CSeq:",call_oset->getCurrentSequence("INVITE_A") + " BYE");

		p = ctxt_store.find("tohead_200ok_a");
		string tohead_200ok_a = *((string*)p->second);
		p = ctxt_store.find("fromhead_200ok_a");
		string fromhead_200ok_a = *((string*)p->second);
		// are inverted
		message->setGenericHeader("To:",fromhead_200ok_a);
		message->setGenericHeader("From:",tohead_200ok_a);
		message->setGenericHeader("Call-ID:", call_oset->getCallId_X());


		message->compileMessage();
		//message->dumpVector();
		//done in client sm
		//message->setLock();
		//call_oset->insertLockedMessage(message);
#ifdef NONESTEDPW
		int r;
		call_oset->getSL_CO()->call(message,r);
#else
		sl_cc->p_w(message);
#endif

	}
        PRINTDIFF("VALO::onBye")

}
void VALO::on200Ok(MESSAGE* _message){

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
		NEWPTR(string*, fromhead, string(_message->getGenericHeader("To:")),"fromhead_200ok_b")
		DEBALO("STORE FROM HEAD of 200 ok", fromhead)
		TRYCATCH(ctxt_store.insert(pair<string, void*>("fromhead_200ok_b", (void*) fromhead )))
	}

	//this shoudl go into call_oset call_idy
	if (ctxt_store.find("callid_200ok_b") == ctxt_store.end()){
		NEWPTR(string*, callid_200ok_b, string(_message->getGenericHeader("CallID:")),"callid_200ok_b")
		DEBALO("STORE CALL ID of 200 ok", callid_200ok_b)
		TRYCATCH(ctxt_store.insert(pair<string, void*>("callid_200ok_b", (void*) callid_200ok_b )))
	}

	DEBALO("on200Ok MESSAGE GENERATOR", __message)
	CREATEMESSAGE(ok_x, __message, SODE_ALOPOINT , SODE_TRNSCT_SV)

	//ok_x->setTypeOfInternal(TYPE_MESS);

	DEBALO("ok_x","SIP/2.0 200 OK")

	if (ok_x->hasSDP()){
		//SDP must copy the SDP from incoming OK and put here
		vector< pair<char*, bool> > sdps  = _message->getSDP();
		ok_x->purgeSDP();
		ok_x->dropHeader("Content-Length:");

		ok_x->setSDP(sdps);
	}

	ok_x->setGenericHeader("Contact:","<sip:sipsl@grog:5060>");

	SipUtil.genASideReplyFromBReply(_message, __message, ok_x);
	ok_x->compileMessage();

	if (ctxt_store.find("tohead_200ok_a") == ctxt_store.end()){
		DEBALO("STORE tags of 200 OK to A",ok_x->getHeadTo()->getContent() << "]["<<ok_x->getHeadFrom()->getContent())
		NEWPTR(string*, tohead_a, string(ok_x->getGenericHeader("To:")),"tohead_200ok_a")
		TRYCATCH(ctxt_store.insert(pair<string, void*>("tohead_200ok_a", (void*) tohead_a )))
	}
	if (ctxt_store.find("fromhead_200ok_a") == ctxt_store.end()){
		DEBALO("STORE FROM HEAD of 200 ok", ok_x->getHeadTo()->getContent())
		NEWPTR(string*, fromhead_a, string(ok_x->getGenericHeader("From:")),"fromhead_200ok_a")
		TRYCATCH(ctxt_store.insert(pair<string, void*>("fromhead_200ok_a", (void*) fromhead_a )))
	}

//	//TODO the 200 OK B for now is deleted
//	PURGEMESSAGE(_message)

	//ok_x->dumpVector();
#ifdef NONESTEDPW
		int r;
		call_oset->getSL_CO()->call(ok_x,r);
#else
		sl_cc->p_w(ok_x);
#endif

    PRINTDIFF("VALO::on200Ok")


}

