#include "mcl/mcl_api.h"

#include "rover_ugpa.h"
#include "mars_domain.h"
#include "mars_agent_actions.h"
#include "rover_mcl_wrappers.h"

using namespace mcMars;

#include "mars_logging.h"
using namespace umbc;

mmcl::rover_mcl_control_layer::rover_mcl_control_layer(rover_mcl_wrapper* mcla,
						       controller* inna_cont)
  : mclAware::mcl_control_layer(mcla,inna_cont),my_state(STATE_MONITORING),
    my_rover_wrapper(mcla) {};

bool mmcl::rover_mcl_control_layer::control_in() {
  uLog::annotate(MONL_HOSTMSG,personalize("mcl layer online"));

  if (has_pending_tla()) {
    tlaspec()->prepare_for_activation();
    process_action(tlaspec()->underlying_action());
    advance_q();
    uLog::annotate(MONL_HOSTMSG,personalize("mcl layer activating " + 
					    tlaspec()->underlying_action()->get_name()));
    return true;
  }
  else return peel()->control_in();
}

bool mmcl::rover_mcl_control_layer::control_out() {
  bool rv = peel()->control_out();  

  // set up an update and monitor
  mclMA::observables::update mcl_update_obj;
  my_mclagent()->fill_out_update(mcl_update_obj);
  responseVector mclrv = mclMA::monitor(mcl_key_of(),
					mcl_update_obj);
  if (mclrv.empty()) {
    switch (my_state){
    case STATE_RESPONDING:
      if (!has_pending_tla()) {
	// RESPONDING means responding to MCL -- not responding and pending
	// a retry...
	my_state=STATE_MONITORING;
      }
      else {
	// still going, I guess...
      }
      break;
    case STATE_RETRY_PENDING:
      if (!has_pending_tla()) {
	peel()->tell("retry");
	my_state = STATE_RETRYING;
      }
      else { /* still going, I guess... */  }
      break;
    case STATE_RETRYING:
      // it would be preferable to be able to detect when the retry is done
      my_state=STATE_MONITORING;
      break;
    case STATE_MONITORING:
      break;
    default:
      sprintf(uLog::annotateBuffer,"mcl layer in unknown state: (code=0x%x)",
	      my_state);
      uLog::annotateFromBuffer(MONL_HOSTERR);
      my_state=STATE_MONITORING;
      break;
    }
  }
  else {
    responseVector::iterator mclrvi = mclrv.begin();
    while (mclrvi != mclrv.end()) {
      umbc::declarations::declare(personalize("mcl.responsecount"));
      mclMonitorResponse* tmr = (*mclrvi);
      if (tmr->recommendAbort())
	peel()->abort_current();
      if (!tmr->requiresAction()) // don't do anything
	{}
      else {
	mclMonitorCorrectiveResponse* tcr = 
	  dynamic_cast<mclMonitorCorrectiveResponse*>(tmr);
	if (tcr == NULL) {
	}
	else {
	  switch (tcr->responseCode()) {
	  case CRC_RESCUE:
	    add_aspec_back(new actionspec(my_wrapped_rover()->rover_of()->get_repair("rescue")));
	    peel()->note_preempted();
	    my_state = STATE_RETRY_PENDING;
	    break;
	  case CRC_TRY_AGAIN:
	    peel()->tell("retry");
	    my_state = STATE_RETRYING;
	    break;
	  default: {
	    sprintf(uLog::annotateBuffer,"%s being ignored: (ref=0x%lx code=0x%x) msg='%s'",
		    tmr->rclass().c_str(),tmr->referenceCode(),
		    tcr->responseCode(),tmr->responseText().c_str());
	    uLog::annotateFromBuffer(MONL_WARNING);
	    mclMA::suggestionIgnored(mcl_key_of(),tmr->referenceCode());
	    break;
	  }
	  }
	}
      }
      delete tmr;
      mclrvi = mclrv.erase(mclrvi);
    }
  }
  return rv;
}


bool mmcl::rover_mcl_control_layer::process_action(action* ua) {
  bool aarv = true;
  if (!ua)
    return false;
  else {
    aarv &= ua->sensorimotor_agent_of()->activate_action(ua);
  }
  if (aarv)
    umbc::declarations::declare(personalize("mcl.activation.ok"));
  else
    umbc::declarations::declare(personalize("mcl.activation.fail"));
  return aarv;
}

//////////////////////////////////////////////////////////////
// REACTIVE/MCL FUNCTIONS

// TRY AGAIN
//
// here's the thing -- are we going to try the action again, or the plan?
// I think there's an explicit replan response, so here is an attempt
// to retry the current action

