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
#include <sys/time.h>
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */

using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Source / Destination of the message:
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define SODE_NOPOINT 0
#define SODE_KILL 9999
#define SODE_KILLDOA 9
#define SODE_FWD 1
#define SODE_BKWD 2
#define SODE_ALOPOINT 3
#define SODE_SMCLPOINT 4
#define SODE_SMSVPOINT 5
#define SODE_TIMERPOINT 6
#define SODE_NTWPOINT 7 //undefined a or b
//V5
#define SODE_TRNSCT_CL 4
#define SODE_TRNSCT_SV 5
#define SODE_CALL_SM 8

#define TYPE_TRNSCT 0 // reserved for ack
#define TYPE_NNTRNSCT 1

//dead on arrival: the message carring this will trigger the deletion of call_oset
#define NOT_DOA 0
#define DOA_REQUESTED 1
#define DOA_CONFIRMED 2

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// SL_CO overall states:
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define OS_INIT 0

//Server
#define OS_CALLING 1

//Client
#define OS_TRYING 2

#define OS_PROCEEDING 3
#define OS_COMPLETED 4
#define OS_TERMINATED 5
#define OS_CONFIRMED 6

#define TYPE_MESS 1
#define TYPE_OP 2


#define TYPE_OP_NOOP 0
#define TYPE_OP_TIMER_ON 3
#define TYPE_OP_TIMER_OFF 4
//Used to send command to a state machine
#define TYPE_OP_SMCOMMAND 5


//#define SODE_SMSVCALLPOINT 8
//#define SODE_SMCLCALLPOINT 9


#define MAXLINES 50

//Type of request or reply
#define REQSUPP 1
#define REQUNSUPP 3
#define REPSUPP 2
#define REPUNSUPP 4

class CALL_OSET;
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//// BASEMESSAGE
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

typedef struct sockaddr_in sockaddr_inX;

class MESSAGE {

	////////////////////////////////
	public:
		MESSAGE(char* incMessBuff,
				int genEntity,
				SysTime inc_ts,
				int sock,
				struct sockaddr_in echoClntAddr);
		MESSAGE(MESSAGE*,
				int genEntity,
				SysTime creaTime);
		~MESSAGE();

		////////////////////////////////
	private:
		MESSAGE* sourceMessage;

		////////////////////////////////
	public:
		void setValid(int);
        //MainMessage
	private:
        int invalid;

		////////////////////////////////
	public:
		bool getLock(void);
		void unSetLock(CALL_OSET*);
	private:
    	bool lock;

		////////////////////////////////
	public:
        string &getKey(void);
        void setKey(string key);
	private:
    	string key;

		////////////////////////////////
	public:
		int fillIn(void);
		bool isFilled(void);
		//Exact length of the original buffer
		int getDimString(void);
		char* getOriginalString(void);
		bool hasSDP(void);
	private:
		char* message_char;
		//this does not change
		char* original_message;
		//char* and bool true if the line is new false if it is pointing to the strtok
		vector< pair<char*, bool> > message_line;
		vector< pair<char*, bool> > via_line;
		vector< pair<char*, bool> > sdp_line;
		bool filledIn;
		bool hasvialines;
		bool hasSdp;

		////////////////////////////////
		//Network
	public:
		int getSock(void);
		struct sockaddr_in getEchoClntAddr(void);
	private:
		//Network
        int sock;
        sockaddr_inX echoClntAddr;
        SysTime inc_ts;

		////////////////////////////////
	public:
		//Routing
        int getRequestDirection(void);
        void setRequestDirection(int);
        int getGenEntity(void);
        void setGenEntity(int);
        void setDestEntity(int);
        int getDestEntity(void);
	private:
    	int requestDirection;
		int genEntity;
		int destEntity;

		////////////////////////////////
        //Timer
	public:
        lli MESSAGE::getFireTime(void);
		void MESSAGE::setFireTime(lli);
		//Timer support
	private:
		lli fireTime;

		////////////////////////////////
	public:
		string getOrderOfOperation(void);
		void setOrderOfOperation(string);
		int getType_trnsct(void);
		void setType_trnsct(int);
		int getTypeOfInternal(void);
		void setTypeOfInternal(int);
		int getTypeOfOperation(void);
		void setTypeOfOperation(int);
	private:
    	string orderOfOperation;
    	int type_trnsct;
    	int typeOfInternal;
    	int typeOfOperation;

		////////////////////////////////
	public:
		int getModulus(void);
	private:
        int modulus;



		////////////////////////////////
        //SIP headers
	public:
    	void setGenericHeader(string header, string content);
    	string getGenericHeader(string header);
    	bool queryGenericHeader(string header); //if is present
    	void addGenericHeader(string header, string content);
    	void dropHeader(string header);
    	string getProperty(string,string); //header name, property


		////////////////////////////////
    	//MOST USED INFORMATION
    	//Via (last row)
	public:
    	string getViaLine(void);
    	string getViaBranch(void);
	private:
    	string branch;
    	bool parsedBranch;

