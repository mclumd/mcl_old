#include <unistd.h>
#include <iostream>

#include "umbc/declarations.h"
#include "umbc/instrumentation.h"
#include "substrate/timeline.h"
#include "substrate/domain_configurator.h"
#include "substrate/agent.h"
#include "substrate/controller.h"
#include "substrate/mcl_wrappers.h"
// #include "mcl/mcl_api.h"

#include "mars_domain.h"
#include "mars_scoring.h"
#include "mars_rover.h"
#include "relay_station.h"
#include "rover_mcl_wrappers.h"

#include <time.h>

#include "umbc/settings.h"
#include "mars_logging.h"
using namespace umbc;
using namespace std;
using namespace mcMars;

void setup_instrumentation(instrumentation::dataset& data,
			   mars_domain* dom);

int main(int argc, char* args[]) {
  // start by reading the defaults
  settings::readSystemProps("mars"); 
  settings::readSystemProps("moncon"); 

  // reads the command line args to the Mars Settings base
  settings::args2SysProperties(argc,args,"mars");
  
  if (settings::getSysPropertyString("mars.logfile","") == "")
    uLog::setLogToStdOut();
  else {
    uLog::setLogToFile(settings::getSysPropertyString("mars.logfile"));
  }

  // DOMAIN SECTION -- REQUIRES EDITING -- PLEASE FILL IN
  // domain_t dom = #error "please complete the domain section in main.cc"
  mars_domain dom;

  uLog::annotate(MARSL_HOSTMSG, "Configuring domain...");
  // reads in the domain configuration
  #define DEFAULT_CONFIG_FILENAME "default.mars"
  string dconfig_fn = settings::getSysPropertyString("mars.config",DEFAULT_CONFIG_FILENAME);
  if (dconfig_fn == DEFAULT_CONFIG_FILENAME)
    uLog::annotate(MARSL_HOSTERR, "mars.config is not set, configuring from '"+dconfig_fn+"' by default.");
  if (!domain_config::configure_domain(&dom,dconfig_fn)) {
    uLog::annotate(MARSL_HOSTERR, "Can't configure from '"+dconfig_fn+"'...");
    exit(-1);
  }

  // should probably allow this to be a static seed
  srand((int)time(NULL));

  // this is the MCL wrapper / init section!
  uLog::annotate(MARSL_HOSTMSG, "[MARS]::MCL init section follows...");
  for (int ai=0; ai<dom.num_agnts(); ai++) {
    agent* ali = dom.get_agent(ai);
    string pn = "mars.usemcl."+ali->class_name();
    if (settings::getSysPropertyBool(pn,false)) { 
      try {
	uLog::annotate(MARSL_HOSTMSG, "[MARS]::attempting mcl wrap for "+ali->get_name()+"...");
	agent* nuag = NULL;
	if (ali->class_name() == "mars_rover") {
	  nuag = mmcl::rover_mcl_wrapper::attempt_wrap(ali);
	}
	else
	  nuag = MonCon::mclAware::mcl_agent::attempt_wrap(ali);
	if (nuag) {
	  dom.rpl_agent(ai,nuag);
	  uLog::annotate(MARSL_HOSTMSG,"nu_agent="+nuag->describe());
	  uLog::annotate(MARSL_HOSTMSG,"dp_agent="+dom.get_agent(ai)->describe());
	}
      }
      catch (...) { 
	uLog::annotate(MARSL_HOSTERR, "[MARS]:: an exception occurred trying to mcl-ize "+ali->get_name());
      }
    } 
  }

  uLog::annotate(MARSL_HOSTMSG, "[MARS]::Initializing agents...");
  for (agent_list::iterator ali = dom.al_start();
       ali != dom.al_end();
       ali++) {
    uLog::annotate(MARSL_HOSTMSG, "[MARS]::Initializing "+(*ali)->get_name()+"...");
    (*ali)->initialize(); // the agent should tell its controller to initialize
  }

  // check for timeline
  timeline* schedule=NULL;
  if (settings::getSysPropertyString("mars.timeline","") != "") {
    schedule = dom.get_timeline();
    tlFactory::read_to_timeline(*schedule,settings::getSysPropertyString("mars.timeline",""));
  }

  uLog::annotate(MARSL_HOSTMSG, "Starting instrumentation...");
  int simcnt=0;
  instrumentation::dataset data(settings::getSysPropertyString("mars.datafile",
							       "mars_data.out"));
  {  
    instrumentation::int_ptr_instrument i1("tick",&simcnt);
    data.add_instrument(i1);
  }
  setup_instrumentation(data,&dom);

  int USLEEP_PER_TICK = settings::getSysPropertyInt("mars.throttleUs",100000);

  dom.dump_mxs();
  
  if (settings::getSysPropertyBool("mars.init",false))
    exit(1);

  // sanity check
  uLog::annotateStart(MARSL_PRE);
  *uLog::log << "final sanity check before entering sim...." << endl;
  settings::dumpSystemProps(*uLog::log);
  *uLog::log << "end sanity check." << endl;
  uLog::annotateEnd(MARSL_PRE);

  uLog::annotate(MARSL_HOSTMSG, "Starting simulation...");
  while(true) {
    if ((simcnt % 100) == 0) cout << "simulation step " << simcnt << endl;
    uLog::annotate(MARSL_SEPERATOR);
    data.writeline();
    sprintf(uLog::annotateBuffer,"[MARS]::entering sim loop at step %d",
	    simcnt++);
    uLog::annotateFromBuffer(MARSL_HOSTMSG);
    
    // if (scorer != NULL) scorer->update();
    
    // dumpsv(dom);

    usleep(USLEEP_PER_TICK);
    if (schedule != NULL) schedule->update();

    // THIS IS IMPORTANT !! CONTROL FLOW IMPORTANT TO MCL !!

    // STEP 1: CONTROL_IN

    for (agent_list::iterator ali = dom.al_start();
	 ali != dom.al_end();
	 ali++) {
      if ((*ali)->get_active_controller() != NULL)
	(*ali)->get_active_controller()->control_in();
    }

    // STEP 2: SIMULATE

    dom.simulate();

    // STEP 3: CONTROL_OUT

    for (agent_list::iterator ali = dom.al_start();
	 ali != dom.al_end();
	 ali++) {
      if ((*ali)->get_active_controller() != NULL)
	(*ali)->get_active_controller()->control_out();
    }

    // STEP 3: MONITOR + CONTROL_REACT
    // note  : control react has kind of disappeared.
    
    for (agent_list::iterator ali = dom.al_start();
	 ali != dom.al_end();
	 ali++) {
      /*
	mcl_sensing* aams = dynamic_cast<mcl_sensing*>(*ali);
	if (aams != NULL) {
	responseVector r = aams->monitor_mcl();
	(*ali)->get_active_controller()->control_react((const responseVector)r);
	}
	else {
	sprintf(uLog::annotateBuffer,"[MARS]::agent %s is not MCL compatible.",
	(*ali)->get_name().c_str());
	uLog::annotateFromBuffer(MARSL_HOSTMSG);
	}
      */
    }
    uLog::annotate(MARSL_HOSTMSG,umbc::declarations::dec_cnts_as_str());
  }

  data.writeline();

  for (agent_list::iterator ali = dom.al_start();
       ali != dom.al_end();
       ali++) {
    (*ali)->shutdown(); // the agent should tell its controller to shutdown
  }

}

