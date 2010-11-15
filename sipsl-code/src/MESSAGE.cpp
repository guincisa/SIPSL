//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Service Layer
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

#include <string>
#include <stack>
#include <assert.h>
#include <map>
#include <vector>
#include <sys/time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>

#ifndef UTIL_H
#include "UTIL.h"
#endif

#ifndef CS_HEADERS_H
#include "CS_HEADERS.h"
#endif

#ifndef MESSAGE_H
#include "MESSAGE.h"
#endif

// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
// BASEMESSAGE
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
BASEMESSAGE::BASEMESSAGE(string _incMessBuff, int _genEntity, SysTime _inc_ts, int _sock,
                    sockaddr_inX _echoClntAddr):
                    incMessBuff(_incMessBuff){

	genEntity = _genEntity;
	inc_ts = _inc_ts;
	sock = _sock;
	echoClntAddr = _echoClntAddr;

	arrayFilled = false;

	requestDirection = 0;

	//others
    id = 0; //Used in spin buffer
    destEntity = 0;
	key="";

    totLines=0;
    arrayFilled=false;

    invalid = 0;

	return;
}
BASEMESSAGE::BASEMESSAGE(string _incMessBuff, SysTime _inc_ts):
						incMessBuff(_incMessBuff){
	//TODO only for test
	incMessBuff=_incMessBuff;

	inc_ts = _inc_ts;

	arrayFilled = false;

	requestDirection = 0;

    invalid = 0;

	return;
}
BASEMESSAGE::BASEMESSAGE(BASEMESSAGE* _basemessage, int _genEntity, SysTime _creaTime){
	/*
	SysTime inTime;
	GETTIME(inTime);
	char bu[512];
	sprintf(bu, "%x#%lld",this,inTime.tv.tv_sec*1000000+inTime.tv.tv_usec);
	string key(bu);
	*/
    DEBOUT("BASEMESSAGE::BASEMESSAGE","copy")

	echoClntAddr = _basemessage->getAddress();
    sock = _basemessage->getSock();

    inc_ts = _creaTime;

    //flex_line = _basemessage->flex_line;
    incMessBuff = _basemessage->getIncBuffer();

    arrayFilled = false;

    genEntity = _genEntity;

	requestDirection = _basemessage->getRequestDirection();

    invalid = 0;

	return;
}
//BASEMESSAGE::BASEMESSAGE(const BASEMESSAGE& x){
//	DEBASSERT("BASEMESSAGE copy constructor");
//}

void BASEMESSAGE::setValid(int _valid){
	invalid = _valid;
}
vector<string> BASEMESSAGE::getLines(void){
	if (invalid == 1)
		DEBASSERT("BASEMESSAGE::getLines invalid")

	return flex_line;
}

void BASEMESSAGE::fillLineArray(void){

	if (invalid == 1)
		DEBASSERT("BASEMESSAGE::fillLineArray invalid")

    if (arrayFilled)
        return;

    Tuple s = brkin2(incMessBuff,"\n");
    string t = s.Rvalue;
    string strim = trimCR(s.Lvalue);
    flex_line.push_back(strim);
    while (t.compare("")!=0){

    	s = brkin2(t,"\n");
    	t = s.Rvalue;

    	if (s.Lvalue.substr(0,1).compare(" ") == 0 || s.Lvalue.substr(0,1).compare("\t") == 0){
    		string tt = flex_line.back();
    		string ttt = tt + trimCR(s.Lvalue);
    		flex_line.pop_back();
            flex_line.push_back(ttt);
    	} else{
    		string strim = trimCR(s.Lvalue);
    		TRYCATCH(flex_line.push_back(strim))
    	}
    }

    arrayFilled = true;

    return;
}

