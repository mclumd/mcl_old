#include "substrate/mcl_wrappers.h"
#include "substrate/mcl_aware.h"
#include "substrate/observables.h"
#include "substrate/controller.h"
#include "substrate/moncon_logging.h"
#include "substrate/actions.h"
#include "umbc/settings.h"
#include "umbc/exceptions.h"

using namespace MonCon;
using namespace umbc;

spvType mc2mcl_odt(observable_data_type dt);
spvType mc2mcl_osc(observable_sensor_class sc);

mclAware::mcl_agent::mcl_agent(sensorimotor_agent* subagent,bool autowrap) :
  sensorimotor_agent_wrapper(subagent),
  has_mcl_key() {
  if (autowrap) {
    autowrap_actions();
    autowrap_controller();
  }
}

agent* mclAware::mcl_agent::attempt_wrap(agent* R) {
  // slippery slope -- this will fail if we start mutliply-wrapping agents
  if (!R) exceptions::signal_exception("cannot wrap NULL.");
  else {
    sensorimotor_agent* RaMR = dynamic_cast<sensorimotor_agent*>(R);
    if (RaMR)
      return new mcl_agent(RaMR);
    else
      exceptions::signal_exception("mcl_agent cannot cast "+
				   R->get_name()+" to SM AGENT for wrapping.");
  }
  return R;
}

void mclAware::mcl_agent::autowrap_actions() {
  // auto-replace the actions with mcl-wrapped variants 
  // (if they are not already wrapped)
  //
  // I'd say 10 times out of 10 you want to override this method with one
  // that uses mcl_action extended classes so you can override
  // generate_expectations() and actually generate them
  //
  for (int i=0;i<sub->num_actions();i++) {
    action* k  = sub->get_action(i);
    if (!action_wrappers::action_wrapper::get_typed_wrapper<mclAware::mcl_action*>(k)) {
      action* wa = new mcl_action(this,k);
      sub->replace_action(k,wa);
      uLog::annotate(MONL_HOSTMSG,sub->get_name()+": wrapped action (g) "+k->get_name());
    }
  }
}

void mclAware::mcl_agent::autowrap_controller() {
  controller* nu_cntrl = 
    new mcl_control_layer(this,get_active_controller());
  set_active_controller(nu_cntrl);
}

bool mclAware::mcl_agent::initialize()  { 
  bool rv = declare_mcl();
  rv &= peel()->initialize();
  return rv;
}

bool mclAware::mcl_agent::shutdown() { 
  bool rv = peel()->shutdown();
  rv &= undeclare_mcl();
  return rv;
}

bool mclAware::mcl_agent::simulate() {
  bool rv = peel()->simulate();
  // rv &= update_mcl_update();
  return rv;
}

string mclAware::mcl_agent::mcl_key_of() {
  return domain_of()->get_name()+"."+get_name();
}

bool mclAware::mcl_agent::undeclare_mcl() {
  return mclMA::releaseMCL(mcl_key_of());
}

bool mclAware::mcl_agent::declare_mcl() {
  if (settings::getSysPropertyBool("moncon.usemcl",true)) {
    string kee = mcl_key_of();
    // initialize with MA MCL API
    mclMA::initializeMCL(kee,0); // assumes synchronous
    // now specify configuration
    if (get_active_controller())
      mclMA::configureMCL(kee,domain_of()->class_name(),
			  peel()->class_name(),
			  controller_wrapper::get_core_controller(get_active_controller())->class_name());
    else
      mclMA::configureMCL(kee,domain_of()->class_name(),class_name());
    // now find the correct ontology
    mclMA::chooseOntology(kee,
			  mclAware::select_ontology_for_agent(get_name(),
						    domain_of()->class_name(),
						    peel()->class_name(),
						    controller_wrapper::get_core_controller(get_active_controller())->class_name()));
    // now register observables (sensors)
    for (observable_list::iterator sli = observables()->begin();
	 sli != observables()->end();
	 sli++) {
      // declare the sensor the best you can
      mclMA::observables::declare_observable_self(kee,(*sli)->get_name());
      mclMA::observables::set_obs_prop_self(kee,(*sli)->get_name(),PROP_DT,
					    mc2mcl_odt((*sli)->get_odt()));
      mclMA::observables::set_obs_prop_self(kee,(*sli)->get_name(),PROP_SCLASS,
					    mc2mcl_osc((*sli)->get_osc()));
    }
    
    // new with the observables update...
    // now create the mcl observable updater
    mclMA::observables::update mcl_update_obj;
    fill_out_update(mcl_update_obj);
    mclMA::updateObservables(kee,mcl_update_obj);
    
    // and initialize it over on the MCL side
    return true;
  }
  else
    return false;
}

