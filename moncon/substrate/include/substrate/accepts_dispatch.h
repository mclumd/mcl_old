#ifndef MC_SUBSTRATE_ACC_DISPATCH_H
#define MC_SUBSTRATE_ACC_DISPATCH_H

#include "substrate/dispatch.h"
#include "umbc/command_grammar.h"

namespace MonCon {

  class agent;

  class accepts_dispatched_commands : public umbc::publishes_grammar {
  public:
    accepts_dispatched_commands() {};
    virtual ~accepts_dispatched_commands() {};
    
    string success_msg() { return dispatch::generate_success_msg(); };
    string success_msg(string reason) 
      { return dispatch::generate_success_msg(reason); };
    string ignore_msg()  { return dispatch::generate_ignored_msg(); };
    string ignore_msg(string explain)  
      { return dispatch::generate_ignored_msg(explain); };
    string fail_msg(string reason) 
      { return dispatch::generate_fail_msg(reason); };
    
    virtual string process_command(string command)=0;
    
    virtual string agent_command2tles(agent *a,string command);
    
    // virtual string grammar_commname()=0;
    // virtual bool   publish_grammar(command_grammar& grammar)=0;
    
  };

};

#endif
