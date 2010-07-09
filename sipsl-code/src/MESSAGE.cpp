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

    	// if space or tab is the first char then the line must be merged into the previous one
    	// header: aa
    	// <sp>bb
    	// <tb>cc
    	// will be filled in this way
    	// header: aa<sp>bb<sp>cc

    	s = brkin2(t,"\n");
    	t = s.Rvalue;

    	if (s.Lvalue.substr(0,1).compare(" ") == 0 || s.Lvalue.substr(0,1).compare("\t") == 0){
    		string tt = flex_line.back();
    		string ttt = tt + trimCR(s.Lvalue);
    		flex_line.pop_back();
            flex_line.push_back(ttt);
            DEBOUT("COMPACTING", ttt)
    	} else{
            flex_line.push_back(trimCR(s.Lvalue));
    	}
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
int BASEMESSAGE::getInitialGenEntity(void){
	return initialGenEntity;
}
void BASEMESSAGE::setInitialGenEntity(int _genEntity){
	initialGenEntity = _genEntity;
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

	source=0x0;

	isInternal = false;

	type_trnsct = TYPE_TRNSCT;

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
	sdpVector_p = false;
	sdpSize = 0;
	reqRep = 0;
	//, headRoute_e;

	lock = false;

	type_trnsct = TYPE_TRNSCT;



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
    if (getGenericHeader("Content-Type:").size() == 0)
    	flex_line.insert(theIterator ,1 , "");

    for( theIterator = flex_line.begin(); theIterator != flex_line.end(); theIterator++ ) {

    	incMessBuff = incMessBuff + (*theIterator) + "\r\n";

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
			C_HeadVia* s = new C_HeadVia(flex_line[i].substr(5));
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
		flex_line.insert(theIterator ,1 , _content);
	}
}
void MESSAGE::purgeSDP(void){

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

	if ( sdpVector_p )  {
		return sdpSize;
	}
	else {
		getSDP();
		return sdpSize;
	}

}
void MESSAGE::importSDP(vector<string> _sdp){

	vector<string>::iterator theIterator;

	sdpVector = _sdp;

	theIterator = _sdp.begin();
	sdpSize = 0;
	sdpVector_p = true;
	if (theIterator!= _sdp.end()){
		flex_line.push_back("Content-Length: 0");
		flex_line.push_back("Content-Type: application/sdp");
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
			headTo.setContent(flex_line[i].substr(4));
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
			headFrom.setContent(flex_line[i].substr(6));
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
void MESSAGE::replaceHeadTo(string _content){

	headTo_p = false;
	headTo.setContent(_content);

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
			headCSeq.setContent(flex_line[i].substr(6));
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
void MESSAGE::replaceHeadCSeq(int _cseq, string _method){

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
			headContact.setContent(flex_line[i].substr(9));
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
string MESSAGE::getGenericHeader(string _header){

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

	DEBOUT("MESSAGE::addGenericHeader",_header << " " << _content)

	flex_line.push_back(_header + " " + _content);

}

void MESSAGE::setFireTime(SysTime _systime){

	fireTime = _systime;
}
SysTime MESSAGE::getFireTime(void){

	return fireTime;
}
string MESSAGE::getTransactionExtendedCID(void){

	return getHeadCallId().getNormCallId() + getSTKHeadVia().top()->getC_AttVia().getViaParms().findRvalue("branch");

}
string MESSAGE::getDialogExtendedCID(void){
	//Call id and FromTag
	DEBY
	DEBOUT_UTIL("MESSAGE::getDialogExtendedCID(void) fromtag part", getHeadFrom().getC_AttUriParms().getTuples().findRvalue("tag"))
	return getHeadCallId().getNormCallId() + getHeadFrom().getC_AttUriParms().getTuples().findRvalue("tag");
}
void MESSAGE::setLock(void){
	lock = true;
}
bool MESSAGE::getLock(void){
	return lock;
}
void MESSAGE::unSetLock(void){
	lock=false;
}

