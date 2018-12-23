#include <iostream>
#include <fstream>
#include <algorithm>

#ifdef WIN32
#include <direct.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include "ontology_config.h"
#include "textUtils.h"
#include "mcl.h"
#include "mclExceptions.h"

using namespace std;

extern char mcl_perror[2048];
// map<string,string> CPTmap;

bool ignore_errors = false;

bool checkConsistent(mclNode *n,vector<string>& deps);
bool checkSorted(mclNode *n,vector<string>& deps);
bool resortAndInitCPT(mclNode *n,
		      vector<string>& deps,
		      vector<double>& cpv);
bool ensure_path(string path);


bool cpt_cfg::ignore_cfg_errors() {
  return (ignore_errors=true);
}

bool cpt_cfg::load_cpts(cpt_configuration_type& dest,string pth) {
  // cout << "attempting to load cpt config from '" << pth << "'" << endl;
  dest.clear();
  
  string conf_is = pth+"/cp_tables.mcl";
  // string conf_is= pathToConfigRead("cp_tables.mcl");

  mclLogger::annotate(MCLA_MSG,"loading conditional probability priors from " +
		      conf_is + "...");
  string conf_s = textFunctions::file2string(conf_is);
  bool running=true;
  tokenMachine level1(conf_s);
  while (running) {
    string table_s = level1.nextToken();
    // cout << "::> '" << table_s << "'" << endl;
    if (table_s.size() == 0) 
      running=false;
    else {
      tokenMachine level2(table_s);
      level2.trimParens();
      string node_s = textFunctions::dequote(level2.nextToken());
      dest[node_s]=table_s;
    }
  }
  sprintf(mclLogger::annotateBuffer,"%d tables loaded.", dest.size());
  mclLogger::annotateFromBuffer(MCLA_MSG);
}

string cfg_globalize(string path) {
  char* foo = getenv("MCL_CORE_PATH");
  if (foo) {
    string pathToMCLCore = foo;
    return pathToMCLCore+"config/"+path;
  }
  else {
    mclLogger::annotate(MCLA_WARNING,
			"could not get MCL_CORE_PATH to globalize '"+path+"', defaulting to CWD for writing config.");
    return path;
  }
}

string cfg_homize(string path) {
  string pathToHome = getenv("HOME");
  if (pathToHome != "") return pathToHome+"/.mcl_config/"+path;
  return path;
}

bool cpt_cfg::save_cpts_to_global(mclFrame *f) {
  // cout << "save cpts to global (autopath)." << endl;
  return save_cpts_to_global(f->MCL()->getConfigKey(),f);
}

bool cpt_cfg::save_cpts_to_cwd(mclFrame *f) {
  return save_cpts(f);
}

bool cpt_cfg::save_cpts_to_home(mclFrame *f) {
  // cout << "save cpts to home (autopath)." << endl;
  return save_cpts_to_home(f->MCL()->getConfigKey(),f);
}

bool cpt_cfg::save_cpts_to_global(string path, mclFrame *f) {
  // cout << "save cpts to global (" << path << ")." << endl;
  return save_cpts(cfg_globalize(path),f);
}

bool cpt_cfg::save_cpts_to_cwd(string path, mclFrame *f) {
  return save_cpts(path,f);
}

bool cpt_cfg::save_cpts_to_home(string path, mclFrame *f) {
  return save_cpts(cfg_homize(path),f);
}

bool cpt_cfg::save_cpts(mclFrame *f) {
  string pithy_path = f->MCL()->getConfigKey();
  // cout << "save path would be '" << pithy_path << "'" << endl;
  return save_cpts(pithy_path,f);
}

