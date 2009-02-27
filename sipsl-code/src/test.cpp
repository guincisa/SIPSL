#include <vector>
#include <string>
#include <map>
#include <assert.h>
#include <sys/types.h>
#include <stack>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */

#ifndef UTIL_H
#include "UTIL.h"
#endif
#ifndef SPIN_H
#include "SPIN.h"
#endif
#ifndef ENGINE_H
#include "ENGINE.h"
#endif
#ifndef SIPENGINE_H
#include "SIPENGINE.h"
#endif
#ifndef SUDP_H
#include "SUDP.h"
#endif
#ifndef CS_HEADERS_H
#include "CS_HEADERS.h"
#endif
#ifndef MESSAGE_H
#include "MESSAGE.h"
#endif

/*
#include "REG_AC.h"
#include "PROXY_AC.h"
*/

int main(void) {

	SUDP mystack;

	SIPENGINE gg;

	mystack.init(5061, &gg, "gugli.com");


    mystack.start();
    pthread_mutex_t gu = PTHREAD_MUTEX_INITIALIZER;
    int res = pthread_mutex_lock(&gu);
    res = pthread_mutex_lock(&gu);
    res = pthread_mutex_lock(&gu);

    return 0;

//    cout << "begin" << endl << flush;
//    SUDP mystack;
//    SIPRATYPE rt;
//    CSL_CC csl_cc(&rt);
///* del b2bua
//    REG_AC regac(&rt);
//    PROXY_AC proxyac(&rt);
//*/
//    AC_CALLBACK cb;
//    //rt.associateCB(&cb);
//    //del b2bua rt.associateAC(&regac,&proxyac);
//    rt.associateCSL_CC(&csl_cc);
///* del b2bua
//    regac.associateSIPSTACK(&mystack);
//    proxyac.associateSIPSTACK(&mystack);
//*/
//    csl_cc.associateSIPSTACK(&mystack);
//
//    // blocca
//    mystack.init(5061, & rt, "es.atosorigin.com");
//    mystack.start();
//    pthread_mutex_t gu = PTHREAD_MUTEX_INITIALIZER;
//    int res = pthread_mutex_lock(&gu);
//    res = pthread_mutex_lock(&gu);
//    res = pthread_mutex_lock(&gu);
//
//    return 0;

}
