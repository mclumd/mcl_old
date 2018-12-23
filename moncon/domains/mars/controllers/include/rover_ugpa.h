#ifndef MARS_ROVER_UGPA_H
#define MARS_ROVER_UGPA_H

#include "substrate/ugpa_controller.h"
#include "mars_domain_models.h"
#include "mars_rover.h"

using namespace MonCon;

namespace mcMars {

class rover_goal;

 class rover_ugpa_controller : public ugpa_controller, 
  public shares_mars_domain_model {
  
 public:
  rover_ugpa_controller(mars_rover* rover);

  virtual ~rover_ugpa_controller() {};

  virtual bool replan(ugpa_goal* g);

  // formalities
  virtual string class_name() { return "rover_ugpa_controller"; };
  virtual string describe() { return "<rover_ugpa "+get_name()+">"; };

  virtual bool abort_current();
  virtual bool tell(string message);

  virtual bool   initialize();

  virtual string process_command(string command);

  virtual timeline_entry * parse_tle_spec( simTimeType firetime, 
					   timeline* tl, string tles );

  virtual string grammar_commname() { 
    return agent_of()->class_name()+"_"+class_name()+"_command"; 
  };
  virtual bool publish_grammar(umbc::command_grammar& grammar);
  void grab_all_models();

  mars_rover* rover_of() { return my_rover; };

 protected:

  virtual bool plan_for(rover_goal* goal);
  virtual bool plan_get_to(rover_goal* goal,int dest,bool improve);
  virtual bool improve_plan(rover_goal* goal, int loc);
  virtual int  closest_location_recharge();
  virtual int  closest_location_localize();

  virtual rover_goal* text2goal(string spec);

 private:
  mars_rover* my_rover;

};

//////////////////////////////////////////////////////////////////////
// ROVER GOAL, EXTENDS UGPA_GOAL

#define ROVER_UGPA_MAX_GOAL_ARGS         5
#define ROVER_UGPA_NO_LOC_SPEC          -1
#define ROVER_UGPA_USE_CURRENT_LOCATION -2
#define ROVER_UGPA_GOAL_NO_ARG          -999999

class rover_goal : public ugpa_goal {
 public:
  rover_goal(int ac_code) : ugpa_goal(),
    goal_action_code(ac_code),
    goal_action_location(ROVER_UGPA_NO_LOC_SPEC),
    goal_action_argcount(0)
    {};

  static rover_goal* make_goal(int ac_code) 
    { return new rover_goal(ac_code); };

  static rover_goal* make_goal_at(int ac_code,int loc) { 
    rover_goal* r = new rover_goal(ac_code); 
    r->set_action_loc(loc);
    return r;
  };

  void set_arg(int argno, int argval) { 
    if (argno < ROVER_UGPA_MAX_GOAL_ARGS)
      goal_action_args[argno]=argval;
    if (argno >= goal_action_argcount)
      goal_action_argcount = argno+1;
  }

  int get_arg(int argno) {
    if (argno >= goal_action_argcount)
      return ROVER_UGPA_GOAL_NO_ARG;
    else return goal_action_args[argno];
  }

  int action_code() { return goal_action_code; };
  int action_loc() { return goal_action_location; };
  
  void set_action_loc(int loc) { goal_action_location = loc; };

  virtual double priority() { return 1.0; };

  virtual string class_name() { return "rover_goal"; };
  virtual string describe() { 
    char v[256];
    sprintf(v, "<rover_goal %d(%d)@%d>",action_code(),get_arg(0),
	    action_loc());
    return v;
  }

 private:
  int goal_action_code, goal_action_location;
  int goal_action_args[ROVER_UGPA_MAX_GOAL_ARGS];
  int goal_action_argcount;

};

 class rover_ugpa_timeline_goal : public ugpa_timeline_goal {
 public:
  rover_ugpa_timeline_goal(int firetime,timeline* tl,ugpa_goal* ug,
			   ugpa_controller* uc)
    : ugpa_timeline_goal(firetime,tl,ug,uc) {};
  virtual void fire();
};
 
};

#endif
