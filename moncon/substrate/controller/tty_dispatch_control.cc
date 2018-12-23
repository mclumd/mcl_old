#include "substrate/tty_dispatch_control.h"
#include "substrate/dispatch_agent.h"
#include "umbc/token_machine.h"
#include "mcl/mcl_api.h"

#include <string.h>
#include <iostream>

using namespace std;
using namespace MonCon;
using namespace umbc;

bool tty_dispatch_control::control_in() {
  char buff[128];
  buff[0]='\0';
  while (strcmp(buff,"exit") != 0) {
    cout << agent_of()->get_name() << "> ";
    cin.getline(buff,127);
    if (strlen(buff) == 0)
      return true;
    else if ((strcmp(buff,"h") == 0) || (strcmp(buff,"help") == 0)) {
      print_help();
    }
    else if (strcmp(buff,"d") == 0) {
      agent_of()->domain_of()->dump();
    }
    else if (strcmp(buff,"k") == 0) {
      cout << umbc::declarations::dec_cnts_as_str() << endl;
    }
    else if (strcmp(buff,"a") == 0) {
      cout << show_agents() << endl;
    }
    else if (strcmp(buff,"m") == 0) {
      mclMA::dumpMCL();
    }
    else if (strcmp(buff,"g") == 0) {
      cout << generate_grammar() << endl;
    }
    else if (strcmp(buff,"exit") == 0) { }
    else if (strncmp(buff,"send",4) == 0) {
      tokenMachine tm(buff);
      tm.nextToken(); // strip off the command
      cout << ((basic_dispatch_agent*)agent_of())->dispatch_command(tm.rest())
	   << endl;
    }
    else {
      cout << "unrecognized command '" << buff << "'" << endl;
    }
  }
  exit(0);
}

bool tty_dispatch_control::control_out()   { return true; }

void tty_dispatch_control::print_help() {
  cout << "# You are using the MonCon TTY Dispatch Controller." << endl;
  cout << "# This controller dispatches keyboard commands to other MonCon agents." << endl << endl;
  cout << "# TTY Local Commands:" << endl;
  cout << "#  'd' to dump domain configuration." << endl;
  cout << "#  'h' or help to display this message." << endl;
  cout << "#  'a' to list available agents." << endl;
  cout << "#  'k' to display the declarations." << endl;
  cout << "#  'exit' to end the MonCon simulation." << endl;
  cout << "#  hit return to end TTY input and advance the simulation one step." << endl;
  cout << "#  type 'send <agent> <command>' to send a text command to another agent." << endl;
  cout << "q to quit" << endl;
  
}

bool tty_dispatch_control::publish_grammar(umbc::command_grammar& grammar) {
  return false;
}
