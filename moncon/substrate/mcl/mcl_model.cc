#include "substrate/mcl_model.h"
#include "substrate/domain.h"
#include "substrate/actions.h"
#include "substrate/model_effects.h"
#include "substrate/observables.h"
#include "substrate/agent.h"
#include "substrate/simEntity.h"
#include "substrate/moncon_logging.h"
#include "umbc/settings.h"
#include "mcl/mcl_api.h"

using namespace MonCon;



/*

mcl_action_model::mcl_action_model(action* a) : action_model(a),has_eg_key() {
  // make sure that the agent is mcl_sensing or the deal is off...
  sensorimotor_agent* mfa_as_mcls = 
    dynamic_cast<sensorimotor_agent*>(a->agent_of());
  if (mfa_as_mcls != NULL)
    model_agent=mfa_as_mcls;
  else
    umbc::uLog::annotate(MONL_HOSTERR,"[mclmodel]:: model created for agent not of type mcl_sensing.");
}

bool mcl_action_model::tell_mcl(string key,egkType parent_key,resRefType response_ref) {
  umbc::uLog::annotate(MONL_HOSTERR,"[mcs/mcl]:: tell_mcl DOES NOT WORK.");
  // note that model_for is hardwired as the egk here.... maybe not desirable.
  if (umbc::settings::getSysPropertyBool("moncon.usemcl",true)) {
    
    umbc::uLog::annotate(MONL_HOSTMSG,"[mcs/mcl]:: DEG IN.");
    mclMA::declareExpectationGroup(key,mcl_eg_key_for(),
				   parent_key,response_ref);
    umbc::uLog::annotate(MONL_HOSTMSG,"[mcs/mcl]:: DEG OUT.");
    for (list<action_effect*>::iterator aeli = effects.begin();
	 aeli != effects.end();
	 aeli++) {
      umbc::uLog::annotate(MONL_HOSTMSG,"[mcs/mcl]:: genExp (key="+key+")");
      // (*aeli)->gen_expectation(key,mcl_eg_key_for(),model_agent);
    }
    umbc::uLog::annotate(MONL_HOSTMSG,"[mcs/mcl]:: tell_mcl for "+key+" returns true");
    return true;
  }
  else {
    return false;
    umbc::uLog::annotate(MONL_HOSTMSG,"[mcs/mcl]:: tell_mcl for "+key+" returns false");
  }
}

string mcl_action_model::describe() {
  string ov = "<mcl_model";
  if (models_this_action()) ov+=" " + models_this_action()->get_name();
  ov+=">";
  return ov;
}

*/
