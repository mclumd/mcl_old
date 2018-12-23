#include "expectations.h"
#include "mcl.h"
#include <math.h>

using namespace metacog;

mclExp *expectationFactory::makeRealtimeExp(mcl *m,float seconds) {
   int secs = (int)floorf(secs);
   int usec = (int)(seconds - secs);
   return new mclRealTimeExp(m,secs,usec);
}

mclExp *expectationFactory::makeTickExp(mcl *m,float ticks) {
   return new mclTickExp(m,(int)ticks);
}

mclExp *expectationFactory::makeNetChangeExp(mcl *m,string sname) {
  return new mclAnyNetChangeExp(sname,m);
}

mclExp *expectationFactory::makeNZExp(mcl *m,string sname) {
  return new mclNoNetChangeExp(sname,m);
}

mclExp *expectationFactory::makeGoDownExp(mcl *m,string sname) {
  return new mclGoDownExp(sname,m);
}

mclExp *expectationFactory::makeGoUpExp(mcl *m,string sname) {
  return new mclGoUpExp(sname,m);
}

mclExp *expectationFactory::makeTakeValExp(mcl *m,string sname,float val) {
  return new mclTakeValueExp(sname,m,val);
}

mclExp *expectationFactory::makeUBExp(mcl *m,string sname,float val) {
  return new mclUpperBoundExp(sname,m,val);
}

mclExp *expectationFactory::makeLBExp(mcl *m,string sname,float val) {
  return new mclLowerBoundExp(sname,m,val);
}

mclExp *expectationFactory::makeMVExp(mcl *m,string sname) {
  return new mclMaintainValueExp(sname,m);
}

mclExp *expectationFactory::makeMVExp(mcl *m,string sname,float val) {
  return new mclMaintainValueExp(sname,m,val);
}

mclExp *expectationFactory::makeDelayedExp
(mcl *m,double seconds,mclExp *sub) {
  return new mclDelayedMaintenanceExp(m,seconds,sub);
}
