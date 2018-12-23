#include "umbc/exceptions.h"
#include "substrate/controller.h"
#include "substrate/agent.h"
#include "mars_rover.h"
#include "mars_agent_sensors.h"
#include "mars_rover_actions.h"
#include "mars_logging.h"
#include "umbc/token_machine.h"

// #include "substrate/mcl_wrappers.h"
// #include "mcl/mcl_api.h"

using namespace mcMars;
using namespace umbc;

int mars_rover::MAX_NRG  = 100;
int mars_rover::MAX_BANK =  __MR_IMGBANK_SIZE;
int mars_rover::PANO_IMG =   6;
int mars_rover::SCI_IMG  =   4;

mars_rover::mars_rover(string name, mars_domain* d) : 
  basic_agent(name,d),model_conduit(d),the_bank(MAX_BANK),my_mars_domain(d) {
  // auto-init mars with sensors and actions....
  if (!(build_sensors()) || !(build_effectors()) || !build_repair_actions())
    umbc::exceptions::signal_exception("failed to build rover's mc section.");
}

bool mars_rover::build_sensors() {
  // build sensors
  basic_observable* mds = NULL;
  s.push_front(new breakable(*(new uniformly_noisy(*(mds=new mars_resource("power",MAX_NRG,this)),0.1))));
  s.push_front(new breakable(*(mds=new ib_capacity_sensor("bank",the_bank,this))));
  s.push_front(mds=new mars_env_sensor("location",1.0,this,MDAP_LOCATION));
  s.push_front(mds=new mars_db_timer("t_pano",this,"simStep"));
  s.push_front(mds=new mars_db_timer("t_xmit",this,"simStep"));
  s.push_front(mds=new mars_state("error",0.0,this));
  s.push_front(mds=new mars_state("cal",(double)CALSTATE_UNCAL,this));
  s.push_front(mds=new mars_msgbox("status",(double)STATUS_OKAY,this));
  s.push_front(mds=new controlling_layer_observable("control",this));
  // mds->set_mcl_sclass(SC_SPATIAL);
  // mds->set_mcl_data_type(DT_SYMBOL);
  // mds->set_mcl_sclass(SC_MESSAGE);
  return true;
}

bool mars_rover::build_effectors() {
  a.push_front(new mars_moveto(this));
  a.push_front(new mars_do_science(this,the_bank));
  a.push_front(new action_wrappers::breakable_layer(new mars_calibrate(this)));
  a.push_front(new action_wrappers::breakable_layer(new mars_recharge(this)));
  a.push_front(new mars_take_panoramic(this,the_bank));
  a.push_front(new action_wrappers::intermittent_layer(new mars_xmit(this,the_bank),0.0));
  a.push_front(new mars_localize(this));
  return true;
}

bool mars_rover::build_repair_actions() {
  r.push_back(new mars_rebuild_models(this));
  r.push_back(new mars_sensor_reset(this));
  r.push_back(new mars_effector_reset(this));
  r.push_back(new mars_rescue(this));
  return true;
}

bool mars_rover::busy()  { return get_active_controller()->busy(); }

string mars_rover::process_command(string command) {
  tokenMachine tm(command);
  string t1 = tm.nextToken();
  if (t1 == "ack")
    if (ack()) return success_msg(); else return fail_msg("agent failed to ack()");
  else if (t1 == "sv") {
    // print sensor vector
    return success_msg(sv_as_str());
  }
  else {
    cout << "[mc/marsR]:: " << get_name() << " delegates '" << command << "' to basic_agent" << endl;
    return basic_agent::process_command(command);
  }
}

bool mars_rover::publish_grammar(umbc::command_grammar& grammar) {
  // these are pretty general but are actually handled by the rover...
  grammar.add_production(grammar_commname(),"sv");
  grammar.add_production(grammar_commname(),"ack");
  grammar.add_production(grammar_commname(),get_active_controller()->grammar_commname());
  get_active_controller()->publish_grammar(grammar);
  return true;
}

string mars_rover::describe() {
  if (get_active_controller())
    return "(rover_agent "+get_active_controller()->get_name()+")";
  else
    return "(rover_agent !no_controller)";
}

bool mars_rover::initialize() {
  bool scrv = basic_agent::initialize();
  // if the rover needs to do anything....
  return scrv;

}

bool mars_rover::simulate() {
  uLog::annotate(MARSL_HOSTMSG,get_name()+": "+sv_as_str());
  uLog::annotate(MARSL_HOSTMSG,get_name()+": "+ev_as_str());
  return basic_agent::simulate();
}

timeline_entry* mars_rover::parse_tle_spec( int firetime, timeline* tl,
				       string tles ) {
  
  // if it gets here then probably we should check with the controller
  return basic_agent::parse_tle_spec( firetime, tl, tles );
}
