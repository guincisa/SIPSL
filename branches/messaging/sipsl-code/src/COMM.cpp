//**********************************************************************************
//**********************************************************************************
//**********************************************************************************
// SIPCSL Sip Core And Service Layer
// Copyright (C) 2007 Guglielmo Incisa di Camerana
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

#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <pthread.h>
#include <string>
#include <iostream>
#include <memory>
#include <sys/time.h>
#include <sys/socket.h>

#include "COMPAR.h"
#include "COMM.h"

// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
ThreadWrapper::ThreadWrapper(void) {
    pthread_mutex_init(&mutex, NULL);
    return;
}
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
// MESSAGE
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
MESSAGE::MESSAGE() {
}
MESSAGE::MESSAGE(string _s, int _sock, struct sockaddr_in _echoClntAddr ) {

    incomingMessage = _s;
    sock = _sock;
    echoClntAddr = _echoClntAddr;
}
void MESSAGE::createReplyTemplate(string pay) {
    //replyTemplate.I_I = I_I;
    //replyTemplate.P_P = pay;
    char m[256];
    //sprintf(m, "M:REPLY-%s\nI:%s\nR:%d\nP:%s", M_M.c_str(), I_I.c_str(), R_R, pay.c_str());
    sprintf(m, "gugli reply template");
    replyTemplate.message = m;
    return;
}
void MESSAGE::sendReply(string message) {
        char ecco[ECHOMAX];
        //replyTemplate.out_ts = gethrtime();
        GETTIME(replyTemplate.out_ts)
        sprintf(ecco, "%s\nTI:%lld\nTO:%lld", replyTemplate.message.c_str(), in_ts, replyTemplate.out_ts);
        sendto(sock, ecco, ECHOMAX, 0, (struct sockaddr *) &(echoClntAddr), sizeof(echoClntAddr));
        return;

}
// ack
void MESSAGE::sendAck(string message) {
        char ecco[ECHOMAX];
        //sprintf(ecco, "M:OK %s\nI:%s\nR:%d", message.c_str(), I_I.c_str(), R_R);
        sprintf(ecco, "gugli ack");
        sendto(sock, ecco, ECHOMAX, 0, (struct sockaddr *) &(echoClntAddr), sizeof(echoClntAddr));

}
void MESSAGE::sendNAck(int errnum, string message) {
        char ecco[ECHOMAX];
        //sprintf(ecco, "M:NOK! %s\nI:%s\nR:%d", message.c_str(), I_I.c_str(), R_R);
        sprintf(ecco, "gugli nack");
        sendto(sock, ecco, ECHOMAX, 0, (struct sockaddr *) &(echoClntAddr), sizeof(echoClntAddr));
}
//auto_ptr<REGISTER200OK> MESSAGE::create200OK(void) {
REGISTER200OK * MESSAGE::p_create200ok(void) {
    REGISTER200OK * p_notuse;
    p_notuse = new REGISTER200OK;
    return p_notuse;
}
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
// BASEMESSAGE
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
void BASEMESSAGE::parseLines(void){

    bool stilllines = true;
    int iii=1,jjj=0,kkk=0,ij=0;

    if (areparsed)
        return;

    linePosition[0] = 0;
    while (stilllines  && iii < MAXLINES) {
        //search: METHOD, CallId, To, From, Cseq, expires
        kkk = incomingMessage.find("\n", jjj );
        if ( kkk < 0) {
            stilllines = false;
        }
        else {
            linePosition[iii] = kkk + 1;
            jjj = kkk+1;
            cout <<"lines[iii] " <<linePosition[iii] << " iii " << iii <<endl;
            iii ++;
        }
    }
    for (ij = 1; ij < iii;ij++) {
        lineMessage[ij -1] = incomingMessage.substr(linePosition[ij-1],linePosition[ij] - linePosition[ij-1] -2);
        cout <<"line " << ij-1<< " [" <<lineMessage[ij-1] <<"]\n";
    }

    totLines = iii -1;
    cout << "tot lines " <<  totLines << endl;
    areparsed = true;
    return;
}
// *****************************************************************************************
// *****************************************************************************************
int BASEMESSAGE::getTotLines() {

    if (!areparsed)
        parseLines();

    return(totLines);
}
// *****************************************************************************************
// *****************************************************************************************
string BASEMESSAGE::getLineMessage(int pos){
    // no copia REFERRENCE

    if (!areparsed)
        parseLines();

    if (pos >= totLines)
        pos = totLines-1;

    return(lineMessage[pos]);
}
// *****************************************************************************************
// *****************************************************************************************
int BASEMESSAGE::getLineParsed(int pos){

    if (!areparsed)
        parseLines();

    if (pos >= totLines)
        pos = totLines-1;

    return(lineParsed[pos]);
}
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
// CallID
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
bool CallID::doParse(void){

    if (isparsed)
        return iscorrect;

    int iii = 1, iPos=0;
    int totlines = baseMessage->getTotLines();
    hasValue = 0;
    while ( iii <= totlines) {
       if (baseMessage->getLineParsed(iii) > 0){  // skip linekk
               iii ++;
       }
       else {
          iPos = baseMessage->getLineMessage(iii).find("Call-ID: ", 0 , 9);
          if ( iPos < 0 ) {
              iii ++;
          }
          else {
              value = baseMessage->getLineMessage(iii);
              callid = baseMessage->getLineMessage(iii).substr(9, -1 );
              hasValue = 1;
              baseMessage->setLineParsed(iii,1);
              iii = totlines + 1;
          }
       }
    }
    if ( hasValue != 1){
        iscorrect = 0;
        isparsed = true;
        return false;
    }
    else {
        isparsed = true;
        iscorrect = true;
    }
    return true;
}
string CallID::getCallId(void) {

    if (!isparsed) {
        doParse();
    }
    //empty if incorrect
    return callid;
}
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
// To
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
bool To::doParse(void){

    if (isparsed)
        return iscorrect;

    int iii = 1,iPos,ePos,iiPos,eePos;
    int totlines = baseMessage->getTotLines();
    while ( iii <= totlines ) {
           if (baseMessage->getLineParsed(iii) > 0){  // skip line
               iii ++;
           }
           else {
              iPos = baseMessage->getLineMessage(iii).find("To: ", 0 , 4);
              if ( iPos < 0 ) {
                  iii ++;
              }
              else {
                  value = baseMessage->getLineMessage(iii).substr(4, -1 );
                  hasValue = 1;
                  baseMessage->setLineParsed(iii,1);
                  iii = totlines + 1;
              }
           }
        }
        if ( hasValue != 1){
            isparsed = true;
            iscorrect = false;
            return false;
        }
        ToEruri.setInitValue(value);
        isparsed = true;
        iscorrect = true;
        return true;
}
ERURI& To::getToEruri(void){

    if (!isparsed)
        doParse();
    return ToEruri;

}
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
// Contact
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
//
bool Contact::doParse(void){
    if (isparsed)
        return iscorrect;

    int iii = 1,iPos,iiPos,eePos,iiiPos,sPos;
    int totlines = baseMessage->getTotLines();

    star = false;

    while ( iii <= totlines ) {
           if (baseMessage->getLineParsed(iii) > 0){  // skip line
               iii ++;
           }
           else {
              iPos = baseMessage->getLineMessage(iii).find("Contact: ", 0 , 9);
              if ( iPos < 0 ) {
                  iii ++;
              }
              else {
                  value = baseMessage->getLineMessage(iii).substr(9, -1 );
                  hasValue = 1;
                  baseMessage->setLineParsed(iii,1);
                  iii = totlines + 1;
              }
           }
        }
        if ( hasValue != 1){
            isparsed = true;
            iscorrect = false;
            return false;
        }

        iiPos = value.find("<", iPos);
        eePos = value.find(">", iiPos+1);
        if (iiPos < 0 && eePos < 0) {
            sPos = value.find("*", iPos);
            if ( sPos > 0 ) {
                star = true;
                isparsed = true ;
                iscorrect = true;
                return iscorrect;
            }
        }
        contEruri.setInitValue(value); // can be with or without <>
        isparsed = true;
        iscorrect = true;
        return true;
}
ERURI& Contact::getContEruri(void){

    if (!isparsed)
        doParse();
    return contEruri;
}
bool Contact::isStar(void) {
    if (!isparsed)
        doParse();
    return star;
}
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
// Contact
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
/*
bool URI::doParse(void) {

    int iPos,ePos,iiPos,eePos,add;

    if (isparsed)
        return iscorrect;

    if(!hasValue) {
        iscorrect = false;
        isparsed = false; //can fill it later
        return false;
    }
    // sip:UNCHAL@lucent.com
    iPos = value.find("sip:",  0, 4);
    if (iPos < 0) {
        iPos = value.find("sips:", 0, 5);
        if (iPos < 0) {
            isparsed = true;
            iscorrect = false;
            return false;
        }
        else {
            secure = true;
            add = 4;
        }
    }
    else {
        secure = false;
        add = 5;
    }

    ePos = value.find("@",iPos + add);
    if (ePos < 0) {
        isparsed = true;
        iscorrect = false;
        return false;
    }
    else {
        user = value.substr(iPos + add -1, ePos-iPos -add +1);
        isparsed = true;
        iscorrect = true;
    }
    host = "DO THAT";
    uriparms.setParms("do=that");

    return true;
}
string URI::getUser(void) {

    if(!isparsed){
        doParse();
    }
        return user;
}
bool URI::isSecure(void) {

    if(!isparsed){
        doParse();
    }
        return secure;
}

string URI::getHost(void) {

    if(!isparsed){
        doParse();
    }
        return host;
}
Parameters URI::getParms(void) {
    return uriparms;
}
*/
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
// HeaderValue
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
bool HeaderValue::doParse(void) {
    return true;
}
bool HeaderValue::isCorrect(void){
    return(doParse());
}
void HeaderValue::setBaseMessage(MESSAGE * p){

    baseMessage = p;
}
string HeaderValue::getHeader(int pos) {
    //TODO
    return"";
}
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
// Parameters
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
void Parameters::setParms(string p) {
    listofparms = p;
    return;
}
string Parameters::getParmValue(string head) {

    int iPos=-1,iiPos=-1;
    DEBOUT("Looking for",head)
    iPos = listofparms.find(head.c_str(), 0 , head.length());
    if ( iPos < 0 ) {
        DEBOUT("Parameters::getParmsValue not found:",head)
        return"";
    }
    iiPos = listofparms.find(";",iPos,1);
    if ( iiPos < 0 ) {
        // probably the last parm
        DEBOUT("Last parameter of the string","")
        iiPos = listofparms.length();
    }
    string outps = listofparms.substr(iPos+head.length() + 1,iiPos-iPos-head.length() -1 );
    DEBOUT("Extracted",outps)

    return outps;
}
string Parameters::getValue(void) {
    return listofparms;
}
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
// StringParse
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
// NEW PARSER
StringParse::StringParse() {
    value = "";
    iscorrect = false;
    isparsed = false;
    return;
}
void StringParse::setInitValue(string s) {
    //cout << "\n**set initial value " << this <<  endl;
    value = s;
}
string StringParse::getValue(void) {
    return value;
}
bool StringParse::doParse(void) {
    return true;
}
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
// RequestLine
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
bool RequestLine::doParse(void) {

    if(isparsed) {
        return iscorrect;
    }
    else {
//cout << " RequestLine::doParse parsing" << endl;

        baseMessage->setLineParsed(0,1);

        int iPos,ePos,iiPos,eePos;
        int method_length=0;
        // PARSE METHOD
        // REGISTER
        methodId = -1;
        //if (methodId == -1) {
            iPos = baseMessage->getLineMessage(0).find("REGISTER", 0 , 8 );
            if (iPos >= 0 ){
                method_length = 8;
                methodId = REGISTER_METHOD;
            }
        //}
        // INVITE
        if ( methodId == -1 ) {
            iPos = baseMessage->getLineMessage(0).find("INVITE", 0, 6);
            if (iPos >= 0 ){
                method_length = 6;
                methodId = INVITE_METHOD;
            }
        }
        if ( methodId == -1 ) {
DEBOUT("",baseMessage->getLineMessage(0))
            iPos = baseMessage->getLineMessage(0).find("INTERNALS", 0, 9);
            if (iPos >= 0 ){
DEBOUT("","")
                method_length = 9;
                methodId = INTERNALS_METHOD;
                iscorrect = true;
                isparsed = true;
                return true;
            }
        }

        if (methodId == -1) {
            isparsed = true;
            iscorrect = false;
            return false;
        }
        value = baseMessage->getLineMessage(0);
        // PARSE SIPURI SIPSURI ABSURI
        // look for sip or sips
        iPos = -1;
        iPos = baseMessage->getLineMessage(0).find("sip", method_length + 1 , 3 );
        if (iPos < 0 ) {
            isparsed = true;
            iscorrect = false;
            return false;
        }
        else {
            // space
            ePos = baseMessage->getLineMessage(0).find(" ", method_length + 1, 1);
            if (ePos < 0 ) {
                isparsed = true;
                iscorrect = false;
                return false;
            }
            else {
                //cout << "*RequestLine::doParse [" << baseMessage->getLineMessage(0).substr(iPos, ePos-iPos) << "]*" << endl;
                reqUri.setInitValue( baseMessage->getLineMessage(0).substr(iPos, ePos-iPos));

                //cout << "*RequestLine::doParse end [" << baseMessage->getLineMessage(0).substr(ePos + 1,-1) << "]*" << endl;
                protVs = baseMessage->getLineMessage(0).substr(ePos + 1,-1);
                isparsed = true;
                iscorrect = true;


                return true;
            }

        }

    }
}
int RequestLine::getMethodId(void) {

    if (!isparsed) {
        doParse();
    }
    return methodId;
}
RURI& RequestLine::getURI(void) {

    if (!isparsed) {
        doParse();
    }
    return reqUri;
}
string RequestLine::getProtVs(void) {

    if (!isparsed) {
        doParse();
    }
    return protVs;
}
void RequestLine::setMethodId(int mid) {
		methodId = mid;
}
// *****************************************************************************************
// *****************************************************************************************
// RURI
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
bool RURI::doParse(void) {

    if(isparsed) {
        return iscorrect;
    }
    else {
    type = 0;
//cout << " RURI::doParse do parsing" << endl;
    int iPos;
    iPos = value.find("sip:", 0 , 4 );
    if (iPos < 0 ) {
        iPos = value.find("sips:", 0 , 5 );
        if (iPos < 0 ) {
            //absoluteURI still not supported
            isparsed = true;
            iscorrect = false;
            return false;
        }
        else {
            iPos = iPos+1;
        }
    }
    type = 1;
    iPos = iPos+4;
    sipUri.setInitValue(value.substr(iPos)) ;
    isparsed = true;
    iscorrect = true;
    return true;
    }
}
int RURI::getType(void) {

    if (!isparsed) {
//cout << " RURI::getType do parsing" << endl;
        doParse();
    }
//cout << " RURI::getType not parsing" << endl;
    return type;
}

