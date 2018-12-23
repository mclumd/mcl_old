#include "substrate/domain_configurator.h"
#include "substrate/dispatch_agent.h"
#include "substrate/tty_dispatch_control.h"
#include "substrate/socket_dispatch_control.h"

#include "mars_domain.h"
#include "mars_domain_timeline.h"
#include "mars_events.h"
#include "mars_rover.h"
#include "rover_ugpa.h"
#include "rover_reactive_layer.h"
#include "rover_maintenance_layer.h"
#include "rover_opportunistic_layer.h"
#include "relay_station.h"

#include <errno.h>

using namespace mcMars;

#include "mars_logging.h"
#include "umbc/text_utils.h"
#include "umbc/token_machine.h"
using namespace umbc;

mars_domain::mars_domain() : basic_domain("marsSim"),
			     model_oracle(),has_mars_domain_model(this)
			     // num_waypoints(0),reachability(NULL),
			     // num_scipoints(0),sciability(NULL),
			     // move_costs(NULL),localizable(NULL),
			     // rechargable(NULL),action_costs(NULL) 
{
  cout << "[mc/mdom]:: created domain @0x" << hex << (long)this << endl;
  cout << "[mc/mdom]:: name is '" << get_name() << "'"
       << " or '" << name << "'" << endl;
  // generate map
  make_default_map();
  // set up the action model oracle
  init_action_oracle();
}

void mars_domain::set_num_waypoints(int x) {
  my_mars_domain_model.set_num_waypoints(x);
}

void mars_domain::set_num_scipoints(int x) {
  my_mars_domain_model.set_num_scipoints(x);
}

bool mars_domain::simulate() {
  bool srv = basic_domain::simulate();
  // execute action
  for (agent_list::iterator ali = a_list.begin();
       ali != a_list.end();
       ali++) {
    // cout << "supposed to simulate agent action here" << endl;
    (*ali)->simulate();
    // ((mars_domain_action*)active_action)->simulate();
  }
  // check for event
  mars_anomaly::simulate_random_events(this);
  return srv;
}

void mars_domain::force_event(int event_code,long seed) { 
  mars_anomaly a(this);
  a.force_event(event_code,seed); 
}

void mars_domain::set_reachable(int s,int d,int cost) {
  my_mars_domain_model.set_reachable(s,d,cost);
}

void mars_domain::set_sciable(int maploc,int sci,bool v) {
  my_mars_domain_model.set_calable(maploc,sci,v);
}

bool mars_domain::is_reachable(int s,int d) {
  return my_mars_domain_model.is_reachable(s,d);
}

bool mars_domain::is_sciable(agent* a,int sci) {
  return is_sciable((int)get_agent_property(a,MDAP_LOCATION),sci);
}
bool mars_domain::is_sciable(int maploc,int sci) {
  return my_mars_domain_model.is_calable(maploc,sci);
}

int mars_domain::move_cost(int s,int d) {
  return my_mars_domain_model.move_cost(s,d);
}

// int mars_domain::scindex(int m,int sci) { return (m*num_scipoints)+sci; }
// int mars_domain::rindex(int s,int d) { return (s*num_waypoints)+d; }

void mars_domain::set_rechargable(int maploc,bool v) {
  my_mars_domain_model.set_rechargable(maploc,v);
}

bool mars_domain::is_rechargable(agent* a) {
  return is_rechargable((int)get_agent_property(a,MDAP_LOCATION));
}

bool mars_domain::is_rechargable(int maploc) {
  return my_mars_domain_model.is_rechargable(maploc);
}

void mars_domain::set_localizable(int maploc,bool v) {
  my_mars_domain_model.set_localizable(maploc,v);
}

bool mars_domain::is_localizable(agent* a) {
  return mars_domain::is_localizable((int)get_agent_property(a,MDAP_LOCATION));
}

bool mars_domain::is_localizable(int s) {
  return my_mars_domain_model.is_localizable(s);
}

bool mars_domain::can_calibrate(agent* a,int targ) {
  return mars_domain::can_calibrate((int)get_agent_property(a,MDAP_LOCATION),targ);
}

bool mars_domain::can_calibrate(int src,int targ) {
  return is_sciable(src,targ);
}