// *****************************************************************************************
// *****************************************************************************************
int BASEMESSAGE::getTotLines() {
	if (invalid == 1)
		DEBASSERT("BASEMESSAGE::getTotLines invalid")

    if (!arrayFilled){
    	fillLineArray();
    }

    return(flex_line.size());
}
// *****************************************************************************************
// *****************************************************************************************
string &BASEMESSAGE::getLine(int pos){
	if (invalid == 1)
		DEBASSERT("BASEMESSAGE::getLine invalid")

    // no copia REFERRENCE

    if (!arrayFilled)
    	fillLineArray();

    int i = getTotLines();

    if (pos >= i)
        pos = i-1;

    return(flex_line[pos]);
}
string &BASEMESSAGE::getIncBuffer(void){
	if (invalid == 1)
		DEBASSERT("BASEMESSAGE::getIncBuffer invalid")

	return incMessBuff;
}
int BASEMESSAGE::getGenEntity(void){
	if (invalid == 1)
		DEBASSERT("BASEMESSAGE::getGenEntity invalid")

	return genEntity;
}
void BASEMESSAGE::setGenEntity(int _genEntity){
	if (invalid == 1)
		DEBASSERT("BASEMESSAGE::setGenEntity invalid")

	genEntity = _genEntity;
}
int BASEMESSAGE::getRequestDirection(void){
	if (invalid == 1)
		DEBASSERT("BASEMESSAGE::getRequestDirection invalid")

	return requestDirection;
}
void BASEMESSAGE::setRequestDirection(int _genEntity){
	if (invalid == 1)
		DEBASSERT("BASEMESSAGE::setRequestDirection invalid")

	requestDirection = _genEntity;
}
int BASEMESSAGE::getDestEntity(void){
	if (invalid == 1)
		DEBASSERT("BASEMESSAGE::getDestEntity invalid")

	return destEntity;
}
void BASEMESSAGE::setDestEntity(int _destEntity){
	if (invalid == 1)
		DEBASSERT("BASEMESSAGE::setDestEntity invalid")

	destEntity = _destEntity;
}
void BASEMESSAGE::setEndPoints(int _genEntity, int _destEntity){
	if (invalid == 1)
		DEBASSERT("BASEMESSAGE::setEndPoints invalid")

	destEntity = _destEntity;
	genEntity = _genEntity;
}

string &BASEMESSAGE::getKey(void){
	if (invalid == 1)
		DEBASSERT("BASEMESSAGE::getKey invalid")

	return key;
}
void BASEMESSAGE::setKey(string _key){
	if (invalid == 1)
		DEBASSERT("BASEMESSAGE::setKey invalid")

	key = _key;
}
SysTime BASEMESSAGE::getCreationTime(void){
	if (invalid == 1)
		DEBASSERT("BASEMESSAGE::getCreationTime invalid")

	return inc_ts;
}
struct sockaddr_in BASEMESSAGE::getAddress(void){
	if (invalid == 1)
		DEBASSERT("BASEMESSAGE::getAddress invalid")

	return echoClntAddr;
}
int BASEMESSAGE::getSock(void){
	if (invalid == 1)
		DEBASSERT("BASEMESSAGE::getSock invalid")

	return sock;
}
void BASEMESSAGE::removeHeader(int _pos){
	if (invalid == 1)
		DEBASSERT("BASEMESSAGE::removeHeader invalid")

	flex_line[_pos] = "xxDxx";
}
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
// MESSAGE
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
MESSAGE::MESSAGE(string _incMessBuff, int _genEntity, SysTime _inc_ts, int _sock,
        struct sockaddr_in _echoClntAddr):
	BASEMESSAGE(_incMessBuff, _genEntity, _inc_ts, _sock, _echoClntAddr),
	headSipRequest(""),
	headSipReply(""),
	headMaxFwd(""),
	headCallId(""),
	headCSeq(""){

	reqRep = 0;

	s_headVia_p = false;
	headMaxFwd_p = false;
	headContact_p = false;
	headTo_p = false;
	headFrom_p = false;
	headCallId_p = false;
	headCSeq_p = false;
	headRoute_p = false;
	headRoute_e = false;
	sdpVector_p = false;
	sdpSize = 0;

	lock = false;

	source=MainMessage;

	isInternal = false;

	modulus = 0;

	typeOfInternal = TYPE_MESS; // Message or operation
	typeOfOperation = TYPE_OP_NOOP; // Type of operation
	orderOfOperation = ""; //Alarm id in case more alarms are triggered with the same message

	type_trnsct = TYPE_TRNSCT;

	//Pointers
	headTo = NULL;
	headFrom = NULL;
	headContact = NULL;
	headRoute = NULL;

}
//MESSAGE::MESSAGE(const MESSAGE& x){
//	DEBASSERT("MESSAGE copy constructor");
//}

