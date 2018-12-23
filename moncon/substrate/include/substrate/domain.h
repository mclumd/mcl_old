#ifndef MC_SUBSTRATE_DOMAIN_H
#define MC_SUBSTRATE_DOMAIN_H

#include "substrate/simEntity.h"
#include "substrate/timeline_aware.h"
#include <stdlib.h>
#include <vector>
using namespace std;

namespace MonCon {

  class agent;

typedef vector<agent*> agent_list;

class domain : public simEntity, public timeline_aware {
 public:
  domain(string name);
  domain(string name, timeline* tl);
  
  virtual ~domain() {};
  
  virtual bool simulate()=0;
  virtual bool configure(string conf)=0;
  
  virtual agent* get_agent(string n)=0;

  virtual agent_list list_agents()=0;
  virtual agent_list::iterator al_start()=0;
  virtual agent_list::iterator al_end()=0;

  virtual int    num_agnts()=0;
  virtual agent* get_agent(int n)=0;
  virtual void   rpl_agent(int n,agent* k)=0;

  virtual void add_agent(agent* a)=0;

  // was a PIA and who terminates agents?
  // virtual void terminate_agent(agent* a)=0;

  virtual unsigned int tick()=0;

  virtual double dbl_rand();

 protected:
  struct drand48_data domain_seed;
    
};

class knows_domain {
 public:
  knows_domain(domain* d) :dom(d) {};
  domain* domain_of() { return dom; };
 private:
  domain* dom;
};

};
#endif
