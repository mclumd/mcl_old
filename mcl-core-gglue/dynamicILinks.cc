#include "mcl.h"
#include "mclNode.h"
#include "mclExceptions.h"
#include "mclLogger.h"

/** \file
 * contains the functionality for linking expectation violations into the MCL ontology fringe.
 */

////
///  this function links an expectation into from a frame's indications
////

void dumpLTS(list<string> &linkage) {
  *mclLogger::mclLog << " fringe linkage: <";
  for (list<string>::iterator sti = linkage.begin();
       sti != linkage.end();
       sti++) 
    *mclLogger::mclLog << " " << *sti;
  *mclLogger::mclLog << " >" << endl;
}

void mcl::linkToIndicationFringe(mclFrame *f,mclExp *e) {

  // phase 1: ask for linkage from the expectation
  list<string> linkage = e->generateLinkTags();

  mclLogger::annotateStart(MCLA_PRE);
  *mclLogger::mclLog << "[mcl/ilinks]::";
  dumpLTS(linkage);
  mclLogger::annotateEnd(MCLA_PRE);

  // phase 2: activate the named linkages
  mclOntology *io = f->getIndicationCore();
  for (list<string>::iterator sti = linkage.begin();
       sti != linkage.end();
       sti++) {
    // cout << "fringe linkage to " << *sti << endl;
    mclNode *nta = io->findNamedNode(*sti);
    if (nta == NULL) {
      signalMCLException("failure to dynamically link to indication node \""+*sti+"\"");
    }
    else {
      nta->set_evidence(true);
      // nta->assertTrue();
    }
  }

}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

// utility

string sc_to_fringe[] = {
  "state","control","spatial","temporal","resource","reward","ambient",
  "objectprop","message","counter","unspecified_sc" };

string scNodeLookup(spvType scCode) {
  if (scCode >= SC_NUMCODES_LEGAL) {
    signalMCLException("Illegal sensorCode->indicationFringe lookup (out of range).");
    return "";
  }
  else 
    return sc_to_fringe[scCode];
}

// here are the internal linkage generators for the individual 
// expectations

list<string> mclTickExp::generateLinkTags() {
  list<string> rv;
  rv.push_back("temporal");
  rv.push_back("late");
  return rv;
}

list<string> mclRealTimeExp::generateLinkTags() {
  list<string> rv;
  rv.push_back("temporal");
  rv.push_back("late");
  return rv;
}

list<string> mclPollAndTestExp::generateLinkTags() {
  // cout << "polltest generating linkage tags." << endl;
  list<string > mrv;

  // get the sensor properties and push them onto the list...
  mcl *thisMCL = MCL();
  mclSensorDef *esd = thisMCL->getSensorDef(sname);

  // sensor class
  string scs = scNodeLookup(esd->getSensorProp(PROP_SCLASS));
  if (scs.length() > 0)
    mrv.push_back(scs);

  // value class
  //   if (esd->testSensorProp(PROP_DT,DT_RATIONAL) ||
  //       esd->testSensorProp(PROP_DT,DT_INTEGER))
  //     mrv.push_back("ordinal");
  //   else
  //     mrv.push_back("discrete");

  // maintenance has been removed... temporarily?
  // mrv.push_back("maintenance");

  return mrv;
}

list<string> mclPollOnExitExp::generateLinkTags() {
  // cout << "pollExit generating linkage tags." << endl;
  list<string > mrv;

  // get the sensor properties and push them onto the list...
  mcl *thisMCL = MCL();
  mclSensorDef *esd = thisMCL->getSensorDef(sname);

  // sensor class
  string scs = scNodeLookup(esd->getSensorProp(PROP_SCLASS));
  if (scs.length() > 0)
    mrv.push_back(scs);

  // value class
  //   if (esd->testSensorProp(PROP_DT,DT_RATIONAL) ||
  //       esd->testSensorProp(PROP_DT,DT_INTEGER))
  //     mrv.push_back("ordinal");
  //   else
  //     mrv.push_back("discrete");

  // mrv.push_back("effect");
  return mrv;
}

list<string> mclTakeValueExp::generateLinkTags() {
  list<string> crv = mclPollOnExitExp::generateLinkTags();
  float csv  = itsMCL->sensorValue(sname);
  if (csv > target)
    crv.push_back("long-of-target");
  else 
    crv.push_back("short-of-target");
  // now check to see if the sensor never changed...
  if (initial == csv)
    crv.push_back("missed-unchanged");
  return crv;
}

list<string> mclAnyNetChangeExp::generateLinkTags() {
  list<string> crv = mclPollOnExitExp::generateLinkTags();
  crv.push_back("missed-unchanged");
  return crv;
}

list<string> mclGoUpExp::generateLinkTags() {
  list<string> crv = mclPollOnExitExp::generateLinkTags();
  float csv  = itsMCL->sensorValue(sname);
  bool unch = (csv == initial);
  if (unch)
    crv.push_back("missed-unchanged");
  else 
    crv.push_back("missed-wrongway");
  return crv;
}

list<string> mclGoDownExp::generateLinkTags() {
  list<string> crv = mclPollOnExitExp::generateLinkTags();
  float csv  = itsMCL->sensorValue(sname);
  bool unch = (csv == initial);
  if (unch)
    crv.push_back("missed-unchanged");
  else 
    crv.push_back("missed-wrongway");
  return crv;
}

list<string> mclNoNetChangeExp::generateLinkTags() {
  list<string> crv = mclPollOnExitExp::generateLinkTags();
  float csv  = itsMCL->sensorValue(sname);
  bool tlong = (csv > initial);
  if (tlong)
    crv.push_back("cwa-increase");
  else
    crv.push_back("cwa-decrease");
  return crv;
}

list<string> mclLowerBoundExp::generateLinkTags() {
  // cout << "lowerbound generating linkage tags." << endl;
  list<string> crv = mclPollAndTestExp::generateLinkTags();
  crv.push_back("breakout-low");
  return crv;
}

list<string> mclUpperBoundExp::generateLinkTags() {
  // cout << "upperbound generating linkage tags." << endl;
  list<string> crv = mclPollAndTestExp::generateLinkTags();
  crv.push_back("breakout-high");
  return crv;
}

list<string> mclMaintainValueExp::generateLinkTags() {
  // cout << "upperbound generating linkage tags." << endl;
  list<string> crv = mclPollAndTestExp::generateLinkTags();
  float csv  = itsMCL->sensorValue(sname);
  bool tlong = (csv > target);
  if (nochange) {
    // it's an "abberation" (not supposed to change but it did)
    if (tlong)
      crv.push_back("cwa-increase");
    else
      crv.push_back("cwa-decrease");
  }
  else {
    if (initial == csv)
      crv.push_back("missed-unchanged");
    else if (tlong)
      crv.push_back("long-of-target");
    else
      crv.push_back("short-of-target");
  }
  return crv;
}

list<string> mclDelayedMaintenanceExp::generateLinkTags() {
  list<string> crv = sub->generateLinkTags();
  crv.push_back("temporal");
  return crv;
}
