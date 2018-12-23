#include "mcl_multiagent_api.h"
#include "mcl_ma_internal_api.h"
#include "APITypes.h"
#include "APICodes.h"
#include "mcl.h"

#include "mclExceptions.h"
// #include "configManager.h"
#include "ontology_config.h"
#include "noiseProfiles.h"

#include "mclSettings.h"
#include "mclLogger.h"

#include <iostream>
#include <map>

using namespace std;

map<string,mcl*> mcl_ma_map;

char _ma_perror[512];

//////////////////////////////////////////////////////////////////
// INITIALIZE / SETUP

mcl* get_or_create(string k) {
  if (mcl_ma_map.find(k) != mcl_ma_map.end()) {
    return mcl_ma_map[k];
  }
  else {
    mcl* nuMCL = new mcl();
    nuMCL->setMAkey(k);
    mcl_ma_map[k]=nuMCL;
    return mcl_ma_map[k];
  }
}

void dumpbase() {
  for (map<string,mcl*>::iterator mmi = mcl_ma_map.begin();
       mmi != mcl_ma_map.end();
       mmi++) {
    printf(" M(%s) = 0x%08x\n",mmi->first.c_str(),mmi->second);
  }
}

inline mcl* get_mcl(string k) { 
  return mcl_ma_map[k]; 
}

bool mclMA::initializeMCL(string key,int Hz) {
  mclLogger::annotate(MCLA_MSG,
		      "[mcl/maAPI]::Initializing MCL ~> supervising " + key);
  setExceptionHandlers();
  mcl* this_mcl = get_or_create(key);
  if (Hz == 0) 
    this_mcl->setSynchronous();
  else
    this_mcl->setAsynchronous(Hz);
  this_mcl->clearSensorDefs();
  if (mclSettings::debug) {
    mclLogger::annotate(MCLA_DBG,
			"[mcl/maAPI]::baseline PV = ");
    this_mcl->getDefaultPV()->dumpEntity(mclLogger::mclLog);
  }
  this_mcl->start();
  // old domain config...
  // cfg::loadConfig(key);
  return true;
}

bool mclMA::configureMCL(string key,string domain) {
  mcl* this_mcl = get_or_create(key);
  this_mcl->setConfigKey(ontology_configurator::require_config(domain));
}

bool mclMA::configureMCL(string key, string domain, string agent) {
  mcl* this_mcl = get_or_create(key);
  this_mcl->setConfigKey(ontology_configurator::require_config(domain,agent));
}

bool mclMA::configureMCL(string key, string domain, string agent, string controller) {
  mcl* this_mcl = get_or_create(key);
  this_mcl->setConfigKey(ontology_configurator::require_config(domain,agent,controller));
}

//////////////////////////////////////////////////////////////////
// Sensor Vector Compatibility Mode

bool mclMA::initializeSV(string key,float *sv,int svs) {
  mcl* this_mcl = get_mcl(key);
  this_mcl->setSV(sv,svs);
  mclLogger::annotate(MCLA_MSG,"[mcl/hostAPI]:: "+key+" sensor vector initialized.");
  return true;
}

bool mclMA::updateSV(string key,float *sv,int svs) {
  mcl* this_mcl = get_mcl(key);
  this_mcl->setSV(sv,svs);  
  return true;
}

bool mclMA::registerSensor(string key,string name) {
  observables::declare_observable_self(key,name);

  /* old compatibility...
     mcl* this_mcl = get_mcl(key);
     this_mcl->addSensorDef(new mclSensorDef(name,this_mcl));
     mclLogger::annotate(MCLA_MSG,"[mcl/hostAPI]:: "+key+" registered sensor '"+name+"'");
  */
  return true;
}

//////////////////////////////////////////////////////////////////
// Host Initiated Anomalies

