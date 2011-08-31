//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPCSL Sip Core And Service Layer
// Copyright (C) 2011 Guglielmo Incisa di Camerana
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

#include <vector>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <map>

#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stack>

#ifndef UTIL_H
#include "UTIL.h"
#endif
#ifndef LOGGER_H
#include "LOGGER.h"
#endif
#ifndef SPIN_H
#include "SPIN.h"
#endif


extern "C" void* threadparserlog (void*);

ThreadWrapperLog::ThreadWrapperLog(void) {
    //pthread_mutex_init(&mutex, NULL);
    return;
};

//**********************************************************************************
//**********************************************************************************
LOGGER::LOGGER(int _lines, int _rotations) {

	lines = _lines;
	rotations = _rotations;

	currline = 0;
	currotation = 0;


    LOGtuple *t;

	sb = new SPINC();


	t = new LOGtuple;

	t->ps = this;
	t->id = 0;


	parsethread = new ThreadWrapperLog();

	pthread_create(&(parsethread->thread), NULL, threadparserlog, (void *) t);

	//open files
	for(int i = 0 ; i < rotations ;  i++){
		stringstream name;
		name << "log" << i;
		logFile[i] = new ofstream;
		logFile[i]->open(name.str().c_str());
	}


}
void LOGGER::logit(char* _m) {

    pthread_mutex_unlock(&(sb->condvarmutex));

    if ( currline == lines){
    	logFile[currotation]->close();
    	currotation ++;
    	currotation = currotation % rotations;
    	currline = 0;
    }
    *logFile[currline] << _m << endl;
    logFile[currline]->flush();
    currline++;
    delete _m;

}
void LOGGER::p_w(char* _m) {

//	if (!activeLog[_type])
//		return;

    pthread_mutex_lock(&(sb->condvarmutex));

    sb->put(_m);
    pthread_cond_signal(&(sb->condvar));

    pthread_mutex_unlock(&(sb->condvarmutex));

    return;

}
//**********************************************************************************
//**********************************************************************************
void * threadparserlog (void * _pt){

    LOGtuple *pt = (LOGtuple *)  _pt;
    LOGGER * ps = pt->ps;
    while(true) {
        pthread_mutex_lock(&(ps->sb->condvarmutex));

        //if empty will entere here and wait signal
        while(ps->sb->isEmpty() ) {
            //this be definition will unlock above lock while it waits
            pthread_cond_wait(&(ps->sb->condvar), &(ps->sb->condvarmutex));
            //now here there is a message in queue
            //get it and parse
        }
        //if not empty will be here
        //so get it and parse

        //e' gia cosi...

        void* m = ps->sb->get();
        pt->ps->logit((char*)m);

    }
    return (NULL);
}
