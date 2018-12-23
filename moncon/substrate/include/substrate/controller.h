#ifndef MC_SUBSTRATE_CONTROL_H
#define MC_SUBSTRATE_CONTROL_H

#include "substrate/agent.h"
#include "substrate/simEntity.h"
#include "substrate/timeline.h"
#include "substrate/accepts_dispatch.h"

namespace MonCon {

  class domain;
  
  class controller : public simEntity, public knows_agent,
    public understands_timeline_entries,
    public accepts_dispatched_commands {
  public:
  controller(agent* a,string c) : simEntity(c),knows_agent(a)  { };
      
      virtual ~controller() {};

      // any runtime initialization required
      // return false on failure
      virtual bool initialize()=0;
      // control being handed over - start of MC loop
      // return false on failure
      virtual bool control_in()=0;
      // state simulated and control coming out of MC loop
      // return false on failure
      virtual bool control_out()=0;
      // return true if controller is doing something
      // return false if nothing to do
      virtual bool busy()=0;
      // function for external sources to inform this controller it has been
      // preempted. return confirms note.
      virtual bool note_preempted()=0;
      // function for external sources to order this controller to abort.
      // return confirms abort.
      virtual bool abort_current()=0;
      // function for external sources to send a string command.
      // return confirms processing of message.
      virtual bool tell(string message)=0;
      // function that returns true if this specific controller is doing
      // something (executing an action)
      virtual bool is_locally_controlling()=0;
      
      virtual timeline_entry* parse_tle_spec( int firetime, timeline* tl,
					      string tles ) {
	// controller is the end of the line...for now
	return NULL;
      };
      
      virtual string personalize(string input) 
      { return agent_of()->personalize(get_name()+":"+input); }
      
  protected:
      
    };

  // this class provides support for local control declarations
  // to support is_locally_controlling explicitly 
  class explicit_local_control {
  public:
  
  explicit_local_control() : is_controlling(false) {};

  protected:
    virtual void controlling(bool cv) { is_controlling=cv; };
    bool is_controlling;

  };

  // the class of controllers with sensors and effectors
  // uses explicit_local_control for is_locally_controlling behavior
  class sensorimotor_controller : public controller, 
    public explicit_local_control {
  public:
  sensorimotor_controller(sensorimotor_agent* a,string c) : 
      controller(a,c),explicit_local_control(),sm_agent(a) {};
    virtual ~sensorimotor_controller() {};
    
    sensorimotor_agent* sm_agent_of() { return sm_agent; };  
    virtual bool is_locally_controlling() { return is_controlling; };
    
  protected:
    sensorimotor_agent* sm_agent;
    
  };
  
};
#endif
