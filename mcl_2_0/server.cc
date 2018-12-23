#include "umbc/settings.h"
#include "umbc/exceptions.h"
#include "umbc/logger.h"
#include "umbc/text_utils.h"
#include "umbc/token_machine.h"

#include "mcl_multiagent_api.h"
#include "mcl_symbols.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <exception>
#include <iostream>

#include "umbc/settings.h"

// Darwin (BSD) does not define this send flag
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

using namespace std;
using namespace umbc;
using namespace metacog;

string handle_command(string command);
void handle_connection(int newsockfd);
string rv2string(responseVector& rv);
string process_update(string key,bool* success,string update);
string process_update_into(string key,bool* success,
			   mclMA::observables::update& into,string update);

int main(int argc, char **argv) {
  settings::readSystemProps("umbcutil");
  settings::readSystemProps("mcl");
  settings::args2SysProperties( argc, argv);
  int mcl_port = settings::getSysPropertyInt("mcl.port",5150);
  int sockfd, newsockfd, clilen;
  struct sockaddr_in serv_addr, cli_addr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) 
    exceptions::signal_exception("ERROR opening socket");
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(mcl_port);
  if (bind(sockfd, (struct sockaddr *) &serv_addr,
	   sizeof(serv_addr)) < 0) 
    exceptions::signal_exception("ERROR on binding");
  cout << "[mcl_srvr]:: listening to port " 
       << dec << mcl_port << endl;
  listen(sockfd,5);
  clilen = sizeof(cli_addr);
  while (true) {
    newsockfd = accept(sockfd, 
		       (struct sockaddr *) &cli_addr, 
		       ((socklen_t*)&clilen));
    cout << "[tcp_agnt]:: connection from client initiated..." << endl;
    if (newsockfd < 0) 
      exceptions::signal_exception("ERROR on accept");
    else {
      handle_connection(newsockfd);
      close(newsockfd);
    }
  }
  pthread_exit((void*)0);  
}

string fail(string msg) { return "fail("+msg+")"; };
string succ(string msg) { return "ok("+msg+")"; };

void handle_connection(int newsockfd) {
  char buffer[256];
  int n;
  bool up=true;
  string this_ones_key="";
  while (up) {
    bzero(buffer,256);
    n = read(newsockfd,buffer,255);
    if (n < 0) exceptions::signal_exception("ERROR reading from socket");
    while (n>=0 && ((buffer[n]=='\0') || isspace(buffer[n]))) { 
        buffer[n--] = '\0'; //DW 12/22/2009
    }
    uLog::annotate(UMBCLOG_MSG,"[mcl_srv]::command received on socket = '"+((string)buffer)+"'");
    if ((strcmp(buffer,"close") == 0) || 
        (strcmp(buffer,"bye")   == 0) ||
        (strcmp(buffer,"exit")  == 0)) {  //DW 12/23/2009 
      uLog::annotate(UMBCLOG_WARNING,"[mcl_srv]:: connection closed by client.");
      up=false;
    }
    else {
      int q=0;
      try {
	string response = handle_command((string)buffer);
	if (strncmp(response.c_str(),"ok(initialized '",16) == 0) {
	  this_ones_key = response.substr(0,response.size()-3);
	}
	// if (settings::getSysPropertyString("reply","none") == "prolog")
	// response = agents::prologify(response);
	response+="\n";
	q = send(newsockfd,response.c_str(),response.length(),MSG_NOSIGNAL);
      }
      catch (exception e) {
	string explanation = e.what();
	// explanation = agents::fail_msg(explanation);
	// if (settings::getSysPropertyString("reply","none") == "prolog")
	// explanation = agents::prologify(explanation);
	explanation+="\n";
	q = send(newsockfd,explanation.c_str(),
		 explanation.length(),MSG_NOSIGNAL);
      }
      if (q < 0) {
	char errbuff[512];
	strerror_r(errno,errbuff,512);
	string exstr = errbuff;
	if (errno == EPIPE) {
#ifdef BREAK_ON_SIGPIPE
	  exceptions::signal_exception(exstr);
#else
	  uLog::annotate(UMBCLOG_ERROR,"[mcl_srv]:: broken pipe, closing connection...");
	  up=false;
#endif
	}
	else exceptions::signal_exception(exstr);
      }
    }
  }
  // only one way to get here, and that is when up=false, either by
  // explicit close or a broken pipe.
  if (this_ones_key.size() != 0) 
    mclMA::releaseMCL(this_ones_key);
}

