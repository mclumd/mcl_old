#include "substrate/mcl_aware.h"
#include "substrate/model_effects.h"
#include "substrate/moncon_logging.h"
#include "substrate/agent.h"
#include "substrate/observables.h"

using namespace MonCon;
using namespace umbc;

bool post4ste(string mcl_k, egkType eg_k, stay_effect* ae);
bool post4ce(string mcl_k, egkType eg_k, change_effect* ae);
bool post4se(string mcl_k, egkType eg_k,sensing* sa,simple_effect* ae);
bool post4ate(string mcl_k, egkType eg_k,sensing* sa,has_args* ha,
	      argtarg_effect* ae);

bool mclAware::expgen::ae2expectations(string  mcl_k,
				       egkType eg_k,
				       sensing* sa,
				       action_effect* ae) {
  if (ae->class_name() == change_effect::class_id) {
    return post4ce(mcl_k,eg_k,static_cast<change_effect*>(ae));
  }
  if (ae->class_name() == simple_effect::class_id) {
    return post4se(mcl_k,eg_k,sa,static_cast<simple_effect*>(ae));
  }
  else {
    uLog::annotate(MONL_HOSTERR,"unchecked action effect type '"
		   +ae->class_name()+
		   "' in expgen::ae2expectations(4/no_args)");
    return false;
  }
}

bool mclAware::expgen::ae2expectations(string  mcl_k,
				       egkType eg_k,
				       sensing* sa,
				       has_args* ha,
				       action_effect* ae) {
  if (ae->class_name() == change_effect::class_id) {
    return post4ce(mcl_k,eg_k,static_cast<change_effect*>(ae));
  }
  else if (ae->class_name() == simple_effect::class_id) {
    return post4se(mcl_k,eg_k,sa,static_cast<simple_effect*>(ae));
  }
  else if (ae->class_name() == argtarg_effect::class_id) {
    return post4ate(mcl_k,eg_k,sa,ha,static_cast<argtarg_effect*>(ae));
  }
  else {
    uLog::annotate(MONL_HOSTERR,"unchecked action effect type '"
		   +ae->class_name()+
		   "' in expgen::ae2expectations(5/has_args)");
    return false;
  }
}

// argtarg effect
bool post4ate(string mcl_k, egkType eg_k, 
	      sensing* sa, has_args* ha, argtarg_effect* ae) {
  if (ae->is_relative()) {
    mclMA::declareExpectation(mcl_k,eg_k,
			      ae->sensor_of(),EC_TAKE_VALUE,
			      sa->get_observable(ae->sensor_of())->double_value() + ha->get_arg_int(ae->arg_of()));
  }
  else {
    mclMA::declareExpectation(mcl_k,eg_k,
			      ae->sensor_of(),EC_TAKE_VALUE,
			      ha->get_arg_int(ae->arg_of()));
  }
  return true;
}

bool post4ce(string mcl_k, egkType eg_k, change_effect* ae) {
  if (ae->change_unspec()) 
    mclMA::declareExpectation(mcl_k,eg_k,ae->sensor_of(),EC_ANY_CHANGE);
  else {
    if (ae->change_up())
      mclMA::declareExpectation(mcl_k,eg_k,ae->sensor_of(),EC_GO_UP);
    else 
      mclMA::declareExpectation(mcl_k,eg_k,ae->sensor_of(),EC_GO_DOWN);
  }
  return true;
}

bool post4ste(string mcl_k, egkType eg_k, stay_effect* ae) {
  mclMA::declareExpectation(mcl_k,eg_k,ae->sensor_of(),EC_NET_ZERO);
  return true;
}

bool post4se(string mcl_k, egkType eg_k,sensing* sa,simple_effect* ae) {
  double tvo = (ae->is_relative()) ?
    sa->get_observable(ae->sensor_of())->double_value()+ae->amount_of() :
    ae->amount_of();
  mclMA::declareExpectation(mcl_k,eg_k,ae->sensor_of(),EC_TAKE_VALUE,tvo);  
  return true;
}

