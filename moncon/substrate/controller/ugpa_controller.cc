#include "substrate/ugpa_controller.h"
#include "substrate/actions.h"
#include "substrate/moncon_logging.h"
#include "umbc/settings.h"
#include "mcl/mcl_api.h"

#include <stdio.h>

using namespace MonCon;
using namespace umbc;

////////////////////////////////////////////////////////////////
// PLAN CODE

bool ugpa_plan::execute_step(ugpa_controller* the_cont) {

  if (planstate == UGPA_PLAN_INITIALIZED) 
    planstate=UGPA_PLAN_RUNNING;

  if (planstate==UGPA_PLAN_RUNNING) {
    if (has_pending_tla()) {
      tlaspec()->prepare_for_activation();
      the_cont->process_action(tlaspec()->underlying_action());
      advance_q();
      return true;
    }
    else {
      // okay for whatever reason, there is no Top Level Action in the plan
      planstate=UGPA_PLAN_COMPLETED;
      return false;
    }
  }
  else {
    sprintf(uLog::annotateBuffer,"[mcs/ugpac]:: attempt to execute an ugpa plan step in a state is not RUNNING (state=%d)",planstate);
    uLog::annotateFromBuffer(MONL_HOSTERR);
    planstate=UGPA_PLAN_ERROR;
    return false;
  }
}

string ugpa_plan::describe() {
  char b[1024];
  sprintf(b,"<ugpa_plan %s>",its_goal_ref->get_name().c_str());
  return b;
}

////////////////////////////////////////////////////////////////
// GOAL CODE

bool ugpa_goal::selected(ugpa_controller* the_cont) {
  // now, we might not do anything here... or we might 
  // ask the_cont to replan, etc....
  //
  // the current model is always replan.
  return the_cont->replan(this);
}

bool ugpa_goal::unselected(ugpa_controller* the_cont) {
  // maybe not necessary...
  clear_plan();
  return true;
}

void ugpa_goal::clear_plan() { 
  if (the_plan) {
    the_plan->clear(); 
  }
  else 
    the_plan=make_initialized_plan();
};


////////////////////////////////////////////////////////////////
// CONTROLLER CODE
//
// control_xxx()

bool ugpa_controller::control_in() {
  // control_in activates an action

  sprintf(uLog::annotateBuffer,"%s conrol_in (state=%d)",
	  get_name().c_str(),ugpa_state);
  uLog::annotateFromBuffer(MONL_HOSTMSG);
  dump_action_q();
  
  // check 0: check pre-empted state
  if (ugpa_state == UGPA_STATE_PREEMPTED) {
    // replan...
    if (currently_executing_goal) {
      uLog::annotate(MONL_HOSTMSG,
		     "[mcs/upga]:: plan has been preempted, replanning...");
      currently_executing_goal->selected(this);
    }
    ugpa_state = UGPA_STATE_OK;
    return true;
  }
  else if (ugpa_state == UGPA_STATE_OK) {

    // check 1: priority goes to controller action q
    // the controller's action q is *not* for the planner to queue actions!
    // it's for preemption / reactive actions!
    if (has_pending_tla()) {
      // here we should look to see if a goal is being preempted...
      tlaspec()->prepare_for_activation();
      process_action(tlaspec()->underlying_action());
      advance_q();
      return true;
    }
    else if (is_pursuing_goal()) {
      // execute this:
      cout << " pursuing goal." << endl;
      if (!currently_executing_goal->execute_step(this)) {
	uLog::annotate(MONL_HOSTMSG,personalize("execute_step() returns false."));
	if (currently_executing_goal->is_done_executing()) {
	  uLog::annotate(MONL_HOSTMSG,personalize("current_goal is completed."));
	  // this could be a method, probably.
	  currently_executing_goal->unselected(this);
	  pending_goals.remove(currently_executing_goal);
	  processed_goals.push_back(currently_executing_goal);
	  currently_executing_goal=NULL;
	  umbc::declarations::declare(personalize("goal.processed"));
	  return true;
	}
	else {
	  // oh, so execute returned fail, but the goal is not done?
	  // not good.
	  uLog::annotate(MONL_HOSTERR,personalize("current_goal is in a problematic state (aborting)."));
	  currently_executing_goal->unselected(this);
	  pending_goals.remove(currently_executing_goal);
	  aborted_goals.push_back(currently_executing_goal);
	  currently_executing_goal=NULL;
	  umbc::declarations::declare(personalize("goal.auto-aborted"));
	  return true;
	}
      }
      else {
	// execute step returns true (everything is okay)
	return true;
      }
    }
    else if (has_pending_goal()) {
      // pick a pending goal and start it up
      currently_executing_goal = pending_goals.front();
      currently_executing_goal->selected(this);
      uLog::annotate(MONL_HOSTMSG,
		     "selected new goal: " + 
		     currently_executing_goal->get_name());
      return true;
    }
    else return false;
  }
  else {
    uLog::annotate(MONL_HOSTERR,personalize("upga_state is untrapped!"));
    return false;
  }
}