string handle_command(string command) {
  string cmd = textFunctions::getFunctor(command);
  string params = textFunctions::getParameters(command);
  bool sv_fail = false;
  cout << "cmd[" << cmd << "] parms[" << params << "]" << endl;
  paramStringProcessor psp(params);
  try {
    string key = psp.getNextParam();
    if (cmd == "initialize") {
      if (psp.hasMoreParams()) {
	mclMA::initializeMCL(key,
			     textFunctions::numval(psp.getNextParam()));
      }
      else
	mclMA::initializeMCL(key,0);
      return succ("initialized '"+key+"'.");
    }
    else if (cmd == "ontology") {
      if (!psp.hasMoreParams())
	return fail("missing ONTOLOGY parameter.");
      else {
	string o = psp.getNextParam();
	if (mclMA::chooseOntology(key,o))
	  return succ("ontology set to "+o);
	else
	  return fail("ontology "+o+" not available.");
      }	
    }
    else if (cmd == "configure") {
      string dom_name = psp.getNextParam();
      if (psp.hasMoreParams()) {
	string agent_name = psp.getNextParam();
	if (psp.hasMoreParams()) {
	  string controller_name = psp.getNextParam();
	  if (mclMA::configureMCL(key,dom_name,agent_name,controller_name)) 
	    return succ("configured.");
	  else return fail("error configuring.");
	}
	else {
	  if (mclMA::configureMCL(key,dom_name,agent_name)) 
	    return succ("configured.");
	  else return fail("error configuring.");
	}
      }
      else {
	if (mclMA::configureMCL(key,dom_name)) return succ("configured.");
	else return fail("error configuring.");
      }
      return fail("strange configure error.");
    }
    else if (cmd == "newDefaultPV") {
      mclMA::newDefaultPV(key);
      return succ("default PV created.");
    }
    else if (cmd == "popDefaultPV") {
      mclMA::popDefaultPV(key);
      return succ("default PV popped.");
    }
    else if (cmd == "setPropertyDefault") {
      if (psp.hasMoreParams()) {
	int ind = symbols::smartval_int(psp.getNextParam(),&sv_fail);
	if (psp.hasMoreParams()) {
	  int pv = symbols::smartval_int(psp.getNextParam(),&sv_fail);
	  mclMA::setPropertyDefault(key,ind,pv);
	  return succ("default set.");
	}
	else return fail("missing VALUE parameter.");      
      }
      else return fail("missing PROPERTY_KEY parameter.");
    }
    else if (cmd == "setEGPropoerty") {
      if (psp.hasMoreParams()) {
	egkType egkey = (egkType)textFunctions::unumval(psp.getNextParam());
	if (psp.hasMoreParams()) {
	  pkType pkey = symbols::smartval_int(psp.getNextParam(),&sv_fail);
	  if (psp.hasMoreParams()) {
	    cerr << "setEGProperty may have problems: egkType is a pointer type, pval is a bool using smartval_int"
		 << endl;
	    int pnum = symbols::smartval_int(psp.getNextParam(),&sv_fail);
	    pvType pval = (pnum == 0) ? false : true;
	    mclMA::setEGProperty(key,egkey,pkey,pval);
	    return succ("property set.");
	  }
	  else return fail("missing PVAL parameter.");
	}
	else return fail("missing PKEY parameter.");
      }
      else return fail("missing EGKEY parameter.");
    }
    else if (cmd == "resetDefaultPV") {
      mclMA::reSetDefaultPV(key);
      return succ("default PV created.");
    }
    else if (cmd == "registerHIA") {
      if (!psp.hasMoreParams())
	return fail("missing NAME parameter.");
      else {
	string hianame = psp.getNextParam();
	if (!psp.hasMoreParams())
	  return fail("missing NODENAME parameter.");
	else {
	  string node = psp.getNextParam();
	  mclMA::HIA::registerHIA(key,hianame,node);
	}
      }
      return succ("HIA registered.");
    }
    else if (cmd == "signalHIA") {
      if (!psp.hasMoreParams())
	return fail("missing NAME parameter.");
      else {
	string hianame = psp.getNextParam();
	if (!psp.hasMoreParams())
	  return fail("missing NODENAME parameter.");
	else {
	  string node = psp.getNextParam();
	  mclMA::HIA::registerHIA(key,hianame,node);
	}
      }
      return succ("HIA registered.");
    }
    else if (cmd == "declareEG") {
      if (!psp.hasMoreParams())
	return fail("missing EG_KEY parameter.");
      else {
	egkType egkey = (egkType)textFunctions::unumval(psp.getNextParam());
	if (!psp.hasMoreParams()) {
	  mclMA::declareExpectationGroup(key,egkey);
	  return succ("expectation group declared (no parent/ref).");
	}
	else {
	  egkType parkey = (egkType)textFunctions::unumval(psp.getNextParam());
	  resRefType ref = RESREF_NO_REFERENCE;
	  if (!psp.hasMoreParams()) (resRefType)textFunctions::unumval(psp.getNextParam());
	  mclMA::declareExpectationGroup(key,egkey,parkey,ref);	  
	  return succ("expectation group declared (with parent/ref).");
	}
      }
    }
    else if (cmd == "EGabort") {
      if (psp.hasMoreParams()) {
	egkType egkey = (egkType)textFunctions::unumval(psp.getNextParam());
	mclMA::expectationGroupAborted(key,egkey);
	return succ("expectation group aborted.");
      }
      return fail("missing EG_KEY parameter.");
    }
    else if (cmd == "EGcomplete") {
      if (psp.hasMoreParams()) {
	egkType egkey = (egkType)textFunctions::unumval(psp.getNextParam());
	if (psp.hasMoreParams()) {
	  // this should be a sensor update
	  bool okay=false;
	  string rv = process_update(key,&okay,psp.getNextParam());
	  mclMA::expectationGroupComplete(key,egkey);
	  if (okay) return succ("EG Completed.");
	  else return fail(rv);
	}
	else {
	  mclMA::expectationGroupComplete(key,egkey);
	  return succ("EG Completed (no update)."); 
	}
      }
      else
	return fail("missing EG_KEY parameter.");
    }
    else if (cmd == "declareExp") {
      if (!psp.hasMoreParams()) return fail("missing GROUP_KEY parameter.");
      else {
	egkType egkey = (egkType)textFunctions::unumval(psp.getNextParam());
	if (!psp.hasMoreParams()) return fail("missing EC_CODE parameter.");
	else {
	  bool sv_fail = false;
	  string ec_str = psp.getNextParam();
	  ecType code = symbols::smartval_int(ec_str,&sv_fail);
	  if (sv_fail)
	    return fail("failed to parse EC_CODE "+ec_str+".");
	  else if (!psp.hasMoreParams()) return fail("missing ARG parameter.");
	  else {
	    double arg = textFunctions::dubval(psp.getNextParam());
	    mclMA::declareExpectation(key,egkey,code,arg);
	    return succ("general expectation declared.");
	  }
	}
      }
    }
    else if (cmd == "declareSelfExp") {
      if (!psp.hasMoreParams()) return fail("missing GROUP_KEY parameter.");
      else {
	egkType egkey = (egkType)textFunctions::unumval(psp.getNextParam());
	if (!psp.hasMoreParams()) return fail("missing OBSERVABLE parameter.");
	else {
	  string obs = psp.getNextParam();
	  if (!psp.hasMoreParams()) return fail("missing EC_CODE parameter.");
	  else {
	    bool sv_fail = false;
	    string ec_str = psp.getNextParam();
	    ecType code = symbols::smartval_int(ec_str,&sv_fail);
	    if (sv_fail)
	      return fail("failed to parse EC_CODE "+ec_str+".");
	    else if (!psp.hasMoreParams()) {
	      mclMA::declareSelfExpectation(key,egkey,obs,code);
	      return succ("self expectation declared (0-arg).");
	    }
	    else {
	      double arg = textFunctions::dubval(psp.getNextParam());
	      mclMA::declareSelfExpectation(key,egkey,obs,code,arg);
	      return succ("self expectation declared (1-arg).");
	    }
	  }
	}
      }
    }
    else if (cmd == "declareObjExp") {
      if (!psp.hasMoreParams()) return fail("missing GROUP_KEY parameter.");
      else {
	egkType egkey = (egkType)textFunctions::unumval(psp.getNextParam());
	if (!psp.hasMoreParams()) return fail("missing OBJECT parameter.");
	else {
	  string obj = psp.getNextParam();
	  if (!psp.hasMoreParams()) 
	    return fail("missing OBSERVABLE parameter.");
	  else {
	    string obs = psp.getNextParam();
	    if (!psp.hasMoreParams()) 
	      return fail("missing EC_CODE parameter.");
	    else {
	      bool sv_fail = false;
	      string ec_str = psp.getNextParam();
	      ecType code = symbols::smartval_int(ec_str,&sv_fail);
	      if (sv_fail)
		return fail("failed to parse EC_CODE "+ec_str+".");
	      else if (!psp.hasMoreParams()) {
		mclMA::declareObjExpectation(key,egkey,obj,obs,code);
		return succ("obj expectation declared (0-arg).");
	      }
	      else {
		double arg = textFunctions::dubval(psp.getNextParam());
		mclMA::declareObjExpectation(key,egkey,obj,obs,code,arg);
		return succ("obj expectation declared (1-arg).");
	      }
	    }
	  }
	}
      }
    }
    else if (cmd == "declareDelayedExp") {
      return fail("unimplemented.");
    }
    else if (cmd == "suggestionImp") {
      if (psp.hasMoreParams()) {
	resRefType ref = (resRefType)textFunctions::unumval(psp.getNextParam());
	mclMA::suggestionImplemented(key,ref);
	return succ("noted Implemented.");
      }
      else
	return fail("missing REFERENT parameter.");
    }
    else if (cmd == "suggestionFail") {
      if (psp.hasMoreParams()) {
	resRefType ref = (resRefType)textFunctions::unumval(psp.getNextParam());
	mclMA::suggestionFailed(key,ref);
	return succ("noted Fail.");
      }
      else
	return fail("missing REFERENT parameter.");
    }
    else if (cmd == "suggestionIgnored") {
      if (psp.hasMoreParams()) {
	resRefType ref = (resRefType)textFunctions::unumval(psp.getNextParam());
	mclMA::suggestionIgnored(key,ref);
	return succ("noted Ignore.");
      }
      else
	return fail("missing REFERENT parameter.");
    }
    else if (cmd == "provideFeedback") {
      if (!psp.hasMoreParams()) {
	return fail("missing FEEDBACK parameter.");
      }
      else {
	bool fb = textFunctions::boolval(psp.getNextParam());
	if (!psp.hasMoreParams()) {
	  return fail("missing REFERENT parameter.");
	}
	else {
	  resRefType ref = (resRefType)textFunctions::unumval(psp.getNextParam());
	  mclMA::provideFeedback(key,fb,ref);
	  return succ("feedback noted.");
	}
      }
    }
    else if (cmd == "monitor") {
      if (psp.hasMoreParams()) {
	// this should be a sensor update
	bool okay=true;
	mclMA::observables::update this_update;
	string rv = process_update_into(key,&okay,this_update,psp.getNextParam());
	responseVector rev = mclMA::monitor(key,this_update);
	if (okay) return succ(rv2string(rev));
	else return fail(rv);
      }
      else {
	return fail("Update is required for monitor() to work"); 
      }
    }
    else if (cmd == "declareObservableSelf") {
      if (psp.hasMoreParams()) {
	string oname = psp.getNextParam();
	if (psp.hasMoreParams()) {
	  double defv = textFunctions::numval(psp.getNextParam());
	  mclMA::observables::declare_observable_self(key,oname,defv);
	  return succ("declared '"+oname+"'.");
	}
	else {
	  mclMA::observables::declare_observable_self(key,oname);
	  return succ("declared sefl-obs '"+oname+"'.");
	}
      }
      else return fail("missing OBSERVABLE_NAME parameter.");
    }
    else if (cmd == "declareObservableObjType") {
      if (psp.hasMoreParams()) {
	string oname = psp.getNextParam();
	mclMA::observables::declare_observable_object_type(key,oname);
	return succ("declared object type '"+oname+"'.");
      }
      else return fail("missing OBJECT_TYPENAME parameter.");
    }
    else if (cmd == "declareObjField") {
      if (psp.hasMoreParams()) {
	string tname = psp.getNextParam();
	if (psp.hasMoreParams()) {
	  string fname = psp.getNextParam();
	  mclMA::observables::declare_object_type_field(key,tname,fname);
	  return succ("declared obj-field '"+tname+"."+fname+"'.");
	}
	else return fail("missing OBJECT_TYPENAME parameter.");
      }
      else return fail("missing OBJECT_FIELDNAME parameter.");
    }
    else if (cmd == "noticeObj") {
      if (psp.hasMoreParams()) {
	string tname = psp.getNextParam();
	if (psp.hasMoreParams()) {
	  string oname = psp.getNextParam();
	  mclMA::observables::notice_object_observable(key,tname,oname);
	  return succ("noticed '"+tname+"::"+oname+"'.");
	}
	else return fail("missing OBJECT_TYPENAME parameter.");
      }
      else return fail("missing OBJECT_NAME parameter.");
    }
    else if (cmd == "unnoticeObj") {
      if (psp.hasMoreParams()) {
	string oname = psp.getNextParam();
	mclMA::observables::notice_object_unobservable(key,oname);
	return succ("un-noticed '"+oname+"'.");
      }
      else return fail("missing OBJECT_NAME parameter.");
    }
    else if (cmd == "updateObs") {
      if (psp.hasMoreParams()) {
	string oname = psp.getNextParam();
	if (psp.hasMoreParams()) {
	  double defv = textFunctions::numval(psp.getNextParam());
	  mclMA::observables::update_observable(key,oname,defv);
	  return succ("updated '"+oname+"'.");
	}
	else return fail("missing OBSERVABLE_NAME parameter.");      
      }
      else return fail("missing VALUE parameter.");
    }
    else if (cmd == "updateObjObs") {
      if (psp.hasMoreParams()) {
	string oname = psp.getNextParam();
	if (psp.hasMoreParams()) {
	  string fname = psp.getNextParam();
	  if (psp.hasMoreParams()) {
	    double defv = textFunctions::numval(psp.getNextParam());
	    mclMA::observables::update_observable(key,oname,fname,defv);
	    return succ("updated '"+oname+"'.");
	  }
	  else return fail("missing VALUE parameter.");
	}
	else return fail("missing FIELD_NAME parameter.");      
      }
      else return fail("missing OBJECT_NAME parameter.");
    }
    else if (cmd == "getOV") {
      return succ(mclMA::observables::ov_as_string(key));
    }
    else if (cmd == "setObsPropSelf") {
      if (psp.hasMoreParams()) {
	string oname = psp.getNextParam();
	if (psp.hasMoreParams()) {
	  int pkey = symbols::smartval_int(psp.getNextParam(),&sv_fail);
	  if (psp.hasMoreParams()) {
	    int pval = symbols::smartval_int(psp.getNextParam(),&sv_fail);
	    mclMA::observables::set_obs_prop_self(key,oname,pkey,pval);
	    return succ("set prop for '"+oname+"'.");
	  }
	  else return fail("missing PROP_VALUE parameter.");
	}
	else return fail("missing PROP_KEY parameter.");      
      }
      else return fail("missing OBSERVABLE_NAME parameter.");
    }
    else if (cmd == "setObsPropObj") {
      if (psp.hasMoreParams()) {
	string oname = psp.getNextParam();
	if (psp.hasMoreParams()) {
	  string fname = psp.getNextParam();
	  if (psp.hasMoreParams()) {
	    int pkey = symbols::smartval_int(psp.getNextParam(),&sv_fail);
	    if (psp.hasMoreParams()) {
	      int pval = textFunctions::numval(psp.getNextParam());
	      mclMA::observables::set_obs_prop(key,oname,fname,pkey,pval);
	      return succ("set prop for '"+oname+"'.");
	    }
	    else return fail("missing PROP_VALUE parameter.");
	  }
	  else return fail("missing PROP_KEY parameter.");      
	}
	else return fail("missing FIELD_NAME parameter.");
      }
      else return fail("missing OBJECT_NAME parameter.");
    }
    else if (cmd == "setObsNoisePropSelf") {
      if (psp.hasMoreParams()) {
	string oname = psp.getNextParam();
	if (psp.hasMoreParams()) {
	  int npkey = symbols::smartval_int(psp.getNextParam(),&sv_fail);
	  if (psp.hasMoreParams()) {
	    double nparg = textFunctions::numval(psp.getNextParam());
	    mclMA::observables::set_obs_noiseprofile_self(key,oname,npkey,nparg);
	    return succ("set noise profile for '"+oname+"' (1 arg).");
	  }
	  else {
	    mclMA::observables::set_obs_noiseprofile_self(key,oname,npkey);
	    return succ("set noise profile for '"+oname+"' (0 args).");
	  }
	}
	else return fail("missing NOISE_PROFILE_KEY parameter.");
      }
      else return fail("missing OBSERVABLE_NAME parameter.");
    }
    else if (cmd == "setObsNoisePropObj") {
      if (psp.hasMoreParams()) {
	string oname = psp.getNextParam();
	if (psp.hasMoreParams()) {
	  string fname = psp.getNextParam();
	  if (psp.hasMoreParams()) {
	    int npkey = symbols::smartval_int(psp.getNextParam(),&sv_fail);
	    if (psp.hasMoreParams()) {
	      double narg = textFunctions::numval(psp.getNextParam());
	      mclMA::observables::set_obj_obs_noiseprofile(key,oname,fname,npkey,narg);
	      return succ("set noise profile for '"+oname+"' (1 arg).");
	    }
	    else {
	      mclMA::observables::set_obj_obs_noiseprofile(key,oname,fname,npkey);
	      return succ("set noise profile for '"+oname+"' (0 args).");
	    }
	  }
	  else return fail("missing NOISE_PROFILE_KEY parameter.");
	}
	else return fail("missing FIELD_NAME parameter.");
      }
      else return fail("missing OBJECT_NAME parameter.");
      
    }
    else if (cmd == "updateObservables") {
      if (psp.hasMoreParams()) {
	bool okay=false;
	string rv = process_update(key,&okay,psp.getNextParam());
	if (okay) 
	  return succ(rv);
	else 
	  return fail(rv);
      }
      else
	return fail("no update found.");
    }
    else if (cmd == "addObsLegalVal") {
      if (!psp.hasMoreParams())
	return fail("missing OBJNAME parameter.");
      else {
	string objname = psp.getNextParam();
	if (!psp.hasMoreParams())
	  return fail("missing OBSNAME parameter.");
	else {
	  string obsname = psp.getNextParam();
	  if (!psp.hasMoreParams())
	    return fail("missing LEGALVAL parameter.");
	  else {
	    double lval = textFunctions::numval(psp.getNextParam());
	    mclMA::observables::add_obs_legalval(key,objname,obsname,lval);
            return succ(cmd);
	  }
	}
      }
    }
    else if (cmd == "addObsLegalValSelf") {
      if (!psp.hasMoreParams())
	return fail("missing OBSNAME parameter.");
      else {
	string obsname = psp.getNextParam();
	if (!psp.hasMoreParams())
	  return fail("missing LEGALVAL parameter.");
	else {
	  double lval = textFunctions::numval(psp.getNextParam());
	  mclMA::observables::add_obs_legalval_self(key,obsname,lval);
          return succ(cmd); //DW 12/30/2009
	}
      }
    }
    else if (cmd == "setObsLegalRange") {
      if (!psp.hasMoreParams())
	return fail("missing OBJNAME parameter.");
      else {
	string objname = psp.getNextParam();
	if (!psp.hasMoreParams())
	  return fail("missing OBSNAME parameter.");
	else {
	  string obsname = psp.getNextParam();
	  if (!psp.hasMoreParams())
	    return fail("missing RANGEMIN parameter.");
	  else {
	    double rmin = textFunctions::numval(psp.getNextParam());
	    if (!psp.hasMoreParams())
	      return fail("missing RANGEMAX parameter.");
	    else {
	      double rmax = textFunctions::numval(psp.getNextParam());
	      mclMA::observables::set_obs_legalrange(key,objname,obsname,
		 				     rmin,rmax);
              return succ(cmd);//DW 12/30/2009
	    }
	  }
	}
      }
    }
    else if (cmd == "setObsLegalRangeSelf") {
      if (!psp.hasMoreParams())
	return fail("missing OBSNAME parameter.");
      else {
	string obsname = psp.getNextParam();
	if (!psp.hasMoreParams())
	  return fail("missing RANGEMIN parameter.");
	else {
	  double rmin = textFunctions::numval(psp.getNextParam());
	  if (!psp.hasMoreParams())
	    return fail("missing RANGEMAX parameter.");
	  else {
	    double rmax = textFunctions::numval(psp.getNextParam());
	    mclMA::observables::set_obs_legalrange_self(key,obsname,
							rmin,rmax);
            return succ(cmd); //DW 12/30/2009
	  }
	}
      }
    }
    else 
      return fail("unknown command.");
  }
  catch (exception e) {
    return fail(e.what());
  }
  return fail("internal error."); //DW 12/22/2009
}

