#include "ontology_config.h"
#include "textUtils.h"
#include "mclExceptions.h"
#include "oNodes.h"

#include <map>

#include <iostream>
#include <fstream>

using namespace std;

extern char mcl_perror[2048];
map<string,string> RCmap;

bool rc_cfg::load_response_costs(string path) {
  RCmap.clear();
  string conf_is = path+"/response_costs.mcl";
  // string conf_is= pathToConfigRead("response_costs.mcl");
  cout << "loading response node costs from " 
       << conf_is << "...";
  string conf_s = textFunctions::file2string(conf_is);
  bool running=true;
  tokenMachine level1(conf_s);
  while (running) {
    string table_s = level1.nextToken();
    if (table_s.size() == 0)
      running=false;
    else {
      tokenMachine level2(table_s);
      level2.trimParens();
      string node_s = textFunctions::dequote(level2.nextToken());
      RCmap[node_s]=table_s;
    }
  }
  cout << RCmap.size() << " costs loaded." << endl;
}

bool rc_cfg::save_response_costs(string path,mclFrame *f) {
  // backup
  string is = path+"/response_costs.mcl";
  // string is = pathToConfigWrite("response_costs.mcl");
  string os = is+".bak";
  cout << "moving old config file to " << os << endl;
  ifstream i(is.c_str(),ios_base::in|ios_base::binary);
  ofstream o(os.c_str(),ios_base::out|ios_base::binary);
  o << i.rdbuf();  

  cout << "writing new config file to " << is << endl;
  ofstream no(is.c_str(),ios_base::out);

  for (nodeList::iterator oni = f->getResponseCore()->firstNode();
       oni != f->getResponseCore()->endNode();
       oni++) {
    mclConcreteResponse* oniacr = dynamic_cast<mclConcreteResponse*>(*oni);
    if (oniacr != NULL) {
      no << "(\"" << (*oni)->entityBaseName() << "\" "
	 << oniacr->cost() << ")" << endl;
    }
  }
}

bool rc_cfg::apply_rc_config(mclFrame *f) {
  rc_cfg::apply_rc_config(f->getIndicationCore());
  rc_cfg::apply_rc_config(f->getFailureCore());
  rc_cfg::apply_rc_config(f->getResponseCore());
}

bool rc_cfg::apply_rc_config(mclOntology *o) {
  for (nodeList::iterator nli = o->firstNode();
       nli != o->endNode();
       nli++) {
    rc_cfg::apply_rc_config((*nli),RCmap[(*nli)->entityBaseName()]);
  }
}

bool rc_cfg::apply_rc_config(mclNode *n,string config) {
  if (config.size()==0) return true;
  mclConcreteResponse *ovon = dynamic_cast<mclConcreteResponse *>(n);
  if (ovon == NULL) {
    signalMCLException("Attempting to configure cost of "+n->baseClassName()+" "+n->entityBaseName());
  }
  else {
    tokenMachine ctm(config);
    ctm.trimParens();
    ctm.nextToken();
    double cost=textFunctions::dubval(ctm.nextToken());
    if (cost > 0)
      ovon->setCost(cost);
    else 
      cerr << "warning: rc_config for " << n->entityName()
	   << " is <= 0 (" << cost << ")" << endl;
  }
}
