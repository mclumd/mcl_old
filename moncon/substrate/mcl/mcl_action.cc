#include "substrate/mcl_wrappers.h"
#include "substrate/moncon_logging.h"
#include "substrate/models.h"

using namespace MonCon;
using namespace umbc;

bool mclAware::mcl_action::activate() {
  // do the MCL stuff here...
  mclMA::declareExpectationGroup(mcl_key_of(),eg_key_of(),
				 get_parentref(),get_resref());
  // now declare expectations...
  // this is a virtual function that needs to be extended.
  declare_expectations();
  return peel()->activate();
}

bool mclAware::mcl_action::deactivate() {
  // do the MCL stuff here...
  bool rv = peel()->deactivate();
  unset_resref(); 
  unset_parent();
  mclMA::observables::update mcl_update_obj;
  my_mcl_agent->fill_out_update(mcl_update_obj);
  mclMA::expectationGroupComplete(mcl_key_of(),eg_key_of(),mcl_update_obj);
  return rv;
}

bool mclAware::mcl_action::declare_expectations() {
  // ask the agent for a model pointer
  uLog::annotate(MONL_HOSTERR,"can't generate expectations for at mcl_action level because plumbing (has_model/model_provider) is not in place.");
  return true;
}
