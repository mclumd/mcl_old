#include "makeDot.h"
#include "mcl.h"
#include "mclSettings.h"
#include "mclFrame.h"
#include "oNodes.h"
#include "linkFactory.h"

#include <iostream>
#include <list>

void writeOntology(mclOntology *m) {
  writeOntology(m->entityBaseName()+".dot",m);
}

void writeOntology(string filename,mclOntology *m) {
  string is = ontology_configurator::autoPath(m,filename);
  string os = is+".bak";
  cout << "moving old dot file to " << os << endl;
  ifstream i(is.c_str(),ios_base::in|ios_base::binary);
  ofstream o(os.c_str(),ios_base::out|ios_base::binary);
  o << i.rdbuf();  

  cout << "writing new dot file to " << is << endl;
  ofstream no(is.c_str(),ios_base::out);
  no << "digraph " << m->entityBaseName() << " {" << endl;
  no << "  size=\"8,10\"" << endl;

  // start with nodes...
  for (nodeList::iterator ni1 = m->firstNode();
       ni1 != m->endNode();
       ni1++) {
    no << "  " << (*ni1)->dotDescription() << endl;
  }

  // then add links...
  for (nodeList::iterator ni2 = m->firstNode();
       ni2 != m->endNode();
       ni2++) {
    for (llIterator ogli = (*ni2)->inLink_begin();
	 ogli != (*ni2)->inLink_end();
	 ogli++) {
      no << "  " << (*ogli)->dotDescription() << endl; 

    }
  }

  no << "}" << endl;
}

void writeAll(mclFrame *f) {
  writeAll("mclOntologies.dot",f);
}

void writeAll(string filename,mclFrame *f) {
  string is = ontology_configurator::autoPath(f,filename);
  string os = is+".bak";
  cout << "moving old dot file to " << os << endl;
  ifstream i(is.c_str(),ios_base::in|ios_base::binary);
  ofstream o(os.c_str(),ios_base::out|ios_base::binary);
  o << i.rdbuf();  

  cout << "writing new dot file to " << is << endl;
  ofstream no(is.c_str(),ios_base::out);
  no << "digraph MCL {" << endl;
  no << "  size=\"16,20\"" << endl;
  // no << "  rankdir=LR" << endl;
  no << "  compound=true" << endl;

  // start with nodes...
  for (int oi = 0;oi < 3;oi++) {
    mclOntology* m = f->getCoreOntology(oi);
    no << "  subgraph cluster_" << m->entityBaseName() << " {" << endl;
    no << "    label = \"" << m->entityBaseName() << "\";" << endl;
    for (nodeList::iterator ni1 = m->firstNode();
	 ni1 != m->endNode();
	 ni1++) {
      // cout << "[writing " << (*ni1)->className() << ":" 
      // << (*ni1)->entityBaseName() << "]" << endl;
      no << "    " << (*ni1)->dotDescription() << endl;
    }
    no << "  }" << endl;
  }

  // then add links...
  for (int oi = 0;oi < 3;oi++) {
    mclOntology* m = f->getCoreOntology(oi);
    for (nodeList::iterator ni2 = m->firstNode();
	 ni2 != m->endNode();
	 ni2++) {
      for (llIterator ogli = (*ni2)->inLink_begin();
	   ogli != (*ni2)->inLink_end();
	   ogli++) {
	no << "  " << (*ogli)->dotDescription() << endl; 
	
      }
    }
  }

  no << "}" << endl;
}

#ifdef EXECUTABLE
int main(int argc, char **argv) {
  if (argc > 1)
    cfg::loadConfig(argv[1]);
  else cfg::loadConfig("default");
  mclSettings::quiet = true;
  mcl         *m = new mcl();
  mclFrame    *f = new mclFrame(m);
  
  writeOntology(f->getIndicationCore());
  writeOntology(f->getFailureCore());
  writeOntology(f->getResponseCore());

  writeAll(f);

}
#endif
