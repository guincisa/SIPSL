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

#define COMM
//#include <pthread.h>
//#include <string>
//#include <iostream>
//#include <memory>
//#include <sys/time.h>
//#include <sys/socket.h>
//#include <arpa/inet.h>

//SPINBUFFER
#define WW  2
#define RR  1
#define FF  0

#define REGISTER_METHOD 1
#define INVITE_METHOD 2
#define TRY_METHOD 3
#define RINGING_METHOD 4
#define OK_METHOD 5
#define INTERNALS_METHOD 9000

#define TRY_TIMER 1003


using namespace std;
//char bu[512];
//        hrtime_t inittime;
//        hrtime_t endtime;

struct exxtime {
        struct timeval tv;
        struct timezone tz;
} typedef hrtime_t;

#define GETTIME(mytime) gettimeofday(&mytime.tv, &mytime.tz);

#define DECTIME char bu[512];hrtime_t inittime;hrtime_t endtime;
#define STARTTIME {GETTIME(inittime)}
#define ENDTIME {GETTIME(endtime); sprintf(bu, "[%s %d] init %lld end %lld tot %lld\n",__FILE__, __LINE__, inittime.tv.tv_sec*1000000+inittime.tv.tv_usec,endtime.tv.tv_sec*1000000+endtime.tv.tv_usec ,endtime.tv.tv_sec*1000000+endtime.tv.tv_usec  - inittime.tv.tv_sec*1000000+inittime.tv.tv_usec ); cout << bu << flush;}

#define DEBOUT(m1,m2)  cout << "DEBOUT " << __FILE__ <<" " <<__LINE__<< " "<< m1 << "[" << m2 << "]"<< endl;
#define DEBERROR(m1)  cout << "**** RUNTIME ERROR **** " << __FILE__ <<" " <<__LINE__<< "[" << m1 << "]"<< endl;

        /*
class SLEEEXCEPTION {
    public:
    int id;
    string message;
};
*/
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Source / Destination of the message:
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#define NOPOINT 0
#define STOPPOINT 9999 
#define APOINT 1
#define BPOINT 2
#define ALOPOINT 3
#define SMCLPOINT 4
#define SMSVPOINT 5
#define TIMERPOINT 6
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//// BASEMESSAGE
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class BASEMESSAGE {

//NEW REVISION START
    public:

        BASEMESSAGE(string inMessBuff, int internal_id, hrtime_t inc_ts, int sock,
                    struct sockaddr_in echoClntAddr);

    private:
        string incMessBuff;
        int internal_id; //SPINBUFFER ??

        string line[MAXLINES];
        
        // reply network info
        hrtime_t inc_ts;
        int sock;
        struct sockaddr_in echoClntAddr;

    public:
        string getLine(int);
        // execute the parsing on the line
        int runParseLine(int);
        int getTotLines(void);

    private:

        void fillLineArray(void);
        int totLines;
	bool arrayFilled;

        int lineParsedStatus[MAXLINES]; // 0 not parsed, 1 parsed, 2 ....

//NEW REVISION END

//???
int linePosition[MAXLINES];


};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//// SIPMESSAGE
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SIPMESSAGE : public BASEMESSAGE {

//NEW REVISION START
    private:
    // SIP headers
        RequestLine _ReqLine;

        Via         _Via[16];
        MaxForwards _MaxForwards;
        Contact     _Contact;
        To          _To;
        From        _From;
        CallID      _CallID;
        CSeq        _CSeq;
        Expires     _Expires;
        Allow       _Allow;
        UserAgent   _UserAgent;
        ContentLength _ContentLength;
	SDPInfo     _sdpInfo;

    private:
    // INTERNAL params
        int headerType[MAXLINES];
        
        // Internal call id limited to 32 chars
        string _CallID32;
        int source;  
        int destination;
//NEW REVISION END

        // in case this message is generated for the same side
	SIPMESSAGE * replygenmessage;    
        // in case this message is generated for the other side
	SIPMESSAGE * b2bgenmessage; 

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

};


class BASEREPLY {
    public:
        int dummy;
        hrtime_t out_ts;

};
///////////////////////////////////////////////////////////////////////////////
class REPLY : public BASEREPLY{
public:
    string message;
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//// HEADERS
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class MESSAGE;
class Method;
class CallID;
class To;
class From;

class HeaderValue {

	// basic header value
	// xxx: yyy
	
    public:
    string value; // unparsed

    //needed???
    int hasValue;

    //needed???
    void setBaseMessage(MESSAGE *);

    virtual bool isCorrect(void);
    virtual string getHeader(int);

