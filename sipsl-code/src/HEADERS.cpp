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
#ifndef HEADER 
#include "HEADERS.h"
#endif

#ifndef UTIL
#include "UTIL.h"
#endif

inline vector<string> parse(string par, string head, string sep) {

    int h = 0; // if has head then 1

    vector<string> output;

    if (par.substr(0,1) == head) {
        h = 1;
    }
    int a = par.find(sep.c_str(),0);
    while (a > 0) {
       output.push_back(par.substr(h,a-h));
       par = par.substr(a+1,-1);
       h=0;
       a = par.find(sep.c_str(),0);
    }
    if (a < 0) {
       output.push_back(par.substr(h,a-1));
    }
    return output;
}
inline Tuple getLRvalue(string couple) {

    int a = couple.find("=", 0);
    Tuple tt;
    tt.Lvalue = couple.substr(0, a);
    tt.Rvalue = couple.substr(a+1, -1);
}
inline vector<string> brkSpaces(string s) {

    //separate string into words
    //TODO TOKENIZER????
    vector<string> output;
    int a = 0;
    a = s.find(" ", 0);
    while (a > 0) {
        output.push_back(s.substr(0,a));
        s = s.substr(a+1,-1);
        a = s.find(" ", 0);
    }
    output.push_back(s);
    return output;
} 
// *********************************************************************************
// *********************************************************************************
// HeadGeneric
// *********************************************************************************
// *********************************************************************************
HeadGeneric::HeadGeneric(string buffer, int _genEntity) {

    content = buffer;
    genEntity = _genEntity;

    parsed = false;
    correct = true;
    isSet = true;

}
void HeadGeneric::setContent(string _content, int _genEntity) {

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
string HeadGeneric::getContent(void) {
    return content;
}
bool HeadGeneric::isParsed(void) {
    return parsed;
}
bool HeadGeneric::isCorrect(void) {
    return correct;
}
// *********************************************************************************
// *********************************************************************************
// AttGeneric
// *********************************************************************************
// *********************************************************************************
AttGeneric::AttGeneric(string _content) {
   content = _content;
   parsed = false;
   correct = true;
   isSet = false;
}
AttGeneric::AttGeneric(void) {
   parsed = false;
   correct = true;
   isSet = false;
}
string AttGeneric::getContent(void) {
   return content;
}
bool AttGeneric::isParsed(void) {
    return parsed;
}
bool AttGeneric::isCorrect(void) {
    return correct;
}
void AttGeneric::setContent(string _content){
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
TupleVector::TupleVector(string tuples, string _separator) : AttGeneric(tuples) {

   separator = _separator;
   hasheader = false;
   
}
TupleVector::TupleVector(string tuples, string _separator, string _header) : AttGeneric(tuples) {

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
    map<string,string>::iterator ii = tuples.find(_Lvalue);
    string s = ii->second;
    return(s);
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// HeadSipRequest
// HeadSipRequest "Method RequestURI SipVersion<CRLF>"
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HeadSipRequest::doParse(void) {

    if(parsed)
        return;

    vector<string> elements = brkSpaces(content);
    
    //AttMethod
    vector<string>::iterator iter;
    iter = elements.begin();
    method.setContent(*iter);

    iter ++;
    reqUri.setContent(*iter);

    iter++;
    sipvs.setContent(*iter);
}
AttMethod HeadSipRequest::getMethod(void){
    if(!parsed)
        doParse();
    return method;
}
AttSipUri HeadSipRequest::getAttSipUri(void){
    if(!parsed)
        doParse();
    return reqUri;
}
AttSipVersion HeadSipRequest::getSipVs(void){
    if(!parsed)
        doParse();
    return sipvs;
}
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
// HeadSipReply
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HeadSipReply::doParse(void){

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
AttReply HeadSipReply::getReply(void) {

    if(!parsed) {
        doParse();
    }
    return reply;
}
AttSipVersion HeadSipReply::getSipVersion(void) {

    if(!parsed) {
        doParse();
    }
    return sipvs;
}

