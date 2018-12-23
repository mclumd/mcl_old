#include "makeDot.h"
#include "mcl_api.h"
#include "mclConstants.h"
#include "mcl.h"
#include "umbc/settings.h"
#include "umbc/exceptions.h"
#include "mclFrame.h"
#include "mclLogging.h"
#include "ontology_config.h"

#include <algorithm>

using namespace metacog;
using namespace umbc;

// REB = Re Entrant Behavior
#define DEFAULT_REB_IGNORE   1 
#define DEFAULT_REB_REENTER  2
#define DEFAULT_REB_NEWFRAME 3
#define DEFAULT_REB          DEFAULT_REB_REENTER

mcl::mcl() : 
  mclEntity(),hName("no host"),cKey(""),maKey(""),
  active(false),synchronous(true),Hz(0),tickCounter(0),
  sv(NULL),svl(0) {
  my_ov = new observable_vector(this,maKey+"-ov"); 
  oName=settings::getSysPropertyString("mcl.defaultOntology","basic");
}

void mcl::setOntologyBase(string ont) { 
  oName=ont; 
  uLog::annotate(MCLA_DBG,
		    entityName()+": ontology set to '"+getOntologyBase()+"'");
}

void mcl::setConfigKey(string kc) { 
  cKey=kc; 
  uLog::annotate(MCLA_DBG,
		    entityName()+": config key set to '"+getConfigKey()+"'");
}

void mcl::clearSensorDefs() {
  for (sdVec::iterator si = sensorDefs.begin();
       si!=sensorDefs.end();
       si++)
    delete (*si);
  sensorDefs.clear();
}

// THIS MONITOR FUNCTION USES THE MULTI-FRAME ALGORITHM FOR DEALING WITH
// CONTEMPORANEOUS VIOLATIONS ....

responseVector mcl::monitor() {
  uLog::annotate(MCLA_MSG,"[mcl/mcl]:: entering monitor call ("+getMAkey()+")");
  dump_ov();
  responseVector frrvec; // frame-response reply vector
  frameVec::iterator fvi;
  if (active && (synchronous || ticker.expired())) {
    if (!synchronous) {
      ticker.restartHz(Hz);
    }
    tickCounter++;
    //  NOTE
    frameVec *frvec = note();
    sprintf(umbc::uLog::annotateBuffer,
	    "[mcl/mcl]:: note returned %ld frame(s)",
	    (long)frvec->size());
    umbc::uLog::annotate(MCLA_VRB);
	    
    if (frvec->size() != 0) {

      // run through the frames
      for (fvi = frvec->begin();
	   fvi != frvec->end();
	   fvi++) {
	//  ASSESS
	assess(*fvi);
      }

      // attempt to merge frames...
      // <!!!>
      // mergeFrames(frvec);
 
      //  GUIDE - go over merged frames, collect responses
      for (fvi = frvec->begin();
	   fvi != frvec->end();
	   fvi++) {
	mclMonitorResponse *r = guide(*fvi);
	if (r != NULL)
	  frrvec.push_back(r);
	else {
	  // there is no response -- should we kill this frame?
	  (*fvi)->setState(FRAME_DEADEND);
	}
      }
    }
    delete frvec;
  }
  else {
    if (active)
      umbc::uLog::annotate(MCLA_MSG,"[mcl/mcl]:: mcl throttling.");
    else 
      umbc::uLog::annotate(MCLA_MSG,"[mcl/mcl]:: mcl inactive.");
  }
  // check for marked groups
  deleteMarkedGroups();

  umbc::uLog::annotate(MCLA_MSG,"[mcl/mcl]:: exiting monitor call");
  return frrvec;
}