MESSAGE::~MESSAGE(){

	while (!s_headVia.empty()){
		DELPTR((C_HeadVia*)(s_headVia.top()), "Delete via")
		s_headVia.pop();
	}
	if (headTo != NULL){
		DELPTR(headTo, "C_HeadTo")
	}
	if (headFrom != NULL){
		DELPTR(headFrom, "C_HeadFrom")
	}
	if (headContact != NULL){
		DELPTR(headContact, "C_HeadContact")
	}
	if (headRoute != NULL){
		DELPTR(headRoute, "C_HeadRoute")
	}



}
MESSAGE::MESSAGE(MESSAGE* _message, int _genEntity, SysTime _creaTime):
	BASEMESSAGE(_message, _genEntity, _creaTime),
	headSipRequest(""),
	headSipReply(""),
	headMaxFwd(""),
	//headTo(""),
	headCallId(""),
	headCSeq(""){
	DEBOUT("MESSAGE::MESSAGE(MESSAGE* _message, int _genEntity):","")
	source = _message;
	s_headVia_p = false;
	headMaxFwd_p = false;
	headContact_p = false;
	headTo_p = false;
	headFrom_p = false;
	headCallId_p = false;
	headCSeq_p = false;
	headRoute_p = false;
	sdpVector_p = false;
	sdpSize = 0;
	reqRep = 0;
	//, headRoute_e;

	lock = false;

	type_trnsct = TYPE_TRNSCT;

	modulus = 0;
	typeOfInternal = TYPE_MESS; // Message or operation
	typeOfOperation = TYPE_OP_NOOP; // Type of operation
	orderOfOperation = ""; //Alarm id in case more alarms are triggered with the same message

	//Pointers
	headTo = NULL;
	headFrom = NULL;
	headContact = NULL;
	headRoute = NULL;


	return;
}
//ONLY FOR TEST
#ifdef TESTING
MESSAGE::MESSAGE(string _incMessBuff, SysTime _inc_ts):
	BASEMESSAGE(_incMessBuff, inc_ts),
	headSipRequest(""),
	headSipReply(""),
	headMaxFwd(""),
	headContact(""),
	headTo(""),
	headFrom(""),
	headCallId(""),
	headCSeq(""),
	headRoute(""){

	assert(0);

	reqRep = 0;
}
#endif
/*
 * get SDP position
 * XXX: blabla
 * <empty>
 * x=yxz
 *
 * position in the array of the <empty> line
 */
vector<string>::iterator MESSAGE::getSDPposition(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::getSDPposition invalid")


	vector<string>::iterator theIterator;

	for( theIterator = flex_line.begin(); theIterator != flex_line.end(); theIterator++ ) {
    	if ((*theIterator).size() == 0) {
    		return theIterator--;
    	}
    }
    return theIterator;
}
void MESSAGE::dumpVector(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::dumpVector invalid")

	vector<string>::iterator theIterator;
    for( theIterator = flex_line.begin(); theIterator != flex_line.end(); theIterator++ ) {
    	DEBOUT("Message vector", *theIterator)
    }
}
int MESSAGE::getReqRepType(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::getReqRepType invalid")


	if (reqRep != 0) {
		return reqRep;
	}
	if(flex_line[0].substr(0,3).compare("SIP")==0){
		DEBOUT("MESSAGE::getReqRepType", flex_line[0])
		headSipReply.setContent(flex_line[0]);
		reqRep = REPSUPP;
	}
	if(flex_line[0].substr(0,6).compare("INVITE")==0){
		headSipRequest.setContent(flex_line[0]);
		reqRep = REQSUPP;
	}
	else if(flex_line[0].substr(0,3).compare("ACK")==0){
		headSipRequest.setContent(flex_line[0]);
		reqRep = REQSUPP;
	}
	else if(flex_line[0].substr(0,3).compare("BYE")==0){
		headSipRequest.setContent(flex_line[0]);
		reqRep = REQSUPP;
	}
	else if(flex_line[0].substr(0,6).compare("CANCEL")==0){
		headSipRequest.setContent(flex_line[0]);
		reqRep = REQSUPP;
	}
	else if(flex_line[0].substr(0,8).compare("REGISTER")==0){
		headSipRequest.setContent(flex_line[0]);
		reqRep = REQUNSUPP;
	}
	return reqRep;
}
void MESSAGE::compileMessage(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::compileMessage invalid")


	// first look for xxDxx and remove it
	vector<string>::iterator theIterator;
    for( theIterator = flex_line.begin(); theIterator != flex_line.end(); theIterator++ ) {
    	if ((*theIterator).substr(0,5).compare("xxDxx") == 0 ) {
    		flex_line.erase(theIterator);
    		theIterator--;
    	}
    }
    incMessBuff = "";
    if (getGenericHeader("Content-Type:").size() == 0)
    	flex_line.insert(theIterator ,1 , "");

    for( theIterator = flex_line.begin(); theIterator != flex_line.end(); theIterator++ ) {

    	incMessBuff = incMessBuff + (*theIterator) + "\r\n";

    }

}
MESSAGE* MESSAGE::getSourceMessage(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::getSourceMessage invalid")

	return source;
}
void MESSAGE::setSourceMessage(MESSAGE* _source){
	if (invalid == 1)
		DEBASSERT("MESSAGE::setSourceMessage invalid")

	source = _source;
}


