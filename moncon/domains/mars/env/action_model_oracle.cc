#include "mars_domain.h"
#include "mars_rover.h"
using namespace mcMars;

#include "substrate/model_effects.h"

#include "umbc/exceptions.h"
#include "mars_logging.h"
using namespace umbc;

bool mars_domain::has_action_model_for(simEntity* e) {
  if (e)
    return (action_model_base.find(e->class_name()) != action_model_base.end());
  else {
    exceptions::signal_exception("attempt to ask oracle for a model for NULL (check that identify_self_to_provider(this) is executed in model_is_provided subclasses.");
    return NULL;
  }
}

action_model* mars_domain::action_model_for(simEntity* e) {
  if (!has_action_model_for(e)) 
    return NULL;
  else {
    if (action_model_cache.find(e) != action_model_cache.end())
      return (action_model_cache[e]);
    else {
      action_model* bm = action_model_base[e->class_name()];
      bm->set_underlying_entity(e);
      action_model_cache[e]=bm;
      return bm;
    }
  }
}

action_model* make_rover_recharge_model() {
  action_model* rm = new action_model("recharge");
  rm->add_effect(new simple_effect("power",false,mars_rover::MAX_NRG));
  return rm;
}

action_model* make_rover_moveto_model() {
  action_model* rm = new action_model("moveto");
  rm->add_effect(new change_effect("location"));
  rm->add_effect(new change_effect("power"));

  // we might want an expectation of power usage.

  return rm;
}


action_model* make_rover_doscience_model() {
  action_model* rm = new action_model("doscience");
  rm->add_effect(new simple_effect("bank",true,-1*mars_rover::SCI_IMG));

  // science should consumer power/expect it?

  return rm;
}

action_model* make_rover_panoramic_model() {
  action_model* rm = new action_model("panoramic");
  rm->add_effect(new simple_effect("bank",true,-1*mars_rover::PANO_IMG));  
  rm->add_effect(new simple_effect("t_pano",false,0));  
  return rm;
}

action_model* make_rover_localize_model() {
  action_model* rm = new action_model("localize");
  rm->add_effect(new simple_effect("error",false,0));
  return rm;
}

action_model* make_rover_xmit_model() {
  action_model* rm = new action_model("xmit");
  rm->add_effect(new simple_effect("bank",false,mars_rover::MAX_BANK));
  rm->add_effect(new simple_effect("t_xmit",false,0));
  // expectation on reward??
  return rm;
}

action_model* make_rover_calibrate_model() {
  action_model* rm = new action_model("calibrate");
  rm->add_effect(new argtarg_effect("cal",false,0));
  return rm;
}

void mars_domain::init_action_oracle() {
  action_model_base["mr_recharge"]=make_rover_recharge_model();
  action_model_base["mr_moveto"]=make_rover_moveto_model();
  action_model_base["mr_do_science"]=make_rover_doscience_model();
  action_model_base["mr_take_panoramic"]=make_rover_panoramic_model();
  action_model_base["mr_localize"]=make_rover_localize_model();
  action_model_base["mr_xmit"]=make_rover_xmit_model();
  action_model_base["mr_calibrate"]=make_rover_calibrate_model();
}
