#ifndef MARS_DOMAIN_MODELING_H
#define MARS_DOMAIN_MODELING_H

#include "substrate/i_models.h"
#include <limits>

namespace mcMars {

  using namespace MonCon;
  class mars_domain;

  class mars_domain_model_suite : public model {
  public:
    mars_domain_model_suite(mars_domain* d);

    virtual ~mars_domain_model_suite();

    // constants?
    static const int NOWHERE = -1;
    static       int unreachable_cost();

    // model interface...
    bool model_equality(model* m);
    
    // functions for setting the internal models...
    void set_mcmx(cost_mx_model* ncmx)
    { if (movecosts) delete movecosts; movecosts=ncmx; };
    void set_cal_mx(adj_mx_model* nbmx)
    { if (cal_mx) delete cal_mx; cal_mx=nbmx; };
    void set_move_mx(adj_mx_model* nbmx)
    { if (move_mx) delete move_mx; move_mx=nbmx; };
    void set_action_cv(cost_vector_model* ncv)
    { if (actioncosts) delete actioncosts; actioncosts=ncv; };
    void set_charge_pv(pc_vector_model* npcv)
    { if (charge_pc) delete charge_pc; charge_pc=npcv; };
    void set_local_pv(pc_vector_model* npcv)
    { if (local_pc) delete local_pc; local_pc=npcv; };
    bool verify_model();

    // functions replacing the mars_domain internal API
    void set_num_waypoints(int x);
    void set_num_scipoints(int x);
    void set_reachable(int s, int d, int cost);
    void set_calable(int s, int d, bool v=true);
    bool is_reachable(int s, int d);
    bool is_calable(int way, int sci);
    int  move_cost(int s, int d);
    void set_rechargable(int s, bool v);
    bool is_rechargable(int s);
    void set_localizable(int s, bool v);
    bool is_localizable(int s);

    // these are for the planner...
    int  closest_calpoint(int cloc) { 
      if (cal_mx) return cal_mx->closest_major(cloc); 
      else return NOWHERE;
    };
    int* cost_mx_ptr() { 
      if (movecosts) return movecosts->matrix_ptr(); 
      else return NULL;
    };
    int  num_scipoints() { 
      if (cal_mx) return cal_mx->minor_length(); 
      else return 0;
    };
    int  num_waypoints() { 
      if (movecosts) return movecosts->major_length(); 
      else return 0;
    };
    bool can_charge(int loc) { 
      if (charge_pc) return charge_pc->sat(loc); 
      else return false;
    };
    bool can_loc(int loc) { 
      if (local_pc) return local_pc->sat(loc); 
      else return false;
    };

    virtual string class_name() { return "mars_domain_model_suite"; };
    virtual string describe() { return "("+class_name()+" "+get_name()+")"; };

    virtual model* clone();
    virtual simEntity* model_of();

    virtual void copy_to(mars_domain_model_suite& target);

  protected:
    cost_mx_model*     movecosts;
    adj_mx_model*      move_mx;
    adj_mx_model*      cal_mx;
    cost_vector_model* actioncosts;
    pc_vector_model*   charge_pc;
    pc_vector_model*   local_pc;
    int n_waypoints;
    int n_scipoints;
    mars_domain*       of_this;
    
  };

  // this stuff should all be handled using the models stuff
  // it is -- partially, but not completely and I am not sure
  // why as I document this.

  // this should subclass 'has_model'
  class has_mars_domain_model {
  public:
    // okay, so the mars_domain_model is a member variable and is 
    // constructed using the domain d, that's why it looks like this
  has_mars_domain_model(mars_domain* d) : my_mars_domain_model(d) {};

  protected:
    mars_domain_model_suite my_mars_domain_model;

  };

  // this should be a 'model_conduit' or at least 'provides_models'
  class shares_mars_domain_model : public has_mars_domain_model {
  public:
  shares_mars_domain_model(mars_domain* d) : has_mars_domain_model(d) {};

    mars_domain_model_suite* ptr_to_mdms() { return &my_mars_domain_model; };

  };


};

#endif