void mars_domain::make_default_map() {
  set_num_waypoints(8);

  set_reachable(0,1,8);
  set_reachable(1,2,10);
  set_reachable(3,4,10);
  set_reachable(4,5,8);
  set_reachable(4,2,10);
  set_reachable(2,7,8);
  set_reachable(7,6,8);
  set_reachable(2,6,10);

  dump();

  set_num_scipoints(3);
  set_sciable(5,2,true);
  set_sciable(7,2,true);
  set_sciable(3,1,true);
  set_sciable(0,0,true);
  set_sciable(2,0,true);
  set_sciable(6,0,true);

  set_rechargable(0,true);
  set_rechargable(4,true);

  set_localizable(0,true);
  set_localizable(3,true);
  set_localizable(7,true);  

  // set_action_cost_vec();
}

/*
void mars_domain::set_action_cost_vec() {
  if (action_costs == NULL) 
    action_costs = new int[num_actions()];
  int i=0;
  for (action_list::iterator ali = actions()->begin();
       ali != actions()->end();
       ali++) {
    action_costs[i]=((mars_domain_action*)(*ali))->cost();
    i++;
  }  
}  
*/

string mars_domain::describe() {
  // uLog::annotateStart(MCLA_PRE);
  // *uLog::log << "Mars Rover domain configuration: " << endl;
  return "<mars_domain>";
  /*
  *uLog::log << sensors()->size() << " sensors..." << endl;
  for (sensor_list::iterator sli = sensors()->begin();
       sli != sensors()->end();
       sli++) {
    *uLog::log << (*sli)->get_name() << " = " << (*sli)->doubleValue() << endl;
  }
  */
  // dump_mxs();
  // uLog::annotateEnd(MCLA_PRE);
}

void mars_domain::dump_mxs() {
  uLog::annotateStart(UMBCLOG_PRE);
  *uLog::log << get_num_waypoints() << " move_mx:" << endl;
  *uLog::log << "   ";
  for (int p=0;p<get_num_waypoints() ;p++) {
    char b[32];
    sprintf(b," %2d  ",p);
    *uLog::log << b;
  }
  *uLog::log << endl;
  for (int ii=0;ii<get_num_waypoints() ;ii++) {
    char b[32];
    sprintf(b,"%2d ",ii);
    *uLog::log << b;
    for (int i=0;i<get_num_waypoints() ;i++) {
      if (is_reachable(i,ii)) {
	char b[32];
	sprintf(b,"%2d",move_cost(i,ii));
	*uLog::log << "(" << b << ") ";
      }
      else *uLog::log << " --  ";
    }
    *uLog::log << endl;
  }

  for (int s=0;s<get_num_scipoints() ;s++) {
    char b[32];
    sprintf(b,"s%1d ",s);
    *uLog::log << b;
    for (int w=0;w<get_num_waypoints() ;w++) {
      if (is_sciable(w,s)) {
	*uLog::log << " +++ ";
      }
      else *uLog::log << " --- ";
    }
    *uLog::log << endl;
  }
  
  *uLog::log << "r: ";
  for (int i=0;i<get_num_waypoints() ;i++) {
    if (is_rechargable(i))
      *uLog::log << " +++ ";
    else *uLog::log << " --- ";
  }
  *uLog::log << endl;
  *uLog::log << "l: ";
  for (int i=0;i<get_num_waypoints();i++) {
    if (is_localizable(i))
      *uLog::log << " +++ ";
    else *uLog::log << " --- ";
  }
  *uLog::log << endl;
  uLog::annotateEnd(UMBCLOG_PRE);
}

/*
void mars_domain::copy_models2controller(has_mars_models* hmm) {
  // cout << "copying domain models to controller...." << endl;
  // hmm->set_numlocs(num_waypoints);
  hmm->set_mcmx(new cost_mx_model(move_costs,num_waypoints,num_waypoints));
  hmm->set_cal_mx(new adj_mx_model(sciability,num_waypoints,num_scipoints));
  hmm->set_move_mx(new adj_mx_model(reachability,num_waypoints,num_waypoints));
  // hmm->set_action_cv(new cost_vector_model(action_costs,num_actions()));
  // cout << "copying recharge model." << endl;
  hmm->set_charge_pv(new pc_vector_model(rechargable,num_waypoints));
  // cout << "copying localize model." << endl;
  hmm->set_local_pv(new pc_vector_model(localizable,num_waypoints));
  // cout << "done." << endl;
}
*/

