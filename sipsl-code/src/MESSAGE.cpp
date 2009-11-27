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

	//others
    id = 0; //Used in spin buffer
    destEntity = 0;
	key="";

    totLines=0;
    arrayFilled=false;

	return;
}
BASEMESSAGE::BASEMESSAGE(string _incMessBuff, SysTime _inc_ts):
						incMessBuff(_incMessBuff){
	//TODO only for test
	incMessBuff=_incMessBuff;

	inc_ts = _inc_ts;

	arrayFilled = false;


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

	return;
}

vector<string> BASEMESSAGE::getLines(void){
	return flex_line;
}

void BASEMESSAGE::fillLineArray(void){

    if (arrayFilled)
        return;

    Tuple s = brkin2(incMessBuff,"\n");
    string t = s.Rvalue;
    flex_line.push_back(trimCR(s.Lvalue));
    while (t.compare("")!=0){
    	s = brkin2(t,"\n");
    	t = s.Rvalue;
        flex_line.push_back(trimCR(s.Lvalue));
    }

    arrayFilled = true;

    return;
}
// *****************************************************************************************
// *****************************************************************************************
int BASEMESSAGE::getTotLines() {

    if (!arrayFilled){
    	fillLineArray();
    }

    return(flex_line.size());
}
// *****************************************************************************************
// *****************************************************************************************
string &BASEMESSAGE::getLine(int pos){
    // no copia REFERRENCE

    if (!arrayFilled)
    	fillLineArray();

    int i = getTotLines();

    if (pos >= i)
        pos = i-1;

    return(flex_line[pos]);
}
string &BASEMESSAGE::getIncBuffer(void){
	return incMessBuff;
}
int BASEMESSAGE::getGenEntity(void){
	return genEntity;
}
void BASEMESSAGE::setGenEntity(int _genEntity){
	genEntity = _genEntity;
}
int BASEMESSAGE::getDestEntity(void){
	return destEntity;
}
void BASEMESSAGE::setDestEntity(int _destEntity){
	destEntity = _destEntity;
}
void BASEMESSAGE::setEndPoints(int _genEntity, int _destEntity){
	destEntity = _destEntity;
	genEntity = _genEntity;
}