bool cpt_cfg::save_cpts(string path, mclFrame *f) {
  if (!ensure_path(path)) {
    cerr << "could not ensure a writeable path at '"
	 << path << "'... aborting write." << endl;
    return false;
  }

  string is = path+"/cp_tables.mcl";
  // make a backup
  string os = is+".bak";
  mclLogger::annotate(MCLA_MSG,"moving old config file to "+os);
  ifstream i(is.c_str(),ios_base::in|ios_base::binary);
  ofstream o(os.c_str(),ios_base::out|ios_base::binary);
  o << i.rdbuf();  
    
  mclLogger::annotate(MCLA_MSG,"writing new config file to " + is);
  ofstream no(is.c_str(),ios_base::out);
  if (!no.good()) {
    mclLogger::annotate(MCLA_ERROR, is + " is not good.");
    return false;
  }

  for (int indx=0; indx<f->numOntologies(); indx++) {
    // cout << "writing ontology " << indx << endl;
    mclOntology *this_o = f->getCoreOntology(indx);
    for (nodeList::iterator oni = this_o->firstNode();
	 oni != this_o->endNode();
	 oni++) {
      // cout << "writing node " << (*oni)->entityBaseName() << endl;
      no << "(\"" << (*oni)->entityBaseName() << "\" "; 
      // deps
      if ((*oni)->inLinkCnt() > 0) {
	no << ":dep (";
	for (llIterator ili = (*oni)->inLink_begin();
	     ili != (*oni)->inLink_end();
	     ili++) {
	  no << "\"" << (*ili)->sourceNode()->entityBaseName() << "\" ";
	}
	no << ") ";
      }
      // cpt
      {
	int cpt_size = (int)(pow(2,(*oni)->inLinkCnt()));
	double* CPTaa = new double[cpt_size];
	(*oni)->writeCPTtoArray(CPTaa);
	no << ":cpv (";
	for (int c = 0;
	     c < cpt_size;
	     c++) {
	  no << CPTaa[c] << " ";
	}
	no << ")";
	no << ")" << endl;
	delete[] CPTaa;
      }
    }
  }
}


bool ensure_path(string path) {
#ifdef WIN32
  return (_mkdir(local_system_config_path.c_str())==0);
#else      
  // check the whole path....
  DIR* d = opendir(path.c_str());
  if (d != NULL) {
    closedir(d);
    return true;
  }
  else {
    // okay, now unwind using a tokenMachine
    string base = textFunctions::substChar(path,'/',' ');
    // cout << "(" << base << ")" << endl;
    tokenMachine tm(base);
    string ccd = "";
    if (path.at(0) == '/')
      ccd+="/";
    while (tm.moreTokens()) {
      ccd+=tm.nextToken()+"/";
      DIR* d = opendir(ccd.c_str());
      if (d != NULL) {
	closedir(d);
      }
      else {
	mclLogger::annotate(MCLA_MSG,"[mcl/cfgcpt]:: creating "+ccd);
	// mode_t omask = 	umask();
	int rv = mkdir(ccd.c_str(),0777);
	if (rv != 0) {
	  perror("error in mkdir");
	  return false;
	}
      }
    }
    return true;
  }
#endif  
}

bool cpt_cfg::apply_cpt_config(cpt_configuration_type& dest,mclFrame *f) {
  bool rv = true;
  rv &= cpt_cfg::apply_cpt_config(dest, f->getIndicationCore());
  rv &= cpt_cfg::apply_cpt_config(dest, f->getFailureCore());
  rv &= cpt_cfg::apply_cpt_config(dest, f->getResponseCore());
  return rv;
}

bool cpt_cfg::apply_cpt_config(cpt_configuration_type& dest,mclOntology *o) {
  for (nodeList::iterator nli = o->firstNode();
       nli != o->endNode();
       nli++) {
    cpt_cfg::apply_cpt_config((*nli),dest[(*nli)->entityBaseName()]);
  }
}

