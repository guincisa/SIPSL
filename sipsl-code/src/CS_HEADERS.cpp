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

    DEBOUT("Parsing TupleVector",content)

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
    DEBOUT("_Lvalue",_Lvalue)
    map<string,string>::iterator ii = tuples.find(_Lvalue);
    string s = ii->second;
    DEBOUT("s",s)
    return(s);
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// HeadSipRequest
// HeadSipRequest "Method RequestURI SipVersion<CRLF>"
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
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
S_AttReply::S_AttReply(string _replyID, string _code) 
    :S_AttGeneric(_code + " " +_replyID){

    if (!compare_it(_replyID)) {
DEBOUT("","")
        parsed = true;
        correct = false;
        return;
    }
    else {
DEBOUT("","")
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
        replyID = OK_RESPONSE;
        parsed = true;
        correct = true;
        return true;
    }
    if (_reply.compare("RINGING") == 0){
        replyID = RINGING_RESPONSE;
        parsed = true;
        correct = true;
        return true;
    }
    if (_reply.compare("TRY") == 0){
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
// C_HeadSipReply
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void C_HeadSipReply::doParse(void){

    if(parsed){
        return;
    }

    vector<string> elements = brkSpaces(content);
    
    vector<string>::iterator iter;
    iter = elements.begin();
    sipvs.setContent(*iter);

    iter ++;
    //TODO S_AttReply will re-parse it...
    reply.setContent(*iter + " " +*(++iter));

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
// S_AttUserInfo stub
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
S_AttUserInfo::S_AttUserInfo(string _content){assert(0);return;}
void S_AttUserInfo::doParse(void){assert(0);return;}
string S_AttUserInfo::getUserName(void){assert(0);return "EMPTY";}
string S_AttUserInfo::getPassword(void){assert(0);return "EMPTY";}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// S_AttHostPort stub
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
S_AttHostPort::S_AttHostPort(string _content){assert(0);return;}
void S_AttHostPort::doParse(void){assert(0);return;}
string S_AttHostPort::getHostName(void){assert(0);return "EMPTY";}
int S_AttHostPort::getPort(void){assert(0);return -1;}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// C_AttUriParms stub
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
C_AttUriParms::C_AttUriParms(string _content)
    : tuples(_content, "=")
    {assert(0);return;}
void C_AttUriParms::doParse(void){assert(0);return;}
TupleVector C_AttUriParms::getTuples(void){assert(0);return tuples;}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// C_AttUriHeaders stub
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
C_AttUriHeaders::C_AttUriHeaders(string _content)
    : tuples(_content, "&", "?")
    {assert(0);return;}
void C_AttUriHeaders::doParse(void){assert(0);return;}
TupleVector C_AttUriHeaders::getTuples(void){assert(0);return tuples;}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// C_AttSipUri implementing
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

void C_AttSipUri::doParse(void){

    // "sip:alice:secretword@atlanta.com;transport=tcp;ttl=15?to=alice%40atalnta.com&priority=urgent"
    // break ;

    Tuple s1 = brkin2(content, ":");
    // sip
    // alice:secretword@atlanta.com;transport=tcp;ttl=15?to=alice%40atalnta.com&priority=urgent"
    assert(s1.Lvalue.compare("sips"));

    Tuple s2 = brkin2(s1.Rvalue, ";");
    
    assert(0);
    return;
}
bool C_AttSipUri::getIsSec(void){assert(0);return false;}
S_AttUserInfo C_AttSipUri::getS_AttUserInfo(void){assert(0);return userInfo;}
S_AttHostPort C_AttSipUri::getS_AttHostPort(void){assert(0);return hostPort;}
C_AttUriParms C_AttSipUri::getC_AttUriParms(void){assert(0);return uriParms;}
C_AttUriHeaders C_AttSipUri::getC_AttUriHeads(void){assert(0);return uriHeads;}