bool mclAware::mcl_agent::fill_out_update(mclMA::observables::update& mcl_update_obj) {
  for (observable_list::iterator sli = observables()->begin();
       sli != observables()->end();
       sli++) {
    mcl_update_obj.set_update((*sli)->get_name(),
			      (float)((*sli)->double_value()));
  }
  return true;
}

string mclAware::select_ontology_for_agent(const string& agent_name,
					   const string& domain_class_name,
					   const string& agent_class_name,
					   const string& cont_class_name) {
  // agent-name as key...
  string anbos = settings::getSysPropertyString(agent_name+".mclOntology","");
  uLog::annotate(MONL_HOSTMSG,"Trying agent_name... setting="+anbos);
  if ((anbos != "") && mclMA::legalOntologySpec(anbos)) return anbos;

  // agent-spec from controller to domain as key...
  string ccbos = settings::getSysPropertyString(domain_class_name+"."+
				    agent_class_name+"."+
				    cont_class_name+".mclOntology",
				    "");
  uLog::annotate(MONL_HOSTMSG,"Trying full agent spec... setting="+ccbos);
  if ((ccbos != "") && mclMA::legalOntologySpec(ccbos)) return ccbos;

  // agent-spec using agent & domain as key...
  string acbos = settings::getSysPropertyString(domain_class_name+"."+
				    agent_class_name+".mclOntology",
				    "");
  uLog::annotate(MONL_HOSTMSG,"Trying full agent/dom spec... setting="+acbos);
  if ((acbos != "") && mclMA::legalOntologySpec(acbos)) return acbos;

  // agent-spec is domain only...
  string dcbos = settings::getSysPropertyString(domain_class_name+".mclOntology","");
  uLog::annotate(MONL_HOSTMSG,"Trying full domain spec... setting="+dcbos);
  if ((dcbos != "") && mclMA::legalOntologySpec(dcbos)) return dcbos;
  else {
    uLog::annotate(MONL_HOSTMSG,"no ontology setting for "+agent_name);
    return "";
  }

}


/*

responseVector mclAware::mcl_agent::monitor_mcl() {
  update_mcl_update();
  if (settings::getSysPropertyBool("moncon.usemcl",true))
    return mclMA::monitor(mcl_agentspec(),mcl_update_obj);
  else return mclMA::emptyResponse();
}

*/

spvType mc2mcl_odt(observable_data_type dt) {
  switch (dt) {
  case ODT_INTEGER: return DT_INTEGER;
  case ODT_FLOAT: return DT_RATIONAL;
  case ODT_BOOL: return DT_BINARY;
  case ODT_SYMBOL: return DT_SYMBOL;
  default: return DT_SYMBOL;
  }
}

spvType mc2mcl_osc(observable_sensor_class sc) {
  switch (sc) {
  case OSC_RESOURCE: return SC_RESOURCE;
  case OSC_DOMAIN_SENSOR: return SC_AMBIENT;
  case OSC_STATE: return SC_STATE;
  case OSC_MESSAGE: return SC_MESSAGE;
  case OSC_TEMPORAL: return SC_TEMPORAL;
  case OSC_UNSPEC:
  default: return SC_UNSPEC;
  }
}
