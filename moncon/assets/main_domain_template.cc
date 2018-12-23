#include <unistd.h>
#include <iostream>
#include "mars_domain.h"
#include "ttyControl.h"
#include "randombot.h"
#include "ttybot.h"
#include "socketBot.h"

#include "mclLogger.h"
#include "mclHostAPI.h"
#include "mclMonitorResponse.h"
#include "mclSettings.h"
#include "mars_scoring.h"

#include "declarations.h"

#include <time.h>

using namespace std;

#define USLEEP_PER_TICK 100000

/*

void dumpsv(domain *domain) {

    const float* fsv = domain->mcl_sensor_vector();
    string waiting_reply="{ ";
    for (int i = 0;i<domain->mcl_sensor_vector_size();i++) {
      basic_domain* bdom = static_cast<basic_domain*>(domain);
      char buff[255];
      const char *snam = bdom->get_sensor_n(i)->get_name().c_str();
      breakable* qab = dynamic_cast<breakable*>(bdom->get_sensor_n(i));
      if ((qab != NULL) && qab->is_broken())
	sprintf(buff,"%s*=%f ",snam,fsv[i]);
      else
	sprintf(buff,"%s=%f ",snam,fsv[i]);
      // sprintf(buff,"s%d=%f ",i,fsv[i]);
      waiting_reply += buff;
    }
    waiting_reply+="}\n";
    mclLogger::annotate(MCLA_HOSTMSG,waiting_reply);

}

*/

int main(int argc, char* args[]) {
  // reads the command line args to the mcl Settings base
  mclSettings::args2SysProperties(argc,args);

  mclLogger::annotate(MCLA_HOSTMSG, "Mars Rover sim starting...");

  if (mclSettings::getSysPropertyString("out","") == "")
    mclLogger::setLogToStdOut();
  else {
    mclLogger::setLogToFile(mclSettings::getSysPropertyString("out"));
  }

  /*
  mars_scorer* scorer = NULL;
  if (mclSettings::getSysPropertyString("score","") == "")
    scorer = new mars_scorer();
  else {
    scorer = new mars_scorer(mclSettings::getSysPropertyString("score",""));
  }
  */

  srand((int)time(NULL));

  // build units
  basic_domain * domain     = new mars_domain();
  agent        * rover1     = new mars_rover(domain);
  controller   * r1cont     = new plannerBot(rover1);

  mclAPI::initializeMCL("marsSim",0);
  domain->declare_mcl();
  controller->declare_mcl();
  mclAPI::initializeSV((float *)(domain->mcl_sensor_vector()),
		       domain->mcl_sensor_vector_size());
  // check for timeline
  domain->set_active_controller(controller);
  timeline* schedule=NULL;
  if (mclSettings::getSysPropertyString("timeline","") != "")
    schedule = tlFactory::read_timeline(domain,mclSettings::getSysPropertyString("timeline",""));
  

  int simcnt=1;
  while(true) {
    mclLogger::annotate(MCLA_SEPERATOR);
    sprintf(mclLogger::annotateBuffer,"[MARS]::entering sim loop at step %d",
	    simcnt++);
    mclLogger::annotateFromBuffer(MCLA_HOSTMSG);
    
    if (scorer != NULL) scorer->update();
    
    dumpsv(domain);

    usleep(USLEEP_PER_TICK);
    if (schedule != NULL) schedule->update();
    controller->control_in();
    domain->simulate();
    controller->control_out();
    responseVector r = mclAPI::monitor((float *)(domain->mcl_sensor_vector()),
				       domain->mcl_sensor_vector_size());
    controller->control_react((const responseVector)r);

    dumpsv(domain);
    mclLogger::annotate(MCLA_HOSTMSG,declarations::dec_cnts_as_str());
  }

}
