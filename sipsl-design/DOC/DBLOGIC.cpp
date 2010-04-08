//============================================================================
// Name        : DBLOGIC.cpp
// Author      : Gugli
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <pthread.h>
#include <errno.h>
#include <sys/time.h>

using namespace std;



pthread_mutex_t muread;
pthread_mutex_t muwrite;

int readinprogress = 0;
pthread_mutex_t convarmutex;
pthread_cond_t readinp;


pthread_mutex_t m1;
pthread_mutex_t m2;
pthread_mutex_t m3;
pthread_mutex_t m4;
pthread_mutex_t m5;
pthread_mutex_t m6;
pthread_mutex_t m7;

typedef struct {
	struct timeval tv;
	struct timezone tz;
} SysTime;

SysTime progtime;


//void* read_d(){
//	cout << "read" <<endl;
//
//	int rc = pthread_mutex_trylock(&muwrite);
//	if (rc == EBUSY){
//		pthread_mutex_lock(&muread);
//		pthread_mutex_unlock(&muread);
//
//		a = a + "lockedread\n"
//
//	} else{
//		pthread_mutex_unlock(&muwrite);
//		a = a + "unlockedread\n"
//
//	}
//}
//void* read_d2(){
//	cout << "read" <<endl;
//
//	int rc = pthread_mutex_trylock(&muread);
//	if (rc == EBUSY){
//		pthread_mutex_lock(&muread);
//		pthread_mutex_unlock(&muread);
//
//		a = a + "lockedread\n"
//
//	} else{
//		a = a + "unlockedread\n"
//
//	}
//}
void* read_d1(void*){

	SysTime mytime;

	timespec sleep_time;
	sleep_time.tv_sec = 5;
	sleep_time.tv_nsec = 0;

	while(true){
		pthread_mutex_lock(&m1);



		gettimeofday(&mytime.tv, &mytime.tz);
		cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " TH start read 1" << endl;

		pthread_mutex_lock(&muread);
		gettimeofday(&mytime.tv, &mytime.tz);
		cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " TH got read lock read 1" << endl;
		pthread_mutex_unlock(&muread);

		pthread_mutex_lock(&convarmutex);
		readinprogress ++;
		pthread_mutex_unlock(&convarmutex);

		gettimeofday(&mytime.tv, &mytime.tz);
		cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " TH reading on 1" << endl;

		nanosleep(&sleep_time,NULL);
		gettimeofday(&mytime.tv, &mytime.tz);
		cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " TH reading off 1" << endl;

		pthread_mutex_lock(&convarmutex);
		readinprogress--;
		if (readinprogress == 0){
			pthread_cond_signal(&readinp);
		}
		pthread_mutex_unlock(&convarmutex);


	}

}
void* read_d2(void*){

	SysTime mytime;

	timespec sleep_time;
	sleep_time.tv_sec = 5;
	sleep_time.tv_nsec = 0;

	while(true){
		pthread_mutex_lock(&m2);



		gettimeofday(&mytime.tv, &mytime.tz);
		cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " TH start read 2" << endl;

		pthread_mutex_lock(&muread);
		gettimeofday(&mytime.tv, &mytime.tz);
		cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " TH got read lock read 2" << endl;
		pthread_mutex_unlock(&muread);

		pthread_mutex_lock(&convarmutex);
		readinprogress ++;
		pthread_mutex_unlock(&convarmutex);


		gettimeofday(&mytime.tv, &mytime.tz);
		cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " TH reading on 2" << endl;

		nanosleep(&sleep_time,NULL);
		gettimeofday(&mytime.tv, &mytime.tz);
		cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " TH reading off 2" << endl;

		pthread_mutex_lock(&convarmutex);
		readinprogress--;
		if (readinprogress == 0){
			pthread_cond_signal(&readinp);
		}
		pthread_mutex_unlock(&convarmutex);


	}

}
void* read_d3(void*){

	SysTime mytime;

	timespec sleep_time;
	sleep_time.tv_sec = 5;
	sleep_time.tv_nsec = 0;

	while(true){
		pthread_mutex_lock(&m3);



		gettimeofday(&mytime.tv, &mytime.tz);
		cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " TH start read 3" << endl;

		pthread_mutex_lock(&muread);
		gettimeofday(&mytime.tv, &mytime.tz);
		cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " TH got read lock read 3" << endl;
		pthread_mutex_unlock(&muread);

		pthread_mutex_lock(&convarmutex);
		readinprogress ++;
		pthread_mutex_unlock(&convarmutex);


		gettimeofday(&mytime.tv, &mytime.tz);
		cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " TH reading on 3" << endl;

		nanosleep(&sleep_time,NULL);
		gettimeofday(&mytime.tv, &mytime.tz);
		cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " TH reading off 3" << endl;

		pthread_mutex_lock(&convarmutex);
		readinprogress--;
		if (readinprogress == 0){
			pthread_cond_signal(&readinp);
		}
		pthread_mutex_unlock(&convarmutex);


	}

}
void* read_d4(void*){

	SysTime mytime;

	timespec sleep_time;
	sleep_time.tv_sec = 5;
	sleep_time.tv_nsec = 0;

	while(true){
		pthread_mutex_lock(&m4);


		gettimeofday(&mytime.tv, &mytime.tz);
		cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " TH start read 4" << endl;

		pthread_mutex_lock(&muread);
		gettimeofday(&mytime.tv, &mytime.tz);
		cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " TH got read lock read 4" << endl;
		pthread_mutex_unlock(&muread);

		pthread_mutex_lock(&convarmutex);
		readinprogress ++;
		pthread_mutex_unlock(&convarmutex);


		gettimeofday(&mytime.tv, &mytime.tz);
		cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " TH reading on 4" << endl;

		nanosleep(&sleep_time,NULL);
		gettimeofday(&mytime.tv, &mytime.tz);
		cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " TH reading off 4" << endl;

		pthread_mutex_lock(&convarmutex);
		readinprogress--;
		if (readinprogress == 0){
			pthread_cond_signal(&readinp);
		}
		pthread_mutex_unlock(&convarmutex);


	}

}

