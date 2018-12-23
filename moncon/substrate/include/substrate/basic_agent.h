#ifndef MC_SUBSTRATE_BAGENT_H
#define MC_SUBSTRATE_BAGENT_H

#include "substrate/agent.h"
#include "substrate/timeline.h"

#include <list>
using namespace std;

namespace MonCon {

class basic_agent : public sensorimotor_agent, public has_repair_actions {
 public:
  basic_agent(string name, domain* d) : 
    sensorimotor_agent(name,d),
    active_action(NULL), active_controller(NULL)
    {};

  virtual bool activate_action(action* a);
  virtual bool deactivate_action();
    
  virtual bool shutdown();

  virtual int          num_actions() { return a.size(); };
  virtual action_list* actions() { return &a; };
  virtual action*      get_action(string a);
  virtual action*      get_action(int i);
  virtual int          get_action_index(string a);
  virtual action*      get_active_action() {return active_action;};
  virtual void         replace_action(action* orig, action* nu);
  virtual observable_list* observables() { return &s; };
  virtual MonCon::observable*      get_observable(string s);
  virtual MonCon::observable*      get_observable_n(int i);
  virtual int              num_observables() { return s.size(); };
  virtual int              get_observable_index(string s);

  virtual MonCon::observable*  get_observable_core(string s);
  virtual MonCon::observable*  get_observable_core_n(int i);

  virtual double get_observable_value_dbl(string s);
  virtual int get_observable_value_int(string s);
  
  virtual int           num_repairs() { return r.size(); };
  virtual simple_repair_action* get_repair(string a);
  virtual simple_repair_action* get_repair(int i);
  virtual repair_list*  get_repair_list() { return &r; };

  // virtual bool activate_repair(basic_repair_action* a);
  // virtual bool deactivate_repair();

  virtual void         set_active_controller(controller * c);
  virtual controller*  get_active_controller();

  virtual bool         simulate();
  virtual bool         initialize() { return true; };

  virtual string sv_as_str();
  virtual string ev_as_str();

  virtual timeline_entry* parse_tle_spec( int firetime, timeline* tl,
					  string tles );

  virtual string process_command(string command);

  virtual bool   ack() { return true; };

 protected:
  
  action_list     a;
  observable_list s;
  repair_list     r;
  action        * active_action;
  controller    * active_controller;

};

};
#endif