/*
 * Request
 */
C_HeadSipRequest &MESSAGE::getHeadSipRequest(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::getHeadSipRequest invalid")

	if(reqRep == 0){
		reqRep = getReqRepType();
	}

	if (reqRep == REQSUPP || reqRep == REQUNSUPP){
		return headSipRequest;
	}
    DEBASSERT("MESSAGE::getHeadSipRequest illegal instruction");
}
void MESSAGE::setHeadSipRequest(string _content){
	if (invalid == 1)
		DEBASSERT("MESSAGE::setHeadSipRequest invalid")

	reqRep = 0;
	flex_line[0] = _content;
	headSipRequest.setContent(_content);
}
void MESSAGE::setHeadSipReply(string _content){
	if (invalid == 1)
		DEBASSERT("MESSAGE::setHeadSipReply invalid")

	reqRep = 0;
	flex_line[0] = _content;
	headSipReply.setContent(_content);
}
C_HeadSipReply &MESSAGE::getHeadSipReply(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::getHeadSipReply invalid")


	if(reqRep == 0){
		reqRep = getReqRepType();
	}

	if (reqRep == REPSUPP || reqRep == REPUNSUPP){
		return headSipReply;
	}
	DEBASSERT("MESSAGE::getHeadSipReply illegal instruction");
}
/*
 * Via
 */
stack<C_HeadVia*> &MESSAGE::getSTKHeadVia(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::getSTKHeadVia invalid")


	if(s_headVia_p){
		return s_headVia;
	}

	unsigned int i;
	unsigned int j = 1;

	for( i =   flex_line.size() - 1 ; i > 0 ; i --){
		if(flex_line[i].substr(0,4).compare("Via:") == 0){

			C_HeadVia* s;
			NEWPTR2(s, C_HeadVia(flex_line[i].substr(5)), "New via")
			s_headVia.push(s);
		}
	}
	s_headVia_p = true;
	return s_headVia;
}
void MESSAGE::popSTKHeadVia(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::popSTKHeadVia invalid")


	DEBASSERT("popSTKHeadVia")
//	if(s_headVia_p){
//		getSTKHeadVia();
//	}
//
//	unsigned int i;
//	unsigned int j = 1;
//	bool topout = false;
//	for(i = 1; i < flex_line.size(); i ++){
//		if(flex_line[i].substr(0,4).compare("Via:") == 0){
//			if (!topout){
//				flex_line[i] = "xxDxx";
//				topout = true;
//			} else {
//				C_HeadVia* s = new C_HeadVia(flex_line[i]);
//				s_headVia.push(s);
//			}
//		}
//	}
//	s_headVia_p = true;
}
//
void MESSAGE::purgeSTKHeadVia(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::purgeSTKHeadVia invalid")


	unsigned int i;

	for(i = 1; i < flex_line.size(); i ++){
		if(flex_line[i].substr(0,4).compare("Via:") == 0){
			removeHeader(i);
		}
	}

	if(!s_headVia_p){
		// not initialized
		return;
	}
	//else clear the vector
	else {
		while(!s_headVia.empty()){
			//delete s_headVia.top();
			s_headVia.pop();
			DELPTR((C_HeadVia*)s_headVia.top(), "(C_HeadVia*)s_headVia.top()");
		}
//		stack<C_HeadVia*> tmp;
//		s_headVia = tmp;
		s_headVia_p = false;
	}
}
void MESSAGE::pushHeadVia(string _content){
	if (invalid == 1)
		DEBASSERT("MESSAGE::pushHeadVia invalid")


	NEWPTR(C_HeadVia*, s, C_HeadVia(_content),"New via")
	s_headVia.push(s);
	// first search Via and insert before
	// if no via, then search SDP
	unsigned int i;
	bool found = false;
	vector<string>::iterator theIterator;
    for( theIterator = flex_line.begin(); theIterator != flex_line.end(); theIterator++ ) {
    	if ((*theIterator).substr(0,4).compare("Via:") == 0 ) {
			found = true;
			break;
    	}
    }
	if (found){
		string tmp = "Via: " + _content;
		flex_line.insert(theIterator ,1 , _content);
	}
	else {
		theIterator = getSDPposition();
		DEBOUT("string tmp", _content)
		flex_line.insert(theIterator ,1 , "Via: " + _content);
	}
}
void MESSAGE::purgeSDP(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::purgeSDP invalid")


	DEBOUT("MESSAGE::purgeSDP","" )

	//setGenericHeader("Content-Length:","0");
	dropHeader("Content-Length:");
	dropHeader("Content-Type:");

	vector<string>::iterator theIterator;
	vector<string>::iterator theIteratorEnd;

	theIterator = getSDPposition();
	theIteratorEnd = flex_line.end();
	sdpSize = 0;
	if ( sdpVector_p ) {
		sdpVector.clear();
	} else {
		sdpVector_p = true;
	}

	if (theIterator == flex_line.end()){
		//nothing to erase
		return;
	}

	flex_line.erase(theIterator, theIteratorEnd);
}
vector<string> MESSAGE::getSDP(void) {
	if (invalid == 1)
		DEBASSERT("MESSAGE::getSDP invalid")



	if ( sdpVector_p )  {
		return sdpVector;
	}
	else {
		vector<string>::iterator theIterator;

		theIterator = getSDPposition();

		if (theIterator != flex_line.end()){
			theIterator ++;
		}
		sdpSize = 0;
		while(theIterator != flex_line.end()){
			sdpSize = sdpSize + (*theIterator).length() + 2;
			sdpVector.push_back((*theIterator));
			theIterator++;
		}
		sdpVector_p = true;
		return sdpVector;
	}
}
int MESSAGE::getSDPSize(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::getSDPSize invalid")


	if ( sdpVector_p )  {
		return sdpSize;
	}
	else {
		getSDP();
		return sdpSize;
	}

}
void MESSAGE::importSDP(vector<string> _sdp){
	if (invalid == 1)
		DEBASSERT("MESSAGE::importSDP invalid")


	vector<string>::iterator theIterator;

	sdpVector = _sdp;

	theIterator = _sdp.begin();
	sdpSize = 0;
	sdpVector_p = true;
	if (theIterator!= _sdp.end()){
		flex_line.push_back("Content-Type: application/sdp");
		flex_line.push_back("Content-Length: 0");
		flex_line.push_back("");
	}
	while (theIterator != _sdp.end()){
		DEBOUT("the iterator", *theIterator)
		sdpSize = sdpSize + (*theIterator).length() + 2;
		flex_line.push_back((*theIterator));
		theIterator++;
	}
	char aaa[10];
	sprintf(aaa,"%d", sdpSize);
	setGenericHeader("Content-Length:", aaa );

}

