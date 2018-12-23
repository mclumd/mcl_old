#ifndef MC_RELAY_STATION_H
#define MC_RELAY_STATION_H

#include "substrate/domain.h"
#include "substrate/basic_agent.h"
#include "substrate/dispatch_agent.h"
#include "substrate/goalspec.h"
#include "imagebank.h"

using namespace MonCon;

namespace mcMars {

class relay_station : public basic_agent {
 public:
  relay_station(string name, domain *d);

  virtual string process_command(string command);

  virtual string class_name() { return "relay_station"; };
  virtual string grammar_commname() { return class_name()+"_command"; };
  virtual bool   publish_grammar(umbc::command_grammar& grammar);

  bool receive(image_bank& ib);
  bool receive_goal(goalspec gs,bool pano);

  virtual string describe();

  virtual bool busy() { return false; };

 protected:
  vector<goalspec> outstanding_goals;
  
};

};

#endif