bool mcl::noteForExpGroup(frameVec* resVec,mclExpGroup* eGrp,bool complete) {

   // this is a make-or-break condition
   if (eGrp->markedForDelete() && !complete)
      return false;

   // okay, either we are monitoring a live group or a marked group 
   // that is completing
   if (complete)
     umbc::uLog::annotate(MCLA_VRB,"[mcl/mcl]:: noteForExpGrp called on completion of " + eGrp->entityName());
   else
     umbc::uLog::annotate(MCLA_VRB,"[mcl/mcl]:: noteForExpGrp called to monitor " + eGrp->entityName());

   bool rv=false;
   int  rvc=0;

   // determine whether or not a frame exists for this eGroup
   mclFrame *m=NULL;
   
   {
     umbc::uLog::annotateStart(MCLA_PRE);
     *umbc::uLog::log << "[mcl/mcl]::checking for matching frames..." 
			<< endl;
     *umbc::uLog::log << "[mcl/mcl]:: there are " << frames.size() 
			<< " frame(s) on hand." << endl;
     *umbc::uLog::log << "[mcl/mcl]:: current eg is characterized by < ref="
			<< hex << eGrp->get_referent() << ",key=" 
			<< hex << eGrp->get_egKey() << ",parent="
			<< hex << eGrp->get_parent_egKey() << " >" << endl;
     umbc::uLog::annotateEnd(MCLA_PRE);
   }

   m = recoverExistingFrame(eGrp->get_referent(),
			    eGrp->get_egKey(),eGrp->get_parent_egKey());
   if (m!=NULL)
     umbc::uLog::annotate(MCLA_MSG,"[mcl/mcl]: frame exists: "+m->entityName());

   // run through expectations... 
   for (expList::iterator elI = eGrp->expListHead();
        elI != eGrp->expListButt();
        elI++) {
    if (((*elI)->checkAlways() && (*elI)->violation()) ||
	((*elI)->checkOnExit() && complete && (*elI)->violation())) {
      eGrp->setLastViolationTick(tickCounter);
      {
	umbc::uLog::annotateStart(MCLA_VIOLATION);
	*umbc::uLog::log << "[mcl/mcl]::VIOLATION ~> " 
			   << (*elI)->entityName() 
			   << " in group " << hex << eGrp->get_egKey() << endl;
	umbc::uLog::annotate(MCLA_BREAK);
	umbc::uLog::annotateStart(MCLA_PRE);
	*umbc::uLog::log << "     exp>> " << (*elI)->describe() << endl;
	*umbc::uLog::log << "     OV >> " << ov2string() << endl;	
	*umbc::uLog::log << "  mcl.breakOnViolation="
			 << settings::describeSystemProp("mcl.breakOnViolation") 
			 << endl;
	umbc::uLog::annotateEnd(MCLA_PRE);
	umbc::uLog::annotateEnd(MCLA_VIOLATION);
	if (settings::getSysPropertyBool("mcl.breakOnViolation",false)) {
	  exceptions::signal_exception("mcl.breakOnViolation=true exception");
	}
      }
      // this is where the frame will go...
      // check to see if we are re-entering on a response
      // <!!!>

      if (m == NULL){
	// diagnostic
	printf("** eGroup @ 0x%8lx key=0x%8lx\n",(unsigned long)eGrp,
	       eGrp->get_egKey());
	dump_eg_table();
	// create a new frame and store it
	m = new mclFrame(this,eGrp->get_egKey(),eGrp->get_parent_egKey());
	frames.push_back(m);
	// dynamic link / intrinsic activation
	linkToIndicationFringe(m,*elI);
	resVec->push_back(m);
	rvc++;
	rv=true;
      }
      else {
	m->note_refail();
	// decide what to do for re-entrant behavior
	switch (DEFAULT_REB) {
	case DEFAULT_REB_IGNORE:
	  umbc::uLog::annotate(MCLA_WARNING,"[mcl/mcl]::WARNING ~> re-entering frame but default behavior is to ignore existing frames!");
	  m->note_refail();
	  break;
	case DEFAULT_REB_REENTER:
	  m->reEnterFrame(*elI,false,REENTRY_CODE_RECURRENCE);
	  safeFramePush(m,resVec);
	  rvc++;
	  rv=true;
	  break;
	case DEFAULT_REB_NEWFRAME:
	  m = new mclFrame(this,eGrp->get_egKey(),eGrp->get_parent_egKey());
	  safeFramePush(m,&frames);
	  // dynamic link / intrinsic activation
	  linkToIndicationFringe(m,*elI);
	  safeFramePush(m,resVec);
	  rvc++;
	  rv=true;
	  break;
	}
      }
    }
   }
   return rv;
}

