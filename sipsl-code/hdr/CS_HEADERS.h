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

#define CS_HEADERS_H

#include <map>
#include <vector>

#ifndef UTIL_H
#include "UTIL.h"
#endif
using namespace std;
#define REGISTER_REQUEST 1
#define INVITE_REQUEST 2
#define ACK_REQUEST 3
#define BYE_REQUEST 4
#define CANCEL_REQUEST 5

#define TRY_RESPONSE 51
#define RINGING_RESPONSE 52
#define OK_RESPONSE 53

#define INTERNALS_METHOD 9000

//#include <pthread.h>
//#include <string>
//#include <iostream>
//#include <memory>
//#include <sys/time.h>
//#include <sys/socket.h>
//#include <arpa/inet.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// tuples
//    a=b
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
typedef struct {
    string Lvalue;
    string Rvalue;
} Tuple;

typedef map<string,string> TupleMap;

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
inline string trimCR(string s){
	int ii = s.find("\r",0);
    if(ii >= 0){
        return(s.substr(0,ii));
    }
    else {
    	return(s);
    }
}
inline string trimUseless(string s) {

    string output = "";
    string final = "";
    int a = 0;
    a = s.find(" ", 0);
    string tmp;
    while (a >= 0) {

        tmp = s.substr(0,a);
        output = output + tmp;
        s = s.substr(a+1,-1);
        a = s.find(" ", 0);
    }
    output = output+s;

    return trimCR(output);
}
inline Tuple brkin2(string couple, string separator) {

    int a = couple.find(separator, 0);
    Tuple tt;
    if (a < 0) {
        tt.Lvalue = couple.substr(0, a);
        tt.Rvalue = "";
        return tt;
    }
    if (a == couple.length()) {
        tt.Lvalue = couple.substr(0, a);
        tt.Rvalue = "";
        return tt;
    }

    tt.Lvalue = couple.substr(0, a);
    tt.Rvalue = couple.substr(a+1, -1);

    return tt;
}
inline int hasQuote(string s) {

    int a = s.find("\"", 0);

    if (a >= 0) {
    	//has a quote
    	return a;
    }
    else
    	return a;
}
inline string replaceHttpChars(string s){

	int a = s.find(" ", 0);
	if (a >= 0){
		Tuple t = brkin2(s," ");
		return t.Lvalue + "%20" + t.Rvalue;
	}
	else
		return s;

}
inline vector<string> parse(string _par, string head, string sep, bool _trimspaces) {

    int h = 0; // if has head then 1

    string par;
    if (_trimspaces)
    	par = trimUseless(_par);
    else
    	par = _par;

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

    return tt;
}