bool mclMA::HIA::registerHIA(string key,string name,string failureNodeName) {
  mcl* this_mcl = get_mcl(key);
  this_mcl->addHIADef(new mclHIADef(name,failureNodeName,this_mcl));
  mclLogger::annotate(MCLA_MSG,"[mcl/hostAPI]::registered host-initiated indication '"+name+"'");
  return true;
}

bool mclMA::HIA::signalHIA(string key,string name) {
  mcl* this_mcl = get_mcl(key);
  return this_mcl->signalHIA(name);
}

bool mclMA::HIA::signalHIA(string key,string name,resRefType referent) {
  mcl* this_mcl = get_mcl(key);
  return this_mcl->signalHIA(name,referent);
}

//////////////////////////////////////////////////////////////////
// noise profiles

bool mclMA::setSensorNoiseProfile(string mclkey, string sname,spvType npKey) {
  observables::set_obs_noiseprofile_self(mclkey,sname,npKey);
  /* old version
  mcl* this_mcl = get_mcl(mclkey);
  mclSensorDef *sd = this_mcl->getSensorDef(sname);
  printf("[mclmc]:: setting np @ %s.%s[%08x]\n",
	 mclkey.c_str(),sname.c_str(),npKey);
  if (sd != NULL) {
    sd->setSensorProp(PROP_NOISEPROFILE,npKey);
    noiseProfile* theNP = noiseProfile::createNoiseProfile(npKey);
    noiseProfile::establishNoiseProfileFor(mclkey,sname,theNP);
    return true;
  }
  else {
    sprintf(_ma_perror,"Attempt set property of non-existant sensor '%s'",sname.c_str());
    signalMCLException(_ma_perror);
    return false;
  }
  */
}

bool mclMA::setSensorNoiseProfile(string mclkey, string sname,spvType npKey,
				  double param) {
  observables::set_obs_noiseprofile_self(mclkey,sname,npKey,param);
  /*
  mcl* this_mcl = get_mcl(mclkey);
  mclSensorDef *sd = this_mcl->getSensorDef(sname);
  printf("[mclmc]:: setting np @ %s.%s[%08x:%f]\n",
	 mclkey.c_str(),sname.c_str(),npKey,param);
  if (sd != NULL) {
    sd->setSensorProp(PROP_NOISEPROFILE,npKey);
    noiseProfile* theNP = noiseProfile::createNoiseProfile(npKey,param);
    noiseProfile::establishNoiseProfileFor(mclkey,sname,theNP);
    return true;
  }
  else {
    sprintf(_ma_perror,"Attempt set property of non-existant sensor '%s'",sname.c_str());
    signalMCLException(_ma_perror);
    return false;
  }
  */
}

bool mclMA::setSensorProp(string mclkey,string obsname,
			  spkType key,spvType pv) {
  if (key == PROP_NOISEPROFILE)
    observables::set_obs_noiseprofile_self(mclkey,obsname,pv);
  else 
    observables::set_obs_prop_self(mclkey,obsname,key,pv);

  /* this is the old shit....
  mcl* this_mcl = get_mcl(mclkey);
  mclSensorDef *sd = this_mcl->getSensorDef(name);
  printf("[mclmc]:: setting %s.%s[%08x]=%08x\n",
	 mclkey.c_str(),name.c_str(),key,pv);
  if (sd != NULL) {
    sd->setSensorProp(key,pv);
    if (key == PROP_NOISEPROFILE) {
      // this will create a 0-arg noise profile as specified...
      // USE setSesnorNoiseProfile() TO SPECIFY PARAMETERS!!
      noiseProfile* theNP = noiseProfile::createNoiseProfile(pv);
      noiseProfile::establishNoiseProfileFor(mclkey,name,theNP);
    }
    return true;
  }
  else {
    sprintf(_ma_perror,"Attempt set property of non-existant sensor '%s'",name.c_str());
    signalMCLException(_ma_perror);
    return false;
  }
  */
}

void mclMA::newDefaultPV(string key) {
  mcl* this_mcl = get_mcl(key);
  this_mcl->newDefaultPV();
}

