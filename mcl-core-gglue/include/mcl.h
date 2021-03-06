#ifndef MCL_BASE_H
#define MCL_BASE_H

/** \file
 * \brief class definition for the MCL structure.
 */

#include "mclTimer.h"
#include "mclEntity.h"
#include "mclSensorDef.h"
#include "mcl_observables.h"
#include "mclHIIDef.h"
#include "mclMonitorResponse.h"
#include "mclFrame.h"
#include "mclProperties.h"
#include "APITypes.h"
#include "APICodes.h"
#include "expectations.h"
#include <vector>
#include <map>

typedef vector<mclExpGroup *>    egMap;
typedef vector<mclFrame *>       frameVec;
typedef vector<mclSensorDef *>   sdVec;
typedef vector<mclHIADef *>      HIAVec;

/** the Meta Cognitive Loop class.
 * manages expectations, monitoring of expectations, and meta-reasoning
 * about failures for a host system.
 */
class mcl : public mclEntity {
 public:
  mcl();

  string baseClassName() { return "mcl"; };
  
  mcl *MCL() { return this; };

  void setHostName(string hn) { hName=hn; };
  void setConfigKey(string kc);
  string getConfigKey() { return cKey; };
  void setMAkey(string mk) { maKey=mk; };
  string getMAkey() { return maKey; };

  //! sets operation of the MCL to be synchronous with #monitor() calls.
  void setSynchronous()       { synchronous=true; };
  //! sets operation of the MCL to be asynchronous with #monitor() calls.
  //! Asynchronous operation requires a Hz response rate. If #monitor() is
  //! called more frequently than the specified Hz response rate, #monitor()
  //! will return NULL in the off-cycle calls.
  void setAsynchronous(int h) { synchronous=false; Hz=h; };

  void clearSensorDefs();

  void addSensorDef(mclSensorDef *sd) { sensorDefs.push_back(sd); };
  mclSensorDef *getSensorDef(string name);

  void addHIADef(mclHIADef *sd)       { HIAdefs.push_back(sd); };
  mclMonitorResponse *signalHIA(string name);
  mclMonitorResponse *signalHIA(string name,resRefType referent);
  mclMonitorResponse *activateNodeDirect(string name,resRefType referent);

  /** passes over active expectations to check for violations.
   * Actually, a ton of stuff goes on in monitor(). The chief thing is to 
   * run through expectations looking for violations, though. For each,
   * an #mclFrame is created and inference is run over the ontologies.
   * For each new or updated (existing mclFrames are also updated at 
   * this point) frame, an @mclMonitorResponse is created and returned in a
   * vector to the host.
   */
  responseVector monitor();

  void start()   { active=true;  };
  void stop () { active=false; };

  mclExpGroup* getExpGroup(egkType eg_key, bool returnMarkedGroups=false);
  void addExpGroup(egkType eg_key,mclExpGroup* eg)
    { expGroups.push_back(eg); };
  void declareExpectationGroup(egkType key,egkType parent,resRefType ref);
  void expectationGroupAborted(egkType eg_key);
  void expectationGroupComplete(egkType eg_key);

  void dump_eg_table();

  //! returns the value of the named sensor.
  float sensorValue(string sn);
  //! returns the value of the sensor indicated by its index into the sensor vector.
  float sensorValue(int     q);
  //! sets the sensor values in the sensor vector.
  //! @param sv a pointer to an array of floats
  //! @param len the length of the float array
  void  setSV(float *sv,int len);

  //! issues a new poperty vector and places it on the top of the PV stack.
  void newDefaultPV() { defaultPropertyStack.newPropertyVector(); };
  //! pops the topmost property vector off the PV stack.
  void popDefaultPV() { defaultPropertyStack.popPropertyVector(); };
  //! returns the topmost property vector from the default PV stack.
  mclPropertyVector *getDefaultPV() 
    { return defaultPropertyStack.currentPV(); };
  //! tests a property value from the topmost property vector on the PV stack.
  //! @param propKey the key for the property to be tested (see #APICodes.h)
  //! @param propVal the target value to be tested against (see #APICodes.h)
  bool testPropertyDefault(pkType propKey, pvType propVal) {
    return getDefaultPV()->testProperty(propKey,propVal);
    // return (getDefaultPV()->getPropertyValue(propKey) == propVal);
  };

  //! returns the number of #monitor() calls that have been processed.
  int tickNumber() { return tickCounter; };

