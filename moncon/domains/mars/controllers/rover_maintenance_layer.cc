#include "rover_maintenance_layer.h"
#include "umbc/settings.h"

using namespace umbc;
using namespace mcMars;

rover_maintenance_layer::rover_maintenance_layer(controller* inna) 
  : maintenance_layer(inna) {
  // set up schedule...
  set_scheduled_action("rebuild",
		       settings::getSysPropertyInt("mars.rover.mlMRevery",100));
  set_scheduled_action("sensReset",
		       settings::getSysPropertyInt("mars.rover.mlSRevery",100));
  set_scheduled_action("effReset",
		       settings::getSysPropertyInt("mars.rover.mlERevery",100));
  // set_scheduled_action("rescue",100);
}
