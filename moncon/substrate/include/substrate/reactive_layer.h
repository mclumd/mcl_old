#ifndef MC_SUBSTRATE_RLAYER_H
#define MC_SUBSTRATE_RLAYER_H

#include "substrate/controller_wrapper.h"

namespace MonCon {
  
  class reactive_layer : public controller_wrapper {
  public:
  reactive_layer(controller* inner_controller) : 
    controller_wrapper(inner_controller) {};
    virtual ~reactive_layer() { };
    
    /* these will need overrides in your subclass...
       virtual bool control_in()=0;
       virtual bool control_out()=0;
       virtual bool control_react(const responseVector& rv)=0;
       virtual bool initialize()=0;
       virtual bool busy()=0;
    */
    
    virtual bool publish_grammar(umbc::command_grammar& grammar) 
    { return peel()->publish_grammar(grammar); };

    virtual timeline_entry* parse_tle_spec( int firetime, timeline* tl,
					    string tles ) {
      // reactive layer should not touch the timeline
      return peel()->parse_tle_spec(firetime,tl,tles);
    };
    virtual string personalize(string input) 
    { return peel()->personalize(input); }

  };

};

#endif