string &BASEMESSAGE::getKey(void){
	return key;
}
void BASEMESSAGE::setKey(string _key){
	key = _key;
}
SysTime BASEMESSAGE::getCreationTime(void){
	return inc_ts;
}
struct sockaddr_in BASEMESSAGE::getAddress(void){
	return echoClntAddr;
}
int BASEMESSAGE::getSock(void){
	return sock;
}
void BASEMESSAGE::removeHeader(int _pos){
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
	headContact(""),
	headTo(""),
	headFrom(""),
	headCallId(""),
	headCSeq(""),
	headRoute(""){

	reqRep = 0;
	isInternal = false;

	s_headVia_p = false;
	headMaxFwd_p = false;
	headContact_p = false;
	headTo_p = false;
	headFrom_p = false;
	headCallId_p = false;
	headCSeq_p = false;
	headRoute_p = false;
	headRoute_e = false;

	source=0x0;

	isInternal = false;

}

MESSAGE::MESSAGE(MESSAGE* _message, int _genEntity, SysTime _creaTime):
	BASEMESSAGE(_message, _genEntity, _creaTime),
	headSipRequest(""),
	headSipReply(""),
	headMaxFwd(""),
	headContact(""),
	headTo(""),
	headFrom(""),
	headCallId(""),
	headCSeq(""),
	headRoute(""){
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
	//, headRoute_e;

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

	vector<string>::iterator theIterator;

    for( theIterator = flex_line.begin(); theIterator != flex_line.end(); theIterator++ ) {
    	if ((*theIterator).size() == 0) {
    		return theIterator--;
    	}
    }
    return theIterator;
}
void MESSAGE::dumpVector(void){
	vector<string>::iterator theIterator;
    for( theIterator = flex_line.begin(); theIterator != flex_line.end(); theIterator++ ) {
    	DEBOUT("Message vector", *theIterator)
    }
}
int MESSAGE::getReqRepType(void){

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

	// first look for xxDxx and remove it
	vector<string>::iterator theIterator;
    for( theIterator = flex_line.begin(); theIterator != flex_line.end(); theIterator++ ) {
    	if ((*theIterator).substr(0,5).compare("xxDxx") == 0 ) {
    		flex_line.erase(theIterator);
    		theIterator--;
    	}
    }
    incMessBuff = "";
	flex_line.insert(theIterator ,1 , "");
    for( theIterator = flex_line.begin(); theIterator != flex_line.end(); theIterator++ ) {
    	incMessBuff = incMessBuff + (*theIterator) + "\r\n";
//    	if (theIterator != flex_line.end()) {
//    		incMessBuff = incMessBuff + (*theIterator) + "\r\n";
//    	}else {
//    		incMessBuff = incMessBuff + (*theIterator) + "\r\n";
//    	}

    }

}
MESSAGE* MESSAGE::getSourceMessage(void){
	return source;
}
void MESSAGE::setSourceMessage(MESSAGE* _source){
	source = _source;
}


/*
 * Request
 */
C_HeadSipRequest &MESSAGE::getHeadSipRequest(void){
	if(reqRep == 0){
		reqRep = getReqRepType();
	}

	if (reqRep == REQSUPP || reqRep == REQUNSUPP){
		return headSipRequest;
	}
    assert("MESSAGE::getHeadSipRequest illegal instruction");
    return headSipRequest;
}
void MESSAGE::setHeadSipRequest(string _content){
	reqRep = 0;
	flex_line[0] = _content;
	headSipRequest.setContent(_content);
}
void MESSAGE::setHeadSipReply(string _content){
	reqRep = 0;
	flex_line[0] = _content;
	headSipReply.setContent(_content);
}
C_HeadSipReply &MESSAGE::getHeadSipReply(void){

	if(reqRep == 0){
		reqRep = getReqRepType();
	}

	if (reqRep == REPSUPP || reqRep == REPUNSUPP){
		return headSipReply;
	}
    assert("MESSAGE::getHeadSipReply illegal instruction");
    return headSipReply;
}
/*
 * Via
 */
stack<C_HeadVia*> &MESSAGE::getSTKHeadVia(void){

	if(s_headVia_p){
		return s_headVia;
	}

	unsigned int i;
	unsigned int j = 1;

	for(i = 1; i < flex_line.size(); i ++){
		if(flex_line[i].substr(0,4).compare("Via:") == 0){
			C_HeadVia* s = new C_HeadVia(flex_line[i]);
			s_headVia.push(s);
		}
	}
	s_headVia_p = true;
	return s_headVia;
}
void MESSAGE::popSTKHeadVia(void){

	if(s_headVia_p){
		getSTKHeadVia();
	}

	unsigned int i;
	unsigned int j = 1;
	bool topout = false;
	for(i = 1; i < flex_line.size(); i ++){
		if(flex_line[i].substr(0,4).compare("Via:") == 0){
			if (!topout){
				flex_line[i] = "xxDxx";
				topout = true;
			} else {
				C_HeadVia* s = new C_HeadVia(flex_line[i]);
				s_headVia.push(s);
			}
		}
	}
	s_headVia_p = true;
}

void MESSAGE::purgeSTKHeadVia(void){

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
			delete s_headVia.top();
			s_headVia.pop();
		}
		stack<C_HeadVia*> tmp;
		s_headVia = tmp;
		s_headVia_p = false;
	}
}
void MESSAGE::pushHeadVia(string _content){

	C_HeadVia* s = new C_HeadVia(_content);
	s_headVia.push(s);
	DEBOUT("MESSAGE::pushHeadVia", "pushed")
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
		DEBOUT("MESSAGE::pushHeadVia", "found")
		flex_line.insert(theIterator ,1 , "Via: " + _content);
	}
	else {
		DEBOUT("MESSAGE::pushHeadVia", "not found")
		theIterator = getSDPposition();
		flex_line.insert(theIterator ,1 , "Via: " + _content);
	}
}
void MESSAGE::purgeSDP(void){

	vector<string>::iterator theIterator;
	vector<string>::iterator theIteratorEnd;

	theIterator = getSDPposition();
	theIteratorEnd = flex_line.end();
	if (theIterator == flex_line.end()){
		//nothing to erase
		return;
	}

	flex_line.erase(theIterator, theIteratorEnd);
}

