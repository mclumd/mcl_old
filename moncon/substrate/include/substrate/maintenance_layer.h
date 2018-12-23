#ifndef MCS_MLAYER_H
#define MCS_MLAYER_H

#include "substrate/controller_wrapper.h"
#include "substrate/control_common.h"
#include <map>

using namespace std;

namespace MonCon {

  class maintenance_layer : public has_action_q,public controller_wrapper {

  public:
    maintenance_layer(controller* inna_cont);
    virtual ~maintenance_layer() {};

    virtual bool control_in();
    virtual bool control_out();
    virtual bool initialize() { return peel()->initialize(); };
    virtual bool busy();
    virtual bool note_preempted() { return peel()->note_preempted(); };
    
    virtual string process_command(string c)
    { return peel()->process_command(c); };
    virtual timeline_entry* parse_tle_spec( int firetime, timeline* tl,
					    string tles ) {
      // maintenance layer should not touch the timeline
      return peel()->parse_tle_spec(firetime,tl,tles);
    };

    virtual bool publish_grammar(umbc::command_grammar& grammar) 
    { return peel()->publish_grammar(grammar); };
    virtual string grammar_commname() { return peel()->grammar_commname(); };
    virtual string class_name() { return "maintenance_layer"; };
    virtual string describe() 
    { return "(maintenance_layer wraps"+peel()->describe()+")"; };

    // none of this does anything so....
    virtual bool abort_current();
    virtual bool tell(string message);

    /// SCHEDULE MAINTENANCE STUFF !!
    typedef unsigned int si;
    static const unsigned int DO_NEVER = 0;
    void set_scheduled_action(string a_name, si every)
    { s_every[a_name] = every; };

  protected:

    /// VIRTUAL STUFF THAT CAN/SHOULD BE OVERRIDDEN BY EXTENSIONS
    virtual actionspec* generate_aspec_for(string aname);
    virtual bool process_action(action* ua);


    /// PROTECTED SCHEDULE MAINTENANCE STUFF !!
    map<string,si> s_every;
    map<string,si> s_last;

    bool do_never(string an);
    si how_often(string an);
    si scheduled_last(string an);
    void just_scheduled(string an);
    void reset_cntr(string a_name);
    bool is_due(string a_name);
    string next_due();
    
    string schedule2str();
  
  };

};

#endif
