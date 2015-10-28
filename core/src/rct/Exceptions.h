/*
 * Exceptions.h
 *
 *  Created on: Oct 19, 2015
 *      Author: leon
 */

#pragma once

#include <exception>

/**
 * \brief General exception for the rct library.
 */
class RctException: public std::exception {
public:
    RctException(const std::string &msg) :
            msg(msg) {

    }
    RctException(const std::string &msg, const std::exception &reason) :
            msg(msg), reason(reason) {

    }
    RctException(const std::exception &reason) :
            reason(reason) {

    }
    virtual ~RctException() throw () {
    }
    virtual const char* what() const throw () {
        std::string out;
        if (msg.empty()) {
            out = "RctException";
        } else {
            out = msg;
        }
        if (reason.what() != "") {
            out += ". Reason: ";
            out += reason.what();
        }
        return out.c_str();
    }

private:
    std::string msg;
    std::exception reason;
};

/**
 * \brief Exception for errors related to impossible extrapolation of transformations into the
 * future or past.
 *
 * This usually occurs when lookups for some point in time are requested that would require
 * extrapolation beyond current limits.
 */
class ExtrapolationException: public RctException {
public:
    ExtrapolationException(const std::string &msg) :
            RctException(msg) {

    }
    ExtrapolationException(const std::string &msg, const std::exception &reason) :
            RctException(msg, reason) {

    }
    ExtrapolationException(const std::exception &reason) :
            RctException("ExtrapolationException", reason) {

    }
    virtual ~ExtrapolationException() throw () {
    }
};

/**
 * \brief Exception for errors related to impossible transformation lookup.
 */
class LookupException: public RctException {
public:
    LookupException(const std::string &msg) :
            RctException(msg) {

    }
    LookupException(const std::string &msg, const std::exception &reason) :
            RctException(msg, reason) {

    }
    LookupException(const std::exception &reason) :
            RctException("LookupException", reason) {

    }
    virtual ~LookupException() throw () {
    }
};

/**
 * \brief Exception for errors related to wrong arguments.
 */
class InvalidArgumentException: public RctException {
public:
    InvalidArgumentException(const std::string &msg) :
            RctException(msg) {

    }
    InvalidArgumentException(const std::string &msg, const std::exception &reason) :
            RctException(msg, reason) {

    }
    InvalidArgumentException(const std::exception &reason) :
            RctException("InvalidArgumentException", reason) {

    }
    virtual ~InvalidArgumentException() throw () {
    }
};

/**
 * \brief Exception for errors related to unconnected transformation trees.
 */
class ConnectivityException: public RctException {
public:
    ConnectivityException(const std::string &msg) :
            RctException(msg) {

    }
    ConnectivityException(const std::string &msg, const std::exception &reason) :
            RctException(msg, reason) {

    }
    ConnectivityException(const std::exception &reason) :
            RctException("ConnectivityException", reason) {

    }
    virtual ~ConnectivityException() throw () {
    }
};

