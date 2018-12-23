#include "substrate/maintenance_layer.h"
#include "substrate/moncon_logging.h"
#include "substrate/actions.h"
#include "umbc/exceptions.h"

#include <stdio.h>

using namespace MonCon;
using namespace umbc;

maintenance_layer::maintenance_layer(controller* inna_cont)
  : has_action_q(),controller_wrapper(inna_cont) {
}

bool maintenance_layer::busy() { 
  return (has_pending_tla() || peel()->busy());
}

bool maintenance_layer::control_in() {
  uLog::annotate(MONL_HOSTMSG,
		 personalize("maintenance = "+schedule2str()));

  if (!peel()->busy()) {
    // if repair is on the schedule, and no one downstream is busy, 
    // queue a repair
    string doa = next_due();
    if (doa != "") {
      uLog::annotate(MONL_HOSTMSG,"maintenance layer queueing '"+doa+"'");
      // add a aspec
      actionspec* aspec = generate_aspec_for(doa);
      if (aspec) 
	add_aspec_back(aspec);
      else 
	exceptions::signal_exception("attempt to schedule action "+doa+" that does not exist.");
      just_scheduled(doa);
    }
  }

  if (has_pending_tla()) {
    tlaspec()->prepare_for_activation();
    // uLog::annotate(MONL_HOSTERR,"MAINTENANCE LAYER ACTIVATION NOT IMPLEMENTED!!");
    process_action(tlaspec()->underlying_action());
    advance_q();
    return true;
  }
  
  // if we got here there was nothing for the ML to do
  return peel()->control_in();

}

bool maintenance_layer::control_out() {
  return peel()->control_out();
}

///////////////////////////////////////////////////////////////////////
/// VIRTUAL INTERNAL STUFF !!

actionspec* maintenance_layer::generate_aspec_for(string aname) {
  actionspec* ras = NULL;
  action*     bac = NULL;
  // check for SMA / get_action()
  sensorimotor_agent* sma = dynamic_cast<sensorimotor_agent*>(agent_of());
  if (sma) {
    bac = sma->get_action(aname);
  }
  // no? check for HRA / get_repair()
  if (!bac) {
    has_repair_actions* hra = dynamic_cast<has_repair_actions*>(sma);
    if (hra) {
      bac = hra->get_repair(aname);
    }
  }
  // still no action? that's a problem...
  if (!bac)
    exceptions::signal_exception("maintenance_layer::generate_aspec_for() failed to retrieve the following action: "+aname);
  else {
    // dude, the base level can't deal with args...
    if (bac->get_argcount() > 0)
      uLog::annotate(MONL_HOSTERR,"action "+aname+" is activated by the maintenance_layer level generate_aspec_for() but requires arguments... override is prescribed.");
    else {
      ras = new actionspec(bac);
    }
  }
  return ras;
}


bool maintenance_layer::process_action(action* ua) {
  bool aarv = true;
  if (!ua)
    return false;
  else {
    aarv &= ua->sensorimotor_agent_of()->activate_action(ua);
  }
  if (aarv)
    umbc::declarations::declare(personalize("maintenance.activation.ok"));
  else
    umbc::declarations::declare(personalize("maintenance.activation.fail"));
  return aarv;
}

///////////////////////////////////////////////////////////////////////
/// SCHEDULE STUFF !!

string maintenance_layer::schedule2str() {
  string q="{ ";
  for (map<string,si>::iterator sii = s_every.begin();
       sii != s_every.end();
       sii++) {
    char buff[128];
    sprintf(buff,"%s(every %d, last %d) ",sii->first.c_str(),
	    how_often(sii->first),scheduled_last(sii->first));
    q+=buff;
  }
  q+="}";
  return q;
}

maintenance_layer::si maintenance_layer::how_often(string an) { 
  if (s_every.find(an) != s_every.end()) return s_every[an];
  else return 0;
}

maintenance_layer::si maintenance_layer::scheduled_last(string an) { 
  if (s_every.find(an) != s_every.end())  return s_last[an];
  else return 0;
}

void maintenance_layer::just_scheduled(string an) { 
  if (s_every.find(an) != s_every.end())  
    s_last[an]=agent_of()->domain_of()->tick();
}

bool maintenance_layer::do_never(string an) {
  return ((s_every.find(an) == s_every.end()) || 
	  (s_every[an] == DO_NEVER));
}

bool maintenance_layer::is_due(string a_name) {
  if (!do_never(a_name))
    return (agent_of()->domain_of()->tick() >=
	    (scheduled_last(a_name)+how_often(a_name)));
  else return false;
}

string maintenance_layer::next_due() {
  for (map<string,si>::iterator sii = s_every.begin();
       sii != s_every.end();
       sii++) {
    if (is_due(sii->first))
      return sii->first;
  }
  return "";
}


bool maintenance_layer::abort_current() {
  if (has_pending_tla()) {
    clear_action_q();
    return peel()->abort_current();
  }
  else return peel()->abort_current();
}

bool maintenance_layer::tell(string message) {
  // semantics -- if it's doing something 
  return peel()->tell(message);
}
