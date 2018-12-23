#include "substrate/domain_configurator.h"
#include "substrate/moncon_logging.h"
#include "mcl/mcl_api.h"
#include "umbc/token_machine.h"
#include "umbc/settings.h"

#include <string.h>
#include <fstream>

using namespace MonCon;

char* domain_config::error_msg = new char[1025];

bool domain_config::configure_domain(domain *d, string config_file) {
  using namespace umbc;
  ifstream in;
  in.open(config_file.c_str());
  if (in.is_open()) {
    string line;
    getline(in,line); // header line -- use it for something?
    int q=2;
    while (!in.eof()) {
      getline (in,line);
      if (line.size() > 0) {
	sprintf(error_msg,"unknown error.");
	if (strncmp(line.c_str(),"#",1) == 0) {
	}
	else if (strncmp(line.c_str(),"set",3) == 0) {
	  tokenMachine qs(line);
	  qs.nextToken(); // eat the "set" token
	  if (qs.moreTokens()) {
	    string setting = qs.nextToken();
	    if (qs.moreTokens()) {
	      string val = qs.nextToken();
	      settings::setSysProperty(setting,val);
	      uLog::annotate(MONL_MSG,"configuration override in config "+setting+"="+val+"("+settings::getSysPropertyString(setting,"*error*")+")");
	    }
	    else { 
	      sprintf(uLog::annotateBuffer," > %s line %d: malformed 'set' command.",
		      config_file.c_str(),q);
	      uLog::annotateFromBuffer(MONL_HOSTERR);
	    }
	  }
	  else {
	    sprintf(uLog::annotateBuffer," > %s line %d: malformed 'set' command.",
		    config_file.c_str(),q);
	    uLog::annotateFromBuffer(MONL_HOSTERR);
	  }
	}
	else if (!d->configure(line)) {
	  uLog::annotate(MONL_HOSTERR," > "+line);
	  sprintf(uLog::annotateBuffer," > %s line %d: %s",
		  config_file.c_str(),q,error_msg);
	  uLog::annotateFromBuffer(MONL_HOSTERR);
	  cerr << uLog::annotateBuffer << endl;
	  return false;
	}
      }
      q++;
    }
    in.close();
    return true;
  }
  else {
    sprintf(uLog::annotateBuffer," %s not found...",
	    config_file.c_str());
    uLog::annotateFromBuffer(MONL_HOSTERR);
    return false;
  }
}
