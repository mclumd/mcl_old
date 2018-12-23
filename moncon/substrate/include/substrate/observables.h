#ifndef SENSOR_H
#define SENSOR_H

#include "substrate/simEntity.h"
#include "substrate/agent.h"

#include <stdlib.h>

namespace MonCon {

  // let's be honest, these are for MCL 
  enum observable_data_type { ODT_INTEGER, ODT_FLOAT, ODT_BOOL, ODT_SYMBOL,
			      ODT_UNSPEC};
  enum observable_sensor_class { OSC_RESOURCE, OSC_DOMAIN_SENSOR, OSC_STATE,
				 OSC_MESSAGE, OSC_TEMPORAL, OSC_UNSPEC };

class observable : public simEntity, public knows_agent {
 public:
  observable(string n, agent* a) : simEntity(n),knows_agent(a) {};
  virtual ~observable() {};

  virtual double      double_value()=0;
  virtual int         int_value()=0;
  virtual bool        attempt_set(int value)    =0;
  virtual bool        attempt_set(double value) =0;
  virtual bool        attempt_dec(double value) =0;
  virtual bool        attempt_inc(double value) =0;

  virtual observable_data_type    get_odt()=0;
  virtual observable_sensor_class get_osc()=0;
  virtual void  set_odt(observable_data_type odt)    =0;
  virtual void  set_osc(observable_sensor_class osc) =0;

  // should be overriden by wrapper observables (see below)
  virtual observable* core_observable() { return this; };
  virtual observable* peel() { return this; };

  virtual string describe();
  
};

class observable_wrapper : public observable {
 public:
  observable_wrapper(observable& base) : 
    observable(base.get_name(),base.agent_of()),
    the_observable(base) { };
  virtual ~observable_wrapper() {};

  virtual bool   attempt_set(double val) 
    { return peel()->attempt_set(val); }
  virtual bool   attempt_set(int val) 
    { return peel()->attempt_set(val); }
  virtual bool   attempt_dec(double val) 
    { return peel()->attempt_dec(val); }
  virtual bool   attempt_inc(double val) 
    { return peel()->attempt_inc(val); }

  virtual observable_data_type    get_odt()
  { return peel()->get_odt(); };
  virtual observable_sensor_class get_osc()
  { return peel()->get_osc(); };
  virtual void  set_odt(observable_data_type odt)
  { peel()->set_odt(odt); };
  virtual void  set_osc(observable_sensor_class osc) 
  { peel()->set_osc(osc); };

  virtual observable* peel() { return &the_observable; };
  virtual observable* core_observable() { 
    return the_observable.core_observable(); 
  };

 protected:
  observable& the_observable;

};

class breakable : public observable_wrapper {
 public:
  breakable(observable& base,double bv,double br) :
    observable_wrapper(base),break_value(bv),break_rate(br),
    broken(false),forbid_breakage(false) {};
  breakable(observable& base) :
    observable_wrapper(base),break_value(0.0),break_rate(base_break_rate),
    broken(false),forbid_breakage(false) {};

  virtual ~breakable() {};

  virtual bool   is_broken() { return broken; };
  virtual double broken_value() { return break_value; };
  virtual double breakage_rate() { return break_rate; };
  virtual void   break_it() { 
    if (!forbid_breakage) { cout << "!BREAK!" << endl; broken=true; }
  };
  virtual void   fix_it() { broken=false; };
  virtual void   set_break_value(double nbv) { break_value = nbv; };
  // why you would want these is unknown
  virtual void   disallow_breakage() { forbid_breakage=true; };
  virtual void   allow_breakage() { forbid_breakage=false; };
  
  static double base_break_rate;

  virtual double double_value();
  virtual int    int_value();

  virtual string describe();
  virtual string class_name() { return "breakable"; };

 protected:
  double break_value,break_rate;
  bool broken,forbid_breakage;

};

class noisy : public observable_wrapper {
 public: 
  noisy(observable& base) : observable_wrapper(base) {};
  virtual ~noisy() {};

  virtual double noisify(double input)=0;

  double double_value() { return noisify(peel()->double_value()); };
  int    int_value()    { return (int)double_value(); };

  virtual string describe();

};

class uniformly_noisy : public noisy {
 public:
  uniformly_noisy(observable& base) : noisy(base),noise_rate(0.0) {};
  uniformly_noisy(observable& base,double factor) : 
    noisy(base),noise_rate(factor) {};
  double noisify(double input) { 
    double rand_rate = ((double)rand()/(double)RAND_MAX);
    return input + ((noise_rate * rand_rate) - noise_rate/2.0);
  }
  virtual string class_name() { return "uniform_noisy"; };

 protected:
  double noise_rate;
};

namespace observable_utils {
  breakable*                get_breakable(observable* o);
  noisy*                    get_noisy(observable* o);

  bool truly_eq (observable* o, double value);
  bool truly_lt (observable* o, double value);
  bool truly_lte(observable* o, double value);
  bool truly_gt (observable* o, double value);
  bool truly_gte(observable* o, double value);
  
};

};

#endif
