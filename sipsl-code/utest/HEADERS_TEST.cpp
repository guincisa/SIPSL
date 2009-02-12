#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <map>
#include <vector>
#include <string>
#include "CS_HEADERS.h"
#include "P_HEADERS.h"
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stack>
#include "MESSAGE.h"

#define NEWT cout << "##############################################\nNEW TEST " << __LINE__ << "\n##############################################"<<endl;
#define NEWS cout << "\n* * * * * * * * * * * * * * * * * * * * * * *\n NEW SUITE\n* * * * * * * * * * * * * * * * * * * * * * *\n" << endl;

void subtest_C_AttVia(C_AttVia &s, string protocol,
	string version,
	string transport,
	string host,
	string port,
	string pL1,
        string pR1,
	string pL2,
	string pR2) {

	string via = protocol + "/" + version + "/" + transport + " " + host + ":" + port + ";" + pL1 + "=" + pR1 + ";" + pL2 + "=" +pR2;


	cout << "Att Via [" + via  + "]"  <<endl;
        cout << "Att Via [" + s.getContent()  + "]"  <<endl;
        assert(!s.getContent().compare(via));

        cout << "version [" + s.getS_AttSipVersion().getVersion() + "]" <<endl;
        assert(!s.getS_AttSipVersion().getVersion().compare(version));

        cout << "protocol [" + s.getS_AttSipVersion().getProtocol() + "]" <<endl;
        assert(!s.getS_AttSipVersion().getProtocol().compare(protocol));

        cout << "transport [" + s.getTransport() + "]" <<endl;
        assert(!s.getTransport().compare(transport));

        cout << "host [" + s.getS_HostHostPort().getHostName() + "]" <<endl;
        assert(!s.getS_HostHostPort().getHostName().compare(host));

        cout << "port [" << s.getS_HostHostPort().getPort() << "]" <<endl;
        assert(!(s.getS_HostHostPort().getPort()-atoi(port.c_str())));

        cout << "parms [" + s.getViaParms().getContent() + "]" <<endl;
        assert(!s.getViaParms().getContent().compare(pL1 + "=" + pR1 + ";" + pL2 + "=" +pR2));

        cout << pL1 + "[" + s.getViaParms().findRvalue(pL1) + "]" <<endl;
        assert(!s.getViaParms().findRvalue(pL1).compare(pR1));

        cout << pL2 + "[" + s.getViaParms().findRvalue(pL2) + "]" <<endl;
        assert(!s.getViaParms().findRvalue(pL2).compare(pR2));
}

