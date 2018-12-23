#ifndef MC_SUBSTRATE_SOCKET_DISPATCH_H
#define MC_SUBSTRATE_SOCKET_DISPATCH_H

#include "substrate/dispatch_control_core.h"
#include "substrate/dispatch_agent.h"

#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "pthread.h"

namespace MonCon {

class socket_dispatch_control : public dispatch_control_core {

 public:
  socket_dispatch_control(basic_dispatch_agent* a, string c, bool blocking=false);
  virtual ~socket_dispatch_control() {};
  virtual bool control_in();
  virtual bool control_out();

  virtual bool initialize() { return true; };
  virtual bool note_preempted() { return true; };

  virtual bool is_locally_controlling() { return false; };

  virtual bool publish_grammar(umbc::command_grammar& grammar);

  virtual string describe() 
    { return "<tcp_dispatch_control "+get_name()+">"; };
  virtual string class_name() { return "socket_dispatch_control"; };

  virtual string grammar_commname() { return class_name()+"_command"; };
  int process_socket_command(char *command,int newsockfd);

  static int DEFAULT_PORT_NUMBER;

  // none of this does anything so....
  virtual bool abort_current() { return false; };
  virtual bool tell(string message) {return false; };
  
 protected:
  int       faults, portno;
  pthread_t server_thread;
  bool      synchronous;

  bool block,synchStepOK,stepWhileBusy;

};

};

#endif
