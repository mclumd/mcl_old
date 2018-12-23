#include "output.h"
#include "mclOntology.h"
#include "mclFrame.h"
#include "mclExceptions.h"
#include "mcl.h"
#include "mclLogger.h"
#include "mclSettings.h"

mclNode *mclOntology::findNamedNode(string n) {
  for (list<mclNode *>::iterator ni = nodes.begin();
       ni != nodes.end();
       ni++) {
    if ((*ni)->matchesName(n))
      return (*ni);
  }
  return NULL;
}

mcl *mclOntology::MCL() { return myFrame()->MCL(); };

void mclOntology::addNode(mclNode *m) { 
  nodes.push_back(m); 
} 

void mclOntology::dumpEntity(ostream *strm) {
  dumpOntology(strm,this);
}

// void mclOntology::update() {
//   for (list<mclNode *>::iterator ni = nodes.begin();
//        ni != nodes.end();
//        ni++) {
//     (*ni)->ensure_();
//   }
// }

mclNode* mclOntology::maxPNode() {
  double   maxP=0;
  mclNode* maxN=NULL;
  for (list<mclNode *>::iterator ni = nodes.begin();
       ni != nodes.end();
       ni++) {
    /*
      mclLogger::annotate(MCLA_MSG,"[mcl/mclOntology]::computing p for " +
      (*ni)->entityName() +
      "...");
    */
    if ((*ni)->p_true() > maxP) {
      maxP=(*ni)->p_true();
      maxN=(*ni);
    }
  }
  return maxN;
}

void mclOntology::autoActivateProperties(mclPropertyVector& mpv) {
  for (nodeList::iterator nli = firstNode();
       nli != endNode();
       nli++) {
    mclHostProperty* tn = dynamic_cast<mclHostProperty*>(*nli); 
    if (tn != NULL) {
      string ooc;
      if (mpv.testProperty(tn->propCode(),PC_YES)) {
	// was (MCL()->testProperty(tn->propCode(),PC_YES))
	ooc = "off";
	tn->set_evidence(true);
      }	
      else {
	ooc = "off";
	tn->set_evidence(false);
      }
      sprintf(mclLogger::annotateBuffer,
	      "[mcl/ontology]::checking %s for prop (0x%x) activation...%s",
	      (*nli)->entityName().c_str(),
	      tn->propCode(),
	      ooc.c_str());
      mclLogger::annotateFromBuffer(MCLA_DBG);
    }
  }
}
