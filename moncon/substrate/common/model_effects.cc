#include "substrate/model_effects.h"
#include "substrate/agent.h"
#include "substrate/observables.h"

using namespace MonCon;

const char*   stay_effect::class_id = "stay_effect";
const char* change_effect::class_id = "change_effect";
const char* simple_effect::class_id = "simple_effect";
const char* argtarg_effect::class_id = "argtarg_effect";

//////////////////////////////////////////////////////////////
// gen expectation should never get called outside of tell_mcl
// so in theory it should be okay not to surround the mclMA
// calls with checks to the "mcl" setting

/*

bool simple_effect::gen_expectation(string key,egkType et,sensing* sa) {
  if (relative) {
    double cv = sa->get_observable(sensor)->double_value();
    mclMA::declareExpectation(key,et,sensor,EC_TAKE_VALUE,cv+amt);
  }
  else {
    mclMA::declareExpectation(key,et,sensor,EC_TAKE_VALUE,amt);
  }
  return true;
}

bool change_effect::gen_expectation(string key,egkType et,sensing* d) {
  if (unspec)
    mclMA::declareExpectation(key,et,sensor,EC_ANY_CHANGE);
  else if (up)
    mclMA::declareExpectation(key,et,sensor,EC_GO_UP);
  else
    mclMA::declareExpectation(key,et,sensor,EC_GO_DOWN);
  return true;
}

*/
