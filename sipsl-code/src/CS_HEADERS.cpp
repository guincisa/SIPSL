//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPCSL Sip Core And Service Layer
// Copyright (C) 2011 Guglielmo Incisa di Camerana
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

#include <algorithm>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <iostream>
#include <map>
#include <math.h>
#include <memory>
#include <pthread.h>
#include <signal.h>
#include <sstream>
#include <stack>
#include <stdio.h>
#include <stdlib.h>     /* for atoi() and exit() */
#include <string>
#include <string.h>
#include <sys/socket.h> /* for socket() and bind() */
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <vector>
#include <unordered_map>

#include "UTIL.h"
#ifndef CS_HEADERS_H
#include "CS_HEADERS.h"
#endif
#ifndef MESSAGE_H
#include "MESSAGE.h"
#endif
#ifndef SPIN_H
#include "SPIN.h"
#endif
#ifndef ENGINE_H
#include "ENGINE.h"
#endif
#ifndef SIPENGINE_H
#include "SIPENGINE.h"
#endif
#ifndef SL_CC_H
#include "SL_CC.h"
#endif
#ifndef ACTION_H
#include "ACTION.h"
#endif
#ifndef DOA_H
#include "DOA.h"
#endif
#ifndef SUDP_H
#include "SUDP.h"
#endif
#ifndef CALL_OSET_H
#include "CALL_OSET.h"
#endif
#ifndef COMAP_H
#include "COMAP.h"
#endif
#ifndef ALO_H
#include "ALO.h"
#endif
#ifndef VALO_H
#include "VALO.h"
#endif
#ifndef SIP_PROPERTIES_H
#include "SIP_PROPERTIES.h"
#endif
#ifndef ALARM_H
#include "ALARM.h"
#endif
#ifndef SIPUTIL_H
#include "SIPUTIL.h"
#endif
// *********************************************************************************
// *********************************************************************************
// S_HeadGeneric
// *********************************************************************************
// *********************************************************************************
S_HeadGeneric::S_HeadGeneric(string buffer) {

    content = buffer;
    contentReady = true;
    parsed = false;
    correct = true;

}
S_HeadGeneric::S_HeadGeneric(void) {
	   DEBASSERT("S_HeadGeneric empty constructor");
}
S_HeadGeneric::S_HeadGeneric(const S_HeadGeneric& x) {

    DEBASSERT("S_HeadGeneric copy constructor");
    content = x.content;
    contentReady = true;
    parsed = false;
    correct = true;

}
void S_HeadGeneric::setContent(string _content) {

    content = _content;
    parsed = false;
    correct = true;
    contentReady = true;
    return;

}
void S_HeadGeneric::setContent(char* _content) {

    content = _content;
    parsed = false;
    correct = true;
    contentReady = true;
    return;

}

