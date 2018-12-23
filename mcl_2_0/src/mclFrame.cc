#include "mclFrame.h"
#include "ontologyRecord.h"
#include "ontology_reader.h"
#include "oNodes.h"
#include "mcl.h"
#include "makeDot.h"
#include "mclLogging.h"
#include "umbc/settings.h"
#include "umbc/text_utils.h"
#include "umbc/exceptions.h"

const string THIS_FILE = "[mcl/frm]:: ";

using namespace metacog;

resRefType mclFrame::FRAME_ID_COUNTER = 1;

mclConcreteResponse *selectConcreteNodeCBA(mclFrame *f);

mclFrame::mclFrame(mcl *m,egkType veg,egkType veg_parent) : 
  mclEntity(),core(NULL),myMCL(m),state(FRAME_NEW),
  active_response(NULL),veg_key(veg),veg_parent_key(veg_parent),
  refails(0),successes(0),last_update(0),dirty(false),
  default_glue_key(glueFactory::getDefaultGlueKey()) { 
  // nervous about initFrame() as it didn't seem to be there....
  initFrame();
  frame_id = (FRAME_ID_COUNTER++);
  // the frame gets a copy of the property vector associated with either...
  // ... the violated expectation group
  if (veg_key) {
    if (m->getExpGroup(veg_key,true)) 
      m->getExpGroup(veg_key,true)->getPV()->copyValues(myPV);
    else {
      umbc::exceptions::signal_exception("creating mclFrame with missing but supplied VEG key.");
    }
  }
  // ... or the default MCL property vector (in the case of an HII)
  else {
    // this will happen with HIIs
    m->getDefaultPV()->copyValues(myPV);
  }
}

mclMonitorResponse *mclFrame::generateResponse() {
  mclConcreteResponse *maxnode=selectConcreteNodeCBA(this);
  if (maxnode == NULL) {
    setState(FRAME_DEADEND);
    sprintf(umbc::uLog::annotateBuffer,
	    "Frame %s has entered the DEAD_END state.",
	    entityName().c_str());
    umbc::uLog::annotateFromBuffer(MCLA_WARNING);
    return new mclMonitorNoMoreOptions(frame_id,veg_key);
  }
  else {
    // eventually...
    // mclMonitorResponse *mmr1 = maxnode->node2response(this);
    mclMonitorResponse *mmr = 
      new mclMonitorCorrectiveResponse(frame_id,veg_key,maxnode->respCode());
    // mmr->setResponse("MCL recommendation is to "+respName(maxnode->respCode()));
    bool none,mult;
    string restr=symbols::reverse_lookup("crc",maxnode->respCode(),&none,&mult);
    char x[32];
    sprintf(x,"%d",maxnode->respCode());
    string xs = x;
    if (none)
      mmr->setResponse("MCL response = ("+
		       xs+","+
		       maxnode->entityBaseName()+","+
		       "?unknown response?)");
    else if (mult)
      mmr->setResponse("MCL response = ("+
		       xs+","+
		       maxnode->entityBaseName()+","+
		       restr+")");
    else
      mmr->setResponse("MCL response = ("+
		       xs+","+
		       maxnode->entityBaseName()+","+
		       restr+")");
      
    setState(FRAME_RESPONSE_ISSUED);
    set_active_response(maxnode);
    if (active_response != NULL)
      umbc::uLog::annotate(MCLA_MSG,
			   "[mcl/mclFrame]:: set active response to "+
			   active_response->entityName());
    else
      umbc::uLog::annotate(MCLA_MSG,"[mcl/mclFrame]:: active response somehow not set...");
    // this uses the Dot/dump feature in writeAll for diagnostics
    if (umbc::settings::getSysPropertyBool("mcl.logFrames",false)) {
      char b[12];
      sprintf(b,"-%d-%d",refails,successes);
      writeAll("dumps/"+entityName()+b+".dot",this);
    }
    return mmr;
  }
}

