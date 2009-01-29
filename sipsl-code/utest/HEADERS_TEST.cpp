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
 
    NEWS

    return 0;

}
