/*
 * File:   cogwheelftpserverreply.h
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */
#ifndef COGWHEELFTPSERVERREPLY_H
#define COGWHEELFTPSERVERREPLY_H

//
// Class: CogWheelFtpServerReply
//
// Description: Used to signal a program error and to trigger a reply to server. Three constructors
// are provided for object creation with the default code of 550 being used if none is supplied
// and a message is empty if that isn't passed.
//

// =============
// INCLUDE FILES
// =============

// =================
// CLASS DECLARATION
// =================

class CogWheelFtpServerReply : public std::exception {

public:

    CogWheelFtpServerReply (quint16 responseCode) noexcept : responseCode(responseCode) { }
    CogWheelFtpServerReply (quint16 responseCode, QString const& message) noexcept :  message(message), responseCode(responseCode) { }
    CogWheelFtpServerReply (QString const& message) noexcept  :  message(message)  { }

    QString getMessage() const { return message; }
    quint16 getResponseCode() const { return responseCode; }

private:
    QString message;           // Default ""
    quint16 responseCode=550;  // Default code: error action not taken.

};

#endif // COGWHEELFTPSERVERREPLY_H
