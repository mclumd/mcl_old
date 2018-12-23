#include "rover_ugpa.h"
#include "mars_rover_actions.h"
#include "mars_domain.h"
#include "mcl/mcl_api.h"

#include <errno.h>

using namespace mcMars;

#include "umbc/text_utils.h"
#include "umbc/token_machine.h"
#include "umbc/exceptions.h"
#include "mars_logging.h"
using namespace umbc;

extern char timeline_error_msg[1024];

rover_ugpa_controller::rover_ugpa_controller(mars_rover* rover) : 
  ugpa_controller(rover,"ugpa_rc"),
  shares_mars_domain_model(rover->mars_domain_of()),
  my_rover(rover) {

  // uLog::annotate(MONL_HOSTERR,"UGPA Controller wants to copy the domain models but that is not implemented!");

  // suck in the domain models
  grab_all_models();
}

void rover_ugpa_controller::grab_all_models() {
  mars_domain* damd = dynamic_cast<mars_domain*>(rover_of()->domain_of());
  if (!damd) exceptions::signal_exception("rover_ugpa domain is not a mars domain?");
  mars_domain_model_suite* m = 
    static_cast<mars_domain_model_suite*>(damd->provide_ptr_to_model(rover_of()->domain_of()));
  if (m) m->copy_to(my_mars_domain_model);
  else exceptions::signal_exception("rover_ugpa can't get models from domain.");
}

bool rover_ugpa_controller::initialize() {
  bool rv = ugpa_controller::initialize();
  return rv;
}

/////////////////////////////////////////////////////////////////
// STRING PROCESSING -- 
//  PROCESS_COMMAND ... FROM TTY / SOCKET
//

string text2error_string = "unset error code.";

string rover_ugpa_controller::process_command(string command) {
  tokenMachine tm(command);
  string t1 = tm.nextToken();
  if (t1 == "cack") {
    if (busy())
      return success_msg("busy");
    else
      return success_msg("idle");
  }
  else if (tm.containsToken("at")) {
    string tle_version = agent_command2tles(agent_of(),command);
    cout << "AT DETECTED! : '" << tle_version << "'" << endl;
    tlFactory::do_not_exit_on_parse_failure();
    if (tlFactory::read_entry_to_timeline(*(domain_of()->get_timeline()),tle_version))
      return success_msg("");
    else
      return fail_msg(timeline_error_msg);
    // now somehow add it to the domain timeline?
    // domain_of->process_tles(tle_version);
  }
  else if (t1 == "action") {
    // uLog::annotate(MARSL_HOSTMSG,"[mars/rugpa]::attempt to parse actionspec.");
    actionspec* nasp = has_action_q::fp_string2aspec(tm.rest(),(basic_agent*)agent_of());
    // if (accept_action_string(tm.rest(),(basic_agent*)agent_of()))
    if (nasp) {
      preempt(nasp);
      return success_msg();
    }
    else
      return fail_msg("unparsable action-spec '" + tm.rest() + "'");
  }
  else if (t1 == "goal") {
    rover_goal* trg = text2goal(tm.rest());

    if (trg != NULL) {
      add_goal(trg);
      
      // now test here for imaging goal...
      // bad, because it checks against MR (Mars Rover) codes
      if (trg->action_code() == MR_TAKE_PANORAMIC) {
	goalspec gs = make_gs(trg->action_code(),trg->get_arg(0));
	static_cast<mars_domain*>(domain_of())->broadcast_imaging_goal_rcvd(agent_of(),gs,true);
      }
      else if (trg->action_code() == MR_DO_SCIENCE) {
	goalspec gs = make_gs(trg->action_code(),trg->get_arg(0));
	static_cast<mars_domain*>(domain_of())->broadcast_imaging_goal_rcvd(agent_of(),gs,false);
      }
      else 
	uLog::annotate(MARSL_HOSTMSG,"[" + get_name() + "]:: not broadcasting goal");
      
      return success_msg();
    }
    else return fail_msg(text2error_string);
  }
  return fail_msg("unhandled command '"+t1+"'");
}

