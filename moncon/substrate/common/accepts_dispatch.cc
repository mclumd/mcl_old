#include "substrate/accepts_dispatch.h"
#include "substrate/agent.h"
#include "umbc/token_machine.h"
#include "mcl/mcl_api.h"

using namespace MonCon;
using namespace umbc;
 
string accepts_dispatched_commands::agent_command2tles(agent* a,string command) {
  string rs = "";
  tokenMachine tm(command);
  string attime = tm.keywordValue("at");
  rs += "at " + attime;
  rs += " for "+a->get_name()+" ";
  string cmat = tm.removeKWP("at");
  rs += cmat;
  return rs;
}
