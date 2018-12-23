#ifndef MC_SUBSTRATE_DISPATCH_CORE_H
#define MC_SUBSTRATE_DISPATCH_CORE_H

#include "substrate/controller.h"
#include "substrate/dispatch_agent.h"

namespace MonCon {

class dispatch_control_core : public controller {
 public:
  dispatch_control_core(basic_dispatch_agent* a, string c) :
    controller(a,c) {};
  virtual timeline_entry * parse_tle_spec( int firetime, timeline* tl,
					   string tles );

  virtual string process_command(string command) {
    return dispatch::generate_ignored_msg("Dispatch controller cannot dispatch commands to another dispatcher (or can it?).");
  }

  virtual string generate_grammar();

  virtual bool busy() { return false; };

 protected:
  string show_agents();
  string send_agent_command(string commstr);

};

};
#endif