SIPURI& RURI::getSipUri(void) {

    if (!isparsed) {
//cout << " RURI::getSipUri do parsing" << endl;
        doParse();
    }
//cout << " RURI::getSipUri not parsing" << endl;
    return sipUri;
}
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
// SIPURI
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************

bool SIPURI::doParse(void) {

    if(isparsed) {
        return iscorrect;
    }
    else {
//cout << " SIPURI::doParse do parsing" << endl;
        int iPos,iiPos,iiiPos,iiiiPos,iiiiiPos;
        iPos = value.find("@", 1);
        if (iPos < 0 ) {
            //no username
            iPos = -1; // step back if "@" is not present
        }
        else {
            userinfo = value.substr(0,iPos);

            iiPos = userinfo.find(":", 1);
            if (iiPos < 0 ) {
                //no password
                username = userinfo;
            }
            else {
                username = value.substr(0,iiPos);
                password = value.substr(iiPos+1,iPos -iiPos -1 );
            }
        }
        iiiPos = value.find(";", 1);
        if (iiiPos < 0) {
            // no uriparms
        }

        hostport = value.substr(iPos + 1,iiiPos - iPos -1);
        iiiiPos = hostport.find(":", 1);
        if (iiiiPos < 0) {
            // no port
            host = hostport;
        }
        else {
            host = hostport.substr(0,iiiiPos);
            port = atoi(hostport.substr(iiiiPos +1 , -1).c_str());
        }
        iiiiiPos = value.find(";",1);
        if (iiiiiPos < 0) {
        }
        else {
            uriParms.setParms(value.substr(iiiiiPos,-1));
        }

        isparsed = true;
        iscorrect = (host != "");
    }
    /*
    cout << "######*********************************************************"<< endl;
    cout << "######value [" << value << "]"<< endl;
    cout << "######userinfo [" << userinfo << "]"<< endl;
    cout << "######username [" << username << "]"<< endl;
    cout << "######password [" << password << "]"<< endl;
    cout << "######hostport [" << hostport << "]"<< endl;
    cout << "######host [" << host << "]"<< endl;
    cout << "######port [" << port << "]"<< endl;
    cout << "######parms [" << uriParms.getValue() << "]"<< endl;
    cout << "######*********************************************************"<< endl;
    */
    return iscorrect;
}

    /*
string userinfo;
string hostport;
    if(isparsed) {
        return iscorrect;
    }
    else {
cout << " SIPURI::doParse do parsing" << endl;
    int iPos,iiPos,iiiPos,iiiiPos;
    string value[10];
    value[0] = "stronzenboy:cacone@ceppen.com:3121;transport=tcp?staminchia=qui&staceppa=lalla";
    value[1] = "stronzenboy@ceppen.com:3121;transport=tcp?staminchia=qui&staceppa=lalla";
    value[2] = "ceppen.com:3121;transport=tcp?staminchia=qui&staceppa=lalla";
    value[3] = "stronzenboy:cacone@ceppen.com;transport=tcp?staminchia=qui&staceppa=lalla";
    value[4] = "stronzenboy:cacone@ceppen.com";
    value[5] = "stronzenboy:cacone@";
    value[6] = "stronzenboy:cacone@;transport=tcp?staminchia=qui&staceppa=lalla";
    value[7] = ":cacone@ceppen.com:3121;transport=tcp?staminchia=qui&staceppa=lalla";
    cout << value << endl;
    int i;
    for (i = 0 ; i < 8; i++ ) {
    userinfo = "";
    username = "";
    password = "";
    hostport = "";
    host = "";
    port = -1;
    iPos = 0;
    iiPos = 0;
    iiiPos = 0;
    iiiiPos = 0;

    iPos = value[i].find("@", 1);
    if (iPos < 0 ) {
        //no username
        iPos = -1; // skip back the non existent "@"
        cout <<" qui xx" << endl;
    }
    else {
        userinfo = value[i].substr(0,iPos);

        iiPos = userinfo.find(":", 1);
        if (iiPos < 0 ) {
        //no password
            username = userinfo;
        }
        else {
            username = value[i].substr(0,iiPos);
            password = value[i].substr(iiPos+1,iPos -iiPos -1 );
        }
    }
    iiiPos = value[i].find(";", 1);
    if (iiiPos < 0) {
        // no uriparms
    }
    if (iiiPos == 0) {
        //no host and port, bad!
        cout << "nobbuono"<< endl;
    }
    else {
        cout <<"qui"<< iPos <<" "<<iiiPos<<endl;
        hostport = value[i].substr(iPos + 1,iiiPos - iPos -1);
        iiiiPos = hostport.find(":", 1);
        if (iiiiPos < 0) {
            // no port
            host = hostport;
        }
        else {
            host = hostport.substr(0,iiiiPos);
            port = atoi(hostport.substr(iiiiPos +1 , -1).c_str());
        }


    }

    cout << "*********************************************************"<< endl;
    cout << "value [" << value[i] << "]"<< endl;
    cout << "userinfo [" << userinfo << "]"<< endl;
    cout << "username [" << username << "]"<< endl;
    cout << "password [" << password << "]"<< endl;
    cout << "hostport [" << hostport << "]"<< endl;
    cout << "host [" << host << "]"<< endl;
    cout << "port [" << port << "]"<< endl;
    }
    return true;
    */