string &S_HeadGeneric::getContent(void) {
	if (contentReady){
		return content;
	}
	else{
		buildContent();
	}
	return content;
}
string S_HeadGeneric::copyContent(void) {
	if (contentReady){
		return content;
	}else{
		buildContent();
	}
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
S_AttGeneric::S_AttGeneric(const S_AttGeneric& x){

	DEBASSERT("S_AttGeneric copy constructor");

    content = x.content;
    parsed = false;
    correct = true;
    isSet = false;
    contentReady = true;

}
S_AttGeneric::S_AttGeneric(string _content) {
   content = _content;
   parsed = false;
   correct = true;
   isSet = false;
   contentReady = true;
}
S_AttGeneric::S_AttGeneric(void) {
   DEBASSERT("S_AttGeneric empty constructor")
}
string &S_AttGeneric::getContent(void) {
	if (contentReady){
		return content;
	}
	else {
		buildContent();
		return content;
	}

}
string S_AttGeneric::copyContent(void) {
	if (contentReady){
		return content;
	}
	else {
		buildContent();
		return content;
	}
}
bool S_AttGeneric::isParsed(void) {
    return parsed;
}
bool S_AttGeneric::isCorrect(void) {
    return correct;
}
void S_AttGeneric::setContent(string _content){
	contentReady = true;
	content = _content;
	parsed = false;
	correct = true;
}
// *********************************************************************************
// *********************************************************************************
// TupleVector
// *********************************************************************************
// *********************************************************************************
TupleVector::TupleVector(void) {
	DEBASSERT("TupleVector::TupleVector(void)")
}
TupleVector::TupleVector(const TupleVector& _t) {
	DEBASSERT("COPY of TupleVector")
}
TupleVector::TupleVector(string _tuples, string _separator) : S_AttGeneric(_tuples) {
	separator = _separator;
	hasheader = false;
	header = "";
}
TupleVector::TupleVector(string _tuples, string _separator, string _header) : S_AttGeneric(_tuples) {
	separator = _separator;
	header = _header;
	hasheader = true;
}
void TupleVector::setTupleVector(string _tuples, string _separator, string _header){
	setContent(_tuples);
	parsed=false;
	correct = true;
	content = _tuples;
	header = _header;
	hasheader = false;
	separator = _separator;
}
void TupleVector::setTupleVector(string _tuples, string _separator){
	setContent(_tuples);
	parsed=false;
	correct = true;
	content = _tuples;
	header = "";
	hasheader = false;
	separator = _separator;
}
void TupleVector::doParse(void) {

    if (parsed)
        return;
    try {
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
#ifdef DEBCODE
			TupleMap::iterator it;
			it = tuples.find(tt.Lvalue);
			if (it != tuples.end()){
				DEBASSERT("TupleVector::doParse")
			}
#endif
			tuples.insert(make_pair(tt.Lvalue, tt.Rvalue));
		}
		parsed = true;
    }catch(...){
    	correct = false;
    	throw HeaderException("TupleVector malformed [" + content + "]");
    }
}
const string TupleVector::findRvalue(string _Lvalue){

	if (!correct){
		throw HeaderException("TupleVector malformed [" + _Lvalue + "]");
	}
    if (!parsed) {
        doParse();
    }
    try {
		map<string,string>::iterator ii = tuples.find(_Lvalue);
		if (ii == tuples.end()) {
			return "";
		}
		string s = ii->second;
		return(s);
    }
    catch(...){
    	correct = false;
    	throw HeaderException("TupleVector malformed [" + _Lvalue + "]");
    }
}
void TupleVector::replaceRvalue(string _Lvalue, string _Rvalue){

	if(!correct){
		throw HeaderException("TupleVector Malformed [" + _Lvalue + " ** " + _Rvalue + "]");
	}
    if (!parsed) {
        doParse();
    }

    try {
		map<string,string>::iterator ii = tuples.find(_Lvalue);
		if (ii == tuples.end()) {
			return;
		}
		tuples.erase(ii);
		tuples.insert(make_pair(_Lvalue, _Rvalue));
		ii = tuples.find(_Lvalue);

		contentReady = false;

		return;
    }
    catch(...){
    	correct = false;
		throw HeaderException("TupleVector Malformed [" + _Lvalue + " ** " + _Rvalue + "]");
    }
}
void TupleVector::buildContent(void){

    if (contentReady) {
		return;
	}
    DEBSIP("tv content before", content)

    try {
		map<string,string>::iterator theIterator;

		if (hasheader){
			content = header;
		}
		else {
			content = "";
		}
		for( theIterator = tuples.begin(); theIterator != tuples.end(); theIterator++ ) {
			if (!content.empty())
				content = content + separator+ theIterator->first + "=" + theIterator->second;
			else
				content = content + theIterator->first + "=" + theIterator->second;
		}
		contentReady = true;
		parsed = true;
		DEBSIP("tv content", content)
    }
    catch(...){
    	correct = false;
    	throw HeaderException("TupleVector malformed [" + content + "]");
    }
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
S_AttMethod::S_AttMethod(void){
	DEBASSERT("S_AttMethod::S_AttMethod(void)")
}
S_AttMethod::S_AttMethod(const S_AttMethod& x){
	DEBASSERT("S_AttMethod::S_AttMethod(void)")
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
void S_AttMethod::buildContent(void){

    if (contentReady) {
		return;
	}
    else {
    	content = methodName;
    	contentReady = true;
    }
}
void S_AttMethod::setMethodID(int _i) {

    if (!parsed) {
        doParse();
    }

	if ( _i == INVITE_REQUEST){
        methodName = "INVITE";
        methodID = INVITE_REQUEST;
        contentReady = false;
	}
	if ( _i == ACK_REQUEST){
        methodName = "ACK";
        methodID = ACK_REQUEST;
        contentReady = false;
	}
	if ( _i == BYE_REQUEST){
        methodName = "BYE";
        methodID = BYE_REQUEST;
        contentReady = false;
	}
	if ( _i == CANCEL_REQUEST){
        methodName = "CANCEL";
        methodID = CANCEL_REQUEST;
        contentReady = false;
	}
}
void S_AttMethod::setMethodName(string _method) {

    if (!parsed) {
        doParse();
    }

    if (_method.compare("INVITE") == 0){
        methodID = INVITE_REQUEST;
        contentReady = false;
    }
    if (_method.compare("ACK") == 0){
        methodID = ACK_REQUEST;
        contentReady = false;
    }
    if (_method.compare("BYE") == 0){
        methodID = BYE_REQUEST;
        contentReady = false;
    }
    if (_method.compare("CANCEL") == 0){
        methodID = CANCEL_REQUEST;
        contentReady = false;
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// S_AttReply
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
S_AttReply::S_AttReply(){
	DEBASSERT("S_AttReply()")
}
S_AttReply::S_AttReply(const S_AttReply& x){
	DEBASSERT("S_AttReply(const S_AttReply& x)")
}

S_AttReply::S_AttReply(string _content)
    :S_AttGeneric(_content){

    code = 0;
    replyID = 0;
    reply = "";
}
S_AttReply::S_AttReply(string _replyID, string _code)
    :S_AttGeneric(_code + " " +_replyID){

	code = 0;
	replyID = 0;
	reply = "";

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
void S_AttReply::buildContent(void) {

    if (contentReady) {
		return;
	}
    else {
    	contentReady = true;
    }

}
void S_AttReply::setContent(string _code, string _replyID){

    if (!compare_it(_replyID)) {
        parsed = true;
        correct = false;
        return;
    }
    else {
        code = atoi(_code.c_str());
    }
    contentReady = false;
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
    else if (_reply.compare("Ringing") == 0){
        reply = "Ringing";
        replyID = RINGING_RESPONSE;
        parsed = true;
        correct = true;
        return true;
    }
    else if (_reply.compare("Trying") == 0){
        reply = "Trying";
        replyID = TRY_RESPONSE;
        parsed = true;
        correct = true;
        return true;
    }
    else if (_reply.compare("Dialog") == 0){
        reply = "Dialog";
        replyID = DIAEST_RESPONSE;
        parsed = true;
        correct = true;
        return true;
    }
    else if (_reply.compare("Decline") == 0){
        reply = "Decline";
        replyID = DECLINE_RESPONSE;
        parsed = true;
        correct = true;
        return true;
    }
    else if (_reply.compare("Busy") == 0){
        reply = "Busy";
        replyID = BUSY_HERE;
        parsed = true;
        correct = true;
        return true;
    }
    else if (_reply.compare("Server Unavailable") == 0){
            reply = "Server Unavailable";
            replyID = SU_RESPONSE;
            parsed = true;
            correct = true;
            return true;
    }
    else {
    	return false;
    }

}
int S_AttReply::getCode(void){

   if(!parsed) {
        doParse();
   }
   return code;
}
void S_AttReply::setCode(int _code){

	if (!parsed)
		doParse();
	code = _code;
    contentReady = false;
}
int S_AttReply::getReplyID(void){

   if(!parsed) {
        doParse();
   }
   return replyID;
}
void S_AttReply::setReplyID(int _replyID){

	if (!parsed)
		doParse();

    if (_replyID == OK_RESPONSE){
        reply = "OK";
        replyID = OK_RESPONSE;
        contentReady = false;
    }
    if (_replyID == RINGING_RESPONSE){
        reply = "RINGING";
        replyID = RINGING_RESPONSE;
        contentReady = false;
    }
    if (_replyID == TRY_RESPONSE){
        reply = "TRY";
        replyID = TRY_RESPONSE;
        contentReady = false;
    }
    if (_replyID == SU_RESPONSE){
        reply = "SU";
        replyID = SU_RESPONSE;
        contentReady = false;
    }

}
string &S_AttReply::getReply(void){

   if(!parsed) {
        doParse();
   }
   return reply;
}
void S_AttReply::setReply(string _reply){

	DEBASSERT("S_AttReply::setReply method needs to be rewritten")
	   if(!parsed) {
	        doParse();
	   }

    if (_reply.compare("OK") == 0){
        reply = "OK";
        replyID = OK_RESPONSE;
        parsed = true;
        correct = true;
        contentReady = false;
    }
    if (_reply.compare("RINGING") == 0){
        reply = "RINGING";
        replyID = RINGING_RESPONSE;
        parsed = true;
        correct = true;
        contentReady = false;
    }
    if (_reply.compare("TRY") == 0){
        reply = "TRY";
        replyID = TRY_RESPONSE;
        parsed = true;
        correct = true;
        contentReady = false;
    }
    if (_reply.compare("SU") == 0){
        reply = "SU";
        replyID = SU_RESPONSE;
        parsed = true;
        correct = true;
        contentReady = false;
    }

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
S_AttSipVersion::S_AttSipVersion(){
	DEBASSERT("S_AttSipVersion::S_AttSipVersion()")
}

S_AttSipVersion::S_AttSipVersion(const S_AttSipVersion& x){
	DEBASSERT("S_AttSipVersion::S_AttSipVersion()")
}
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
void S_AttSipVersion::buildContent(void){
    if (contentReady) {
		return;
	}
    else {
    	content = protocol + "/" + version;
    	contentReady = true;
    }
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
void S_AttSipVersion::setProtocol(string _protocol) {
    if(!parsed){
        doParse();
    }
    contentReady = false;
    protocol = _protocol;
    return;
}
void S_AttSipVersion::setVersion(string _version){
    if(!parsed){
        doParse();
    }
    contentReady = false;
    version = _version;
    return;
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
	DEBASSERT("S_AttUserInfo::S_AttUserInfo(const S_AttUserInfo& x)");
}
S_AttUserInfo::S_AttUserInfo(){
	DEBASSERT("S_AttUserInfo::S_AttUserInfo()");
}
void S_AttUserInfo::buildContent(void){

    if (contentReady) {
		return;
	}
    else {
    	content = userName+":"+password;
    	contentReady = true;
    }
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
void S_AttUserInfo::setUserName(string _userName){

    if (!parsed) {
        doParse();
    }

	userName = _userName;
    contentReady = false;
}
void S_AttUserInfo::setPassword(string _password){

    if (!parsed) {
        doParse();
    }

	password = _password;
    contentReady = false;
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
S_AttHostPort::S_AttHostPort(){
	DEBASSERT("S_AttHostPort()")
}
S_AttHostPort::S_AttHostPort(const S_AttHostPort& x){
	DEBASSERT("S_AttHostPort(const S_AttHostPort& )")
}

void S_AttHostPort::doParse(void){

	if(!correct){
		throw HeaderException("HostPort malformed [" + content + "]");
	}
    if (parsed) {
        return;
    }
    try {
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
    catch(...){
    	correct = false;
    	throw HeaderException("HostPort malformed [" + content + "]");
    }
}
void S_AttHostPort::buildContent(void){

    if (contentReady) {
		return;
	}
    else {
    	ostringstream s1;
    	s1 << hostName << ":" << port;
    	content = s1.str();
    	contentReady = true;
    }
}
void S_AttHostPort::setPort(int _port){

	if(!correct){
		throw HeaderException("HostPort malformed getPort");
	}
    if (!parsed)
        doParse();
    contentReady = false;
	port = _port;
}
void S_AttHostPort::setHostName(string _hostName){
	if(!correct){
		throw HeaderException("HostPort malformed [" + _hostName + "]");
	}
    if (!parsed)
        doParse();
    contentReady = false;
    hostName = _hostName;
}
string &S_AttHostPort::getHostName(void){
	if(!correct){
		throw HeaderException("HostPort malformed [" + content + "]");
	}
    if (!parsed)
        doParse();
    return hostName;
}
string S_AttHostPort::copyHostName(void){
	if(!correct){
		throw HeaderException("HostPort malformed [" + hostName + "]");
	}
    if (!parsed)
        doParse();
    return hostName;
}
int S_AttHostPort::getPort(void){
	if(!correct){
		throw HeaderException( "HostPort malformed getPort");
	}
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
    tuples.setTupleVector(content,";");
    parsed = true;
    return;
}
void C_AttUriParms::buildContent(void){

    if (contentReady) {
		return;
	}
    else {
    	tuples.getContent();
    	contentReady = true;
    }
}
TupleVector &C_AttUriParms::getTuples(void){
	if (!parsed)
		doParse();
    return tuples;
}
TupleVector &C_AttUriParms::getChangeTuples(void){
	contentReady = false;
	if (!parsed)
		doParse();
    return tuples;
}
TupleVector C_AttUriParms::copyTuples(void){
	if (!parsed)
		doParse();
    return tuples;
}
C_AttUriParms::C_AttUriParms(const C_AttUriParms& _p) {
	DEBASSERT("C_AttUriParms(const C_AttUriParms& _p)")
}
C_AttUriParms::C_AttUriParms(){
	DEBASSERT("C_AttUriParms()")
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
C_AttUriHeaders::C_AttUriHeaders(){
	DEBASSERT("C_AttUriHeaders()")
}
C_AttUriHeaders::C_AttUriHeaders(const C_AttUriHeaders& x){
	DEBASSERT("C_AttUriHeaders(const C_AttUriHeaders& x)")
}
void C_AttUriHeaders::doParse(void){
    return;
}
void C_AttUriHeaders::buildContent(void){

//    if (contentReady) {
//		return;
//	}
//    else {
    	tuples.getContent();
    	contentReady = true;
//    }
}
TupleVector &C_AttUriHeaders::getChangeTuples(void){
	contentReady = false;
    return tuples;
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
	DEBASSERT("C_AttSipUri(const C_AttSipUri& x)")
}
C_AttSipUri::C_AttSipUri(){
	DEBASSERT("C_AttSipUri()")
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
void C_AttSipUri::buildContent(void){

    if (contentReady) {
		return;
	}
    else {
    	//sip:alice:secretword@atlanta.com;transport=tcp
        //isSecure; //sip or sips
    	ostringstream s1;
    	s1 << "sip:" << userInfo.getContent() << "@" << hostPort.getContent() << ";" << uriHeads.getContent();
    	content = s1.str();
    	contentReady = true;
    }
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
S_AttUserInfo &C_AttSipUri::getChangeS_AttUserInfo(void){
    if (!parsed)
        doParse();
    contentReady = false;
    return userInfo;
}
S_AttHostPort &C_AttSipUri::getS_AttHostPort(void){
    if (!parsed)
        doParse();
    return hostPort;
}
S_AttHostPort &C_AttSipUri::getChangeS_AttHostPort(void){
    if (!parsed)
        doParse();
    contentReady = false;
    return hostPort;
}
C_AttUriParms &C_AttSipUri::getC_AttUriParms(void){
    if (!parsed)
        doParse();
    return uriParms;
}
C_AttUriParms &C_AttSipUri::getChangeC_AttUriParms(void){
    if (!parsed)
        doParse();
    contentReady = false;
    return uriParms;
}
C_AttUriHeaders &C_AttSipUri::getC_AttUriHeads(void){
    if (!parsed)
        doParse();
    return uriHeads;
}
C_AttUriHeaders &C_AttSipUri::getChangeC_AttUriHeads(void){
    if (!parsed)
        doParse();
    contentReady = false;
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
C_AttVia::C_AttVia(const C_AttVia& x){
	DEBASSERT("C_AttVia(const C_AttVia& x)")
}
C_AttVia::C_AttVia(){
	DEBASSERT("C_AttVia()")
}

void C_AttVia::doParse(void){

    if(parsed)
        return;

    //SIP/2.0/UDP 127.0.0.1:5062;rport;branch=z9hG4bKldbblwtj

    DEBSIP("C_AttVia content", content)
    Tuple s1 = brkin2(content, " ");
    DEBSIP("C_AttVia first brkin2",s1.Lvalue << "][" <<  s1.Rvalue)

    Tuple s2 = brkin2(s1.Lvalue, "/");
    DEBSIP("C_AttVia second brkin2",s2.Lvalue << "][" <<  s2.Rvalue)


    Tuple s3 = brkin2(s2.Rvalue, "/");
    DEBSIP("C_AttVia third brkin2",s3.Lvalue << "][" <<  s3.Rvalue)


    transport = s3.Rvalue;
    version.setVersion(s3.Lvalue);
    version.setProtocol(s2.Lvalue);
    //fix the contentReady
    version.getContent();


    Tuple s4 = brkin2(s1.Rvalue, ";");
    hostPort.setContent(s4.Lvalue);
    viaParms.setContent(s4.Rvalue);

    correct = true;
    parsed = true;

    return;
}
void C_AttVia::buildContent(void){

    if (contentReady) {
		return;
	}
    else {
    	//   Via: SIP/2.0/TCP client.atlanta.example.com:5060;branch=z9hG4bK74b76;received=192.0.2.101
//        S_AttSipVersion version;
//        string transport;
//        S_AttHostPort hostPort;
//        TupleVector viaParms;

    	ostringstream s1;
    	DEBSIP("version.getContent", version.getContent())
    	s1 << version.getContent() << "/" << transport << " " << hostPort.getContent() << ";" << viaParms.getContent();
    	content = s1.str();
    	contentReady = true;
    }
}
S_AttSipVersion &C_AttVia::getS_AttSipVersion(void) {
    if (!parsed)
        doParse();
    return version;
}
S_AttSipVersion &C_AttVia::getChangeS_AttSipVersion(void) {
    if (!parsed)
        doParse();
    contentReady = false;
    return version;
}
string &C_AttVia::getTransport(void) {
    if (!parsed)
        doParse();
    return transport;
}
void C_AttVia::setTransport(string _transport) {
    if (!parsed)
        doParse();
    contentReady = false;
    transport = _transport;
}
S_AttHostPort &C_AttVia::getS_HostHostPort(void) {
    if (!parsed)
        doParse();
    return hostPort;
}
S_AttHostPort &C_AttVia::getChangeS_HostHostPort(void) {
    if (!parsed)
        doParse();
    contentReady = false;
    return hostPort;
}
TupleVector &C_AttVia::getViaParms(void){
    if (!parsed)
        doParse();
    return viaParms;
}
TupleVector &C_AttVia::getChangeViaParms(void) {
    if (!parsed)
        doParse();
    contentReady = false;
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
C_AttContactElem::C_AttContactElem(){
	DEBASSERT("C_AttContactElem::C_AttContactElem")
}
C_AttContactElem::C_AttContactElem(const C_AttContactElem& x):
    S_AttGeneric(x.content),
    sipUri(""),
    uriParms(""){
	DEBASSERT("C_AttContactElem::C_AttContactElem")
}
void C_AttContactElem::doParse(void){

    if(parsed)
        return;

    string line;
    DEBSIP("content",content)
    int q = hasQuote(content);
    if (q >=0){
    	string line1 = replaceHttpChars(content.substr(q+1, -1));

		line = trimUseless(content.substr(0,q+1) + line1);

    }
    else {
    	line = trimUseless(content);
    }
    DEBSIP("line after",line)

    Tuple s1 = brkin2(line, "<");
    nameUri = s1.Lvalue; // "Mr. Watson"
    string tmp = "<" + s1.Rvalue;
    DEBSIP("rvalue", tmp)

    Tuple s2 = brkin2(tmp, ";");
    DEBSIP("rvalue2", s2.Lvalue)
    DEBSIP("rvalue3", s2.Rvalue)
    sipUri.setContent(s2.Lvalue);
    uriParms.setContent(s2.Rvalue);

    parsed = true;
    return;
}
//    "Mr. Watson" <sip:watson@worcester.bell-telephone.com>
//       ;q=0.7; expires=3600
void C_AttContactElem::buildContent(void){

    if (contentReady) {
		return;
	}
    else {
    	ostringstream s1;
    	s1 << "\"" << nameUri << "\" " << "<sip:" << sipUri.getContent() << ">;" << uriParms.getContent();
    	content = s1.str();
    	contentReady = true;
    }
}
string &C_AttContactElem::getNameUri(void){

    if(!parsed)
        doParse();

    return nameUri;
}
void C_AttContactElem::setNameUri(string _name){

    if(!parsed)
        doParse();

	nameUri = _name;
	contentReady = false;

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
C_AttSipUri &C_AttContactElem::getChangeC_AttSipUri(void){

    if(!parsed)
        doParse();
    contentReady = false;
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
C_AttUriParms &C_AttContactElem::getChangeC_AttUriParms(void){

    if(!parsed)
        doParse();
    contentReady = false;
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
C_AttContactList::C_AttContactList(){
	DEBASSERT("C_AttContactList()")
}
C_AttContactList::C_AttContactList(const C_AttContactList& x){
	DEBASSERT("C_AttContactList(const C_AttContactList&)")
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
void C_AttContactList::buildContent(void){

    if (contentReady) {
		return;
	}
    else {
    	//TODO
    	contentReady = true;
    	DEBASSERT("void C_AttContactList::buildContent(void)")
    }
}
vector<C_AttContactElem> &C_AttContactList::getContactList(void){

    if (!parsed)
        doParse();

    return contactList;
}
vector<C_AttContactElem> &C_AttContactList::getChangeContactList(void){

    if (!parsed)
        doParse();
    contentReady = false;
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
C_HeadSipRequest::C_HeadSipRequest(string _content)
    : S_HeadGeneric(_content),
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
    if (iter != elements.end()){
    	reqUri.setContent(*iter);
        iter++;
    } else {
    	reqUri.setContent("");
    }
    if (iter != elements.end()){
    	sipvs.setContent(*iter);
    }else {
    	sipvs.setContent("");
    }
    correct = true;
    parsed = true;
}
void C_HeadSipRequest::buildContent(void){

    if (contentReady) {
		return;
	}
    else {
    	//TODO
    	contentReady = true;
    	DEBASSERT("void C_HeadSipRequest::buildContent(void)")
    }
}
S_AttMethod &C_HeadSipRequest::getS_AttMethod(void){
    if(!parsed)
        doParse();
    return method;
}
S_AttMethod &C_HeadSipRequest::getChangeS_AttMethod(void){
    if(!parsed)
        doParse();
    contentReady = false;
    return method;
}
C_AttSipUri &C_HeadSipRequest::getC_AttSipUri(void){
    if(!parsed)
        doParse();

    return reqUri;
}
C_AttSipUri &C_HeadSipRequest::getChangeC_AttSipUri(void){
    if(!parsed)
        doParse();
    contentReady = false;
    return reqUri;
}
S_AttSipVersion &C_HeadSipRequest::getS_AttSipVersion(void){
    if(!parsed)
        doParse();
    return sipvs;
}
S_AttSipVersion &C_HeadSipRequest::getChangeS_AttSipVersion(void){
    if(!parsed)
        doParse();
    contentReady = false;
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
C_HeadSipReply::C_HeadSipReply(string _content)
    : S_HeadGeneric(_content),
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
void C_HeadSipReply::buildContent(void){

    if (contentReady) {
		return;
	}
    else {
    	//TODO
    	contentReady = true;
    	DEBASSERT("void C_HeadSipReply::buildContent(void)")
    }
}
S_AttReply &C_HeadSipReply::getReply(void) {

    if(!parsed) {
        doParse();
    }
    return reply;
}
S_AttReply &C_HeadSipReply::getChangeReply(void) {

    if(!parsed) {
        doParse();
    }
    contentReady = false;
    return reply;
}
S_AttSipVersion &C_HeadSipReply::getSipVersion(void) {

    if(!parsed) {
        doParse();
    }
    return sipvs;
}
S_AttSipVersion &C_HeadSipReply::getChangeSipVersion(void) {

    if(!parsed) {
        doParse();
    }
    contentReady = false;
    return sipvs;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// HeadVia
// Via: xxxxx
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
C_HeadVia::C_HeadVia(string _content) :
    S_HeadGeneric(_content),
    via("") {
}
C_HeadVia::C_HeadVia(const C_HeadVia& x):via(""){
	DEBASSERT("C_HeadVia::C_HeadVia copy constructor")
}

void C_HeadVia::doParse(void) {

	DEBSIP("C_HeadVia::doParse", content)

    if(parsed)
        return;

//    Tuple s1 = brkin2(content, " ");
//    string s_tmp;
//    if (s1.Rvalue.length()==0)
//    	s_tmp = s1.Lvalue;
//    else
//    	s_tmp = s1.Rvalue;

    via.setContent(content);

    parsed = true;
}
C_AttVia &C_HeadVia::getC_AttVia(void){

    if (!parsed)
        doParse();

    return via;
}
C_AttVia &C_HeadVia::getChangeC_AttVia(void) {

    if (!parsed)
        doParse();
    contentReady = false;
    return via;
}
void C_HeadVia::buildContent(void){

//	via.getChangeViaParms();
//	via.getChangeS_AttSipVersion();
//	via.getChangeS_HostHostPort();
//
//    content = "Via: " + "SIP/" + via.S_AttSipVersion().getContent() + "/" + via.getTransport() + " " + via.getS_HostHostPort().getContent() +
//    		";" + via.getViaParms().getContent();

    DEBSIP("C_HeadVia::buildContent", content);

	if (contentReady) {
		return;
	}
    else {
    	//TODO
    	contentReady = true;
    	via.getContent();
    	DEBSIP("void C_HeadVia::buildContent(void)", via.getContent())
    }

}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// S_HeadMaxFwd
// Max-Forwards: 70
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
S_HeadMaxFwd::S_HeadMaxFwd(string _content) :
    S_HeadGeneric(_content){

    mxfwd = 0;
}
void S_HeadMaxFwd::doParse(void) {

    if(parsed)
        return;

    Tuple s1 = brkin2(content, " ");
    mxfwd = atoi(s1.Rvalue.c_str());

    parsed = true;
}
void S_HeadMaxFwd::buildContent(void){

	if (contentReady) {
		return;
	}
    else {
    	//TODO
    	contentReady = true;
    	DEBASSERT("void S_HeadMaxFwd::buildContent(void)")
    }
}
int S_HeadMaxFwd::getMaxFwd(void){
    if (!parsed)
        doParse();
    return mxfwd;
}
void S_HeadMaxFwd::setMaxFwd(int _mxfwd){
    mxfwd = _mxfwd;
    contentReady = false;
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
C_HeadContact::C_HeadContact(string _content) :
    S_HeadGeneric(_content),
    contactList(_content){

	star = false;
}
void C_HeadContact::doParse(void){

	if (parsed)
		return;

	//TODO if *

	parsed = true;

}
void C_HeadContact::buildContent(void){

	if (contentReady) {
		return;
	}
    else {
    	//TODO
    	contentReady = true;
    	DEBASSERT("void C_HeadContact::buildContent(void)")
    }
}
C_AttContactList &C_HeadContact::getContactList(void){
	if (!parsed)
		doParse();
	return contactList;
}
C_AttContactList &C_HeadContact::getChangeContactList(void){
	if (!parsed)
		doParse();
	contentReady = false;
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
//C_HeadTo::C_HeadTo(string _content) :
//    S_HeadGeneric(_content),
//    to(_content){
//}
//void C_HeadTo::doParse(void){
//
//	if (parsed)
//		return;
//
//	parsed = true;
//
//}
//void C_HeadTo::buildContent(void){
//
//	if (contentReady) {
//		return;
//	}
//    else {
//    	//TODO
//    	contentReady = true;
//    	DEBASSERT("void C_HeadTo::buildContent(void)")
//    }
//}
//C_AttContactElem &C_HeadTo::getTo(void){
//	if (!parsed)
//		doParse();
//	return to;
//}
//C_AttContactElem &C_HeadTo::getChangeTo(void){
//	if (!parsed)
//		doParse();
//	contentReady = true;
//	return to;
//}
//C_AttContactElem C_HeadTo::copyTo(void){
//	if (!parsed)
//		doParse();
//	return to;
//}
C_HeadTo::C_HeadTo(string _content) :
    S_AttGeneric(_content),
    sipUri(""),
    uriParms(""){
}
void C_HeadTo::doParse(void){

    if(parsed)
        return;

    DEBSIP("C_HeadTo::doParse",content)

    string newCo = content;
    string line;
    int q = hasQuote(newCo);
    if (q >=0){
    	string line1 = replaceHttpChars(newCo.substr(q+1, -1));

		line = trimUseless(newCo.substr(0,q+1) + line1);

    }
    else {
    	line = trimUseless(newCo);
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
void C_HeadTo::buildContent(void){

	if (contentReady) {
		return;
	}
    else {
    	//TODO
    	contentReady = true;
    	DEBASSERT("void C_HeadFrom::buildContent(void)")
    }
}
string &C_HeadTo::getNameUri(void){

    if(!parsed)
        doParse();

    return nameUri;
}
void C_HeadTo::setNameUri(string _nameUri){

    if(!parsed)
        doParse();
    nameUri = _nameUri;
    contentReady = false;
    return;
}

string C_HeadTo::copyNameUri(void){

    if(!parsed)
        doParse();

    return nameUri;
}

C_AttSipUri &C_HeadTo::getC_AttSipUri(void){

    if(!parsed)
        doParse();

    return sipUri;
}
C_AttSipUri &C_HeadTo::getChangeC_AttSipUri(void){

    if(!parsed)
        doParse();
    contentReady = false;
    return sipUri;
}
C_AttSipUri C_HeadTo::copyC_AttSipUri(void){

    if(!parsed)
        doParse();

    return sipUri;
}
C_AttUriParms &C_HeadTo::getC_AttUriParms(void){

    if(!parsed)
        doParse();

    return uriParms;
}
C_AttUriParms &C_HeadTo::getChangeC_AttUriParms(void){

    if(!parsed)
        doParse();
    contentReady = false;
    return uriParms;
}
C_AttUriParms C_HeadTo::copyC_AttUriParms(void){

    if(!parsed)
        doParse();

    return uriParms;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// C_HeadFrom
//  From: Bob <sip:bob@biloxi.example.com>;tag=8321234356
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
C_HeadFrom::C_HeadFrom(string _content) :
    S_AttGeneric(_content),
    sipUri(""),
    uriParms(""){
}
//C_HeadFrom::C_HeadFrom(const C_AttContactElem& x):
//    S_AttGeneric(x.content),
//    sipUri(""),
//    uriParms(""){
//    DEBSIP("C_AttContactElem copy constructor", x.content)
//
//    //content = x.copyContent();
//    parsed = false;
//}
void C_HeadFrom::doParse(void){

    DEBSIP("C_HeadFrom::doParse", content)

    if(parsed)
        return;

	Tuple s = brkin2(content, " ");
	string newCo;

    string line;

	if (s.Rvalue.length() == 0) {
		newCo = s.Lvalue;
	}
	else {
		newCo = s.Rvalue;
		line = s.Lvalue;
	}

	//DEBSIP("C_HeadFrom::doParse newCo", newCo)
	int q = hasQuote(line);
	if( line.length() > 0 && q > 0){
    	string line1 = replaceHttpChars(line.substr(q+1, -1));
    	nameUri = trimUseless(line.substr(0,q+1) + line1);
    }
    else {
    	nameUri = trimUseless(line);
    }

    //DEBSIP("C_HeadFrom::doParse nameUri", nameUri)

    Tuple s2 = brkin2(newCo, ">");

    //DEBSIP("C_HeadFrom::doParse s2.Lvalue plus >", s2.Lvalue+ ">")
    //DEBSIP("C_HeadFrom::doParse s2.Rvalue", s2.Rvalue )

    sipUri.setContent(s2.Lvalue + ">");
    uriParms.setContent(s2.Rvalue);

    parsed = true;
    return;
}
void C_HeadFrom::buildContent(void){

	if (contentReady) {
		return;
	}
    else {
    	//TODO
    	contentReady = true;
    	DEBASSERT("void C_HeadFrom::buildContent(void)")
    }
}
string &C_HeadFrom::getNameUri(void){

    if(!parsed)
        doParse();

    string nameUrit = cleanHttpChars(nameUri);
    nameUri = trimUseless(nameUrit);

    return nameUri;
}
void C_HeadFrom::setNameUri(string _nameUri){

    if(!parsed)
        doParse();
    nameUri = _nameUri;
    contentReady = false;
    return;
}

string C_HeadFrom::copyNameUri(void){

    if(!parsed)
        doParse();

    return nameUri;
}

C_AttSipUri &C_HeadFrom::getC_AttSipUri(void){

    if(!parsed)
        doParse();

    return sipUri;
}
C_AttSipUri &C_HeadFrom::getChangeC_AttSipUri(void){

    if(!parsed)
        doParse();
    contentReady = false;
    return sipUri;
}
C_AttSipUri C_HeadFrom::copyC_AttSipUri(void){

    if(!parsed)
        doParse();

    return sipUri;
}
C_AttUriParms &C_HeadFrom::getC_AttUriParms(void){

    if(!parsed)
        doParse();

    return uriParms;
}
C_AttUriParms &C_HeadFrom::getChangeC_AttUriParms(void){

    if(!parsed)
        doParse();
    contentReady = false;
    return uriParms;
}
C_AttUriParms C_HeadFrom::copyC_AttUriParms(void){

    if(!parsed)
        doParse();

    return uriParms;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// HeadCallID
// 238556723098563298463789@hsfalkgjhaslgh.com
// id1@id2
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
C_HeadCallId::C_HeadCallId(string _content):
	S_HeadGeneric(_content){
}
void C_HeadCallId::doParse(void){

	if (parsed)
		return;

	callId = brkin2(content, "@");

	parsed = true;

	return;
}
void C_HeadCallId::buildContent(void){

	if (contentReady) {
		return;
	}
    else {
    	//TODO
    	contentReady = true;
    	DEBASSERT("void C_HeadCallId::buildContent(void)")
    }
}
Tuple& C_HeadCallId::getCallId(void){
	if (!parsed){
		doParse();
	}
	return callId;
}
Tuple& C_HeadCallId::getChangeCallId(void){
	if (!parsed){
		doParse();
	}
	contentReady = false;
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
C_HeadCSeq::C_HeadCSeq(string _content):
	S_HeadGeneric(_content),
	method(""){
}
void C_HeadCSeq::doParse(void){

	DEBSIP("C_HeadCSeq::doParse", content)

	if(parsed)
		return;

	//Tuple s = brkin2(content, " ");
	Tuple ss = brkin2(content, " ");
	sequence = atoi(ss.Lvalue.c_str());
	method.setContent(ss.Rvalue);

	parsed = true;

	return;
}
void C_HeadCSeq::buildContent(void){

	if (contentReady) {
		return;
	}
    else {
    	//TODO
    	contentReady = true;
    	DEBASSERT("void C_HeadCSeq::buildContent(void)")
    }
}
int C_HeadCSeq::getSequence(void){

	if(!parsed)
		doParse();
	return sequence;
}
void C_HeadCSeq::setSequence(int _seq){

	if(!parsed)
		doParse();
	sequence = _seq;
	contentReady = false;
	return;
}
S_AttMethod &C_HeadCSeq::getMethod(void){

	if(!parsed)
		doParse();
	return method;
}
S_AttMethod &C_HeadCSeq::getChangeMethod(void){

	if(!parsed)
		doParse();
	contentReady = false;
	return method;
}
S_AttMethod C_HeadCSeq::copyMethod(void){

	if(!parsed)
		doParse();
	return method;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Route
//     Route: <sip:127.0.0.1:5060;lr>
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
C_HeadRoute::C_HeadRoute(string _content):
	S_HeadGeneric(_content),
	routeHost(""){
}
void C_HeadRoute::doParse(void){

	if(!correct){
		throw HeaderException("Route malformed");
	}
	if(parsed)
		return;
	try {
		int i = content.find("<");
		int ii = content.find(">");
		//TODO if not < and > ??
		//if (i > 0 && ii > 0){
			string tmp = content.substr(i+5,ii-i-5);
		Tuple s = brkin2(tmp, ";");
		routeHost.setContent(s.Lvalue);
		lr = s.Rvalue;

		parsed = true;

		return;
	}
	catch(...){
		correct = false;
		throw HeaderException("Route malformed");
	}

}
void C_HeadRoute::buildContent(void){

	if (contentReady) {
		return;
	}
    else {
    	//TODO
    	contentReady = true;
    }
}

S_AttHostPort &C_HeadRoute::getRoute(void){
	if(!correct){
		throw HeaderException("Route malformed");
	}
	if(!parsed)
		doParse();
	return routeHost;
}
S_AttHostPort &C_HeadRoute::getChangeRoute(void){
	if(!correct){
		throw HeaderException("Route malformed");
	}
	if(!parsed)
		doParse();
	contentReady = false;
	return routeHost;
}