  //! activates the relevant indication fringe nodes for a violation.
  //! the source for this functionality can be found in @dynamicILinks.cc
  //! and uses an expectation method called #generateLinkTags() to generate
  //! a listing of the appropriate fringe node names to activate.
  //! @param m an mclFrame describing an expectation violation
  //! @param e an expectation that has been violated
  void linkToIndicationFringe(mclFrame* m,mclExp* e);

   // re-entrant behavior

  //! the host has ignored the response suggested in the specified frame.
  void processResponseIgnored(mclFrame* frame);

  //! the host is providing feedback to an interactive response
  void processHostFeedback(bool hostReply, mclFrame* frame);

  //! the host has signaled failure of a frame recommendation.
  //! "Off cycle" refers to any MCL/Host event that occurs off of a #monitor()
  //! cycle. Since #monitor() is the only time which MCL communicates to the
  //! Host, off cycle events must be added to a queue for processing at the
  //! beginning of the next monitor() call.
  void processOffCycleFailure(mclFrame* frame);

  //! attempts to recover a frame using provided context.
  //! This function will return the Frame most relevant to the provided 
  //! context: using the referent if possible, then the EG Key of the 
  //! violation, then the parent's key.
  //! @param r a Response Referent (a pointer to an MCL Frame)
  //! @param e an expectation group key
  //! @param p the expectation group of the parent
  mclFrame* recoverExistingFrame(resRefType r,egkType e,egkType p);

  void deleteMarkedGroups();
  void dumpMostRecentFrame();
  void dumpFrameDot(int which);

  //! specifies that the Frame has pending bidness with MCL.
  void pending(mclFrame* nf);

  // these are to support the enhanced observables code
  void add_observable(observable* noo)
    { my_ov->add_observable(noo); };
  void add_observable_object_def(object_def* od)
    { my_ov->add_observable_object_def(od); };
  void add_object_field_def(string tname,string fname,mclSensorDef& sd)
    { my_ov->add_object_field_def(tname,fname,sd); };
  void notice_object_is_observable(string otype,string oname) 
    { my_ov->observe_observable_object(otype,oname); };
  void notice_object_unobservable(string oname)
    { my_ov->stow_observable_object(oname); };
  void observable_update_pp(string obsname, double v)
    { my_ov->observable_update_pp(obsname,v); };
  void observable_update(string obsname, double v)
    { my_ov->observable_update(obsname,v); };
  void observable_update(string objname, string obsname, double v)
    { my_ov->observable_update(objname,obsname,v); };
  void dump_ov() { my_ov->dump(); };
  void set_obs_prop_self(string obsname,spkType key,spvType pv)
    { my_ov->set_obs_prop_self(obsname,key,pv); };
  void set_obs_prop(string objname, string obsname,spkType key,spvType pv)
    { my_ov->set_obs_prop(objname,obsname,key,pv); };
  void dump_obs(string obsname)
    { my_ov->dump_obs(obsname); };
  void dump_obs(string objname, string obsname)
    { my_ov->dump_obs(objname,obsname); };
  string ov2string() { return my_ov->active2string(); };

 private:

  // general info / parameters
  string hName;
  string cKey; // configuration key to be used by ontology_configurator
  string maKey;

  // stuff for doing updates
  bool     active;
  bool     synchronous;
  int      Hz;
  mclTimer ticker; 
  int      tickCounter;

  // vectors of things mcl needs to keep track of
  sdVec    sensorDefs;
  HIAVec   HIAdefs;
  egMap    expGroups;
  frameVec frames;

  observable_vector* my_ov;
  
  //! pending frames is where frames that have been generated "off cycle"
  //! (not in the note function) are stored for processing at the next note
  //! phase
  frameVec pendingFrames;

  mclPropertyVectorStack defaultPropertyStack;

  float   *sv;
  int      svl;

  // internal functions

  void safeFramePush(mclFrame* nf,frameVec* fv);
  mclHIADef *getHIADef(string name);
  //! destroys an expectation group without checking effects expectations.
  void deleteExpGroup(mclExpGroup* eg);
  //! queues an expectation group for deletion on cycle.
  void markExpGroupForDelete(egkType eg_key);
  //! queues an expectation group for deletion on cycle.
  void markExpGroupForDelete(mclExpGroup* egp);

  // NAG
  frameVec           *note  ();
  bool                assess(mclFrame *m);
  mclMonitorResponse *guide (mclFrame *m);  

  //! executes the note step for a single expectation group.
  //! @note internal use only.
  bool noteForExpGroup(frameVec* resVec,mclExpGroup* eGrp,bool complete);
  //! considers retiring a frame if it is a success.
  void maybeRetireFrame(mclFrame* the_frame);

};

#endif
