#include "mars_agent_sensors.h"

using namespace mcMars;

mars_env_sensor::mars_env_sensor(string nm, apvType iv, 
				 agent* a, apkType env_property) :
  abstract_environmental_sensor(nm,a),prop(env_property)
{
  ((mars_domain*)domain_of())->attempt_init_agent_property(agent_of(),
							   env_property,
							   iv);
  cout << "[mcd/mas]:: created " << nm << " poll=" << double_value() << endl;
}

double mars_env_sensor::double_value() {
  return ((mars_domain*)domain_of())->request_agent_property(agent_of(),prop);
}

mars_db_timer::mars_db_timer(string name, agent* a, const char* decl) :
  declaration_based_observable(name,a,decl) { 
  // the_dec = decl;
  printf("the_dec = %s\n",the_dec.c_str());
  last_set = umbc::declarations::get_declaration_count(the_dec);
  set_osc(OSC_TEMPORAL);
}

