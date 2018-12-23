#include "rover_mcl_wrappers.h"
#include "mars_logging.h"
#include "umbc/exceptions.h"
#include "substrate/mcl_aware.h"

using namespace umbc;
using namespace mcMars;

mmcl::rover_mcl_action::rover_mcl_action(mclAware::mcl_agent* mcla,action* mra) : 
  mcl_action(mcla,mra) {

  // make sure that the core is a rover action
  action* input_core = action_wrappers::action_core(mra);
  mars_rover_action* icamra = dynamic_cast<mars_rover_action*>(input_core);
  if (icamra)
    my_rover_action = icamra;
  else throw action_wrappers::illegal_core(class_name(),input_core->class_name());

};

action* mmcl::rover_mcl_action::attempt_wrap(mclAware::mcl_agent* mr,action* A) {
  if (!action_wrappers::action_wrapper::get_typed_wrapper<mclAware::mcl_action*>(A)) {
    try {
      action* wa = new rover_mcl_action(mr,A);
      return wa;
    }
    catch(action_wrappers::illegal_core& e) {
      uLog::annotate(MARSL_HOSTERR, e.what());
      return A;
    }
    /*
	action* wa = new mclAware::mcl_action(this,k);
	sub->replace_action(k,wa);
	uLog::annotate(MARSL_WARNING,sub->get_name()+
		       ": had to generically wrap "+
		       k->get_name()+
		       " because it did not cast to a rover action.");
    */
  }
  else {
    uLog::annotate(MONL_HOSTERR,A->get_name()+": attempt made to double-wrap?");
    return A;
  }
}

bool mmcl::rover_mcl_action::declare_expectations() {
  model* mm = rover_action_of()->ptr_to_my_model();
  if (!mm) {
    uLog::annotate(MONL_HOSTERR,"no model for "+describe());
  }
  action_model* mam = dynamic_cast<action_model*>(mm);
  if (mam) {
    uLog::annotate(MONL_HOSTMSG,
		   personalize("action expectations being posted."));
    for (unsigned int ai = 0; ai < mam->number_of_effects(); ai++) {
      action_effect* cae = mam->get_effect(ai);
      uLog::annotate(MONL_HOSTMSG,personalize("effect: "+cae->class_name()));
      mclAware::expgen::ae2expectations(mcl_key_of(),eg_key_of(),
					rover_action_of()->rover_of(),
					rover_action_of(),cae);
    }
  }
  else {
    uLog::annotate(MONL_HOSTERR,"no action model for "+describe());
  }
  return true;
}

