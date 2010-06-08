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

	//	// do business logic...
	//	// create b2b invite related message & so on...
	//	//TODO clean this

		CREATEMESSAGE(message, _message, SODE_ALOPOINT)

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

		//change request
		// INVITE INVITE sip:guic2@127.0.0.1:5061 SIP/2.0
		DEBOUT("VALO ", message->getHeadSipRequest().getContent())
		message->setHeadSipRequest("INVITE sip:SIPSLGUIC@172.21.160.162:5062 SIP/2.0");

		//New transaction
		string tmp = "1 INVITE";
		message->replaceHeadCSeq(tmp);
		DEBOUT("VALO Cseq",tmp)

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

		message->setDestEntity(SODE_TRNSCT_CL);

		message->setLock();

		sl_cc->p_w(message);

}
void VALO::onAck(MESSAGE* _message){
//
//	//v4
//	//get invite sent to b
//	DEBOUT("VALO onAck", call_oset->getInviteB()->getIncBuffer())
//	CREATEMESSAGE(message, call_oset->getInviteB(), SODE_ALOPOINT)
//	//CREATEMESSAGE(message, _message, SODE_ALOPOINT)
//	//set as source the original ack, needed to identify call_oset_x when back to call control
//	message->setSourceMessage(_message);
//	message->setDestEntity(SODE_SMCLPOINT);
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
//	message->setHeadSipRequest("ACK sip:SIPSLGUIC@172.21.160.162:5062 SIP/2.0");
//
//	//Cseq new to 1
//	message->replaceHeadCSeq("1 ACK");
//	DEBOUT("VALO","Cseq")
//
//	message->purgeSDP();
//	message->dropHeader("Content-Type:");
//
//	char toTmp[512];
//	map<string, void*> ::iterator p;
//	p = ctxt_store.find("totag");
//	string toTagB = *((string*)p->second);
//	sprintf(toTmp, "%s %s;tag=%s",message->getHeadTo().getNameUri().c_str(), message->getHeadTo().getC_AttSipUri().getContent().c_str(),toTagB.c_str());
//	string toTmpS(toTmp);
//	DEBOUT("******** TO new" , toTmpS)
//	message->replaceHeadTo(toTmpS);
//	DEBOUT("TO",message->getHeadTo().getContent())
//	DEBOUT("TO",message->getHeadTo().getC_AttSipUri().getContent())
//	DEBOUT("TO",message->getHeadTo().getNameUri())
//	DEBOUT("TO",message->getHeadTo().getC_AttUriParms().getContent())
//
//	//Standard changes
//	//SipUtil.genBRequestfromARequest(call_oset->getGenMessage(), _message, message, getSUDP());
//
//	DEBOUT("NEW ACK via","")
//	map<string, void*> ::iterator p2;
//	p2 = ctxt_store.find("allvia");
//	string allVia = *((string*)p2->second);
//	message->purgeSTKHeadVia();
//	message->pushHeadVia("Via: " + allVia);
//	DEBOUT("NEW ACK via",allVia.c_str())
//
//	message->compileMessage();
//
//	message->dumpVector();
//
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
void VALO::onBye(MESSAGE* _message, int _dir){
//
//	//v4
//	//get invite sent to b
//	DEBOUT("VALO onBye", call_oset->getInviteB()->getIncBuffer())
//	CREATEMESSAGE(message, call_oset->getInviteB(), SODE_ALOPOINT)
//	//CREATEMESSAGE(message, _message, SODE_ALOPOINT)
//	//set as source the original ack, needed to identify call_oset_x when back to call control
//	message->setSourceMessage(_message);
//
//	if (_dir == 1 ) {
//		message->setDestEntity(SODE_SMCLPOINT);
//
//		//Cseq new to 1
//		int i = call_oset->getSL_SM_CL()->getControlSequence();
//		DEBOUT("VALO Cseq currn", i)
//		i++;
//		call_oset->getSL_SM_CL()->setControlSequence(i);
//
//		message->replaceHeadCSeq(i, "BYE");
//		DEBOUT("VALO Cseq new", message->getGenericHeader("CSeq"))
//
//
//	} else if (_dir == -1){
//		//PROBABLY WILL BE SENT TO ALO again
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

	ok_x->setGenericHeader("Content-Length:","0");

	if (ok_x->getSDPSize() != 0 ){
		//SDP must copy the SDP from incoming OK and put here
		vector<string> __sdp = _message->getSDP();
		ok_x->purgeSDP();
		DEBOUT("PURGED SDP","")
		ok_x->dumpVector();
		ok_x->importSDP(__sdp);
		ok_x->dumpVector();
	}

	ok_x->replaceHeadContact("<sip:sipsl@grog:5060>");

	SipUtil.genASideReplyFromBReply(_message, __message, ok_x);
	ok_x->compileMessage();
	ok_x->dumpVector();

	sl_cc->p_w(ok_x);

}