		////////////////////////////////
	public:
    	string getHeadCallId(void);
	private:
    	string callId;
    	bool parsedCallId;

		////////////////////////////////
    	//REQUEST-REPLY
	public:
        int getReqRepType(void);
    	void setHeadSipRequest(string content);
    	void setHeadSipReply(string content);
    	int getHeadSipRequestCode(void);
    	string getHeadSipRequest(void);
    	int getHeadSipReplyCode(void);
    	string getHeadSipReply(void);
	private:
    	int reqRep;
    	string headSipRequest;
    	string headSipReply;
    	int replyCode;
    	int requestCode;

		////////////////////////////////
	public:
    	getHeadCSeq
	private:
    	string cSeq;
    	bool parsedCseq;


};

//class BASEMESSAGE {
//
//    public:
//
//        BASEMESSAGE(string incMessBuff, int genEntity, SysTime inc_ts, int sock,
//                    struct sockaddr_in echoClntAddr);
//        BASEMESSAGE(string incMessBuff, SysTime inc_ts);
//
//        BASEMESSAGE(BASEMESSAGE*, int genEntity, SysTime creaTime);
//
////        BASEMESSAGE(const BASEMESSAGE& x);
//
//
//        string &getLine(int);
//        int getTotLines(void);
//
//
//
//        string &getIncBuffer(void);
//
//        int getGenEntity(void);
//        void setGenEntity(int);
//
//        int getRequestDirection(void);
//        void setRequestDirection(int);
//
//        void setDestEntity(int);
//        int getDestEntity(void);
//
//        void setEndPoints(int from, int to);
//
//
//        string &getKey(void);
//        void setKey(string key);
//
//        struct sockaddr_in getAddress(void);
//        int getSock(void);
//
//        SysTime getCreationTime(void);
//
//        void setValid(int);
//
//
//    protected:
//        int genEntity;
//        int destEntity;
//        int requestDirection;
//        vector<string> flex_line;
//        SysTime inc_ts;
//
//        vector<string> getLines(void);
//
//        void removeHeader(int pos);
//
//        int id; //Used in spin buffer
//
//
//    protected:
//        string  incMessBuff;
//
//        // reply network info
//
//        int sock;
//        sockaddr_inX echoClntAddr;
//
//        //V4 workaround to avoid deletion of the sent INVITE
//
//    protected:
//        //Memory protection
//        int invalid;
//
//    private:
//    	//used to store in message map
//    	string key;
//
//        void fillLineArray(void);
//        int totLines;
//        bool arrayFilled;
//
//        vector<int> linePosition;
//}
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
////// MESSAGE
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
//class O_MESSAGE : public BASEMESSAGE {
//
//    private:
//    	//Headers
//    	C_HeadSipRequest 	headSipRequest;
//    	C_HeadSipReply   	headSipReply;
//
//    	//S_HeadGeneric	headSipReqRep;
//    	// REQSUPP request supported, REPSUPP reply supported
//    	// REQUNSUPP request unsupported, REPUNSUPP reply unsupported ??? defined latefr
//    	int reqRep;
//
//    	stack<C_HeadVia*>	s_headVia;
//    	bool 				s_headVia_p;
//    	S_HeadMaxFwd	 	headMaxFwd;
//    	bool 				headMaxFwd_p;
//    	C_HeadContact*	 	headContact;
//    	bool 				headContact_p;
//    	C_HeadTo*	     	headTo;
//    	bool 				headTo_p;
//    	C_HeadFrom*       	headFrom;
//    	bool 				headFrom_p;
//    	C_HeadCallId     	headCallId;
//    	bool 				headCallId_p;
//    	C_HeadCSeq			headCSeq;
//    	bool 				headCSeq_p;
//    	C_HeadRoute*		headRoute;
//							//parsed, exists, correct
//    	bool 				headRoute_p, headRoute_e;
//
//    	vector<string>		sdpVector;
//    	bool				sdpVector_p;
//    	bool				sdpVecrot_e; // exists
//    	int					sdpSize;
//
//    	//Modulus to reach the correct comap
//    	int					modulus;
//
//    	//Allows to purge retranmissions
//    	bool 				lock;
//
//    	// Used to generate a message from an incoming one
//    	// tipically a reply
//    	O_MESSAGE* source;
//
//    	// if false then the message comes from the
//    	// network and cannot be changed
//    	// if true then it has been generated internally and
//    	// can be modified
//    	bool isInternal;
//
//    	int typeOfInternal; // Message or operation
//    	int typeOfOperation; // Type of operation
//    	string orderOfOperation; //Alarm id in case more alarms are triggered with the same message
//    	//int specialAction; //delete co
//
//        //Timer support
//        lli fireTime;
//
//
//    	//Needed for ACK
//    	int type_trnsct;
//
//        //Need to eliminate the source message
//        //SL_CC
//        C_HeadCallId sourceHeadCallId;
//        int sourceModulus;
//
//
//    public:
//
//        O_MESSAGE(string incMessBuff, int genEntity, SysTime inc_ts, int sock,
//                    struct sockaddr_in echoClntAddr);
//        //MESSAGE(string incMessBuff, SysTime inc_ts);
//
//        // this is temporary and needed to create internal messages
//        //MESSAGE(void);
//
//        //
//        O_MESSAGE(O_MESSAGE*, int genEntity, SysTime creaTime);
//
//        ~O_MESSAGE();
//
////        MESSAGE(const MESSAGE& x);
//
//
//    	// also use for getting the callIDx to retrieve CALL_OSET SV side
//        O_MESSAGE* getSourceMessage(void);
//    	void setSourceMessage(O_MESSAGE*);
//
//        //translates the flex_line into a string to be sent to network
//        void compileMessage(void);
//
//    	int getReqRepType(void);
//
//    	//S_HeadGeneric 	&getHeadSipReqRep(void);
//    	C_HeadSipReply   	&getHeadSipReply(void);
//    	C_HeadSipRequest 	&getHeadSipRequest(void);
//
//    	stack<C_HeadVia*>	&getSTKHeadVia(void);
//    	//TODO is this one ok?
//    	void				popSTKHeadVia(void);
//    	S_HeadMaxFwd	 	&getHeadMaxFwd(void);
//    	C_HeadContact*	 	getHeadContact(void);
//    	C_HeadTo*	     	getHeadTo(void);
//    	C_HeadFrom*       	getHeadFrom(void);
//    	C_HeadCallId     	&getHeadCallId(void);
//    	C_HeadCSeq			&getHeadCSeq(void);
//    	C_HeadContentType	&getHeadContentType(void);
//    	S_HeadContentLength &getHeadContentLenght(void);
//    	C_SDPInfo			&getHeadSDPInfo(void);
//    	C_HeadAllow			&getHeadAllow(void);
//    	C_HeadSubject		&getHeadSubject(void);
//    	C_HeadRoute*		getHeadRoute(void) throw (HeaderException);
//
//    	//string getExtendedInternalCID(void);
//
//    	string getTransactionExtendedCID(void);
//
//    	string getDialogExtendedCID(void);
//
//    	void removeHeadRoute(void);
//
//    	void dropHeader(string header);
//
//    	void setHeadSipRequest(string content);
//    	void setHeadSipReply(string content);
//
//    	void replaceHeadCSeq(string content);
//    	void replaceHeadCSeq(int cseq, string method);
//
//
//    	//SDP management
//    	void purgeSDP(void);
//    	void importSDP(vector<string> sdp);
//    	vector<string> getSDP(void);
//    	int getSDPSize(void);
//
//		//purge all vias
//    	void purgeSTKHeadVia(void);
//    	//insert via
//    	void pushHeadVia(string content);
//
//    	void replaceHeadFrom(string content);
//    	void replaceHeadTo(string content);
//
//
//    	void replaceHeadContact(string content);
//
//    	void removeMaxForwards(void);
//    	void increaseMaxForwards(void);
//
//    	void setGenericHeader(string header, string content);
//    	string getGenericHeader(string header);
//    	bool queryGenericHeader(string header); //if is present
//    	void addGenericHeader(string header, string content);
//
//    	// position of first line of SDP
//    	// it is needed when I need to add headers
//    	// which have to be inserted before
//    	// it is found by searching "=" in second position
//    	// it is calculated every time it is invoked...
//    	vector<string>::iterator getSDPposition(void);
//    	void dumpVector(void);
//
//    	void setFireTime(lli fireTime);
//    	lli getFireTime(void);
//
//
//    	int getModulus(void);
//
//    	void setLock(CALL_OSET*);
//    	bool getLock(void);
//    	void unSetLock(CALL_OSET*);
//
//    	string getOrderOfOperation(void);
//    	void setOrderOfOperation(string);
//
//    	int getTypeOfInternal(void);
//    	void setTypeOfInternal(int);
//
//    	int getType_trnsct(void);
//    	void setType_trnsct(int);
//
//    	int getTypeOfOperation(void);
//    	void setTypeOfOperation(int);
//
//
//        C_HeadCallId& getSourceHeadCallId();
//	int getSourceModulus();
//        void setSourceHeadCallId(string);
//	void setSourceModulus(int);
//
//
//#ifdef MESSAGEUSAGE
//		int inuse;
//#endif
//};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//// ALLOCATED MESSAGES TABLE
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//static multimap<const string, MESSAGE *> globalMessTable;
#ifdef USEMESSAGEMAP
extern map<const MESSAGE*, MESSAGE *> globalMessTable[MESSAGEMAPS];
extern pthread_mutex_t messTableMtx[MESSAGEMAPS];
#endif
extern MESSAGE* MainMessage;