bool SIPURI::isSecure(void) {

    if(!isparsed) {
        doParse();
    }
    return issecure;
}
string SIPURI::getUserInfo(void) {

    if(!isparsed) {
        doParse();
    }
    return userinfo;
}
string SIPURI::getHost(void) {

    if(!isparsed) {
        doParse();
    }
    return host;
}
int SIPURI::getPort(void) {

    if(!isparsed) {
        doParse();
    }
    return port;
}
Parameters& SIPURI::getUriParms(void) {

    if(!isparsed) {
        doParse();
    }
    return uriParms;
}
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
// ERURI
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
//
bool ERURI::doParse(void) {

    if (isparsed)
        return iscorrect;

    int iii = 1,iPos,ePos,iiPos,eePos;
    //display name
    iPos = value.find("\"");
    if (iPos < 0) {
        // no user name
        //
    }
    else {
        ePos = value.find("\"", iPos+1);
        if (ePos < 0) {
            isparsed = true;
            iscorrect = false;
            return false;
        }
        displayname = value.substr(iPos+1,ePos - iPos -1);
    }
    iiPos = value.find("<", ePos+1);
    eePos = value.find(">", iiPos+1);
    // check here
    if (iiPos < 0 && eePos < 0) {
        // no < or > which we consider optional (see TILAB user agent
        eruriRuri.setInitValue(value);
        // will not have params eternal to <>
    }
    else {
        eruriRuri.setInitValue(value.substr(iiPos+1, eePos - iiPos -1));
        eruriParms.setParms(value.substr(eePos+1,-1));
    }
    isparsed = true;
    iscorrect = true;
    return true;
}
string ERURI::getEruriDisplayname(void) {

    if(!isparsed) {
        doParse();
    }
    return displayname;
}
RURI& ERURI::getEruriRuri(void) {

    if(!isparsed) {
        doParse();
    }
    return eruriRuri;
}
Parameters& ERURI::getEruriParms(void) {

    if(!isparsed) {
        doParse();
    }
    return eruriParms;
}
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
// From
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
//
bool From::doParse(void){

    if (isparsed)
        return iscorrect;

    int iii = 1,iPos,ePos,iiPos,eePos;
    int totlines = baseMessage->getTotLines();
    while ( iii <= totlines ) {
           if (baseMessage->getLineParsed(iii) > 0){  // skip line
               iii ++;
           }
           else {
              iPos = baseMessage->getLineMessage(iii).find("From: ", 0 , 6);
              if ( iPos < 0 ) {
                  iii ++;
              }
              else {
                  value = baseMessage->getLineMessage(iii).substr(6, -1 );
                  hasValue = 1;
                  baseMessage->setLineParsed(iii,1);
                  iii = totlines + 1;
              }
           }
        }
        if ( hasValue != 1){
            isparsed = true;
            iscorrect = false;
            return false;
        }
        fromEruri.setInitValue(value);
        isparsed = true;
        iscorrect = true;
        return true;
}
ERURI& From::getFromEruri(void){

    if (!isparsed)
        doParse();
    return fromEruri;

}
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
// Expires
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************

