#ifndef MC_SUBSTRATE_BDOMAIN_H
#define MC_SUBSTRATE_BDOMAIN_H

#include "substrate/domain.h"
#include "substrate/timeline.h"

#include <list>
#include <map>
using namespace std;

// class controller;

namespace MonCon {

typedef unsigned int apkType;
typedef double       apvType;

class basic_domain : public domain, public understands_timeline_entries {
 public:
  
  basic_domain(string name);
  virtual ~basic_domain() {};
  
  virtual bool simulate();
  virtual void add_agent(agent* a) { a_list.push_back(a); };
  virtual agent_list list_agents() { return a_list; };
  virtual agent_list::iterator al_start() { return a_list.begin(); };
  virtual agent_list::iterator al_end()   { return a_list.end(); };
  virtual agent* get_agent(string n);

  // no one will use this...
  // virtual void terminate_agent(agent* a);

  virtual int    num_agnts() { return a_list.size(); };
  virtual agent* get_agent(int n) { return a_list[n]; };
  virtual void   rpl_agent(int n,agent* k) { a_list[n]=k; };

  virtual unsigned int tick();
  
  virtual timeline_entry* parse_tle_spec( int firetime, timeline* tl,
					  string tles );

  virtual apvType request_agent_property(agent* a,apkType property_key)=0;
  virtual bool    attempt_set_agent_property(agent* a,apkType property_key, 
					     apvType val)=0;
  
 protected:
  agent_list a_list;

};

class agent_property_table {
 public:
  agent_property_table(agent* a) : agnt(a) {};
  void    set(apkType k,double v) { table[k]=v; };
  apvType get(apkType k) { return table[k]; };

 private:
  agent* agnt;
  map<apkType,apvType> table;
  
};

};

#endif
