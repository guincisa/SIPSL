//***********************************************************************
// PTESTCSLEE
// Written to run perfomance test on CLSEE
// Can measure latency at a given cps, can measure maximum throughput
//
// The tool is made by a scheduler and a set of search threads. The scheduler
// initiates the search threads, each one does a bind and then blocks on his mutex.
// The scheduler searches for the first available thread (not blocked on a
// search) and releases its mutex so the search thread sends the ldap query
// and becomes busy until the search returns. Once the threads return it
// record its stats, re-block on the mutex and sets itself available.
// Once the scheduler releases a mutex of a search thread it also searchs immediately
// for the next available search thread, then waits the delta time = 1/cps
// before activating it. This allows to send searches at a predefined cps.
//
// Notes:
// Ldap data is not extracted, the tool only measures the time of the ldap_search_ext_s
// Time is measured in nanoseconds.
// Waiting of the delta time is done with a for loop and not with nanosleep because
// this one sleeps too much...
// The loop is calibrated at startup, the tool checks how many iterations it must
// do be waiting for one millisecond. This allows to run the tool on any machine.
// The usage of many threads may give incorrects statistics due to the high rate
// of threads switch (well I think so...).
// A datagrid + fast db needs few threads (5-10) a datagrid + slow db will need many
// threads (10-20).
//
// Input parameters are:
// type of test:
//      1: single search: does a single ldap search
//      3: many runs: does a certain number of runs, first one at given cps, second one
//      at 2*cps, third at 2*2*cps, or 3*cps depending on cpsupgrade parameter etc
//      4: burst mode: runs without cps control
// print:
//      1: only the thread number
//      2: thread timing
// cps upgrade:
//      2: the runs of the single test have the initial cps increades by a power of 2:
//      cps, 2*cps, 4*cps, 8*cps...
//      1: the runs of the single test have the initial cps increased linearly:
//      cps, 2*cps, 3*cps, 4*cps...
// time of single run:
//      in seconds: is the time of the every run of the test
// cps:
//      intial call per sec
// number of runs:
//
// ip address:
//      datagrid server
// ip port
//      datagrid server
//
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <time.h>
#include <map>
#include <pthread.h>
#include <string>
#include <sys/socket.h> /* for socket() and bind() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <string.h>     /* for memset() */

#define ECHOMAX 255
#define THN 30
#define USELOOP 1
//Used for self testing
//#define TESTSYNCH 20

//24 Aprile 2007 CondVar
pthread_mutex_t condvarmutex= PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condvar = PTHREAD_COND_INITIALIZER;
bool SearchNeeded = false;

// Create mutexes
pthread_mutex_t p_mux[THN] = {
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,

        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,

        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER/*,

        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,

        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,

        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER */
};
// thread ID, not used anymore
int t_id[THN];
// thread status busy/free
bool readyt[THN];
// Statistics and timing
//hrtime_t startrun[THN], endrun[THN], cumlat[THN], minlat[THN] , maxlat[THN];
long double avglat[THN];
long double sum_sqr[THN]; // stdev
int iters[THN];

// common to all threads
long addtokey; // key + addtokey
//
// Scheduler data common to threads
// run number
int runn;

// input parameters
char IPADDR[64];
int IPPORT;
int TYPEOFTEST;
int OUTPRINT;

using namespace std;

