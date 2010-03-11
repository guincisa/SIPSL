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

		//Cseq new to 1
		message->replaceHeadCSeq("999 INVITE");
		DEBOUT("VALO","Cseq")

		//Standard changes
		SipUtil.genBInvitefromAInvite(_message, message, getSUDP());

		message->dumpVector();
		DEBOUT("New outgoing b2b message", message->getIncBuffer())

		message->setDestEntity(SODE_SMCLPOINT);

		message->setLock();

		sl_cc->p_w(message);

}
void VALO::onAck(MESSAGE* _message){

	//v4
	//get invite sent to b
	DEBOUT("VALO onAck", call_oset->getGenMessage_CL_V4()->getIncBuffer())
	CREATEMESSAGE(message, call_oset->getGenMessage_CL_V4(), SODE_ALOPOINT)
	//CREATEMESSAGE(message, _message, SODE_ALOPOINT)
	//set as source the original ack, needed to identify call_oset_x when back to call control
	message->setSourceMessage(_message);
	message->setDestEntity(SODE_SMCLPOINT);

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
	message->setHeadSipRequest("ACK sip:SIPSLGUIC@172.21.160.162:5062 SIP/2.0");

	//Cseq new to 1
	message->replaceHeadCSeq("999 ACK");
	DEBOUT("VALO","Cseq")

	message->purgeSDP();
	message->dropHeader("Content-Type:");

	//Standard changes
	SipUtil.genBInvitefromAInvite(call_oset->getGenMessage(), message, getSUDP());

	message->dumpVector();

	DEBOUT("New outgoing b2b message", message->getIncBuffer())


	//PURGE ACK
	//PURGEMESSAGE(_message,"Purging ack from A")

	sl_cc->p_w(message);

	//build ack

}
void VALO::onBye(MESSAGE* _message){

	//v4
	//get invite sent to b
	DEBOUT("VALO onBye", call_oset->getGenMessage_CL_V4()->getIncBuffer())
	CREATEMESSAGE(message, call_oset->getGenMessage_CL_V4(), SODE_ALOPOINT)
	//CREATEMESSAGE(message, _message, SODE_ALOPOINT)
	//set as source the original ack, needed to identify call_oset_x when back to call control
	message->setSourceMessage(_message);
	message->setDestEntity(SODE_SMCLPOINT);

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
	message->setHeadSipRequest("BYE sip:SIPSLGUIC@172.21.160.117:5062 SIP/2.0");

	//Cseq new to 1
	message->replaceHeadCSeq("999 BYE");
	DEBOUT("VALO","Cseq")

	message->purgeSDP();
	message->dropHeader("Content-Type:");

	SipUtil.genBInvitefromAInvite(call_oset->getGenMessage(), message, getSUDP());
	message->dumpVector();
	DEBOUT("New outgoing b2b message", message->getIncBuffer())


	//PURGE ACK
	//PURGEMESSAGE(_message,"Purging ack from A")

	sl_cc->p_w(message);

	//build ack

}
void VALO::on200Ok(MESSAGE* _message){

		MESSAGE* __message = call_oset->getGenMessage();
		DEBOUT("on200Ok MESSAGE GENERATOR", __message)
		CREATEMESSAGE(ok_x, __message, SODE_ALOPOINT)
		ok_x->setDestEntity(SODE_SMSVPOINT);
		ok_x->typeOfInternal = TYPE_MESS;

		DEBOUT("ok_x","SIP/2.0 200 OK")
		ok_x->setHeadSipReply("SIP/2.0 200 OK");

		DEBOUT("ok_x","delete User-Agent:")
		ok_x->dropHeader("User-Agent:");
		DEBOUT("ok_x","delete Max-Forwards:")
		ok_x->removeMaxForwards();
		DEBOUT("ok_x","delete Allow:")
		ok_x->dropHeader("Allow:");
		DEBOUT("ok_x","delete Route:")
		ok_x->dropHeader("Route:");
		DEBOUT("ok_x","delete Date:")
		ok_x->dropHeader("Date:");

		// change tag
//		char totmp[512];
//		// check if NameUri is empty
//		sprintf(totmp, "%s %s;tag=%x",ok_x->getHeadTo().getNameUri().c_str(), ok_x->getHeadTo().getC_AttSipUri().getContent().c_str(),__message);
//		string totmpS(totmp);
//		DEBOUT("******** 200 OK TO tag new" , totmpS)
//		ok_x->replaceHeadTo(totmpS);

		//via add rport
		DEBY
		C_HeadVia* viatmp = (C_HeadVia*) ok_x->getSTKHeadVia().top();
		//TODO 124??
		DEBOUT("viatmp->getContent", viatmp->getContent())
		viatmp->getChangeC_AttVia().getChangeViaParms().replaceRvalue("rport", "124");
		ok_x->popSTKHeadVia();
		ok_x->pushHeadVia("Via: "+viatmp->getC_AttVia().getContent());

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


		ok_x->compileMessage();
		ok_x->dumpVector();

		sl_cc->p_w(ok_x);


}