timeline_entry * mars_domain::parse_tle_spec( simTimeType firetime, 
					      timeline* tl, string tles ) {
  tokenMachine q(tles);
  string dom_tok = q.nextToken();
  if (dom_tok == "domain") {
    // okay, maybe we can parse this...
    string com_tok = q.nextToken();
    if (com_tok == "event") {
      string ecode_name = q.nextToken();
      int evcode = mars_anomaly::lookup_anomaly_code(ecode_name);
      if (evcode == -1) {
	// unknown eCode -- call the parent!
	return basic_domain::parse_tle_spec( firetime, tl, tles );
      }
      // known code, make a mars event
      mars_timeline_event* mte = 
	new mars_timeline_event( firetime, tl, evcode );
      while (q.moreTokens()) {
	string kw = q.nextToken();
	string kwv = q.nextToken();
	if (kw == "seed")
	  mte->set_seed(textFunctions::longval(kwv));
	else 
	  uLog::annotate(MARSL_WARNING,"[mars/dom]:: unknown timeline/event keyword "+kw);
      }
      // uLog::annotate(MARSL_HOSTMSG,"[mars/dom]:: adding event "+mte->describe());
      return mte;
    }
  }
  // call the parent's method
  return basic_domain::parse_tle_spec( firetime, tl, tles );
}

bool mars_domain::configure(string cs) {
  tokenMachine tl(cs);
  // tl.printWSource();
  string obj = tl.nextToken();
  // cout << obj << endl;
  if (obj == "agent") {
    string at = tl.nextToken(),
           aa = tl.nextToken();
    return create_agent(at,aa);    
  }
  else if (obj == "controller") {
    string ct = tl.nextToken(),
           ar = tl.nextToken(),
           ca = tl.nextToken();
    return create_controller(ct,textFunctions::dequote(ar),ca);
  }
  else {
    sprintf(domain_config::error_msg,
	    "unparsable domain cofiguration object '%s'",
	    obj.c_str());
    return false;
  }
  return true;  
}

bool mars_domain::create_agent(string a_type, string a_args) {
  uLog::annotate(MARSL_HOSTMSG,"create_agent("+a_type+","+a_args+")");
  tokenMachine am(a_args);
  am.trimParens();
  if (a_type == "mars_rover") {
    string a_name = textFunctions::dequote(am.nextToken());
    mars_rover* r = new mars_rover(a_name,this);
    sprintf(uLog::annotateBuffer,
	    "create: %s @ %lx",
	    a_name.c_str(),
	    (long unsigned int)r);
    uLog::annotateFromBuffer(MONL_WARNING);
    add_agent(r);
    return true;
  }
  else if (a_type == "dispatch") {
    string a_name = textFunctions::dequote(am.nextToken());
    basic_dispatch_agent* bda = new basic_dispatch_agent(a_name,this);
    add_agent(bda);
    return true;
  }
  else if (a_type == "relay_station") {
    string a_name = textFunctions::dequote(am.nextToken());
    relay_station* rel = new relay_station(a_name,this);
    int start = 0;
    if (am.moreTokens())
      start = textFunctions::numval(am.nextToken());
    attempt_init_agent_property(rel,MDAP_LOCATION,start);
    add_agent(rel);
    return true;
  }
  else {
    sprintf(domain_config::error_msg,
	    "unknown agent type '%s'",
	    a_type.c_str());
    return false;
  }
}

