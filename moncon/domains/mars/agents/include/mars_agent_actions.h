#ifndef MARS_AGENT_ACT_H
#define MARS_AGENT_ACT_H

#include "substrate/actions.h"
// #include "substrate/mcl_model.h"
#include "substrate/basic_agent.h"

using namespace MonCon;

#define MA_NO_ARG -1

namespace mcMars {

class mars_domain_action : public simple_action {
 public:
  mars_domain_action(string nm,int key,basic_agent* a) : 
    simple_action(nm,a,0),ac(false),code(key),nrg_cost(CALLOUT_COST) {};

  mars_domain_action(string nm,int key,basic_agent* a,int argc) : 
    simple_action(nm,a,argc),ac(false),code(key),nrg_cost(CALLOUT_COST) {
    for (int q=0;q<argc;q++)
      set_arg(MA_NO_ARG);
  };

  virtual bool active() { return ac; };
  virtual bool activate() { ac=true; return true; };
  virtual bool deactivate() { ac=false; return true; };
  
  int get_code() { return code; };

  virtual bool simulate();
  
  /// ### MCL-EXTRACTION
  // virtual MonCon::mcl_action_model* cheat_get_model()=0;

  virtual void setcost(int cost) { nrg_cost=cost; };
  virtual int cost() { return nrg_cost; };

  static const int CALLOUT_COST = 998998;

  // it's okay to do this, look at the constructor
  basic_agent* basic_agent_of() { return (basic_agent*)agent_of(); };

 protected:
  bool         ac;
  int          code;
  int          nrg_cost;

};

// MCL REPAIR-ISH STUFF

class mars_repair_action : public simple_repair_action {
 public:
  mars_repair_action(string name,int code,basic_agent* a) : 
    simple_repair_action(name,a,0),referenceCode(NULL), rpr_code(code)
    { };
  mars_repair_action(string name,int code,basic_agent* a,int numargs) : 
    simple_repair_action(name,a,numargs),referenceCode(NULL), rpr_code(code)
    { };
  virtual bool simulate();
  void setRefCode(resRefType rc) { referenceCode = rc; };

  virtual bool active() { return ac; };
  virtual bool activate() { ac=true; return true; };
  virtual bool deactivate() { ac=false; return true; };
  
 protected:
  resRefType referenceCode;
  bool       ac;
  int        rpr_code;
  
};

};
#endif
