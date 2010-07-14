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

		CREATEMESSAGE(message, _message, SODE_ALOPOINT)

		message->setDestEntity(SODE_TRNSCT_CL);

		message->setSourceMessage(_message);

		try {
			DEBOUT("VALO message->getHeadRoute().getRoute().getHostName()",message->getHeadRoute().getRoute().getHostName())
			DEBOUT("VALO message->getHeadRoute().getRoute().getPort()",message->getHeadRoute().getRoute().getPort())
			DEBOUT("VALO","remove route")
			message->removeHeadRoute();
		}
		catch(HeaderException e){
			DEBOUT("Exception ", e.getMessage())
		}

		//TODO use registrar
		//change request
		// INVITE INVITE sip:guic2@127.0.0.1:5061 SIP/2.0
		DEBOUT("VALO ", message->getHeadSipRequest().getContent())
		message->setHeadSipRequest("INVITE sip:GUGLISIPSL@bphone.gugli.com:5061 SIP/2.0");

		//New transaction
		message->replaceHeadCSeq(call_oset->getNextSequence("INVITE_B"), "INVITE");

		//Standard changes
		SipUtil.genBInvitefromAInvite(_message, message, getSUDP());

		if (_message->getSDPSize() != 0 ){
			//SDP must copy the SDP from incoming OK and put here
			vector<string> __sdp = _message->getSDP();
			message->purgeSDP();
			DEBOUT("PURGED SDP","")
			message->importSDP(__sdp);
		}

		message->compileMessage();
		message->dumpVector();
		DEBOUT("New outgoing b2b message", message->getIncBuffer())
		//create the transaction
		//v5 call_oset->createTransactionY(message);

		DEBOUT("STORE CSeq for ack", message->getGenericHeader("CSeq:"))
		string* CSeqB2BINIVTE = new string(message->getGenericHeader("CSeq:"));
		ctxt_store.insert(pair<string, void*>("CSeqB2BINIVTE", (void*) CSeqB2BINIVTE ));

		message->setLock();

		DEBOUT("STORING now call id", message->getHeadCallId().getContent())
		call_oset->setCallId_Y(message->getHeadCallId().getContent());

		//store this invite
		ctxt_store.insert(pair<string, void*>("invite_b", (void*) message ));

		sl_cc->p_w(message);

}
void VALO::onAck(MESSAGE* _message){
	DEBASSERT("VALO::onAck")
}
void VALO::onAckNoTrnsct(MESSAGE* _message){

	DEBOUT("VALO::onAckNoTrnsct",_message->getHeadSipRequest().getContent())


	//V4
	//get invite sent to b
//	DEBOUT("VALO onAck", call_oset->getInviteB()->getIncBuffer())
//	CREATEMESSAGE(message, call_oset->getInviteB(), SODE_ALOPOINT)
//	//CREATEMESSAGE(message, _message, SODE_ALOPOINT)

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
	newack->setHeadSipRequest("ACK sip:GUGLISIPSL@bphone.gugli.com:5061 SIP/2.0");

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
	p = ctxt_store.find("totag");
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
	p2 = ctxt_store.find("allvia");
	string allVia = *((string*)p2->second);
	newack->purgeSTKHeadVia();
	newack->pushHeadVia("Via: " + allVia);
	DEBOUT("NEW ACK via",allVia.c_str())

	newack->compileMessage();
	newack->dumpVector();

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
	string CSeqB2BINIVTE = *((string*)p->second);

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
	newack->setGenericHeader("CSeq:", CSeqB2BINIVTE);

	newack->compileMessage();
	newack->dumpVector();


	DEBOUT("New outgoing b2b message", newack->getIncBuffer())
	sl_cc->p_w(newack);
//
//
}
void VALO::onBye(MESSAGE* _message){

	DEBOUT("VALO::onBye", _message->getIncBuffer())

	//The direction of the bye is recognized by the CSEQ

//	map<string, void*> ::iterator p;
//	p = ctxt_store.find("invite_b");
//	MESSAGE* invite_b = ((MESSAGE*)p->second);
//
//	CREATEMESSAGE(message, invite_b, SODE_ALOPOINT)

	CREATEMESSAGE(message, _message, SODE_ALOPOINT)

	int _dir = 1;

	if (_dir == 1 ) {

		message->setDestEntity(SODE_TRNSCT_CL);

		message->setHeadSipRequest("BYE sip:GUGLISIPSL@bphone.gugli.com:5061 SIP/2.0");

		char viatmp[512];
		sprintf(viatmp, "SIP/2.0/UDP %s:%d;branch=z9hG4bK%s;rport",getSUDP()->getDomain().c_str(),getSUDP()->getPort(),message->getKey().c_str());
		string viatmpS(viatmp);
		message->purgeSTKHeadVia();
		message->pushHeadVia("Via: " + viatmpS);

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
	else if (_dir == -1 ) {

		DEBOUT("Search for INVITE A sequence", call_oset->getCurrentSequence("INVITE_A"));
		TRNSCT_SM* trnsct_sv = call_oset->getTrnsctSm("INVITE", SODE_TRNSCT_SV, call_oset->getCurrentSequence("INVITE_A"));
		MESSAGE* __message = trnsct_sv->getA_Matrix();
		message->setDestEntity(SODE_TRNSCT_CL);

		DEBOUT("message->setHeadSipRequest(__message->getHeadSipRequest().getContent())", __message->getHeadSipRequest().getContent())
		DEBOUT("BYE and rest of request", "BYE*" <<__message->getHeadSipRequest().getC_AttSipUri().getContent() << "*"<<__message->getHeadSipRequest().getChangeS_AttSipVersion().getContent())
		message->setHeadSipRequest("BYE "+ __message->getHeadSipRequest().getC_AttSipUri().getContent() +" "+__message->getHeadSipRequest().getChangeS_AttSipVersion().getContent());

		char viatmp[512];
		sprintf(viatmp, "SIP/2.0/UDP %s:%d;branch=z9hG4bK%s;rport",getSUDP()->getDomain().c_str(),getSUDP()->getPort(),message->getKey().c_str());
		string viatmpS(viatmp);
		message->purgeSTKHeadVia();
		message->pushHeadVia("Via: " + viatmpS);

		//TODO FIX THIS!
		//must understadn here which dialog I am closing
		message->replaceHeadCSeq(call_oset->getCurrentSequence("INVITE_A"), "BYE");
		DEBOUT("VALO Cseq new", message->getGenericHeader("CSeq"))

		map<string, void*> ::iterator p;
		p = ctxt_store.find("tohead_200ok_a");
		string tohead_200ok_b = *((string*)p->second);
		p = ctxt_store.find("fromhead_200ok_a");
		string fromhead_200ok_b = *((string*)p->second);
		// are inverted
		message->replaceHeadTo(fromhead_200ok_b);
		message->replaceHeadFrom(tohead_200ok_b);

		message->compileMessage();
		message->dumpVector();
		sl_cc->p_w(message);
	}


//	if (_dir == 1 ) {
//		message->setDestEntity(SODE_SMCLPOINT);
//
//		message->setHeadSipRequest("BYE sip:SIPSLGUIC@172.21.160.162:5062 SIP/2.0");
//
//		message->purgeSDP();
//
//		char viatmp[512];
//		sprintf(viatmp, "SIP/2.0/UDP %s:%d;branch=z9hG4bK%s;rport",getSUDP()->getDomain().c_str(),getSUDP()->getPort(),_message->getKey().c_str());
//		string viatmpS(viatmp);
//		message->purgeSTKHeadVia();
//		message->pushHeadVia("Via: " + viatmpS);
//
//
//		message->replaceHeadCSeq(call_oset->getNextSequence("BYE"), "BYE");
//		DEBOUT("VALO Cseq new", message->getGenericHeader("CSeq"))
//		message->compileMessage();
//		message->dumpVector();
//
//		BTRANSMIT(message)
//
//		//sl_cc->p_w(message);
//
//	}
//
//
//	} else if (_dir == -1){
//		//PROBABLY WILL BE SENT TO ALO againive/Competitions/worldcup/matchday=18/day=1/match=300111113/index.html
//		message->setDestEntity(SODE_SMSVPOINT);
//		DEBOUT("PROBABLY WILL BE SENT TO ALO again","")
//	}
//
//	try {
//		DEBOUT("VALO message->getHeadRoute().getRoute().getHostName()",message->getHeadRoute().getRoute().getHostName())
//		DEBOUT("VALO message->getHeadRoute().getRoute().getPort()",message->getHeadRoute().getRoute().getPort())
//		DEBOUT("VALO","remove route")
//		message->removeHeadRoute();
//	}
//	catch(HeaderException e){
//		DEBOUT("Exception ", e.getMessage())
//	}
//
//	//change request
//	// INVITE INVITE sip:guic2@127.0.0.1:5061 SIP/2.0
//	DEBOUT("VALO ", message->getHeadSipRequest().getContent())
//	message->setHeadSipRequest("BYE sip:SIPSLGUIC@172.21.160.117:5062 SIP/2.0");
//
//
//	//TOTAG
//	char toTmp[512];
//	map<string, void*> ::iterator p;
//	p = ctxt_store.find("totag");
//	string toTagB = *((string*)p->second);
//	sprintf(toTmp, "%s %s;tag=%s",message->getHeadTo().getNameUri().c_str(), message->getHeadTo().getC_AttSipUri().getContent().c_str(),toTagB.c_str());
//	string toTmpS(toTmp);
//	DEBOUT("******** TO new" , toTmpS)
//	message->replaceHeadTo(toTmpS);
//
//	//VIA new branch
//	char viabranch[512];
//	p = ctxt_store.find("allvia");
//	C_AttVia tmpvia(*((string*)p->second));
//	DEBOUT("Old BYE via", *((string*)p->second))
//	sprintf(viabranch, "z9hG4bK%s",message->getKey().c_str());
//	tmpvia.getChangeViaParms().replaceRvalue("branch", viabranch);
//	message->purgeSTKHeadVia();
//	message->pushHeadVia("Via: " + tmpvia.getContent());
//	DEBOUT("New BYE Via",tmpvia.getContent())
//	DEBOUT("New BYE branch",viabranch)
//
//	message->purgeSDP();
//	message->dropHeader("Content-Type:");
//
//	message->compileMessage();
//	message->dumpVector();
//	DEBOUT("New outgoing b2b message", message->getIncBuffer())
//
//
//	//PURGE ACK
//	//PURGEMESSAGE(_message,"Purging ack from A")
//
//	sl_cc->p_w(message);
//
//	//build ack
//
}
void VALO::on200Ok(MESSAGE* _message){

	TRNSCT_SM* trnsct_cl = call_oset->getTrnsctSm(_message->getHeadCSeq().getMethod().getContent(), SODE_TRNSCT_CL, _message->getHeadCSeq().getSequence());

	MESSAGE* __message = ((TRNSCT_SM_INVITE_CL*)trnsct_cl)->getA_Matrix();

	DEBOUT("Store TO TAG ",_message->getHeadTo().getC_AttUriParms().getContent())
	DEBOUT("Store TO TAG value ",_message->getHeadTo().getC_AttUriParms().getTuples().findRvalue("tag"));

	string* totag = new string(_message->getHeadTo().getC_AttUriParms().getTuples().findRvalue("tag"));
	ctxt_store.insert(pair<string, void*>("totag", (void*) totag ));

	stack<C_HeadVia*>	tmpViaS;
	tmpViaS = _message->getSTKHeadVia();
	DEBOUT("200 ok get via rport and others 1", tmpViaS.top()->getContent())
	DEBOUT("200 ok get via rport and others 2", tmpViaS.top()->getC_AttVia().getContent())
	string* allvia = new string(tmpViaS.top()->getC_AttVia().getContent());
	DEBOUT("200 ok get via rport and others 3", *allvia)
	ctxt_store.insert(pair<string, void*>("allvia", (void*) allvia ));

	// Need to store the FROM and TO
	// To create the ACK
	DEBOUT("STORE TO HEAD ok 200 ok from B", _message->getHeadTo().getContent())
	string* tohead = new string(_message->getHeadTo().getContent());
	ctxt_store.insert(pair<string, void*>("tohead_200ok_b", (void*) tohead ));
	DEBOUT("STORE FROM HEAD of 200 ok", _message->getHeadTo().getContent())
	string* fromhead = new string(_message->getHeadFrom().getContent());
	ctxt_store.insert(pair<string, void*>("fromhead_200ok_b", (void*) fromhead ));



	//this shoudl go into call_oset call_idy
	DEBOUT("STORE CALL ID of 200 ok", _message->getHeadCallId().getContent())
	string* callid_200ok_b = new string(_message->getHeadCallId().getContent());
	ctxt_store.insert(pair<string, void*>("callid_200ok_b", (void*) callid_200ok_b ));


//
//		DEBOUT("200 ok get via rport and others 3", tmpViaS.top()->getC_AttVia().getViaParms().getContent())
//		DEBOUT("200 ok get via rport and others 4 branch", tmpViaS.top()->getC_AttVia().getViaParms().findRvalue("branch"))
//		DEBOUT("200 ok get via rport and others 4 rport", tmpViaS.top()->getC_AttVia().getViaParms().findRvalue("rport"))
//		string* rport = new string(tmpViaS.top()->getC_AttVia().getViaParms().findRvalue("rport"));
//		ctxt_store.insert(pair<string, void*>("rport", (void*) totag ));
//		DEBOUT("200 ok get via rport and others 4 received", tmpViaS.top()->getC_AttVia().getViaParms().findRvalue("received"))
//		string* received = new string(tmpViaS.top()->getC_AttVia().getViaParms().findRvalue("received"));
//		ctxt_store.insert(pair<string, void*>("received", (void*) totag ));

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
	string* tohead_a = new string(ok_x->getHeadTo().getContent());
	ctxt_store.insert(pair<string, void*>("tohead_200ok_a", (void*) tohead_a ));
	DEBOUT("STORE FROM HEAD of 200 ok", ok_x->getHeadTo().getContent())
	string* fromhead_a = new string(ok_x->getHeadFrom().getContent());
	ctxt_store.insert(pair<string, void*>("fromhead_200ok_a", (void*) fromhead_a ));

	ok_x->dumpVector();

	sl_cc->p_w(ok_x);

}

