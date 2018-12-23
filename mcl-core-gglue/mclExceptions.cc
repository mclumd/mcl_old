#include "mclExceptions.h"
#include <stdlib.h>
#include <iostream>

using namespace std;

mclException default_exception;
mclException *last_exception = &default_exception;

char mcl_perror[2048];

void mclUnexpectedHandler() {
#if defined(MCL_ABORTS_ON_EXCEPTION)
  abort();
#elif defined(MCL_RETHROWS_ON_EXCEPTION)
  throw mclException();
#endif
}

void mclTerminateHandler() {
}

void setExceptionHandlers() {
  set_unexpected(mclUnexpectedHandler);
  set_terminate(mclTerminateHandler);
}

void signalMCLException(string ms) {
  last_exception = new mclException(ms);
  cerr << "~~>> MCL exception: " << last_exception->what() << endl;
  throw last_exception;
}
