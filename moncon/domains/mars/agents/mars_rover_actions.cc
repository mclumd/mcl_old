#include "umbc/declarations.h"
#include "substrate/mcl_model.h"
#include "substrate/controller.h"
#include "substrate/basic_observables.h"

#include "mars_domain.h"
#include "mars_rover.h"
#include "mars_rover_actions.h"
#include "mars_agent_sensors.h"
#include "mars_logging.h"
#include "mcl/mcl_api.h"

using namespace mcMars;
using namespace umbc;

// STATIC FUNCTIONS / UTILS

string rover_action_names[] = { "moveto", "charge", "pano", "science",
				"xmit", "loc", "cal" };
int mars_rover_action::action_name2code(string name) {
  for (int q=0;q<MR_NUMBER_OF_ACTIONS;q++) {
    if (name == rover_action_names[q])
      return q;
  }
  return -1;
}

mars_rover_action::mars_rover_action(string nm,int key,mars_rover* a) :
  mars_domain_action(nm,key,a),model_is_provided(a->mars_domain_of()),my_rover(a) {
  identify_self_to_provider(this);
}

mars_rover_action::mars_rover_action(string nm,int key,mars_rover* a,int argc) : 
  mars_domain_action(nm,key,a,argc),model_is_provided(a->mars_domain_of()),my_rover(a) {
  for (int q=0;q<argc;q++)
    set_arg(MA_NO_ARG);
  identify_self_to_provider(this);
}


// RECHARGE

string mars_recharge::describe() {
  return "(recharge_action)";
}

bool mars_recharge::simulate() {
  // if (mars_domain_action::simulate())
  if (((mars_domain*)agent_of()->domain_of())->is_rechargable(agent_of())) {
    basic_agent_of()->get_observable("power")->attempt_set(mars_rover::MAX_NRG);
    basic_agent_of()->get_observable("status")->attempt_set(mars_rover::STATUS_OKAY);
    return true;
  }
  else {
    basic_agent_of()->get_observable("status")->attempt_set(mars_rover::STATUS_RC_ERROR);
    return false;
  }
}

// LOCALIZE

string mars_localize::describe() {
  return "(localize_action)";
}

bool mars_localize::simulate() {
  if (((mars_domain*)agent_of()->domain_of())->is_localizable(agent_of())) {
    (static_cast<basic_observable_state*>(basic_agent_of()->get_observable("error")))->attempt_set(0.0);
    (static_cast<basic_message_box*>(basic_agent_of()->get_observable("status")))->attempt_set(mars_rover::STATUS_OKAY);
    return true;
  }
  else {
    (static_cast<basic_message_box*>(basic_agent_of()->get_observable("status")))->attempt_set(mars_rover::STATUS_LOC_ERROR);
    return false;
  }
}


// XMIT

string mars_xmit::describe() {
  return "(xmit_action)";
}

bool mars_xmit::simulate() { 
  uLog::annotate(MARSL_SUCCESS,
		      "[mars/aact]::XMIT needs underpinnings in the simulator!");
  if (my_imagebank.get_numimgs() == 0) {
    basic_agent_of()->get_observable("t_xmit")->attempt_set(0.0);
    basic_agent_of()->get_observable("status")->attempt_set(mars_rover::STATUS_OKAY);
    umbc::declarations::declare(personalize("empty"));
    return true;
  }
  else if (((mars_domain*)agent_of()->domain_of())->attempt_xmit_ibank(agent_of(),my_imagebank)) {
    basic_agent_of()->get_observable("t_xmit")->attempt_set(0.0);
    basic_agent_of()->get_observable("status")->attempt_set(mars_rover::STATUS_OKAY);
    umbc::declarations::declare(personalize("ok"));
    my_imagebank.clear();
    return true;
  }
  else {
    (static_cast<basic_message_box*>(basic_agent_of()->get_observable("status")))->attempt_set(mars_rover::STATUS_XMIT_ERROR);
    umbc::declarations::declare(personalize("fail"));
    return false;
  }
}

// CALIBRATE

string mars_calibrate::describe() {
  char vv[128];
  sprintf(vv,"(calibrate_action L=%d)",get_arg_int());
  return vv;
}

bool mars_calibrate::simulate() {
  int targ = get_arg_int();
  if (((mars_domain*)agent_of()->domain_of())->can_calibrate(agent_of(),targ)) {
    (static_cast<basic_observable_state*>(basic_agent_of()->get_observable("cal")))->attempt_set(targ);
    (static_cast<basic_message_box*>(basic_agent_of()->get_observable("status")))->attempt_set(mars_rover::STATUS_OKAY);
    return true;
  }
  else {
    (static_cast<basic_observable_state*>(basic_agent_of()->get_observable("cal")))->attempt_set(mars_rover::CALSTATE_UNCAL);
    (static_cast<basic_message_box*>(basic_agent_of()->get_observable("status")))->attempt_set(mars_rover::STATUS_CAL_ERROR);
    return false;
  } 
}

string mars_calibrate::grammar_argspec() {
  return "sci_pt:int";
}

bool mars_calibrate::publish_grammar(umbc::command_grammar& cg) {
  string func_string=get_name()+"(";
  // now get the arguments...
  func_string+=grammar_argspec();
  func_string+=")";
  cg.add_production(grammar_commname(),func_string);
  // should not be hardwired!
  cg.add_production("sci_pt:int","0..3");
  return true;
}

// PANORAMIC

string mars_take_panoramic::describe() {
  return "(take_pano_action)";
}

