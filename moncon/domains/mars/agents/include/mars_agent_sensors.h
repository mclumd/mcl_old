#ifndef MARS_AGENT_SEN_H
#define MARS_AGENT_SEN_H

#include "substrate/basic_observables.h"
#include "mcl/mcl_api.h"
#include "mars_domain.h"

using namespace MonCon;

namespace mcMars {

class mars_state : public basic_observable_state {
 public:
  mars_state(string nm, double iv, agent* a) : 
    basic_observable_state(nm,iv,a) { };
  virtual string class_name() { return "mars_state"; };
};

class mars_msgbox : public basic_message_box {
 public:
  mars_msgbox(string nm, double iv, agent* a) : 
    basic_message_box(nm,iv,a) { };
  virtual string class_name() { return "mars_msgbox"; };
};

class mars_timer : public basic_observable_timer {
 public:
  mars_timer(string nm, double iv, agent* a) : 
    basic_observable_timer(nm,iv,a) {  };
  virtual string class_name() { return "mars_timer"; };
};

class mars_resource : public basic_observable_resource {
 public:
  mars_resource(string nm, double iv, agent* a) : 
    basic_observable_resource(nm,a,iv) { };
  virtual string class_name() { return "mars_resource"; };
};

class mars_env_sensor : public abstract_environmental_sensor {
 public:
  mars_env_sensor(string nm, apvType iv, agent* a, apkType env_property);

  virtual double double_value();
  virtual int    int_value()     { return (int)double_value(); };

  virtual string class_name() { return "mars_env_sensor"; };

 protected:
  apkType prop;

};

class mars_db_timer : public declaration_based_observable {
 public:
  mars_db_timer(string name, agent* a, const char* decl);

  virtual ~mars_db_timer() {};
  
  virtual int int_value() { return umbc::declarations::get_declaration_count(the_dec) - last_set; };
  virtual bool attempt_set(double v)  { 
    last_set = umbc::declarations::get_declaration_count(the_dec) + (int)v;
    return true; 
  };
  virtual string class_name() { return "mars_db_timer"; };
  
 protected:
  int last_set;

};

};

#endif
