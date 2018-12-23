#include "expectations.h"
#include "mcl.h"
#include "mclExceptions.h"
#include "mclLogger.h"
#include "mclSettings.h"

//////
///// group constructors moved here due to mcl references
//////

mclExpGroup::mclExpGroup(mcl *m) : mclEntity(),myMCL(m),
				   this_key(NULL),parent_key(NULL),
				   this_ref(NULL),
				   deleteMe(false),lastViolationTick(0) {
  signalMCLException("deprecated constructor mclExpGroup(mcl) used.");
  // copies from default MCL property vector
  if (m->getDefaultPV())
    m->getDefaultPV()->copyValues(myPV);
  else {
    mclLogger::annotate(MCLA_WARNING,"[mcl/exp]:: could not get DPV from MCL (using global defaults.");
    myPV.reSetProperties();
  }
}

mclExpGroup::mclExpGroup(mcl *m,egkType ekey,egkType pkey,resRefType ref) 
  : mclEntity(),myMCL(m),this_key(ekey),parent_key(pkey),this_ref(ref),
    deleteMe(false),lastViolationTick(0) {
  // make a copy of the best known PV
  if (pkey) {
    mclExpGroup* pgroup = m->getExpGroup(pkey);
    if (pgroup && pgroup->getPV())
      pgroup->getPV()->copyValues(myPV);
    else if (m->getDefaultPV())
      m->getDefaultPV()->copyValues(myPV);
  }
  else if (m->getDefaultPV())
    m->getDefaultPV()->copyValues(myPV);
}

//////
///// THIS IS THE BEGINNING OF A MILLION EXPECTATION CLASS DEFS
//////

//////
///// but first, there could be some mixin classes
//////



//////
///// MAINTENANCE EXPECTATIONS LISTED FIRST
//////
//////
///// tick-based timer
//////

mclTickExp::mclTickExp(mcl *m, int maxTicks) : 
  mclMaintenanceExp("TickExpectation",m),tickMax(maxTicks),
  ticks(0) {
  lastTick = m->tickNumber();
}

bool mclTickExp::violation() {
  if (itsMCL->tickNumber() != lastTick) {
    ticks++;
    if (ticks >= tickMax)
      return true;
  }
  return false;
}

//////
///// real-time timer
//////

bool mclRealTimeExp::violation() {
  return internalTimer.expired();
}

//////
///// maintainValue (hold value throughout lifetime of exp)
//////

mclMaintainValueExp::mclMaintainValueExp(string sensor,mcl *m) : 
  mclPollAndTestExp(sensor,"MaintainValue",m),nochange(true)
{ target=initial=itsMCL->sensorValue(sensor); }

mclMaintainValueExp::mclMaintainValueExp(string sensor,mcl *m,float targ) : 
  mclPollAndTestExp(sensor,"MaintainValue",m),target(targ),nochange(false) 
{ initial=itsMCL->sensorValue(sensor); }

bool mclMaintainValueExp::violation() {
  noiseProfile* np =noiseProfile::getNoiseProfileFor(itsMCL->getMAkey(),sname);
  if (np)
    return !tolerate(np->p_that_EQ(itsMCL->sensorValue(sname),target));
  else
    return (itsMCL->sensorValue(sname) != target);
}

//////
///// UpperBound (must never exceed upper threshold)
//////

bool mclUpperBoundExp::violation() {
  noiseProfile* np =noiseProfile::getNoiseProfileFor(itsMCL->getMAkey(),sname);
  if (np)
    return !tolerate(np->p_that_LT(itsMCL->sensorValue(sname),bound));
  else return (itsMCL->sensorValue(sname) > bound);
}

//////
///// LowerBound (must never go below lower threshold)
//////

bool mclLowerBoundExp::violation() {
  noiseProfile* np =noiseProfile::getNoiseProfileFor(itsMCL->getMAkey(),sname);
  if (np)
    return !tolerate(np->p_that_GT(itsMCL->sensorValue(sname),bound));
  else return (itsMCL->sensorValue(sname) < bound);
}

//////
///// EFFECT EXPECTATIONS LISTED SECOND
//////