/*
 * MaxFwd
 */
S_HeadMaxFwd& MESSAGE::getHeadMaxFwd(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::getHeadMaxFwd invalid")


	if(headMaxFwd_p){
		return headMaxFwd;
	}

	int i;

	for(i = 1; i < flex_line.size(); i ++){
		if(flex_line[i].substr(0,13).compare("Max-Forwards:")==0){
			headMaxFwd.setContent(flex_line[i]);
			break;
		}
	}
	headMaxFwd_p = true;
		return headMaxFwd;
}
C_HeadTo* MESSAGE::getHeadTo(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::getHeadTo invalid")


	if (headTo == 0x0){
		NEWPTR2(headTo, C_HeadTo(""), "C_HeadTo")
	}

	if(headTo_p){
		return headTo;
	}

	int i;

	for(i = 1; i < flex_line.size(); i ++){
		if(flex_line[i].substr(0,3).compare("To:")==0){
			headTo->setContent(flex_line[i].substr(4));
			break;
		}
	}
	headTo_p = true;
	return headTo;
}
/*
 * From
 */
C_HeadFrom* MESSAGE::getHeadFrom(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::getHeadFrom invalid")


	if (headFrom == 0x0){
		NEWPTR2(headFrom, C_HeadFrom(""), "C_HeadFrom")
	}


	if(headFrom_p){
		return headFrom;
	}

	unsigned int i;

	for(i = 1; i < flex_line.size(); i ++){
		if(flex_line[i].substr(0,5).compare("From:")==0){
			headFrom->setContent(flex_line[i].substr(6));
			break;
		}
	}
	headFrom_p = true;
	return headFrom;
}
void MESSAGE::replaceHeadFrom(string _content){
	if (invalid == 1)
		DEBASSERT("MESSAGE::replaceHeadFrom invalid")


	headFrom_p = false;

	if (headFrom == NULL){
		NEWPTR2(headFrom, C_HeadFrom(""), "C_HeadFrom")
	}

	headFrom->setContent(_content);

	// replace in flex_line
	unsigned int i;
	bool found = false;
	for(i = 1; i < flex_line.size(); i ++){
		if(flex_line[i].substr(0,5).compare("From:")==0){
			flex_line[i] = "From: " + _content;
			found = true;
			break;
		}
	}
	if (!found) {
		DEBOUT("MESSAGE::replaceHeadFrom from header is missing","")
	}
}
void MESSAGE::replaceHeadTo(string _content){
	if (invalid == 1)
		DEBASSERT("MESSAGE::replaceHeadTo invalid")


	headTo_p = false;

	if (headTo == NULL){
		NEWPTR2(headTo, C_HeadTo(""), "C_HeadTo")
	}

	headTo->setContent(_content);

	// replace in flex_line
	unsigned int i;
	bool found = false;
	for(i = 1; i < flex_line.size(); i ++){
		if(flex_line[i].substr(0,3).compare("To:")==0){
			flex_line[i] = "To: " + _content;
			found = true;
			break;
		}
	}
	if (!found) {
		DEBOUT("MESSAGE::replaceHeadTo from header is missing","")
	}
}

