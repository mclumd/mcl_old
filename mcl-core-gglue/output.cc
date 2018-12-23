#include "output.h"
#include <list>
#include <algorithm>

void DOL(ostream *o,mclOntology *mo,mclNode *tn,list<mclNode *> *xl,int indent) {
  list<mclNode *>::iterator fixl = find(xl->begin(),xl->end(),tn);
  if (fixl == xl->end()) {
    for (int i=0;i<indent;i++)
      *o << "~";
    tn->dumpEntity(o);
    // *o << endl;
    xl->push_back(tn);
    // now check links...
    for (linkList::iterator ios = tn->outLink_begin();
	 ios != tn->outLink_end();
	 ios++) {
      DOL(o,mo,(*ios)->destinationNode(),xl,indent + 1);
    }
  }
}

void dumpOntology(ostream *o,mclOntology *mo) {
  list<mclNode *> xcl;
  for (list<mclNode *>::iterator ni = mo->firstNode();
	ni != mo->endNode();
	ni++) {
    DOL(o,mo,*ni,&xcl,1);
  }
}
