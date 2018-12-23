#ifndef MC_SUBSTRATE_DISPAGENT_H
#define MC_SUBSTRATE_DISPAGENT_H

#include "substrate/agent.h"
#include "substrate/timeline.h"
#include "umbc/command_grammar.h"

namespace MonCon {

class controller;

class basic_dispatch_agent : public agent {
 public:
  basic_dispatch_agent(string name, domain* d) : agent(name,d),ac(NULL) {};
  virtual ~basic_dispatch_agent() {};
  virtual void         set_active_controller(controller * c);
  virtual controller*  get_active_controller();
  virtual bool         simulate() { return true; };
  
  virtual string dispatch_command(string cmd);

  virtual bool initialize() { return true; };
  virtual bool shutdown() { return true; };

  virtual string describe() 
    { return "<basic_dispatch_agent "+get_name()+">"; };
  virtual string class_name() { return "basic_dispatch_agent"; };

  virtual string grammar_commname() { return class_name()+"_command"; };
  virtual bool publish_grammar(umbc::command_grammar& grammar);

  virtual bool busy() { return false; };

 protected:
  controller* ac;

};

};
#endif