bool cpt_cfg::apply_cpt_config(mclNode *n,string config) {
  // cout << "applying cpt config for " << n->entityBaseName()
  // << endl;

  if (config.size()==0) {
    mclLogger::annotate(MCLA_WARNING,"no configuration for "+n->entityBaseName());
    return true;
  }

  // parse config string.
  tokenMachine ctm(config);
  ctm.trimParens();
  ctm.nextToken();
  vector<string> deps;
  vector<double> cpv;
  bool running=true;
  while (running) {
    string k = ctm.nextToken();
    string v = ctm.nextToken();
    if ((k.size() == 0) || (v.size() == 0))
      running=false;
    else {
      // :dep keyword specifies incoming links to the current node
      if (k == ":dep") {
	deps=tokenMachine::processAsStringVector(v);
	if (!checkConsistent(n,deps)) {
	  if (mclSettings::getSysPropertyBool("suppressConfigErrors",false)) {
	    mclLogger::annotate(MCLA_WARNING, 
				n->entityName()+":  inconsistent dep vector (using default priors).");
	    return false;
	  }
	  else { 
	    sprintf(mcl_perror,"while applying CPT config: dep vector %s is inconsistent with node %s's incoming link list",v.c_str(),n->entityName().c_str());
	    signalMCLException(mcl_perror);
	  }
	}
      }
      // :cpv keyword specifies conditional probability _vector_
      else if (k == ":cpv") {
	cpv=tokenMachine::processAsDoubleVector(v);
      }
      // those are the only allowable keywords
      else {
	if (ignore_errors) {
	  mclLogger::annotate(MCLA_WARNING, "skipping " + n->entityName() +
			      ": unhandled keyword.");
	}
	else {
	  sprintf(mcl_perror,"while applying CPT config: unhandled keyword '%s'",k.c_str());
	  signalMCLException(mcl_perror);
	}
      }
    }
  }
  // 
  if (checkSorted(n,deps)) {
    // cout << "sort is kosher for CPT reconstruction." << endl;
    n->initPriors(deps,cpv);
  }
  else {
    // cout << "sort is not kosher for CPT reconstruction (resort)." << endl;
    resortAndInitCPT(n,deps,cpv);
  }
}

//! checks to make sure the deps vector is consistent with incoming link
//! list of "n"
bool checkConsistent(mclNode *n,vector<string>& deps) {
  if ((unsigned int)n->inLinkCnt() == deps.size()) {
    for (llIterator nli = n->inLink_begin();
	 nli != n->inLink_end();
	 nli++) {
      vector<string>::iterator fp = find(deps.begin(),deps.end(),
					 (*nli)->sourceNode()->entityBaseName());
      if (fp == deps.end())
	return false;
    }
    /* okay, this is not really admissible in the case where there are 
       duplicates in either the source or target, but that should never 
       happen, right? */
    return true;
  }
  else
    return false;
}

//! checks to ensure that the deps vector is sorted the same way as
//! the incoming link list of "n"
bool checkSorted(mclNode *n,vector<string>& deps) {
  if ((unsigned int)n->inLinkCnt() != deps.size()) {
    sprintf(mclLogger::annotateBuffer,
	    "link count mismatch in checkSorted: %s linkcount= %d  depsize=%d",
	    n->entityName().c_str(), n->inLinkCnt() , deps.size());
    mclLogger::annotateFromBuffer(MCLA_WARNING);
    return false;
  }
  else {
    llIterator nli = n->inLink_begin();
    vector<string>::iterator dvi = deps.begin();
    while ((nli != n->inLink_end()) &&
	   (dvi != deps.end())) {
      if ((*nli)->sourceNode()->entityBaseName() != (*dvi)) {
	return false;
      }
      nli++;
      dvi++;
    }
  }
  return true;
}

//! if this worked it would autosort the deps and cp vectors to match
//! "n"'s incomcing link list so everything would get stored in the right
//! place
bool resortAndInitCPT(mclNode *n,
		      vector<string>& deps,
		      vector<double>& cpv) {
  if (ignore_errors) {
    mclLogger::annotate(MCLA_ERROR,"resort/init failed because resorting is not implemented, "+n->entityName()+" config failed...");
    return false;
  }
  else {
    sprintf(mcl_perror,"Dependency list for %s is improperly ordered in CPT file. resortAndInitCPT() is unimplemented.",n->entityName().c_str());
    signalMCLException(mcl_perror);
  }
}