mclPollOnExitExp::mclPollOnExitExp(string sensor,string name,mcl *m) :
  mclEffectExp(name,m),sname(sensor) {
  initial=itsMCL->sensorValue(sensor);
}

string mclPollOnExitExp::describe() {
  char x[255];
  sprintf(x,"<%s s=%s i=%.2f>",className().c_str(),sname.c_str(),initial);
  return (string)x;
}

//////
///// GoUp (must have gone up when exp group is dissolved)
//////

mclGoUpExp::mclGoUpExp(string sensor, mcl *m) :
  mclPollOnExitExp(sensor,"GoUp",m) { }

bool mclGoUpExp::violation() {
  noiseProfile* np=noiseProfile::getNoiseProfileFor(itsMCL->getMAkey(),sname);
  if (np)
    return !tolerate(np->p_that_GT(itsMCL->sensorValue(sname),initial));
  else return (itsMCL->sensorValue(sname) <= initial);
}

//////
///// GoDown (must have gone down when exp group is dissolved)
//////

mclGoDownExp::mclGoDownExp(string sensor, mcl *m) :
  mclPollOnExitExp(sensor,"GoDown",m) { }

bool mclGoDownExp::violation() {
  noiseProfile* np=noiseProfile::getNoiseProfileFor(itsMCL->getMAkey(),sname);
  if (np)
    return !tolerate(np->p_that_LT(itsMCL->sensorValue(sname),initial));
  else return (itsMCL->sensorValue(sname) >= initial);
}

//////
///// NoNetChange (must not have changed when exp group is dissolved)
//////

mclAnyNetChangeExp::mclAnyNetChangeExp(string sensor, mcl *m) :
  mclPollOnExitExp(sensor,"AnyChange",m) {
}

bool mclAnyNetChangeExp::violation() {
  noiseProfile* np=noiseProfile::getNoiseProfileFor(itsMCL->getMAkey(),sname);
  if (np)
    return !tolerate(np->p_that_EQ(itsMCL->sensorValue(sname),initial));
  else return (itsMCL->sensorValue(sname) == initial);
}

//////
///// NoNetChange (must not have changed when exp group is dissolved)
//////

mclNoNetChangeExp::mclNoNetChangeExp(string sensor, mcl *m) :
  mclPollOnExitExp(sensor,"NoChange",m) {
}

bool mclNoNetChangeExp::violation() {
  return (itsMCL->sensorValue(sname) != initial);
}

//////
///// TakeValue (must have taken assigned value when exp group is dissolved)
//////

mclTakeValueExp::mclTakeValueExp(string sensor, mcl *m,float tar) :
  mclPollOnExitExp(sensor,"TakeValue",m),target(tar) { }

bool mclTakeValueExp::violation() {
  noiseProfile* np=noiseProfile::getNoiseProfileFor(itsMCL->getMAkey(),sname);
  if (np)
    return !tolerate(np->p_that_EQ(itsMCL->sensorValue(sname),target));
  else return (itsMCL->sensorValue(sname) != target);
}

string mclTakeValueExp::describe() {
  char x[255];
  sprintf(x,"<%s s=%s t=%.2f>",className().c_str(),sname.c_str(),target);
  return (string)x;
}

//////
///// DelayedMaintenance (sub-expectation becomes active after delay)
//////

mclDelayedMaintenanceExp::mclDelayedMaintenanceExp
(mcl *m,double delay, mclExp *subExp) :
  mclMaintenanceExp("DelayedExpectation",m),secs((int)delay),
  usecs((int)((delay-(int)delay)*1000000)),
  sub(subExp) {
  internalTimer.restart(secs,usecs);
}

bool mclDelayedMaintenanceExp::violation() {
  if (sub == NULL)
    signalMCLException("DelayedMaintenanceExp built with NULL subExpectation.");
  return (internalTimer.expired() && sub->violation());
}

//////
///// phaseChange (delay, changes to target value then maintains)
//////

mclPhaseChangeExp::mclPhaseChangeExp(mcl *m, 
				     double delay, 
				     string sensor, 
				     float target) :
  mclDelayedMaintenanceExp(m,delay,
			   new mclMaintainValueExp(sensor,m,target)) {};