void mclMA::popDefaultPV(string key) {
  mcl* this_mcl = get_mcl(key);
  this_mcl->popDefaultPV();
}

void mclMA::setEGProperty(string key,egkType eg_key,int index,pvType value) {
  mcl* this_mcl = get_mcl(key);
  if (this_mcl->getExpGroup(eg_key))
    this_mcl->getExpGroup(eg_key)->getPV()->setProperty(index,value);
  else { 
    sprintf(_ma_perror,"Attempt set property of non-existant EG stack '%s.%x'",key.c_str(),eg_key);
    signalMCLException(_ma_perror);
  }
}

void mclMA::setPropertyDefault(string key,int index,pvType value) {
  mcl* this_mcl = get_mcl(key);
  this_mcl->getDefaultPV()->setProperty(index,value);
}

void mclMA::reSetDefaultPV(string key) {
  mcl* this_mcl = get_mcl(key);
  this_mcl->getDefaultPV()->reSetProperties();
}

//////////////////////////////////////////////////////////////////
// OPERABILITY

responseVector mclMA::monitor(string key,float *sv,int svs) {
  mcl* this_mcl = get_mcl(key);
  mclLogger::annotateStart(MCLA_MSG);
  if (mclSettings::debug) {
    *mclLogger::mclLog << "[mcl/hostAPI]::monitor[";
    for (int i=0;i<svs;i++) {
      *mclLogger::mclLog << sv[i];
      if (i<svs-1) *mclLogger::mclLog << ",";
    }
    *mclLogger::mclLog << "]" << endl;
  }
  this_mcl->setSV(sv,svs);
  return this_mcl->monitor();
  mclLogger::annotateEnd(MCLA_MSG);
}

responseVector mclMA::monitor(string key,observables::update& the_update) {
  mcl* this_mcl = get_mcl(key);

  // mclLogger::annotateStart(MCLA_MSG);
  // the_update.dumpEntity(mclLogger::mclLog);
  // mclLogger::annotateEnd(MCLA_MSG);

  observables::apply_update(key,the_update);
  return this_mcl->monitor();

}

responseVector mclMA::emptyResponse() {
  responseVector frrvec;
  return frrvec;
}

bool mclMA::updateObservables(string key,observables::update& the_update) {
  observables::apply_update(key,the_update);
  return true;
}

//////////////////////////////////////////////////////////////////
// EXPECTATIONS

void mclMA::declareExpectationGroup(string key,egkType eg_key) {
  // mcl* this_mcl = get_mcl(key);
  declareExpectationGroup(key,eg_key,NULL,NULL);
}

void mclMA::declareExpectationGroup(string key, egkType eg_key,
				    egkType parent_key, resRefType ref) {
  sprintf(mclLogger::annotateBuffer,"[mcl/hostAPI]:: expectation group being declared <m=%s k=0x%x pk=0x%x ref=0x%x>",key.c_str(),eg_key,parent_key,ref);
  mclLogger::annotateFromBuffer(MCLA_MSG);
  mcl* this_mcl = get_mcl(key);
  sprintf(mclLogger::annotateBuffer,"[mcl/hostAPI]:: expectation group declared <m=0x%x k=0x%x pk=0x%x ref=0x%x>",this_mcl,eg_key,parent_key,ref);
  mclLogger::annotateFromBuffer(MCLA_MSG);
  this_mcl->declareExpectationGroup(eg_key,parent_key,ref);
}

void mclMA::expectationGroupAborted(string key,egkType eg_key) {
  mcl* this_mcl = get_mcl(key);
  this_mcl->expectationGroupAborted(eg_key);
}

void mclMA::expectationGroupComplete(string key,egkType eg_key,float *sv,int svs) {
  mcl* this_mcl = get_mcl(key);
  this_mcl->setSV(sv,svs);
  this_mcl->expectationGroupComplete(eg_key);
}

void mclMA::expectationGroupComplete(string key,egkType eg_key,
				     observables::update& the_update) {
  mcl* this_mcl = get_mcl(key);
  observables::apply_update(key,the_update);
  this_mcl->expectationGroupComplete(eg_key);
}

