#include "substrate/actionspec.h"
#include "substrate/actions.h"
#include "substrate/moncon_logging.h"
#include "umbc/exceptions.h"
#include "mcl/mcl_api.h"

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace MonCon;
using namespace umbc;

actionspec::actionspec(action* a) : ac(a),argcount(0) { 
  arg[0]=0; 
  if (a == NULL) 
    umbc::exceptions::signal_exception("actionspec created with NULL action pointer.");
}

actionspec::actionspec(action* a,int arg1) : ac(a),argcount(1) { 
  arg[0]=arg1; 
  if (a == NULL) 
    umbc::exceptions::signal_exception("actionspec created with NULL action pointer.");
}

/** prepares an actionspec for execution.
 * amounts to setting the parameters of the uncderlying action (ac)
 * to those set in the actionspec.
 */
void actionspec::prepare_for_activation() {
  uLog::annotate(MONL_HOSTMSG,"[mcs/aspec]:: activating " + to_string());
  // PREPARATIONS STEP #1: SET ACTION ARGS....
  simple_has_args* ac_as_sha = dynamic_cast<simple_has_args*>(ac);
  if (ac_as_sha == NULL) {
    if (argcount > 0) {
      cout << "--> crashing: actionspec has args, action can't be cast to has_args ("
	   << ac->get_name()
	   << ")" << endl;
      exit(-1);
    }
    // otherwise we don't do shit
  }
  else if (argcount < ac_as_sha->get_argcount()) {
    cout << "--> crashing: actionspec has fewer args than action it refers to ("
	 << ac->get_name()
	 << ")" << endl;
    exit(-1);
  }
  else {
    // copy args to action
    for (int i=0;i<argcount;i++) {
      sprintf(uLog::annotateBuffer,"[mars/as]:: setting acspec arg(%d)=%d",i,arg[i]);
      uLog::annotateFromBuffer(MONL_HOSTDBG);
      ac_as_sha->set_arg(i,arg[i]);
    }
  }
  /// ###mcl-EXTRACTION
  // PREPARATIONS STEP #2: PASS THROUGH RESPONSE_REF ARGS....
  // ac->set_mclref(get_mclref());
}

/** textual representation of an actionspec. 
 */
string actionspec::to_string() {
  char oo[512];
  if (ac == NULL)
    sprintf(oo,"AS|null|");
  else {
    sprintf(oo,"AS|%s|",ac->get_name().c_str());
    for (int i=0;i<argcount;i++) {
      sprintf(oo,"%s %d",oo,arg[i]);
    }
  }
  string oor = "("+(string)oo+")";
  return oor;
}
