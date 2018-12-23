#include "umbc/exceptions.h"
#include "umbc/logger.h"
#include "umbc/token_machine.h"
#include "agent.h"
#include "controller.h"

using namespace raccoon;
using namespace raccoon::controllers;
using namespace std;


#include <stdio.h>
#include <sys/types.h> 
#include <netinet/in.h>
#include <unistd.h>
#include <exception>
#include <iostream>

void *_ba_start_server(void *argptr);

agents::agent_basic::~agent_basic() {
  while (!available_controllers.empty()) {
    controller* fc = available_controllers.front();
    available_controllers.pop_front();
    delete fc;
  }
}

bool agents::agent_basic::initialize() {
  cout << "initializing " << get_name() << endl;
  return monitor_thread_detach();
}

bool agents::agent_basic::monitor() {
  // cout << " monitoring " << describe() << endl;
  bool tv=true;
  // monitor loop...
  list<controller*>::iterator aci = active_controllers.begin();
  while (aci != active_controllers.end()) {
    cout << " monitor: controller @ " << hex << (*aci) << endl;
    tv &= (*aci)->monitor();
    if ((*aci)->self_deactivated()) {
      (*aci)->stop();
      aci=active_controllers.erase(aci);
    }
    else
      aci++;
  }
  return tv;
}

bool agents::agent_basic::shutdown() {
  // should shutdown all active controllers
  for (list<controller*>::iterator aci = active_controllers.begin();
       aci != active_controllers.end();
       aci++) {
    (*aci)->shutdown();
  }
  active_controllers.clear();
  return true;
}

string agents::agent_basic::query(string command) {
  umbc::tokenMachine tm(command);
  string quer = tm.nextToken();
  if (quer == "ack") {
    return okay_msg("ack");
  }
  else if (quer == "controllers") {
    if (available_controllers.empty())
      return okay_msg("");
    else {
      string rv=" ";
      for (list<controller*>::iterator ali = available_controllers.begin();
	   ali != available_controllers.end();
	   ali++) {
	rv+=(*ali)->get_name()+" ";
      }
      return okay_msg(rv);
    }
  }
  else if (quer == "busy") {
    if (active_controllers.empty())
      return okay_msg("idle");
    else {
      string rv=" ";
      for (list<controller*>::iterator ali = active_controllers.begin();
	   ali != active_controllers.end();
	   ali++) {
	rv+=(*ali)->get_name()+" ";
      }
      return okay_msg(rv);
    }
  }
  else return ignore_msg("unimplemented query");
}

string agents::agent_basic::command(string command) {
  umbc::tokenMachine tm(command);
  string directive = tm.nextToken();
  if ((directive == "start") || (directive == "stop")) {
    string acspec = tm.nextToken();
    string contr = umbc::textFunctions::getFunctor(acspec);
    string args  = umbc::textFunctions::getParameters(acspec);
    controller* ac_c = get_controller(contr);
    if (!ac_c)
      return fail_msg("unknown controller \""+contr+"\"");
    else {
      if (directive == "start") {
	assumed_dbl_arguments argpak;
	argpak.set_arg_paramstring(args);
	cout << " agent_command args = " << argpak.describe() << endl;	
	start_controller(ac_c,argpak);
	return okay_msg(contr+" started");
      }
      else
	stop_controller(ac_c);
	return okay_msg(contr+" stopped");
    }
  }
  else return ignore_msg("unhandled: agent_basic");
}

controllers::controller* agents::agent_basic::get_controller(string nm) {
  cout << "getting controller " <<nm << endl;
  cout << available_controllers.size() << " controllers available" << endl;
  for (list<controller*>::iterator ali = available_controllers.begin();
       ali != available_controllers.end();
       ali++) {
    cout << "compare: c<" << (*ali)->get_name() << "> t<" << nm << ">" 
	 << endl;
    if ((*ali)->get_name() == nm)
      return *ali;
  }
  return NULL;
}

bool agents::agent_basic::start_controller(controller* c) { 
  c->start(); 
  active_controllers.push_back(c); 
  return true; 
};

bool agents::agent_basic::start_controller(controller* c,arguments& cargo) { 
  c->start(cargo); 
  active_controllers.push_back(c); 
  return true; 
};

bool agents::agent_basic::stop_controller(controller* c) { 
  cout << " stop_controller called @ " << hex << c << endl;
  active_controllers.remove(c);
  c->stop(); 
  return true; 
};

bool agents::agent_basic::monitor_thread_detach() {
  int pthread_res = pthread_create(&server_thread,NULL,_ba_start_server,this);
  if (pthread_res < 0)
    umbc::exceptions::signal_exception("failed to create pthread for "+get_name()+" agent (exiting)");
  pthread_detach(server_thread);
  return true;
}

void *_ba_start_server(void * argptr) {
  agents::agent_basic* ba = 
    static_cast<agents::agent_basic*>(argptr);
  umbc::uLog::annotate(umbc::UMBCLOG_MSG,"starting thread for "+ba->get_name());
  ba->set_running(true);
  while (ba->is_alive()) {
    usleep(100000); 
    // check the return value?
    ba->monitor();
  }
  ba->set_running(false);
  pthread_exit((void*)0);
}

bool agents::agent_basic::publish_grammar(umbc::command_grammar& cg) {
  cg.add_production(grammar_commname()+"_query","ack");
  cg.add_production(grammar_commname()+"_query","controllers");
  cg.add_production(grammar_commname()+"_query","busy");
  cg.add_production(grammar_commname()+"_command",
		    "start "+grammar_commname()+"_controllerspec");
  cg.add_production(grammar_commname()+"_command",
		    "stop "+grammar_commname()+"_controllerspec");
  for (list<controller*>::const_iterator aci = available_controllers.begin();
       aci != available_controllers.end();
       aci++) {
    cg.add_production(grammar_commname()+"_controllerspec",
		      (*aci)->grammar_commname());
  }  
  return true;
}

string agents::agent_basic::describe() {
  string rv = "<" + get_name() + "(active: ";
  for (list<controller*>::const_iterator aci = active_controllers.begin();
       aci != active_controllers.end();
       aci++) {
    if ((*aci) == NULL)
      rv+= "NULL ";
    else
      rv+= (*aci)->get_name()+" ";
  }
  rv+=")>";
  return rv;
}
