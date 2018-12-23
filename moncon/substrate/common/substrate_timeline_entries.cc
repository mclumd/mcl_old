#include "substrate/substrate_timeline_entries.h"
#include "substrate/agent.h"
#include "substrate/actions.h"
#include "substrate/observables.h"
#include "substrate/moncon_logging.h"
#include "umbc/settings.h"
#include "mcl/mcl_api.h"

#include <stdio.h>

using namespace MonCon;
using namespace umbc;

void timeline_sensorbreak::fire() {
  if (settings::getSysPropertyBool("moncon.noSBEs",false)) {
    umbc::uLog::annotate(MONL_HOSTMSG,"[mars/mdtime]:: suppressing sensorbreak due to (moncon.noSBEs=true).");
  }
  else {
    uLog::annotate(MONL_HOSTMSG,"[mcs/stle]:: sensorbreak event.");
    breakable*   dbs = observable_utils::get_breakable(a->get_observable_n(sensor));
    if (dbs != NULL) {
      uLog::annotate(MONL_HOSTEVENT,"[mcs/stle]:: sensor broken "+a->get_observable_n(sensor)->get_name());
      umbc::declarations::make_declaration(1,"timeline.sBreak");
      dbs->break_it();
    }
    else {
      uLog::annotateStart(MONL_WARNING);
      *uLog::log << "[mcs/stle]:: breakage scheduled on unbreakable sensor " << sensor << endl;
      uLog::annotateEnd(MONL_WARNING);
      umbc::declarations::make_declaration(1,"timeline.timelineError");
    }
  }
}

string timeline_sensorbreak::describe() {
  char b[257];
  sprintf(b,"<event _sens-brk_ %s,%d>",a->get_name().c_str(),sensor);
  return b;
}

void timeline_effectorbreak::fire() {
  if (settings::getSysPropertyBool("moncon.noEBEs",false)) {
    umbc::uLog::annotate(MONL_HOSTMSG,"[mars/mdtime]:: suppressing effectorbreak due to (moncon.noSBEs=true).");
  }
  else {
    uLog::annotate(MONL_HOSTMSG,"[mcs/stle]:: effectorbreak event.");
    action* wac = a->get_action(action_name);
    if (wac) { 
      action_wrappers::breakable_layer* wacbl = 
	action_wrappers::action_wrapper::get_typed_wrapper<action_wrappers::breakable_layer*>(wac);
      if (wacbl) {
	wacbl->break_it();
	uLog::annotate(MONL_HOSTEVENT,"[mcs/stle]:: effector broken "+action_name);
	umbc::declarations::make_declaration(1,"timeline.eBreak");
      }
    }
    else {
      uLog::annotate(MONL_WARNING,
		     "[mcs/stle]:: breakage scheduled on unbreakable action "
		     +action_name);
      umbc::declarations::make_declaration(1,"timeline.timelineError");
    }
  }
}

string timeline_effectorbreak::describe() {
  char b[257];
  sprintf(b,"<event _eff-brk_ %s,%s>",
	  a->get_name().c_str(),action_name.c_str());
  return b;
}