void handleTryAgain(mclMonitorResponse *mmr, bool direct_mcl) {
  // now try to try again (using ref code in MMR)
  uLog::annotate(MARSL_HOSTMSG,"[mars/planbot]::Attempting to 'try_again'");
  /*
  if (curr_aspec() != NULL) {
    actionspec* copy_of_curr = curr_aspec()->clone();
    // copy_of_curr->set_mclref(mmr->referenceCode());
    add_aspec_front(copy_of_curr);
    if (direct_mcl) {
      mclMA::suggestionImplemented(mcl_agentspec(),mmr->referenceCode());
    }
  }
  else {
    uLog::annotate(MARSL_WARNING,"[mars/planbot]::most recent action not stored (try_again fails)");
    if (direct_mcl) {
      mclMA::suggestionFailed(mcl_agentspec(),mmr->referenceCode());
    }
  }
  */
}

// TRY AGAIN
//

void handleSensorReset(mclMonitorResponse *mmr) {
  /*
    basic_agent* ba = static_cast<basic_agent*>(agent_of());
    actionspec* tra = new actionspec(ba->get_repair("sensReset"));
    // tra->set_mclref(mmr->referenceCode());
    preempt(tra);
  */
  /*
  for (unsigned int i=0;i < sm_agent_of()->observables()->size();i++) {
    observable*    q   = ((basic_agent*)sm_agent_of())->get_observable_n(i);
    breakable* qab = dynamic_cast<breakable*>(q);
    if ((qab != NULL) && qab->is_broken()) {
      sprintf(uLog::annotateBuffer,"[mars/planbot]:: fixing observable '%s'...", q->get_name().c_str());
      uLog::annotateFromBuffer(MARSL_HOSTMSG);
      qab->fix_it();
      if (qab->is_broken())
	uLog::annotate(MARSL_HOSTERR,"[mars/planbot]:: observable is still broken?");
      else
	uLog::annotate(MARSL_HOSTMSG,"[mars/planbot]:: observable is fixed");
    }
  }
  return;
  */
}

// SENSOR DIAGNOSTIC
//

void handleSensorDiag(mclMonitorResponse *mmr) {
  uLog::annotate(MARSL_HOSTVRB,"[mars/planbot]:: handling sensor diagnostic code...");
  /*
  for (unsigned int i=0;i<sm_agent_of()->observables()->size();i++) {
    observable*    q   = ((basic_agent*)sm_agent_of())->get_observable_n(i);
    breakable* qab = dynamic_cast<breakable*>(q);
    if ((qab != NULL) && qab->is_broken()) {
      sprintf(uLog::annotateBuffer,
	      "[mars/planbot]:: sensor '%s' has been found to be broken...",
	      q->get_name().c_str());
      uLog::annotateFromBuffer(MARSL_HOSTMSG);
      mclMA::provideFeedback(mcl_agentspec(),true,mmr->referenceCode());
      return;
    }
  }
  mclMA::provideFeedback(mcl_agentspec(),false,mmr->referenceCode());
  return;
  */
  // mclMA::suggestionIgnored(mcl_agentspec(),mmr->referenceCode());
  // handleTryAgain(mmr);
}

// TRY AGAIN
//

void handleEffectorDiag(mclMonitorResponse *mmr) {
  //  mclMA::suggestionIgnored(mcl_agentspec(),mmr->referenceCode());
  // handleTryAgain(mmr);
}

// TRY AGAIN
//

void handleRebuild(mclMonitorResponse *mmr) {
  uLog::annotate(MARSL_HOSTMSG,"[mars/planbot]::Rebuilding Models (MCL recommendation)");
  /*
  for (action_list::iterator ali = sm_agent_of()->actions()->begin();
       ali != sm_agent_of()->actions()->end();
       ali++) {
    // uh oh, have to assume it's a mars domain action
    set_model(*ali,((mars_domain_action*)(*ali))->cheat_get_model());
  }
  // now suck in the extraneous models
  mars_domain* damd = dynamic_cast<mars_domain*>(sm_agent_of()->domain_of());
  if (damd != NULL)
    damd->copy_models2controller(this);
  uLog::annotate(MARSL_HOSTMSG,"[mars/planbot]::Rebuilding models completed.");
  handleTryAgain(mmr,false);
  */
}

// TRY AGAIN
//

void handleAmend(mclMonitorResponse *mmr) {
  /*
  handleRebuild(mmr);
  */
}

/////////////// LEFTOVAS

/* here is the old try_again, reused for replan when that is ready 
void handleRePlan(mclMonitorResponse *mmr, bool direct_mcl) {
  // now try to try again (using ref code in MMR)
  uLog::annotate(MARSL_HOSTMSG,"[mars/planbot]::Attempting to 'rePlan'");
  if (!gs_nongoal(most_recent_goal)) {
    generalizedPlanFor(most_recent_goal);
    if (direct_mcl) {
      mclMA::suggestionImplemented(mcl_agentspec(),mmr->referenceCode());
    }
  }
  else {
    uLog::annotate(MARSL_WARNING,"[mars/planbot]::Most Recent Goal not stored (replan fails)");
    if (direct_mcl) {
      mclMA::suggestionFailed(mcl_agentspec(),mmr->referenceCode());
    }
  }
}
*/
