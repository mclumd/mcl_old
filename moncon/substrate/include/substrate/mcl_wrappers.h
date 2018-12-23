#ifndef MCS_MCL_WRAPPERS_H
#define MCS_MCL_WRAPPERS_H

#include "actions.h"
#include "agent_wrapper.h"
#include "controller_wrapper.h"
#include "mcl/mcl_api.h"
#include "substrate/control_common.h"

namespace MonCon {

  namespace mclAware {
    
    class has_mcl_key {
    public:
      has_mcl_key() {};
      virtual ~has_mcl_key() {};
      virtual string mcl_key_of()=0;
    };

    class mcl_agent : public sensorimotor_agent_wrapper,public has_mcl_key {
    public:
      mcl_agent(sensorimotor_agent* subagent, bool autowrap=false);
      virtual ~mcl_agent() {};

      static agent* attempt_wrap(agent* R);

      virtual string mcl_key_of();
      virtual bool fill_out_update(mclMA::observables::update& uo);
      
      /* member function overrides */

      virtual bool initialize();
      virtual bool shutdown();
      virtual bool simulate();

      virtual string class_name() { return "mcl_agent_wrap"; };
      virtual string describe() { return "mcl_agent("+peel()->describe()+")"; };

    protected:

      virtual bool declare_mcl();
      virtual bool undeclare_mcl();
      virtual void autowrap_actions();
      virtual void autowrap_controller();

    };

    class mcl_action : public action_wrappers::action_wrapper,
      public has_mcl_key {
    public:
	
    mcl_action(mcl_agent* mcla,action* subaction) : 
	action_wrapper(subaction), has_mcl_key(),my_mcl_agent(mcla),
	  use_this_reference(RESREF_NO_REFERENCE), parent_ref(EGK_NO_EG) {};

      virtual bool activate();
      virtual bool deactivate();
      virtual bool simulate() { return peel()->simulate(); };
      virtual string class_name() { return "mcl_layer"; };

      virtual string mcl_key_of() { return my_mcl_agent->mcl_key_of(); };

    protected:
      virtual bool declare_expectations();

      virtual egkType eg_key_of() 
      { return (egkType)action_wrappers::action_core(this); };
      virtual egkType get_parentref()
      { return parent_ref; };
      virtual resRefType get_resref()
      { return use_this_reference; };
      virtual void  unset_resref() 
      { use_this_reference = RESREF_NO_REFERENCE; };
      virtual void  unset_parent()
      { parent_ref = EGK_NO_EG; };
      virtual void  set_resref(resRefType r)
      { use_this_reference = r; };
      virtual void  set_parent(egkType p)
      { parent_ref = p; };
      virtual mcl_agent* my_mclagent() { return my_mcl_agent; };

    private:
      mcl_agent* my_mcl_agent;
      resRefType use_this_reference;
      egkType    parent_ref;

    };

    class mcl_control_layer : public has_action_q,public controller_wrapper {
    public:
      mcl_control_layer(mcl_agent* mcla,controller* inna_cont);
      virtual ~mcl_control_layer() {};

      // maybe not necessary
      // static controller* attempt_wrap(mcl_agent* afor,controller* c);

      virtual string mcl_key_of() { return my_mcl_agent->mcl_key_of(); };
      
      virtual bool control_in();
      virtual bool control_out();
      virtual bool initialize();
      virtual bool busy();
      virtual bool note_preempted() { return peel()->note_preempted(); };

      // no one tells MCL to abort!
      virtual bool abort_current() { return peel()->abort_current(); };
      // no one tells MCL what to do!
      virtual bool tell(string message) { return peel()->tell(message); };
      
      // unfortunately, ince the mcl_control_layer can be instantiated,
      // it needs to implement this.
      virtual bool is_locally_controlling();

      virtual string process_command(string c)
      { return peel()->process_command(c); };
      virtual timeline_entry* parse_tle_spec( int firetime, timeline* tl,
					      string tles ) {
	// mcl layer should not touch the timeline
	return peel()->parse_tle_spec(firetime,tl,tles);
      };

      virtual bool publish_grammar(umbc::command_grammar& grammar) 
      { return peel()->publish_grammar(grammar); };
      virtual string grammar_commname() { return peel()->grammar_commname(); };
      virtual string class_name() { return "mcl_control_layer"; };
      virtual string describe() 
      { return "(mcl_layer wraps"+peel()->describe()+")"; };
      virtual mcl_agent* my_mclagent() { return my_mcl_agent; };

    private:
      mcl_agent* my_mcl_agent;
      
    };

  };
    
};

#endif
