#ifndef MARS_RRL_H
#define MARS_RRL_H

#include "substrate/reactive_layer.h"
#include "substrate/control_common.h"
#include "substrate/actions.h"

using namespace MonCon;

namespace mcMars {

  class rover_ugpa_controller;

  // this class is kind of stupid because it tries to use the ugpa's action q
  //
  // it will be superseded by a new one soon.
  //
  class rugpa_reactive_layer : public MonCon::has_action_q, public MonCon::reactive_layer {
  public:
    rugpa_reactive_layer(controller* inna);
    virtual ~rugpa_reactive_layer() {};
    
    virtual bool control_in();
    virtual bool control_out();
    virtual bool initialize() { return peel()->initialize(); };
    virtual bool busy() { return peel()->busy(); };
    virtual bool note_preempted() { return peel()->note_preempted(); };
    virtual bool abort_current() { return peel()->abort_current(); };
    virtual bool tell(string msg);

    virtual string process_command(string c)
    { return peel()->process_command(c); };

    virtual string grammar_commname() { return peel()->grammar_commname(); };
    virtual string class_name() { return "rugpa_reactive_layer"; };
    virtual string describe() 
    { return "(rugpa_reactive_layer wraps"+peel()->describe()+")"; };

  protected:
    rover_ugpa_controller* my_rugpa;

    virtual bool process_action(action* ua);
    
  };

};

#endif