bool ugpa_controller::control_out() {
  bool rv = sm_agent_of()->deactivate_action();
  sprintf(uLog::annotateBuffer,"%s conrol_in (state=%d)",
	  get_name().c_str(),ugpa_state);
  uLog::annotateFromBuffer(MONL_HOSTMSG);
  return rv;
}

bool ugpa_controller::abort_current() {
  clear_action_q();
  currently_executing_goal->unselected(this);
  pending_goals.remove(currently_executing_goal);
  aborted_goals.push_back(currently_executing_goal);
  if (aborted_goals.size() > ag_maxsize) {
    ugpa_goal* foo = aborted_goals.front();
    aborted_goals.pop_front();
    delete foo;
  }
  currently_executing_goal=NULL;
  umbc::declarations::declare(personalize("goal.aborted"));
  return true;
}

bool ugpa_controller::tell(string message) {
  if (message == "retry") {
    uLog::annotate(MONL_HOSTMSG,personalize("someone is telling me to retry."));
    if (aborted_goals.empty()) {
      uLog::annotate(MONL_HOSTERR,personalize("I can't 'retry' with no aborted goals..."));
      uLog::annotate(MONL_WARNING,personalize("behavior in this case is to attempt next pending goal."));
      if (has_pending_goal()) {
	// pick a pending goal and start it up
	currently_executing_goal = pending_goals.front();
	currently_executing_goal->selected(this);
	uLog::annotate(MONL_HOSTMSG,
		       "selected new goal: " + 
		       currently_executing_goal->get_name());
	return true;
      }
      else {
	uLog::annotate(MONL_WARNING,personalize("nothing to do."));
	return false;
      }
    }
    else {
      currently_executing_goal = aborted_goals.back();
      aborted_goals.pop_back();
      currently_executing_goal->selected(this);
      ugpa_state = UGPA_STATE_OK;
      uLog::annotate(MONL_HOSTMSG,
		     personalize("retry processed for: " + 
				 currently_executing_goal->get_name()));
      return true;
    }
  }
  else return false;
}

////////////////////////////////////////////////////////////////
// management

void ugpa_controller::preempt(actionspec* aspec) {
  uLog::annotate(MONL_HOSTMSG,"preempting "+get_name()+" for "+aspec->to_string());
  ugpa_state = UGPA_STATE_PREEMPTED;
  // check for existing goal
  if (currently_executing_goal) 
    currently_executing_goal->preempt_goal();
  // uses the controller action_q
  add_aspec_back(aspec);
}

////////////////////////////////////////////////////////////////
// helper codes

bool ugpa_controller::has_pending_goal() {
  return !pending_goals.empty();
}

bool ugpa_controller::is_idle() {
  return (currently_executing_goal == NULL);
}