bool mars_domain::create_controller(string c_type,string a_name,
				    string c_args) {
  uLog::annotate(MARSL_HOSTMSG,
		 "create_controller("+c_type+","+a_name+"," +c_args+")");
  // have to be able to create a wrapped controller...
  list<string> c_chain;
  tokenMachine am(c_type);
  am.trimParens();
  am.printSource();
  am.printWSource();
  while (am.moreTokens()) {
    c_chain.push_front(am.nextToken());
  }
  agent* agnt = get_agent(a_name);
  if (agnt == NULL) {
    sprintf(domain_config::error_msg,
	    "could not find agent named '%s'",
	    a_name.c_str());
    return false;
  }
  else {
    controller* c_head=NULL;
    for (list<string>::iterator i = c_chain.begin();
	 i != c_chain.end();
	 i++) {
      uLog::annotate(MARSL_HOSTMSG,"[mars/dcfg]:: creating "+(*i)+" for "+a_name+"...");

      /* ROVER UGPA !! */
      if (*i == "rover_ugpa") {
	mars_rover* smagnt = dynamic_cast<mars_rover*>(agnt);
	if (smagnt == NULL) {
	  sprintf(domain_config::error_msg,
		  "agent '%s' is not mars_rover, required by controller '%s'",
		  a_name.c_str(),
		  (*i).c_str());
	  return false;
	}
	if (c_head) {
	  sprintf(domain_config::error_msg,
		  "controller '%s' cannot wrap class '%s'",
		  (*i).c_str(),
		  c_head->class_name().c_str());
	  return false;
	}
	else c_head = new rover_ugpa_controller(smagnt);
      }

      /* RUGPA REACTIVE (WRAPPER) !! */
      else if (*i == "rugpa_reactive_layer") {
	if (!c_head) {
	  sprintf(domain_config::error_msg,
		  "controller '%s' must wrap another [core] controller)",
		  (*i).c_str());
	  return false;
	}
	else c_head = new rugpa_reactive_layer(c_head);
      }

      /* RUGPA MAINTENANCE (WRAPPER) !! */
      else if (*i == "rover_maintenance_layer") {
	if (!c_head) {
	  sprintf(domain_config::error_msg,
		  "controller '%s' must wrap another [core] controller)",
		  (*i).c_str());
	  return false;
	}
	else c_head = new rover_maintenance_layer(c_head);
      }

      /* RUGPA MAINTENANCE (WRAPPER) !! */
      else if (*i == "rugpa_opportunistic_layer") {
	if (!c_head) {
	  sprintf(domain_config::error_msg,
		  "controller '%s' must wrap another [core] controller)",
		  (*i).c_str());
	  return false;
	}
	else c_head = new rugpa_opportunistic_layer(c_head);
      }

      /* TTY DISPATCH !! */
      else if (*i == "tty_dispatch") {
	basic_dispatch_agent* bdagnt = 
	  dynamic_cast<basic_dispatch_agent*>(agnt);
	if (bdagnt == NULL) {
	  sprintf(domain_config::error_msg,
		  "agent '%s' is not dispatch_agent, required by controller '%s'",
		  a_name.c_str(),
		  (*i).c_str());
	  return false;
	}
	if (c_head) {
	  sprintf(domain_config::error_msg,
		  "controller '%s' cannot wrap class '%s'",
		  (*i).c_str(),
		  c_head->class_name().c_str());
	  return false;
	}
	else {
	  tokenMachine am(c_args);
	  am.trimParens();
	  string c_name = textFunctions::dequote(am.nextToken());
	  c_head = new tty_dispatch_control(bdagnt,c_name);
	}
      }

      /* TCP DISPATCH !! */
      else if (*i == "tcp_dispatch") {
	basic_dispatch_agent* bdagnt = 
	  dynamic_cast<basic_dispatch_agent*>(agnt);
	if (bdagnt == NULL) {
	  sprintf(domain_config::error_msg,
		  "agent '%s' is not dispatch_agent, required by controller '%s'",
		  a_name.c_str(),
		  (*i).c_str());
	  return false;
	}
	if (c_head) {
	  sprintf(domain_config::error_msg,
		  "controller '%s' cannot wrap class '%s'",
		  (*i).c_str(),
		  c_head->class_name().c_str());
	  return false;
	}
	else {
	  tokenMachine am(c_args);
	  am.trimParens();
	  string c_name = textFunctions::dequote(am.nextToken());
	  bool   block  = true;
	  if (am.moreTokens()) {
	    if (textFunctions::numval(am.nextToken()) == 0)
	      block = false;
	  }
	  c_head = new socket_dispatch_control(bdagnt,c_name,block);
	}
      }

      /* UNKNOWN BS CONTROLLER !! */
      else {
	sprintf(domain_config::error_msg,
		"unknown controller type '%s'",
		(*i).c_str());
	return false;
      }
    }

    agnt->set_active_controller(c_head);
    return true;

  }
  return false;
}

bool mars_domain::can_set_agent_property(agent* a,apkType property_key, 
					     double val) {
  if (property_key == MDAP_LOCATION)
    return (is_reachable((int)get_agent_property(a,property_key),(int)val));
  else return true;
}

