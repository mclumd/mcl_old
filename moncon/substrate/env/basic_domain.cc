#include "substrate/basic_domain.h"
#include "substrate/agent.h"
#include "substrate/substrate_timeline_entries.h"

#include "umbc/declarations.h"
#include "umbc/token_machine.h"

#include "mcl/mcl_api.h"

#include <errno.h>

using namespace MonCon;

basic_domain::basic_domain(string name) : 
  domain(name),understands_timeline_entries() { 
  cout << "[mcs/bdom]:: name supplied = " << name 
       << ", set to " << get_name() 
       << ", @0x" << hex << (unsigned long)this << endl;
};

agent* basic_domain::get_agent(string n) {
  for (agent_list::iterator li = a_list.begin();
       li != a_list.end();
       li++) {
    if ((*li)->get_name() == n)
      return *li;
  }
  return NULL;  
}

/*
void basic_domain::terminate_agent(agent* a) {
  a_list.remove(a);
  delete a;
}
*/

bool basic_domain::simulate() {
  umbc::declarations::make_declaration(1,"simStep");
  return true;
}

unsigned int basic_domain::tick() {
  return (unsigned int)umbc::declarations::get_declaration_count("simStep");
}

// this is for understands_timeline_entries, and ensures that the methods
// will always end here at the root.
//
// the subclasses of basic_domain should, if they do not know how to handle
// the specific type of tle_spec they are seeing, make a recursive call to the
// parent-class' method, which will eventually end up here to pass the 
// parse to the controller (if one exists)

timeline_entry* basic_domain::parse_tle_spec( int firetime, timeline* tl,
					      string tles ) {
  umbc::tokenMachine q(tles);
  string keyword = q.nextToken();
  if (keyword == "domain") {
    // check for any really generic domain events...
    string dcomm = q.nextToken();
    if (dcomm == "event") {
      string dcommtype = q.nextToken();
      if (dcommtype == "effectorbreak") {
	// cout << "tlspec: sensorbreak." << endl;
	// now we're talking
	string aname = q.nextToken();
	sensorimotor_agent* agnt = 
	  dynamic_cast<sensorimotor_agent*>(get_agent(aname));
	if (agnt == NULL) {
	  sprintf(timeline_error_msg,"effectorbreak attempted on non-sm agent.");
	  return NULL;
	}
	else {
	  // cout << "tlspec: sensorbreak for sm_agnt." << endl;
	  string which_ac = (q.nextToken());
	  return new timeline_effectorbreak(firetime, tl, agnt, which_ac);
	}
      }
      else if (dcommtype == "sensorbreak") {
	// cout << "tlspec: sensorbreak." << endl;
	// now we're talking
	string aname = q.nextToken();
	sensorimotor_agent* agnt = 
	  dynamic_cast<sensorimotor_agent*>(get_agent(aname));
	if (agnt == NULL) {
	  sprintf(timeline_error_msg,"sensorbreak attempted on non-sm agent.");
	  return NULL;
	}
	else {
	  // cout << "tlspec: sensorbreak for sm_agnt." << endl;
	  string which_sens = (q.nextToken());
	  int six = agnt->get_observable_index(which_sens);
	  if (six < 0) {
	    // GOI returns -1 if it can't find a named sensor....
	    // see if the supplied thingy is a number
	    errno=0;
	    six = umbc::textFunctions::numval(which_sens);
	    if (errno == 0) {
	      // cout << "tlspec: sensorbreak for numbered sensor." << endl;
	      return new timeline_sensorbreak(firetime, tl, agnt, six);
	    }
	    else {
	      sprintf(timeline_error_msg,"could not find sensor '%s'. must be a number or valid named sensor.",which_sens.c_str());
	      return NULL;
	    }
	  }
	  else {
	    return new timeline_sensorbreak(firetime, tl, agnt, six);
	  }
	}
      }
    }
    // if it's a "domain" keyword and it's gotten here, it is untrapped
    // it should be trapped/handled in overrides
    sprintf(timeline_error_msg,"'domain' command untrapped by any domain class.");
    return NULL;
  }
  else if (keyword == "for") {
    string agent_name = q.nextToken();
    agent* agnt = get_agent(agent_name);
    if (agnt == NULL) {
      sprintf(timeline_error_msg,"could not find agent '%s'.",agent_name.c_str());
      return NULL;
    }
    else {
      return agnt->parse_tle_spec( firetime, tl, q.rest() );
    }
  }
  else {
    sprintf(timeline_error_msg,"unknown command keyword '%s'.",keyword.c_str());
    return NULL;
  }
}
