#include "substrate/dispatch_agent.h"
#include "substrate/accepts_dispatch.h"
#include "substrate/controller.h"
#include "umbc/token_machine.h"
#include "mcl/mcl_api.h"

using namespace MonCon;

controller*  basic_dispatch_agent::get_active_controller() { return ac; }

void basic_dispatch_agent::set_active_controller(controller * c) { ac=c; }

string basic_dispatch_agent::dispatch_command(string cmd_str) {
  umbc::tokenMachine tm(cmd_str);
  if (tm.moreTokens()) {
    string agnn = tm.nextToken();
    agent* agnt_da = domain_of()->get_agent(agnn);
    if (agnt_da != NULL) {
      string acmd = tm.rest();
      string response = agnt_da->process_command(acmd);
      if (dispatch::is_ignored_msg(response))
	return ignore_msg();
      else return response;
    }
    else { 
      return fail_msg("no agent '" + agnn + "' to accept dispatch.");
    }
  }
  else {
    return fail_msg("ill-formed dispatch: '" + cmd_str + "'");
  }
}

bool basic_dispatch_agent::publish_grammar(umbc::command_grammar& grammar) {
  if (get_active_controller()) {
    grammar.add_production(grammar_commname(),get_active_controller()->grammar_commname());
    return get_active_controller()->publish_grammar(grammar);
  }
  else
    return false;
}
