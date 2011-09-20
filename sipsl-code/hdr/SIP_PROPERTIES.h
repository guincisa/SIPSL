//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPSL Sip Service Layer
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

#ifndef SIP_PROPERTIES_H
#define SIP_PROPERTIES_H

#define MAX_INVITE_RESEND 5
//seconds
//#define TIMER_1_sc 0
////microseconds 1.000.000
//#define TIMER_1_mc 500000

#define TIMER_1 500000
#define TIMER_S 5000000

//To remove alarms put this timer
//#define TIMER_1 500000000

#define TIMER_zero 0


#ifdef SPARC
//#define BPHONE "172.21.160.73"
#define BPHONE "10.21.99.79"
#else
#define BPHONE "127.0.0.1"
#endif

#endif
