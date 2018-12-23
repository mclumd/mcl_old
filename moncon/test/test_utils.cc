#include "mc_utils/command_grammar.h"
#include <iostream>

int main(int argc, char** argv) {
  command_grammar x;
  x.add_production("command","send agent agent_action");
  x.add_production("command","sv");
  x.add_production("agent","rover1");
  x.add_production("agent","rover2");
  x.add_production("agent_action","goal");
  cout << x.to_string();
}
