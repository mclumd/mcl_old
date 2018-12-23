#include "mars_domain_models.h"
#include "mars_domain.h"
#include "mars_logging.h"

#include <limits>

using namespace MonCon;
using namespace mcMars;

int mars_domain_model_suite::unreachable_cost() {
  return std::numeric_limits< int >::max();
}

mars_domain_model_suite::mars_domain_model_suite(mars_domain* d) : 
  model(d->get_name()+"_model"),
  movecosts(NULL),move_mx(NULL),cal_mx(NULL),
  actioncosts(NULL),charge_pc(NULL),local_pc(NULL),
  n_waypoints(-1),n_scipoints(-1),of_this(d) {};

mars_domain_model_suite::~mars_domain_model_suite() {
  if (movecosts) delete movecosts;
  if (move_mx) delete move_mx;
  if (cal_mx) delete cal_mx;
  if (actioncosts) delete actioncosts;
  if (charge_pc) delete charge_pc;
  if (local_pc) delete local_pc;
}

bool mars_domain_model_suite::model_equality(model* m) {
  umbc::uLog::annotate(MARSL_HOSTERR,"model equality incomplete for MDMS.");
  return (m == this);
}

simEntity* mars_domain_model_suite::model_of() { return of_this; }

void mars_domain_model_suite::set_num_scipoints(int x) {
  cout << "setting scips: x=" << x << " sci(before)=" << num_scipoints() << endl;
  if (x == num_scipoints()) {
    n_scipoints=x;
    return;
  }
  else {
    n_scipoints=x; 
    if (cal_mx) delete cal_mx;
    bool bmx[x*n_waypoints];
    bzero(bmx,sizeof(bool)*x*n_waypoints);
    cal_mx = new adj_mx_model(bmx,n_waypoints,x);
  }
}

void mars_domain_model_suite::set_num_waypoints(int x) {
  if (x == num_waypoints()) {
    n_waypoints=x;
    return;
  }
  else {
    n_waypoints=x;
    {
      if (move_mx) delete move_mx;
      bool bmx[x*x];
      bzero(bmx,sizeof(bool)*x*x);
      move_mx = new adj_mx_model(bmx,x,x);
    }
    // cost mx
    {
      if (movecosts) delete movecosts;
      int cmx[x*x];
      for (int c=0;c<x*x;c++) cmx[c]= std::numeric_limits< int >::max();
      movecosts = new cost_mx_model(cmx,x,x);
    }
    // cal mx 
    {
      if (n_scipoints > 0) {
	if (cal_mx) delete cal_mx;
	bool calmx[x*n_scipoints];
	bzero(calmx,sizeof(bool)*x*n_scipoints);
	cal_mx = new adj_mx_model(calmx,x,n_scipoints);
      }
    }
    // loc vec
    if (local_pc != NULL) delete local_pc;
    local_pc = new pc_vector_model(x);
    // recharge
    if (charge_pc != NULL) delete charge_pc;
    charge_pc = new pc_vector_model(x);
    // actioncosts
    if (actioncosts != NULL) delete actioncosts;
    actioncosts = new cost_vector_model(x);
  }
}

void mars_domain_model_suite::set_calable(int waypt, int scipt,bool v) {
  cal_mx->set_reach(waypt,scipt,v);
}

bool mars_domain_model_suite::is_calable(int waypt, int scipt) {
  return cal_mx->reach(waypt,scipt);
}

void mars_domain_model_suite::set_reachable(int s, int d, int cost) {
  move_mx->set_reach(d,s,(cost == unreachable_cost()) ? false : true);
  move_mx->set_reach(s,d,(cost == unreachable_cost()) ? false : true);
  movecosts->set_cost(d,s,cost);
  movecosts->set_cost(s,d,cost);
}

bool mars_domain_model_suite::is_reachable(int s, int d) {
  return move_mx->reach(s,d);
}

int mars_domain_model_suite::move_cost(int s, int d) {
  return movecosts->cost(s,d);
}

void mars_domain_model_suite::set_rechargable(int s, bool v) {
  charge_pc->set(s,v);
}
bool mars_domain_model_suite::is_rechargable(int s) {
  return charge_pc->sat(s);
}
void mars_domain_model_suite::set_localizable(int s, bool v) {
  local_pc->set(s,v);
}
bool mars_domain_model_suite::is_localizable(int s) {
  return local_pc->sat(s);
}

bool mars_domain_model_suite::verify_model() {
  return true;
}

model* mars_domain_model_suite::clone() {
  mars_domain_model_suite* target = new mars_domain_model_suite(of_this);
  target->set_mcmx((cost_mx_model*)movecosts->clone());
  target->set_cal_mx((adj_mx_model*)cal_mx->clone());
  target->set_move_mx((adj_mx_model*)move_mx->clone());
  target->set_action_cv((cost_vector_model*)actioncosts->clone());
  target->set_local_pv((pc_vector_model*)local_pc->clone());
  target->set_charge_pv((pc_vector_model*)charge_pc->clone());
  target->verify_model();
  return target;
}

void mars_domain_model_suite::copy_to(mars_domain_model_suite& target) {
  target.set_mcmx((cost_mx_model*)movecosts->clone());
  target.set_cal_mx((adj_mx_model*)cal_mx->clone());
  target.set_move_mx((adj_mx_model*)move_mx->clone());
  target.set_action_cv((cost_vector_model*)actioncosts->clone());
  target.set_local_pv((pc_vector_model*)local_pc->clone());
  target.set_charge_pv((pc_vector_model*)charge_pc->clone());
  target.verify_model();
}

/*
bool has_mars_domain_models::copy_models_to(has_mars_domain_models* hmm) {
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
