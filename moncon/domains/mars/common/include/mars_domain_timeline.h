#ifndef MARS_DOMAIN_TIMELINE_H
#define MARS_DOMAIN_TIMELINE_H

#include "substrate/timeline.h"
#include "mars_domain.h"

using namespace MonCon;

namespace mcMars {

  class mars_timeline_entry : public timeline_entry {
  public:
  mars_timeline_entry(int firetime, timeline* tl) :
    timeline_entry(firetime,tl) {};
    
  protected:
    mars_domain* mars_domain_of() { 
      return (mars_domain*)(get_timeline()->domain_of()); 
    };
    
  };

  class mars_timeline_anomaly : public timeline_anomaly {
  public:
  mars_timeline_anomaly(int firetime, timeline* tl) :
    timeline_anomaly(firetime,tl) {};
    
  protected:
    mars_domain* mars_domain_of() { 
      return (mars_domain*)(get_timeline()->domain_of()); 
    };
    
  };
  
  class mars_timeline_event : public mars_timeline_anomaly {
  public:
    
  mars_timeline_event(int firetime,timeline* tl,int event,int earg=0) 
    : mars_timeline_anomaly(firetime,tl),evCode(event),evArg(earg),evSeed(0) {};
    
    virtual void fire();
    virtual string describe();
    virtual string class_name() { return "mars_timeline_event"; };
    virtual void set_seed(long seed) { evSeed=seed; };
    // virtual void dump();
    
  protected:
    int evCode,evArg;
    long evSeed;
    // int evArg[MAXEVARGS];
    
  };
  
};

#endif
