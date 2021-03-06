#include "ontology_config.h"
#include "mclLogging.h"
#include "umbc/exceptions.h"
#include "mcl.h"
#include "mclFrame.h"

#include <map>

using namespace std;
using namespace metacog;

map<string,ontology_configuration*> _config_base;

string ontology_configurator::require_config(string domain) {
  string id = ontology_id(domain);
  umbc::uLog::annotate(MCLA_MSG,"[mcl/oc]:: config required: "+id);
  if (_config_base.find(id) == _config_base.end()) {
    _config_base[id]=new ontology_configuration(domain);
  }
  return id;
}

string ontology_configurator::require_config(string domain, string agent) {
  string id = ontology_id(domain,agent);
  umbc::uLog::annotate(MCLA_MSG,"[mcl/oc]:: config required: "+id);
  if (_config_base.find(id) == _config_base.end()) {
    _config_base[id]=new ontology_configuration(domain,agent);
  }
  return id;
}

string ontology_configurator::require_config(string domain, string agent, string controller) {
  string id = ontology_id(domain,agent,controller);
  umbc::uLog::annotate(MCLA_MSG,"[mcl/oc]:: config required: "+id);
  if (_config_base.find(id) == _config_base.end()) {
    _config_base[id]=
      new ontology_configuration(domain,agent,controller);
  }
  return id;
}

string ontology_configurator::cKey2path(string cKey) {
  return cKey;
}
  
bool ontology_configurator::apply_config(mclFrame* f) {
  // first try to get the frame-parent's cKey
  if (f && f->MCL()) {
    umbc::uLog::annotate(MCLA_MSG,
			"[mcl/oc]:: "+ f->MCL()->entityName() + 
			" attempting to apply config '" +
			f->MCL()->getConfigKey() + "'....");
    if (f->MCL()->getConfigKey() == "") {
      umbc::uLog::annotate(MCLA_WARNING,
			  "[mcl/oc]:: mcl " + 
			  f->MCL()->entityName() +
			  " not configurated properly, using default config.");
      f->MCL()->setConfigKey(require_config("default"));
    }
    ontology_configuration* toc = _config_base[f->MCL()->getConfigKey()];
    if (toc == NULL) {
      umbc::exceptions::signal_exception("configuration key '"+f->MCL()->getConfigKey()+"' is empty in _config_base.");
    }
    else {
      toc->apply_rc_config(f);
      toc->apply_cpt_config(f);
      umbc::uLog::annotate(MCLA_MSG,"[mcl/oc]:: configuration applied.");
    }
  }
  else {
    umbc::exceptions::signal_exception("attempted to restore configuration for NULL frame/MCL.");
  }
}

string ontology_configurator::ontology_id(string domain) {
  return domain;
}
string ontology_configurator::ontology_id(string domain, string agent) {
  return domain+"/"+agent;
}
string ontology_configurator::ontology_id(string domain, string agent, string controller) {
  return domain+"/"+agent+"/"+controller;
}

string ontology_configurator::autoPath(mclEntity* m,string fn) {
  if (m && m->MCL()) {
    if (m->MCL()->getConfigKey() == "") {
      umbc::uLog::annotate(MCLA_WARNING,
			  "[mcl/oc]:: autoPath used for unconfigured MCL object " + m->entityName() + " -- using path 'unconfig'");
      m->MCL()->setConfigKey("unconfig");
    }
    return cKey2path(m->MCL()->getConfigKey())+"/"+fn;
  }
  else {
    umbc::uLog::annotate(MCLA_ERROR,
			"[mcl/oc]:: autoPath used for lost (NULL/unattached) MCL object " + m->entityName() + " -- using path 'lost'");
    return "lost/"+fn;
  }  
}