frameVec* mcl::note() { 
  umbc::uLog::annotate(MCLA_MSG,"[mcl/note]::entering note phase");
  sprintf(umbc::uLog::annotateBuffer,"[mcl/note]::received %ld pending frame(s)",
	  (long)pendingFrames.size());
  umbc::uLog::annotateFromBuffer(MCLA_MSG);
		     
  frameVec* rvec = new frameVec;
  
  // move pending frames to rvec (and frames, if necessary)
  // this has the effect of "waking up" frames without actually
  // going through the note process. 
  // these frames will be automatically pushed through assess and guide
  for (frameVec::iterator pfi = pendingFrames.begin();
       pfi != pendingFrames.end();
       pfi++) {
    safeFramePush(*pfi,&frames);
    safeFramePush(*pfi,rvec);
    sprintf(umbc::uLog::annotateBuffer,"[mcl/note]::pending frame %s ... framecount(%ld) rvcount(%ld)",(*pfi)->entityName().c_str(),(long)frames.size(),(long)rvec->size());
    umbc::uLog::annotateFromBuffer(MCLA_MSG);
  }
  pendingFrames.clear();

  // run through new expectation groups
  for (egMap::iterator egI=expGroups.begin();egI!=expGroups.end();egI++) {
    // run through expectations
    mclExpGroup *tg = *egI;
    if (tg != NULL) {
      // make a frame for each new violation
      noteForExpGroup(rvec,tg,false);
    }
  }

  return rvec; 
}

bool mcl::assess(mclFrame *m) { 
  umbc::uLog::annotate(MCLA_MSG,"[mcl/assess]::entering assess phase");

  mclOntology *fo = m->getFailureCore();

  // should compute frame properties and possibly do merging
  
  mclNode* mpn=fo->maxPNode();

  if (mpn) {
    sprintf(umbc::uLog::annotateBuffer,"[mcl/assess]:: most probable node is %s, [def]p=%.2f",mpn->entityBaseName().c_str(), mpn->p_true());
    umbc::uLog::annotateFromBuffer(MCLA_MSG);
  }
  else {
    umbc::uLog::annotate(MCLA_WARNING,"[mcl/assess]::no maximum P node?");
  }

  return true; 
}

mclMonitorResponse* mcl::guide(mclFrame* m) { 
  umbc::uLog::annotate(MCLA_MSG,"[mcl/guide]::entering guide for "
		      + m->entityName());

  // now either make a recommendation or don't

  return m->generateResponse();

}  

float mcl::sensorValue(string selfSensorName) {
  return my_ov->v_double(selfSensorName);
}

float mcl::sensorValue(string objName, string obsName) {
  return my_ov->v_double(objName,obsName);
}

float mcl::sensor_value_pp(string sName) {
  return my_ov->v_double_pp(sName);
}

float mcl::sensorValue(int sn) {
  umbc::exceptions::signal_exception("sensorValue(int) is no longer supported.");
  #ifdef SAFE
  if (sv == NULL)
    umbc::exceptions::signal_exception("Sensor vector for "+entityName()+" not initialized (NULL).");
  #endif
  return sv[sn];
}

void mcl::setSV(float *svIN,int len) {
  umbc::exceptions::signal_exception("setSV(float*,int) is no longer supported.");
  if (len != svl) {
    if (sv != NULL) delete[] sv;
    sv  = new float[len];
    svl = len;
  }
  memcpy(sv,svIN,sizeof(float)*len);
}

