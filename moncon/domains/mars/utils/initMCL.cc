#include "mcl_multiagent_api.h"
#include "mclSettings.h"

#include "substrate/domain_configurator.h"
#include "substrate/agent.h"

#include <unistd.h>
#include <iostream>

/* BEGIN DOMAIN-SPECIFIC REQUIRED INCLUDES HERE */
#include "mars_domain.h"

using namespace std;

int main(int argc, char **argv) {
  mclSettings::args2SysProperties(argc,argv);
  mclSettings::setSysProperty("suppressConfigErrors",true);
  mclSettings::getSysPropertyBool("writeConfigGlobal",true);

  mars_domain * dom = new mars_domain();
  string dconfig_fn = mclSettings::getSysPropertyString("config",".mc_config");
  if (!domain_config::configure_domain(dom,dconfig_fn)) {
    cerr << "Can't configure from '" << dconfig_fn << "'." << endl;
    exit(-1);
  }

  for (agent_list::iterator ali = dom->al_start();
       ali != dom->al_end();
       ali++) {
    (*ali)->initialize(); // the agent should tell its controller to initialize
  }

  for (agent_list::iterator ali = dom->al_start();
       ali != dom->al_end();
       ali++) {
    mcl_sensing* aama = dynamic_cast<mcl_sensing*>(*ali);
    if (aama) {
      cout << "agent " << (*ali)->get_name() << " is " << aama->mcl_agentspec()
	   << endl;
      mclMA::writeInitializedCPTs(aama->mcl_agentspec());
    }
  }

  return 0;
}



  /* I can only assume this was a first pass...

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
  */