bool mars_domain::attempt_set_agent_property(agent* a,apkType property_key, 
					     double val) {
  if (can_set_agent_property(a,property_key,val)) {
    set_agent_property(a,property_key,val);
    return true;
  }
  else { 
    return false;
  }
}

bool mars_domain::attempt_init_agent_property(agent* a,apkType property_key, 
					      double val) {
  // the domain gets to decide whether or not the property actually gets 
  // set as requested....
  // (not really doing any deciding yet)
  set_agent_property(a,property_key,val);
  return true;
}

double mars_domain::get_agent_property(agent* a,apkType property_key) {
  // cout << "[mcd/mdom]:: get(" << a->get_name()
  // << "@0x" << hex << (int)a << "," << property_key << ")" << endl;
  if (ap_lookup.find(a) == ap_lookup.end())
    return DEFAULT_MDAP_VALUE;
  else {
    return (ap_lookup[a])->get(property_key);
  }
}

void mars_domain::set_agent_property(agent* a, apkType pkey, double val) {
  // cout << "[mcd/mdom]:: set(" << a->get_name()
  // << "@0x" << hex << (int)a << "," << pkey
  // << "=" << val << ")" << endl;
  if (ap_lookup.find(a) != ap_lookup.end()) {
    if (ap_lookup[a]==NULL)
      cout << "LOOKUP=NULL" << endl;
    else {
      cout << "LOOKUP=" << ap_lookup[a] << endl;
      (ap_lookup[a])->set(pkey,val);
    }
  }
  else {
    cout << "LOOKUP FAILED" << endl;
    agent_property_table *napt = new agent_property_table(a);
    napt->set(pkey,val);
    ap_lookup[a]=napt;
    cout << "HARD_SET@" << ap_lookup[a] << endl;    
    cout << "HARD_SET=" << ap_lookup[a]->get(pkey) << endl;
  }
}

bool mars_domain::request_image_ambient(agent* a,image_bank& b,int count) {
  cout << "request for image..." << endl;
  double loc = (int)get_agent_property(a,MDAP_LOCATION);
  for (int i = 0 ; i < count ; i++) {
    imagetype ta = create_img((int)loc,(int)loc,i);
    img_set_pano(ta);
    if (!b.add_image(ta))
      return false;
  }
  return true;
}

bool mars_domain::request_image_targets(agent* a,image_bank& b,int count, int cal) {
  uLog::annotate(MARSL_MSG,personalize(": request for science image..."));
  double loc = (int)get_agent_property(a,MDAP_LOCATION);
  for (int i = 0 ; i < count ; i++) {
    imagetype ta = create_img((int)loc,cal,i);
    if (!b.add_image(ta))
      return false;
  }
  return true;
}

bool mars_domain::broadcast_imaging_goal_rcvd(agent* a, goalspec gs, bool is_pano) {
  bool rv=false;
  // again, dangerously oversimplified. there is no effective
  // limitation on the number of relay stations but if there is more
  // than one then things could get ugly...
  for (agent_list::iterator ali = al_start();
       ali != al_end();
       ali++) {
    relay_station* ars = dynamic_cast<relay_station*>(*ali);
    if (ars && ars->ack()) {
      uLog::annotate(MARSL_MSG,personalize(": attempting to broadcast goal to " + ars->get_name()));
      ars->receive_goal(gs,is_pano);
      rv=true;
    }
  }
  return rv;
}

bool mars_domain::attempt_xmit_ibank(agent* a, image_bank& ib) {
  // this is oversimplified since we don't simulate at the level required
  // to do range / visibility computations
  for (agent_list::iterator ali = al_start();
       ali != al_end();
       ali++) {
    relay_station* ars = dynamic_cast<relay_station*>(*ali);
    if (ars && ars->ack()) {
      return ars->receive(ib);
    }
  }
  return false;
}

model* mars_domain::provide_ptr_to_model(simEntity* e) {
  // cout << "e=" << hex << e << " this=" << this << endl;
  if (e == this) 
    return &my_mars_domain_model;
  else if (has_action_model_for(e)) {
    return action_model_for(e);
  }
  else return NULL;
}

model* mars_domain::provide_copy_of_model(simEntity* e) {
  model* foo = provide_ptr_to_model(e);
  if (foo) return foo->clone();
  else return NULL;
}
