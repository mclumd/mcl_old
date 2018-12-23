#ifndef MC_SUBSTRATE_B_OBSERVABLE_H
#define MC_SUBSTRATE_B_OBSERVABLE_H

#include "umbc/declarations.h"
#include "substrate/observables.h"
#include <math.h>

namespace MonCon {

class agent;

class basic_observable : public observable {
 public:
 basic_observable(string n, agent* a) : 
  observable(n,a),my_odt(ODT_UNSPEC),my_osc(OSC_UNSPEC) {};

  virtual observable_data_type    get_odt() { return my_odt; };
  virtual observable_sensor_class get_osc() { return my_osc; };
  virtual void  set_odt(observable_data_type odt)    { my_odt=odt; };
  virtual void  set_osc(observable_sensor_class osc) { my_osc=osc; };

 private:
  observable_data_type my_odt;
  observable_sensor_class my_osc;

};

class basic_settable_observable_dbl : public basic_observable {
 public:
  basic_settable_observable_dbl(string n, agent* a) : 
    basic_observable(n,a),BSO_value(0.0) {};
  virtual bool   attempt_set(double nv);
  virtual bool   attempt_set(int nv);
  virtual bool   attempt_dec(double nv);
  virtual bool   attempt_inc(double nv);
  virtual double double_value() { return BSO_value; };
  virtual int    int_value()    { return (int)BSO_value; };

  virtual bool   increment() { BSO_value++; return true; };
  virtual bool   increment(double a) { BSO_value+=a; return true; };

 protected:
  double BSO_value;

};

// the agent has unrestricted, idealized access to state variables

class basic_observable_state : public basic_settable_observable_dbl {
 public:
  basic_observable_state(string n, agent* a) : 
    basic_settable_observable_dbl(n,a)
    { set_osc(OSC_STATE); };
  basic_observable_state(string n,double iv, agent* a) : 
    basic_settable_observable_dbl(n,a)
    { set_osc(OSC_STATE); attempt_set(iv); };
  
};

class basic_observable_timer : public basic_settable_observable_dbl {
 public:
  basic_observable_timer(string n, agent* a) : 
    basic_settable_observable_dbl(n,a)
    { set_osc(OSC_TEMPORAL); };
  basic_observable_timer(string n,double iv, agent* a) : 
    basic_settable_observable_dbl(n,a)
    { set_osc(OSC_TEMPORAL); attempt_set(iv); };
  
};

class basic_message_box : public basic_settable_observable_dbl {
 public:
  basic_message_box(string n, agent* a) : 
    basic_settable_observable_dbl(n,a)
    { set_osc(OSC_MESSAGE); };
  basic_message_box(string n,double iv, agent* a) : 
    basic_settable_observable_dbl(n,a)
    { set_osc(OSC_MESSAGE); attempt_set(iv); };

};

// the agent models the creation and consumption of resources but may not have
// ideal access to their value

class basic_observable_resource : public basic_settable_observable_dbl {
 public:
  basic_observable_resource(string n, agent* a) : 
    basic_settable_observable_dbl(n,a)
    { set_osc(OSC_RESOURCE); };
  basic_observable_resource(string n, agent* a,double start_amnt) : 
    basic_settable_observable_dbl(n,a)
    { set_osc(OSC_RESOURCE); attempt_set(start_amnt); };
  
};

class abstract_environmental_sensor : public basic_observable {
 public:
  abstract_environmental_sensor(string n, agent* a) : 
    basic_observable(n,a) {};

  virtual bool attempt_set(int val) { return false; };
  virtual bool attempt_set(double val) { return false; };
  virtual bool attempt_dec(double val) { return false; };
  virtual bool attempt_inc(double val) { return false; };
  // can't set!! an abstract class !!
  // meant to be full of overrides !!

};

class declaration_based_observable : public basic_observable {
 public:
  declaration_based_observable(string name, agent* a, const char* declaration);
  
  virtual double double_value() { return (double)int_value(); };
  virtual int    int_value()    { 
    return umbc::declarations::get_declaration_count(the_dec); 
  };

  virtual bool   attempt_set(double v);
  virtual bool   attempt_set(int v)  { return attempt_set((double)v); };
  virtual bool   attempt_dec(double val);
  virtual bool   attempt_inc(double val);

 protected:
  string the_dec;
  
};

 class controlling_layer_observable : public basic_observable {
 public:
  controlling_layer_observable(string name, agent* a);
  virtual double double_value() { return (double) int_value(); };
  virtual int    int_value();

  virtual bool attempt_set(int val) { return false; };
  virtual bool attempt_set(double val) { return false; };
  virtual bool attempt_dec(double val) { return false; };
  virtual bool attempt_inc(double val) { return false; };

  virtual string class_name() { return "controlling_layer_obs"; };

};

};

#endif
