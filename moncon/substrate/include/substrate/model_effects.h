#ifndef _MODEL_EFFECTS_H
#define _MODEL_EFFECTS_H

// #include "models.h"

#include <string>
using namespace std;

namespace MonCon {
  class action;

  class action_effect {
  public:
    action_effect() {};
    virtual ~action_effect() {};
    virtual action_effect* clone()=0;
    virtual string class_name()=0;
    
    // virtual bool gen_expectation(string key,egkType et,sensing* d)=0;
    
  };

  class stay_effect : public action_effect {
  public:
  stay_effect(string s) : 
    action_effect(),sensor(s) {};

    virtual action_effect* clone() { return new stay_effect(*this); };
    virtual string sensor_of() { return sensor; };

    static const char* class_id;
    virtual string class_name() { return class_id; };
    
  protected:
    string sensor;
    
  };
  
  class change_effect : public action_effect {
  public:
  change_effect(string s) : 
    action_effect(),sensor(s),up(false),unspec(true)
      {};
  change_effect(string s,bool goup) : 
    action_effect(),sensor(s),up(goup),unspec(false)
      {};
    // virtual bool gen_expectation(string key,egkType et,sensing* d);

    virtual action_effect* clone() { return new change_effect(*this); };
    virtual string sensor_of() { return sensor; };
    virtual bool   change_unspec() { return unspec; };
    virtual bool   change_up() { return up; };

    static const char* class_id;
    virtual string class_name() { return class_id; };
    
  protected:
    string sensor;
    bool   up;
    bool   unspec;
    
  };
  
  class simple_effect : public action_effect {
  public:
  simple_effect(string s,bool rel,double mag) : 
    action_effect(),sensor(s),relative(rel),amt(mag) {};
    virtual ~simple_effect() {};
    virtual action_effect* clone() { return new simple_effect(*this); };

    virtual string sensor_of()   { return sensor; };
    virtual bool   is_relative() { return relative; };
    virtual double amount_of()    { return amt; };

    static const char* class_id;
    virtual string class_name() { return class_id; };
    
  protected:
    string sensor;
    bool relative;
    double amt;
    
  };

  // sensor will take on a target value specified in arg[argnum]
  class argtarg_effect : public action_effect {
  public:
  argtarg_effect(string s,bool rel,int anum) : 
    action_effect(),sensor(s),relative(rel),argnum(anum) {};
    virtual ~argtarg_effect() {};
    virtual action_effect* clone() { return new argtarg_effect(*this); };

    virtual string sensor_of() { return sensor; };
    virtual int    arg_of()    { return argnum; };
    virtual bool   is_relative() { return relative; };

    static const char* class_id;
    virtual string class_name() { return class_id; };
    
  protected:
    string sensor;
    bool relative;
    int argnum;
  };
  
  class cost_lookup_effect : public action_effect {
  public:
    cost_lookup_effect(string s,int* ctable,int ctable_size,action* a) {};
  };

};

#endif
