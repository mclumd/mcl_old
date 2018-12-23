#include "rover_opportunistic_layer.h"
#include "rover_ugpa.h"
#include "mars_logging.h"

#include "umbc/exceptions.h"

using namespace mcMars;
using namespace umbc;

rugpa_opportunistic_layer::rugpa_opportunistic_layer(controller* inna) 
  : has_action_q(),reactive_layer(inna) {
  my_rugpa = controller_wrapper::get_typed_wrapper<rover_ugpa_controller*>(inna);
}

bool rugpa_opportunistic_layer::control_in() {
  uLog::annotate(MONL_HOSTMSG,personalize("opportunistic layer online"));
  
  // all rules must be zero-impact to the core layer
  
  mars_domain_model_suite* mars_model = my_rugpa->ptr_to_mdms();
  sensorimotor_agent* msma = my_rugpa->sm_agent_of();
  
  // charge rule
  int onrgt = 25; // this should be computed
  int cloc  = msma->get_observable("location")->int_value();
  int nr    = msma->get_observable("power")->int_value();
  
  // xmit rule
  int tx = msma->get_observable("t_xmit")->int_value();
  int bl = msma->get_observable("bank")->int_value();
  static int xl = (int)(((double)mars_rover::MAX_BANK)*0.25);
  
  // charge rule (priority 1)
  if ((nr <= onrgt) &&  (mars_model->can_charge(cloc))) {
      uLog::annotate(MONL_HOSTMSG,
		   personalize("opportunistic layer scheduling CHARGE."));
    add_aspec_back(new actionspec(msma->get_action("charge")));
    umbc::declarations::declare(personalize("opportunistic.charge"));
  }
  else if ((bl < xl) || ((bl < mars_rover::MAX_BANK) && (tx > 50))) {
    uLog::annotate(MONL_HOSTMSG,personalize("opportunistic layer scheduling XMIT."));
    add_aspec_back(new actionspec(msma->get_action("xmit")));
    umbc::declarations::declare(personalize("opportunistic.xmit"));
  }
  // pano rule
  else if (msma->get_observable("t_pano")->int_value() > 60) {
    uLog::annotate(MONL_HOSTMSG,personalize("opportunistic layer scheduling PANO."));
    add_aspec_back(new actionspec(msma->get_action("pano")));
    umbc::declarations::declare(personalize("opportunistic.pano"));
  }
  
  // never more than one opportunistic rule gets scheduled, so this is safe here.
  if (has_pending_tla()) {
    controlling(true);
    peel()->note_preempted();
    tlaspec()->prepare_for_activation();
    process_action(tlaspec()->underlying_action());
    advance_q();
    return true;
  }
  else {
    controlling(false);
    return peel()->control_in();
  }
}

bool rugpa_opportunistic_layer::control_out() {
  return peel()->control_out();
}

bool rugpa_opportunistic_layer::process_action(action* ua) {
  bool aarv = true;
  if (!ua)
    return false;
  else {
    aarv &= ua->sensorimotor_agent_of()->activate_action(ua);
  }
  if (aarv)
    umbc::declarations::declare(personalize("opportunistic.activation.ok"));
  else
    umbc::declarations::declare(personalize("opportunistic.activation.fail"));
  return aarv;
}

bool rugpa_opportunistic_layer::tell(string msg) {
  // this will respond to "suppress" at some point
  return peel()->tell(msg);
}
