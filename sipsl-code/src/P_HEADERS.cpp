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
#ifndef CS_HEADERS 
#include "CS_HEADERS.h"
#endif
#ifndef P_HEADERS 
#include "P_HEADERS.h"
#endif

#ifndef UTIL
#include "UTIL.h"
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// P_HeadSipRequest
// P_HeadSipRequest "Method RequestURI SipVersion<CRLF>"
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void P_HeadSipRequest::doParse(void) {

    if(parsed)
        return;

    vector<string> elements = brkSpaces(content);
    
    //AttMethod
    vector<string>::iterator iter;
    iter = elements.begin();
    method = new S_AttMethod(*iter);
    //method.setContent(*iter);

    iter ++;
    reqUri = new P_AttSipUri(*iter);
    //reqUri.setContent(*iter);

    iter++;
    sipvs = new S_AttSipVersion(*iter);
    //sipvs.setContent(*iter);
}
S_AttMethod *P_HeadSipRequest::getMethod(void){
    if(!parsed)
        doParse();
    return method;
}
P_AttSipUri *P_HeadSipRequest::getAttSipUri(void){
    if(!parsed)
        doParse();
    return reqUri;
}
S_AttSipVersion *P_HeadSipRequest::getSipVs(void){
    if(!parsed)
        doParse();
    return sipvs;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// P_AttSipUri
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
P_AttSipUri::P_AttSipUri(string content) 
    : S_AttGeneric(content) {
}
/*
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// AttMethod
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AttMethod::doParse(void){

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
int AttMethod::getMethodID(void) {

    if (!parsed) {
        doParse();
    }
    return methodID;
}
string AttMethod::getMethodName(void) {
    
    if (!parsed){
        doParse();
    }
    return methodName;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// AttReply
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AttReply::doParse(void) {

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
    reply = *ii;

    if (reply.compare("OK") == 0){
        replyID = OK_RESPONSE;
        parsed = true;
        correct = true;
        return;
    }
    if (content.compare("RINGING") == 0){
        replyID = RINGING_RESPONSE;
        parsed = true;
        correct = true;
        return;
    }
    if (content.compare("TRY") == 0){
        replyID = TRY_RESPONSE;
        parsed = true;
        correct = true;
        return;
    }
    parsed = true;
    correct = false;
    return;
}
int AttReply::getCode(void){

   if(!parsed) {
        doParse();
   }
   return code;
}
int AttReply::getReplyID(void){

   if(!parsed) {
        doParse();
   }
   return replyID;
}
string AttReply::getReply(void){

   if(!parsed) {
        doParse();
   }
   return reply;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// AttSipVersion
// Fake
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AttSipVersion::doParse(void){

    if(parsed){
        return;
    }
    //TODO
    parsed = true;
}
string AttSipVersion::getProtocol(void) {
    //TODO
    return("SIP");
}
int AttSipVersion::getVersion(void){
    //TODO
    return(2);
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// P_HeadSipReply
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void P_HeadSipReply::doParse(void){

    if(parsed){
        return;
    }

    vector<string> elements = brkSpaces(content);
    
    vector<string>::iterator iter;
    iter = elements.begin();
    sipvs.setContent(*iter);

    iter ++;
    //TODO AttReply will re-parse it...
    reply.setContent(*iter + " " +*(++iter));

    parsed = true;
    //TODO
    correct = true;

    return;
}
AttReply P_HeadSipReply::getReply(void) {

    if(!parsed) {
        doParse();
    }
    return reply;
}
AttSipVersion P_HeadSipReply::getSipVersion(void) {

    if(!parsed) {
        doParse();
    }
    return sipvs;
}

*/
