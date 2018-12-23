#ifndef MARS_ROVER_MCL_WRAPS_H
#define MARS_ROVER_MCL_WRAPS_H

#include "substrate/mcl_wrappers.h"
#include "mars_rover.h"
#include "mars_rover_actions.h"

using namespace MonCon;

namespace mcMars {

  namespace mmcl {

    class rover_mcl_wrapper : public mclAware::mcl_agent {
    public:
      rover_mcl_wrapper(mars_rover* rover);

      static agent* attempt_wrap(agent* R);
      virtual string describe() 
      { return "rover_mcl("+peel()->describe()+")"; };
      virtual string process_command(string command)
      { return peel()->process_command(command); };
      mars_rover* rover_of() { return my_rover; };

    protected:
      virtual void autowrap_actions();
      virtual void autowrap_controller();
      mars_rover*  my_rover;

    };

    class rover_mcl_control_layer : public mclAware::mcl_control_layer {
    public:

      enum rover_mcl_control_states {
	STATE_MONITORING, STATE_RESPONDING, STATE_RETRY_PENDING,
	STATE_RETRYING };

      rover_mcl_control_layer(rover_mcl_wrapper* mcla,controller* inna_cont);
      virtual ~rover_mcl_control_layer() {};

      virtual bool control_in();
      virtual bool control_out();
      rover_mcl_wrapper* my_wrapped_rover() { return my_rover_wrapper; };

      virtual bool process_action(action* ua);

    private:
      int                my_state;
      rover_mcl_wrapper* my_rover_wrapper;

    };

    class rover_mcl_action : public mclAware::mcl_action {

    public:

      rover_mcl_action(mclAware::mcl_agent* mcla, action* mra); 

      static action* attempt_wrap(mclAware::mcl_agent* mcla, action* A);

    protected:
      mars_rover_action* rover_action_of() { return my_rover_action; };
      bool declare_expectations();

    private:
      mars_rover_action* my_rover_action;

    };

  };

};

#endif
