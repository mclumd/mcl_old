#include "mcl.h"
#include "mclFrame.h"
#include "mcl_multiagent_api.h"
#include "mcl_ma_internal_api.h"
#include "mclSettings.h"

// #include "ontology_configurator.h"
// #include "mclOntology.h"
// #include "oNodes.h"
// #include "linkFactory.h"
// #include "configManager.h"

#include <iostream>
#include <list>

using namespace std;

void usage(char** argv);

int main(int argc, char **argv) {
  mclSettings::setSysProperty("suppressConfigErrors",true);
  if (argc < 3) {
    usage(argv);
  }
  else {
    // this initializes MCL and loads the base configuration
    if (argc < 3) 
      usage(argv);
    else {
      string mod = argv[1];
      string dom = argv[2];
      mclMA::initializeMCL("test",0);
      if (argc > 3) {
	string agnt = argv[3];
	if (argc > 4) {
	  string cont = argv[4];
	  mclMA::configureMCL("test",dom,agnt,cont);
	}
	else 
	  mclMA::configureMCL("test",dom,agnt);
      }
      else
	mclMA::configureMCL("test",dom);
      
      // now perform the function...
      if (mod == "cpt") {
	cout << "Starting to configure the CPTs." << endl;
	mclFrame    *f = new mclFrame(mclMAint::getMCLFor("test"),NULL,NULL);
	if (cpt_cfg::save_cpts_to_global(f)) {
	  cout << "configuration written to " 
	       << "'" << f->MCL()->getConfigKey() << "'"
	       << endl;
	  return 0;
	}
	else {
	  cout << "a problem occurred during config." << endl;
	  return 1;
	}
      }
      else if (mod == "costs") {
	cout << "Starting to configure costs." << endl;
	mclFrame    *f = new mclFrame(mclMAint::getMCLFor("test"),NULL,NULL);
      }
    }
  }
  return 0;
}

void usage(char** argv) {
  cout << "usage: " << argv[0] << " <module> [domain] [agent] [controller]" << endl;
  cout << " <module> = config component (valid: cpt, cost)" << endl;
  cout << " [domain] = domain directory to write to" << endl;
  cout << " [agent]  = agent directory to write to" << endl;
  cout << " [controller] = controller directory to write to" << endl;
  exit(-1);
}

      // cfg::setSystem(sys);
      // cfg::loadCPTs();
      // cfg::ignore_cfg_errors();
      // cfg::applyCPTconfig(f);
      // cfg::saveCPTs(f);
      // cfg::saveResponseCosts(f);      
