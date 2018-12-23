#ifndef MCL_EXCEPTIONS_H
#define MCL_EXCEPTIONS_H

#include <exception>
#include <string>
#include "APITypes.h"

/** \file
 * \brief MCL high level exception handling.
 * contains code for throwing and handling exceptions.
 */

#define MCL_ABORTS_ON_EXCEPTION
// #define MCL_RETHROWS_ON_EXCEPTION

//! sets up the default exception handlers for MCL.
void setExceptionHandlers();

//! signals a known MCL exception.
void signalMCLException(string msg);

#endif