bool ugpa_controller::is_pursuing_goal() {
  return (currently_executing_goal != NULL);
}

bool ugpa_controller::process_action(action* ua) {
  bool aarv = true;
  if (!ua)
    return false;
  else {
    aarv &= ua->sensorimotor_agent_of()->activate_action(ua);
  }
  if (aarv)
    umbc::declarations::declare(personalize("actionActivation.ok"));
  else
    umbc::declarations::declare(personalize("actionActivation.fail"));
  return aarv;
}

/// ###mcl-EXTRACTION
/*
bool ugpa_controller::process_action_mcl(action* ua,has_action_models *h) {
  if (settings::getSysPropertyBool("moncon.usemcl",true)) {
    action* actual_action = (!ua) ? NULL : action_wrappers::get_core_action(ua);
    if (!actual_action) {
      if (ua)
	uLog::annotate(MONL_WARNING,"[mcs/ugpa]:: '"+ua->get_name()+
		       ": in pa_MCL, core_action is NULL?");
      else 
	uLog::annotate(MONL_WARNING,"[mcs/ugpa]:: trying to process for MCL with NULL action?");
      return false;
    }
    else {
      string mcl_key="";
      if (actual_action && actual_action->sensorimotor_agent_of())
	mcl_key=actual_action->sensorimotor_agent_of()->mcl_agentspec();
      uLog::annotate(MONL_HOSTMSG,"[mcs/ugpa]:: '"+actual_action->get_name()+
		     "' -- invoking MCL methods.");
      {
	// here is where the action model gets done up
	if (h &&
	    (h->get_model(actual_action) != NULL) &&
	    (h->get_model(actual_action)->tell_mcl(mcl_key))) {
	  // now, we should have just created an expectation group, 
	  // so we can set its property vectors
	  
	  mclMA::setEGProperty(mcl_key,h->get_model(actual_action)->mcl_eg_key_for(),
			       PCI_INTENTIONAL,PC_YES);
	  mclMA::setEGProperty(mcl_key,h->get_model(actual_action)->mcl_eg_key_for(),
			       PCI_ACTION_IN_PLAY,PC_YES);
	}
	return true;
      }
    }
  }
  return false;
}
*/

bool ugpa_controller::initialize() {
  return true;
}

/// ###mcl-EXTRACTION
/*
bool ugpa_controller::declare_mcl() {
  if ((mcl_agentspec() != "") &&
      (settings::getSysPropertyBool("moncon.usemcl",true))) {
    mclMA::initializeMCL(mcl_agentspec(),0);    
    mclMA::setPropertyDefault(mcl_agentspec(),CRC_TRY_AGAIN,PC_YES);
    mclMA::setPropertyDefault(mcl_agentspec(),CRC_REBUILD_MODELS,PC_YES);
    return true;
  }
  else return false;
}
*/

////////////////////////////////////////////////////////////////
// TIMELINE/GOAL CODE

void ugpa_timeline_goal::fire() {
  if (owner_controller && the_goal) {
    uLog::annotate(MONL_MSG,"[mars/mctime]:: a goal was added to the goalQ by a timeline event.");
    // the proper way to do it is to clone the goal here, but....
    // ugpa_goal* g = the_goal->clone();

    owner_controller->add_goal(the_goal);

    // this is to prevent the goal from possibly being accessed after the 
    // ugpa goal q manager has deleted it (the ugpa q assumes total control)
    the_goal = NULL; 

  }
  else {
    uLog::annotate(MONL_WARNING,"[mars/mctime]:: a goal event is firing but either owner_controller or the_goal is NULL.");
  }
}

string ugpa_timeline_goal::describe() {
  char b[512];
  sprintf(b,"<event[%d] _goal_ %s>",fires,the_goal->describe().c_str());
  return b;
}

void ugpa_timeline_goal::dump() {
  cout << describe() << endl;
}