mclConcreteResponse *selectConcreteNodeCBA(mclFrame *f) {
  mclOntology *ro = f->getResponseCore();
  mclConcreteResponse *bn = NULL;
  double   cb = 0.0001;
  for (list<mclNode *>::iterator ni = ro->firstNode();
       ni != ro->endNode();
       ni++) {
    mclConcreteResponse *mcr = dynamic_cast<mclConcreteResponse *>(*ni);
    if (mcr != NULL) {
      sprintf(umbc::uLog::annotateBuffer,
	      "[mcl/mclFrame]:: %s p=%.2f cost=%.2f u=%.2f",
	      mcr->entityName().c_str(), mcr->p_true(), mcr->cost(),
	      (mcr->p_true() / mcr->cost()));
      umbc::uLog::annotateFromBuffer(MCLA_VRB);
      if ((mcr->p_true() / mcr->cost()) > cb) {
	bn=mcr;
	cb=mcr->p_true() / mcr->cost();
      }
    }
  }
  // will return NULL if nothing in the failure ontology is active
  return bn;
}

void mclFrame::dumpEntity(ostream* o) {
  *o << "*********************************" << endl;
  *o << entityName() << ": state=" << getState() << endl;
  *o << "ACTIVE RESPONSE @" << active_response << endl;
  *o << "INDICATIONS :" << endl;
  getIndicationCore()->dumpEntity(o);
  *o << "FAILURES    :" << endl;
  getFailureCore()->dumpEntity(o);
  *o << "RESPONSES   :" << endl;
  getResponseCore()->dumpEntity(o);
}

void mclFrame::reEnterFrame(mclExp* exp,bool hostInitiated,int entryCode) {
  sprintf(umbc::uLog::annotateBuffer,
	  "[mcl/mclFrame]:: re-entering frame @0x%lx state=0x%x entryCode=0x%x",
	  (unsigned long)this,(int)getState(),entryCode);
  umbc::uLog::annotateFromBuffer(MCLA_MSG);
  switch (entryCode) {

    // the frame has been recovered as a result of a recurrence of the original
    // violation.
  case REENTRY_CODE_RECURRENCE:
    // first what we'd normally do is figure out if the new violation is 
    // the same or related to the original/most-recent one
    // * here we assume that the violation is related and is a reattempt
    //   after a response (if possible)
    // - decide what to do with the new violation?
    if (exp != NULL) {
      MCL()->linkToIndicationFringe(this,exp);
    }
    // - decide whether or not to add the frame as pending
    if (active_response != NULL) {
      umbc::uLog::annotate(MCLA_MSG,"[mcl/mclFrame]:: frame re-entered with active response "+active_response->entityBaseName());
      sprintf(umbc::uLog::annotateBuffer,"frame statistics for %s = %s",
	      active_response->entityBaseName().c_str(),
	      active_response->countsAsString().c_str());
      umbc::uLog::annotateFromBuffer(MCLA_MSG);
      active_response->inc_failedAttempts();
      if (active_response->isNotWorking()) {
	active_response->set_evidence(false);
	umbc::uLog::annotate(MCLA_MSG,"[mcl/mclFrame]:: active response does not appear to be working.");
      }
      else {
	umbc::uLog::annotate(MCLA_MSG,"[mcl/mclFrame]:: active response has been granted a stay of execution.");
      }
    }
    // - decide whether or not the active_response is a failure
    MCL()->pending(this);
    break;

    // the frame has been recovered because the host indicated ignore
  case REENTRY_CODE_IGNORED:
    state=FRAME_RESPONSE_IGNORED;
    if (active_response != NULL) {
      // do we really want to totally zero this response?
      active_response->set_evidence(false);
      active_response->inc_ignores();
    }
    MCL()->pending(this);
    break;

    // the frame has been recovered because the host indicated response abort
  case REENTRY_CODE_ABORTED:
    state=FRAME_RESPONSE_ABORTED;
    if (active_response != NULL) {
      active_response->inc_abortedAttempts();
      // maybe? i dunno...
      active_response->set_evidence(false);
    }
    MCL()->pending(this);
    break;

    // the frame has been recovered because the host indicated response failure
  case REENTRY_CODE_FAILED:
    state=FRAME_RESPONSE_FAILED;
    note_refail(); // this is for framestate
    if (active_response != NULL) {
      active_response->inc_failedAttempts(); // this is nodestate
      // failures for durative responses are to be expected ...
      if (!active_response->isDurative() || hostInitiated) {
	umbc::uLog::annotate(MCLA_MSG,"[mcl/mclFrame]:: response has failed and is being suppressed.");
	active_response->set_evidence(false);
	MCL()->pending(this);
      }	
      else {
	umbc::uLog::annotate(MCLA_WARNING,"[mcl/mclFrame]:: re-enter/failure for durative repairs is not implemented (uh oh).");
      }
    }
    else
      umbc::uLog::annotate(MCLA_WARNING,"[mcl/mclFrame]:: in a bad state (re-enter, state=active, response=NULL).");
    break;

    // frame is being re-entered (somehow) because of a success
  case REENTRY_CODE_SUCCESS:
    state=FRAME_RESPONSE_SUCCEEDED;
    note_success(); // this is for framestate    
    break;

  default:
    umbc::uLog::annotate(MCLA_ERROR,"[mcl/mclFrame]:: frame re-entered with unhandled e-code: "+umbc::textFunctions::num2string(entryCode));
    break;
  }
}

