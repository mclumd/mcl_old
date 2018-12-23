#include "rover_reactive_layer.h"
#include "rover_ugpa.h"
#include "mars_logging.h"

#include "umbc/exceptions.h"

using namespace mcMars;
using namespace umbc;

#define RESCUE_PROBATION_DEADLINE 5

rugpa_reactive_layer::rugpa_reactive_layer(controller* inna) 
  : has_action_q(),reactive_layer(inna) {
  my_rugpa = controller_wrapper::get_typed_wrapper<rover_ugpa_controller*>(inna);
}

enum resc_state { OFF, PROB, CHARGE, RESCUE };

bool rugpa_reactive_layer::control_in() {
  static unsigned int resc_ddlin = 0;
  static unsigned int resc_state = OFF;
  uLog::annotate(MONL_HOSTMSG,personalize("reactive layer online"));

  // check the high-priority/zero-impact rules
  // xmit rule
  int tx = my_rugpa->sm_agent_of()->get_observable("t_xmit")->int_value();
  int bl = my_rugpa->sm_agent_of()->get_observable("bank")->int_value();
  int xl = (int)(((double)mars_rover::MAX_BANK)*0.25);
  int nr = my_rugpa->sm_agent_of()->get_observable("power")->int_value();

  if (nr < 10) {
    switch (resc_state) {
    case OFF:
      resc_state=PROB;
      resc_ddlin=domain_of()->tick()+RESCUE_PROBATION_DEADLINE;
      break;
    case PROB:
      if (domain_of()->tick() > resc_ddlin) {
	uLog::annotate(MONL_HOSTMSG,
		       personalize("reactive layer scheduling CHARGE."));
	add_aspec_back(new actionspec(my_rugpa->sm_agent_of()->get_action("charge")));
	umbc::declarations::declare(personalize("reactive.charge"));
	resc_state=CHARGE;
      }
      // else it's on probation	
      break;
    case RESCUE:
      // initial rescue failed?
      uLog::annotate(MONL_HOSTERR,
		     personalize("state indicates RESCUE attempt failed."));
    case CHARGE:
      // charge must have failed.
      uLog::annotate(MONL_HOSTMSG,
		     personalize("reactive layer scheduling RESCUE."));
      add_aspec_back(new actionspec(((mars_rover*)my_rugpa->sm_agent_of())->get_repair("rescue")));
      umbc::declarations::declare(personalize("reactive.rescue"));    
      break;
    default:
      exceptions::signal_exception("illegal state in reactive rover control.");
      break;
    }
  }
  else {
    resc_state=OFF;
    if ((bl < xl) || ((bl < mars_rover::MAX_BANK) && (tx > 50))) {
      uLog::annotate(MONL_HOSTMSG,personalize("reactive layer scheduling XMIT."));
      add_aspec_back(new actionspec(my_rugpa->sm_agent_of()->get_action("xmit")));
      umbc::declarations::declare(personalize("reactive.xmit"));
    }
    // pano rule
    else if (my_rugpa->sm_agent_of()->get_observable("t_pano")->int_value() > 60) {
      uLog::annotate(MONL_HOSTMSG,personalize("reactive layer scheduling PANO."));
      add_aspec_back(new actionspec(my_rugpa->sm_agent_of()->get_action("pano")));
      umbc::declarations::declare(personalize("reactive.pano"));
    }
  }

  // never more than one reactive rule gets scheduled, so this is safe here.
  if (has_pending_tla()) {
    peel()->note_preempted();
    tlaspec()->prepare_for_activation();
    process_action(tlaspec()->underlying_action());
    advance_q();
    return true;
  }
  else return peel()->control_in();
}

bool rugpa_reactive_layer::control_out() {
  return peel()->control_out();
}

bool rugpa_reactive_layer::process_action(action* ua) {
  bool aarv = true;
  if (!ua)
    return false;
  else {
    aarv &= ua->sensorimotor_agent_of()->activate_action(ua);
  }
  if (aarv)
    umbc::declarations::declare(personalize("reactive.activation.ok"));
  else
    umbc::declarations::declare(personalize("reactive.activation.fail"));
  return aarv;
}

bool rugpa_reactive_layer::tell(string msg) {
  // this will respond to "suppress" at some point
  return peel()->tell(msg);
}