/*
 * Call Id
 */
C_HeadCallId &MESSAGE::getHeadCallId(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::getHeadCallId invalid")


	if(headCallId_p){
		DEBOUT("CALL ID already parsed", headCallId.getContent())
		return headCallId;
	}

	unsigned int i;

	for(i = 1; i < flex_line.size(); i ++){
		if(flex_line[i].substr(0,8).compare("Call-ID:") == 0){
			headCallId.setContent(flex_line[i].substr(9));
			break;
		}
	}
	headCallId_p = true;
	return headCallId;
}
/*
 * CSeq
 */
C_HeadCSeq &MESSAGE::getHeadCSeq(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::getHeadCSeq invalid")


	if(headCSeq_p){
		return headCSeq;
	}

	unsigned int i;

	for(i = 1; i < flex_line.size(); i ++){
		if(flex_line[i].substr(0,5).compare("CSeq:")==0){
			headCSeq.setContent(flex_line[i].substr(6));
			break;
		}
	}
	headCSeq_p = true;
	return headCSeq;
}
void MESSAGE::replaceHeadCSeq(string _content){
	if (invalid == 1)
		DEBASSERT("MESSAGE::replaceHeadCSeq invalid")


	headCSeq_p = false;
	headCSeq.setContent(_content);

	// replace in flex_line
	unsigned int i;
	bool found = false;
	for(i = 1; i < flex_line.size(); i ++){
		if(flex_line[i].substr(0,5).compare("CSeq:")==0){
			flex_line[i] = "CSeq: " + _content;
			found = true;
			break;
		}
	}
	if (!found) {
		DEBASSERT("MESSAGE::replaceHeadCSeq from header is missing")
	}
}
void MESSAGE::replaceHeadCSeq(int _cseq, string _method){
	if (invalid == 1)
		DEBASSERT("MESSAGE::replaceHeadCSeq invalid")


	char tt[128];
	sprintf(tt, "%d %s", _cseq, _method.c_str());
	headCSeq_p = false;
	headCSeq.setContent(tt);

	// replace in flex_line
	unsigned int i;
	bool found = false;
	for(i = 1; i < flex_line.size(); i ++){
		if(flex_line[i].substr(0,5).compare("CSeq:")==0){
			flex_line[i] = "CSeq: " + (string)(tt);
			found = true;
			break;
		}
	}
	if (!found) {
		DEBASSERT("MESSAGE::replaceHeadCSeq from header is missing")
	}
}

/*
 * Route
 */
C_HeadRoute* MESSAGE::getHeadRoute(void) throw (HeaderException){
	if (invalid == 1)
		DEBASSERT("MESSAGE::getHeadRoute invalid")


	if (headRoute == 0x0){
		NEWPTR2(headRoute, C_HeadRoute(""),"C_HeadRoute")
	}
	if(headRoute_e){
		DEBOUT("MESSAGE::getHeadRoute","No Route header")
		DELPTR(headRoute,"C_HeadRoute")
		headRoute = NULL;
		throw HeaderException("No Route header");
	}
	if(headRoute_p){
		return headRoute;
	}

	unsigned int i;
	bool found = false;
	for(i = 1; i < flex_line.size(); i ++){
		if(flex_line[i].substr(0,6).compare("Route:")==0){
			found = true;
			headRoute->setContent(flex_line[i]);
			break;
		}
	}
	headRoute_p = true;
	if (found == true){
		return headRoute;
	}
	else {
		headRoute_e = true;
		DELPTR(headRoute,"C_HeadRoute")
		headRoute = NULL;
		DEBOUT("MESSAGE::getHeadRoute","No Route header")
		throw HeaderException("No Route header");
	}
}
void MESSAGE::removeHeadRoute(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::removeHeadRoute invalid")

	headRoute_p = false;
	unsigned int i;
	for(i = 1; i < flex_line.size(); i ++){
		if(flex_line[i].substr(0,6).compare("Route:")==0){
			removeHeader(i);
			headRoute = NULL;
			DELPTR(headRoute,"C_HeadRoute")
			break;
		}
	}
	return;
}
/*
 * Contact
 */