mclSensorDef *mcl::getSensorDef(string name) {
  return my_ov->get_sensorDef_pp(name);
  /*
  for (sdVec::iterator sdvi=sensorDefs.begin();
       sdvi != sensorDefs.end();
       sdvi++)
    if ((*sdvi)->matchesName(name))
      return *sdvi;
  return NULL;
  */
}

////////////////

void mcl::set_noise_profile(string selfOname,spvType npKey) {
  noiseProfile* theNP = noiseProfile::createNoiseProfile(npKey);
  my_ov->establish_np(selfOname,theNP);
  set_obs_prop_self(selfOname,PROP_NOISEPROFILE,npKey);
}

void mcl::set_noise_profile(string selfOname,spvType npKey,double param) {
  noiseProfile* theNP = noiseProfile::createNoiseProfile(npKey,param);
  my_ov->establish_np(selfOname,theNP);
  set_obs_prop_self(selfOname,PROP_NOISEPROFILE,npKey);
}

void mcl::set_noise_profile(string objnm,string obsnm,spvType npKey) {
  noiseProfile* theNP = noiseProfile::createNoiseProfile(npKey);
  my_ov->establish_np(objnm,obsnm,theNP);
  set_obs_prop(objnm,obsnm,PROP_NOISEPROFILE,npKey);
}

void mcl::set_noise_profile(string objnm,string obsnm,
			    spvType npKey,double param) {
  noiseProfile* theNP = noiseProfile::createNoiseProfile(npKey,param);
  my_ov->establish_np(objnm,obsnm,theNP);
  set_obs_prop(objnm,obsnm,PROP_NOISEPROFILE,npKey);
}

////////////////

mclHIADef *mcl::getHIADef(string name) {
  for (HIAVec::iterator hdvi=HIAdefs.begin();
       hdvi != HIAdefs.end();
       hdvi++)
    if ((*hdvi)->matchesName(name))
      return *hdvi;
  return NULL;
}

mclMonitorResponse *mcl::activateNodeDirect(string name,resRefType referent) {
  // establish a frame
  bool newFrame=false;
  mclFrame* thisFrame=referent2frame(referent);
  if (thisFrame==NULL) {
    thisFrame = new mclFrame(this,EGK_NO_EG,RESREF_NO_REFERENCE);
    thisFrame->setEntityName("hii-"+name+"-frame");
    newFrame  = true;
  }
  
  // output
  umbc::uLog::annotate(MCLA_MSG,"[mcl/mcl]::node "+ name +
		      " activated direct from host ~~ " +
		      thisFrame->entityName());

  // find named node
  mclNode* thisNode = thisFrame->findNamedNode(name);
  if (thisNode == NULL) {
    mclMonitorResponse *r = new mclInternalErrorResponse();
    r->setResponse("could not find node "+name);
    if (newFrame) 
      delete thisFrame;
    // could go this way....
    // else thisFrame->setState(FRAME_ERROR);
    return r;
  }

  // bookkeeping
  if (newFrame) safeFramePush(thisFrame,&frames);
  // thisNode->assertTrue();
  thisNode->set_evidence(true);

  // now we should do assess then guide
  assess(thisFrame);
  mclMonitorResponse *r = guide(thisFrame);
  return r;

}

mclMonitorResponse *mcl::signalHIA(string name,resRefType referent) {
  mclHIADef * hia = getHIADef(name);
  if (hia == NULL) 
    return activateNodeDirect(name,referent);
  else
    return activateNodeDirect(hia->targetNodeName(),referent);
}

mclMonitorResponse *mcl::signalHIA(string name) {
  return signalHIA(name,RESREF_NO_REFERENCE);
}

mclFrame* mcl::referent2frame(resRefType r) {
  for (frameVec::iterator fvi = frames.begin();
       fvi != frames.end();
       fvi++) {
    if (r == (*fvi)->frame_id)
      return (*fvi);
  }
  return NULL;
}

