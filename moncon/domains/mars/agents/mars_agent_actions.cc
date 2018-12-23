#include "umbc/declarations.h"
#include "mars_agent_actions.h"
#include "mars_domain.h"
#include "mars_logging.h"
#include "mcl/mcl_api.h"

using namespace mcMars;

bool mars_domain_action::simulate() {
  umbc::declarations::declare(agent_of()->personalize("actionAttempted"));
  umbc::uLog::annotate(MARSL_HOSTERR,"[maa]::MAA LEVEL simulate() FUNCTION UNIMPLAMENTED!");
  /*
  if (cost() == CALLOUT_COST)
    return true; // bail here and let the parent handle the callout
  else if (its_domain->get_sensor("power")->intValue() < cost()) {
    its_domain->set_sensor("status",STATUS_NRG_FAILURE);
    return false;
  }
  else {
    its_domain->inc_sensor("power",-1*cost());
    return true;
  }
  */
  return false;
}

bool mars_repair_action::simulate() {
  return simple_repair_action::simulate();
}
