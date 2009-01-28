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

#define P_HEADERS
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//P_AttUriParms
//    transport=tcp;ttl=15;...
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class P_AttUriParms : public S_AttGeneric{
// 27 01 2009
    private:
        void doParse(void);
        
        TupleVector *tuples;

    public:
        TupleVector *getTuples(void);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//P_AttUriHeads
//    ?to=alice%40atalnta.com&priority=urgent&...
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class P_AttUriHeaders : public S_AttGeneric{
//27 01 2009
    private:
        void doParse(void);

        TupleVector *tuples;

    public:
        TupleVector *getTuples(void);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// P_AttSipUri
//     "sip:alice:secretword@atlanta.com;transport=tcp"
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class P_AttSipUri : public S_AttGeneric {
//27 01 2009
        private:
        void doParse(void);

        bool isSecure; //sip or sips
        S_AttUserInfo *userInfo; // alice:secretword@
        S_AttHostPort *hostPort; // gateway.conm:123
        P_AttUriParms *uriParms; // transport=tcp;ttl=15;...
        P_AttUriHeaders *uriHeads; // ?to=alice%40atalnta.com&priority=urgent&...

     public:
        bool getIsSec(void);
        S_AttUserInfo *getS_AttUserInfo(void);
        S_AttHostPort *getS_AttHostPort(void);
        P_AttUriParms *getAttUriParms(void);
        P_AttUriHeaders *getAttUriHeads(void); 
        P_AttSipUri(string);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//P_AttVia
//   ???? based on old sipsl
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class P_AttVia : public S_AttGeneric {
//NEW REVISION

    private:
        void doParse(void);
// OLD CODE
        S_AttHostPort *hostPort;
        TupleVector *viaParms;

    public:
        S_AttHostPort *getHostPort(void);
        TupleVector *getViaParms(void);
} ;       
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// P_HeadSipRequest
// P_HeadSipRequest "Method RequestURI SipVersion<CRLF>"
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class P_HeadSipRequest : public S_HeadGeneric { //i
//27 01 2009

    private:
        void doParse(void);

        S_AttMethod *method;

        P_AttSipUri *reqUri;
        S_AttSipVersion *sipvs;

    public:

        S_AttMethod *getMethod(void);
        P_AttSipUri *getAttSipUri(void);
        S_AttSipVersion *getSipVs(void);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// P_HeadSipReply
// P_HeadSipReply "SIP/2.0 200 OK"
// P_HeadSipReply "SIP/2.0 xxx reply"
// Status-Line	= SIP-Version Status-Code Reason-Phrase 
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class P_HeadSipReply : public S_HeadGeneric { //i
//NEW REVISION
    private:
        void doParse(void);

        S_AttSipVersion *sipvs;
        S_AttReply *reply;

    public:

        S_AttReply *getReply(void);
        S_AttSipVersion *getSipVersion(void);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// P_HeadVia
// Via: xxxxx 
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class P_HeadVia : public S_HeadGeneric {
//NEW CODE

    private:
        void doParse(void);

        P_AttVia *via;

    public:
        P_AttVia *getP_AttVia(void);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// P_HeadContact
// TODO set as AttSipUri
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class P_HeadContact : public S_HeadGeneric {
//NEW REVISION

    private:
        void doParse(void);
        P_AttSipUri *contactUri;
        bool star; // "Contact: *" in register

    public:
        P_AttSipUri *getContact(void);
        bool isStar(void);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// P_HeadTo
// TODO set as AttSipUri
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class P_HeadTo : public S_HeadGeneric {
//NEW REVISION

    private:
        void doParse(void);
        P_AttSipUri *toUri;

    public:
        P_AttSipUri *getTo(void);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// P_HeadFrom
// TODO set as AttSipUri
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class P_HeadFrom : public S_HeadGeneric {
//NEW REVISION

    private:
        void doParse(void);
        P_AttSipUri *fromUri;

    public:
        P_AttSipUri *getFrom(void);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// P_HeadCSeq
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class P_HeadCSeq : public S_HeadGeneric {
//NEW REVISION

    private:
        void doParse(void);
        int sequence;
        S_AttMethod *method;

    public:
        string getCallID(void);
        S_AttMethod *getMethod(void);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// P_HeadContentType
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class P_HeadContentType : public S_HeadGeneric {
//NEW REVISION

    private:
        void doParse(void);
        Tuple *contType;

    public:
        Tuple *getContentType(void);
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// P_SDPInf
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class P_SDPInfo : public S_HeadGeneric {
//NEW REVISION

    private:
        void doParse(void);
        TupleVector *sdp;

    public:
        TupleVector *getSDP(void);
};
