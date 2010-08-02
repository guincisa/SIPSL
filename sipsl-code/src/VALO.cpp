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



#ifndef MESSAGE_H
#include "MESSAGE.h"
#endif
#ifndef VALO_H
#include "VALO.h"
#endif
#ifndef SIPUTIL_H
#include "SIPUTIL.h"
#endif

SIPUTIL SipUtil;

VALO::VALO(ENGINE* _e, CALL_OSET* _co):ALO(_e, _co){}

void VALO::onInvite(MESSAGE* _message){

	DEBOUT("VALO::onInvite", _message->getHeadSipRequest().getContent())

	CREATEMESSAGE(message, _message, SODE_ALOPOINT)
	message->setDestEntity(SODE_TRNSCT_CL);

	try {
		DEBOUT("VALO message->getHeadRoute().getRoute().getHostName()",message->getHeadRoute().getRoute().getHostName())
		DEBOUT("VALO message->getHeadRoute().getRoute().getPort()",message->getHeadRoute().getRoute().getPort())
		DEBOUT("VALO","remove route")
		message->removeHeadRoute();
	}
	catch(HeaderException e){
		DEBOUT("Exception ", e.getMessage())
	}

	//TODO use Registrar
	message->setHeadSipRequest("INVITE sip:GUGLISIPSL@bphone.gugli.com:5062 SIP/2.0");

	message->replaceHeadCSeq(call_oset->getNextSequence("INVITE_B"), "INVITE");

	//Standard changes
	SipUtil.genBInvitefromAInvite(_message, message, getSUDP());
	message->replaceHeadContact("<sip:sipsl@grog:5060>");
	DEBOUT("NEW CONTACT", message->getHeadContact().getContent())

	message->compileMessage();
	message->dumpVector();

	DEBMESSAGE("New outgoing b2b message", message->getIncBuffer())

	//TODO ???
	DEBOUT("STORE CSeq sequence number for ack", message->getHeadCSeq().getSequence())
	int* CSeqB2BINIVTE = new int(message->getHeadCSeq().getSequence());
	ctxt_store.insert(pair<string, void*>("CSeqB2BINIVTE", (void*) CSeqB2BINIVTE ));

	message->setLock();

	DEBOUT("STORING now call id", message->getHeadCallId().getContent())
	call_oset->setCallId_Y(message->getHeadCallId().getContent());

	//store this invites
	ctxt_store.insert(pair<string, void*>("invite_b", (void*) message ));
	ctxt_store.insert(pair<string, void*>("invite_a", (void*) _message ));

	sl_cc->p_w(message);

}
void VALO::onAck(MESSAGE* _message){
	DEBOUT("VALO::onAck",_message->getHeadSipRequest().getContent())


	//V5 trying to build the ACK using the ACK_A instead of INVITE b
	CREATEMESSAGE(newack, _message, SODE_ALOPOINT)
	//set as source the original ack, needed to identify call_oset_x when back to call control
	newack->setSourceMessage(_message);
	newack->setDestEntity(SODE_SMCLPOINT);

	//Remove route
	try {
		DEBOUT("VALO message->getHeadRoute().getRoute().getHostName()",newack->getHeadRoute().getRoute().getHostName())
		DEBOUT("VALO message->getHeadRoute().getRoute().getPort()",newack->getHeadRoute().getRoute().getPort())
		DEBOUT("VALO","remove route")
		newack->removeHeadRoute();
	}
	catch(HeaderException e){
		DEBOUT("Exception ", e.getMessage())
	}

	//change request
	DEBOUT("VALO ", newack->getHeadSipRequest().getContent())
	newack->setHeadSipRequest("ACK sip:GUGLISIPSL@bphone.gugli.com:5062 SIP/2.0");

	//don't change CSEQ
//	char buff[64];
//	sprintf(buff, "%d ACK", call_oset->getNextSequence("ACK"));
//	newack->replaceHeadCSeq(buff);
//	DEBOUT("VALO","Cseq")

	//Purge SDP
	newack->purgeSDP();
	newack->addGenericHeader("Content-Length:","0");

//
	//TOTAG
	char toTmp[512];
	map<string, void*> ::iterator p;
	p = ctxt_store.find("totag_200OK_b");
	string toTagB = *((string*)p->second);
	sprintf(toTmp, "%s %s;tag=%s",newack->getHeadTo().getNameUri().c_str(), newack->getHeadTo().getC_AttSipUri().getContent().c_str(),toTagB.c_str());
	string toTmpS(toTmp);
	DEBOUT("******** TO new" , toTmpS)
	newack->replaceHeadTo(toTmpS);
	DEBOUT("TO",newack->getHeadTo().getContent())
	DEBOUT("TO",newack->getHeadTo().getC_AttSipUri().getContent())
	DEBOUT("TO",newack->getHeadTo().getNameUri())
	DEBOUT("TO",newack->getHeadTo().getC_AttUriParms().getContent())

	DEBOUT("NEW ACK via","")
	map<string, void*> ::iterator p2;
	p2 = ctxt_store.find("allvia_200OK_b");
	string allVia = *((string*)p2->second);
	newack->purgeSTKHeadVia();
	newack->pushHeadVia(allVia);
	DEBOUT("NEW ACK via",allVia.c_str())

	// THIS ACK needs the B_INVITE call id and
	// 200 ok from B from and to headers!!!
	DEBOUT("CHECK THIS CALLID +++++++++++", call_oset->getCallId_Y());

	p = ctxt_store.find("tohead_200ok_b");
	string tohead_200ok_b = *((string*)p->second);
	p = ctxt_store.find("fromhead_200ok_b");
	string fromhead_200ok_b = *((string*)p->second);
	p = ctxt_store.find("callid_200ok_b");
	string callid_200ok_b = *((string*)p->second);
	p = ctxt_store.find("CSeqB2BINIVTE");

	//TODO cseq for ACK
	int CSeqB2BINIVTE = *((int*)p->second);

	DEBOUT("CHECK THIS TO HEAD +++++++++++", tohead_200ok_b);
	DEBOUT("CHECK THIS FROM HEAD +++++++++++", fromhead_200ok_b);
	DEBOUT("CHECK THIS CSeq  +++++++++++", CSeqB2BINIVTE);

	DEBOUT("CHECK THIS CALL ID from context map +++++++++++", callid_200ok_b);
	DEBOUT("CHECK THIS CALL ID from call_oset +++++++++++", call_oset->getCallId_Y());

//	!!!!
//	To: To: <sip:gugli_linphone@172.21.160.181:5061>;tag=9f7bc830
//	From: From: <sip:gugli_twinkle@guglicorp.com>;tag=9d448d81276175425117411

	newack->replaceHeadTo(tohead_200ok_b);
	newack->replaceHeadFrom(fromhead_200ok_b);
	newack->setGenericHeader("Call-ID:", call_oset->getCallId_Y());
	newack->getHeadCSeq().setSequence(CSeqB2BINIVTE);

	char viatmp[512];
	sprintf(viatmp, "SIP/2.0/UDP %s:%d;branch=z9hG4bK%s;rport",getSUDP()->getDomain().c_str(),getSUDP()->getPort(),newack->getKey().c_str());
	string viatmpS(viatmp);
	newack->purgeSTKHeadVia();
	newack->pushHeadVia(viatmpS);
	DEBOUT("newack->pushHeadVia(viatmpS);", viatmpS)

	newack->compileMessage();
	newack->dumpVector();


	DEBMESSAGE("New outgoing b2b message", newack->getIncBuffer())
	sl_cc->p_w(newack);
}
void VALO::onAckNoTrnsct(MESSAGE* _message){

	DEBASSERT("VALO::onAckNoTrnsct")

//	DEBOUT("VALO::onAckNoTrnsct",_message->getHeadSipRequest().getContent())
//
//	//V4
//	//get invite sent to b
////	DEBOUT("VALO onAck", call_oset->getInviteB()->getIncBuffer())
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
//		DEBOUT("VALO message->getHeadRoute().getRoute().getHostName()",newack->getHeadRoute().getRoute().getHostName())
//		DEBOUT("VALO message->getHeadRoute().getRoute().getPort()",newack->getHeadRoute().getRoute().getPort())
//		DEBOUT("VALO","remove route")
//		newack->removeHeadRoute();
//	}
//	catch(HeaderException e){
//		DEBOUT("Exception ", e.getMessage())
//	}
//
//	//change request
//	DEBOUT("VALO ", newack->getHeadSipRequest().getContent())
//	newack->setHeadSipRequest("ACK sip:GUGLISIPSL@bphone.gugli.com:5062 SIP/2.0");
//
//	//don't change CSEQ
////	char buff[64];
////	sprintf(buff, "%d ACK", call_oset->getNextSequence("ACK"));
////	newack->replaceHeadCSeq(buff);
////	DEBOUT("VALO","Cseq")
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
//	DEBOUT("******** TO new" , toTmpS)
//	newack->replaceHeadTo(toTmpS);
//	DEBOUT("TO",newack->getHeadTo().getContent())
//	DEBOUT("TO",newack->getHeadTo().getC_AttSipUri().getContent())
//	DEBOUT("TO",newack->getHeadTo().getNameUri())
//	DEBOUT("TO",newack->getHeadTo().getC_AttUriParms().getContent())
//
//	DEBOUT("NEW ACK via","")
//	map<string, void*> ::iterator p2;
//	p2 = ctxt_store.find("allvia");
//	string allVia = *((string*)p2->second);
//	newack->purgeSTKHeadVia();
//	newack->pushHeadVia(allVia);
//	DEBOUT("NEW ACK via",allVia.c_str())
//
//	newack->compileMessage();
//	newack->dumpVector();
//
//	// THIS ACK needs the B_INVITE call id and
//	// 200 ok from B from and to headers!!!
//	DEBOUT("CHECK THIS CALLID +++++++++++", call_oset->getCallId_Y());
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
//	DEBOUT("CHECK THIS TO HEAD +++++++++++", tohead_200ok_b);
//	DEBOUT("CHECK THIS FROM HEAD +++++++++++", fromhead_200ok_b);
//	DEBOUT("CHECK THIS CSeq  +++++++++++", CSeqB2BINIVTE);
//
//	DEBOUT("CHECK THIS CALL ID from context map +++++++++++", callid_200ok_b);
//	DEBOUT("CHECK THIS CALL ID from call_oset +++++++++++", call_oset->getCallId_Y());
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

	DEBMESSAGE("VALO::onBye", _message->getIncBuffer())

	//The direction of the bye is recognized by the CSEQ

//	map<string, void*> ::iterator p;
//	p = ctxt_store.find("invite_b");
//	MESSAGE* invite_b = ((MESSAGE*)p->second);
//
//	CREATEMESSAGE(message, invite_b, SODE_ALOPOINT)

	CREATEMESSAGE(message, _message, SODE_ALOPOINT)

	DEBOUT("BYE DIRECTION",_message->getHeadCSeq().getContent() << " " << _message->getRequestDirection())

	if (_message->getRequestDirection() == SODE_FWD ) {

		message->setDestEntity(SODE_TRNSCT_CL);

		message->setHeadSipRequest("BYE sip:GUGLISIPSL@bphone.gugli.com:5062 SIP/2.0");

		char viatmp[512];
		sprintf(viatmp, "SIP/2.0/UDP %s:%d;branch=z9hG4bK%s;rport",getSUDP()->getDomain().c_str(),getSUDP()->getPort(),message->getKey().c_str());
		string viatmpS(viatmp);
		message->purgeSTKHeadVia();
		message->pushHeadVia(viatmpS);

		//TODO FIX THIS!
		//must understadn here which dialog I am closing
		message->replaceHeadCSeq(call_oset->getNextSequence("INVITE_B"), "BYE");
		DEBOUT("VALO Cseq new", message->getGenericHeader("CSeq"))

		map<string, void*> ::iterator p;
		p = ctxt_store.find("tohead_200ok_b");
		string tohead_200ok_b = *((string*)p->second);
		p = ctxt_store.find("fromhead_200ok_b");
		string fromhead_200ok_b = *((string*)p->second);
		p = ctxt_store.find("callid_200ok_b");
		string callid_200ok_b = *((string*)p->second);
		message->replaceHeadTo(tohead_200ok_b);
		message->replaceHeadFrom(fromhead_200ok_b);
		message->setGenericHeader("Call-ID:", call_oset->getCallId_Y());


		message->compileMessage();
		message->dumpVector();
		sl_cc->p_w(message);

	}
	else if (_message->getRequestDirection() == SODE_BKWD ) {

		map<string, void*> ::iterator p;
		p = ctxt_store.find("invite_a");
		MESSAGE* __message = (MESSAGE*)p->second;

		DEBOUT("Search for INVITE A sequence", call_oset->getCurrentSequence("INVITE_A"));
		message->setDestEntity(SODE_TRNSCT_CL);

		//Request has to be made using INVITE_A via address
		C_HeadVia* viatmps= (C_HeadVia*) __message->getSTKHeadVia().top();
		DEBOUT("BYE and rest of request", "BYE sip:ceppadim@"+viatmps->getC_AttVia().getS_HostHostPort().getContent() + " " +__message->getHeadSipRequest().getS_AttSipVersion().getContent())
		message->setHeadSipRequest("BYE sip:ceppadim@"+viatmps->getC_AttVia().getS_HostHostPort().getContent() + " " +__message->getHeadSipRequest().getS_AttSipVersion().getContent());

		char viatmp[512];
		sprintf(viatmp, "SIP/2.0/UDP %s:%d;branch=z9hG4bK%s;rport",getSUDP()->getDomain().c_str(),getSUDP()->getPort(),message->getKey().c_str());
		string viatmpS(viatmp);
		message->purgeSTKHeadVia();
		message->pushHeadVia(viatmpS);

		//TODO FIX THIS!
		//must understadn here which dialog I am closing
		message->replaceHeadCSeq(call_oset->getCurrentSequence("INVITE_A"), "BYE");
		DEBOUT("VALO Cseq new", message->getGenericHeader("CSeq"))

		p = ctxt_store.find("tohead_200ok_a");
		string tohead_200ok_a = *((string*)p->second);
		p = ctxt_store.find("fromhead_200ok_a");
		string fromhead_200ok_a = *((string*)p->second);
		// are inverted
		message->replaceHeadTo(fromhead_200ok_a);
		message->replaceHeadFrom(tohead_200ok_a);

		message->setGenericHeader("Call-ID:", call_oset->getCallId_X());


		message->compileMessage();
		message->dumpVector();
		sl_cc->p_w(message);
	}

}
void VALO::on200Ok(MESSAGE* _message){

	TRNSCT_SM* trnsct_cl = call_oset->getTrnsctSm(_message->getHeadCSeq().getMethod().getContent(), SODE_TRNSCT_CL, ((C_HeadVia*) _message->getSTKHeadVia().top())->getC_AttVia().getViaParms().findRvalue("branch"));
	MESSAGE* __message = ((TRNSCT_SM*)trnsct_cl)->getA_Matrix();

	NEWPTR(string*, totag, string(_message->getHeadTo().getC_AttUriParms().getTuples().findRvalue("tag")))
	DEBOUT("STORE totag", totag)
	TRYCATCH(ctxt_store.insert(pair<string, void*>("totag_200OK_b", (void*) totag )))

	stack<C_HeadVia*>	tmpViaS;
	tmpViaS = _message->getSTKHeadVia();
	NEWPTR(string*, allvia, string(tmpViaS.top()->getC_AttVia().getContent()))
	DEBOUT("STORE totag", allvia)
	TRYCATCH(ctxt_store.insert(pair<string, void*>("allvia_200OK_b", (void*) allvia )))

	// Need to store the FROM and TO
	// To create the ACK
	NEWPTR(string*, tohead,  string(_message->getHeadTo().getContent()) )
	DEBOUT("STORE TO HEAD ok 200 ok from B", tohead)
	TRYCATCH(ctxt_store.insert(pair<string, void*>("tohead_200ok_b", (void*) tohead )))
	NEWPTR(string*, fromhead, string(_message->getHeadFrom().getContent()))
	DEBOUT("STORE FROM HEAD of 200 ok", fromhead)
	TRYCATCH(ctxt_store.insert(pair<string, void*>("fromhead_200ok_b", (void*) fromhead )))

	//this shoudl go into call_oset call_idy
	NEWPTR(string*, callid_200ok_b, string(_message->getHeadCallId().getContent()))
	DEBOUT("STORE CALL ID of 200 ok", callid_200ok_b)
	TRYCATCH(ctxt_store.insert(pair<string, void*>("callid_200ok_b", (void*) callid_200ok_b )))

	DEBOUT("on200Ok MESSAGE GENERATOR", __message)
	CREATEMESSAGE(ok_x, __message, SODE_ALOPOINT)
	ok_x->setDestEntity(SODE_TRNSCT_SV);
	ok_x->typeOfInternal = TYPE_MESS;

	DEBOUT("ok_x","SIP/2.0 200 OK")

	if (ok_x->getSDPSize() != 0 ){
		//SDP must copy the SDP from incoming OK and put here
		vector<string> __sdp = _message->getSDP();
		ok_x->purgeSDP();
		ok_x->dropHeader("Content-Length:");

		DEBOUT("PURGED SDP","")
		ok_x->dumpVector();
		ok_x->importSDP(__sdp);
		ok_x->dumpVector();
	}

	ok_x->replaceHeadContact("<sip:sipsl@grog:5060>");

	SipUtil.genASideReplyFromBReply(_message, __message, ok_x);
	ok_x->compileMessage();

	DEBOUT("STORE tags of 200 OK to A",ok_x->getHeadTo().getContent() << "]["<<ok_x->getHeadFrom().getContent())
	NEWPTR(string*, tohead_a, string(ok_x->getHeadTo().getContent()))
	TRYCATCH(ctxt_store.insert(pair<string, void*>("tohead_200ok_a", (void*) tohead_a )))
	DEBOUT("STORE FROM HEAD of 200 ok", ok_x->getHeadTo().getContent())
	NEWPTR(string*, fromhead_a, string(ok_x->getHeadFrom().getContent()))
	TRYCATCH(ctxt_store.insert(pair<string, void*>("fromhead_200ok_a", (void*) fromhead_a )))

	ok_x->dumpVector();

	sl_cc->p_w(ok_x);

}

