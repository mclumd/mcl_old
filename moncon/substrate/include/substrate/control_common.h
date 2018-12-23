#ifndef CONTROL_COMMON_H
#define CONTROL_COMMON_H

#include "substrate/actionspec.h"
#include <list>

using namespace std;

namespace MonCon {

class acting;

class has_action_q {
 public:
  has_action_q() : current_aspec(NULL) {};
  virtual ~has_action_q();

  bool        has_pending_tla() { return !actionq.empty(); };
  actionspec* tlaspec() { return actionq.front(); };
  actionspec* curr_aspec() { return current_aspec; };
  void        advance_q();
  void        clear_current();
  // void        crush_tlaspec();
  void        add_aspec_front(actionspec* a) { actionq.push_front(a); };
  void        add_aspec_back (actionspec* a) { actionq.push_back(a); };
  void        clear_action_q();
  void        dump_action_q();

  bool        accept_action_string(string as, acting* a);
  static      actionspec* fp_string2aspec(string fp_aspec, acting* a);
  
 protected:
  list<actionspec*> actionq;
  actionspec*       current_aspec;

};

};

#endif