bool mars_take_panoramic::simulate() {
  // cout << "simulating pano..." << endl;
  
  //  for (int i=0;i<mars_rover::PANO_IMG;i++) {
  if (!((mars_domain*)agent_of()->domain_of())->request_image_ambient(agent_of(),my_imagebank,mars_rover::PANO_IMG)) {
    // domain could not allow it....
    (static_cast<basic_message_box*>(basic_agent_of()->get_observable("status")))->attempt_set(mars_rover::STATUS_BANK_OVERFLOW);
    umbc::declarations::declare(personalize("bankOverflow"));
    return false;
  }
  else {
    uLog::annotate(MARSL_HOSTMSG,"[mars/mra]:: panoramic images supposedly taken correctly...");
    if (basic_agent_of()->get_observable("t_pano")->attempt_set(0.0))
      cout << "attempt set returned true : " << basic_agent_of()->get_observable("t_pano")->double_value() << endl;
    else
      cout << "attempt set returned false." << endl;
    return true;
  }
}

// SCIENCE

string mars_do_science::describe() {
  char cc[128];
  sprintf(cc,"(do_science_action L=%d>\n",get_arg_int());
  return cc;
}

string mars_do_science::grammar_argspec() {
  return "sci_pt:int";
}

bool mars_do_science::publish_grammar(umbc::command_grammar& cg) {
  string func_string=get_name()+"(";
  // now get the arguments...
  func_string+=grammar_argspec();
  func_string+=")";
  cg.add_production(grammar_commname(),func_string);
  // should not be hardwired!
  cg.add_production("sci_pt:int","0..3");
  return true;
}

bool mars_do_science::simulate() {
  int dst = get_arg_int();
  if (dst == MA_NO_ARG) {
    (static_cast<basic_message_box*>(basic_agent_of()->get_observable("status")))->attempt_set(mars_rover::STATUS_ARG_ERROR);
    return false;
  }
  else {
    mars_domain* mmd = static_cast<mars_domain*>(agent_of()->domain_of());
    if (mmd->request_image_targets(agent_of(),my_imagebank,
				   mars_rover::SCI_IMG,
				   (static_cast<basic_observable_state*>(basic_agent_of()->get_observable("cal")))->int_value())) {
      (static_cast<basic_message_box*>(basic_agent_of()->get_observable("status")))->attempt_set(mars_rover::STATUS_OKAY);
      return true;
    }
    else {
      (static_cast<basic_message_box*>(basic_agent_of()->get_observable("status")))->attempt_set(mars_rover::STATUS_SCI_ERROR);
      return false;
    }
  }
}

// MOVETO

string mars_moveto::describe() {
  char cc[128];
  sprintf(cc,"(moveto_action L=%d>\n",get_arg_int());
  return cc;
}

string mars_moveto::grammar_argspec() {
  return "way_pt:int";
}

bool mars_moveto::publish_grammar(umbc::command_grammar& cg) {
  string func_string=get_name()+"(";
  // now get the arguments...
  func_string+=grammar_argspec();
  func_string+=")";
  cg.add_production(grammar_commname(),func_string);
  cg.add_production("way_pt:int","0..7");
  return true;
}

bool mars_moveto::simulate() {
  mars_domain* damd = dynamic_cast<mars_domain*>(domain_of());
  if (damd) {
    int src = (int)damd->request_agent_property(agent_of(),MDAP_LOCATION);
    int dst = get_arg_int();
    int cost = damd->move_cost(src,dst);
    sprintf(uLog::annotateBuffer,"[mars/mra]:: the move cost (%d~>%d) is %d",src,dst,cost);
    uLog::annotateFromBuffer(MARSL_HOSTMSG);
    if (damd->can_set_agent_property(agent_of(),MDAP_LOCATION, dst)) {
      // check power...
      if (observable_utils::truly_gte(basic_agent_of()->get_observable("power"),cost)) {
	uLog::annotate(MARSL_HOSTMSG,"[mars/mra]:: the move is being simulated "+
			    basic_agent_of()->get_name()+
			    " (satisfactory)");
	// change loc...
	damd->attempt_set_agent_property(agent_of(),MDAP_LOCATION, dst);
	// power draw...
	basic_agent_of()->get_observable("power")->attempt_dec(cost);
	// decalibrate...
	(static_cast<basic_observable_state*>(basic_agent_of()->get_observable("cal")))->attempt_set(mars_rover::CALSTATE_UNCAL);
	// accumulate error...
	(static_cast<basic_observable_state*>(basic_agent_of()->get_observable("error")))->increment(cost*0.1);
	(static_cast<basic_message_box*>(basic_agent_of()->get_observable("status")))->attempt_set(mars_rover::STATUS_OKAY);
	return true;
      }
      else { // insufficient power
	uLog::annotate(MARSL_HOSTDBG,"[mars/mra]:: insufficient power to move "+
			    basic_agent_of()->get_name()+
			    " (disappointing)");
	(static_cast<basic_message_box*>(basic_agent_of()->get_observable("status")))->attempt_set(mars_rover::STATUS_NRG_FAILURE);
	return false;
      }
      // can this statement ever be reached?
      uLog::annotate(MARSL_HOSTEVENT,
		     "[mars/aact]::moveto generic simulation complete.");
    }
    else {
      // attempt to set loc failed
      uLog::annotate(MARSL_HOSTDBG,"Domain disallowed move for "+
			  basic_agent_of()->get_name()+
			  " (suspicious)");
      (static_cast<basic_message_box*>(basic_agent_of()->get_observable("status")))->attempt_set(mars_rover::STATUS_MOVE_FAILURE);
      return false;
    }
  }
  else {
    // could not cast to mars domain?
    uLog::annotate(MARSL_HOSTERR,"Could not cast domain for "+
			basic_agent_of()->get_name()+
			" to mars_domain (potentially catastrophic)");
    (static_cast<basic_message_box*>(basic_agent_of()->get_observable("status")))->attempt_set(mars_rover::STATUS_UNDEF_ERROR);
    return false;
  }
}