void*
do_search(void* data)
{
    // set thread identifier
    int rc;
    int me = *((int*)data);

    char echoString[ECHOMAX+1];
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    struct sockaddr_in fromAddr;     /* Source address of echo */
    unsigned short echoServPort;     /* Echo server port */
    char *servIP;                    /* IP address of server */
    unsigned int fromSize;   
    string incmessage,incmessagenew;
    char outputs[256];

    // infinite loop
    while (true) {
        int pI, pIe, pInew,pIenew;
        char echoBuffer1[ECHOMAX+1];
        char echoBuffer2[ECHOMAX+1];
        char echoBuffer3[ECHOMAX+1];
        char echoBuffer4[ECHOMAX+1];
        char echoBuffer5[ECHOMAX+1];
        char echoBuffer6[ECHOMAX+1];
        char echoBuffer7[ECHOMAX+1];
        char echoBuffer8[ECHOMAX+1];
        char echoBuffer9[ECHOMAX+1];
        int echoStringLen;
        //lock thread in a mutex and wait, the scheduler unlocks the mutex when
        //the thread needs to send the search
        readyt[me] = true;
        // COND VAR
        //rc = pthread_mutex_lock(&p_mux[me]);
        rc = pthread_mutex_lock(&condvarmutex);
        while( ! SearchNeeded) {
            pthread_cond_wait(&condvar, &condvarmutex);

        }
        SearchNeeded = false;
        pthread_mutex_unlock(&condvarmutex);


//        hrtime_t inittime = gethrtime();

        sprintf(echoString,"M:INIT\nI:\nR:1\nP:hello");
        //cout << "Sending\n+++\n" << echoString << "\n+++" << endl;
        echoStringLen = strlen(echoString);
        sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
        memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
        echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
        echoServAddr.sin_addr.s_addr = inet_addr(IPADDR);  /* Server IP address */
        echoServAddr.sin_port   = htons(IPPORT);     /* Server port */
        sendto(sock, echoString, echoStringLen, 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr));
    
        fromSize = sizeof(fromAddr);
    
        recvfrom(sock, echoBuffer1, ECHOMAX, 0, (struct sockaddr *) &fromAddr, &fromSize);
        //cout << "Receiving\n+++\n" << echoBuffer << "\n+++" << endl;
        recvfrom(sock, echoBuffer2, ECHOMAX, 0, (struct sockaddr *) &fromAddr, &fromSize);
        //cout << "Receiving\n+++\n" << echoBuffer << "\n+++" << endl;

        incmessage = echoBuffer2;
        pI = incmessage.find("I:");
        pIe = incmessage.find("\n", pI );
        sprintf(outputs,"Thread %d CallID a %s \n",me, incmessage.substr(pI+2,pIe-pI-2).c_str());
        cout << outputs << flush;
        //cout << "I : [" << incmessage.substr(pI+2,pIe-pI-2) << "]" << endl;
        // message[k].I_I = message[k].incomingMessage.substr(pI+2,pIe-pI-2);
        sprintf(echoString,"M:CONT\nI:%s\nR:1\nS:1\nP:primo",incmessage.substr(pI+2,pIe-pI-2).c_str() );
        echoStringLen = strlen(echoString);
        //cout << "Sending\n+++\n" << echoString << "\n+++" << endl;
        sendto(sock, echoString, echoStringLen, 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr));
        recvfrom(sock, echoBuffer3, ECHOMAX, 0, (struct sockaddr *) &fromAddr, &fromSize);
        //cout << "Receiving\n+++\n" << echoBuffer << "\n+++" << endl;
        recvfrom(sock, echoBuffer4, ECHOMAX, 0, (struct sockaddr *) &fromAddr, &fromSize);
        //cout << "Receiving\n+++\n" << echoBuffer << "\n+++" << endl;
        incmessagenew = echoBuffer4;
        pInew = incmessagenew.find("I:");
        pIenew = incmessagenew.find("\n", pI );
        sprintf(outputs,"Thread %d CallID b %s \n",me, incmessagenew.substr(pInew+2,pIenew-pInew-2).c_str());
        cout << outputs << flush;
        if (incmessage.substr(pI+2,pIe-pI-2) != incmessagenew.substr(pInew+2,pIenew-pInew-2)) {
            cout <<"Inconsistency" << endl;
        }

        /*
        sprintf(echoString,"M:CONT\nI:%s\nR:1\nS:2\nP:secondo",incmessage.substr(pI+2,pIe-pI-2).c_str() );
        echoStringLen = strlen(echoString);
        //cout << "Sending\n+++\n" << echoString << "\n+++" << endl;
        sendto(sock, echoString, echoStringLen, 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr));
        recvfrom(sock, echoBuffer5, ECHOMAX, 0, (struct sockaddr *) &fromAddr, &fromSize);
        //cout << "Receiving\n+++\n" << echoBuffer << "\n+++" << endl;
        recvfrom(sock, echoBuffer6, ECHOMAX, 0, (struct sockaddr *) &fromAddr, &fromSize);
        //cout << "Receiving\n+++\n" << echoBuffer << "\n+++" << endl;
        incmessagenew = echoBuffer6;
        pInew = incmessagenew.find("I:");
        pIenew = incmessagenew.find("\n", pI );
        sprintf(outputs,"Thread %d CallID c %s \n",me, incmessagenew.substr(pInew+2,pIenew-pInew-2).c_str());
        cout << outputs << flush;
        if (incmessage.substr(pI+2,pIe-pI-2) != incmessagenew.substr(pInew+2,pIenew-pInew-2)) {
            cout <<"Inconsistency" << endl;
        }
        */

        sprintf(echoString,"M:END\nI:%s\nR:1\nS:2\nP:terzo",incmessage.substr(pI+2,pIe-pI-2).c_str() );
        echoStringLen = strlen(echoString);
        //cout << "Sending\n+++\n" << echoString << "\n+++" << endl;
        sendto(sock, echoString, echoStringLen, 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr));
        recvfrom(sock, echoBuffer7, ECHOMAX, 0, (struct sockaddr *) &fromAddr, &fromSize);
        //cout << "Receiving\n+++\n" << echoBuffer << "\n+++" << endl;
        recvfrom(sock, echoBuffer8, ECHOMAX, 0, (struct sockaddr *) &fromAddr, &fromSize);
        //cout << "Receiving\n+++\n" << echoBuffer << "\n+++" << endl;
        incmessagenew = echoBuffer8;
        pInew = incmessagenew.find("I:");
        pIenew = incmessagenew.find("\n", pI );
        sprintf(outputs,"Thread %d CallID d %s \n",me, incmessagenew.substr(pInew+2,pIenew-pInew-2).c_str());
        cout << outputs << flush;
        if (incmessage.substr(pI+2,pIe-pI-2) != incmessagenew.substr(pInew+2,pIenew-pInew-2)) {
            cout <<"Inconsistency" << endl;
        }

