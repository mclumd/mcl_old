#ifndef MCL_EXPECTATIONS_H
#define MCL_EXPECTATIONS_H

#include "mclTimer.h"
#include "mclEntity.h"
#include "mclProperties.h"
#include "noiseProfiles.h"

#include <list>

namespace metacog {

class mcl;

/** \file 
 * \brief contains the expectation class hierarchy for MCL.
 */

//! base class for MCL expectations.
class mclExp : public mclEntity {
 public:

  mclExp(string cn, mcl *m) : mclEntity(cn),itsMCL(m) {};

  //! virtual function, test for violation.
  virtual bool violation()=0;
  //! virtual function, true if the expectation should be checked before
  //! it is removed (action effect).
  virtual bool checkOnExit()=0;
  //! virtual function, true if the expectation should be checked always
  //! (maintenance expectation).
  virtual bool checkAlways()=0;

  virtual mcl *MCL() { return itsMCL; };

  virtual bool tolerate(double p_of_satisfactory_condition) {
    return (p_of_satisfactory_condition > .90);
  }

  //! generates link tags for the expectation.
  //! link tags are strings that refer to indication fringe nodes and
  //! define evidence for auto-activating those nodes when the expectation
  //! is violated.
  virtual list<string> generateLinkTags()=0;

 protected:
  mcl *itsMCL;

};

//! effect expectation class (check on exit but at no other time).
class mclEffectExp : public mclExp {
 public:
  mclEffectExp(string cn, mcl *m) : mclExp(cn,m) {};
  virtual bool checkOnExit() { return true; };
  virtual bool checkAlways() { return false; };
};

//! maintenance expectation class (checked on monitor() calls but not on exit).
//! @see monitor()
class mclMaintenanceExp : public mclExp {
 public:
  mclMaintenanceExp(string cn, mcl *m) : mclExp(cn,m) {};
  virtual bool checkOnExit() { return false; };
  virtual bool checkAlways() { return true; };
};

//! class of expectations that are allowed a delay before monitoring begins.
class mclDelayedMaintenanceExp : public mclMaintenanceExp {
 public:
  mclDelayedMaintenanceExp(mcl *m, double delay, mclExp *subExp);

  virtual string baseClassName() { return "DelayedMaintenanceExp"; };
  virtual bool violation();
  virtual list<string> generateLinkTags();  

protected:
  //! protected timer object used to activate the expectation after delay.
  mclTimer internalTimer;
  //! delay, in seconds and microseconds.
  int secs,usecs;
  //! a sub-expectation, which is activated after the delay period.
  mclExp *sub;

};

//! class of realtime expectations.
//! violation() returns true if the internal timer expires before the 
//! expectation is revoked.
class mclRealTimeExp : public mclMaintenanceExp {
 public:
  mclRealTimeExp(mcl *m, int seconds,int useconds) :
    mclMaintenanceExp("RealTimeExpectation",m),
    secs(seconds),usecs(useconds) 
    { internalTimer.restart(secs,usecs); };

  virtual string baseClassName() { return "RealTimeExpectation"; };
  virtual bool violation();
  virtual list<string> generateLinkTags();

 private:
  mclTimer internalTimer;
  int secs,usecs;

};

//! class of tick-based timer expectations.
//! as realtime expectation, but tick-based. each monitor() call represents
//! a single tick.
//! violation() returns true if monitor() is called more times than the 
//! expectation allows.
class mclTickExp : public mclMaintenanceExp {
 public:
  mclTickExp(mcl *m, int maxTicks); 
  virtual string baseClassName() { return "TickExpectation"; };
  virtual bool violation();
  virtual list<string> generateLinkTags();

 private:
  int tickMax,ticks,lastTick;

};

//! abstract class of generic sensor polling expectations.
class mclPollAndTestExp : public mclMaintenanceExp {
 public:
  virtual list<string> generateLinkTags();
  virtual void dumpEntity(ostream *strm) {
    *strm << entityName() << "(" << sname << ")";
  }
  
 protected:
  mclPollAndTestExp(string sensor,string name,mcl *m) :
    mclMaintenanceExp(name,m),sname(sensor) {};
  string sname;
  
};

//! poll-and-test expectation class that requires a sensor value to stay
//! above a specified lower bound.
class mclLowerBoundExp : public mclPollAndTestExp {
 public:
  mclLowerBoundExp(string sensor,mcl *m,float lowerBound) : 
    mclPollAndTestExp(sensor,"lowerBound",m),bound(lowerBound)
    {};
  virtual string baseClassName() { return "floorExpectation"; };
  virtual list<string> generateLinkTags();

  virtual bool violation();

 private:
  float bound;

};

//! poll-and-test expectation class that requires a sensor value to stay
//! below a specified upper bound.
class mclUpperBoundExp : public mclPollAndTestExp {
 public:
  mclUpperBoundExp(string sensor,mcl *m,float upperBound) : 
    mclPollAndTestExp(sensor,"UpperBound",m), bound(upperBound)
    {};
  virtual string baseClassName() { return "CeilingExpectation"; };
  virtual list<string> generateLinkTags();

  bool violation();

 private:
  float bound;

};

//! poll-and-test expectation class that requires a sensor value to remain
//! unchanged throughout the lifetime of the expectation.
class mclMaintainValueExp : public mclPollAndTestExp {
 public:
  //! constructor, maintain-value is assumed to be current value.
  mclMaintainValueExp(string sensor,mcl *m);
  //! constructor, allows the maintain-value to be specified.
  mclMaintainValueExp(string sensor,mcl *m,float targ);

  virtual string baseClassName() { return "MaintainValueExpectation"; };
  virtual list<string> generateLinkTags();