void *write_d1(void*){
	SysTime mytime;


	timespec sleep_time;
	sleep_time.tv_sec = 10;
	sleep_time.tv_nsec = 0;


	while(true){
		pthread_mutex_lock(&m5);

		gettimeofday(&mytime.tv, &mytime.tz);
		cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " TH start write 1" << endl;


		pthread_mutex_lock(&muwrite);
		gettimeofday(&mytime.tv, &mytime.tz);
		cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " TH got write lock write 1" << endl;

		pthread_mutex_lock(&muread);
		gettimeofday(&mytime.tv, &mytime.tz);
		cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " TH writing on 1" << endl;

		pthread_mutex_lock(&convarmutex);
		if (readinprogress > 0){
			pthread_cond_wait(&readinp,&convarmutex);
		}
		pthread_mutex_unlock(&convarmutex);


		nanosleep(&sleep_time,NULL);

		gettimeofday(&mytime.tv, &mytime.tz);
		cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " TH writing off 1" << endl;

		pthread_mutex_unlock(&muread);
		pthread_mutex_unlock(&muwrite);

		gettimeofday(&mytime.tv, &mytime.tz);
		cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " TH write end 1" << endl;
	}

}
void *write_d2(void*){
	SysTime mytime;


	timespec sleep_time;
	sleep_time.tv_sec = 10;
	sleep_time.tv_nsec = 0;


	while(true){
		pthread_mutex_lock(&m6);

		gettimeofday(&mytime.tv, &mytime.tz);
		cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " TH start write 2" << endl;


		pthread_mutex_lock(&muwrite);
		gettimeofday(&mytime.tv, &mytime.tz);
		cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " TH got write lock write 2" << endl;

		pthread_mutex_lock(&muread);
		gettimeofday(&mytime.tv, &mytime.tz);
		cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " TH writing on 2" << endl;

		pthread_mutex_lock(&convarmutex);
		if (readinprogress > 0){
			pthread_cond_wait(&readinp,&convarmutex);
		}
		pthread_mutex_unlock(&convarmutex);


		nanosleep(&sleep_time,NULL);

		gettimeofday(&mytime.tv, &mytime.tz);
		cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " TH writing off 2" << endl;

		pthread_mutex_unlock(&muread);
		pthread_mutex_unlock(&muwrite);

		gettimeofday(&mytime.tv, &mytime.tz);
		cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " TH write end 2" << endl;
	}

}

