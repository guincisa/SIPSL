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
P_HeadSipRequest::P_HeadSipRequest(string _content, int genEntity) 
    : S_HeadGeneric(_content, genEntity) {

    method = NULL;
    reqUri = NULL;
    sipvs = NULL;
}
void P_HeadSipRequest::doParse(void) {

    if(parsed)
        return;

    vector<string> elements = brkSpaces(content);
    
    //AttMethod
    vector<string>::iterator iter;
    iter = elements.begin();
    method = new S_AttMethod(*iter);

    iter ++;
    reqUri = new P_AttSipUri(*iter);

    iter++;
    sipvs = new S_AttSipVersion(*iter);
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
void P_AttSipUri::doParse(void){return;}
bool P_AttSipUri::getIsSec(void){return false;}
S_AttUserInfo *P_AttSipUri::getS_AttUserInfo(void){return userInfo;}
S_AttHostPort *P_AttSipUri::getS_AttHostPort(void){return hostPort;}
P_AttUriParms *P_AttSipUri::getP_AttUriParms(void){return uriParms;}
P_AttUriHeaders *P_AttSipUri::getP_AttUriHeads(void){return uriHeads;}
P_AttSipUri::P_AttSipUri(string content) 
    : S_AttGeneric(content) {

    userInfo = NULL;
    hostPort = NULL;
    uriParms = NULL;
    uriHeads = NULL;

    return;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// P_HeadSipReply
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
P_HeadSipReply::P_HeadSipReply(string _content, int _genEntity)
    :S_HeadGeneric(_content, genEntity) {

    sipvs = NULL;
    reply = NULL;
}
void P_HeadSipReply::doParse(void){

    if(parsed){
        return;
    }

    vector<string> elements = brkSpaces(content);
    
    vector<string>::iterator iter;
    iter = elements.begin();

    sipvs = new S_AttSipVersion(*iter);

    iter ++;
    reply = new S_AttReply(*iter, *(++iter));

    parsed = true;
    //TODO
    correct = true;

    return;
}
S_AttReply *P_HeadSipReply::getReply(void) {

    if(!parsed) {
        doParse();
    }
    return reply;
}
S_AttSipVersion *P_HeadSipReply::getSipVersion(void) {

    if(!parsed) {
        doParse();
    }
    return sipvs;
}
