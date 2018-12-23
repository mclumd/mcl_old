#ifndef MC_SUBSTRATE_TTY_DISPATCH_H
#define MC_SUBSTRATE_TTY_DISPATCH_H

#include "substrate/dispatch_control_core.h"

namespace MonCon {

class tty_dispatch_control : public dispatch_control_core {
 public:
  tty_dispatch_control(basic_dispatch_agent* a, string c) :
    dispatch_control_core(a,c) {};
  virtual ~tty_dispatch_control() {};
  virtual bool control_in();
  virtual bool control_out();

  virtual bool initialize() { return true; };
  virtual bool note_preempted() { return true; };

  virtual bool is_locally_controlling() { return false; };
  
  virtual string describe() 
    { return "<tty_dispatch_control "+get_name()+">"; };
  virtual string class_name() { return "tty_dispatch_control"; };
  virtual string grammar_commname() { return "command"; };
  virtual bool   publish_grammar(umbc::command_grammar& grammar);

  // none of this does anything so....
  virtual bool abort_current() { return false; };
  virtual bool tell(string message) {return false; };
    
 protected:
  void print_help();

};

};

#endif