int main(void) {

    cout << "Begin HEADERS test" << endl << flush;

    NEWS
    {
        cout << "Begin brkin2 test" << endl << flush;
        NEWT
        string ss = "aaa;bbb";
        Tuple s = brkin2(ss, ";");
        cout << ss << " left " << s.Lvalue << " right " << s.Rvalue << endl;
        assert(!s.Lvalue.compare("aaa"));
        assert(!s.Rvalue.compare("bbb"));
    }
    {
        NEWT
        string ss = "aaa;";
        Tuple s = brkin2(ss, ";");
        cout << ss << " left " << s.Lvalue << " right " << s.Rvalue << endl;
        assert(!s.Lvalue.compare("aaa"));
        assert(!s.Rvalue.compare(""));
    }
    {
        NEWT
        string ss = "aaa";
        Tuple s = brkin2(ss, ";");
        cout << ss << " left " << s.Lvalue << " right " << s.Rvalue << endl;
        assert(!s.Lvalue.compare("aaa"));
        assert(!s.Rvalue.compare(""));
    }
    {
        NEWT
        string ss = ";aaa";
        Tuple s = brkin2(ss, ";");
        cout << ss << " left " << s.Lvalue << " right " << s.Rvalue << endl;
        assert(!s.Lvalue.compare(""));
        assert(!s.Rvalue.compare("aaa"));
    }


    {
    NEWS
    cout << "Begin TupleVector test" << endl << flush;
    cout << "Sunny Day" << endl << flush;

    string s1 = "aa=bb";
    string s2 = "aa=bb,cc=dd";
    string s3 = "aa=bb,cc=dd,ee=ff";
    string s4 = "*aa=bb";
    string s5 = "*aa=bb,cc=dd";
    string s6 = "*aa=bb,cc=dd,ee=ff";


    NEWT
    cout << "utest 1 [" << s1 << "]" << endl << flush;
    TupleVector tv1(s1,",");
    cout << "find R value for aa [" << tv1.findRvalue("aa") <<"]"<< endl << flush;
    assert(!tv1.findRvalue("aa").compare("bb"));

    NEWT
    TupleVector tv2(s2,",");
    cout << "utest 2 [" << s2 << "]" << endl << flush;
    cout << "find R value for aa [" << tv2.findRvalue("aa") <<"]"<< endl << flush;
    assert(!tv2.findRvalue("aa").compare("bb"));
    cout << "find R value for cc [" << tv2.findRvalue("cc") <<"]"<< endl << flush;
    assert(!tv2.findRvalue("cc").compare("dd"));


    NEWT
    TupleVector tv3(s3,",");
    cout << "utest 3 [" << s3 << "]" << endl << flush;
    cout << "find R value for aa [" << tv3.findRvalue("aa") <<"]"<< endl << flush;
    assert(!tv3.findRvalue("aa").compare("bb"));
    cout << "find R value for cc [" << tv3.findRvalue("cc") <<"]"<< endl << flush;
    assert(!tv3.findRvalue("cc").compare("dd"));
    cout << "find R value for ee [" << tv3.findRvalue("ee") <<"]"<< endl << flush;
    assert(!tv3.findRvalue("ee").compare("ff"));


    NEWT
    TupleVector tv4(s4,",","*");
    cout << "utest 4 [" << s4 << "]" << endl << flush;
    cout << "find R value for aa [" << tv4.findRvalue("aa") <<"]"<< endl << flush;
    assert(!tv4.findRvalue("aa").compare("bb"));

    NEWT
    TupleVector tv5(s5,",","*");
    cout << "utest 5 [" << s5 << "]" << endl << flush;
    cout << "find R value for aa [" << tv5.findRvalue("aa") <<"]"<< endl << flush;
    assert(!tv5.findRvalue("aa").compare("bb"));
    cout << "find R value for cc [" << tv5.findRvalue("cc") <<"]"<< endl << flush;
    assert(!tv5.findRvalue("cc").compare("dd"));

    NEWT
    TupleVector tv6(s6,",","*");
    cout << "utest 6 [" << s6 << "]" << endl << flush;
    cout << "find R value for aa [" << tv6.findRvalue("aa") <<"]"<< endl << flush;
    assert(!tv6.findRvalue("aa").compare("bb"));
    cout << "find R value for cc [" << tv6.findRvalue("cc") <<"]"<< endl << flush;
    assert(!tv6.findRvalue("cc").compare("dd"));
    cout << "find R value for ee [" << tv6.findRvalue("ee") <<"]"<< endl << flush;
    assert(!tv6.findRvalue("ee").compare("ff"));
    }

    NEWS
    {
    cout << "Begin S_AttMethod test" << endl << flush;
    cout << "Sunny Day" << endl << flush;
    NEWT
    S_AttMethod s1("INVITE");
    cout << "INVITE getMethodID [" << s1.getMethodID()<<"] getMethodName [" << s1.getMethodName() <<"]"<<endl;
    assert(!(s1.getMethodID() - INVITE_REQUEST));
    assert(!s1.getMethodName().compare("INVITE"));
    NEWT
    S_AttMethod s2("BYE");
    cout << "BYE getMethodID [" << s2.getMethodID()<<"] getMethodName [" << s2.getMethodName() <<"]"<<endl;
    assert(!(s2.getMethodID() - BYE_REQUEST));
    assert(!s2.getMethodName().compare("BYE"));
    NEWT
    S_AttMethod s3("CANCEL");
    cout << "CANCEL getMethodID [" << s3.getMethodID()<<"] getMethodName [" << s3.getMethodName() <<"]"<<endl;
    assert(!(s3.getMethodID() - CANCEL_REQUEST));
    assert(!s3.getMethodName().compare("CANCEL"));
    NEWT
    S_AttMethod s4("ACK");
    cout << "ACK getMethodID [" << s4.getMethodID()<<"] getMethodName [" << s4.getMethodName() <<"]"<<endl;
    assert(!(s4.getMethodID() - ACK_REQUEST));
    assert(!s4.getMethodName().compare("ACK"));

    cout << "Rainy Day" << endl << flush;
    NEWT
    S_AttMethod s5("INVI");
    cout << "INVI getMethodID [" << s5.getMethodID()<<"] getMethodName [" << s5.getMethodName() <<"]"<<endl;
    assert(!(s5.getMethodID()));
    assert(s5.getMethodName().compare("INVITE"));
    assert(s5.getMethodName().compare("CANCEL"));
    assert(s5.getMethodName().compare("ACK"));
    assert(s5.getMethodName().compare("BYE"));
    NEWT
    S_AttMethod s6("");
    cout << "empty getMethodID [" << s6.getMethodID()<<"] getMethodName [" << s6.getMethodName() <<"]"<<endl;
    assert(!(s6.getMethodID()));
    assert(s6.getMethodName().compare("INVITE"));
    assert(s6.getMethodName().compare("CANCEL"));
    assert(s6.getMethodName().compare("ACK"));
    assert(s6.getMethodName().compare("BYE"));
    }

    NEWS
    {
    cout << "Begin S_AttReply test" << endl << flush;
    cout << "Sunny Day" << endl << flush;
    NEWT
    S_AttReply s1("180 RINGING");
    cout << "180 RINGING getCode [" << s1.getCode()<<"] getReplyID [" << s1.getReplyID() <<"]"<<endl;
    assert(!(s1.getCode() - 180));
    assert(!s1.getReplyID() - RINGING_RESPONSE);
    NEWT
    S_AttReply s2("RINGING", "180");
    cout << "180 and RINGING getCode [" << s2.getCode()<<"] getReplyID [" << s2.getReplyID() <<"]"<<endl;
    assert(!(s2.getCode() - 180));
    assert(!s2.getReplyID() - RINGING_RESPONSE);
    }
    {
    NEWT
    S_AttReply s1("200 OK");
    cout << "200 OK getCode [" << s1.getCode()<<"] getReplyID [" << s1.getReplyID() <<"]"<<endl;
    assert(!(s1.getCode() - 200));
    assert(!s1.getReplyID() - OK_RESPONSE);
    NEWT
    S_AttReply s2("OK", "200");
    cout << "200 and OK getCode [" << s2.getCode()<<"] getReplyID [" << s2.getReplyID() <<"]"<<endl;
    assert(!(s2.getCode() - 200));
    assert(!s2.getReplyID() - OK_RESPONSE);
    }
    {
    NEWT
    S_AttReply s1("100 TRY");
    cout << "100 TRY getCode [" << s1.getCode()<<"] getReplyID [" << s1.getReplyID() <<"]"<<endl;
    assert(!(s1.getCode() - 100));
    assert(!s1.getReplyID() - RINGING_RESPONSE);
    NEWT
    S_AttReply s2("TRY", "100");
    cout << "100 and TRY getCode [" << s2.getCode()<<"] getReplyID [" << s2.getReplyID() <<"]"<<endl;
    assert(!(s2.getCode() - 100));
    assert(!s2.getReplyID() - TRY_RESPONSE);
    }

    {
    NEWS
    cout << "Begin S_SipVersion test" << endl << flush;
    cout << "Sunny Day" << endl << flush;
    }
    {
    NEWT
    S_AttSipVersion s1("SIP/2.0");
    cout << "SIP/2.0 getProtocol [" << s1.getProtocol()<<"] getVersion [" << s1.getVersion() <<"]"<<endl;
    assert(!(s1.getProtocol().compare("SIP")));
    assert(!s1.getVersion().compare("2.0"));
    }
    {
    NEWT
    S_AttSipVersion s1("SIP","2.0");
    cout << "SIP and 2.0 getProtocol [" << s1.getProtocol()<<"] getVersion [" << s1.getVersion() <<"]"<<endl;
    assert(!(s1.getProtocol().compare("SIP")));
    assert(!s1.getVersion().compare("2.0"));
    }

    NEWS
    {
    //C_AttSipUri
    cout << "Begin C_AttSipUri test" << endl << flush;
    cout << "Sunny Day" << endl << flush;
    string sipuri1 = "alice:secretword";
    string sipuri2 = "atlanta.com:4040";
    string sipuri3 = "transport=tcp;ttl=15";
    string sipuri4 = "to=alice%40atalnta.com&priority=urgent";
    {
        NEWT
        string sipuri = "sip:" + sipuri1 + "@" + sipuri2 + ";" + sipuri3 + "?" + sipuri4;
        C_AttSipUri s(sipuri);
        cout << "SipUri " + sipuri <<endl;
        cout << "IsSecure " + s.getIsSec()<<endl;
        assert(!s.getIsSec());
        cout << "UserInfo [" + s.getS_AttUserInfo().getContent() + "]" <<endl;
        assert(!s.getS_AttUserInfo().getContent().compare(sipuri1));

        cout << "HostPort [" + s.getS_AttHostPort().getContent() + "]" <<endl;
        assert(!s.getS_AttHostPort().getContent().compare(sipuri2));

        cout << "UriParms [" + s.getC_AttUriParms().getContent() + "]" <<endl;
        assert(!s.getC_AttUriParms().getContent().compare(sipuri3));

        cout << "UriHeads [" + s.getC_AttUriHeads().getContent() + "]" <<endl;
        assert(!s.getC_AttUriHeads().getContent().compare(sipuri4));
    }
    {
        NEWT
        sipuri1 = "alice";
        string sipuri = "sip:" + sipuri1 + "@" + sipuri2 + ";" + sipuri3 + "?" + sipuri4;
        C_AttSipUri s(sipuri);
        cout << "SipUri " + sipuri <<endl;
        cout << "IsSecure " + s.getIsSec()<<endl;
        assert(!s.getIsSec());
        cout << "UserInfo [" + s.getS_AttUserInfo().getContent() + "]" <<endl;
        assert(!s.getS_AttUserInfo().getContent().compare(sipuri1));
        cout << "HostPort [" + s.getS_AttHostPort().getContent() + "]" <<endl;
        assert(!s.getS_AttHostPort().getContent().compare(sipuri2));
        cout << "UriParms [" + s.getC_AttUriParms().getContent() + "]" <<endl;
        assert(!s.getC_AttUriParms().getContent().compare(sipuri3));
        cout << "UriHeads [" + s.getC_AttUriHeads().getContent() + "]" <<endl;
        assert(!s.getC_AttUriHeads().getContent().compare(sipuri4));
    }
    {
        NEWT
        sipuri2 = "atlanta.com";
        string sipuri = "sip:" + sipuri1 + "@" + sipuri2 + ";" + sipuri3 +"?" + sipuri4;
        C_AttSipUri s(sipuri);
        cout << "SipUri " + sipuri <<endl;
        cout << "IsSecure " + s.getIsSec()<<endl;
        assert(!s.getIsSec());
        cout << "UserInfo [" + s.getS_AttUserInfo().getContent() + "]" <<endl;
        assert(!s.getS_AttUserInfo().getContent().compare(sipuri1));
        cout << "HostPort [" + s.getS_AttHostPort().getContent() + "]" <<endl;
        assert(!s.getS_AttHostPort().getContent().compare(sipuri2));
        cout << "UriParms [" + s.getC_AttUriParms().getContent() + "]" <<endl;
        assert(!s.getC_AttUriParms().getContent().compare(sipuri3));
        cout << "UriHeads [" + s.getC_AttUriHeads().getContent() + "]" <<endl;
        assert(!s.getC_AttUriHeads().getContent().compare(sipuri4));
    }
    {
        NEWT
        sipuri4 = "";
        string sipuri = "sip:" + sipuri1 + "@" + sipuri2 + ";" + sipuri3 ;
        C_AttSipUri s(sipuri);
        cout << "SipUri " + sipuri <<endl;
        cout << "IsSecure " + s.getIsSec()<<endl;
        assert(!s.getIsSec());
        cout << "UserInfo [" + s.getS_AttUserInfo().getContent() + "]" <<endl;
        assert(!s.getS_AttUserInfo().getContent().compare(sipuri1));
        cout << "HostPort [" + s.getS_AttHostPort().getContent() + "]" <<endl;
        assert(!s.getS_AttHostPort().getContent().compare(sipuri2));
        cout << "UriParms [" + s.getC_AttUriParms().getContent() + "]" <<endl;
        assert(!s.getC_AttUriParms().getContent().compare(sipuri3));
        cout << "UriHeads [" + s.getC_AttUriHeads().getContent() + "]" <<endl;
        assert(!s.getC_AttUriHeads().getContent().compare(sipuri4));
    }
    {
        NEWT
        sipuri3 = "";
        sipuri4 = "";
        string sipuri = "sip:" + sipuri1 + "@" + sipuri2 ;
        C_AttSipUri s(sipuri);
        cout << "SipUri " + sipuri <<endl;
        cout << "IsSecure " + s.getIsSec()<<endl;
        assert(!s.getIsSec());
        cout << "UserInfo [" + s.getS_AttUserInfo().getContent() + "]" <<endl;
        assert(!s.getS_AttUserInfo().getContent().compare(sipuri1));
        cout << "HostPort [" + s.getS_AttHostPort().getContent() + "]" <<endl;
        assert(!s.getS_AttHostPort().getContent().compare(sipuri2));
        cout << "UriParms [" + s.getC_AttUriParms().getContent() + "]" <<endl;
        assert(!s.getC_AttUriParms().getContent().compare(sipuri3));
        cout << "UriHeads [" + s.getC_AttUriHeads().getContent() + "]" <<endl;
        assert(!s.getC_AttUriHeads().getContent().compare(sipuri4));
    }
    }
    NEWS
    {
    //S_AttUserInfo
    cout << "Begin C_AttUserInfo test" << endl << flush;
    cout << "Sunny Day" << endl << flush;
    string sipuri1 = "alice";
    string sipuri2 = "passwx";
    {
        NEWT
        S_AttUserInfo s(sipuri1 + ":" + sipuri2);
        cout << "UserInfo [" + sipuri1 + ":" + sipuri2 + "]"  <<endl;
        cout << "User [" + s.getUserName() + "]" <<endl;
        assert(!s.getUserName().compare(sipuri1));
        cout << "Password [" + s.getPassword() + "]" <<endl;
        assert(!s.getPassword().compare(sipuri2));
    }
    {
        NEWT
        string sipuri2 = "";
        S_AttUserInfo s(sipuri1);
        cout << "UserInfo [" + sipuri1 + "]"  <<endl;
        cout << "User [" + s.getUserName() + "]" <<endl;
        assert(!s.getUserName().compare(sipuri1));
        cout << "Password [" + s.getPassword() + "]" <<endl;
        assert(!s.getPassword().compare(sipuri2));
    }

    }
    NEWS
    {
    //S_AttHostPort
    cout << "Begin S_AttHostPort" << endl << flush;
    cout << "Sunny Day" << endl << flush;
    string sipuri1 = "gugli.net";
    string sipuri2 = "1234";
    {
        NEWT
        S_AttHostPort s(sipuri1 + ":" + sipuri2);
        cout << "HostPort [" + sipuri1 + ":" + sipuri2 + "]"  <<endl;
        cout << "Host [" + s.getHostName() + "]" <<endl;
        assert(!s.getHostName().compare(sipuri1));
        cout << "Port [" << s.getPort() << "]" <<endl;
        assert(!(s.getPort()- 1234));
    }
    {
        NEWT
        string sipuri2 = "";
        S_AttHostPort s(sipuri1 );
        cout << "HostPort [" + sipuri1  + "]"  <<endl;
        cout << "Host [" + s.getHostName() + "]" <<endl;
        assert(!s.getHostName().compare(sipuri1));
        cout << "Port [" << s.getPort() << "]" <<endl;
        assert(!(s.getPort()));
    }

    }
    NEWS
    {
    //C_AttUriParms
    cout << "Begin C_UriParms" << endl << flush;
    cout << "Sunny Day" << endl << flush;
    NEWT
    {
        string sipuri = "transport=tcp;ttl=15;name=ciccio;age=2";
        C_AttUriParms s(sipuri );
        cout << "UriParms [" + sipuri  + "]"  <<endl;
        cout << "transport [" + s.getTuples().findRvalue("transport") + "]" <<endl;
        assert(!s.getTuples().findRvalue("transport").compare("tcp"));

        cout << "ttl [" + s.getTuples().findRvalue("ttl") + "]" <<endl;
        assert(!s.getTuples().findRvalue("ttl").compare("15"));

        cout << "name [" + s.getTuples().findRvalue("name") + "]" <<endl;
        assert(!s.getTuples().findRvalue("name").compare("ciccio"));
        cout << "age [" + s.getTuples().findRvalue("age") + "]" <<endl;
        assert(!s.getTuples().findRvalue("age").compare("2"));
        cout << "sex [" + s.getTuples().findRvalue("sex") + "]" <<endl;
        assert(!s.getTuples().findRvalue("sex").compare(""));
    }

    NEWT
    {
        string sipuri = "branch=z9hG4bK74b76;received=192.0.2.101";
        C_AttUriParms s(sipuri );
        cout << "UriParms [" + sipuri  + "]"  <<endl;
        cout << "received [" + s.getTuples().findRvalue("received") + "]" <<endl;
        assert(!s.getTuples().findRvalue("received").compare("192.0.2.101"));
        cout << "branch [" + s.getTuples().findRvalue("branch") + "]" <<endl;
        assert(!s.getTuples().findRvalue("branch").compare("z9hG4bK74b76"));
    }
    NEWT
    {
        string sipuri = "branch=z9hG4bK74b76; received=192.0.2.101";
        C_AttUriParms s(sipuri );
        cout << "UriParms [" + sipuri  + "]"  <<endl;
        cout << "received [" + s.getTuples().findRvalue("received") + "]" <<endl;
        assert(!s.getTuples().findRvalue("received").compare("192.0.2.101"));
        cout << "branch [" + s.getTuples().findRvalue("branch") + "]" <<endl;
        assert(!s.getTuples().findRvalue("branch").compare("z9hG4bK74b76"));
    }
    }
    NEWS
    {
    //C_AttUriHeads
    cout << "Begin C_UriHeads" << endl << flush;
    cout << "Sunny Day" << endl << flush;
    NEWT
    {
        string sipuri = "to=alice%40atalnta.com&priority=urgent";
        C_AttUriHeaders s(sipuri );
        cout << "UriHeads [" + sipuri  + "]"  <<endl;
        cout << "to [" + s.getTuples().findRvalue("to") + "]" <<endl;
        assert(!s.getTuples().findRvalue("to").compare("alice%40atalnta.com"));
        cout << "priority [" + s.getTuples().findRvalue("priority") + "]" <<endl;
        assert(!s.getTuples().findRvalue("priority").compare("urgent"));
        cout << "sex [" + s.getTuples().findRvalue("sex") + "]" <<endl;
        assert(!s.getTuples().findRvalue("sex").compare(""));
    }
    }
    NEWS
    {
    	//C_AttVia
	//string via = "SIP/2.0/TCP client.atlanta.example.com:5060;branch=z9hG4bK74b76;received=192.0.2.101";
	string s1="SIP";
	string s2="2.0";
	string s3="TCP";
	string s4="client.atlanta.example.com";
	string s5="5060";
	string s6="branch";
	string s7="z9hG4bK74b76";
	string s8="received";
	string s9="192.0.2.101";
	string tot = s1 + "/" + s2 + "/" + s3 + " " + s4 + ":" + s5 + ";" + s6 + "=" + s7 + ";" + s8 + "=" +s9;
	C_AttVia s(tot);
        cout <<"passed " << __LINE__ << endl;

    	subtest_C_AttVia(s,s1,s2,s3,s4,s5,s6,s7,s8,s9);
    }
    NEWS
    {
    //S_AttReply
    cout << "Begin S_AttReply" << endl << flush;
    cout << "Sunny Day" << endl << flush;
    NEWT
    {
        string r1 = "200";
        int val = 200;
        string r2 = "OK";
        S_AttReply s(r1+" "+r2);
        cout << "Att Reply [" + s.getContent()  + "]"  <<endl;
        assert(!s.getContent().compare(r1+" "+r2));

        cout << "Code [" << s.getCode() << "]" <<endl;
        assert(!(s.getCode()-val));

        cout << "reply id [" << s.getReplyID() << "]" <<endl;
        assert(!s.getReplyID()- 53);

        cout << "reply [" + s.getReply() + "]" <<endl;
        assert(!s.getReply().compare(r2));
    }
    }
    NEWS
    {
    {
        NEWT
        string r1 = "INVITE";
        int invite = 2;
        string r2 = "gi4";
        string pass = "";
        string r3 = "lucent.com";
        string r4 = "SIP";
        string r5 = "2.0";
        string up = "";
        string uh = "";
        string req = r1+" sip:"+r2+"@"+r3 +" "+r4+"/"+r5;
        C_HeadSipRequest rs(req, 1);
        cout << "Request [" << req <<"]" <<endl;
        cout << "Request [" << rs.getContent() <<"]" <<endl;
        assert(!(rs.getContent().compare(req)));

        cout << "Method [" << rs.getS_AttMethod().getMethodID() << "]" << endl;
        assert(!(rs.getS_AttMethod().getMethodID() - invite));

        cout << "MethodName [" << rs.getS_AttMethod().getMethodName() << "]" << endl;
        assert(!(rs.getS_AttMethod().getMethodName().compare(r1)));

        cout << "SipUri username[" << rs.getC_AttSipUri().getS_AttUserInfo().getUserName() << "]" << endl;
        assert(!(rs.getC_AttSipUri().getS_AttUserInfo().getUserName().compare(r2)));

        cout << "SipUri password[" << rs.getC_AttSipUri().getS_AttUserInfo().getPassword() << "]" << endl;
        assert(!(rs.getC_AttSipUri().getS_AttUserInfo().getPassword().compare(pass)));

        cout << "HostPort host[" << rs.getC_AttSipUri().getS_AttHostPort().getHostName() << "]" << endl;
        assert(!(rs.getC_AttSipUri().getS_AttHostPort().getHostName().compare(r3)));

        cout << "HostPort port[" << rs.getC_AttSipUri().getS_AttHostPort().getPort() << "]" << endl;
        assert(!(rs.getC_AttSipUri().getS_AttHostPort().getPort()));

        cout << "Sip version [" << rs.getS_AttSipVersion().getProtocol() << "]" << endl;
        assert(!(rs.getS_AttSipVersion().getProtocol().compare(r4)));

        cout << "Sip version [" << rs.getS_AttSipVersion().getVersion() << "]" << endl;
        assert(!(rs.getS_AttSipVersion().getVersion().compare(r5)));

        cout << "UriParms[" << rs.getC_AttSipUri().getC_AttUriParms().getContent() << "]" << endl;
        assert(!(rs.getC_AttSipUri().getC_AttUriParms().getContent().compare(up)));
        cout << "UriHeads[" << rs.getC_AttSipUri().getC_AttUriHeads().getContent() << "]" << endl;
        assert(!(rs.getC_AttSipUri().getC_AttUriHeads().getContent().compare(uh)));
    }

    }
    NEWS
    {
    {
        NEWT
        string r1 = "SIP";
        string r2 = "2.0";
        string r3 = "200";
        int val = 200;
        string r4 = "OK";

        string req = r1+"/"+r2+" "+r3 +" "+r4;
        C_HeadSipReply rs(req, 1);
        cout << "Reply [" << req <<"]" <<endl;
        cout << "Reply [" << rs.getContent() <<"]" <<endl;
        assert(!(rs.getContent().compare(req)));

        cout << "Version [" << rs.getSipVersion().getVersion() << "]" << endl;
        assert(!(rs.getSipVersion().getVersion().compare(r2)));

        cout << "Protocol [" << rs.getSipVersion().getProtocol() << "]" << endl;
        assert(!(rs.getSipVersion().getProtocol().compare(r1)));

        cout << "Code [" << rs.getReply().getCode() << "]" <<endl;
        assert(!(rs.getReply().getCode()-val));

        cout << "reply id [" << rs.getReply().getReplyID() << "]" <<endl;
        assert(!rs.getReply().getReplyID()- 53);

        cout << "reply [" + rs.getReply().getReply() + "]" <<endl;
        assert(!rs.getReply().getReply().compare(r4));
    }

    }
    NEWS
    {
        NEWT
	//C_HeadVia
	string s1="SIP";
	string s2="2.0";
	string s3="TCP";
	string s4="client.atlanta.example.com";
	string s5="5060";
	string s6="branch";
	string s7="z9hG4bK74b76";
	string s8="received";
	string s9="192.0.2.101";
	string tot = "Via: " + s1 + "/" + s2 + "/" + s3 + " " + s4 + ":" + s5 + ";" + s6 + "=" + s7 + ";" + s8 + "=" +s9;
	C_HeadVia s(tot,1,1);

        cout << "Head Via [" + s.getContent() + "]" <<endl;
        assert(!s.getContent().compare(tot));

    	subtest_C_AttVia(s.getC_AttVia(),s1,s2,s3,s4,s5,s6,s7,s8,s9);

        cout << "Head Via position [" << s.getPosition() << "]" <<endl;
        assert(!(s.getPosition()-1));

        cout << "Head Via genEntity [" << s.getGenEntity() << "]" <<endl;
        assert(!(s.getGenEntity()-1));

    }
    NEWS
    {
        NEWT
	cout << "S_HeadMaxFwd" << endl;
	string s1="Max-Forwards: 44";
        int mxfwd = 44;
	S_HeadMaxFwd s(s1, 1);

        cout << "Head Max-Forwards [" + s.getContent() + "]" <<endl;
        assert(!s.getContent().compare(s1));

        cout << "Head Max-Forwards [" << s.getMaxFwd() << "]" <<endl;
        assert(!(s.getMaxFwd() - mxfwd));

    }
    NEWS
    {
			NEWT
		//C_AttContactList
		//string s1="\"Mr. Watson\" <sip:watson@worcester.bell-telephone.com>;q=0.7; expires=3600, \"Mr. Watson\" <mailto:watson@uffa.com> ;q=0.1 ";
		string s2 = "\"Mr. Watson\"";
		string s2x = "\"Mr.%20Watson\"";
		string s3 = "<sip:watson@worcester.bell-telephone.com>";
		string s4a ="q=0.7";
		string s4b ="expires=3600";
		string s5 = "<mailto:watson@uffa.com>";
		string s6 = "q=0.1";

		string stot = s2+ " " + s3 + "; " + s4a + "; " + s4b +", " + s2 + " "+ s5 + "; " + s6;
		cout << "To parse " << stot << endl;

		C_AttContactList s(stot);

        cout << "C_AttContactList [" + s.getContent() + "]" <<endl;
        assert(!s.getContent().compare(stot));

        vector<C_AttContactElem> list;
        list = s.getContactList();
        cout << "C_AttContactList element [" + list[0].getContent() + "]" <<endl;
        assert(!list[0].getContent().compare(s2 + " " + s3 + "; " + s4a + "; " + s4b));

        cout << "C_AttContactList element [" + list[1].getContent() + "]" <<endl;
        assert(!list[1].getContent().compare(" " + s2+ " " +s5 + "; " + s6));


		cout << "C_AttContactList name [" + list[0].getNameUri() + "] " + s2x <<endl;
		assert(!list[0].getNameUri().compare(s2x));
		cout << "C_AttContactList getC_AttSipUri [" + list[0].getC_AttSipUri().getContent() + "]" <<endl;
		assert(!list[0].getC_AttSipUri().getContent().compare(s3));
		cout << "C_AttContactList getC_AttUriParms [" + list[0].getC_AttUriParms().getContent() + "]" <<endl;
		assert(!list[0].getC_AttUriParms().getContent().compare(s4a+";"+s4b));

		cout << "C_AttContactList name [" + list[1].getNameUri() + "]" <<endl;
		assert(!list[1].getNameUri().compare(s2x));
		cout << "C_AttContactList getC_AttSipUri [" + list[1].getC_AttSipUri().getContent() + "]" <<endl;
		assert(!list[1].getC_AttSipUri().getContent().compare(s5));
		cout << "C_AttContactList getC_AttUriParms [" + list[1].getC_AttUriParms().getContent() + "]" <<endl;
		assert(!list[1].getC_AttUriParms().getContent().compare(s6));

    }    NEWS
    {
			NEWT
		//C_HEadContact
		//string s1="\"Mr. Watson\" <sip:watson@worcester.bell-telephone.com>;q=0.7; expires=3600, \"Mr. Watson\" <mailto:watson@uffa.com> ;q=0.1 ";
		string s2 = "\"Mr. Watson\"";
		string s2x = "\"Mr.%20Watson\"";
		string s3 = "<sip:watson@worcester.bell-telephone.com>";
		string s4a ="q=0.7";
		string s4b ="expires=3600";
		string s5 = "<mailto:watson@uffa.com>";
		string s6 = "q=0.1";

		string stot = s2+ " " + s3 + "; " + s4a + "; " + s4b +", " + s2 + " "+ s5 + "; " + s6;
		cout << "To parse " << stot << endl;

		C_HeadContact s(stot,1);

        cout << "C_HeadContact [" + s.getContent() + "]" <<endl;
        assert(!s.getContent().compare(stot));


        C_AttContactList &cl = s.getContactList();
        vector<C_AttContactElem> &list = cl.getContactList();

        /*
        cout << "C_HeadContact element [" + list[0].getContent() + "]" <<endl;
        cout << "C_HeadContact element c [" + s2x+  s3 + ";" + s4a + ";" + s4b + "]" <<endl;
        assert(!list[0].getContent().compare(s2x + s3 + ";" + s4a + ";" + s4b));

        cout << "C_HeadContact element [" + list[1].getContent() + "]" <<endl;
        cout << "C_HeadContact element c [" + s2x + s5 + ";" + s6 + "]" <<endl;
        assert(!list[1].getContent().compare(s2x+ s5 + ";" + s6));
        */

		cout << "C_HeadContact name [" + list[0].getNameUri() + "]" <<endl;
		assert(!list[0].getNameUri().compare(s2x));
		cout << "C_HeadContact getC_AttSipUri [" + list[0].getC_AttSipUri().getContent() + "]" <<endl;
		assert(!list[0].getC_AttSipUri().getContent().compare(s3));
		cout << "C_HeadContact getC_AttUriParms [" + list[0].getC_AttUriParms().getContent() + "]" <<endl;
		assert(!list[0].getC_AttUriParms().getContent().compare(s4a+";"+s4b));

		cout << "C_HeadContact name [" + list[1].getNameUri() + "]" <<endl;
		assert(!list[1].getNameUri().compare(s2x));
		cout << "C_HeadContact getC_AttSipUri [" + list[1].getC_AttSipUri().getContent() + "]" <<endl;
		assert(!list[1].getC_AttSipUri().getContent().compare(s5));
		cout << "C_HeadContact getC_AttUriParms [" + list[1].getC_AttUriParms().getContent() + "]" <<endl;
		assert(!list[1].getC_AttUriParms().getContent().compare(s6));

    }
    NEWS
    {
			NEWT{
		//C_HEadTo
		//string s1="\"Mr. Watson\" <sip:watson@worcester.bell-telephone.com>;q=0.7; expires=3600, \"Mr. Watson\" <mailto:watson@uffa.com> ;q=0.1 ";
		string s2 = "\"Mr. Watson\"";
		string s2x = "\"Mr.%20Watson\"";
		string s3 = "<sip:watson@worcester.bell-telephone.com>";
		string s4a ="tag=1234";


		string stot = s2+ " " + s3 + "; " + s4a;
		cout << "To parse " << stot << endl;

		C_HeadTo s(stot,1);

        cout << "C_HeadTo [" + s.getContent() + "]" <<endl;
        assert(!s.getTo().getContent().compare(stot));


        C_AttContactElem &ce = s.getTo();

        /*
        cout << "C_HeadTo element [" + ce.getContent() + "]" <<endl;
        cout << "C_HeadTo element c [" + s2x+  s3 + ";" + s4a + "]" <<endl;
        assert(!ce.getContent().compare(s2x + s3 + ";" + s4a));
        */


		cout << "C_HeadTo name [" + ce.getNameUri() + "]" <<endl;
		assert(!ce.getNameUri().compare(s2x));
		cout << "C_HeadTo getC_AttSipUri [" + ce.getC_AttSipUri().getContent() + "]" <<endl;
		assert(!ce.getC_AttSipUri().getContent().compare(s3));
		cout << "C_HeadTo getC_AttUriParms [" + ce.getC_AttUriParms().getContent() + "]" <<endl;
		assert(!ce.getC_AttUriParms().getContent().compare(s4a));

		}
		NEWT{
			//C_HEadTo
			//string s1="\"Mr. Watson\" <sip:watson@worcester.bell-telephone.com>;q=0.7; expires=3600, \"Mr. Watson\" <mailto:watson@uffa.com> ;q=0.1 ";
			string s2 = "Watson";
			string s3 = "<sip:watson@worcester.bell-telephone.com>";
			string s4a ="tag=1234";


			string stot = s2+ " " + s3 + "; " + s4a;
			cout << "To parse " << stot << endl;

			C_HeadTo s(stot,1);

			cout << "C_HeadTo [" + s.getContent() + "]" <<endl;
	        assert(!s.getTo().getContent().compare(stot));


			C_AttContactElem &ce = s.getTo();


			/*
			cout << "C_HeadTo element [" + ce.getContent() + "]" <<endl;
			assert(!ce.getContent().compare(s2 + s3 + ";" + s4a));
			*/


			cout << "C_HeadTo name [" + ce.getNameUri() + "]" <<endl;
			assert(!ce.getNameUri().compare(s2));
			cout << "C_HeadTo getC_AttSipUri [" + ce.getC_AttSipUri().getContent() + "]" <<endl;
			assert(!ce.getC_AttSipUri().getContent().compare(s3));
			cout << "C_HeadTo getC_AttUriParms [" + ce.getC_AttUriParms().getContent() + "]" <<endl;
			assert(!ce.getC_AttUriParms().getContent().compare(s4a));
		}

    }
    NEWS
    {
			NEWT{
		//C_HEadFrom
		//string s1="\"Mr. Watson\" <sip:watson@worcester.bell-telephone.com>;q=0.7; expires=3600, \"Mr. Watson\" <mailto:watson@uffa.com> ;q=0.1 ";
		string s2 = "\"Mr. Watson\"";
		string s2x = "\"Mr.%20Watson\"";
		string s3 = "<sip:watson@worcester.bell-telephone.com>";
		string s4a ="tag=1234";


		string stot = s2+ " " + s3 + "; " + s4a;
		cout << "To parse " << stot << endl;

		C_HeadFrom s(stot,1);

        cout << "C_HeadFrom [" + s.getContent() + "]" <<endl;
        assert(!s.getFrom().getContent().compare(stot));


        C_AttContactElem &ce = s.getFrom();

        /*
        cout << "C_HeadTo element [" + ce.getContent() + "]" <<endl;
        cout << "C_HeadTo element c [" + s2x+  s3 + ";" + s4a + "]" <<endl;
        assert(!ce.getContent().compare(s2x + s3 + ";" + s4a));
        */


		cout << "C_HeadFrom name [" + ce.getNameUri() + "]" <<endl;
		assert(!ce.getNameUri().compare(s2x));
		cout << "C_HeadFrom getC_AttSipUri [" + ce.getC_AttSipUri().getContent() + "]" <<endl;
		assert(!ce.getC_AttSipUri().getContent().compare(s3));
		cout << "C_HeadFrom getC_AttUriParms [" + ce.getC_AttUriParms().getContent() + "]" <<endl;
		assert(!ce.getC_AttUriParms().getContent().compare(s4a));

		}
		NEWT{
			//C_HEadTo
			//string s1="\"Mr. Watson\" <sip:watson@worcester.bell-telephone.com>;q=0.7; expires=3600, \"Mr. Watson\" <mailto:watson@uffa.com> ;q=0.1 ";
			string s2 = "Watson";
			string s3 = "<sip:watson@worcester.bell-telephone.com>";
			string s4a ="tag=1234";


			string stot = s2+ " " + s3 + "; " + s4a;
			cout << "To parse " << stot << endl;

			C_HeadFrom s(stot,1);

			cout << "C_HeadFrom [" + s.getContent() + "]" <<endl;
	        assert(!s.getFrom().getContent().compare(stot));


			C_AttContactElem &ce = s.getFrom();


			/*
			cout << "C_HeadTo element [" + ce.getContent() + "]" <<endl;
			assert(!ce.getContent().compare(s2 + s3 + ";" + s4a));
			*/


			cout << "C_HeadFrom name [" + ce.getNameUri() + "]" <<endl;
			assert(!ce.getNameUri().compare(s2));
			cout << "C_HeadFrom getC_AttSipUri [" + ce.getC_AttSipUri().getContent() + "]" <<endl;
			assert(!ce.getC_AttSipUri().getContent().compare(s3));
			cout << "C_HeadFrom getC_AttUriParms [" + ce.getC_AttUriParms().getContent() + "]" <<endl;
			assert(!ce.getC_AttUriParms().getContent().compare(s4a));
		}

    }

    NEWS
       {
   			NEWT{
   		//C_HeadCallId
   		//string s1="\"Mr. Watson\" <sip:watson@worcester.bell-telephone.com>;q=0.7; expires=3600, \"Mr. Watson\" <mailto:watson@uffa.com> ;q=0.1 ";
   		string s1 = "53208975320573805738";
   		string s2 = "fjkdjkshfahfjksaslkasdfjkl.com";



      	C_HeadCallId s(s1+"@"+s2,1);

           cout << "C_HeadCallId [" + s.getContent() + "]" <<endl;
           assert(!s.getContent().compare(s1+"@"+s2));
           cout << "C_HeadCallId 1 [" + s.getCallId().Lvalue + "]" <<endl;
           assert(!s.getCallId().Lvalue.compare(s1));
           cout << "C_HeadCallId 2[" + s.getCallId().Rvalue + "]" <<endl;
           assert(!s.getCallId().Rvalue.compare(s2));

   		}
   			NEWT{
   		//C_HeadCallId
   		//string s1="\"Mr. Watson\" <sip:watson@worcester.bell-telephone.com>;q=0.7; expires=3600, \"Mr. Watson\" <mailto:watson@uffa.com> ;q=0.1 ";
   		string s1 = "53208975320573805738";
   		string s2 = "";



   		C_HeadCallId s(s1,1);

           cout << "C_HeadCallId [" + s.getContent() + "]" <<endl;
           assert(!s.getContent().compare(s1));
           cout << "C_HeadCallId 1 [" + s.getCallId().Lvalue + "]" <<endl;
           assert(!s.getCallId().Lvalue.compare(s1));
           cout << "C_HeadCallId 2[" + s.getCallId().Rvalue + "]" <<endl;
           assert(!s.getCallId().Rvalue.compare(s2));

   		}
       }
    NEWS
           {
       			NEWT{
       		//C_HeadCSeq
       		//string s1="\"Mr. Watson\" <sip:watson@worcester.bell-telephone.com>;q=0.7; expires=3600, \"Mr. Watson\" <mailto:watson@uffa.com> ;q=0.1 ";
       		string s1 = "3";
       		string s2 = "INVITE";



       		C_HeadCSeq s(s1+" "+s2,1);

               cout << "C_HeadCSeq [" + s.getContent() + "]" <<endl;
               assert(!s.getContent().compare(s1+" "+s2));
               cout << "C_HeadCSeq 1 [" << s.getSequence() << "]" <<endl;
               assert(!(s.getSequence()-3));
               cout << "C_HeadCSeq 2[" + s.getMethod().getMethodName() + "]" <<endl;
               assert(!s.getMethod().getMethodName().compare(s2));

       		}
           }


    NEWS{
    	NEWT{
    		string messaggio ="aaaa bbbb\ncc dd\neeeee\nuffa";

    		SysTime mytime;
    		GETTIME(mytime);
    		BASEMESSAGE bm(messaggio, mytime);

    		cout << "Mess [" << messaggio <<"]" << endl;
    		cout << "Tot lines " << bm.getTotLines() << endl;
    		cout << "Get line [" << bm.getLine(0) <<"]" <<endl;
    		cout << "Get line [" << bm.getLine(1) <<"]" <<endl;
    		cout << "Get line [" << bm.getLine(2) <<"]" <<endl;
    		cout << "Get line [" << bm.getLine(3) <<"]" <<endl;
    		cout << "Get line [" << bm.getLine(4) <<"]" <<endl;
    	}
    	NEWT{
    		string messaggio ="aaaa bbbb\ncc dd\neeeee\nuffa\n";

    		SysTime mytime;
    		GETTIME(mytime);
    		BASEMESSAGE bm(messaggio, mytime);

    		cout << "Mess [" << messaggio <<"]" << endl;
    		cout << "Tot lines " << bm.getTotLines() << endl;
    		cout << "Get line [" << bm.getLine(0) <<"]" <<endl;
    		cout << "Get line [" << bm.getLine(1) <<"]" <<endl;
    		cout << "Get line [" << bm.getLine(2) <<"]" <<endl;
    		cout << "Get line [" << bm.getLine(3) <<"]" <<endl;
    		cout << "Get line [" << bm.getLine(4) <<"]" <<endl;
    	}
    	NEWT{
			string messaggio ="INVITE sip:gugli@lucent.com SIP/2.0\nVia: SIP/2.0/UDP 192.168.0.3:6003;rport;branch=z9hG4bKykvnbfuj\nMax-Forwards: 70\nTo: <sip:gugli@lucent.com>\nFrom: \"Ciccio\" <sip:ciccio@es.atosorigin.com>;tag=qcirp\nCall-ID: tgucxgkqjhkdltj@192.168.0.3\nCSeq: 134 INVITE\nContact: <sip:ciccio@192.168.0.3:6003>\nContent-Type: application/sdp\nAllow: INVITE,ACK,BYE,CANCEL,OPTIONS,PRACK,REFER,NOTIFY,SUBSCRIBE,INFO,MESSAGE\nOrganization: ATOS ORIGIN\nSubject: ciao\nSupported: replaces,norefersub,100rel\nUser-Agent: Twinkle/1.1\nContent-Length: 306\n\nv=0\no=ciccio 1320585760 1779093879 IN IP4 192.168.0.3\ns=-\nc=IN IP4 192.168.0.3\nt=0 0\nm=audio 8000 RTP/AVP 98 97 8 0 3 101\na=rtpmap:98 speex/16000\na=rtpmap:97 speex/8000\na=rtpmap:8 PCMA/8000\na=rtpmap:0 PCMU/8000\na=rtpmap:3 GSM/8000\na=rtpmap:101 telephone-event/8000\na=fmtp:101 0-15\na=ptime:20";
    		SysTime mytime;
    		GETTIME(mytime);
    		BASEMESSAGE bm(messaggio, mytime);

    		cout << "Mess [" << messaggio <<"]" << endl;
    		cout << "Tot lines " << bm.getTotLines() << endl;
    		int i ;
    		for(i = 0; i <60; i++){
    			cout << "Get line [" << bm.getLine(i) <<"]" <<endl;
    		}

    	}
    }
    return 0;
}