  bool violation();

 private:
  float target, initial;
  bool  nochange;

};

//! abstract class of expectations in which a sensor value is polled when 
//! the expectation is expired (action effect).
class mclPollOnExitExp : public mclEffectExp {
 public:
  virtual list<string> generateLinkTags();
  virtual string describe();

 protected:
  mclPollOnExitExp(string sensor,string name,mcl *m);
  string sname;
  float  initial;

};

//! class of expectations in which the sensor value must go up from its
//! original value by the time the expectation expires.
class mclGoUpExp : public mclPollOnExitExp {
 public:
  mclGoUpExp(string sensor,mcl *m);
  virtual bool violation();
  virtual string baseClassName() { return "GoUp"; };
  virtual list<string> generateLinkTags();
 private:
};

//! class of expectations in which the sensor value must go down from its
//! original value by the time the expectation expires.
class mclGoDownExp : public mclPollOnExitExp {
 public:
  mclGoDownExp(string sensor,mcl *m);
  virtual bool violation();
  virtual string baseClassName() { return "GoDown"; };
  virtual list<string> generateLinkTags();
 private:
};

//! class of expectations in which the sensor value must change from its
//! original value by the time the expectation expires.
class mclAnyNetChangeExp : public mclPollOnExitExp {
 public:
  mclAnyNetChangeExp(string sensor,mcl *m);
  virtual bool violation();
  virtual string baseClassName() { return "NetChangeExpectation"; };
  virtual list<string> generateLinkTags();

 private:

};

//! class of expectations in which the sensor value must not have changed 
//! from its original value by the time the expectation expires.
class mclNoNetChangeExp : public mclPollOnExitExp {
 public:
  mclNoNetChangeExp(string sensor,mcl *m);
  virtual bool violation();
  virtual string baseClassName() { return "NetZeroExpectation"; };
  virtual list<string> generateLinkTags();
 private:
};

//! class of expectations in which the sensor value must not have taken 
//! a specified value by the time the expectation expires.
class mclTakeValueExp : public mclPollOnExitExp {
 public:
  mclTakeValueExp(string sensor,mcl *m,float targ);
  virtual bool violation();
  virtual string baseClassName() { return "TakeValueEffect"; };
  virtual list<string> generateLinkTags();
  virtual string describe();

 private:
  float target;
};

//! class of expectations in which the sensor value changes discretely after
//! a delay.
class mclPhaseChangeExp : public mclDelayedMaintenanceExp {
 public:
  mclPhaseChangeExp(mcl *m, double delay, string sensor, float target);

  virtual string baseClassName() { return "phaseChange"; };
  // use the defaults
  // virtual bool violation();
  // virtual list<string> generateLinkTags();

};

//! a list of expectations.
typedef list<mclExp *> expList;

//! class of expectation groups.
//! an organizational structor for associating expectations.
class mclExpGroup : public mclEntity {
 public:

  //! the only usable constructor for expectation groups.
  //! @param ekey the group key -- key usage managed by the host.
  //! @param pkey the parent group key
  //! @param ref  a reference to a previous mclFrame
  mclExpGroup(mcl *m,egkType ekey,egkType pkey,resRefType ref);

  virtual string baseClassName() { return "expectationGroup"; }; 
  virtual mcl *MCL() { return myMCL; };

  //! adds an expectation to be monitored as part of this group.
  void addExp(mclExp *me) { 
    cout << "MCL: expectation added ~> " << me->entityName() << endl;
    exps.push_back(me); 
  };
  
  expList::iterator expListHead() { return exps.begin(); };
  expList::iterator expListButt() { return exps.end(); };

  egkType get_egKey() { return this_key; };
  egkType get_parent_egKey() { return parent_key; };
  resRefType get_referent() { return this_ref; };

  mclPropertyVector* getPV() { return &myPV; };

  //! marks the expectation for deletion during the next monitor call.
  void markDelete() { deleteMe=true; };
  //! unmarks the expectation for deletion during the next monitor call.
  void unMarkDelete() { deleteMe=false; };
  //! returns true if the expectation is marked for deletion.
  bool markedForDelete() { return deleteMe; };

  //! sets the MCL tick during which the last violation in this group 
  //! occured.
  void setLastViolationTick(int tck) { lastViolationTick=tck; };
  //! returns the MCL tick during which the last violation in this group 
  //! occured.
  int  getLastViolationTick() { return lastViolationTick; };

 private:
  //! all expectations associated with this group.
  expList        exps;
  mcl*           myMCL;
  egkType        this_key,parent_key;
  resRefType     this_ref;
  bool           deleteMe;
  int            lastViolationTick;
  mclPropertyVector myPV;
  
};

//! class providing static expectation building functions.
//! for convenience.
class expectationFactory {
 public:
   static mclExp* makeRealtimeExp(mcl *m,float seconds);
   static mclExp* makeTickExp(mcl *m,float ticks);
   static mclExp* makeNZExp(mcl *m,string sname);
   static mclExp* makeNetChangeExp(mcl *m,string sname);
   static mclExp* makeGoUpExp(mcl *m,string sname);
   static mclExp* makeGoDownExp(mcl *m,string sname);
   static mclExp* makeTakeValExp(mcl *m,string sname,float val);
   static mclExp* makeUBExp(mcl *m,string sname,float val);
   static mclExp* makeLBExp(mcl *m,string sname,float val);
   static mclExp* makeMVExp(mcl *m,string sname);
   static mclExp* makeMVExp(mcl *m,string sname,float val);
   static mclExp* makeDelayedExp(mcl *m,double seconds,mclExp *sub);

 private:
  expectationFactory() {};
};

};
#endif