int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

	pthread_t th[6];
	long t;
	int rc;
	timespec sleep_time;

	SysTime mytime;

	pthread_mutex_init(&muread, NULL);
	pthread_mutex_init(&muwrite, NULL);

	pthread_mutex_lock(&m1);
	pthread_mutex_lock(&m2);
	pthread_mutex_lock(&m3);
	pthread_mutex_lock(&m4);
	pthread_mutex_lock(&m5);
	pthread_mutex_lock(&m6);
	pthread_mutex_lock(&m7);

    rc = pthread_create(&th[0], NULL, write_d1, (void *)t);
    rc = pthread_create(&th[1], NULL, write_d2, (void *)t);
    rc = pthread_create(&th[2], NULL, read_d1, (void *)t);
    rc = pthread_create(&th[3], NULL, read_d2, (void *)t);
    rc = pthread_create(&th[4], NULL, read_d3, (void *)t);
    rc = pthread_create(&th[5], NULL, read_d4, (void *)t);

    gettimeofday(&progtime.tv, &progtime.tz);
    gettimeofday(&mytime.tv, &mytime.tz);
    //cout << (unsigned long)mytime.tv.tv_sec*1000000+mytime.tv.tv_usec - (unsigned long)progtime.tv.tv_sec*1000000+progtime.tv.tv_usec << " MAIN Ready" << endl;
    cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000)<< " MAIN Ready" << endl;

    //////////////////////////////////////////////////////////////////////////////
    //test parallel read
    //read 1
    gettimeofday(&mytime.tv, &mytime.tz);
    cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " MAIN read 1" << endl;
	pthread_mutex_unlock(&m1);

	//read 2
	sleep_time.tv_sec = 2;
	sleep_time.tv_nsec = 0;
	nanosleep(&sleep_time,NULL);
    gettimeofday(&mytime.tv, &mytime.tz);
    cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " MAIN read 2" << endl;
	pthread_mutex_unlock(&m2);

	//read 3
	sleep_time.tv_sec = 2;
	sleep_time.tv_nsec = 0;
	nanosleep(&sleep_time,NULL);
    gettimeofday(&mytime.tv, &mytime.tz);
    cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " MAIN read 2" << endl;
	pthread_mutex_unlock(&m3);

	//read 4
	sleep_time.tv_sec = 2;
	sleep_time.tv_nsec = 0;
	nanosleep(&sleep_time,NULL);
    gettimeofday(&mytime.tv, &mytime.tz);
    cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " MAIN read 2" << endl;
	pthread_mutex_unlock(&m4);


    gettimeofday(&mytime.tv, &mytime.tz);
    cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " MAIN finish" << endl;


	sleep_time.tv_sec = 10;
	sleep_time.tv_nsec = 0;
	nanosleep(&sleep_time,NULL);

    //////////////////////////////////////////////////////////////////////////////
    //test write
    //write 1
    cout << endl;
    gettimeofday(&mytime.tv, &mytime.tz);
    cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " MAIN write 1" << endl;
	pthread_mutex_unlock(&m5);

	//write 2
	sleep_time.tv_sec = 2;
	sleep_time.tv_nsec = 0;
	nanosleep(&sleep_time,NULL);
    gettimeofday(&mytime.tv, &mytime.tz);
    cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " MAIN write 2" << endl;
	pthread_mutex_unlock(&m6);

	sleep_time.tv_sec = 20;
	sleep_time.tv_nsec = 0;
	nanosleep(&sleep_time,NULL);

    //////////////////////////////////////////////////////////////////////////////
    //test write and read
    //write 1
    cout << endl;
    gettimeofday(&mytime.tv, &mytime.tz);
    cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " MAIN write 1" << endl;
	pthread_mutex_unlock(&m5);

	//read 1
	sleep_time.tv_sec = 2;
	sleep_time.tv_nsec = 0;
	nanosleep(&sleep_time,NULL);
    gettimeofday(&mytime.tv, &mytime.tz);
    cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " MAIN read 1" << endl;
	pthread_mutex_unlock(&m1);

	//read 2
	sleep_time.tv_sec = 2;
	sleep_time.tv_nsec = 0;
	nanosleep(&sleep_time,NULL);
    gettimeofday(&mytime.tv, &mytime.tz);
    cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " MAIN read 2" << endl;
	pthread_mutex_unlock(&m2);

	//read 3
	sleep_time.tv_sec = 2;
	sleep_time.tv_nsec = 0;
	nanosleep(&sleep_time,NULL);
    gettimeofday(&mytime.tv, &mytime.tz);
    cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " MAIN read 3" << endl;
	pthread_mutex_unlock(&m3);

	//read 4
	sleep_time.tv_sec = 2;
	sleep_time.tv_nsec = 0;
	nanosleep(&sleep_time,NULL);
    gettimeofday(&mytime.tv, &mytime.tz);
    cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " MAIN read 4" << endl;
	pthread_mutex_unlock(&m4);

	//fin qui tutto ok

	sleep_time.tv_sec = 20;
	sleep_time.tv_nsec = 0;
	nanosleep(&sleep_time,NULL);

    //////////////////////////////////////////////////////////////////////////////
    //test write and read and write
    //write 1
    cout << endl;
    gettimeofday(&mytime.tv, &mytime.tz);
    cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " MAIN write 1" << endl;
	pthread_mutex_unlock(&m5);

	//read 1
	sleep_time.tv_sec = 2;
	sleep_time.tv_nsec = 0;
	nanosleep(&sleep_time,NULL);
    gettimeofday(&mytime.tv, &mytime.tz);
    cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " MAIN read 1" << endl;
	pthread_mutex_unlock(&m1);

	//read 2
	sleep_time.tv_sec = 2;
	sleep_time.tv_nsec = 0;
	nanosleep(&sleep_time,NULL);
    gettimeofday(&mytime.tv, &mytime.tz);
    cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " MAIN read 2" << endl;
	pthread_mutex_unlock(&m2);

	//write 2
	sleep_time.tv_sec = 2;
	sleep_time.tv_nsec = 0;
	nanosleep(&sleep_time,NULL);
    gettimeofday(&mytime.tv, &mytime.tz);
    cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " MAIN write 2" << endl;
	pthread_mutex_unlock(&m6);


	//read 3
	sleep_time.tv_sec = 2;
	sleep_time.tv_nsec = 0;
	nanosleep(&sleep_time,NULL);
    gettimeofday(&mytime.tv, &mytime.tz);
    cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " MAIN read 3" << endl;
	pthread_mutex_unlock(&m3);

	//read 4
	sleep_time.tv_sec = 2;
	sleep_time.tv_nsec = 0;
	nanosleep(&sleep_time,NULL);
    gettimeofday(&mytime.tv, &mytime.tz);
    cout << (unsigned long)(mytime.tv.tv_sec*1000 - progtime.tv.tv_sec*1000) << " MAIN read 4" << endl;
	pthread_mutex_unlock(&m4);


	pthread_mutex_lock(&m7);



	return 0;
}
