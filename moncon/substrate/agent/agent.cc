#include "substrate/agent.h"
#include "substrate/basic_observables.h"
#include "substrate/controller.h"
#include "mcl/mcl_api.h"
#include "umbc/token_machine.h"
#include "substrate/moncon_logging.h"
#include <errno.h>

using namespace MonCon;
using namespace umbc;

agent::agent(string name, domain* d) : 
  simEntity(name),knows_domain(d),
  understands_timeline_entries(), accepts_dispatched_commands() {};

timeline_entry* agent::parse_tle_spec( int firetime, timeline* tl,
				       string tles ) {
  
  // if it gets here then probably we should check with the controller
  return get_active_controller()->parse_tle_spec( firetime, tl, tles );
}

string agent::process_command(string comm) {
  if (comm == "dump") {
    dump();
    return success_msg();
  }
  else if (get_active_controller() != NULL)
    return get_active_controller()->process_command(comm);
  else return ignore_msg();
}

string agent::publish_interface() {
  uLog::annotate(MONL_HOSTERR,
		      "[mc/agent]:: agent "+get_name()+
		      " has an unpublished dispatch interface.");
  return "{ }";
}


timeline_entry* sensorimotor_agent::parse_tle_spec( int firetime, timeline* tl,
						    string tles ) {
  tokenMachine q(tles);
  string command = q.nextToken();

  return agent::parse_tle_spec( firetime, tl, tles );

  /*
  if (command == "break") {
    errno = 0;
    // int sindex = textFunctions::numval(q.nextToken());
    if (errno == 0) {
      // return new timeline_sensorbreak( firetime, tl, sindex );
      sprintf(timeline_error_msg,"sensorbreaks cannot currently be parsed.");
      return NULL;
    }
    else {
      uLog::annotate(MONL_WARNING,"[mars/mdom]::error parsing argument to 'break' timeline entry");
      sprintf(timeline_error_msg,"error parsing break target.");
      return NULL;
    }
  }
  else 
    */

}

