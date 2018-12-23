#ifndef MARS_DOM_H
#define MARS_DOM_H

#include "substrate/basic_domain.h"
#include "substrate/basic_agent.h"
#include "substrate/goalspec.h"
#include "substrate/models.h"
#include "mars_domain_models.h"

#include "imagebank.h"

#define DEFAULT_MDAP_VALUE -65355.65355

using namespace MonCon;

namespace mcMars {

  class has_mars_domain_model;

  enum mars_agentprops_t {
    MDAP_LOCATION
  }; 

  class mars_domain : public basic_domain, public model_oracle, 
    public has_mars_domain_model  {
      
      friend class mars_recharge;
      friend class mars_moveto;
      friend class mars_calibrate;
      friend class mars_localize;
      friend class mars_anomaly;

  public:
      mars_domain();
      
      // domain overrides...
      virtual bool simulate();
      virtual bool configure(string conf);
      
      // simEntity overrides...
      virtual string describe();
      virtual string class_name() { return "mars_domain"; };
      
      // provides_models overrides...
      virtual model* provide_ptr_to_model(simEntity *e);
      virtual model* provide_copy_of_model(simEntity *e);
      
      bool create_agent(string a_type, string a_args);
      bool create_controller(string c_type, string a_name, string c_args);
      
      void force_event(int event_code,long event_seed);
      
      // void copy_models2controller(has_mars_models* hmm);
      
      virtual timeline_entry* parse_tle_spec( simTimeType firetime, 
					      timeline* tl,
					      string tles );
      
      // agent requests .... 
      
      double request_agent_property(agent* a,apkType property_key) 
      { return get_agent_property(a,property_key); };
      
      bool   can_set_agent_property(agent* a,apkType property_key, 
				    double val);
      
      bool   attempt_set_agent_property(agent* a,apkType property_key, 
					double val);
      
      bool   attempt_init_agent_property(agent* a,
					 apkType property_key, 
					 double val);
      
      bool   request_image_ambient(agent* a,image_bank& b,int count);
      bool   request_image_targets(agent* a,image_bank& b,int count,int cal);
      
      bool   broadcast_imaging_goal_rcvd(agent* a, goalspec gs, bool is_pano);
      bool   attempt_xmit_ibank(agent* a,image_bank& b);
      
      // static "constants"
      
      static int MAX_NRG;
      static int MAX_BANK;
      static int PANO_IMG;
      static int SCI_IMG;
      
      void dump_mxs();
      
  protected:
      
      map<agent*,agent_property_table*> ap_lookup;
      map<simEntity*,action_model*> action_model_cache;
      map<string,action_model*> action_model_base;
      
      void make_default_map();
      // i guess this should be a function of agent+environment
      // void set_action_cost_vec();
      //   int rindex(int s,int d);
      //   int scindex(int map,int sci);
      
      int  get_num_waypoints() { return my_mars_domain_model.num_waypoints(); };
      bool is_reachable(int s,int d);
      bool is_sciable(agent* a,int sci);
      bool is_sciable(int maploc,int sci);
      bool is_rechargable(agent* m);
      bool is_rechargable(int g);
      bool is_localizable(agent* m);
      bool is_localizable(int s);
      bool can_calibrate(agent* a,int targ);
      bool can_calibrate(int src,int targ);
      int  move_cost(int s, int d);

      int  get_num_scipoints() { return my_mars_domain_model.num_scipoints(); };
      void set_num_waypoints(int x);
      void set_num_scipoints(int x);
      void set_reachable(int s,int d,int cost);
      void set_sciable(int maploc,int sci,bool v);
      void set_rechargable(int maploc,bool v);
      void set_localizable(int maploc,bool v);
      int* move_cost_mx() { return my_mars_domain_model.cost_mx_ptr(); };
      
      // from basic_domain
      virtual double get_agent_property(agent* a,apkType property_key);
      virtual void   set_agent_property(agent* a,apkType property_key, double val);

      // helper monkeys
      bool          has_action_model_for(simEntity* e);
      action_model* action_model_for(simEntity* e);
      void          init_action_oracle();

      static int unreachable_cost() 
      { return mars_domain_model_suite::unreachable_cost(); }
      
    };
  
};

#endif
