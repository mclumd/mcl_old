#include "mcl.h"
#include "mcl_multiagent_api.h"
#include "mcl_observables.h"

mcl* get_or_create(string k);

// mcl test_mcl;
// observable_vector glob_ov(&test_mcl,"globalvec");

void mclMA::observables::declare_observable_self(string k,
						 string name, int def_v) {
  mcl* tmcl = get_or_create(k);
  string mapname = 
    observable::make_self_named(name);
  observable* noo = new basic_observable(tmcl,mapname,def_v);
  tmcl->add_observable(noo);
}

void mclMA::observables::declare_observable_object_type(string k,
							string tname) {
  mcl* tmcl = get_or_create(k);
  object_def* nod = new object_def(tmcl,tname);
  tmcl->add_observable_object_def(nod);
}

void mclMA::observables::declare_object_type_field(string k,
						   string tname,string fname) {
  mcl* tmcl = get_or_create(k);
  // we can use a locally scoped object because the sensorDef is cloned !!
  mclSensorDef sdef(fname,tmcl);
  tmcl->add_object_field_def(tname,fname,sdef);
}

void mclMA::observables::notice_object_observable(string k,
						  string obj_type,
						  string obj_name) {
  mcl* tmcl = get_or_create(k);
  tmcl->notice_object_is_observable(obj_type,obj_name);
}

void mclMA::observables::notice_object_unobservable(string k,
						    string obj_name) {
  mcl* tmcl = get_or_create(k);
  tmcl->notice_object_unobservable(obj_name);
}

void mclMA::observables::update_observable(string k,
					   string obs_name,double v) {
  mcl* tmcl = get_or_create(k);
  tmcl->observable_update(obs_name,v);
}

void mclMA::observables::update_observable(string k,
					   string obj_name,
					   string obs_name,
					   double v) {
  mcl* tmcl = get_or_create(k);
  tmcl->observable_update(obj_name,obs_name,v);
}

void mclMA::observables::dump_globo(string k) {
  mcl* tmcl = get_or_create(k);
  tmcl->dump_ov();
}

string mclMA::observables::ov_as_string(string k) {
  mcl* tmcl = get_or_create(k);
  return tmcl->ov2string();
}

void mclMA::observables::set_obs_prop_self(string k,string name,
					   spkType key,spvType pv) {
  mcl* tmcl = get_or_create(k);
  tmcl->set_obs_prop_self(name,key,pv);  
}

void mclMA::observables::set_obs_prop(string k,
				      string objname,string obsname,
				      spkType key,spvType pv) {
  mcl* tmcl = get_or_create(k);
  tmcl->set_obs_prop(objname,obsname,key,pv);  
}

void mclMA::observables::dump_obs_self(string k,string obsname) {
  mcl* tmcl = get_or_create(k);
  tmcl->dump_obs(obsname);
}

void mclMA::observables::dump_obs(string k,
				  string objname,string obsname) {
  mcl* tmcl = get_or_create(k);
  tmcl->dump_obs(objname,obsname);
}

void mclMA::observables::set_obs_noiseprofile_self(string mclkey,string obsnm,
						   spvType npKey) {
  mcl* tmcl = get_or_create(mclkey);
}

void mclMA::observables::set_obs_noiseprofile_self(string mclkey,string obsnm,
						   spvType npKey,double prm) {
  mcl* tmcl = get_or_create(mclkey);
} 

void mclMA::observables::set_obj_obs_noiseprofile(string mclkey,
						  string objnm,string obsnm,
						  spvType npKey) {
  mcl* tmcl = get_or_create(mclkey);
}

void mclMA::observables::set_obj_obs_noiseprofile(string mclkey,
						  string objnm,string obsnm,
						  spvType npKey,double param) {
  mcl* tmcl = get_or_create(mclkey);
}

void mclMA::observables::apply_update(string key, update& the_update) {
  mcl* tmcl = get_or_create(key);
  for (update_iterator ui = the_update.begin();
       ui != the_update.end();
       ui++) {
    tmcl->observable_update_pp(the_update.pp_key(ui),the_update.value(ui));
  }
}

// THE UPDATE CLASS


void mclMA::observables::update::set_update(string obj, string obs, double v)
{ u_table[observable::make_name(obj,obs)]=v; };

void mclMA::observables::update::set_update(string obs, double v)
{ u_table[observable::make_self_named(obs)]=v; };

double mclMA::observables::update::get_update(string obj, 
							 string obs)
{ return u_table[observable::make_name(obj,obs)]; };
