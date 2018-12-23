#include "mars_scoring.h"

using namespace umbc;

void mcMars::mars_scorer::add_default_rules() {
  add_scoring_rule("enegyExpended",-0.10);
  add_scoring_rule("simStep",-.05);
  add_scoring_rule("failureToPano",-0.1);
  add_scoring_rule("bankOverflow",-10);
  add_scoring_rule("rescue",-10);
  add_scoring_rule("sensorResetLoop",-1);
  add_scoring_rule("brokenSensorReset",1);
}
