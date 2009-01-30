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

    NEWS
    cout << "Begin HEADERS test" << endl << flush;

    
    {
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

    return 0;

}