rover_goal* rover_ugpa_controller::text2goal(string the_string) {
  tokenMachine tm(the_string);

  string goal_str = tm.nextToken();
  string action_str = textFunctions::getFunctor(goal_str);
  int action_code  = mars_rover_action::action_name2code(action_str);

  if (action_code == -1) {
    uLog::annotate(MARSL_WARNING,"[mars/rugpa]::"+get_name()+" can't parse goal action '"+action_str+"'.");
    text2error_string=get_name()+" can't parse goal action '"+action_str+"'.";
    return NULL;
  }
  else {
    rover_goal* trg = rover_goal::make_goal(action_code);
    
    paramStringProcessor psp(textFunctions::getParameters(goal_str));
    int pctr=0;
    if (psp.hasMoreParams()) {
      string arg_str = psp.getNextParam();
      if (psp.isLegit(arg_str)) {
	errno = 0;
	int arg = textFunctions::numval(arg_str);
	if (errno != 0) {
	  uLog::annotate(MARSL_WARNING,"[mars/rugpa]::can't parse goal command arg.");
	  delete trg;
	  text2error_string=get_name()+" can't parse goal arg '"+arg_str+"'.";
	  return NULL;
	}
	else {
	  trg->set_arg(pctr,arg);
	  pctr++;
	}
      }
    }
    // okay, now process keywords
    if (tm.containsToken("loc")) {
      string location = tm.keywordValue("loc");
      int loc = textFunctions::numval(location);
      trg->set_action_loc(loc);
    }
    
    return trg;
  }

  text2error_string=get_name()+" this error can't happen: '"+action_str+"'.";
  return NULL;

}

timeline_entry * rover_ugpa_controller::parse_tle_spec( simTimeType firetime, 
							timeline* tl, 
							string tles ) {
  tokenMachine q(tles);
  string comm_tk   = q.nextToken();
  if (comm_tk == "goal") {
    string aspec = q.nextToken();
    string astr = textFunctions::getFunctor(aspec);
    int action_code  = mars_rover_action::action_name2code(astr);
    if (action_code == -1) {
      uLog::annotate(MARSL_WARNING,"[mars/rugpa]::can't parse goal command.");
      sprintf(timeline_error_msg,"action name '%s' could not be recognized.",
	      astr.c_str());
      return NULL;
    }

    rover_goal* trg = rover_goal::make_goal(action_code);
    paramStringProcessor psp(textFunctions::getParameters(aspec));

    int pctr=0;
    if (psp.hasMoreParams()) {
      string arg_str = psp.getNextParam();
      if (psp.isLegit(arg_str)) {
	errno = 0;
	int arg = textFunctions::numval(arg_str);
	if (errno != 0) {
	  uLog::annotate(MARSL_WARNING,"[mars/rugpa]::can't parse goal command's argument");
	  sprintf(timeline_error_msg,"goal-action arg could not be parsed.");
	  return NULL;
	}
	else {
	  trg->set_arg(pctr,arg);
	  pctr++;
	}
      }
    }
    
    timeline_entry* rv = new rover_ugpa_timeline_goal(firetime,tl,trg,this);
    return rv;

  }
  else {
    // command not handled at controller level...
    sprintf(timeline_error_msg,"failed to recognize any controller-command.");
    return NULL;
  }
}

bool rover_ugpa_controller::publish_grammar(umbc::command_grammar& grammar) {
  string agnt_action_str = agent_of()->class_name()+"_action";
  // string agnt_goal_str = agent_of()->class_name()+"_goal";
  grammar.add_production(grammar_commname(),"cack");
  // now we need to publish the agent action and goal commands
  sensorimotor_agent* sma = dynamic_cast<sensorimotor_agent*>(agent_of());
  if (sma != NULL) {
    grammar.add_production(grammar_commname(),"action "+agnt_action_str);
    grammar.add_production(grammar_commname(),"goal "+agnt_action_str+" [at-clause] [loc-clause]");
    grammar.add_production("at-clause","at time:int");
    grammar.add_production("loc-clause","loc way_pt:int");
    for (int i = 0; i < sma->num_actions(); i++) {
      grammar.add_production(agnt_action_str,sma->get_action(i)->grammar_commname());
      // grammar.add_production(agnt_goal_str,sma->get_action(i)->grammar_commname());
      sma->get_action(i)->publish_grammar(grammar);
      }
    /* this is the old way of doing it....
      grammar.add_production(agnt_action_str,sma->get_action(i)->get_name()+" "+ba->grammar_argspec());
      // grammar.add_production(agnt_goal_str,sma->get_action(i)->get_name()+" "+ba->grammar_argspec());
      */
  }
  return true;
}

bool rover_ugpa_controller::abort_current() {
  return ugpa_controller::abort_current();
}

bool rover_ugpa_controller::tell(string message) {
  bool rv = ugpa_controller::tell(message);
  if (rv)
    return true;
  else {
    uLog::annotate(MARSL_HOSTERR,
		   personalize("nobody handled "+message+"."));
    return false;
  }
}