mclFrame* mcl::recoverExistingFrame(resRefType r,egkType e,egkType p) {
  {
    mclFrame* rv = referent2frame(r);
    if (rv) {
      if (!umbc::settings::quiet)
	umbc::uLog::annotate(MCLA_MSG,"[mcl/mcl]::recovering frame by direct reference @" + (rv)->entityName());
      return (rv);
    }
  }
  for (frameVec::iterator fvi = frames.begin();
       fvi != frames.end();
       fvi++) {
    if ((*fvi)->get_vegKey() == e) {
      umbc::uLog::annotate(MCLA_MSG,"[mcl/mcl]::recovering frame by similarity @" + (*fvi)->entityName());
      return (*fvi);
    }
  }
  umbc::uLog::annotate(MCLA_MSG,"[mcl/mcl]::no existing frames match incoming case.");
  return NULL;
}

void mcl::deleteExpGroup(mclExpGroup* egp) {
  if (egp != NULL)
    delete egp;
  egMap::iterator egpi = find(expGroups.begin(),expGroups.end(),egp);
  expGroups.erase(egpi);
}

void mcl::deleteMarkedGroups() {
  egMap deleteds;
  for (egMap::iterator egI=expGroups.begin();egI!=expGroups.end();egI++) {
    // run through expectations
    mclExpGroup *tg = (*egI);
    if ((tg != NULL) && (tg->markedForDelete()))
      deleteds.push_back(tg);
  }
  while (!deleteds.empty()) {
    deleteExpGroup(deleteds.back());
    deleteds.pop_back();
  }
}

void mcl::markExpGroupForDelete(egkType eg_key) {
  mclExpGroup* egp = getExpGroup(eg_key);
  if (egp != NULL)
    egp->markDelete();
}

void mcl::markExpGroupForDelete(mclExpGroup* egp) {
   if (egp != NULL)
      egp->markDelete();
}

void mcl::expectationGroupAborted(egkType eg_key) {
  mclExpGroup* egrp = getExpGroup(eg_key);
  sprintf(umbc::uLog::annotateBuffer,"[mcl/mcl]::expectation group 0x%lx is declared aborted.",(unsigned long)eg_key);
  umbc::uLog::annotateFromBuffer(MCLA_MSG);

  markExpGroupForDelete(egrp);
  // we don't do any of the effects checking or anything...
  // but should we???
}

void mcl::expectationGroupComplete(egkType eg_key) {
  mclExpGroup* egrp = getExpGroup(eg_key);

  markExpGroupForDelete(egrp);

  sprintf(umbc::uLog::annotateBuffer,"[mcl/mcl]::expectation group 0x%lx is declared complete.",(unsigned long)eg_key);
  umbc::uLog::annotateFromBuffer(MCLA_MSG);

  // okay, we have to do two things:
  // (1) check for effects violations on the way out
  // (2) check for frame recovery so we can update state/status

  if (egrp == NULL) {
    // this has to be a check/logic error on the host side...
    sprintf(umbc::uLog::annotateBuffer,"[mcl/mcl]::expectation group 0x%lx is declared specified as complete but cannot be found.",(unsigned long)eg_key);
    umbc::uLog::annotateFromBuffer(MCLA_WARNING);
    return;
  }
  else {
    frameVec* newFrameVec = &pendingFrames;
    if (noteForExpGroup(newFrameVec,egrp,true)) {
      // a new violation occurred...
      
    }
    else {
      // the frame exited peacefully, we basically want to try to 
      // recover a frame and update its success field if one exists
      mclFrame* rec_frame = recoverExistingFrame(egrp->get_referent(),
						 egrp->get_egKey(),
						 egrp->get_parent_egKey());
      if (rec_frame == NULL) {
	umbc::uLog::annotate(MCLA_MSG,"[mcl/mcl]:: no relevant frame exists on eGrp complete...");
      }
      else {
	// if indirect reference, note success
	rec_frame->note_success();
	umbc::uLog::annotate(MCLA_MSG,"[mcl/mcl]:: frame recovered and success noted...");
	maybeRetireFrame(rec_frame);
      }
    }
  }
}