string process_update_into(string key,bool* success,
			   mclMA::observables::update& into,string update) {
  *success=true;
  keywordParamMachine kwm(update);
  while (kwm.hasMoreParams()) {
    string kwp   = kwm.getNextKWP();
    string oname = kwm.keyword_of(kwp);
    string oval  = kwm.value_of(kwp);
    double doval = textFunctions::dubval(oval);
    cout << "  ass~> " << oname << " := " << doval << endl;
    if (oname.find(".") != string::npos) {
      string oostr = textFunctions::substChar(oname, '.', ' ');
      tokenMachine ootm(oostr);
      string o1 = ootm.nextToken();
      string o2 = ootm.nextToken();
      into.set_update(o1,o2,doval);
    }
    else {
      into.set_update(oname,doval);
    }
  }
  return "update success.";
}

string process_update(string key,bool* success,string update) {
  *success=true;
  keywordParamMachine kwm(update);
  while (kwm.hasMoreParams()) {
    string kwp   = kwm.getNextKWP();
    string oname = kwm.keyword_of(kwp);
    string oval  = kwm.value_of(kwp);
    double doval = textFunctions::dubval(oval);
    cout << "  ass~> " << oname << " := " << doval << endl;
    if (oname.find(".") != string::npos) {
      string oostr = textFunctions::substChar(oname, '.', ' ');
      tokenMachine ootm(oostr);
      string o1 = ootm.nextToken();
      string o2 = ootm.nextToken();
      mclMA::observables::update_observable(key,o1,o2,doval);
    }
    else {
      mclMA::observables::update_observable(key,oname,doval);
    }
  }
  return "update success.";
}

string rv2string(responseVector& rv) {
  string r="[";
  for (int i=0;i<(int)rv.size();i++) {
    if (i!=0) r+=",";
    r+=rv[i]->to_string();
  }
  return r+"]";
}
