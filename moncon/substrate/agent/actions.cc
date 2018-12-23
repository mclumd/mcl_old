#include "substrate/actions.h"
#include "umbc/declarations.h"

#include <stdio.h>

using namespace MonCon;
using namespace umbc;

// BASE CLASS

action::action(string name,sensorimotor_agent* a,int numargs) : 
  simEntity(name),knows_agent(a),simple_has_args(numargs) {};

// SIMPLE ACTIONS (almost instantiable)

simple_action::simple_action(string name, sensorimotor_agent* a) :
  action(name,a,0),smagent(a) {};

simple_action::simple_action(string name, sensorimotor_agent* a,int numargs) :
  action(name,a,numargs),smagent(a) {};

string simple_action::grammar_argspec() {
  char buff[128];
  string rv="";
  for (int i=0;i<get_argcount();i++) {
    sprintf(buff,"?arg%d",i);
    // printf("(?arg%d)",i);
    rv+=(string)buff;
    if (i+1<get_argcount())
      rv+=" ";
  }
  return rv;
}

bool simple_action::publish_grammar(umbc::command_grammar& cg) {
  string func_string=get_name()+"(";
  // now get the arguments...
  func_string+=grammar_argspec();
  func_string+=")";
  cg.add_production(grammar_commname(),func_string);
  return true;
}

// REPAIR ACTIONS

bool simple_repair_action::simulate() {
  declarations::declare(personalize("count"));
  return simple_action::simulate();
}

// ACTION WRAPPERS

bool action_wrappers::intermittent_layer::simulate() {
  double fail_check = agent_of()->domain_of()->dbl_rand();
  if (fail_check >= current_rate)
    return peel()->simulate();
  else return false;
  // else make a declaration?
}

string action_wrappers::intermittent_layer::describe() { 
  char quee[256];
  sprintf(quee,"({W}%s[%.2lf] %s)",class_name().c_str(),current_rate,
	  peel()->describe().c_str()); 
  return quee;
}

string action_wrappers::breakable_layer::describe() { 
  char quee[256];
  string bd;
  if (is_broken()) bd = "!"; else bd = "";
  sprintf(quee,"({W}%s[%.2lf%s] %s)",class_name().c_str(),break_rate,bd.c_str(),
	  peel()->describe().c_str()); 
  return quee;
}

// ACTION WRAPPERS / UTILS

action* action_wrappers::action_wrapper::get_core_action() {
  action_wrapper* tbo = dynamic_cast<action_wrapper*>(peel());
  if (tbo == NULL)
    return peel();
  else return tbo->get_core_action();
}

action* action_wrappers::action_core(action* a) {
  action_wrapper* tbo = dynamic_cast<action_wrapper*>(a);
  if (tbo)
    return tbo->get_core_action();
  else return a;
}

/*
action_wrappers::breakable_layer* 
action_wrappers::action_wrapper::get_breakable_layer() {
  action_wrapper* tbo = dynamic_cast<action_wrapper*>(peel());
  if (tbo == NULL)
    return NULL;
  else return tbo->get_breakable_layer();
}

action_wrappers::intermittent_layer* 
action_wrappers::action_wrapper::get_intermittent_layer() {
  action_wrapper* tbo = dynamic_cast<action_wrapper*>(peel());
  if (tbo == NULL)
    return NULL;
  else return tbo->get_intermittent_layer();
}
*/