/*
 * MaxFwd
 */
S_HeadMaxFwd& MESSAGE::getHeadMaxFwd(void){

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
C_HeadTo &MESSAGE::getHeadTo(void){

	if(headTo_p){
		return headTo;
	}

	int i;

	for(i = 1; i < flex_line.size(); i ++){
		if(flex_line[i].substr(0,3).compare("To:")==0){
			headTo.setContent(flex_line[i]);
			break;
		}
	}
	headTo_p = true;
	return headTo;
}
/*
 * From
 */
C_HeadFrom &MESSAGE::getHeadFrom(void){

	if(headFrom_p){
		return headFrom;
	}

	unsigned int i;

	for(i = 1; i < flex_line.size(); i ++){
		if(flex_line[i].substr(0,5).compare("From:")==0){
			headFrom.setContent(flex_line[i]);
			break;
		}
	}
	headFrom_p = true;
	return headFrom;
}
void MESSAGE::replaceHeadFrom(string _content){

	headFrom_p = false;
	headFrom.setContent(_content);

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
/*
 * Call Id
 */
C_HeadCallId &MESSAGE::getHeadCallId(void){

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

	if(headCSeq_p){
		return headCSeq;
	}

	unsigned int i;

	for(i = 1; i < flex_line.size(); i ++){
		if(flex_line[i].substr(0,5).compare("CSeq:")==0){
			headCSeq.setContent(flex_line[i]);
			break;
		}
	}
	headCSeq_p = true;
	return headCSeq;
}
void MESSAGE::replaceHeadCSeq(string _content){

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

/*
 * Route
 */
C_HeadRoute &MESSAGE::getHeadRoute(void) throw (HeaderException){

	if(headRoute_e){
		DEBOUT("MESSAGE::getHeadRoute","No Route header")
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
			headRoute.setContent(flex_line[i]);
			break;
		}
	}
	headRoute_p = true;
	if (found == true){
		return headRoute;
	}
	else {
		headRoute_e = true;
		DEBOUT("MESSAGE::getHeadRoute","No Route header")
		throw HeaderException("No Route header");
	}
}
void MESSAGE::removeHeadRoute(void){
	headRoute_p = false;
	unsigned int i;
	for(i = 1; i < flex_line.size(); i ++){
		if(flex_line[i].substr(0,6).compare("Route:")==0){
			removeHeader(i);
			break;
		}
	}
	return;
}
/*
 * Contact
 */
C_HeadContact &MESSAGE::getHeadContact(void){

	if(headContact_p){
		return headContact;
	}

	int i;

	for(i = 1; i < flex_line.size(); i ++){
		if(flex_line[i].substr(0,8).compare("Contact:")==0){
			headContact.setContent(flex_line[i]);
			break;
		}
	}
	headContact_p = true;
		return headContact;
}

void MESSAGE::replaceHeadContact(string _content){

	headContact_p = false;
	headContact.setContent(_content);

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
	return ;
}

void MESSAGE::dropHeader(string _header){

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

	DEBOUT("MESSAGE::setGenericHeader", _header + " " + _content)
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
void addGenericHeader(string _header, string _content){

}

void MESSAGE::setFireTime(SysTime _systime){

	fireTime = _systime;
}
SysTime MESSAGE::getFireTime(void){

	return fireTime;
}
string MESSAGE::getExtendedInternalCID(void){

	return getHeadCallId().getNormCallId() + getSTKHeadVia().top()->getC_AttVia().getViaParms().findRvalue("branch");

}
