#ifndef MC_SUBSTRATE_AGENT_BASE_H
#define MC_SUBSTRATE_AGENT_BASE_H

#include "substrate/simEntity.h"
#include "substrate/domain.h"
#include "substrate/timeline.h"
#include "substrate/accepts_dispatch.h"
#include "mcl/mcl_api.h"

#include <list>

namespace MonCon {

  class observable;
  class action;
  class simple_repair_action;
  class controller;
  class accepts_dispatched_commands;

  typedef list<action*>              action_list;
  typedef list<MonCon::observable*>  observable_list;
  typedef list<simple_repair_action*> repair_list;

class agent : public simEntity, public knows_domain, 
    public understands_timeline_entries,
    public accepts_dispatched_commands {

 public:

  agent(string name, domain* d);
  virtual ~agent() {};

  virtual bool initialize()=0; // MCL is encapsulated into init
  virtual bool shutdown()=0;   // 

  // a *requirement* is that an agent have a controller that can be set

  virtual void         set_active_controller(controller*)=0;
  virtual controller * get_active_controller()=0;

  virtual bool busy()=0;

  // must implement a simulate() method

  virtual bool simulate()=0;

  virtual string process_command(string command);
  virtual string publish_interface();

  virtual timeline_entry* parse_tle_spec( int firetime, timeline* tl,
					  string tles );  
  
};

class sensing {
 public:
  sensing() {};
  virtual ~sensing() {};
  virtual int              num_observables()=0;
  virtual MonCon::observable*      get_observable(string s)=0;
  virtual MonCon::observable*      get_observable_n(int i)=0;
  virtual int              get_observable_index(string s)=0;
  virtual observable_list* observables()=0;

};

class acting {
 public:
  acting() {};
  virtual ~acting() {};
  virtual int     num_actions()=0;
  virtual action* get_action(string a)=0;
  virtual action* get_action(int i)=0;
  virtual int     get_action_index(string s)=0;
  virtual void    replace_action(action* orig, action* nu)=0;
  virtual action_list* actions()=0;

  virtual bool activate_action(action* a)=0;
  virtual bool deactivate_action()=0;

};
 
 class has_repair_actions {
 public:
  has_repair_actions() {};
  virtual ~has_repair_actions() {};
  
  virtual int                   num_repairs()=0;
  virtual simple_repair_action* get_repair(string a)=0;
  virtual simple_repair_action* get_repair(int i)=0;
  virtual repair_list*          get_repair_list()=0;
  
 };

 class sensorimotor_agent : public agent, public acting, public sensing {
 public:
 sensorimotor_agent(string name, domain* d) : 
  agent(name,d),acting(),sensing() {};
  
  virtual bool initialize() { return true; };
  virtual string mcl_agentspec() {
    return domain_of()->get_name() + "." + get_name();
  };
  virtual timeline_entry* parse_tle_spec( int firetime, timeline* tl, 
					  string tles );
  
};

class knows_agent {
 public:
  knows_agent(agent* a) : agnt(a) {};
  agent*  agent_of() { return agnt; };
  domain* domain_of() { return agent_of()->domain_of(); };

 private:
  agent* agnt;

};

};

#endif
