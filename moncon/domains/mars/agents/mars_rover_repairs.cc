#include "umbc/declarations.h"
#include "substrate/mcl_model.h"
#include "mcl/mcl_api.h"
#include "umbc/exceptions.h"
#include "substrate/controller_wrapper.h"

#include "mars_domain.h"
#include "mars_rover.h"
#include "mars_rover_actions.h"
#include "mars_agent_sensors.h"
#include "rover_ugpa.h"
#include "mars_logging.h"

using namespace mcMars;
using namespace umbc;

bool sensor_diag  (mars_rover* mro);
bool effector_diag(mars_rover* mro);
bool rebuild_mdls(mars_rover* mro);

// MCL REPAIR-ISH STUFF NOW IMPLEMENTED AS DOMAIN ACTIONS
// sensor reset

string mars_sensor_reset::describe() {
  return "<sensor_reset_action>";
}

bool mars_sensor_reset::simulate() {
  mars_rover* mro = rover_of();
  if (mars_repair_action::simulate()) {
    bool sdr = sensor_diag(mro);
    mr_response_code = (sdr) ? MRC_SUCCESS : MRC_FAILED;
    return sdr;
  }
  else {
    uLog::annotate(MARSL_HOSTVRB,"[ms/mract]::sensor reset abstract simulate() failed.");
    return false;
  }
}

bool sensor_diag(mars_rover* mro) {
  umbc::declarations::declare(mro->personalize("sensorReset.diag"));
  bool daf = false;
  uLog::annotate(MARSL_HOSTERR,
		 mro->get_name()+": entering sensor diag loop.");
  for (int i=0; i < mro->num_observables(); i++) {
    observable*  q   = mro->get_observable_n(i);
    breakable* qab   = observable_utils::get_breakable(q);
    if ((qab != NULL) && qab->is_broken()) {
      sprintf(uLog::annotateBuffer," + %s: resetting sensor '%s'...", 
	      mro->get_name().c_str(),
	      q->get_name().c_str());
      uLog::annotateFromBuffer(MARSL_HOSTMSG);
      qab->fix_it();
      umbc::declarations::declare(mro->personalize("sensorReset.reset"));
      if (qab->is_broken()) {
	uLog::annotate(MARSL_HOSTERR," + "+mro->get_name()+": sensor is still broken?");
	umbc::declarations::declare(mro->personalize("sensorReset.reset.fail"));
      }
      else {
	daf = true;
	uLog::annotate(MARSL_HOSTMSG," + "+mro->get_name()+": sensor is working");
      }
    }
  }
  return daf;
}

// effector reset

string mars_effector_reset::describe() {
  return "<effector_reset_action>";
}

bool mars_effector_reset::simulate() {
  mars_rover* mro = rover_of();
  if (mars_repair_action::simulate()) {
    bool edr = effector_diag(mro);
    mr_response_code = (edr) ? MRC_SUCCESS : MRC_FAILED;
    return edr;
  }
  else {
    uLog::annotate(MARSL_HOSTVRB,"[MARS/aact]::effector reset abstract simulate() failed.");
    return false;
  }
}

bool effector_diag(mars_rover* mro) {
  umbc::declarations::declare(mro->personalize("effReset.diag"));
  bool daf = false;
  uLog::annotate(MARSL_WARNING,
		 mro->get_name()+": entering effector diag loop.");
  for (int i=0; i < mro->num_actions(); i++) {
    action*      q = mro->get_action(i);
    action_wrappers::breakable_layer* qab =
      action_wrappers::action_wrapper::get_typed_wrapper<action_wrappers::breakable_layer*>(q);
    if ((qab != NULL) && qab->is_broken()) {
      sprintf(uLog::annotateBuffer," + %s:: resetting effector '%s'...", 
	      mro->get_name().c_str(),
	      q->get_name().c_str());
      uLog::annotateFromBuffer(MARSL_HOSTMSG);
      qab->fix_it();
      umbc::declarations::declare(mro->personalize("effReset.reset"));
      if (qab->is_broken()) {
	uLog::annotate(MARSL_HOSTERR," + "+mro->get_name()+":: effector is still broken?");
	umbc::declarations::declare(mro->personalize("effReset.reset.fail"));
      }
      else {
	daf = true;
	uLog::annotate(MARSL_HOSTMSG," + "+mro->get_name()+":: effector is now working");
      }
    }
  }
  return daf;
}

// rebuild models

string mars_rebuild_models::describe() {
  return "<rebuild_models_action>";
}

bool mars_rebuild_models::simulate() {
  mars_rover* mro = rover_of();
  if (mars_repair_action::simulate()) {
    return rebuild_mdls(mro);
  }
  else {
    uLog::annotate(MARSL_HOSTVRB,
		   "[MARS/aact]::rebuild models abstract simulate() failed.");
    return false;
  }
}

bool rebuild_mdls(mars_rover* mro) {
  umbc::declarations::declare(mro->personalize("rebModels"));
  rover_ugpa_controller* acaru = 
    controller_wrapper::get_typed_wrapper<rover_ugpa_controller*>(mro->get_active_controller());
  if (acaru)
    /* okay, so this is maddening. why is this not using the models/provide
       protocol ? */
    acaru->grab_all_models();
  else {
    umbc::declarations::declare(mro->personalize("rebModels.fail"));
    exceptions::signal_exception(mro->personalize("don't know how to grab models unless controller is ROVER_UGPA."));
    return false;
  }
  return true;
}

// rescue

string mars_rescue::describe() {
  return "<rescue_action>";
}

bool mars_rescue::simulate() {
  // this should be happening elsewhere
  umbc::declarations::declare(personalize("count"));
  uLog::annotate(MARSL_HOSTERR,"[mrr]::MARS RESCUE simulate() IS UNIMPLEMENTED.");
  if (mars_repair_action::simulate()) {
    bool rv = rover_of()->get_observable("power")->attempt_set(mars_rover::MAX_NRG);
    rv &= rover_of()->get_observable("error")->attempt_set(0.0);
    rv &= rover_of()->get_observable("status")->attempt_set(mars_rover::STATUS_OKAY);
    rv &= rover_of()->get_observable("cal")->attempt_set(mars_rover::CALSTATE_UNCAL);
    
    rv &= sensor_diag(rover_of());
    rv &= effector_diag(rover_of());
    rv &= rebuild_mdls(rover_of());

    mr_response_code = (rv) ? MRC_SUCCESS : MRC_FAILED;

    return rv;
  }
  else {
    uLog::annotate(MARSL_HOSTVRB,
		   "[MARS/aact]::mars_domain_action abstract simulate() failed.");
    umbc::declarations::declare(personalize("fail"));
    return false;
  }
}
