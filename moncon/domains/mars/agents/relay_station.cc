#include "substrate/null_controller.h"
#include "relay_station.h"
#include "mars_rover.h"
#include "mars_agent_actions.h"
#include "mcl/mcl_api.h"
#include "mars_logging.h"
#include "umbc/token_machine.h"

using namespace mcMars;
using namespace umbc;

relay_station::relay_station(string name, domain *d) : basic_agent(name,d){ 
  set_active_controller(new null_controller(this,name+"_c"));
}

bool relay_station::publish_grammar(umbc::command_grammar& grammar) {
  grammar.add_production(grammar_commname(),"ack");
  return true;
}

string relay_station::process_command(string command) {
  tokenMachine tm(command);
  string t1 = tm.nextToken();
  if (t1 == "ack")
    return success_msg();
  else
    return ignore_msg();
}

#define RS_GS_A0_PANO 0x01
#define RS_GS_A0_TARG 0x02

bool relay_station::receive(image_bank& ib) {
  // if there are no images then don't bother
  if (ib.get_numimgs() > 0) {
    for ( int i = 0; i< ib.get_numimgs(); i++) {
      imagetype img = ib.get_image(i);
      sprintf(uLog::annotateBuffer,
	      "[%s]: received image %08d",get_name().c_str(),img);
      uLog::annotateFromBuffer(MARSL_MSG);
      umbc::declarations::declare(personalize("images.received"));
      // this is maybe not kosher, I am using serial=0 to evaluation goal-sat
      // what is not kosher is that I don't care how many images are rcvd (yet)
      if (img_serial(img) == 0) {
	umbc::declarations::declare(personalize("imageblocks.received"));
	goalspec matching_goal_spec = make_gs((img_is_pano(img) ? 
					       RS_GS_A0_PANO : RS_GS_A0_TARG),
					      img_trg(img));
	vector<goalspec>::iterator ogvi = outstanding_goals.begin();
	bool done = false;
	while (!done && (ogvi != outstanding_goals.end())) {
	  if (gs_equal(*ogvi,matching_goal_spec)) {
	    umbc::declarations::declare(personalize("goals.satisfied"));
	    done=true;
	    outstanding_goals.erase(ogvi);
	  }
	  ogvi++;
	}
      }
      else {
	// ignore serials > 0
      }
    }
  }
  umbc::declarations::set_dec_cnt(personalize("goals.ogsize"),
				  outstanding_goals.size());
  return true;
}

bool relay_station::receive_goal(goalspec gs,bool is_pano) {
  umbc::declarations::declare_n(personalize("goals.received"),1);
  uLog::annotate(MARSL_HOSTMSG,personalize(": goal received "));
  // for our own purposes we overwrite the agent-specific action word
  // with a PANO/TARG code
  goalspec rs_goalspec = make_gs((is_pano ? RS_GS_A0_PANO : RS_GS_A0_TARG),
				 gs_arg(gs));
  outstanding_goals.push_back(rs_goalspec);
  umbc::declarations::set_dec_cnt(personalize("goals.ogsize"),
				  outstanding_goals.size());
  return true;
}

string relay_station::describe() {
  if (get_active_controller())
    return "<relay_station "+get_active_controller()->get_name()+">";
  else
    return "<relay_station !no_controller>";
}
