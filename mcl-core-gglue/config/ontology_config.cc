#include "ontology_config.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef WIN32
#include <direct.h>
#else
#include <dirent.h>
#endif

ontology_configuration::ontology_configuration(string domain) 
  : conf_id(ontology_configurator::ontology_id(domain)),
    source_path(""),write_path(""),specificity(1) {
  load_initial_config();
}

ontology_configuration::ontology_configuration(string domain, string agent)
  : conf_id(ontology_configurator::ontology_id(domain,agent)),
    source_path(""),write_path(""),specificity(2) {
  load_initial_config();
}

ontology_configuration::ontology_configuration(string domain, string agent, 
					       string controller) 
  : conf_id(ontology_configurator::ontology_id(domain,agent,controller)),
    source_path(""),write_path(""),specificity(3) {
  load_initial_config();
}

bool ontology_configuration::load_initial_config() {
  // first compute the path
  string pth = determine_paths();
  cout << pth;
  cout << "determined load path is '" << pth << "'" << endl;
  bool rv = cpt_cfg::load_cpts(cpt_configuration,pth);
  // rv &=  rc_cfg::load_response_costs(pth);
  return rv;
}

#define VERIFY_FN "cp_tables.mcl"

bool ontology_configuration::verify_path(string path) {
  fstream fin;
  string vfn = path+"/"+VERIFY_FN;
  cout << "testing for file <" << vfn << ">...";
  fin.open(vfn.c_str(),ios::in);
  if( fin.is_open() ) {
    fin.close();
    cout << "yes" << endl;
    return true;
  }
    cout << "no" << endl;
  return false;
}

string ontology_configuration::determine_paths() {
  char *pathToHome = getenv("HOME");
  char *pathToMCLCore = getenv("MCL_CORE_PATH");

  string pth="",ptc="";
  if (pathToMCLCore != NULL) ptc = pathToMCLCore;
  ptc+="config/";
  if (pathToHome != NULL) pth = pathToHome;

  string test_path="";

  // start by producing a default write path
  if ((pathToMCLCore != NULL) && 
      (ensure_write_path(pathToMCLCore + conf_id))) {
    write_path = pathToMCLCore + conf_id;
  }
  else if ((pathToHome != NULL) && 
	   ensure_write_path(pathToHome + conf_id)) {
    write_path = pathToHome + conf_id;
  }
  else {
    // otherwise use CWD
    mclLogger::annotate(MCLA_WARNING,"[mcl/oc]:: could not confirm user preference for write path using MCL_CORE_PATH or HOME. Using CWD as root for writing configurations.");
    write_path = "./";
  }

  mclLogger::annotate(MCLA_MSG,"[mcl/oc]:: config write path is '"+write_path+"'");

  cout << "checking CORE binding for full config path..." << endl;
  if (pathToMCLCore != NULL) {
    // check full core binding
    test_path = ptc+conf_id;
    if (verify_path(test_path)) {
      source_path=test_path;
      return source_path;
    }
  }

  cout << "checking HOME binding for full config path..." << endl;
  if (pathToHome != NULL) {
    // check full core binding
    test_path = pth+"/"+conf_id;
    if (verify_path(test_path))
      return source_path=test_path;
  }

  cout << "specificity of mcl config is " << specificity << endl;

  if (specificity > 1) {
    cout << "relaxing CORE and HOME paths for relevant config path..." << endl;
    string relax_path = conf_id;
    bool confirmed = false;
    for (int srcnt = 1 ; srcnt < specificity ; srcnt++) {
      relax_path = relax_config_path(relax_path);
      if (pathToMCLCore != NULL) {
	test_path = ptc+relax_path;
	if (verify_path(test_path)) {
	  confirmed=true; 
	  continue;
	}
      }
      if (pathToHome != NULL) {
	test_path = pth+"/"+relax_path;
	if (verify_path(test_path)) {
	  confirmed=true; 
	  continue;
	}
      }
    }
    if (confirmed) {
      // now figure out what to do before returning the relaxed path
      //
      // do_something_with_relaxed_path();
      return source_path=test_path;
    }
  }
  // attempt to find default config
  cout << "checking MCL_CORE_PATH binding for default config..." << endl;
  if (pathToMCLCore != NULL) {
    // check full core binding
    test_path = ptc+"default";
    if (verify_path(test_path)) {
      source_path=test_path;
      return source_path;
    }
  }
  cout << "checking HOME binding for default config..." << endl;
  if (pathToHome != NULL) {
    // check full core binding
    test_path = pth+"/default";
    if (verify_path(test_path))
      return source_path=test_path;
  }
  // give up, return empty config path
  mclLogger::annotate(MCLA_WARNING,"[mcl/oc]:: could not find config in MCL_CORE_PATH or HOME. Ontologies will not be configured by configManager.\n");
  return "";
}

string ontology_configuration::relax_config_path(string relax_path) {
  size_t lastslash = relax_path.find_last_of("/\\");
  if (lastslash == string::npos)
    return relax_path;
  else
    return relax_path.substr(0,lastslash);
}

bool ontology_configuration::apply_rc_config(mclFrame* f) {
  return rc_cfg::apply_rc_config(f);
}

bool ontology_configuration::apply_cpt_config(mclFrame* f) {
  return cpt_cfg::apply_cpt_config(cpt_configuration,f);
}

bool ontology_configuration::ensure_write_path(string path) {
  return false;
}
