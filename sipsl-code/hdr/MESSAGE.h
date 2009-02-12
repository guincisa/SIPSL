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

#define MESSAGE_H

#ifndef UTIL_H
#include "UTIL.h"
#endif

#ifndef CS_HEADERS_H
#include "CS_HEADERS.h"
#endif

using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Source / Destination of the message:
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define SODE_NOPOINT 0
#define SODE_STOPPOINT 9999
#define SODE_APOINT 1
#define SODE_BPOINT 2
#define SODE_ALOPOINT 3
#define SODE_SMCLPOINT 4
#define SODE_SMSVPOINT 5
#define SODE_TIMERPOINT 6

#define MAXLINES 50


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//// BASEMESSAGE
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class BASEMESSAGE {

    public:

        BASEMESSAGE(string incMessBuff, int genEntity, SysTime inc_ts, int sock,
                    struct sockaddr_in echoClntAddr);
        BASEMESSAGE(string incMessBuff, SysTime inc_ts);

        string &getLine(int);
        int getTotLines(void);

    private:
        string incMessBuff;
        int genEntity;

        vector<string> flex_line;

        // reply network info
        SysTime inc_ts;
        int sock;
        struct sockaddr_in echoClntAddr;

    private:

        void fillLineArray(void);
        int totLines;
        bool arrayFilled;

        vector<int> linePosition;

};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//// MESSAGE
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MESSAGE : public BASEMESSAGE {

    private:
    	//Headers
    	C_HeadSipRequest 	headSipRequest;
    	bool 				headSipRequest_p;
    	C_HeadSipReply   	headSipReply;
    	bool 				headSipReply_p;
    	stack<C_HeadVia>	s_headVia;
    	bool 				s_headVia_p;
    	S_HeadMaxFwd	 	headMaxFwd;
    	bool 				headMaxFwd_p;
    	C_HeadContact	 	headContact;
    	bool 				headContact_p;
    	C_HeadTo	     	headTo;
    	bool 				headTo_p;
    	C_HeadFrom       	headFrom;
    	bool 				headFrom_p;
    	C_HeadCallId     	headCallId;
    	bool 				headCallId_p;
    	C_HeadCSeq			headCSeq;
    	bool 				headCSeq_p;
    	C_HeadContentType	headContentType;
    	bool 				headContentType_p;
    	S_HeadContentLength headContentLenght;
    	bool 				headContentLenght_p;
    	C_SDPInfo			headSDPInfo;
    	bool 				headSDPInfo_p;
    	C_HeadAllow			headAllow;
    	bool 				headAllow_p;
    	C_HeadSubject		headSubject;
    	bool 				headSubject_p;

    public:

    	C_HeadSipRequest 	&getHeadSipRequest(void);
    	C_HeadSipReply   	&getHeadSipReply(void);
    	stack<C_HeadVia>	&getS_headVia(void);
    	S_HeadMaxFwd	 	&getHeadMaxFwd(void);
    	C_HeadContact	 	&getHeadContact(void);
    	C_HeadTo	     	&getHeadTo(void);
    	C_HeadFrom       	&getHeadFrom(void);
    	C_HeadCallId     	&getHeadCallId(void);
    	C_HeadCSeq			&getHeadCSeq(void);
    	C_HeadContentType	&getHeadContentType(void);
    	S_HeadContentLength &getHeadContentLenght(void);
    	C_SDPInfo			&getHeadSDPInfo(void);
    	C_HeadAllow			&getHheadAllow(void);
    	C_HeadSubject		&getHeadSubject(void);


    	/*
    // INTERNAL params
        int headerType[MAXLINES];

        int genEntity;
        int destEntity;

        //TIMER
        int deltams;
        int deltaalg; // if the delta increases with the iterations
        int iters;



        void setMethodPointer(void);


        //REPLY
        REGISTER200OK * p_create200ok(void);


        //REPLY TEMPLATE
        REPLY replyTemplate;

        //REPLY METHODS
        //void sendReply(int, string); // deprecated
        void sendReply(string);
        void sendAck(string);
        void sendNAck(int, string);
        void createReplyTemplate(string);

        private:
        // TODO use pointer...
        char *echoBuffer;
		*/
};
