#include "rover_mcl_wrappers.h"
#include "mars_rover_actions.h"
#include "mars_logging.h"
#include "umbc/exceptions.h"

using namespace umbc;
using namespace mcMars;

mmcl::rover_mcl_wrapper::rover_mcl_wrapper(mars_rover* rover) :
  mcl_agent(rover,false),my_rover(rover) {
  autowrap_actions();
  autowrap_controller();
}

void mmcl::rover_mcl_wrapper::autowrap_controller() {
  controller* nu_cntrl = 
    new rover_mcl_control_layer(this,get_active_controller());
  set_active_controller(nu_cntrl);
}

void mmcl::rover_mcl_wrapper::rover_mcl_wrapper::autowrap_actions() {
  // auto-replace the actions with mcl-wrapped variants 
  // (if they are not already wrapped)
  //
  for (int i=0;i<num_actions();i++) {
    action* k  = get_action(i);
    action* wa = rover_mcl_action::attempt_wrap(this,k);
    if (wa)
      my_rover->replace_action(k,wa);
    else {
      uLog::annotate(MARSL_WARNING,sub->get_name()+": "+
		     k->get_name()+": attempt_wrap failed?");
    }
  }
}

agent* mmcl::rover_mcl_wrapper::attempt_wrap(agent* R) {
  // slippery slope -- this will fail if we start mutliply-wrapping agents
  if (!R) {
    exceptions::signal_exception("cannot wrap NULL.");
    return R;
  }
  else {
    mars_rover* RaMR = dynamic_cast<mars_rover*>(R);
    if (RaMR)
      return new rover_mcl_wrapper(RaMR);
    else {
      exceptions::signal_exception("rover_mcl_wrapper cannot cast "+R->get_name()+" for wrapping.");
      return R;
    }
  }
}