void db_instrument(instrumentation::dataset& d,string decl_name) {
  instrumentation::decl_based_instrument i(decl_name,decl_name);
  d.add_instrument(i);
}

void dba_instrument(instrumentation::dataset& d,
		    string agent_name, string decl_name) {
  string nm = simEntity::personalize_static(agent_name,decl_name);
  instrumentation::decl_based_instrument i(nm,nm);
  d.add_instrument(i);
}

void dbac_instrument(instrumentation::dataset& d,
		     string agent_name, string cont_name, string decl_name) {
  string nm = simEntity::personalize_static(agent_name,cont_name,decl_name);
  instrumentation::decl_based_instrument i(nm,nm);
  d.add_instrument(i);
}

void setup_instrumentation(instrumentation::dataset& d,
			   mars_domain* dom) {
  db_instrument(d,"eventTotal");
  db_instrument(d,"eventNatural");
  db_instrument(d,"timeline.sBreak");
  agent_list::iterator astart = dom->al_start();
  agent_list::iterator aend = dom->al_end();
  for (agent_list::iterator ai = astart;
       ai != aend;
       ai++) {
    dba_instrument(d,(*ai)->get_name(),"mclRVcount");
    relay_station* aars = 
      sensorimotor_agent_wrapper::get_typed_wrapper<relay_station*>(*ai);
    if (aars != NULL) {
	dba_instrument(d,(*ai)->get_name(),"images.received");
	dba_instrument(d,(*ai)->get_name(),"imageblocks.received");
	dba_instrument(d,(*ai)->get_name(),"goals.satisfied");
	dba_instrument(d,(*ai)->get_name(),"goals.received");
	dba_instrument(d,(*ai)->get_name(),"goals.ogsize");
    }
    else {
      mars_rover* aamr = 
	sensorimotor_agent_wrapper::get_typed_wrapper<mars_rover*>(*ai);
      if (aamr != NULL) {
	dba_instrument(d,(*ai)->get_name(),"mclRVcount");
	dba_instrument(d,(*ai)->get_name(),"bankOverflow");
	dba_instrument(d,(*ai)->get_name(),"xmit.ok");
	dba_instrument(d,(*ai)->get_name(),"xmit.fail");
	dba_instrument(d,(*ai)->get_name(),"sensorReset.diag");
	dba_instrument(d,(*ai)->get_name(),"sensorReset.reset");
	dba_instrument(d,(*ai)->get_name(),"effReset.diag");
	dba_instrument(d,(*ai)->get_name(),"effReset.reset");
	dba_instrument(d,(*ai)->get_name(),"rescue.count");
	dbac_instrument(d,(*ai)->get_name(),
			(*ai)->get_active_controller()->get_name(),
			"actionActivation.ok");
	dbac_instrument(d,(*ai)->get_name(),
			(*ai)->get_active_controller()->get_name(),
			"actionActivation.fail");
	dbac_instrument(d,(*ai)->get_name(),
			(*ai)->get_active_controller()->get_name(),
			"goal.processed");
	dbac_instrument(d,(*ai)->get_name(),
			(*ai)->get_active_controller()->get_name(),
			"maintenance.activation.ok");
	dbac_instrument(d,(*ai)->get_name(),
			(*ai)->get_active_controller()->get_name(),
			"maintenance.activation.fail");
	dbac_instrument(d,(*ai)->get_name(),
			(*ai)->get_active_controller()->get_name(),
			"reactive.activation.ok");
	dbac_instrument(d,(*ai)->get_name(),
			(*ai)->get_active_controller()->get_name(),
			"reactive.activation.fail");
      }
    }
  }

}
