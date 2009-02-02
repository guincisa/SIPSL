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

#include <string>
#include <map>
#include <vector>
#include <assert.h>
#ifndef CS_HEADERS 
#include "CS_HEADERS.h"
#endif

#ifndef UTIL
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
    isSet = true;

}
void S_HeadGeneric::setContent(string _content, int _genEntity) {

    if (isSet) {
        return;
    }
    genEntity = _genEntity;
    content = _content;
    parsed = false;
    correct = true;
    isSet = true;
    return;

}
string S_HeadGeneric::getContent(void) {
    return content;
}
bool S_HeadGeneric::isParsed(void) {
    return parsed;
}
bool S_HeadGeneric::isCorrect(void) {
    return correct;
}
// *********************************************************************************
// *********************************************************************************
// S_AttGeneric
// *********************************************************************************
// *********************************************************************************
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
   assert(0);
}
string S_AttGeneric::getContent(void) {
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
        lval_rval = parse(content, header, separator);
    } else {
        lval_rval = parse(content, "", separator);
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
}
string TupleVector::findRvalue(string _Lvalue){

    if (parsed != true) {
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
string S_AttMethod::getMethodName(void) {
    
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
        code = atoi(_code.c_str());
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
string S_AttReply::getReply(void){

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

    if(parsed){
        return;
    }
    //TODO
    parsed = true;
}
string S_AttSipVersion::getProtocol(void) {
    //TODO
    return("SIP");
}
string S_AttSipVersion::getVersion(void){
    //TODO
    return("2.0");
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
string S_AttUserInfo::getUserName(void){
    if (!parsed)
        doParse();
    return userName;
}
string S_AttUserInfo::getPassword(void){
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
string S_AttHostPort::getHostName(void){
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
    return;
}
TupleVector C_AttUriParms::getTuples(void){
    return tuples;
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
TupleVector C_AttUriHeaders::getTuples(void){
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
S_AttUserInfo C_AttSipUri::getS_AttUserInfo(void){
    if (!parsed)
        doParse();
    return userInfo;
}
S_AttHostPort C_AttSipUri::getS_AttHostPort(void){
    if (!parsed)
        doParse();
    return hostPort;
}
C_AttUriParms C_AttSipUri::getC_AttUriParms(void){
    if (!parsed)
        doParse();
    return uriParms;
}
C_AttUriHeaders C_AttSipUri::getC_AttUriHeads(void){
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
S_AttSipVersion C_AttVia::getS_AttSipVersion(void) {
    if (!parsed)
        doParse();
    return version;
}
string C_AttVia::getTransport(void) {
    if (!parsed)
        doParse();
    return transport;
}
S_AttHostPort C_AttVia::getS_HostHostPort(void) {
    if (!parsed)
        doParse();
    return hostPort;
}
TupleVector C_AttVia::getViaParms(void) {
    if (!parsed)
        doParse();
    return viaParms;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// HeadSipRequest
// HeadSipRequest "Method RequestURI SipVersion<CRLF>"
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
C_HeadSipRequest::C_HeadSipRequest(string _content, int genEntity)
    : S_HeadGeneric(_content, genEntity),
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
S_AttMethod C_HeadSipRequest::getS_AttMethod(void){
    if(!parsed)
        doParse();
    return method;
}
C_AttSipUri C_HeadSipRequest::getC_AttSipUri(void){
    if(!parsed)
        doParse();

    return reqUri;
}
S_AttSipVersion C_HeadSipRequest::getS_AttSipVersion(void){
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
C_HeadSipReply::C_HeadSipReply(string _content, int genEntity)
    : S_HeadGeneric(_content, genEntity),
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
S_AttReply C_HeadSipReply::getReply(void) {

    if(!parsed) {
        doParse();
    }
    return reply;
}
S_AttSipVersion C_HeadSipReply::getSipVersion(void) {

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

