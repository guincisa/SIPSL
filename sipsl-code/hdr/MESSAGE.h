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

#include <stack>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */


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

//Type of request or reply
#define REQSUPP 1
#define REQUNSUPP 3
#define REPSUPP 2
#define REPUNSUPP 4


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//// BASEMESSAGE
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

typedef struct sockaddr_in sockaddr_inX;

class BASEMESSAGE {

    public:

        BASEMESSAGE(string incMessBuff, int genEntity, SysTime inc_ts, int sock,
                    struct sockaddr_in echoClntAddr);
        BASEMESSAGE(string incMessBuff, SysTime inc_ts);

        BASEMESSAGE(BASEMESSAGE*);

        string &getLine(int);
        int getTotLines(void);


        int id; //Used in spin buffer

        string &getIncBuffer(void);

        int getGenEntity(void);
        void setGenEntity(int);

        void setDestEntity(int);
        int getDestEntity(void);


        string &getKey(void);
        void setKey(string key);

        struct sockaddr_in getSocket(void);
        int getSock(void);

    protected:
        int genEntity;
        int destEntity;
        vector<string> flex_line;
        SysTime inc_ts;

        vector<string> getLines(void);

        void removeHeader(int pos);

    private:
        string  incMessBuff;

        // reply network info

        int sock;
        sockaddr_inX echoClntAddr;

    private:
    	//used to store in message map
    	string key;

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
    	C_HeadSipReply   	headSipReply;

    	//S_HeadGeneric	headSipReqRep;
    	// REQSUPP request supported, REPSUPP reply supported
    	// REQUNSUPP request unsupported, REPUNSUPP reply unsupported ??? defined latefr
    	int reqRep;

    	stack<C_HeadVia*>	s_headVia;
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
    	C_HeadRoute			headRoute;
    	bool 				headRoute_p;
    	int 				headRoute_pos;
    	//C_HeadContentType	headContentType;
    	//bool 				headContentType_p;
    	//S_HeadContentLength headContentLenght;
    	//bool 				headContentLenght_p;
    	//C_SDPInfo			headSDPInfo;
    	//bool 				headSDPInfo_p;
    	//C_HeadAllow			headAllow;
    	//bool 				headAllow_p;
    	//C_HeadSubject		headSubject;
    	//bool 				headSubject_p;

    	// Used to generate a message from an incoming one
    	// tipically a reply
    	MESSAGE *source;

    	// if false then the message comes from the
    	// network and cannot be changed
    	// if true then it has been generated internally and
    	// can be modified
    	bool isInternal;

    public:

        MESSAGE(string incMessBuff, int genEntity, SysTime inc_ts, int sock,
                    struct sockaddr_in echoClntAddr);
        MESSAGE(string incMessBuff, SysTime inc_ts);

        // this is temporary and needed to create internal messages
        MESSAGE(void);

        //
        MESSAGE(MESSAGE*, int genEntity);

    	int getReqRepType(void);

    	//S_HeadGeneric 	&getHeadSipReqRep(void);
    	C_HeadSipReply   	&getHeadSipReply(void);
    	C_HeadSipRequest 	&getHeadSipRequest(void);

    	stack<C_HeadVia*>	&getSTKHeadVia(void);
    	S_HeadMaxFwd	 	&getHeadMaxFwd(void);
    	C_HeadContact	 	&getHeadContact(void);
    	C_HeadTo	     	&getHeadTo(void);
    	C_HeadFrom       	&getHeadFrom(void);
    	C_HeadCallId     	&getHeadCallId(void);
    	C_HeadCSeq			&getHeadCSeq(void);
    	C_HeadContentType	&getHeadContentType(void);
    	S_HeadContentLength &getHeadContentLenght(void);
    	C_SDPInfo			&getHeadSDPInfo(void);
    	C_HeadAllow			&getHeadAllow(void);
    	C_HeadSubject		&getHeadSubject(void);
    	C_HeadRoute			&getHeadRoute(void);


    	void removeHeadRoute(void);

    	void setHeadSipRequest(string content, int genEntity);

    	void setHeadCSeq(string content, int genEntity);

		//purge all vias
    	void purgeSTKHeadVia(void);
    	//insert via
    	void pushHeadVia(string content, int genEntity, int pos);

    	void setHeadFrom(string content, int genEntity);

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

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//// ALLOCATED MESSAGES TABLE
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static multimap<string, MESSAGE *> globalMessTable;
static pthread_mutex_t messTableMtx;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//// MESSAGE PLACEHOLDER or EMPTYMESSAGE
//// The empty message is a command sent by the states machines
//// the SL_CO will generate the full message using the generator
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class EMPTYMESSAGE {

	private:
		int method;
		int destination;
		int source;

	public:
		EMPTYMESSAGE(int method, int destination, int source);
		int getMethod(void);
		int getDestination(void);
		int getSource(void);
};


