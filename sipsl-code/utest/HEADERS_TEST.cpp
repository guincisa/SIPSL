#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <map>
#include <vector>
#include "CS_HEADERS.h"
#include "P_HEADERS.h"
#include <assert.h>

#define NEWT cout << "##############################################\nNEW TEST\n##############################################"<<endl;
#define NEWS cout << "\n* * * * * * * * * * * * * * * * * * * * * * *\n NEW SUITE\n* * * * * * * * * * * * * * * * * * * * * * *\n" << endl;
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
    cout << "Begin C_AttVia" << endl << flush;
    cout << "Sunny Day" << endl << flush;
    NEWT
    {
        string via = "SIP/2.0/TCP client.atlanta.example.com:5060;branch=z9hG4bK74b76;received=192.0.2.101";
        string version = "SIP/20";
        string transport = "TCP";
        string hostp = "client.atlanta.example.com:5060";
        string params = "branch=z9hG4bK74b76;received=192.0.2.101";
        C_AttVia s(version + "/" + transport + " " + hostp+";"+params );
        cout << "Att Via [" + s.getContent()  + "]"  <<endl;
        cout << "version [" + s.getS_AttSipVersion().getVersion() + "]" <<endl;
        assert(!s.getS_AttSipVersion().getVersion().compare("2.0"));
        cout << "protocol [" + s.getS_AttSipVersion().getProtocol() + "]" <<endl;
        assert(!s.getS_AttSipVersion().getProtocol().compare("SIP"));
        cout << "transport [" + s.getTransport() + "]" <<endl;
        assert(!s.getTransport().compare("TCP"));
        cout << "host [" + s.getS_HostHostPort().getHostName() + "]" <<endl;
        assert(!s.getS_HostHostPort().getHostName().compare("client.atlanta.example.com"));
        cout << "port [" << s.getS_HostHostPort().getPort() << "]" <<endl;
        assert(!(s.getS_HostHostPort().getPort()-5060));
        cout << "parms [" + s.getViaParms().getContent() + "]" <<endl;
        assert(!s.getViaParms().getContent().compare("branch=z9hG4bK74b76;received=192.0.2.101"));

        cout << "received[" + s.getViaParms().findRvalue("received") + "]" <<endl;
        assert(!s.getViaParms().findRvalue("received").compare("192.0.2.101"));
        cout << "branch [" + s.getViaParms().findRvalue("branch") + "]" <<endl;
        assert(!s.getViaParms().findRvalue("branch").compare("z9hG4bK74b76"));
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

    return 0;
}


