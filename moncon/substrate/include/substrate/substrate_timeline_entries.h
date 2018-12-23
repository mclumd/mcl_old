#ifndef MCS_TIMELINE_ENTRIES_H
#define MCS_TIMELINE_ENTRIES_H

#include "agent.h"

namespace MonCon {

  class timeline_sensorbreak : public timeline_anomaly {
  public:
  timeline_sensorbreak(int firetime,timeline* tl,
		       sensorimotor_agent* agnt,int sindex) :
    timeline_anomaly(firetime,tl),a(agnt),sensor(sindex) {};
    virtual void fire();
    virtual string describe();
    virtual string class_name() { return "timeline_sensorbreak"; };
    // virtual void dump();
    
  protected:
    sensorimotor_agent* a;
    int sensor;
    
  };
  
  class timeline_effectorbreak : public timeline_anomaly {
  public:
    
  timeline_effectorbreak(int firetime,timeline* tl,
			 sensorimotor_agent* agnt,string aname) :
    timeline_anomaly(firetime,tl),a(agnt),action_name(aname) {};
    virtual void fire();
    virtual string describe();
    virtual string class_name() { return "timeline_effectorbreak"; };
    
  protected:
    sensorimotor_agent* a;
    string action_name;
    
  };

};
#endif
