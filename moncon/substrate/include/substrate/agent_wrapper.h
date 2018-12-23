#ifndef MC_SUBSTRATE_AGWRAPPER_H
#define MC_SUBSTRATE_AGWRAPPER_H

#include "substrate/agent.h"

namespace MonCon {

  class sensorimotor_agent_wrapper : public sensorimotor_agent {
  public:
    
    sensorimotor_agent_wrapper(sensorimotor_agent* subagent) :
    sensorimotor_agent(subagent->get_name(),subagent->domain_of()),sub(subagent) {};

    virtual ~sensorimotor_agent_wrapper(){};

    /* wrap management */

    static agent* get_core_agent(agent* a) {
      agent* ccp = a; // current controller pointer
      while (ccp) {
	cout << "searching for acore [" << ccp->class_name() << "]" << endl;
	sensorimotor_agent_wrapper* ct = 
	  dynamic_cast<sensorimotor_agent_wrapper*>(ccp);
	if (!ct) return ccp; // if it's the type we are looking for, then c ya
	else {
	  if (ct->peel() == ccp) return NULL;
	  else ccp = ct->peel();
	}
      }
      return NULL;
    };

    template <class AW> static
      AW get_typed_wrapper(agent* a) {
      agent* cap = a; // current action pointer
      while (cap) {
	cout << "searching for arapper [" << cap->class_name() << "]" << endl;
	AW ct = dynamic_cast<AW>(cap);
	if (ct) return ct; // if it's the type we are looking for, then c ya
	else {
	  // ow make sure it's a wrapper then peel
	  sensorimotor_agent_wrapper* aaaw = 
	    dynamic_cast<sensorimotor_agent_wrapper*>(cap);
	  if (aaaw) {
	    if (aaaw->peel() == cap) return NULL;
	    else cap = aaaw->peel();
	  }
	  else return NULL;
	}
      }
      return NULL;
    };

      /* these are passthrough member functions.... */

    virtual int              num_observables() 
    { return peel()->num_observables(); };
    virtual observable* get_observable(string s)
    { return peel()->get_observable(s); };
    virtual observable* get_observable_n(int i)
    { return peel()->get_observable_n(i); };
    virtual int              get_observable_index(string s)
    { return peel()->get_observable_index(s); };
    virtual observable_list* observables() { return peel()->observables(); };
    
    virtual int     num_actions() { return peel()->num_actions(); };
    virtual action* get_action(string a) { return peel()->get_action(a); };
    virtual action* get_action(int i) { return peel()->get_action(i); };
    virtual int     get_action_index(string s) 
    { return peel()->get_action_index(s); };
    virtual void    replace_action(action* orig, action* nu) 
    { return peel()->replace_action(orig,nu); };
    virtual action_list* actions() { return peel()->actions(); };
    
    virtual bool activate_action(action* a) 
    { return peel()->activate_action(a); };
    virtual bool deactivate_action() 
    { return peel()->deactivate_action(); };
    virtual string grammar_commname()
    { return peel()->grammar_commname(); };
    virtual bool publish_grammar(umbc::command_grammar& c)
    { return peel()->publish_grammar(c); };
    
    virtual void         set_active_controller(controller* c)
    { peel()->set_active_controller(c); };
    virtual controller * get_active_controller()
    { return peel()->get_active_controller(); };
    virtual bool busy()
    { return peel()->busy(); };
      
  protected:
    sensorimotor_agent* sub;
    virtual sensorimotor_agent* peel() { return sub; };


  };
};

#endif
