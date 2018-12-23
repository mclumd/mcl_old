#ifndef MCL_MODEL_H
#define MCL_MODEL_H

#include "models.h"
#include "simEntity.h"
#include "mcl_aware.h"
#include "mcl/mcl_api.h"
#include <list>
#include <map>
#include <string.h>

namespace MonCon {
  class action;
  class sensorimotor_agent;
  class action_effect;

/////////////////////////////////////////////////////////////////
/** The model class is generic in the sense that it doesn't
    really have to link the host to MCL (in fact not all models 
    do). The MCL/action model is mcl_action_model, others are
    used for the planner. **/
/////////////////////////////////////////////////////////////////

 class mcl_action_model : public action_model, public mclAware::has_eg_key {
 public:
  mcl_action_model(action* a);
  virtual ~mcl_action_model() {};

  virtual bool tell_mcl(string key) { return tell_mcl(key,NULL,NULL); };
  virtual bool tell_mcl(string key,egkType parent_key,resRefType response_ref);
  // bool add_effect(action_effect* ae) { effects.push_front(ae); return true; };
  
  virtual string class_name() { return "mcl_action_model"; };
  virtual string describe();

  virtual egkType mcl_eg_key_for()
  { return (egkType)models_this_action(); };

 protected:
  sensorimotor_agent* model_agent;
  list<action_effect*> effects;

};

/////////////////////////////////////////////////////////////////
/** This class essentially implements a lookup table for 
    actions and mcl_action_models **/
/////////////////////////////////////////////////////////////////

/*

 class has_action_models {
 public:
   has_action_models() {};
   mcl_action_model* get_model(action* a) { return model_lookup[a]; };
   void set_model(action* a,mcl_action_model* m) { model_lookup[a]=m; };
   
 protected:
   map<action*,mcl_action_model*> model_lookup;
 };

*/

};

#endif