    //needed???
    int position[20];

    string getValue(void); // recreates the header by lookinginto components

    protected:
        bool isparsed;
        bool iscorrect;
		bool isgenerated;
        MESSAGE *baseMessage;
    private:
        virtual bool doParse(void);
};

///////////////////////////////////////////////////////////////////////////////
class Parameters  { // lista di ;header=value
    private:
        string listofparms;
        string namevalue[2][16]; //couple 
        int freeval; // next free position in the array
    public:
        void setParms(string);
        string getParmValue(string);
        string getValue(void);
};

///////////////////////////////////////////////////////////////////////////////
/*
class URI : public HeaderValue {  // not really an header...
    private:
        bool doParse(void);
        bool secure; //sips
        bool iscorrect;

        string user; //mandatory 
        string host; // FQDN or ip?
        Parameters uriparms; // parameters before the ">"
        int port;

    public:
        Parameters getParms(void);
        string getUser(void);
        int getPort(void);
        string getHost(void);
        bool isSecure(void);

};
*/
///////////////////////////////////////////////////////////////////////////////
class StringParse {
    protected:
        string value;
        bool iscorrect;
        bool isparsed;
    private:
        virtual bool doParse(void);
    public:
        StringParse(void);
        void setInitValue(string);
        string getValue(void);
};

///////////////////////////////////////////////////////////////////////////////
class SIPURI : public StringParse{ // sip or sips
    private:
        bool doParse(void);
        
        bool issecure;
        string userinfo;
        string username;
        string password;
        string hostport;
         string host;
         int port;
        //TODO
        string restof;
        Parameters uriParms;
        string headers; // EXPAND::::

    public:
        bool isSecure(void);
        string getUserInfo(void);
        string getHost(void);
        int getPort(void);
        Parameters &getUriParms(void);
        string getHeaders(void);
};

///////////////////////////////////////////////////////////////////////////////
class RURI : public StringParse{ // SIP or SIPS URI or absoluteURI es: sip
    private:
        int type; // 1 SIP or SIPS, 2 absoluteURI
        SIPURI sipUri;
        bool doParse(void);
        //ABSURI absUri;
        
    public:
        int getType(void);
        SIPURI& getSipUri(void);
        //ABSURI getAbsUri(void);
};
///////////////////////////////////////////////////////////////////////////////
// extended RURI
class ERURI : public StringParse {
    private:
        string displayname;
        RURI eruriRuri;
        Parameters eruriParms;
        bool doParse(void);
    public:
        string getEruriDisplayname(void);
        RURI& getEruriRuri(void);
        Parameters& getEruriParms(void);
};
///////////////////////////////////////////////////////////////////////////////
class RequestLine : public HeaderValue {
    private:
        int methodId; // 1 REGISTER
        RURI reqUri;
        string protVs; // "SIP" "/" 1*DIGIT "." 1*DIGIT 
        bool doParse(void);

    public:
        int getMethodId(void);
        RURI& getURI(void);
        string getProtVs(void);
		void setMethodId(int);
};

///////////////////////////////////////////////////////////////////////////////
class CallID : public HeaderValue {
    private:
        bool doParse(void);
        string callid;
    public:
        string getCallId(void);
///////////////////////////////////////////////////////////////////////////////
};
class Contact : public HeaderValue {
    private:
        ERURI contEruri;
        int star;
        bool doParse(void);

    public:

        ERURI& getContEruri(void);
        bool isStar(void);
};

///////////////////////////////////////////////////////////////////////////////
class From : public HeaderValue {
    private:
        ERURI fromEruri;
        bool doParse(void);
    public:
        ERURI& getFromEruri(void);
};
///////////////////////////////////////////////////////////////////////////////
class To : public HeaderValue {
    private:
        ERURI ToEruri;
        bool doParse(void);
    public:
        ERURI& getToEruri(void);
};
///////////////////////////////////////////////////////////////////////////////
class Expires : public HeaderValue {
    private:
        bool doParse(void);
        int expires;
        bool present;
    public:
        int getExpires(void);
        bool isPresent(void);
};

///////////////////////////////////////////////////////////////////////////////
class Via : public HeaderValue {
    private:
        string host;
        int port;
        string hostport;
        Parameters viaParms;
        bool doParse(void);
        bool present;
    public:
        string getViaHost(void);
        int getViaPort(void);
        string getViaHostPort(void);
        Parameters& getViaParms(void);
        bool isPresent(void);
};
///////////////////////////////////////////////////////////////////////////////
class CSeq : public HeaderValue {
    private:
        int sequence;
        string method;
        bool doParse();
    public:
        int getCSeq(void);
        string getCSeqMethod(void);
};

///////////////////////////////////////////////////////////////////////////////
//TODO
///////////////////////////////////////////////////////////////////////////////
class MaxForwards : public HeaderValue {
    public:
        int mxf;
};
///////////////////////////////////////////////////////////////////////////////
class Allow: public HeaderValue {
    public:
        long allowMask;
};
///////////////////////////////////////////////////////////////////////////////
class UserAgent: public HeaderValue {
    public:
        int a;
};
///////////////////////////////////////////////////////////////////////////////
class ContentLength: public HeaderValue {
    public:
        int content;
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SIPREPLYMESSAGE {
    public:
    string placeholder;
};
class REGISTERREPLY : public SIPREPLYMESSAGE {
    public:
    string anotherplaceholder;
};
class REGISTER200OK : public SIPREPLYMESSAGE {
    private:
        string statusLine;
        string _To;
        string _From;
        string _CallID;
        string _CSeq;
        string _Via[16];
        string _Contact;
        string _Date;
        string _Server;
        int _ContentLength;