void mclMA::expectationGroupComplete(string key,egkType eg_key) {
  mcl* this_mcl = get_mcl(key);
  this_mcl->expectationGroupComplete(eg_key);
}

///
/// these are internal functions (not in the API)
///

void ensure_eg(string key,egkType eg_key) {
  mcl* this_mcl = get_mcl(key);
  if (this_mcl->getExpGroup(eg_key) == NULL) {
    sprintf(mclLogger::annotateBuffer,
	    "Attempt to add exp to non-existant EG @0x%x",eg_key);
    mclLogger::annotateFromBuffer(MCLA_ERROR);
    mclMA::declareExpectationGroup(key,eg_key);
  }
}

void ensure_sensor(string key,string sensor) {
  mcl* this_mcl = get_mcl(key);
  if (this_mcl->getSensorDef(sensor) == NULL) {
    sprintf(_ma_perror,"Attempt to add exp for non-existant sensor '%s'",sensor.c_str());
    signalMCLException(_ma_perror);
  }
}

mclExp *_ma_makeExp(string key,ecType code,float arg) {
  mcl* this_mcl = get_mcl(key);
  switch (code) {
  case EC_REALTIME:
     return expectationFactory::makeRealtimeExp(this_mcl,arg);
     break;
  case EC_TICKTIME:
     return expectationFactory::makeTickExp(this_mcl,arg);
     break;
  default:
    sprintf(_ma_perror,
	    "Could not handle EXP code 0x%08x in declaration (args=())",
	    code);
    signalMCLException(_ma_perror);
    return NULL;
  }
}

mclExp *_ma_makeExp(string key,string sensor,ecType code) {
  mcl* this_mcl = get_mcl(key);
  switch (code) {
  case EC_ANY_CHANGE:
    return expectationFactory::makeNetChangeExp(this_mcl,sensor);
    break;
  case EC_NET_ZERO:
    return expectationFactory::makeNZExp(this_mcl,sensor);
    break;
  case EC_GO_UP:
    return expectationFactory::makeGoUpExp(this_mcl,sensor);
    break;
  case EC_GO_DOWN:
    return expectationFactory::makeGoDownExp(this_mcl,sensor);
    break;
  case EC_MAINTAINVALUE:
    return expectationFactory::makeMVExp(this_mcl,sensor);
    break;
  default:
    sprintf(_ma_perror,
	    "Could not handle EXP code 0x%08x in declaration (args=())",
	    code);
    signalMCLException(_ma_perror);
    return NULL;
  }
}

mclExp *_ma_makeExp(string key,string sensor,ecType code,float value) {
  mcl* this_mcl = get_mcl(key);
  switch (code) {
  case EC_STAYUNDER:
    return expectationFactory::makeUBExp(this_mcl,sensor,value);
    break;
  case EC_STAYOVER:
    return expectationFactory::makeLBExp(this_mcl,sensor,value);
    break;
  case EC_MAINTAINVALUE:
    return expectationFactory::makeMVExp(this_mcl,sensor,value);
    break;
  case EC_TAKE_VALUE:
    return expectationFactory::makeTakeValExp(this_mcl,sensor,value);
    break;
  default:
    sprintf(_ma_perror,"Could not handle EXP code 0x%08x in expectation declaration (dispatch with args (float)",
	    code);
    signalMCLException(_ma_perror);
    return NULL;
  }
}

///
/// now here are the published functions
///

// ALL THE declareExpectation FUNCTIONS HAVE BEEN CONVERTED TO ASSUME
// SELF OBSERVABLES. TO GET OBJECT/OBSERVABLE EXPECTATIONS, USE THE NEW
// FUNCTIONS declareObjectExpectation(...)

