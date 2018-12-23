#include "substrate/basic_agent.h"
#include "substrate/actions.h"
#include "substrate/observables.h"
#include "substrate/controller.h"
#include "substrate/moncon_logging.h"
#include "umbc/settings.h"
#include "umbc/exceptions.h"

#include <stdio.h>
#include <algorithm>

using namespace MonCon;
using namespace umbc;

controller*  basic_agent::get_active_controller() 
{ return active_controller; }

void basic_agent::set_active_controller(controller * c)
{ active_controller=c; }

bool basic_agent::activate_action(action* a) {
  if (active_action != NULL) {
    active_action->deactivate();
    active_action=NULL;
  }
  bool rv = a->activate();
  if (a)
    active_action=a;
  return rv;
}

bool basic_agent::deactivate_action() {
  if (active_action != NULL) {
    active_action->deactivate();
    active_action=NULL;
  }
  return true;
}

action* basic_agent::get_action(string a) {
  for (action_list::iterator li = actions()->begin();
       li != actions()->end();
       li++) {
    if ((*li)->get_name() == a)
      return *li;
  }
  return NULL;
}

action* basic_agent::get_action(int i) {
  int q=0;
  // cout << "getting action " << i << endl;
  for (action_list::iterator li = a.begin();
       li != a.end();
       li++) {
    if (q == i)
      return (*li);
    else
      q++;
  }
  return NULL;
}

int basic_agent::get_action_index(string a) {
  int aix = 0;
  for (action_list::iterator li = actions()->begin();
       li != actions()->end();
       li++) {
    if ((*li)->get_name() == a)
      return aix;
    else
      aix++;
  }
  return -1;
}

void basic_agent::replace_action(action* old, action* nu) {
  action_list::iterator optr = find(a.begin(),a.end(),old);
  if (optr == a.end())
    exceptions::signal_exception("replace failure: old action not found in actionlist");
  else {
    optr = a.erase(optr);
    a.insert(optr,nu);
  }
}

simple_repair_action* basic_agent::get_repair(string a) {
  for (repair_list::iterator li = r.begin();
       li != r.end();
       li++) {
    cout << "looking for " << a << "(" << (*li)->get_name() << ")" << endl;
    if ((*li)->get_name() == a)
      return *li;
  }
  return NULL;
}

simple_repair_action* basic_agent::get_repair(int i) {
  int q=0;
  // cout << "getting action " << i << endl;
  for (repair_list::iterator li = r.begin();
       li != r.end();
       li++) {
    if (q == i)
      return (*li);
    else
      q++;
  }
  return NULL;
}

observable* basic_agent::get_observable_n(int i) {
  int q=0;
  for (observable_list::iterator li = observables()->begin();
       li != observables()->end();
       li++) {
    if (q == i)
      return (*li);
    else 
      q++;
  }
  return NULL;
}

int basic_agent::get_observable_index(string s) {
  int i=0;
  for (observable_list::iterator li = observables()->begin();
       li != observables()->end();
       li++) {
    if ((*li)->get_name() == s)
      return i;
    else
      i++;
  }
  return -1;
}

observable*  basic_agent::get_observable_core(string s) 
{ return (get_observable(s))->core_observable(); };

observable* basic_agent::get_observable_core_n(int i) 
{ return (get_observable_n(i))->core_observable(); };

double basic_agent::get_observable_value_dbl(string s)
{ return (get_observable(s))->double_value(); };

int basic_agent::get_observable_value_int(string s)
{ return (get_observable(s))->int_value(); };


observable* basic_agent::get_observable(string a) {
  for (observable_list::iterator li = observables()->begin();
       li != observables()->end();
       li++) {
    if ((*li)->get_name() == a)
      return *li;
  }
  return NULL;
}

bool basic_agent::simulate() {
  if (active_action)
    return active_action->simulate();
  else return true;
}

bool basic_agent::shutdown() { return true; }

timeline_entry* basic_agent::parse_tle_spec( int firetime, timeline* tl,
					     string tles ) {
  // uLog::annotate(MONL_HOSTMSG,
  // "[mc/agent]:: "+get_name()+"(BA) has the tles: "+tles);
  return sensorimotor_agent::parse_tle_spec( firetime, tl, tles );
}

string basic_agent::process_command(string command) {
  if (get_active_controller())
    return get_active_controller()->process_command(command);
  else
    return ignore_msg();
}

string basic_agent::sv_as_str() {
  string rv="{ ";
  for (observable_list::iterator oli = observables()->begin();
       oli != observables()->end();
       oli++) {
    rv+=(*oli)->get_name();
    rv+="=";
    char b[64];
    sprintf(b,"%.2f ",(*oli)->double_value());
    rv+=b;
  }
  rv+="}";
  return rv;
}

string basic_agent::ev_as_str() {
  string rv="{ ";
  for (action_list::iterator ali = actions()->begin();
       ali != actions()->end();
       ali++) {
    rv+=(*ali)->describe();
    rv+=" ";
  }
  rv+="}";
  return rv;
}

/*
string basic_agent::mcl_sv_as_str() {
  return mclMA::observables::ov_as_string(mcl_agentspec());
}
*/
