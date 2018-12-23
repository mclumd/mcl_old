#include "umbc/settings.h"
#include "umbc/exceptions.h"
#include "umbc/file_utils.h"
#include "mars_domain.h"
#include "mars_rover.h"
#include "substrate/domain_configurator.h"
#include "mars_logging.h"

#include "substrate/actions.h"

#include <stdlib.h>
#include <fstream>
#include <math.h>

using namespace umbc;
using namespace std;
using namespace mcMars;

enum RVR_ANOMALIES { SENSOR_BREAK, EFFECTOR_BREAK, RVR_ANOM_MAX };
enum DOM_ANOMALIES { DIST_CHANGE, EDGE_ADD, EDGE_DEL, MV_CHG_PT, 
		     MV_LOC_PT, MV_SCI_PT,  DOM_ANOM_MAX };

extern string mars_event_names[];

string generate_domain_event(struct drand48_data* bigseed,
			     int timestamp, mars_domain* d);
string generate_rover_event(struct drand48_data* bigseed,int timestamp, 
			     mars_rover* d);
string generate_rover_goal(struct drand48_data* bigseed,int timestamp, 
			     mars_rover* d);

int main(int argc, char* args[]) {
  settings::readSystemProps("umbcutil");
  settings::readSystemPropsInDir("assets","tlg");
  settings::readSystemProps("mars"); 
  settings::args2SysProperties(argc,args,"tlg");
  mars_domain* dom = new mars_domain();

  uLog::annotate(MARSL_HOSTMSG, "Configuring domain...");
  string dconfig_fn = 
    settings::getSysPropertyString("tlg.config","assets/demo_config.mars");
  if (!domain_config::configure_domain(dom,dconfig_fn)) {
    uLog::annotate(MARSL_HOSTERR, "Can't configure from '"+dconfig_fn+"'...");
    exit(-1);
  }

  unsigned int seed = (unsigned int)settings::getSysPropertyInt("tlg.seed",0);
  struct drand48_data bigseed;
  if (seed == 0) seed = ((unsigned int)time(NULL));
  srand48_r((long)seed,&bigseed);

  string outfilename = 
    settings::getSysPropertyString("tlg.output",file_utils::fileNameRoot(dconfig_fn)+".tl");
  if (!establish_file(outfilename,file_utils::MODE_MOVE,NULL)) {
    exceptions::signal_exception("can't create output file '"+outfilename+"'");
  }    

  ofstream fileStream(outfilename.c_str(),ios::out);
  if (fileStream.is_open()) {

    fileStream << "Mars Domain" << endl;

    int tlsize = settings::getSysPropertyInt("tlg.length",1000);
    double deden = settings::getSysPropertyDouble("tlg.domain_event_rate",0.001);
    double reden = settings::getSysPropertyDouble("tlg.rover_event_rate",0.001);
    double rgden = settings::getSysPropertyDouble("tlg.rover_goal_density",0.001);
    
    for (int i=0;i<tlsize;i++) {
      // domain event?
      double de_roll;
      drand48_r(&bigseed,&de_roll);
      if ( de_roll < deden ) {
	string atline = generate_domain_event(&bigseed,i,dom);
	fileStream << atline << endl;
      }
      // now agent-specific stuff
      agent_list::iterator astart = dom->al_start();
      agent_list::iterator aend = dom->al_end();
      for (agent_list::iterator ai = astart;
	   ai != aend;
	   ai++) {
	mars_rover* aamr = dynamic_cast<mars_rover*>(*ai);
	if (aamr != NULL) {
	  // event?
	  double ae_roll;
	  drand48_r(&bigseed,&ae_roll);
	  if ( ae_roll < reden ) {
	    string atline = generate_rover_event(&bigseed,i,aamr);
	    fileStream << atline << endl;
	  }
	  // goal?
	  double ag_roll;
	  drand48_r(&bigseed,&ag_roll);
	  if ( ag_roll < rgden ) {
	    string atline = generate_rover_goal(&bigseed,i,aamr);
	    fileStream << atline << endl;
	  }
	}
      }
    }
    fileStream << "at " << tlsize << " exit" << endl;
  }
}