void mclMA::declareExpectation(string key,egkType eg_key,ecType code,float arg) {
  mcl* this_mcl = get_mcl(key);
  ensure_eg(key,eg_key);
  sprintf(mclLogger::annotateBuffer,
	  "[mcl/hostAPI]:: expectation declared (c=0x%x,v=%d)",
	  code,arg);
  mclLogger::annotateFromBuffer(MCLA_VRB);
  mclExp* nuE = _ma_makeExp(key,code,arg);
  this_mcl->getExpGroup(eg_key)->addExp(nuE);
}

void mclMA::declareExpectation(string key,egkType eg_key,string self_sensor,ecType code) {
  mcl* this_mcl = get_mcl(key);
  string sensor = observable::make_self_named(self_sensor);
  ensure_eg(key,eg_key);
  ensure_sensor(key,sensor);
  sprintf(mclLogger::annotateBuffer,
	  "[mcl/hostAPI]:: expectation declared %s(c=0x%x)",
	  sensor.c_str(),code);
  mclLogger::annotateFromBuffer(MCLA_VRB);
  mclExp *nuE = _ma_makeExp(key,sensor,code);
  this_mcl->getExpGroup(eg_key)->addExp(nuE);
}

void mclMA::declareExpectation(string key,egkType eg_key,string self_sensor,
			       ecType code,float value) {
  mcl* this_mcl = get_mcl(key);
  string sensor = observable::make_self_named(self_sensor);
  ensure_eg(key,eg_key);
  ensure_sensor(key,sensor);
  sprintf(mclLogger::annotateBuffer,
	  "[mcl/hostAPI]:: expectation declared %s(c=0x%x,v=%d)",
	  sensor.c_str(),code,value);
  mclLogger::annotateFromBuffer(MCLA_VRB);
  mclExp *nuE=_ma_makeExp(key,sensor,code,value);
  this_mcl->getExpGroup(eg_key)->addExp(nuE);
}

void mclMA::declareDelayedExpectation(string key,double delay,egkType eg_key,
				       string self_sensor,ecType code) {
  mcl* this_mcl = get_mcl(key);
  string sensor = observable::make_self_named(self_sensor);
  ensure_eg(key,eg_key);
  ensure_sensor(key,sensor);
  sprintf(mclLogger::annotateBuffer,
	  "[mcl/hostAPI]:: expectation declared %s+%ds(c=0x%x)",
	  sensor.c_str(),delay,code);
  mclLogger::annotateFromBuffer(MCLA_VRB);
  mclExp *se = _ma_makeExp(key,sensor,code);
  mclExp *de = 
    expectationFactory::makeDelayedExp(this_mcl,delay,se);
  this_mcl->getExpGroup(eg_key)->addExp(de);  
}

void mclMA::declareDelayedExpectation(string key,double delay,egkType eg_key,
				      string self_sensor,ecType code,float v) {
  mcl* this_mcl = get_mcl(key);
  string sensor = observable::make_self_named(self_sensor);
  ensure_eg(key,eg_key);
  ensure_sensor(key,sensor);
  sprintf(mclLogger::annotateBuffer,
	  "[mcl/hostAPI]:: expectation declared %s+%ds(c=0x%x,v=%d)",
	  sensor.c_str(),delay,code,v);
  mclLogger::annotateFromBuffer(MCLA_VRB);
  mclExp *se = _ma_makeExp(key,sensor,code,v);
  mclExp *de = 
    expectationFactory::makeDelayedExp(this_mcl,delay,se);
  this_mcl->getExpGroup(eg_key)->addExp(de);  
}

//
/// interactivity functionality
//

void mclMA::suggestionImplemented(string key,resRefType referent) {
  mclFrame *frame = static_cast<mclFrame *>(referent);
  if (frame == NULL) {
    signalMCLException("suggestionImplemented called with NULL referent.");
  }
  else {
    // it might be smart to do something else here, like
    // record the time at which the suggestion was accepted (for durative
    // & diagnostic purposes)
    mclLogger::annotate(MCLA_MSG,"[mcl/hostAPI]:: host has indicated that response to "+ frame->entityName() +" has been triggered.");
    frame->setState(FRAME_RESPONSE_TAKEN);
  }
}