bool Expires::doParse(void){

    if (isparsed)
        return iscorrect;

    int iii = 1,iPos,ePos,iiPos,eePos;
    int totlines = baseMessage->getTotLines();
    while ( iii <= totlines ) {
           if (baseMessage->getLineParsed(iii) > 0){  // skip line
               iii ++;
           }
           else {
              iPos = baseMessage->getLineMessage(iii).find("Expires: ", 0 , 9);
              if ( iPos < 0 ) {
                  iii ++;
              }
              else {
                  value = baseMessage->getLineMessage(iii).substr(9, -1 );
                  hasValue = 1;
                  baseMessage->setLineParsed(iii,1);
                  iii = totlines + 1;
              }
           }
        }
        if ( hasValue != 1){
            //optional
            present = false;
            isparsed = true;
            iscorrect = true;
            return true;
        }
        expires = atoi(value.c_str());
        present = true;
        isparsed = true;
        iscorrect = true;
        return true;
}
int Expires::getExpires(void){

    if (!isparsed)
        doParse();
    return expires;

}
bool Expires::isPresent(void){

    if (!isparsed)
        doParse();
    return present;

}
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
// Via
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
bool Via::doParse(void){

    if (isparsed)
        return iscorrect;

    int iii,iPos,ePos,iiPos,eePos;
    int totlines = baseMessage->getTotLines();
    iii = totlines;
    while ( iii > 0 ) {
           if (baseMessage->getLineParsed(iii) > 0){  // skip line
               iii --;
           }
           else {
              iPos = baseMessage->getLineMessage(iii).find("Via: ", 0 , 5);
              if ( iPos < 0 ) {
                  iii --;
              }
              else {
                  value = baseMessage->getLineMessage(iii).substr(5, -1 );
                  hasValue = 1;
                  baseMessage->setLineParsed(iii,1);
                  iii = -1;
              }
           }
        }
        if ( hasValue != 1){
            present = false;
            isparsed = true;
            iscorrect = true;
            return true;
        }
        iiPos = value.find(" ");
        eePos = value.find(";");
        if (eePos < 0) {
            hostport =  value.substr(iiPos + 1,-1);
        }
        else {
            hostport =  value.substr(iiPos + 1, eePos -iiPos -1);
            viaParms.setParms(value.substr(eePos, -1));
        }
        present = true;
        isparsed = true;
        iscorrect = true;

        return true;
}
bool Via::isPresent(void) {

    if (!isparsed)
        doParse();
    return present;
}
string Via::getViaHost(void) {
    if (!isparsed)
        doParse();
    return host;
}
string Via::getViaHostPort(void) {
    if (!isparsed)
        doParse();
    return hostport;
}
Parameters& Via::getViaParms(void) {

    if(!isparsed) {
        doParse();
    }
    return viaParms;
}

// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
// CSqe
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
//
bool CSeq::doParse(void) {

    if (isparsed)
        return iscorrect;

    int iii,iPos,ePos,iiPos,eePos;
    int totlines = baseMessage->getTotLines();
    iii = totlines;
    while ( iii > 0 ) {
           if (baseMessage->getLineParsed(iii) > 0){  // skip line
               iii --;
           }
           else {
              iPos = baseMessage->getLineMessage(iii).find("CSeq: ", 0 , 6);
              if ( iPos < 0 ) {
                  iii --;
              }
              else {
                  value = baseMessage->getLineMessage(iii).substr(6, -1 );
                  hasValue = 1;
                  baseMessage->setLineParsed(iii,1);
                  iii = -1;
              }
           }
        }
        if ( hasValue != 1){
            isparsed = true;
            iscorrect = false;
            return iscorrect;
        }
        iiPos = value.find(" ");
        sequence =  atoi(value.substr(0, iiPos).c_str());
        //DEBOUT("method",value.substr(iiPos +1, -1));
        method = value.substr(iiPos +1, -1);
        isparsed = true;
        iscorrect = true;

        return true;
}
int CSeq::getCSeq(void){

    if (!isparsed)
        doParse();
    return sequence;

}
string CSeq::getCSeqMethod(void){

    if (!isparsed)
        doParse();
    return method;

}

// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
// REGISTER200OK
// *****************************************************************************************
// *****************************************************************************************
// *****************************************************************************************
REGISTER200OK::REGISTER200OK(void) {
    DEBOUT("******************** creating 200ok","");
}
REGISTER200OK::~REGISTER200OK(void) {
    DEBOUT("******************** destroying 200ok","");
}
void REGISTER200OK::setHeaders(string _to, string _from, string _callid, string _cseq, string _via[], string _contact){

    DEBOUT("setHeaders TO",_to)
    _To = _to;
    DEBOUT("s_toetHeaders FROM",_from)
    _From = _from;
    DEBOUT("setHeaders CallID",_callid)
    _CallID = _callid;
    DEBOUT("setHeaders CSEQ",_cseq)
    _CSeq = _cseq;
    for (int i = 0; i < 16 ; i++) {
    DEBOUT("setHeaders VIA",_via[i])
    _Via[i] = _via[i];
    }
    DEBOUT("setHeaders Contact",_contact)
    _Contact = _contact;
    _Date = "Wed, 24 Oct 2007 16:00:00 GMT";
    DEBOUT("setHeaders Date",_Date)
    _Server = "SIPSLEE GIC 0.1";
    DEBOUT("setHeaders Server",_Server)
    _ContentLength = 0;
    DEBOUT("setHeaders ContLentgh",_ContentLength)
    return;
}
void REGISTER200OK::setSock(int s){
    sock = s;
    return;
}
void REGISTER200OK::setSockAddr(struct sockaddr_in sa){
    echoClntAddr = sa;
    return;
}
void REGISTER200OK::send(void){

        DEBOUT("SENDING**********************************************************","")
        char ecco[ECHOMAX];
        memset(ecco,0,ECHOMAX);
        sprintf(ecco, "SIP/2.0 200 OK\r\nTo: %s\r\nFrom: %s\r\n%s\r\nCSeq: %s\r\nVia: %s\r\nContact: %s;expires=3600\r\nDate: %s\r\nServer: %s\r\nContent-Length: %d\r\n\r\n", _To.c_str(),_From.c_str(),_CallID.c_str(),_CSeq.c_str(),_Via[15].c_str(),_Contact.c_str(),_Date.c_str(),_Server.c_str(),_ContentLength);
        DEBOUT("SENDING**********************************************************","")
        DEBOUT("SENDING**********************************************************","")
        DEBOUT("SENDING\n",ecco)
        DEBOUT("SENDING**********************************************************","")
        DEBOUT("SENDING**********************************************************","")
        //string secco = ecco;
        DEBOUT("SENDING string secco = ecco","")
        //TODO core???
        //sendto(sock, secco.c_str(), secco.length(), 0, (struct sockaddr *) &(echoClntAddr), sizeof(echoClntAddr));
        sendto(sock, ecco, strlen(ecco), 0, (struct sockaddr *) &(echoClntAddr), sizeof(echoClntAddr));
        DEBOUT("SENT ************************************************************","")
        return;
}