void mcl::declareExpectationGroup(egkType key,
				  egkType parent,
				  resRefType ref) {
  mclExpGroup* eeg = getExpGroup(key);
  if ((eeg != NULL) && !eeg->markedForDelete()) {
    char pe[512];
    sprintf(pe,"Multiply defined expectation group @0x%08lx",(unsigned long)key);
    umbc::exceptions::signal_exception(pe);
  }
  else {
    if (eeg !=NULL)
      umbc::uLog::annotate(MCLA_MSG,"[mcl/mcl]::Redefinition of EG okay because old one is completing.");
    addExpGroup(key,new mclExpGroup(this,key,parent,ref));
  }
}

mclExpGroup* mcl::getExpGroup(egkType key, bool returnMarkedGroups) {
  for (egMap::iterator egi = expGroups.begin();
       egi != expGroups.end();
       egi++) {
    if (((*egi)->get_egKey() == key) && 
	(returnMarkedGroups || !(*egi)->markedForDelete()))
      return (*egi);
  }
  return NULL;
}

void mcl::dump_eg_table() {
  for (egMap::iterator egi = expGroups.begin();
       egi != expGroups.end();
       egi++) {
    if ((*egi)->markedForDelete())
      printf("[X] ");
    else printf("[ ]");
    printf(" *eGrp=0x%8lx eGrp->key=0x%8lx\n",(unsigned long)(*egi),
	   (unsigned long)(*egi)->get_egKey());
  }
}

void mcl::maybeRetireFrame(mclFrame* the_frame) {
  // decide whether or not to throw out the frame
}

void mcl::safeFramePush(mclFrame* nf,frameVec* fv) {
  if (find(fv->begin(),fv->end(),nf) == fv->end())
    fv->push_back(nf);
}

void mcl::pending(mclFrame* nf) {
  safeFramePush(nf,&pendingFrames);
}

void mcl::dumpMostRecentFrame() {
   mclFrame *mrf = frames.front();
   if (mrf != NULL) {
      mrf->dumpEntity(&cout);
   }  
}

void mcl::processOffCycleFailure(mclFrame* m) {
  sprintf(umbc::uLog::annotateBuffer,"[mcl/mcl]::the host has signaled a response failure for frame 0x%lx", (long unsigned int)m);
  umbc::uLog::annotateFromBuffer(MCLA_MSG);
  m->reEnterFrame(NULL,true,REENTRY_CODE_FAILED);
}

void mcl::processResponseIgnored(mclFrame* m) {
  sprintf(umbc::uLog::annotateBuffer,"[mcl/mcl]::the host has signaled a response was ignored for frame 0x%lx", (long unsigned int)m);
  umbc::uLog::annotateFromBuffer(MCLA_MSG);
  m->reEnterFrame(NULL,true,REENTRY_CODE_IGNORED);
}

void mcl::processHostFeedback(bool hostReply, mclFrame* frame) { 
  frame->processFeedback(hostReply); 
  pending(frame);
  umbc::uLog::annotate(MCLA_MSG,
		      "[mcl/mcl]:: the following frame is now pending: " + 
		      frame->entityBaseName());
  // *umbc::uLog::annotate(MCLA_MSG,"[mcl/mcl]::"+pendingFrames.size()+
  // " frame(s) are pending.");
}

void mcl::dumpFrameDot(int which) {
  if (which >= frames.size() || (which < 0)) 
    *umbc::uLog::log << "[mcl/mcl]::illegal frame index " << which << endl;
  else {
    mclFrame* x = frames[which];
    writeAll("dumps/"+ontology_configurator::cKey2path(cKey)+".dot",x);
  }
}
