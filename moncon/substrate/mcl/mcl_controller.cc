#include "substrate/mcl_wrappers.h"
#include "substrate/controller_wrapper.h"
#include "substrate/moncon_logging.h"
#include <stdio.h>

using namespace MonCon;
using namespace umbc;

mclAware::mcl_control_layer::mcl_control_layer(mcl_agent* mcla,
					       controller* inna_cont)
  : has_action_q(),controller_wrapper(inna_cont),my_mcl_agent(mcla) {
}

bool mclAware::mcl_control_layer::busy() {
  // subclasses should override this
  return peel()->busy();
}

bool mclAware::mcl_control_layer::control_in() {
  // maybe preempt...
  // or not...
  bool rv = peel()->control_in();
  return rv;
}

bool mclAware::mcl_control_layer::control_out() {
  bool rv = peel()->control_out();  
  // set up an update and monitor
  mclMA::observables::update mcl_update_obj;
  my_mcl_agent->fill_out_update(mcl_update_obj);
  responseVector mclrv = mclMA::monitor(mcl_key_of(),
					mcl_update_obj);
  // at this level, just tell MCL we are ignoring everything
  uLog::annotate(MONL_WARNING,personalize("control_out() is being called at the mclAware::mcl_control_layer level."));
  for (unsigned int k=0;k<mclrv.size();k++) {
    if (mclrv[k]) {
      mclMonitorResponse* tmr = mclrv[k];
      sprintf(uLog::annotateBuffer,"%s being ignored: (ref=0x%lx) msg='%s'",
	      tmr->rclass().c_str(),tmr->referenceCode(),tmr->responseText().c_str());
      uLog::annotateFromBuffer(MONL_WARNING);
      mclMA::suggestionIgnored(mcl_key_of(),tmr->referenceCode());
      delete tmr;
      mclrv[k]=NULL;
    }
  }
  return rv;
}

bool mclAware::mcl_control_layer::initialize() {
  return peel()->initialize();
}

bool mclAware::mcl_control_layer::is_locally_controlling() {
  uLog::annotate(MONL_HOSTERR,personalize("local control is impossible at the mcl_control_layer level (the base class should NOT be instantiated)."));
  return false; 
}
