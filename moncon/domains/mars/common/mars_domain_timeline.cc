#include "umbc/declarations.h"
#include "umbc/settings.h"
#include "mcl/mcl_api.h"
#include "mars_logging.h"
#include "mars_domain_timeline.h"

using namespace mcMars;
using namespace umbc;

void mars_timeline_event::fire() {
  if (settings::getSysPropertyBool("mars.noEvents",false)) {
    umbc::uLog::annotate(MARSL_HOSTMSG,"[mars/mdtime]:: suppressing event due to (mars.noEvents=true).");
  }
  else {
    umbc::uLog::annotate(MARSL_HOSTMSG,"[mars/mdtime]:: firing event.");
    ((mars_domain*)(get_timeline()->domain_of()))->force_event(evCode,evSeed);
  }
}

string mars_timeline_event::describe() {
  char b[256];
  sprintf(b,"<event _mars_ code=0x%08x seed=%ld>",evCode,evSeed);
  return (string)b;
}
