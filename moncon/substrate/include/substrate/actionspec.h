#ifndef MC_SUBSTRATE_ASPEC_CONTROL_H
#define MC_SUBSTRATE_ASPEC_CONTROL_H

#include "substrate/simEntity.h"

#define ACTIONSPEC_MAXARGS 1

namespace MonCon {

class action;

class actionspec  {
 public:
  actionspec(action* a);
  actionspec(action* a,int arg1);
  virtual ~actionspec() {};

  void    change_argcount(int argval) { argcount=argval; };
  void    change_arg(int argnum,int argval) { arg[argnum]=argval; };

  actionspec* clone() { return new actionspec((const actionspec&)*this); };

  void    prepare_for_activation();
  action* underlying_action() { return ac; };

  virtual string to_string();

 protected:
  action* ac;
  // this is so ghetto
  int     arg[ACTIONSPEC_MAXARGS],
          argcount;
  
};

};

#endif