C_HeadContact* MESSAGE::getHeadContact(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::getHeadContact invalid")


	if (headContact == 0x0){
		NEWPTR2(headContact, C_HeadContact(""),"C_HeadContact")
	}

	if(headContact_p){
		return headContact;
	}

	int i;

	for(i = 1; i < flex_line.size(); i ++){
		if(flex_line[i].substr(0,8).compare("Contact:")==0){
			headContact->setContent(flex_line[i].substr(9));
			break;
		}
	}
	headContact_p = true;
		return headContact;
}

void MESSAGE::replaceHeadContact(string _content){
	if (invalid == 1)
		DEBASSERT("MESSAGE::replaceHeadContact invalid")


	if (headContact == 0x0){
		NEWPTR2(headContact, C_HeadContact(""),"C_HeadContact")
	}

	headContact_p = false;
	headContact->setContent(_content);

	// replace in flex_line
	unsigned int i;
	bool found = false;
	for(i = 1; i < flex_line.size(); i ++){
		if(flex_line[i].substr(0,8).compare("Contact:")==0){
			flex_line[i] = "Contact: " + _content;
			found = true;
			break;
		}
	}
	if (!found) {
		DEBOUT("MESSAGE::replaceHeadContact from header is missing","")
	}
}
void MESSAGE::removeMaxForwards(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::removeMaxForwards invalid")


	headMaxFwd_p = false;
	headMaxFwd.setContent("");

	// replace in flex_line
	unsigned int i;
	bool found = false;
	for(i = 1; i < flex_line.size(); i ++){
		if(flex_line[i].substr(0,13).compare("Max-Forwards:")==0){
			removeHeader(i);
			found = true;
			break;
		}
	}
	if (!found) {
		DEBOUT("MESSAGE::removeMaxForwards, MaxForwards is missing","")
	}

}
void MESSAGE::increaseMaxForwards(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::increaseMaxForwards invalid")
	DEBASSERT("MESSAGE::increaseMaxForwards for what?")
	return ;
}

void MESSAGE::dropHeader(string _header){
	if (invalid == 1)
		DEBASSERT("MESSAGE::dropHeader invalid")


	unsigned int i;
	bool found = false;
	for(i = 1; i < flex_line.size(); i ++){
		if(flex_line[i].substr(0,_header.size()).compare(_header)==0){
			removeHeader(i);
			found = true;
			break;
		}
	}
	if (!found) {
		DEBOUT("MESSAGE::removeHeader not found",_header)
	}
}
void MESSAGE::setGenericHeader(string _header, string _content){
	if (invalid == 1)
		DEBASSERT("MESSAGE::setGenericHeader invalid")


	DEBOUT("MESSAGE::setGenericHeader", _header + " " + _content)
	if (_header.compare("To:") == 0){
		DEBASSERT("DON'T USE")
	}
	if (_header.compare("From:") == 0){
		DEBASSERT("DON'T USE")
	}
	if (_header.compare("Contact:") == 0){
		DEBASSERT("DON'T USE")
	}
	if (_header.compare("Route:") == 0){
		DEBASSERT("DON'T USE")
	}


	unsigned int i;
	bool found = false;
	for(i = 1; i < flex_line.size(); i ++){
		if(flex_line[i].substr(0,_header.size()).compare(_header)==0){
			flex_line[i] = _header + " " + _content;
			found = true;
			break;
		}
	}
	if (!found) {
		DEBOUT("MESSAGE::setGenericHeader not found",_header)
	}
}
string MESSAGE::getGenericHeader(string _header){
	if (invalid == 1)
		DEBASSERT("MESSAGE::getGenericHeader invalid")


	DEBOUT("MESSAGE::getGenericHeader", _header)
	unsigned int i;
	bool found = false;
	for(i = 1; i < flex_line.size(); i ++){
		if(flex_line[i].substr(0,_header.size()).compare(_header)==0){
			Tuple s = brkin2(flex_line[i], " ");
			found = true;
			DEBOUT("found header",_header << "***"<< s.Rvalue)
			return s.Rvalue;
		}
	}
	if (!found) {
		DEBOUT("MESSAGE::setGenericHeader not found",_header)
				return "";
	}
}


