#include "umbc/dijkstra.h"
#include "mcl/mcl_api.h"

#include "rover_ugpa.h"
#include "mars_rover_actions.h"
#include "mars_rover.h"

using namespace mcMars;

#include "mars_logging.h"
using namespace umbc;

bool rover_ugpa_controller::replan(ugpa_goal* g) {
  clear_action_q();
  rover_goal* ingoal = dynamic_cast<rover_goal*>(g);
  if (!ingoal) { return false; };
  ingoal->clear_plan();
  bool rv= plan_for(ingoal);
  if (rv)
    ingoal->planning_is_complete();
  return rv;
}

bool rover_ugpa_controller::plan_for(rover_goal* rg) {

  int gact = rg->action_code();
  int cloc = sm_agent_of()->get_observable("location")->int_value();
  int gloc = rg->action_loc();
  if (gloc == ROVER_UGPA_USE_CURRENT_LOCATION) gloc = cloc;
  
  sprintf(uLog::annotateBuffer,
	  "[MARS/rugpa]::plan_for: act=[%d] arg=%d",
	  gact,gloc);
  uLog::annotateFromBuffer(MARSL_HOSTVRB);

  switch (gact) {
  case MR_DO_SCIENCE: {
    int garg = rg->get_arg(0);  // probably should be a #define instead of 0
    if (gloc == ROVER_UGPA_NO_LOC_SPEC) 
      gloc = my_mars_domain_model.closest_calpoint(garg);
    if (plan_get_to(rg,gloc,true)) {
      add_aspec_back(new actionspec(sm_agent_of()->get_action("cal"),garg));
      add_aspec_back(new actionspec(sm_agent_of()->get_action("science"),garg));
      return true;
    }
    else return false; }
    break;

  case MR_TAKE_PANORAMIC: {
    if (gloc == ROVER_UGPA_NO_LOC_SPEC) gloc = cloc;
    if (!plan_get_to(rg,gloc,false)) return false;
    add_aspec_back(new actionspec(sm_agent_of()->get_action("pano")));
    break;
  }

  case MR_RECHARGE:
    if (gloc == ROVER_UGPA_NO_LOC_SPEC) gloc = closest_location_recharge();
    if (!plan_get_to(rg,gloc,false)) return false;
    add_aspec_back(new actionspec(sm_agent_of()->get_action("charge")));
    break;

  case MR_LOCALIZE:
    if (gloc == ROVER_UGPA_NO_LOC_SPEC) gloc = closest_location_localize();
    if (!plan_get_to(rg,gloc,false)) return false;
    add_aspec_back(new actionspec(sm_agent_of()->get_action("loc")));
    break;

  case MR_XMIT:
    if (gloc == ROVER_UGPA_NO_LOC_SPEC) gloc = cloc;
    if (!plan_get_to(rg,gloc,false)) return false;
    add_aspec_back(new actionspec(sm_agent_of()->get_action("xmit")));
    break;

  case MR_MOVE_TO:
    int mloc = rg->get_arg(0);  // probably should be a #define instead of 0
    if (mloc == ROVER_UGPA_GOAL_NO_ARG) return false;
    plan_get_to(rg,mloc,false);
    break;

  }

  return true;
}

bool rover_ugpa_controller::plan_get_to(rover_goal* rg,int gloc,bool improve) {
  int cloc = sm_agent_of()->get_observable("location")->int_value();
  if (gloc == -1)
    return false;
  sprintf(uLog::annotateBuffer,"[MARS/planbot]::Planner ~ planning to get from %d to %d",cloc,gloc);
  uLog::annotateFromBuffer(MARSL_HOSTVRB);
  if (cloc == gloc) return true;
  // put actions on the queue
  long cost = -1;
  list<int> path = 
    umbc::dijkstra::shortestPath(my_mars_domain_model.cost_mx_ptr(),
				 my_mars_domain_model.num_waypoints(),
				 cloc,gloc,&cost);
  for (list<int>::iterator pi = path.begin();
       pi != path.end();
       pi++) {
    actionspec* tas = new actionspec(sm_agent_of()->get_action("moveto"),*pi);
    add_aspec_back(tas);
    if (improve) improve_plan(rg,*pi);
  }
  return true;
}

bool rover_ugpa_controller::improve_plan(rover_goal* rg,int loc) {
  if (my_mars_domain_model.can_charge(loc)) {
    add_aspec_back(new actionspec(sm_agent_of()->get_action("charge")));    
  }
  if (my_mars_domain_model.can_loc(loc)) {
    add_aspec_back(new actionspec(sm_agent_of()->get_action("loc")));    
  }
  return true;
}

int rover_ugpa_controller::closest_location_recharge() {
  int min_d=9999999,min_l=-1;
  int cloc = sm_agent_of()->get_observable("location")->int_value();
  for (int tp = 0; tp < my_mars_domain_model.num_waypoints(); tp++) {
    if (my_mars_domain_model.can_charge(tp)) {
      long cost;
      list<int> path = 
	umbc::dijkstra::shortestPath(my_mars_domain_model.cost_mx_ptr(),
				     my_mars_domain_model.num_waypoints(),
				     cloc,tp,&cost);
      if (cost<min_d) {
	// cout << "current closest recharge point @" << tp << endl;
	min_d = cost;
	min_l = tp;
      }
    }
  }
  return min_l;
}

int rover_ugpa_controller::closest_location_localize() {
  int min_d=9999999,min_l=-1;
  int cloc = sm_agent_of()->get_observable("location")->int_value();
  for (int tp = 0; tp < my_mars_domain_model.num_waypoints(); tp++) {
    if (my_mars_domain_model.can_loc(cloc)) {
      long cost;
      list<int> path = 
	umbc::dijkstra::shortestPath(my_mars_domain_model.cost_mx_ptr(),
				     my_mars_domain_model.num_waypoints(),
				     cloc,tp,&cost);
      if (cost<min_d) {
	min_d = cost;
	min_l = tp;
      }
    }
  }
  return min_l;
}
