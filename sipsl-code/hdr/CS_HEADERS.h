//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Service Layer 
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

#define CS_HEADERS
#ifndef UTIL
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

//NEW REVISION START
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

    return tt;
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
//NEW REVISION

    public: 
        bool isCorrect(void);
        bool isParsed(void);
        string getContent(void);
        void setContent(string buff, int genEntity);
        
        // created using buffer and endpoint id which generates header
	S_HeadGeneric(string buff, int genEntity);

    protected:
        bool parsed;
        bool correct;
        bool isSet;
        
        // Id of the generating endpoint
        // same as SODE_*
        int genEntity;

        string content;
        virtual void doParse(void) = 0;

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

        string getContent(void);

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
class TupleVector : public S_AttGeneric{ //i
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
        //header can be ? or whaterver the string begins with 
        string findRvalue(string Lvalue);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// S_AttMethod
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class S_AttMethod : public S_AttGeneric{ //i
//NEW REVISION

    private:
        void doParse(void);
        int methodID;
        string methodName;

    public:
        int getMethodID(void);
        string getMethodName(void);
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
        string getUserName(void);
        string getPassword(void);
        S_AttUserInfo(string content);
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
        string getHostName(void);
        int getPort(void);
        S_AttHostPort(string content);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//C_AttUriParms
//    transport=tcp;ttl=15;...
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_AttUriParms : public S_AttGeneric{
//NEW REVISION
//
    private:
        void doParse(void);
        
        TupleVector tuples;

    public:
        TupleVector getTuples(void);
        C_AttUriParms(string content);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//AttUriHeads
//    ?to=alice%40atalnta.com&priority=urgent&...
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_AttUriHeaders : public S_AttGeneric{
//NEW REVISION

    private:
        void doParse(void);

        TupleVector tuples;

    public:
        TupleVector getTuples(void);
        C_AttUriHeaders(string content);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// C_AttSipUri
//     "sip:alice:secretword@atlanta.com;transport=tcp"
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_AttSipUri : public S_AttGeneric{ // sip or sips
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
        S_AttUserInfo getS_AttUserInfo(void);
        S_AttHostPort getS_AttHostPort(void);
        C_AttUriParms getC_AttUriParms(void);
        C_AttUriHeaders getC_AttUriHeads(void); 
        C_AttSipUri(string content);
        
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
class S_AttSipVersion : public S_AttGeneric{ //fake i
//NEW REVISION 
    private:
        void doParse(void);
        string version;
        string protocol;
    public:
        string getProtocol(void);
        string getVersion();
        S_AttSipVersion(string content);
        S_AttSipVersion(string protocol, string version);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//C_AttVia
//   ???? based on old sipsl
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_AttVia : public S_AttGeneric {
//NEW REVISION

    private:
        void doParse(void);
// OLD CODE
        S_AttHostPort hostPort;
        TupleVector viaParms;

    public:
        S_AttHostPort getHostPort(void);
        TupleVector getViaParms(void);
} ;       
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// C_HeadSipRequest
// C_HeadSipRequest "Method RequestURI SipVersion<CRLF>"
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_HeadSipRequest : public S_HeadGeneric { //i
//NEW REVISION

    private:
        void doParse(void);

        S_AttMethod method;

        C_AttSipUri reqUri;
        S_AttSipVersion sipvs;

    public:

        S_AttMethod getMethod(void);
        C_AttSipUri getC_AttSipUri(void);
        S_AttSipVersion getSipVs(void);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//S_AttReply
//    200 OK
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class S_AttReply : public S_AttGeneric{ //i
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
        string getReply(void);
        S_AttReply(string content);
        S_AttReply(int replyID, int code);
        S_AttReply(string replyID, string code);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// C_HeadSipReply
// C_HeadSipReply "SIP/2.0 200 OK"
// C_HeadSipReply "SIP/2.0 xxx reply"
// Status-Line	= SIP-Version Status-Code Reason-Phrase 
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_HeadSipReply : public S_HeadGeneric { //i
//NEW REVISION
    private:
        void doParse(void);

        S_AttSipVersion sipvs;
        S_AttReply reply;

    public:

        S_AttReply getReply(void);
        S_AttSipVersion getSipVersion(void);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// HeadVia
// Via: xxxxx 
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class HeadVia : public S_HeadGeneric {
//NEW CODE

    private:
        void doParse(void);

        C_AttVia via;

    public:
        C_AttVia getC_AttVia(void);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// S_HeadMaxFwd
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class S_HeadMaxFwd : public S_HeadGeneric {
//NEW REVISION

    private:
        void doParse(void);
        int mxfwd;

     public:
        int getMaxFwd(void);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// C_HeadContact
// TODO set as C_AttSipUri
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_HeadContact : public S_HeadGeneric {
//NEW REVISION

    private:
        void doParse(void);
        C_AttSipUri contactUri;
        bool star; // "Contact: *" in register

    public:
        C_AttSipUri getContact(void);
        bool isStar(void);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// HeadTo
// TODO set as C_AttSipUri
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_HeadTo : public S_HeadGeneric {
//NEW REVISION

    private:
        void doParse(void);
        C_AttSipUri toUri;

    public:
        C_AttSipUri getTo(void);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// HeadFrom
// TODO set as C_AttSipUri
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_HeadFrom : public S_HeadGeneric {
//NEW REVISION

    private:
        void doParse(void);
        C_AttSipUri fromUri;

    public:
        C_AttSipUri getFrom(void);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// HeadCallID
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class S_HeadCallID : public S_HeadGeneric {
//NEW REVISION

    private:
        void doParse(void);
        string callId;

    public:
        string getCallID(void);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// HeadCSeq
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_HeadCSeq : public S_HeadGeneric {
//NEW REVISION

    private:
        void doParse(void);
        int sequence;
        S_AttMethod method;

    public:
        string getCallID(void);
        S_AttMethod getMethod(void);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// HeadContentType
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_HeadContentType : public S_HeadGeneric {
//NEW REVISION

    private:
        void doParse(void);
        Tuple contType;

    public:
        Tuple getContentType(void);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// HeadContentLength
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class S_HeadContentLength : public S_HeadGeneric {
//NEW REVISION

    private:
        void doParse(void);
        int contLength;

    public:
        int getContentLength(void);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// SDPInfo
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class C_SDPInfo : public S_HeadGeneric {
//NEW REVISION

    private:
        void doParse(void);
        TupleVector sdp;

    public:
        TupleVector getSDP(void);
};
