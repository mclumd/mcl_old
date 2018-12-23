#ifndef MARS_ROVER_MLAYER_H
#define MARS_ROVER_MLAYER_H

#include "substrate/maintenance_layer.h"

using namespace MonCon;
using namespace std;

namespace mcMars {

  class rover_maintenance_layer : public maintenance_layer {

  public:
    rover_maintenance_layer(controller* inna);

    virtual string class_name() { return "rover_maintenance_layer"; };
    virtual string describe() 
    { return "(rover_maintenance_layer wraps"+peel()->describe()+")"; };

  };
  
};

#endif