//        hrtime_t endtime = gethrtime();
        // NEED TO CHECK if message is OK
        /*
        cout << echoBuffer1 << endl << flush;
        cout << echoBuffer2 << endl << flush;
        cout << echoBuffer3 << endl << flush;
        cout << echoBuffer4 << endl << flush;
        cout << echoBuffer5 << endl << flush;
        cout << echoBuffer6 << endl << flush;
        cout << echoBuffer7 << endl << flush;
        cout << echoBuffer8 << endl << flush;
        */
        // end ldap search

        //prepare some output
//        hrtime_t difftime = endtime - inittime;
#ifdef TESTSYNCH
        sprintf(outputs, "RUN %d Thread %d # %lld # ns # %lld # ns # %lld # ns \n", runn, me, inittime, endtime, difftime);
#else
        //if (TYPEOFTEST == 1 || TYPEOFTEST == 3)
        if (OUTPRINT == 2)
            sprintf(outputs, "RUN %d Thread %d # %lld # ns # %lld # ns # %lld # ns \n", runn, me, inittime, endtime, difftime);
        else 
            sprintf(outputs,"%d ",me);
#endif
        // print info
        cout << outputs;

        // work on statistics
        iters[me]++;
        if (startrun[me] == 0 )
            startrun[me] = inittime;
        endrun[me] = inittime;
        cumlat[me] = difftime + cumlat[me];
        if (minlat[me] > difftime || minlat[me] == 0) {
            minlat[me] = difftime;
        }
        if (maxlat[me] < difftime) {
            maxlat[me] = difftime;
        }
        avglat[me] = cumlat[me] / iters[me];
        // stdev
        sum_sqr[me] = sum_sqr[me] + (long double)difftime * (long double)difftime;
    }
    /* terminate the thread */
    pthread_exit(NULL);
    return (void *) 0;
}