//Att* are attibutes
//Head* are headers
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// GenericHeader "xxx: yyy<CRLF>"
//     Call-ID: 1234@gugli.gugli.it
// Virtual class
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class S_HeadGeneric { //i

    protected:
        bool parsed;
        bool correct;

        // Id of the generating endpoint
        // same as SODE_*
        //int genEntity;

        string content;
        virtual void doParse(void) = 0;

    public:
        bool isCorrect(void);
        bool isParsed(void);

        string &getContent(void);
        string copyContent(void);

        void setContent(string buff);


        // created using buffer and endpoint id which generates header
        S_HeadGeneric(string buff);


        int getGenEntity(void);

};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//S_AttGeneric
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class S_AttGeneric { //i
//NEW REVISION

    protected:
        string content;
        bool correct;
        bool parsed;
        bool isSet;

        virtual void doParse(void) = 0;

    public:
        //TODO non applicabile perche content non è sempre sipinibile
        S_AttGeneric(string content);
        S_AttGeneric(void);

        S_AttGeneric(const S_AttGeneric& x);

        string &getContent(void);
        string copyContent(void);

        void setContent(string);

        bool isCorrect(void);
        bool isParsed(void);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Tuple Vector
// TODO ??? C or S???
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TupleVector : public S_AttGeneric{ //i //t
//NEW REVISON
    private:
        vector<string> lval_rval;
        TupleMap tuples;
        void doParse(void);
        string separator;
        string header;
        bool hasheader;

    public:
        TupleVector(string tuples, string separator, string header);
        TupleVector(string tuples, string separator);
        TupleVector();
        TupleVector(const TupleVector& _t);
        //header can be ? or whatever the string begins with
        string findRvalue(string Lvalue);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// S_AttMethod
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class S_AttMethod : public S_AttGeneric{
//NEW REVISION

    private:
        void doParse(void);
        int methodID;
        string methodName;

    public:
        int getMethodID(void);
        string &getMethodName(void); //i t
        string copyMethodName(void); //i t
        S_AttMethod(string);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//S_AttUserInfo
//    alice:secretword@
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class S_AttUserInfo : public S_AttGeneric{
//NEW REVISION
    private:
        void doParse(void);
        string userName;
        string password;

    public:
        string &getUserName(void);//i t
        string copyUserName(void);//i t
        string &getPassword(void);//i t
        string copyPassword(void);//i t
        S_AttUserInfo(string content);
        S_AttUserInfo(const S_AttUserInfo& x);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//S_AttHostPort
//    gateway.conm:123
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class S_AttHostPort : public S_AttGeneric{
//NEW REVISION

    private:
        void doParse(void);
        string hostName;
        int port;

    public:
        string &getHostName(void);//i t
        string copyHostName(void);//i t
        int getPort(void);
        S_AttHostPort(string content);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//C_AttUriParms
//    transport=tcp;ttl=15;...
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_AttUriParms : public S_AttGeneric{ //i //t
//NEW REVISION
//
    private:
        void doParse(void);

        TupleVector tuples;

    public:
        TupleVector &getTuples(void); // i t
        TupleVector copyTuples(void); // i t
        C_AttUriParms(string content);
        C_AttUriParms(const C_AttUriParms& _p);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//AttUriHeads
//    ?to=alice%40atalnta.com&priority=urgent&...
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_AttUriHeaders : public S_AttGeneric{ //i //t
//NEW REVISION

    private:
        void doParse(void);

        TupleVector tuples;

    public:
        TupleVector &getTuples(void); // i t
        TupleVector copyTuples(void); // i t
        C_AttUriHeaders(string content);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// C_AttSipUri
//     "sip:alice:secretword@atlanta.com;transport=tcp"
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_AttSipUri : public S_AttGeneric{ // sip or sips //i //t
//NEW REVISION 26 12 08

    private:
        void doParse(void);

        bool isSecure; //sip or sips
        S_AttUserInfo userInfo; // alice:secretword@
        S_AttHostPort hostPort; // gateway.conm:123
        C_AttUriParms uriParms; // transport=tcp;ttl=15;...
        C_AttUriHeaders uriHeads; // ?to=alice%40atalnta.com&priority=urgent&...

     public:
        bool getIsSec(void);
        S_AttUserInfo &getS_AttUserInfo(void);
        S_AttUserInfo copyS_AttUserInfo(void); //i t

        S_AttHostPort &getS_AttHostPort(void);
        S_AttHostPort copyS_AttHostPort(void); //i t

        C_AttUriParms &getC_AttUriParms(void);
        C_AttUriParms copyC_AttUriParms(void); //i t

        C_AttUriHeaders &getC_AttUriHeads(void);
        C_AttUriHeaders copyC_AttUriHeads(void); //i t

        C_AttSipUri(string content);
        C_AttSipUri(const C_AttSipUri &x);
        void setContent(string content);

};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// AttRequestURI
// AttRequestURI "SIP-URI" or "SIPS-URI" or absoluteURI
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//class AttRequestUri : public S_AttGeneric{
//NEW REVISION
//
//    public
//        int type; //??
//        C_AttSipUri reqUri;
//}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//S_AttSipVersion
//    SIP/2.0
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class S_AttSipVersion : public S_AttGeneric{ //i //t
//NEW REVISION
    private:
        void doParse(void);
        string version;
        string protocol;
    public:
        string &getProtocol(void);
        string copyProtocol(void);

        string &getVersion();
        string copyVersion();

        S_AttSipVersion(string content);
        S_AttSipVersion(string protocol, string version);

        //to set it already parsed
        void setbContent(string protocol, string version);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//C_AttVia
//   Via: SIP/2.0/TCP client.atlanta.example.com:5060;branch=z9hG4bK74b76;received=192.0.2.101
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_AttVia : public S_AttGeneric { //i //t
//NEW REVISION

    private:
        void doParse(void);
        S_AttSipVersion version;
        string transport;
        S_AttHostPort hostPort;
        TupleVector viaParms;

    public:
        S_AttSipVersion &getS_AttSipVersion(void);
        S_AttSipVersion copyS_AttSipVersion(void);

        string &getTransport(void);
        string copyTransport(void);

        S_AttHostPort &getS_HostHostPort(void);
        S_AttHostPort copyS_HostHostPort(void);

        TupleVector &getViaParms(void);
        TupleVector copyViaParms(void);

        C_AttVia(string content);
} ;
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//S_AttReply
//    200 OK
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class S_AttReply : public S_AttGeneric{ //i //t
//NEW REVISION
    private:
        void doParse(void);
        int code;
        int replyID;
        string reply;
        bool compare_it(string reply);

    public:
        int getCode(void);
        int getReplyID(void);

        string &getReply(void);
        string copyReply(void);

        void setContent(string _code, string reply);

        S_AttReply(string content);
        S_AttReply(int replyID, int code);
        S_AttReply(string replyID, string code);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// C_AttContactElem
//    "Mr. Watson" <sip:watson@worcester.bell-telephone.com>
//       ;q=0.7; expires=3600
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_AttContactElem : public S_AttGeneric {

    private:
        void doParse(void);

        string nameUri;
        C_AttSipUri sipUri;
        C_AttUriParms uriParms;

    public:
        string &getNameUri(void);
        string copyNameUri(void);

        C_AttSipUri &getC_AttSipUri(void);
        C_AttSipUri copyC_AttSipUri(void);

        C_AttUriParms &getC_AttUriParms(void);
        C_AttUriParms copyC_AttUriParms(void);

        C_AttContactElem(string content);
        C_AttContactElem(const C_AttContactElem& x);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//      "Mr. Watson" <sip:watson@worcester.bell-telephone.com>
//       ;q=0.7; expires=3600,
//      "Mr. Watson" <mailto:watson@bell-telephone.com>
//        ;q=0.1
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_AttContactList : public S_AttGeneric {

    private:
        void doParse(void);

        vector<C_AttContactElem> contactList;

    public:
        vector<C_AttContactElem> &getContactList(void);
        vector<C_AttContactElem> copyContactList(void);
        C_AttContactList(string content);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// C_HeadSipRequest
// C_HeadSipRequest "Method RequestURI SipVersion<CRLF>"
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_HeadSipRequest : public S_HeadGeneric { //i //t
//NEW REVISION

    private:
        void doParse(void);

        S_AttMethod method;

        C_AttSipUri reqUri;
        S_AttSipVersion sipvs;

    public:

        S_AttMethod &getS_AttMethod(void);
        S_AttMethod copyS_AttMethod(void);

        C_AttSipUri &getC_AttSipUri(void);
        C_AttSipUri copyC_AttSipUri(void);

        S_AttSipVersion &getS_AttSipVersion(void);
        S_AttSipVersion copyS_AttSipVersion(void);

        C_HeadSipRequest(string content);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// C_HeadSipReply
// C_HeadSipReply "SIP/2.0 200 OK"
// C_HeadSipReply "SIP/2.0 xxx reply"
// Status-Line	= SIP-Version Status-Code Reason-Phrase
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_HeadSipReply : public S_HeadGeneric { //i //t
//NEW REVISION
    private:
        void doParse(void);

        S_AttSipVersion sipvs;
        S_AttReply reply;

    public:

        S_AttReply &getReply(void);
        S_AttReply copyReply(void);

        S_AttSipVersion &getSipVersion(void);
        S_AttSipVersion copySipVersion(void);

        C_HeadSipReply(string content, int genEntity);
        C_HeadSipReply(string content);

};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// HeadVia
// Via: xxxxx
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_HeadVia : public S_HeadGeneric { //i //t
//NEW CODE

    private:
        void doParse(void);

        C_AttVia via;

    public:
        C_AttVia &getC_AttVia(void);
        C_AttVia copyC_AttVia(void);

        C_HeadVia(string content);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// SHeadMaxFwd
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class S_HeadMaxFwd : public S_HeadGeneric {
//NEW REVISION

    private:
        void doParse(void);
        int mxfwd;

     public:
        int getMaxFwd(void);
        void setMaxFwd(int mxfwd);
        S_HeadMaxFwd(string content);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// C_HeadContact
// TODO set as C_AttSipUri
//    Contact: "Mr. Watson" <sip:watson@worcester.bell-telephone.com>
//       ;q=0.7; expires=3600,
//      "Mr. Watson" <mailto:watson@bell-telephone.com>
//        ;q=0.1
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_HeadContact : public S_HeadGeneric {
//NEW REVISION

    private:
        void doParse(void);
        C_AttContactList contactList;
        bool star; // "Contact: *" in register

    public:
        C_AttContactList &getContactList(void);
        C_AttContactList copyContactList(void);
        bool isStar(void);
        C_HeadContact(string content);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// HeadTo
// TODO set as C_AttSipUri
//   To: Bob <sip:bob@biloxi.example.com>;tag=8321234356
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_HeadTo : public S_HeadGeneric {
//NEW REVISION

    private:
        void doParse(void);
        C_AttContactElem to;

    public:
    	C_AttContactElem &getTo(void);
    	C_AttContactElem copyTo(void);
        C_HeadTo(string content);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// HeadFrom
// TODO set as C_AttSipUri
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_HeadFrom : public S_AttGeneric {
//NEW REVISION

//    private:
//        void doParse(void);
//        C_AttContactElem from;
//
//    public:
//    	C_AttContactElem &getFrom(void);
//    	C_AttContactElem copyFrom(void);
//        C_HeadFrom(string content, int genEntity);
    private:
        void doParse(void);

        string nameUri;
        C_AttSipUri sipUri;
        C_AttUriParms uriParms;

    public:
        string &getNameUri(void);
        string copyNameUri(void);

        C_AttSipUri &getC_AttSipUri(void);
        C_AttSipUri copyC_AttSipUri(void);

        C_AttUriParms &getC_AttUriParms(void);
        C_AttUriParms copyC_AttUriParms(void);

        C_HeadFrom(string content);
        //C_HeadFrom(const C_AttContactElem& x);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// HeadCallID
// 238556723098563298463789@hsfalkgjhaslgh.com
// id1@id2
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_HeadCallId : public S_HeadGeneric {
//NEW REVISION

    private:
        void doParse(void);
        Tuple callId;

    public:
        Tuple& getCallId(void);
        Tuple copyCallId(void);
        string getNormCallId(void);
        C_HeadCallId(string content);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// HeadCSeq
// CSeq: 1 INVITE
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_HeadCSeq : public S_HeadGeneric {
//NEW REVISION

    private:
        void doParse(void);
        int sequence;
        S_AttMethod method;

    public:
        int getSequence(void);
        S_AttMethod &getMethod(void);
        S_AttMethod copyMethod(void);
        C_HeadCSeq(string content);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// HeadContentType
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_HeadContentType : public S_HeadGeneric { //TODO
//NEW REVISION

    private:
        void doParse(void);
        Tuple contType;

    public:
        Tuple &getContentType(void);
        Tuple copyContentType(void);
        C_HeadContentType(string content);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// HeadContentLength
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class S_HeadContentLength : public S_HeadGeneric { //TODO
//NEW REVISION

    private:
        void doParse(void);
        int contLength;

    public:
        int getContentLength(void);
        S_HeadContentLength(string content);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// SDPInfo
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_SDPInfo : public S_HeadGeneric { //TODO
//NEW REVISION

    private:
        void doParse(void);
        TupleVector sdp;

    public:
        TupleVector &getSDP(void);
        TupleVector copySDP(void);
        C_SDPInfo(string content);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Allow
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_HeadAllow : public S_HeadGeneric {

	private:
		void doParse(void);
		vector<S_AttMethod> allowedMethods;

	public:
		vector<S_AttMethod> &getAllowedMethods(void);
		vector<S_AttMethod> copyAllowedMethods(void);
		bool isAllowed(string method);
		bool isAllowed(S_AttMethod method);
		C_HeadAllow(string content);

};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Subject
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_HeadSubject : public S_HeadGeneric {

	private:
		void doParse(void);
		string subject;

	public:
		string &getSubject(void);
		C_HeadSubject(string content);

};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Route
//     Route: <sip:127.0.0.1:5060;lr>
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_HeadRoute : public S_HeadGeneric {

	private:
		void doParse(void);
		S_AttHostPort routeHost;
		string lr;

	public:
		S_AttHostPort &getRoute(void);
		C_HeadRoute(string content);

};
/*
[Organization: GUGLICORP]
[Supported: replaces,norefersub,100rel]
[User-Agent: Twinkle/1.1]
 */
