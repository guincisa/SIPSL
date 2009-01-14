#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include "SUDP.h"
#include "SIPRATYPE.h"
#include "CALLMNT.h"
#include "CSL_CC.h"
/*
#include "REG_AC.h"
#include "PROXY_AC.h"
*/

int main(void) {

    cout << "begin" << endl << flush;
    SUDP mystack;
    SIPRATYPE rt;
    CSL_CC csl_cc(&rt);
/* del b2bua
    REG_AC regac(&rt);
    PROXY_AC proxyac(&rt);
*/
    AC_CALLBACK cb;
    //rt.associateCB(&cb);
    //del b2bua rt.associateAC(&regac,&proxyac);
    rt.associateCSL_CC(&csl_cc);
/* del b2bua
    regac.associateSIPSTACK(&mystack);
    proxyac.associateSIPSTACK(&mystack);
*/
    csl_cc.associateSIPSTACK(&mystack);

    // blocca
    mystack.init(5061, & rt, "es.atosorigin.com");
    mystack.start();
    pthread_mutex_t gu = PTHREAD_MUTEX_INITIALIZER;
    int res = pthread_mutex_lock(&gu);
    res = pthread_mutex_lock(&gu);
    res = pthread_mutex_lock(&gu);

    return 0;

}