string generate_domain_event(struct drand48_data* bigseed,int timestamp, 
			     mars_domain* d) {
  double roll;
  drand48_r(bigseed,&roll);
  int evcode = (int)floor(roll*(double)DOM_ANOM_MAX);
  long   sseed;
  lrand48_r(bigseed,&sseed);
  // cout << "event generated: " << evcode << endl;
  char buff[1024];
  sprintf(buff,"at %d domain event %s seed %ld",timestamp,mars_event_names[evcode].c_str(),sseed);
  return (string)buff;
}

enum   rover_event_codes { ROVER_SBREAK_EV, ROVER_EBREAK_EV, ROVER_EVENT_MAX };
string rover_event_names[] = { "sensorbreak", "effectorbreak" };

string generate_random_sbreak_args(struct drand48_data* bigseed,mars_rover* d) {
  int bs_count=0;
  for (int i = 0; i < d->num_observables(); i++) 
    if (observable_utils::get_breakable(d->get_observable_n(i)) != NULL) bs_count++;
  if (bs_count == 0)
    return "";
  else {
    double roll;
    drand48_r(bigseed,&roll);
    int sbcode = (int)floor(roll*(double)bs_count);
    for (int i = 0; i < d->num_observables(); i++) {
      if (observable_utils::get_breakable(d->get_observable_n(i)) != NULL) {
	if (sbcode == 0)
	  return d->get_observable_n(i)->get_name();
	else sbcode--;
      }
    }
    return ""; // should not happen
  }
}

string generate_random_ebreak_args(struct drand48_data* bigseed,mars_rover* d) {
  int bs_count=0;
  for (int i = 0; i < d->num_actions(); i++) 
    if (action_wrappers::action_wrapper::get_typed_wrapper<action_wrappers::breakable_layer*>(d->get_action(i)) != NULL) bs_count++;
  if (bs_count == 0)
    return "";
  else {
    double roll;
    drand48_r(bigseed,&roll);
    int sbcode = (int)floor(roll*(double)bs_count);
    for (int i = 0; i < d->num_actions(); i++) {
      if (action_wrappers::action_wrapper::get_typed_wrapper<action_wrappers::breakable_layer*>(d->get_action(i)) != NULL) {
	if (sbcode == 0)
	  return d->get_action(i)->get_name();
	else sbcode--;
      }
    }
    return ""; // should not happen
  }
}

string generate_rover_event(struct drand48_data* bigseed,int timestamp, 
			    mars_rover* d) {
  double roll;
  drand48_r(bigseed,&roll);
  int evcode = (int)floor(roll*(double)ROVER_EVENT_MAX);
  string as = "";
  switch (evcode) {
  case ROVER_SBREAK_EV:
    as = generate_random_sbreak_args(bigseed,d);
    break;
  case ROVER_EBREAK_EV:
    as = generate_random_ebreak_args(bigseed,d);
    break;
  }
  char buff[1024];
  sprintf(buff,"at %d domain event %s %s %s",timestamp,
	  rover_event_names[evcode].c_str(),d->get_name().c_str(),as.c_str());
  return (string)buff;
}

string generate_rover_goal(struct drand48_data* bigseed,int timestamp, 
			   mars_rover* d) {
  double roll;
  drand48_r(bigseed,&roll);
  model* mdm = ((mars_domain*)d->domain_of())->provide_ptr_to_model(d->domain_of());
  int nsp    = ((mars_domain_model_suite*)mdm)->num_scipoints();
  int point  = (int)floor(roll*(double)nsp);
  char buff[1024];
  sprintf(buff,"at %d for %s goal science(%d)",timestamp,d->get_name().c_str(),
	  point);
  return (string)buff;
}
