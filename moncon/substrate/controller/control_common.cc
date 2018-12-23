#include "substrate/control_common.h"
#include "substrate/agent.h"
#include "substrate/actions.h"
#include "mcl/mcl_api.h"
#include "umbc/text_utils.h"
#include "umbc/token_machine.h"
#include "substrate/moncon_logging.h"

#include <iostream>

using namespace MonCon;
using namespace umbc;

has_action_q::~has_action_q() {
  while (!actionq.empty()) {
    actionspec* a = actionq.front();
    actionq.pop_front();
    delete a;
  }
}

void has_action_q::clear_current() {
  // crush the existing "current action"
  if (current_aspec != NULL) {
    delete current_aspec;
    current_aspec = NULL;
  }
}

void has_action_q::advance_q() {
  clear_current();
  // set "current" to front of aq
  current_aspec=tlaspec();
  actionq.pop_front();
}

void has_action_q::clear_action_q() {
  while(has_pending_tla()) 
    advance_q();
  clear_current();
}

void has_action_q::dump_action_q() {
  uLog::annotateStart(MONL_PRE);
  *uLog::log << "ActionQ has " << actionq.size() 
	    << " pending actionz." << endl;
  int c = 1;
  for (list<actionspec*>::iterator asli = actionq.begin();
       asli != actionq.end();
       asli++) {
    string hell = (*asli)->to_string();
    *uLog::log << "  " << dec << c++ << ") " 
	       << hex << (*asli)->underlying_action() << " "
	       << hell << endl;
  }
  uLog::annotateEnd(MONL_PRE);
}

bool has_action_q::accept_action_string(string as, acting* a) {
  tokenMachine tm(as);
  string acname = tm.nextToken();
  for (action_list::iterator ali = a->actions()->begin();
       ali != a->actions()->end();
       ali++) {
    // cout << " ==> [" << (*ali)->get_name() << "] vs. ["
    // << acname << "]" << endl;
    if ((*ali)->get_name() == acname) {
      actionspec* nu_a = new actionspec(*ali);
      int argi = 0;
      while (tm.moreTokens() && (argi < ACTIONSPEC_MAXARGS)) {
	double tad = textFunctions::dubval(tm.nextToken());
	nu_a->change_arg(argi,(int)tad);
	argi++;
	nu_a->change_argcount(argi);
      }
      add_aspec_back(nu_a);
      return true;
    }
  }
  return false;
}

// fp = functor/parameters
// aspec_reststring should start with "functor(parameters ...)"
// 
actionspec* has_action_q::fp_string2aspec(string fp_aspec_reststring,
					  acting* a) {
  tokenMachine tm(fp_aspec_reststring);
  string fp_aspec = tm.nextToken();
  string functor = textFunctions::getFunctor(fp_aspec);

  actionspec* nu_a=NULL;

  for (action_list::iterator ali = a->actions()->begin();
       ali != a->actions()->end();
       ali++) {
    if (((*ali)->get_name() == functor) && !nu_a) {
      nu_a = new actionspec(*ali);

      string params  = textFunctions::getParameters(fp_aspec);
      paramStringProcessor pss(params);
      int argi = 0;
      while (pss.hasMoreParams() && (argi < ACTIONSPEC_MAXARGS)) {
	double tad = textFunctions::dubval(pss.getNextParam());
	nu_a->change_arg(argi,(int)tad);
	argi++;
	nu_a->change_argcount(argi);
      }
      return nu_a;
    }
  }
  return NULL;
}
