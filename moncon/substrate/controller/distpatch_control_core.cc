#include "substrate/dispatch_control_core.h"
#include "umbc/command_grammar.h"
#include "umbc/token_machine.h"
#include "mcl/mcl_api.h"

using namespace MonCon;
using namespace umbc;

string dispatch_control_core::generate_grammar() {
  umbc::command_grammar tcg;
  // add my own commands here... 
  // okay, so the nonterminal for the active dispatcher will come here
  tcg.add_production("S",agent_of()->grammar_commname());
  agent_of()->publish_grammar(tcg);

  return tcg.to_string();
}

string dispatch_control_core::show_agents() {
  string rv="";
  agent_list al = agent_of()->domain_of()->list_agents();
  for (agent_list::iterator ali = al.begin();
       ali != al.end();
       ali++) {
    rv+= "  \"" + (*ali)->get_name() + "\" ~> " + (*ali)->describe() + "\n";
  }  
  return rv;
}

string dispatch_control_core::send_agent_command(string commstr) {
  tokenMachine cst(commstr);
  cst.nextToken(); // strip off 'send'
  string agt = cst.nextToken();
  string cmd = cst.rest();
  agent* agt_tgt = agent_of()->domain_of()->get_agent(agt);
  if (agt_tgt != NULL) {
    // accepts_dispatched_commands* tgt_as_adc = 
    // dynamic_cast<accepts_dispatched_commands*>(agt_tgt);
    // if (tgt_as_adc != NULL)
    // return tgt_as_adc->process_command(cmd);
    return agt_tgt->process_command(cmd);
    // else
    // return dispatch::generate_fail_msg("agent '"+agt+"' apparently does not accept command dispatch.");
  }
  else {
    return dispatch::generate_fail_msg("agent '"+agt+"' not found.");
  } 
}

timeline_entry* dispatch_control_core::parse_tle_spec( int firetime, 
						      timeline* tl,
						      string tles ) {
  return controller::parse_tle_spec(firetime, tl, tles);
}