void mclMA::suggestionFailed(string key,resRefType referent) {
  mcl* this_mcl = get_mcl(key);
  mclFrame *frame = static_cast<mclFrame *>(referent);
  if (frame == NULL) {
    signalMCLException("suggestionFailed called with NULL referent.");
  }
  else {
    this_mcl->processOffCycleFailure(frame);
  }
}

void mclMA::suggestionIgnored(string key,resRefType referent) {
  mcl* this_mcl = get_mcl(key);
  mclFrame *frame = static_cast<mclFrame *>(referent);
  if (frame == NULL) {
    signalMCLException("suggestionIgnored called with NULL referent.");
  }
  else {
    this_mcl->processResponseIgnored(frame);
    // frame->setState(FRAME_RESPONSE_IGNORED);
  }
}

void mclMA::provideFeedback(string key,bool feedback, resRefType referent) {
  mcl* this_mcl = get_mcl(key);
  mclFrame *frame = static_cast<mclFrame *>(referent);
  mclLogger::annotate(MCLA_MSG, "[mcl/hostAPI]:: receiving feedback for " +
		      frame->entityName());
  if (frame == NULL) {
    // probably should issue a warning and not generate an exception
    signalMCLException("provideFeedback called with NULL or NON-FRAME referent.");
  }
  else {
    this_mcl->processHostFeedback(feedback,frame);
  }  
}

// UTILITIES

bool mclMA::writeInitializedCPTs(string key) {
  mcl* m = mclMAint::getMCLFor(key);
  if (m == NULL) {
    sprintf(_ma_perror,"attempting to write CPT for '%s' but it does not have an associated, initialized MCL.",key.c_str());
    signalMCLException(_ma_perror);
  }
  else {
    mclFrame    *f = new mclFrame(m,NULL,NULL);
    bool rv = (mclSettings::getSysPropertyBool("writeConfigGlobal",false)) ?
      cpt_cfg::save_cpts_to_global(f) :      
      cpt_cfg::save_cpts_to_home(f);
    if (rv) {
      return true;
    }
    else {
      cout << "a problem occurred while writing to " 
	   << "'" << f->MCL()->getConfigKey() << "'"
	   << endl;
      return false;
    }
  }
}

bool mclMA::writeInitializedCosts(string key) {
  return false;
}

// DEBUG / DIAGNOSTICS

void mclMA::setOutput(string fn) { mclLogger::setLogToFile(fn); }
void mclMA::setStdOut() { mclLogger::setLogToStdOut(); }
void mclMA::beQuiet()   { mclSettings::quiet=true; }
void mclMA::beVerbose() { mclSettings::quiet=false; }
void mclMA::goDebug()   { mclSettings::debug=true; }
void mclMA::noDebug()   { mclSettings::debug=false; }

void mclMA::dumpOntologiesMostRecent(string key) {
  mcl* this_mcl = get_mcl(key);
  this_mcl->dumpMostRecentFrame();
}

void mclMA::dumpMostRecentFrameDot(string key) {
  dumpFrameDot(key,0);
}
void mclMA::dumpFrameDot(string key,int dot) {
  mcl* this_mcl = get_mcl(key);  
  this_mcl->dumpFrameDot(dot);
}

void mclMA::dumpMCL() {
  cout << "MCL Configurations :" << endl;
  for (map<string,mcl*>::iterator mmI=mcl_ma_map.begin();
       mmI!=mcl_ma_map.end();
       mmI++) {
    cout << " ~> " << mmI->first << ": @0x" << hex << mmI->second << endl;
  }
}

/////////////////////////////////
////// INTERNAL FUNCTIONALITY

mcl* mclMAint::getMCLFor(string k) {
  if (mcl_ma_map.find(k) != mcl_ma_map.end()) {
    return mcl_ma_map[k];
  }
  else return NULL;
}