mclNode** mclFrame::allNodes() {
  const int rvn = nodeCount();
  mclNode** rv = new mclNode *[rvn];
  int nc = 0;
  for (int q=0;q<numOntologies();q++) {
    mclOntology* o = getCoreOntology(q);
    for (nodeList::iterator oi = o->firstNode();
	 oi != o->endNode();
	 oi++) {
      rv[nc] = *oi;
      nc++;
    }
  }
  return rv;
}

vector<mclNode*> mclFrame::allNodesV() {
  vector<mclNode*> allMCLnodes;
  for (int oi = 0; oi < 3; oi++) {
    mclOntology* to = getCoreOntology(oi);
    for (nodeList::iterator toni = to->firstNode();
	 toni != to->endNode();
	 toni++) {
      allMCLnodes.push_back(*toni);
    }
  }
  return allMCLnodes;
}

mclNode* mclFrame::findNamedNode(string nodename) {
  mclNode* rv=NULL;
  rv=getIndicationCore()->findNamedNode(nodename);
  if (rv != NULL) return rv;
  rv=getFailureCore()->findNamedNode(nodename);
  if (rv != NULL) return rv;
  rv=getResponseCore()->findNamedNode(nodename);
  return rv;
}

void mclFrame::processFeedback(bool hostReply) {
  mclInteractiveResponse* arair = 
    dynamic_cast<mclInteractiveResponse*>(active_response);
  if (arair == NULL) {
    if (active_response == NULL)
      umbc::uLog::annotate(MCLA_ERROR,"[mcl/mclFrame]:: Error ?> failed to process feedback with no active response in Frame");
    else
      umbc::uLog::annotate(MCLA_ERROR,"[mcl/mclFrame]:: Error ?> failed to process feedback for non-interactive node "+active_response->entityName());
  }
  else {
    // this does the feedback part for the node...
    arair->interpretFeedback(hostReply);
    // now update the frame
    setState(FRAME_UPDATED);
    umbc::uLog::annotate(MCLA_MSG,"[mcl/mclFrame]:: feedback attributed to "+active_response->entityName()+" and frame/response are set to UPDATED.");
    set_active_response(NULL);
  }
}

#ifndef NO_DEBUG
void mclFrame::dbg_forceFrameState(string node,int state) {
  mclConcreteResponse* maxnode = 
    dynamic_cast<mclConcreteResponse*>(findNamedNode(node));
  if (maxnode == NULL)
    umbc::exceptions::signal_exception("DBG error:> "+node+" does not exist as concrete response.");
  else {
    setState(FRAME_RESPONSE_ISSUED);
    set_active_response(maxnode);
  }
}
#endif

void mclFrame::setCore(ontologyVector* nv) {
	if (core != NULL) {
		delete core;
		core=NULL;
	}
	core = nv;
}

void mclFrame::initFrame() {
  // core = mclGenerateOntologies(this);
  umbc::uLog::annotate(MCLA_MSG,THIS_FILE+"intializing frame from '"+MCL()->getOntologyBase()+"'");
	setCore(ontology_reader::read_ontologies(MCL()->getOntologyBase(),this));
  glueFactory::autoCreateGlue(this);
  if (umbc::settings::getSysPropertyBool("mcl.autoConfigCPTs",true)) {
    ontology_configurator::apply_config(this);
    // cfg::applyCPTconfig(this);
    // cfg::applyRCconfig(this);
  }
}

void mclFrame::record_ostate_if_dirty() {
  if (dirty) {
    dirty=false;
    ontologyRecord* orec = new ontologyRecord_map(this);
    ontology_records.push_back(orec);
  }
}
