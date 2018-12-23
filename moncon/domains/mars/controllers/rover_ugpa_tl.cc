#include "rover_ugpa.h"
#include "mars_rover_actions.h"

#include "mars_logging.h"
using namespace umbc;

using namespace mcMars;

void rover_ugpa_timeline_goal::fire() {
  if (owner_controller && the_goal) {
    rover_goal* trg = dynamic_cast<rover_goal*>(the_goal);
    domain* mydom = owner_controller->domain_of();
    agent*  myagn = owner_controller->agent_of();
    if (!trg) {
      uLog::annotate(MARSL_HOSTERR,personalize("can't cast tl goal ("+the_goal->class_name()+") to rover_goal... failing to instantiate timeline goal."));
      return;
    }  
    else {
      if (trg->action_code() == MR_TAKE_PANORAMIC) {
	goalspec gs = make_gs(trg->action_code(),trg->get_arg(0));
	static_cast<mars_domain*>(mydom)->broadcast_imaging_goal_rcvd(myagn,gs,true);
      }
      else if (trg->action_code() == MR_DO_SCIENCE) {
	goalspec gs = make_gs(trg->action_code(),trg->get_arg(0));
	static_cast<mars_domain*>(mydom)->broadcast_imaging_goal_rcvd(myagn,gs,false);
      }
    }
  }
  ugpa_timeline_goal::fire();
}