        int sock;
        struct sockaddr_in echoClntAddr;

    public:
        void setSock(int);
        void setSockAddr(struct sockaddr_in);
        void setHeaders(string,string,string,string,string[],string);
        void setTo(From);
        void setFrom(From);
        void setCallID(CallID);
        void setCSeq(CSeq);
        void setVia(Via[]);
        void setContact(Contact);

        void send(void);

        REGISTER200OK(void);
        ~REGISTER200OK(void);
};
//*****************************************************************************
/*
 XXX XXX XXXXXX   XXX X   XXX X    XX     XXX X  XXXXXX 
  XX XX   X   X  X   XX  X   XX     X    XX  X    X   X 
  XX XX   XXX     XXXX    XXXX     X X   X        XXX   
  X X X   X           X       X    XXX   X   XXX  X     
  X   X   X   X  X    X  X    X   X   X  XX   X   X   X 
 XXX XXX XXXXXX  XXXXX   XXXXX   XXX XXX  XXXX   XXXXXX 
*/
//*****************************************************************************

class REPLYNOK {
        string message;
};

class ThreadWrapper {
    public:
        pthread_t thread;
        pthread_mutex_t mutex;
        ThreadWrapper();
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//SPINBUFFER
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SPINB;
///////////////////////////////////////////////////////////////////////////////
class ROTQ {
    private:
        MESSAGE Q[ARR];
        int top,bot; // da scrivere, da leggere 
        int state;
        SPINB *sb;

    public:
        ROTQ(void);
        void setSpinb(SPINB *);
        void setState(int);
        int getState(void);
        void put(MESSAGE);
        MESSAGE get(void);
        bool isEmpty(void);
};
///////////////////////////////////////////////////////////////////////////////
class SPINB {
    private:

    //queue<MESSAGE> Q0,Q1,Q2;
    ROTQ Q[3];

    //int state[3]; // 0 free, 1 write, 2 read
    int readbuff, writebuff, freebuff;

    public:
    
    int DIM;
    pthread_mutex_t readmu;
    pthread_mutex_t writemu;

    pthread_mutex_t mudim;
    pthread_mutex_t condvarmutex;
    pthread_cond_t condvar;

    SPINB(void);
    void put(MESSAGE);
    MESSAGE get(void);
    void move(void);
    bool isEmpty(void);
};
///////////////////////////////////////////////////////////////////////////////
/*
template<class MESS> class SPINBTMP;
template<class MESS> class ROTQTMP {
    private:
        MESS Q[ARR];
        int top,bot; // da scrivere, da leggere 
        int state;
        SPINBTMP *sb;

    public:
        ROTQTMP(void);
        void setSpinb(SPINBTMP *);
        void setState(int);
        int getState(void);
        void put(MESS);
        MESS get(void);
        bool isEmpty(void);
};
///////////////////////////////////////////////////////////////////////////////
template<class MESS> class SPINBTMP {
    private:

    //queue<MESSAGE> Q0,Q1,Q2;
    ROTQTMP Q[3];

    //int state[3]; // 0 free, 1 write, 2 read
    int readbuff, writebuff, freebuff;

    public:
    
    int DIM;
    pthread_mutex_t readmu;
    pthread_mutex_t writemu;

    pthread_mutex_t mudim;
    pthread_mutex_t condvarmutex;
    pthread_cond_t condvar;

    SPINBTMP(void);
    void put(MESS);
    MESS get(void);
    void move(void);
    bool isEmpty(void);
};
*/
//
