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

#include <string>
#include <map>
#include <vector>
#include <assert.h>
#include <cstdlib>


#ifndef CS_HEADERS_H
#include "CS_HEADERS.h"
#endif

#ifndef UTIL_H
#include "UTIL.h"
#endif

// *********************************************************************************
// *********************************************************************************
// S_HeadGeneric
// *********************************************************************************
// *********************************************************************************
S_HeadGeneric::S_HeadGeneric(string buffer, int _genEntity) {

    content = buffer;
    genEntity = _genEntity;

    parsed = false;
    correct = true;

}
void S_HeadGeneric::setContent(string _content, int _genEntity) {

    genEntity = _genEntity;
    content = _content;
    parsed = false;
    correct = true;
    return;

}
string &S_HeadGeneric::getContent(void) {
    return content;
}
string S_HeadGeneric::copyContent(void) {
    return content;
}
bool S_HeadGeneric::isParsed(void) {
    return parsed;
}
bool S_HeadGeneric::isCorrect(void) {
    return correct;
}
int S_HeadGeneric::getGenEntity(void) {
    return genEntity;
}
// *********************************************************************************
// *********************************************************************************
// S_AttGeneric
// *********************************************************************************
// *********************************************************************************
S_AttGeneric::S_AttGeneric(const S_AttGeneric& x){
    DEBOUT("S_AttGeneric copy constructor", "")
    assert(0);
}
S_AttGeneric::S_AttGeneric(string _content) {
   content = _content;
   parsed = false;
   correct = true;
   isSet = false;
}
S_AttGeneric::S_AttGeneric(void) {
   parsed = false;
   correct = true;
   isSet = false;
   DEBOUT("S_AttGeneric empty constructor", "")
   assert(0);
}
string &S_AttGeneric::getContent(void) {
   return content;
}
string S_AttGeneric::copyContent(void) {
   return content;
}
bool S_AttGeneric::isParsed(void) {
    return parsed;
}
bool S_AttGeneric::isCorrect(void) {
    return correct;
}
void S_AttGeneric::setContent(string _content){
    if (isSet)
        return;
    else {
        isSet = true;
        content = _content;
    }
}
// *********************************************************************************
// *********************************************************************************
// TupleVector
// *********************************************************************************
// *********************************************************************************
TupleVector::TupleVector(void) {
DEBOUT("TupleVector::TupleVector(void)","")
    assert(0);
}
TupleVector::TupleVector(const TupleVector& _t) {
DEBOUT("COPY of TupleVector","")
    assert(0);
}
TupleVector::TupleVector(string tuples, string _separator) : S_AttGeneric(tuples) {

   separator = _separator;
   hasheader = false;
   header = "";
}
TupleVector::TupleVector(string tuples, string _separator, string _header) : S_AttGeneric(tuples) {

   separator = _separator;
   header = _header;
   hasheader = true;
}
void TupleVector::doParse(void) {

    if (parsed)
        return;
    vector<string> lval_rval;
    if (hasheader) {
        lval_rval = parse(content, header, separator,true);
    } else {
        lval_rval = parse(content, "", separator,true);
    }

    //TODO into tuples...
    vector<string>::iterator iter;
    Tuple tt;
    string ss;
    for ( iter = lval_rval.begin(); iter != lval_rval.end(); iter ++) {
        ss = *iter;
        tt = getLRvalue(ss);
        tuples.insert(make_pair(tt.Lvalue, tt.Rvalue));
    }
    parsed = true;
}
string TupleVector::findRvalue(string _Lvalue){

    if (!parsed) {
        doParse();
    }
    map<string,string>::iterator ii = tuples.find(_Lvalue);
    if (ii == tuples.end()) {
        return"";
    }
    string s = ii->second;
    return(s);
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// S_AttMethod
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
S_AttMethod::S_AttMethod(string _content)
    : S_AttGeneric(_content) {

    methodID = 0;
    methodName = "";
}
void S_AttMethod::doParse(void){

    if(parsed) {
        return;
    }
    if (content.compare("INVITE") == 0){
        methodName = "INVITE";
        methodID = INVITE_REQUEST;
        parsed = true;
        correct = true;
        return;
    }
    if (content.compare("ACK") == 0){
        methodName = "ACK";
        methodID = ACK_REQUEST;
        parsed = true;
        correct = true;
        return;
    }
    if (content.compare("BYE") == 0){
        methodName = "BYE";
        methodID = BYE_REQUEST;
        parsed = true;
        correct = true;
        return;
    }
    if (content.compare("CANCEL") == 0){
        methodName = "CANCEL";
        methodID = CANCEL_REQUEST;
        parsed = true;
        correct = true;
        return;
    }
    parsed = true;
    correct = false;
    return;
}
int S_AttMethod::getMethodID(void) {

    if (!parsed) {
        doParse();
    }
    return methodID;
}
string &S_AttMethod::getMethodName(void) {

    if (!parsed){
        doParse();
    }
    return methodName;
}
string S_AttMethod::copyMethodName(void) {

    if (!parsed){
        doParse();
    }
    return methodName;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// S_AttReply
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
S_AttReply::S_AttReply(string _content)
    :S_AttGeneric(_content){

    code = 0;
    replyID = 0;
    reply = "";
}
void S_AttReply::setContent(string _code, string _reply) {

    if (!compare_it(_reply)) {
        parsed = true;
        correct = false;
        return;
    }
    else {
        code = strtol(_code.c_str(),0x0, 0);
        string tmp = "" + code;
        DEBOUT("strtol ", _code + " " + tmp)
    }
    parsed = true;
    correct = false;
    return;
}
S_AttReply::S_AttReply(string _replyID, string _code)
    :S_AttGeneric(_code + " " +_replyID){

    if (!compare_it(_replyID)) {
        parsed = true;
        correct = false;
        return;
    }
    else {
        code = atoi(_code.c_str());
    }
    parsed = true;
    correct = false;
    return;
}
void S_AttReply::doParse(void) {

    if (parsed) {
        return;
    }

    vector<string> s;
    vector<string>::iterator ii;

    s = brkSpaces(content);
    ii = s.begin();

    string s1;
    s1 = *ii;
    code = atoi(s1.c_str());

    ii++;
    string _reply = *ii;

    if (compare_it(_reply))
        return;

    parsed = true;
    correct = false;
    return;
}
inline bool S_AttReply::compare_it(string _reply) {

    if (_reply.compare("OK") == 0){
        reply = "OK";
        replyID = OK_RESPONSE;
        parsed = true;
        correct = true;
        return true;
    }
    if (_reply.compare("RINGING") == 0){
        reply = "RINGING";
        replyID = RINGING_RESPONSE;
        parsed = true;
        correct = true;
        return true;
    }
    if (_reply.compare("TRY") == 0){
        reply = "TRY";
        replyID = TRY_RESPONSE;
        parsed = true;
        correct = true;
        return true;
    }
}
int S_AttReply::getCode(void){

   if(!parsed) {
        doParse();
   }
   return code;
}
int S_AttReply::getReplyID(void){

   if(!parsed) {
        doParse();
   }
   return replyID;
}
string &S_AttReply::getReply(void){

   if(!parsed) {
        doParse();
   }
   return reply;
}
string S_AttReply::copyReply(void){

   if(!parsed) {
        doParse();
   }
   return reply;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// S_AttSipVersion
// Fake
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
S_AttSipVersion::S_AttSipVersion(string content)
    : S_AttGeneric(content) {
}
S_AttSipVersion::S_AttSipVersion(string _protocol, string _version)
    :S_AttGeneric(_protocol + "/" + _version) {
    parsed = true;
    correct = true;
    isSet = true;

    version = _version;
    protocol = _protocol;

    content = protocol + "/" + version;
}
void S_AttSipVersion::setbContent(string _protocol, string _version) {

    parsed = true;
    correct = true;
    isSet = true;

    version = _version;
    protocol = _protocol;

    content = protocol + "/" + version;
}

void S_AttSipVersion::doParse(void){

    if(parsed)
        return;

    Tuple s1 = brkin2(content, "/");
    protocol = s1.Lvalue;
    version = s1.Rvalue;

    correct = true;
    parsed = true;

    return;
}
string &S_AttSipVersion::getProtocol(void) {
    if(!parsed){
        doParse();
    }
    return protocol;
}
string &S_AttSipVersion::getVersion(void){
    if(!parsed){
        doParse();
    }
    return version;
}
string S_AttSipVersion::copyProtocol(void) {
    if(!parsed){
        doParse();
    }
    return protocol;
}
string S_AttSipVersion::copyVersion(void){
    if(!parsed){
        doParse();
    }
    return version;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// S_AttUserInfo
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
S_AttUserInfo::S_AttUserInfo(string _content)
    :S_AttGeneric(_content){
    return;
}
S_AttUserInfo::S_AttUserInfo(const S_AttUserInfo& x){
assert(0);
}
void S_AttUserInfo::doParse(void){

    if (parsed) {
        return;
    }

    Tuple s1 = brkin2(content, ":");

    userName = s1.Lvalue;
    password = s1.Rvalue;

    correct = true;
    parsed = true;
    return;
}
string &S_AttUserInfo::getUserName(void){
    if (!parsed)
        doParse();
    return userName;
}
string &S_AttUserInfo::getPassword(void){
    if (!parsed)
        doParse();
    return password;
}
string S_AttUserInfo::copyUserName(void){
    if (!parsed)
        doParse();
    return userName;
}
string S_AttUserInfo::copyPassword(void){
    if (!parsed)
        doParse();
    return password;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// S_AttHostPort stub
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
S_AttHostPort::S_AttHostPort(string _content)
    :S_AttGeneric(_content){
    return;
}
void S_AttHostPort::doParse(void){

    if (parsed) {
        return;
    }

    Tuple s1 = brkin2(content, ":");

    hostName = s1.Lvalue;
    if (s1.Rvalue.compare("")!=0){
        port = atoi(s1.Rvalue.c_str());
    }
    else {
        port = 0;
    }

    correct = true;
    parsed = true;
    return;
}
string &S_AttHostPort::getHostName(void){
    if (!parsed)
        doParse();
    return hostName;
}
string S_AttHostPort::copyHostName(void){
    if (!parsed)
        doParse();
    return hostName;
}
int S_AttHostPort::getPort(void){
    if (!parsed)
        doParse();
    return port;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// C_AttUriParms
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
C_AttUriParms::C_AttUriParms(string _content)
    : S_AttGeneric(_content),
      tuples(_content, ";") {
    return;
}
void C_AttUriParms::doParse(void){
    parsed = true;
    return;
}
TupleVector &C_AttUriParms::getTuples(void){
    return tuples;
}
TupleVector C_AttUriParms::copyTuples(void){
    return tuples;
}
C_AttUriParms::C_AttUriParms(const C_AttUriParms& _p) {
DEBOUT("COPY of C_AttUriParms","")
    assert(0);
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// C_AttUriHeaders
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
C_AttUriHeaders::C_AttUriHeaders(string _content)
    : S_AttGeneric(_content),
      tuples(_content, "&", "?") {
    return;
}
void C_AttUriHeaders::doParse(void){
    return;
}
TupleVector &C_AttUriHeaders::getTuples(void){
    return tuples;
}
TupleVector C_AttUriHeaders::copyTuples(void){
    return tuples;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// C_AttSipUri
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
C_AttSipUri::C_AttSipUri(string _content)
    : S_AttGeneric(_content) ,
    userInfo(""),
    hostPort(""),
    uriParms(""),
    uriHeads(""){

    return;
}
C_AttSipUri::C_AttSipUri(const C_AttSipUri& x):
    userInfo(""),
    hostPort(""),
    uriParms(""),
    uriHeads(""){
    DEBOUT("C_AttSipUri copy constructor", "")
assert(0);
}
void C_AttSipUri::setContent(string _content) {
    content = _content;
    isSet = true;
    parsed = false;
    correct = true;
}
void C_AttSipUri::doParse(void){

    if(parsed) {
        return;
    }
    // "sip:alice:secretword@atlanta.com;transport=tcp;ttl=15?to=alice%40atalnta.com&priority=urgent"
    // break ;
    Tuple s1 = brkin2(content, ":");
    isSecure = false;
    if (s1.Lvalue.compare("sips") == 0) {
        isSecure= true;
    }
    Tuple s2 = brkin2(s1.Rvalue, "@");
    userInfo.setContent(s2.Lvalue);

    Tuple s3 = brkin2(s2.Rvalue,";");
    hostPort.setContent(s3.Lvalue);

    if (s3.Rvalue.compare("") != 0) {
        C_AttUriHeaders getC_AttUriHeads(void);
        Tuple s4 = brkin2(s3.Rvalue,"?");
        uriParms.setContent(s4.Lvalue);
        uriHeads.setContent(s4.Rvalue);
    }
    else {
        uriParms.setContent("");
        uriHeads.setContent("");
    }


    parsed = true;
    correct = true;

    return;
}
bool C_AttSipUri::getIsSec(void){
    if (!parsed)
        doParse();
    return isSecure;

}
S_AttUserInfo &C_AttSipUri::getS_AttUserInfo(void){
    if (!parsed)
        doParse();
    return userInfo;
}
S_AttHostPort &C_AttSipUri::getS_AttHostPort(void){
    if (!parsed)
        doParse();
    return hostPort;
}
C_AttUriParms &C_AttSipUri::getC_AttUriParms(void){
    if (!parsed)
        doParse();
    return uriParms;
}
C_AttUriHeaders &C_AttSipUri::getC_AttUriHeads(void){
    if (!parsed)
        doParse();
    return uriHeads;
}
S_AttUserInfo C_AttSipUri::copyS_AttUserInfo(void){
    if (!parsed)
        doParse();
    return userInfo;
}
S_AttHostPort C_AttSipUri::copyS_AttHostPort(void){
    if (!parsed)
        doParse();
    return hostPort;
}
C_AttUriParms C_AttSipUri::copyC_AttUriParms(void){
    if (!parsed)
        doParse();
    return uriParms;
}
C_AttUriHeaders C_AttSipUri::copyC_AttUriHeads(void){
    if (!parsed)
        doParse();
    return uriHeads;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// C_AttVia
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
C_AttVia::C_AttVia(string _content) :
    S_AttGeneric(_content),
    version(""),
    transport(""),
    hostPort(""),
    viaParms("",";"){
    return;
}
void C_AttVia::doParse(void){

    if(parsed)
        return;

    Tuple s1 = brkin2(content, " ");

    Tuple s2 = brkin2(s1.Lvalue, "/");
    Tuple s3 = brkin2(s2.Rvalue, "/");
    transport = s3.Rvalue;
    version.setbContent(s2.Lvalue, s3.Lvalue);

    Tuple s4 = brkin2(s1.Rvalue, ";");
    hostPort.setContent(s4.Lvalue);
    viaParms.setContent(s4.Rvalue);

    correct = true;
    parsed = true;

    return;
}
S_AttSipVersion &C_AttVia::getS_AttSipVersion(void) {
    if (!parsed)
        doParse();
    return version;
}
string &C_AttVia::getTransport(void) {
    if (!parsed)
        doParse();
    return transport;
}
S_AttHostPort &C_AttVia::getS_HostHostPort(void) {
    if (!parsed)
        doParse();
    return hostPort;
}
TupleVector &C_AttVia::getViaParms(void) {
    if (!parsed)
        doParse();
    return viaParms;
}
S_AttSipVersion C_AttVia::copyS_AttSipVersion(void) {
    if (!parsed)
        doParse();
    return version;
}
string C_AttVia::copyTransport(void) {
    if (!parsed)
        doParse();
    return transport;
}
S_AttHostPort C_AttVia::copyS_HostHostPort(void) {
    if (!parsed)
        doParse();
    return hostPort;
}
TupleVector C_AttVia::copyViaParms(void) {
    if (!parsed)
        doParse();
    return viaParms;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// C_AttContactElem
//    "Mr. Watson" <sip:watson@worcester.bell-telephone.com>
//       ;q=0.7; expires=3600
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
C_AttContactElem::C_AttContactElem(string _content) :
    S_AttGeneric(_content),
    sipUri(""),
    uriParms(""){
}
C_AttContactElem::C_AttContactElem(const C_AttContactElem& x):
    S_AttGeneric(x.content),
    sipUri(""),
    uriParms(""){
    DEBOUT("C_AttContactElem copy constructor", x.content)

    //content = x.copyContent();
    parsed = false;
}
void C_AttContactElem::doParse(void){

    if(parsed)
        return;

    string line;

    int q = hasQuote(content);
    if (q >=0){
    	string line1 = replaceHttpChars(content.substr(q+1, -1));

		line = trimSpaces(content.substr(0,q+1) + line1);

    }
    else {
    	line = trimSpaces(content);
    }

    Tuple s1 = brkin2(line, "<");
    nameUri = s1.Lvalue; // "Mr. Watson"
    string tmp = "<" + s1.Rvalue;

    Tuple s2 = brkin2(tmp, ";");
    sipUri.setContent(s2.Lvalue);
    uriParms.setContent(s2.Rvalue);

    parsed = true;
    return;
}
string &C_AttContactElem::getNameUri(void){

    if(!parsed)
        doParse();

    return nameUri;
}
string C_AttContactElem::copyNameUri(void){

    if(!parsed)
        doParse();

    return nameUri;
}

C_AttSipUri &C_AttContactElem::getC_AttSipUri(void){

    if(!parsed)
        doParse();

    return sipUri;
}
C_AttSipUri C_AttContactElem::copyC_AttSipUri(void){

    if(!parsed)
        doParse();

    return sipUri;
}
C_AttUriParms &C_AttContactElem::getC_AttUriParms(void){

    if(!parsed)
        doParse();

    return uriParms;
}
C_AttUriParms C_AttContactElem::copyC_AttUriParms(void){

    if(!parsed)
        doParse();

    return uriParms;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//      "Mr. Watson" <sip:watson@worcester.bell-telephone.com>
//       ;q=0.7; expires=3600,
//      "Mr. Watson" <mailto:watson@bell-telephone.com>
//        ;q=0.1
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
C_AttContactList::C_AttContactList(string _content):
    S_AttGeneric(_content) {
}
void C_AttContactList::doParse(void){

    if (parsed)
        return;

    vector<string> lines = parse(content, "", ",", false);

    vector<string>::iterator iter;
    for( iter = lines.begin(); iter != lines.end(); iter++ ) {
        C_AttContactElem tmp(*iter);
        contactList.push_back(tmp);
    }

    parsed = true;

    return;
}
vector<C_AttContactElem> &C_AttContactList::getContactList(void){

    if (!parsed)
        doParse();

    return contactList;
}
vector<C_AttContactElem> C_AttContactList::copyContactList(void){

    if (!parsed)
        doParse();

    return contactList;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// HeadSipRequest
// HeadSipRequest "Method RequestURI SipVersion<CRLF>"
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
C_HeadSipRequest::C_HeadSipRequest(string _content, int _genEntity)
    : S_HeadGeneric(_content, _genEntity),
        method(""),
        reqUri(""),
        sipvs(""){
}
void C_HeadSipRequest::doParse(void) {

    if(parsed)
        return;

    vector<string> elements = brkSpaces(content);

    //S_AttMethod
    vector<string>::iterator iter;
    iter = elements.begin();
    method.setContent(*iter);

    iter ++;
    reqUri.setContent(*iter);

    iter++;
    sipvs.setContent(*iter);

    correct = true;
    parsed = true;

}
S_AttMethod &C_HeadSipRequest::getS_AttMethod(void){
    if(!parsed)
        doParse();
    return method;
}
C_AttSipUri &C_HeadSipRequest::getC_AttSipUri(void){
    if(!parsed)
        doParse();

    return reqUri;
}
S_AttSipVersion &C_HeadSipRequest::getS_AttSipVersion(void){
    if(!parsed)
        doParse();
    return sipvs;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// C_HeadSipReply
// C_HeadSipReply "SIP/2.0 200 OK"
// C_HeadSipReply "SIP/2.0 xxx reply"
// Status-Line	= SIP-Version Status-Code Reason-Phrase
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
C_HeadSipReply::C_HeadSipReply(string _content, int _genEntity)
    : S_HeadGeneric(_content, _genEntity),
        reply(""),
        sipvs(""){
}
void C_HeadSipReply::doParse(void){

    if(parsed){
        return;
    }

    vector<string> elements = brkSpaces(content);

    vector<string>::iterator iter;
    iter = elements.begin();
    sipvs.setContent(*iter);

    iter++;
    string s1 = *iter;
    iter++;
    string s2 = *iter;
    //TODO S_AttReply will re-parse it...
    reply.setContent(s1, s2);

    parsed = true;
    //TODO
    correct = true;

    return;
}
S_AttReply &C_HeadSipReply::getReply(void) {

    if(!parsed) {
        doParse();
    }
    return reply;
}
S_AttSipVersion &C_HeadSipReply::getSipVersion(void) {

    if(!parsed) {
        doParse();
    }
    return sipvs;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// HeadVia
// Via: xxxxx
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
C_HeadVia::C_HeadVia(string _content, int _genEntity, int _position) :
    S_HeadGeneric(_content, _genEntity),
    via("") {

    position = _position;
}
void C_HeadVia::doParse(void) {

    if(parsed)
        return;

    Tuple s1 = brkin2(content, " ");
    via.setContent(s1.Rvalue);

    parsed = true;
}
int C_HeadVia::getPosition(void){
    return position;
}
void C_HeadVia::setPosition(int _position){
    position = _position;
}
C_AttVia &C_HeadVia::getC_AttVia(void) {

    if (!parsed)
        doParse();

    return via;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// S_HeadMaxFwd
// Max-Forwards: 70
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
S_HeadMaxFwd::S_HeadMaxFwd(string _content, int _genEntity) :
    S_HeadGeneric(_content, _genEntity){

    mxfwd = 0;
}
void S_HeadMaxFwd::doParse(void) {

    if(parsed)
        return;

    Tuple s1 = brkin2(content, " ");
    mxfwd = atoi(s1.Rvalue.c_str());

    parsed = true;
}
int S_HeadMaxFwd::getMaxFwd(void){
    if (!parsed)
        doParse();
    return mxfwd;
}
void S_HeadMaxFwd::setMaxFwd(int _mxfwd){
    mxfwd = _mxfwd;
    return;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// C_HeadContact
//    Contact: "Mr. Watson" <sip:watson@worcester.bell-telephone.com>
//       ;q=0.7; expires=3600,
//      "Mr. Watson" <mailto:watson@bell-telephone.com>
//        ;q=0.1
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
C_HeadContact::C_HeadContact(string _content, int _genEntity) :
    S_HeadGeneric(_content, _genEntity),
    contactList(_content){

	star = false;
}
void C_HeadContact::doParse(void){

	if (parsed)
		return;

	//TODO if *

	parsed = true;

}
C_AttContactList &C_HeadContact::getContactList(void){
	if (!parsed)
		doParse();
	return contactList;
}
C_AttContactList C_HeadContact::copyContactList(void){
	if (!parsed)
		doParse();
	return contactList;
}
bool C_HeadContact::isStar(void){
	if (!parsed)
		doParse();
	return star;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// C_HeadTo
//  To: Bob <sip:bob@biloxi.example.com>;tag=8321234356
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
C_HeadTo::C_HeadTo(string _content, int _genEntity) :
    S_HeadGeneric(_content, _genEntity),
    to(_content){
}
void C_HeadTo::doParse(void){

	if (parsed)
		return;

	parsed = true;

}
C_AttContactElem &C_HeadTo::getTo(void){
	if (!parsed)
		doParse();
	return to;
}
C_AttContactElem C_HeadTo::copyTo(void){
	if (!parsed)
		doParse();
	return to;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// C_HeadFrom
//  From: Bob <sip:bob@biloxi.example.com>;tag=8321234356
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
C_HeadFrom::C_HeadFrom(string _content, int _genEntity) :
    S_HeadGeneric(_content, _genEntity),
    from(_content){
}
void C_HeadFrom::doParse(void){

	if (parsed)
		return;

	parsed = true;

}
C_AttContactElem &C_HeadFrom::getFrom(void){
	if (!parsed)
		doParse();
	return from;
}
C_AttContactElem C_HeadFrom::copyFrom(void){
	if (!parsed)
		doParse();
	return from;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// HeadCallID
// 238556723098563298463789@hsfalkgjhaslgh.com
// id1@id2
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
C_HeadCallId::C_HeadCallId(string _content, int _genEntity):
	S_HeadGeneric(_content, _genEntity){
}
void C_HeadCallId::doParse(void){

	if (parsed)
		return;

	callId = brkin2(content, "@");

	parsed = true;

	return;
}
Tuple &C_HeadCallId::getCallId(void){
	if (!parsed){
		doParse();
	}
	return callId;
}
Tuple C_HeadCallId::copyCallId(void){
	if (!parsed){
		doParse();
	}
	return callId;
}
string C_HeadCallId::getNormCallId(void){
	if (!parsed){
		doParse();
	}
	return content;
	//return (callId.Lvalue + "@" + callId.Rvalue);
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// HeadCSeq
// CSeq: 1 INVITE
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
C_HeadCSeq::C_HeadCSeq(string _content, int _genEntity):
	S_HeadGeneric(_content, _genEntity),
	method(""){
}
void C_HeadCSeq::doParse(void){

	if(parsed)
		return;

	Tuple s = brkin2(content, " ");
	sequence = atoi(s.Lvalue.c_str());
	method.setContent(s.Rvalue);

	parsed = true;

	return;

}
int C_HeadCSeq::getSequence(void){

	if(!parsed)
		doParse();
	return sequence;
}
S_AttMethod &C_HeadCSeq::getMethod(void){

	if(!parsed)
		doParse();
	return method;
}
S_AttMethod C_HeadCSeq::copyMethod(void){

	if(!parsed)
		doParse();
	return method;
}
