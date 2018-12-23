#include "oNodes.h"
#include "mclFrame.h"

/******************************************  basic stuff */

double mclConcreteResponse::cost() { return cost_static; };

// mclMonitorResponse* mclConcreteResponse::node2response(mclFrame *f) { 
//   return NULL; 
// }

/******************************************  interactive stuff */

bool mcl_AD_InteractiveResponse::interpretFeedback(bool feedback) {
  for (vector<string>::iterator pi = positiveList.begin();
       pi != positiveList.end();
       pi++) {
    mclNode* pnode = myFrame()->findNamedNode(*pi);
    if (pnode == NULL) {
      signalMCLException("tried to find node "+*pi+" in interactive response but it does not exist.");
    }
    else if (feedback) {
      mclLogger::annotate(MCLA_MSG,"[mcl/oNode]::asserting A/D+ dependency "+*pi+" set to true.");
      pnode->set_evidence(true);
    }
    else {
      mclLogger::annotate(MCLA_MSG,"[mcl/oNode]::asserting A/D+ dependency "+*pi+" set to false.");
      pnode->set_evidence(false);
    }
  }
  for (vector<string>::iterator ni = negativeList.begin();
       ni != negativeList.end();
       ni++) {
    mclNode* nnode = myFrame()->findNamedNode(*ni);
    if (nnode == NULL)
      signalMCLException("tried to find node "+*ni+" in interactive response but it does not exist.");
    else if (feedback) {
      mclLogger::annotate(MCLA_MSG,"[mcl/oNode]::asserting A/D- dependency "+*ni+" set to false.");
      nnode->set_evidence(false);
    }
    else {
      mclLogger::annotate(MCLA_MSG,"[mcl/oNode]::asserting A/D- dependency "+*ni+" set to true.");
      nnode->set_evidence(true);
    }
  }
  return (true && mclInteractiveResponse::interpretFeedback(feedback));
}

/******************************************  DOT file implementations */

string mclHostProperty::dotDescription() {
  return dotLabel()+" [shape=diamond,color=sienna,fontcolor=sienna];";
}

string mclHostInitiatedIndication::dotDescription() {
  return dotLabel()+" [shape=rectangle,style=filled,fillcolor=sienna];";
}

string mclConcreteIndication::dotDescription() {
  return dotLabel()+" [shape=ellipse,color=sienna,fontcolor=sienna];";
}

string mclGeneralIndication::dotDescription() {
  return dotLabel()+" [shape=ellipse,color=purple,fontcolor=purple];";
}

string mclIndicationCoreNode::dotDescription() {
  return dotLabel()+" [shape=ellipse,color=purple,peripheries=2,fontcolor=purple];";
}

string mclFailure::dotDescription() {
  return dotLabel()+" [shape=ellipse,color=red,fontcolor=red];";
}

string mclGeneralResponse::dotDescription() {
  return dotLabel()+" [shape=ellipse,color=green,fontcolor=green];";
}

string mclConcreteResponse::dotDescription() {
  return dotLabel()+" [shape=rectangle,color=sienna,fontcolor=sienna];";
}
