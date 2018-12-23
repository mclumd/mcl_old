#ifndef MC_SUBSTRATE_GOALS_H
#define MC_SUBSTRATE_GOALS_H

#include "substrate/controller.h"
#include "substrate/control_common.h"
#include "substrate/actionspec.h"
#include "substrate/simEntity.h"
#include "mcl/mcl_api.h"

#include <list>

using namespace std;

namespace MonCon {

class action;
class ugpa_goal;
class ugpa_controller;

enum ugpa_plan_state {
  UGPA_PLAN_PREINIT,
  UGPA_PLAN_INITIALIZED,
  UGPA_PLAN_RUNNING,
  UGPA_PLAN_PREEMPTED,
  UGPA_PLAN_COMPLETED,
  UGPA_PLAN_DECOMMISSIONED,
  UGPA_PLAN_ERROR };

 class ugpa_plan : public has_action_q,public simEntity {
  // a ugpa_plan is a series of actionspecs that achieve a goal
 public:
  ugpa_plan(ugpa_goal* ag) : has_action_q(),simEntity(),
    its_goal_ref(ag),planstate(UGPA_PLAN_PREINIT) {};
  virtual ~ugpa_plan() {};

  virtual bool execute_step(ugpa_controller* the_cont);

  void clear() { 
    clear_action_q(); 
    planstate=UGPA_PLAN_DECOMMISSIONED;
  };

  // ugpa_goal* goal_ref_for() { return its_goal_ref; };

  virtual void add_step_back(actionspec* aspec)
    { add_aspec_back(aspec); };

  virtual void planning_is_complete() 
    { planstate=UGPA_PLAN_INITIALIZED; };

  virtual void preempt_plan() { 
    planstate=UGPA_PLAN_PREEMPTED; 
  };

  virtual string describe();
  virtual string class_name() { return "ugpa_plan"; };

  virtual bool is_done_executing() 
    { return (planstate==UGPA_PLAN_COMPLETED); };

 protected:
  ugpa_goal*      its_goal_ref;
  ugpa_plan_state planstate;

};

class ugpa_goal : public simEntity {
 public:
  ugpa_goal() : the_plan(NULL) {};
  virtual ~ugpa_goal() { if (the_plan) delete the_plan; };
  virtual double priority()=0;

  virtual actionspec* current_step() { return the_plan->curr_aspec(); };
  virtual bool execute_step(ugpa_controller* the_cont) 
    { return the_plan->execute_step(the_cont); };
  
  virtual bool selected(ugpa_controller* the_cont);
  virtual bool unselected(ugpa_controller* the_cont);

  virtual void clear_plan();
  virtual void preempt_goal() { if (the_plan) the_plan->preempt_plan(); };

  virtual void add_step_to_gp_back(actionspec* aspec)
    { the_plan->add_step_back(aspec); };

  virtual void planning_is_complete() 
    { the_plan->planning_is_complete(); };

  virtual ugpa_plan* make_initialized_plan() { return new ugpa_plan(this); };
  
  virtual bool is_done_executing() { return the_plan->is_done_executing(); };
  virtual string class_name() { return "ugpa_goal"; };

 protected:
  ugpa_plan* the_plan;

};

enum ugpa_controller_states { UGPA_STATE_OK, UGPA_STATE_PREEMPTED };

 class ugpa_controller 
   : public sensorimotor_controller, public has_action_q {
 public:

  ugpa_controller(sensorimotor_agent* a,string name) : 
    sensorimotor_controller(a,name),has_action_q(),
      currently_executing_goal(NULL),ugpa_state(UGPA_STATE_OK),ag_maxsize(2) {};

    // inherited from controller....
    virtual bool control_in();
    virtual bool control_out();
    virtual bool initialize();
    virtual bool busy() { return !is_idle(); };
    virtual bool note_preempted() { 
      ugpa_state=UGPA_STATE_PREEMPTED; 
      return true;
    };
    
    virtual void add_goal(ugpa_goal* g) { pending_goals.push_back(g); };
    
    // virtual void interrupt() {};
    // virtual void resume() {};
    virtual void preempt(actionspec* aspec);
    virtual bool replan(ugpa_goal* g)=0;
    virtual bool abort_current();
    virtual bool tell(string message);
    
    // required public by ugpa_plan...
    virtual bool process_action(action* ua);
    
 protected:
    
    virtual bool is_idle();
    virtual bool has_pending_goal();
    virtual bool is_pursuing_goal();
    
    /// ###mcl-EXTRACTION
    // virtual bool process_action_mcl(action* ua, has_action_models *h);
    
    list<ugpa_goal*> pending_goals;
    list<ugpa_goal*> processed_goals;
    list<ugpa_goal*> aborted_goals;
    ugpa_goal*       currently_executing_goal;
    int              ugpa_state;

    unsigned int     ag_maxsize;
    
};

class ugpa_timeline_goal : public timeline_entry {
 public:
  ugpa_timeline_goal(int firetime,timeline* tl,ugpa_goal* ug,
		     ugpa_controller* uc)
    : timeline_entry(firetime,tl),the_goal(ug),owner_controller(uc) {};
  virtual ~ugpa_timeline_goal() {};
  virtual void fire();
  virtual string describe();
  virtual string class_name() { return "ugpa_timeline_goal"; };
  virtual void dump();
  virtual void set_repeating(bool r) { 
    if (r) cerr << "can't set ugpa goal timeline entries to repeat!" << endl;
  };

 protected:
  ugpa_goal*       the_goal;
  ugpa_controller* owner_controller;

};

};
#endif
