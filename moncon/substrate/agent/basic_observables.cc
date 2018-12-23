#include "substrate/basic_observables.h"
#include "substrate/moncon_logging.h"
#include "substrate/controller_wrapper.h"
#include "mcl/mcl_api.h"

using namespace MonCon;
using namespace umbc;

// basic resource stuff

declaration_based_observable::declaration_based_observable
(string name, agent* a, const char* declaration) :
  basic_observable(name,a), the_dec(declaration) {
}

bool   basic_settable_observable_dbl::attempt_set(double nv) {
  BSO_value = nv; 
  return true; 
}

bool   basic_settable_observable_dbl::attempt_set(int nv) { 
  BSO_value = (double)nv; 
  return true; 
}

bool   basic_settable_observable_dbl::attempt_inc(double nv) {
  BSO_value += nv; 
  return true; 
}

bool   basic_settable_observable_dbl::attempt_dec(double nv) {
  BSO_value -= nv; 
  return true; 
}

bool   declaration_based_observable::attempt_set(double v)  {
  // declarations::declare_n(the_dec,(int)v); 
  uLog::annotate(MONL_HOSTERR,"[mcs/bo]:: declaration-based observables should NOT be set (override if necessary)");
  return false; 
}

bool   declaration_based_observable::attempt_dec(double v)  {
  // declarations::declare_n(the_dec,(int)v); 
  uLog::annotate(MONL_HOSTERR,"[mcs/bo]:: declaration-based observables should NOT be set (override if necessary)");
  return false; 
}

bool   declaration_based_observable::attempt_inc(double v)  {
  // declarations::declare_n(the_dec,(int)v); 
  uLog::annotate(MONL_HOSTERR,"[mcs/bo]:: declaration-based observables should NOT be set (override if necessary)");
  return false; 
}

controlling_layer_observable::controlling_layer_observable(string name,agent* a)
  : basic_observable(name,a) {};

int controlling_layer_observable::int_value() {
  controller* c = agent_of()->get_active_controller();
  return controller_wrapper::control_code(c);
}
