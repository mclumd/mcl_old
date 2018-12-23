#ifndef MC_SUBSTRATE_NULLC_H
#define MC_SUBSTRATE_NULLC_H

#include "substrate/controller.h"

namespace MonCon {

class null_controller : public controller {
 public:
  null_controller(agent *a, string cname) : controller(a,cname) {};
  
  virtual bool control_in() { return true; };
  virtual bool control_out() { return true; };
  virtual bool initialize() { return true; };
  virtual bool note_preempted() { return true; };

  virtual bool is_locally_controlling() { return false; };

  virtual string class_name() { return "null_controller"; };
  virtual string grammar_commname() { return get_name()+"_command"; };
  virtual bool publish_grammar(umbc::command_grammar& grammar) { return true; };
  virtual string describe() 
    { return "<null_controller "+get_name()+">"; };
  virtual string process_command(string command) {
    return ignore_msg("null_controller does nothing.");
  }

  virtual bool abort_current() { return true; };
  virtual bool tell(string message) {return false; };

  virtual bool busy() { return false; };

};

};

#endif
