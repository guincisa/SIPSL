//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPCSL Sip Core And Service Layer
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
#include <vector>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <map>

#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stack>

#ifndef UTIL_H
#include "UTIL.h"
#endif
#ifndef CS_HEADERS_H
#include "CS_HEADERS.h"
#endif
#ifndef ENGINE_H
#include "ENGINE.h"
#endif
#ifndef SIPENGINE_H
#include "SIPENGINE.h"
#endif

//**********************************************************************************
//**********************************************************************************
ENGINE * SIPENGINE::getSL_CC(void){
    return sl_cc;
}
//**********************************************************************************
//**********************************************************************************
void SIPENGINE::setSL_CC(ENGINE* _sl_cc) {
    sl_cc = _sl_cc;
}
//**********************************************************************************
//**********************************************************************************

//**********************************************************************************
//**********************************************************************************
void SIPENGINE::parse(MESSAGE* _mess) {

	//B2BUA, NGApplication, SBC

	//Request or Reply
	//check requests or reply
	DEBOUT("SIPENGINE::parse",_mess->getIncBuffer())

	int tl = _mess->getTotLines();
	DEBOUT("SIPENGINE::parse tot lines",tl)

	int type = _mess->getReqRepType();
	DEBOUT("SIPENGINE::parse request type",type)

	if ( type == REQSUPP) {

		DEBY
		int method = _mess->getHeadSipRequest().getS_AttMethod().getMethodID();
		DEBY
		if (method != INVITE_REQUEST && method != BYE_REQUEST && method != ACK_REQUEST) {

			DEBOUT("SIPENGINE::parse unsupported METHOD ",_mess->getIncBuffer())
			pthread_mutex_lock(&messTableMtx);
			globalMessTable.erase(_mess->getKey());
			pthread_mutex_unlock(&messTableMtx);
			delete _mess;
			DEBOUT("SIPENGINE::parse message discarded ","");
			return;
		}else {
			sl_cc->p_w(_mess);
		}

	}
	else if ( type == REPSUPP) {

	}
	else {
		// purge it
		PURGEMESSAGE(_mess, "SIPENGINE::parse delete message")
//		string key = _mess->getKey();
//		pthread_mutex_lock(&messTableMtx);
//		DEBOUT("SIPENGINE::parse delete message",key)
//		globalMessTable.erase(key);
//		delete _mess;
//		pthread_mutex_unlock(&messTableMtx);

		//syntax error
	}
}
////TODO controllare b2bua
//
//    int ij = 0;
//    int iii=1,jjj=0,kkk=0;
//    bool stilllines = true;
//    int iPos=0,ePos=0,iiPos=0,eePos=0;
//    int totlines;
//
//    DEBOUT("SIPENGINE::parseMessage\n-------------------------------------------------------------------\n",om.incomingMessage)
//
//    int len = om.incomingMessage.length();
//    cout << "Len " << len << endl;
//
//    //estract lines
//    try {
//        totlines = om.getTotLines();
//        cout << "tot lines " <<  totlines << endl;
//
//        //NEW PARSER
//        ////////////////////////////////////////////////////////////////
//        // METHOD
//        // fixed position
//        ////////////////////////////////////////////////////////////////
//        cout << "NEW PARSER" << endl;
//        om._ReqLine.setBaseMessage(&om);
//        DEBOUT("","")
//        int methodId2 = om._ReqLine.getMethodId();
//        DEBOUT("","")
//        if ( methodId2 <= 0) {
//            ParseEx e = ParseEx("Method not supported:");
//            throw e;
//        }
//        // Special command
//        if (methodId2 == INTERNALS_METHOD) {
//            DEBOUT("INTERNALS_METHOD","SIPENGINE::parse")
//            _SL_CC->p_w(om);
//            return;
//        }
//        cout << "***************************************************************************"<<endl;
//        cout << "REQUEST *******************************************************************"<<endl;
//        cout << "***************************************************************************"<<endl;
//        cout << "RequestLine [" << om._ReqLine.value << "]" << endl;
//        cout << "RequestLine Method found " << methodId2 << " 1 is REGISTER 2 is INVITE" << endl;
//        cout << "RequestLine Request Uri [" << om._ReqLine.getURI().getValue()<<"]" << endl;
//        cout << "RequestLine Sip Vs [" << om._ReqLine.getProtVs()<<"]"<< endl;
//        cout << "RURI breakdown" << endl;
//        cout << "RURI type " << om._ReqLine.getURI().getType() << endl;
//        cout << "RURI SIPURI [" << om._ReqLine.getURI().getSipUri().getValue() << "]" << endl;
//        cout << "SIPURI breakdown" << endl;
//        cout << "SIPURI is secure [" << om._ReqLine.getURI().getSipUri().isSecure() <<  "]" << endl;
//        cout << "SIPURI user info  [" << om._ReqLine.getURI().getSipUri().getUserInfo() <<  "]" << endl;
//        cout << "SIPURI host [" << om._ReqLine.getURI().getSipUri().getHost() <<  "]" << endl;
//        cout << "SIPURI port [" << om._ReqLine.getURI().getSipUri().getPort() <<  "]" << endl;
//        /*
//        cout << "SIPURI params " << om._ReqLine.getUri().getSipUri().getUriParms().getValue() <<  "]" << endl;
//        cout << "SIPURI headers " << om._ReqLine.getUri().getSipUri().getHeaders() <<  "]" << endl;
//        */
//
//
//
//        ////////////////////////////////////////////////////////////////
//        //Call ID
//        ////////////////////////////////////////////////////////////////
//        cout << "***************************************************************************"<<endl;
//        cout << "CallId ********************************************************************"<<endl;
//        cout << "***************************************************************************"<<endl;
//        om._CallID.setBaseMessage(&om);
//        if (!om._CallID.isCorrect()) {
//            ParseEx e = ParseEx("Call-Id malformed");
//            throw e;
//        }
//        cout << "Call-ID Value [" << om._CallID.getCallId() <<"]" << endl;
//
//        ////////////////////////////////////////////////////////////////
//        //Contact:
//        ////////////////////////////////////////////////////////////////
//        cout << "***************************************************************************"<<endl;
//        cout << "Contact *******************************************************************"<<endl;
//        cout << "***************************************************************************"<<endl;
//        om._Contact.setBaseMessage(&om);
//        if (!om._Contact.isCorrect()) {
//            ParseEx e = ParseEx("Contact malformed");
//            throw e;
//        }
//        cout << "Contact ["<<om._Contact.value <<"]" << endl;
//        cout << "Contact URI [" <<om._Contact.getContEruri().getValue() << "]" << endl;
//        cout << "Contact URI is star [" <<om._Contact.isStar() << "]" << endl;
//        //cout << "Contact params [" << om._Contact.getContParms().getValue() << "]" << endl;
//        /*
//        cout << "SIPURI is secure [" << om._Contact.getContRuri().getSipUri().isSecure() <<  "]" << endl;
//        cout << "SIPURI user info  [" << om._Contact.getContRuri().getSipUri().getUserInfo() <<  "]" << endl;
//        cout << "SIPURI host [" << om._Contact.getContRuri().getSipUri().getHost() <<  "]" << endl;
//        cout << "SIPURI port [" << om._Contact.getContRuri().getSipUri().getPort() <<  "]" << endl;
//        cout << "SIPURI params [" << om._Contact.getContRuri().getSipUri().getUriParms().getValue() <<  "]" << endl;
//        */
//        cout << "SIPURI is secure [" << om._Contact.getContEruri().getEruriRuri().getSipUri().isSecure() <<  "]" << endl;
//        cout << "SIPURI user info  [" << om._Contact.getContEruri().getEruriRuri().getSipUri().getUserInfo() <<  "]" << endl;
//        cout << "SIPURI host [" << om._Contact.getContEruri().getEruriRuri().getSipUri().getHost() <<  "]" << endl;
//        cout << "SIPURI port [" << om._Contact.getContEruri().getEruriRuri().getSipUri().getPort() <<  "]" << endl;
//        cout << "SIPURI params [" << om._Contact.getContEruri().getEruriRuri().getSipUri().getUriParms().getValue() <<  "]" << endl;
//        cout << "Eruri params [" << om._Contact.getContEruri().getEruriParms().getValue() <<  "]" << endl;
//        cout << "Eruri params expires?[" << om._Contact.getContEruri().getEruriParms().getParmValue("expires") <<  "]" << endl;
//
//        ////////////////////////////////////////////////////////////////
//        //From:
//        ////////////////////////////////////////////////////////////////
//        cout << "***************************************************************************"<<endl;
//        cout << "From **********************************************************************"<<endl;
//        cout << "***************************************************************************"<<endl;
//        om._From.setBaseMessage(&om);
//        if (!om._From.isCorrect()) {
//            ParseEx e = ParseEx("From malformed");
//            throw e;
//        }
//        cout << "From Value [" << om._From.value <<"]" << endl;
//        cout << "From displayname [" << om._From.getFromEruri().getEruriDisplayname() <<"]" << endl;
//        cout << "From userinfo [" << om._From.getFromEruri().getEruriRuri().getSipUri().getUserInfo() <<"]" << endl;
//        cout << "From host [" << om._From.getFromEruri().getEruriRuri().getSipUri().getHost() <<"]" << endl;
//        cout << "From SIPURI params [" << om._From.getFromEruri().getEruriRuri().getSipUri().getUriParms().getValue() <<"]" << endl;
//        cout << "From ERURI params [" << om._From.getFromEruri().getEruriParms().getValue() <<"]" << endl;
//
//
//        ////////////////////////////////////////////////////////////////
//        //To:
//        ////////////////////////////////////////////////////////////////
//        cout << "***************************************************************************"<<endl;
//        cout << "To ************************************************************************"<<endl;
//        cout << "***************************************************************************"<<endl;
//        om._To.setBaseMessage(&om);
//        if (!om._To.isCorrect()) {
//            ParseEx e = ParseEx("To malformed");
//            throw e;
//        }
//        cout << "To Value [" << om._To.value <<"]" << endl;
//        cout << "To displayname [" << om._To.getToEruri().getEruriDisplayname() <<"]" << endl;
//        cout << "To userinfo [" << om._To.getToEruri().getEruriRuri().getSipUri().getUserInfo() <<"]" << endl;
//        cout << "To host [" << om._To.getToEruri().getEruriRuri().getSipUri().getHost() <<"]" << endl;
//        cout << "To SIPURI params [" << om._To.getToEruri().getEruriRuri().getSipUri().getUriParms().getValue() <<"]" << endl;
//        cout << "To ERURI params [" << om._To.getToEruri().getEruriParms().getValue() <<"]" << endl;
//
//        ////////////////////////////////////////////////////////////////
//        //Expires:
//        ////////////////////////////////////////////////////////////////
//        cout << "***************************************************************************"<<endl;
//        cout << "Expires *******************************************************************"<<endl;
//        cout << "***************************************************************************"<<endl;
//        om._Expires.setBaseMessage(&om);
//        if (!om._Expires.isCorrect()) {
//            ParseEx e = ParseEx("Expires malformed");
//            throw e;
//        }
//        cout << "Expires Value [" << om._Expires.value <<"]" << endl;
//        cout << "Expires ispresent [" << om._Expires.isPresent() <<"]" << endl;
//        cout << "Expires time [" << om._Expires.getExpires() <<"]" << endl;
//
//        ////////////////////////////////////////////////////////////////
//        //Via:
//        ////////////////////////////////////////////////////////////////
//        cout << "***************************************************************************"<<endl;
//        cout << "Via ***********************************************************************"<<endl;
//        cout << "***************************************************************************"<<endl;
//        //ORRRRRRRRRIBBBBBB
//        int vii = 0;
//        for (vii = 15; vii >= 0 ; vii --) {
//            om._Via[vii].setBaseMessage(&om);
//            if (!om._Via[vii].isCorrect()) {
//                ParseEx e = ParseEx("Expires malformed");
//                throw e;
//            }
//            cout << " Via " << vii << " has value [" << om._Via[vii].value << "]" << endl;
//            cout << " Via " << vii << " is present " << om._Via[vii].isPresent() <<  endl;
//            if ( om._Via[vii].isPresent()) {
//                cout << " Via hostport " << vii <<" "<<  om._Via[vii].getViaHostPort() <<  endl;
//                cout << " Via params " << vii << " " <<  om._Via[vii].getViaParms().getValue() <<  endl;
//            }
//        }
//        ////////////////////////////////////////////////////////////////
//        //CSEQ:
//        ////////////////////////////////////////////////////////////////
//        cout << "***************************************************************************"<<endl;
//        cout << "Cseq **********************************************************************"<<endl;
//        cout << "***************************************************************************"<<endl;
//        om._CSeq.setBaseMessage(&om);
//        if (!om._CSeq.isCorrect()) {
//            ParseEx e = ParseEx("CSeq malformed");
//            throw e;
//        }
//
//        DEBOUT("CSeq", om._CSeq.getCSeq());
//        DEBOUT("CSeq Method", om._CSeq.getCSeqMethod());
//
//    }
//    catch (ParseEx e) {
//        //DEBOUT("Exception while parsing ",e.error,"")
//        cout << "Exception "<< endl;
//        //om.sendNAck(-1, "Exception while parsing");
//        //NO REPLY if request is malformed
//        //and black list for the call ID???
//        return;
//    }
//    // callback is needed to costomize the return message
//    //rtacb->parsecallback(om);
//    //om.sendAck("Parse successful");
//    //cout << "SIPENGINE::parse(MESSAGE m) invoking  _AC->prepareandwork(om);" << endl << flush;
//    if(om._ReqLine.getMethodId() == INVITE_METHOD){
//        _SL_CC->p_w(om);
//    }
//    return;
//};

//void AC_CALLBACK::parsecallback(MESSAGE m, RATYPE * ra, RATYPE * ra1) {
    //cout << "callback" << endl << flush;
        /*
    char echoBuffer[ECHOMAX];
    sprintf(echoBuffer, "Received %s", m.incomingMessage.c_str());
    //cout << "echo " << echoBuffer<< endl << flush;
    //TODO ECHOMAX???
    sendto(rata->sock, echoBuffer, ECHOMAX, 0, (struct sockaddr *) &(rata->echoClntAddr), sizeof(rata->echoClntAddr));
    */
//}

//TODO ma serve?????????????
//AC_CALLBACK::AC_CALLBACK(void) {
        // si perche' la RTACALLBACK(..) verrebbe chiamata con
        // u SIPCALLBACK quindi non risulterebbe definita
//}