void MESSAGE::addGenericHeader(string _header, string _content){
	if (invalid == 1)
		DEBASSERT("MESSAGE::addGenericHeader invalid")


	DEBOUT("MESSAGE::addGenericHeader",_header << " " << _content)

	flex_line.push_back(_header + " " + _content);

}

void MESSAGE::setFireTime(unsigned long long int _firetime){
	if (invalid == 1)
		DEBASSERT("MESSAGE::setFireTime invalid")


	fireTime = _firetime;
}
//
//void MESSAGE::setFireTime_x(SysTime _systime){
//
//	fireTime = _systime;
//}
unsigned long long int MESSAGE::getFireTime(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::getFireTime invalid")


	return fireTime;
}
string MESSAGE::getTransactionExtendedCID(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::getTransactionExtendedCID invalid")


	return getHeadCallId().getNormCallId() + getSTKHeadVia().top()->getC_AttVia().getViaParms().findRvalue("branch");

}
string MESSAGE::getDialogExtendedCID(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::getDialogExtendedCID invalid")

	//Call id and FromTag
	DEBY
	DEBOUT_UTIL("MESSAGE::getDialogExtendedCID(void) fromtag part", getHeadFrom()->getC_AttUriParms().getTuples().findRvalue("tag"))
	return getHeadCallId().getNormCallId() + getHeadFrom()->getC_AttUriParms().getTuples().findRvalue("tag");
}
void MESSAGE::setLock(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::setLock invalid")

	lock = true;
}
bool MESSAGE::getLock(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::getLock invalid")

	return lock;
}
void MESSAGE::unSetLock(void){
	if (invalid == 1){
		DEBASSERT("MESSAGE::unSetLock invalid")}

	DEBOUT("MESSAGE::unSetLock ", this)

	lock=false;
}

string MESSAGE::getOrderOfOperation(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::getOrderOfOperation invalid")

	return orderOfOperation;
}
void MESSAGE::setOrderOfOperation(string _s){
	if (invalid == 1)
		DEBASSERT("MESSAGE::setOrderOfOperation invalid")

	orderOfOperation = _s;
}
int MESSAGE::getTypeOfInternal(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::getTypeOfInternal invalid")

	return typeOfInternal;
}
void MESSAGE::setTypeOfInternal(int _i){
	if (invalid == 1)
		DEBASSERT("MESSAGE::setTypeOfInternal invalid")

	typeOfInternal = _i;
}
int MESSAGE::getType_trnsct(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::getType_trnsct invalid")

	return type_trnsct;
}
void MESSAGE::setType_trnsct(int _t){
	if (invalid == 1)
		DEBASSERT("MESSAGE::setType_trnsct invalid")

	type_trnsct = _t;
}
int MESSAGE::getTypeOfOperation(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::getTypeOfOperation invalid")

	return typeOfOperation;
}
void MESSAGE::setTypeOfOperation(int _t){
	if (invalid == 1)
		DEBASSERT("MESSAGE::setTypeOfOperation invalid")

	typeOfOperation = _t;
}

int MESSAGE::getModulus(void){
	if (invalid == 1)
		DEBASSERT("MESSAGE::getModulus invalid")


	//is calculated in two ways:
	// if first 5 chars of call id is "CoMap" then
	// the modulus is the number after it (single digit)
	// otherwise is calculated

	string s = getHeadCallId().getContent();
	DEBDEV("s.substr(0,5)", s.substr(0,5) )
	DEBDEV("s.substr(5,1)", s.substr(5,1) )
	if (s.substr(0,5).compare("CoMap") == 0){
		DEBY
		modulus = atoi(s.substr(5,1).c_str());
	}else {
		DEBY
		char x[64];
		int k = 64<s.length() ? 64 : s.length();
		sprintf(x,"%s", s.substr(0,k).c_str());
		long long int tot=0;
		for (int i = 0; i < k; i++){
			DEBDEV("substring ", "k [" << k << "][" <<  s.substr(i,1))
			tot = (long long int) atol(s.substr(i,1).c_str()) + tot;
		}
		modulus = (tot+COMAPS)%COMAPS;
	}
	DEBDEV("Modulus found", modulus<<"]["<< s)
	return modulus;



}