int main(int argc, char* argv[])
{
    // other input params
    int CPSUPGRADE;
    int CALLPERSEC;
    hrtime_t RUNTIME;
    int NUMRUNS;

    // counters & other temporary variables
    int i;
    // Thread
    pthread_t  p_thread[THN];
    // thread return value
    int trv;

    // Scheduler data
#ifdef USELOOP
    // needed to calculate resolution time
    hrtime_t t1synch,t2synch;
    float loopstosynch;
    char addintinfo[256];   // Information about synch
#endif
    hrtime_t delta;         // 1/cps in nanosecods
    struct timespec tmdelt; // 1/cps in nanosecs (not needed)
    struct timespec tzero;  // zero, used to sleep
    int runcps;             // increased cps for the run
    char runinfo[256];      // Run initial information
    bool endwhole = false;  // Flag indicates if test is finished
    hrtime_t tr1, tr2;      // start and end time of the run
    int kk = 0;             // thread index used when searching ready thread
    bool mustlaunch = true; // Flag indicates that the run is not finished
    hrtime_t t1, t3, deltas;// t1 time start of the search of free thread, t3 end of search
                            // of free thread
                            // deltas = delta - (t3 - t1) is the time that the scheduler
                            // must wait before activating the new search

    // stats
    long double cumlat_ = 0;
    hrtime_t totstart_ = 0, totend_ = 0, minlat_ = 0, maxlat_ = 0;
    int hits_ = 0;
    long double avglat_ = 0, realcps_ = 0;
    long double sum_sqr_ = 0;
    long double stddev_ = 0;
    char strstat[256];


    // COMMAND LINE 
    TYPEOFTEST = atoi(argv[1]); 
    OUTPRINT = atoi(argv[2]);
    CPSUPGRADE = atoi(argv[3]); 
    RUNTIME = atoi(argv[4]);
    CALLPERSEC = atoi(argv[5]);
    NUMRUNS = atoi(argv[6]);
    sprintf(IPADDR,"%s",argv[7]);
    IPPORT = atoi(argv[8]);

    // print initial information
#ifdef TESTSYNCH
    cout << " ************* TEST SYNCH *********************" << endl;
#endif
    if (TYPEOFTEST == 1)
        cout << "Single search " << endl;
    if (TYPEOFTEST == 2)
        cout << "Single run" << endl;
    if (TYPEOFTEST == 3)
        cout << "Full test" << endl;
    if (TYPEOFTEST == 4)
        cout << "Burst" << endl;
    cout << "Time of single run " << RUNTIME << endl;
    cout << "Start cps " << CALLPERSEC << endl;
    cout << "Ip Address " << IPADDR << endl;
    cout << "Ip Port " << IPPORT << endl;
    cout << "Number of threads " << THN << endl;

#ifdef USELOOP
    // need to calculate how many iterations are needed
    // to wait one millisecond
    // may need to do some more iterations to have better definition
    cout << "Calculating resolution time..." << endl;
        tzero.tv_sec = 0;
        tzero.tv_nsec = 0;
        t1synch = gethrtime();
        for(int ii = 0; ii < 10000;ii++){ nanosleep(&tzero, NULL);}
        //need +- 1 ms to attain 1000 cps
        t2synch = gethrtime();
        loopstosynch = (1000000 / (float)(t2synch - t1synch));
        t1synch = gethrtime();
        for(int ii = 0; ii < 10000;ii++){ nanosleep(&tzero, NULL);}
        //need +- 1 ms to attain 1000 cps
        t2synch = gethrtime();
        loopstosynch = (1000000 / (float)(t2synch - t1synch)) + loopstosynch;
        loopstosynch = loopstosynch/2;
    sprintf(addintinfo, "Resolution %lld %f loops", (t2synch - t1synch),loopstosynch );
    cout << addintinfo << endl;
#endif
    
    // create and lock mutexes
    for (i = 0 ; i < THN ; i++ ) {
        trv = pthread_mutex_lock(&(p_mux[i]));
    }
    cout << "....Mutexes locked" << endl;

    // init all threads
    for (i = 0 ; i < THN ; i++ ) {
        t_id[i] = i;
        trv = pthread_create(&p_thread[i], NULL, do_search, (void*)&t_id[i]);
    }
    // need to wait some time, the threads need to finish bind
    cout << "Wait 5 secs ... " << endl;
    usleep(5000000);
    // Now all threads should be inited and locked in mutexes
    
    // if only one run then do it with thread number 0 
    // and then exit
    if (TYPEOFTEST == 1) {
        trv = pthread_mutex_unlock(&p_mux[0]);
        usleep(5000000);
        return 0;
    }

    // Initialize Scheduler Data
    delta = 1/((long double)CALLPERSEC) * 1000 *1000 * 1000; // mcs
    tmdelt.tv_sec = 0;
    tmdelt.tv_nsec = (long)delta;
    runcps = CALLPERSEC;
    runn = 0;
    while (runn < NUMRUNS) {
        runn ++;
        // Print current run info
        sprintf(runinfo, "\nRun %d - cps %d - delta %lld\n", runn, runcps, delta);
        cout << runinfo << endl;

        endwhole = false;

        for (i = 0 ; i < THN ; i++ ) {
            // initialize statistics of the current run
            startrun[i] = 0;
            endrun[i] = 0;
            cumlat[i] = 0;
            avglat[i] = 0;
            minlat[i] = 0;
            maxlat[i] = 0;
            iters[i] = 0;
            sum_sqr[i] = 0;
        }
        
        // Calculate start time of run
        tr1 = gethrtime();

        // find the thread and wait delta
        // substract to delta the time spent to search for
        // the next free thread
        t1 = gethrtime();
        while (!endwhole) {                
            kk = 0;
            while(mustlaunch){
                // look for a free thread
                if (readyt[kk] == true) {
                    // free thread found, set it to busy
                    readyt[kk] = false;
                    t3 = gethrtime();
                    deltas = t3 - t1;
                    // if burst mode or deltas < 0 don't wait
                    if ( TYPEOFTEST != 4 && deltas < delta ) {
                        tmdelt.tv_nsec = (long)(delta - deltas);
#ifdef TESTSYNCH
                            t1synch = gethrtime();
#endif
                        
#ifdef USELOOP
                            int tosleep = (loopstosynch * 10000) *tmdelt.tv_nsec / 1000000;
                            for(int iii = 0 ; iii < tosleep;iii++){nanosleep(&tzero, NULL);};
#else
                            nanosleep(&tmdelt,NULL);
#endif
#ifdef TESTSYNCH
                            t2synch = gethrtime();
                            sprintf(addintinfo, "Actual time slept %lld ns ## Needed %ld tosleep %d \n",t2synch-t1synch,tmdelt.tv_nsec, tosleep);
                            cout << addintinfo << endl;
#endif
                    }
                    ////Aprile 24
                    //trv = pthread_mutex_unlock(&p_mux[kk]);
                    pthread_mutex_lock(&condvarmutex);
                    SearchNeeded = true;
                    pthread_cond_signal(&condvar);
                    pthread_mutex_unlock(&condvarmutex);
                    t1 = gethrtime();
                    mustlaunch = false;
                }
                else {
                    kk++;
                    kk = kk % THN;
                }
            }
            mustlaunch = true;
            
            // check if run needs to end because of elapsed time
            tr2 = gethrtime();
            if (tr2 - tr1 > (RUNTIME * 1000*1000*1000)) {
                cout << "\nWait 15 seconds for the searches to complete..." << endl;
                usleep(15000000);
                
                //STATISTICS
                cumlat_ = 0;
                totstart_ = 0;
                totend_ = 0;
                minlat_ = 0;
                maxlat_ = 0;
                hits_ = 0;
                avglat_ = 0;
                realcps_ = 0;
                stddev_ = 0;
                sum_sqr_ = 0;

                for (i = 0; i < THN; i ++ ) {
                    if ( iters[i] != 0) {
                        cumlat_ = cumlat_ + cumlat[i];
                        sum_sqr_ = sum_sqr_ + sum_sqr[i];
                        if (totstart_ > startrun[i] || totstart_ == 0) { 
                            totstart_ = startrun[i];
                        }
                        if (totend_ < endrun[i]) { 
                            totend_ = endrun[i];
                        }
                        if (minlat_ > minlat[i] || minlat_ == 0) {
                            minlat_ = minlat[i];
                        }
                        if (maxlat_ < maxlat[i]) {
                            maxlat_ = maxlat[i];
                        }
                        hits_ = hits_ + iters[i];
                    }
                }
                avglat_ = cumlat_/hits_;
                stddev_ = sqrt((sum_sqr_ - cumlat_ * avglat_) / (hits_ -1));
                realcps_ = hits_ / ((long double)(totend_ - totstart_)) * 1000000000;
                sprintf(strstat, "\n\nLatency avg %.2Lf\tmin %.2Lf\tmax %.2Lf\tstddev %.4Lf\nThroughput %.2Lf\tHits %d\tDuration %.2Lf ms", avglat_/1000000, ((long double)minlat_)/1000000, ((long double)maxlat_)/1000000, ((long double)stddev_)/1000000, realcps_, hits_, (long double)(totend_ - totstart_)/1000000);
                cout << strstat << endl << endl;
                endwhole = true;
            }
        }
        // linear cps increase
        if (CPSUPGRADE == 1) {
            runcps = CALLPERSEC * (runn + 1);
            delta = 1/((long double)(CALLPERSEC * (runn + 1))) * 1000 *1000 *1000 ; 
            tmdelt.tv_nsec = (long)delta;
        }
        else {
            runcps = runcps * 2;
            delta = delta / 2;
        }
    }
    
    /* NOT REACHED */
    return 0;
}
