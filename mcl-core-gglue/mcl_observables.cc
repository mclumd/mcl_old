#include "mcl_observables.h"
#include "mcl.h"

#include <list>

/***************************************************************
                   start of observable code
***************************************************************/

const char* observable::SELF_OBJNAME = "self";

bool observable::object_name_match(string obj_name,string obs_name) {
  string onps = obj_name+".";
  return (strncmp(onps.c_str(),obs_name.c_str(),onps.length()) == 0);
}

void observable::dumpEntity(ostream *strm) {
  *strm << "< obs " << entityBaseName() << "("
	<< dec << v_double() << ") >" << endl;
}

void basic_observable::dumpEntity(ostream *strm) {
  *strm << "< b_obs " << entityBaseName() << "("
	<< dec << v_double() << ") ";
  sensor_definition->dumpEntity(strm);
  *strm << " >" << endl;
}


/***************************************************************
                   start of object_def code
***************************************************************/

void object_def::add_object_fields_to_ov(observable_vector* ov, 
					 string object_name) {
  for (fieldmap::iterator i = object_fields.begin();
       i != object_fields.end();
       i++) {
    // push into the observable vector
    observable* noob = 
      new basic_observable(MCL(),
			   observable::make_name(object_name,i->first),
			   i->second);
    ov->add_observable(noob);
  }
}

void object_def::add_field_def(string fieldname, mclSensorDef& sd) {
  // IT SHOULD CLONE THE SENSORDEF!!
  if (object_fields[fieldname] != NULL) {
    mclSensorDef* osd = object_fields[fieldname];
    cout << "field def " << fieldname << " being overwritten (destroyed)."
	 << endl;
    delete osd;
  }
  mclSensorDef* nu = sd.clone();
  cout << "adding field def '" << fieldname << "', cloning "
       << hex << &sd << " to " << hex << nu << endl;
  nu->dumpEntity(&cout); cout << endl;
  object_fields[fieldname] = nu;
}

void object_def::set_obs_prop(string fieldname,spkType key, spvType pv) {
  if (object_fields[fieldname]) {
    object_fields[fieldname]->setSensorProp(key,pv);
  }
}

/***************************************************************
                start of observable_vector code
***************************************************************/

void observable_vector::add_observable(observable* obs) {
  ob_map::iterator fp = active.find(obs->entityBaseName());
  if (fp != active.end()) {
    // kill the old one?
    if (fp->second != NULL) { // why would it?
      observable* ootk = fp->second;
      delete ootk;
    }
  }
  active[obs->entityBaseName()]=obs;
}

void observable_vector::add_observable_object_def(object_def* od) {
  od_map::iterator q = object_defs.find(od->entityBaseName());
  if (q != object_defs.end()) {
    delete object_defs[od->entityBaseName()];
    object_defs[od->entityBaseName()]=NULL;
  }
  object_defs[od->entityBaseName()]=od;
}

void observable_vector::observe_observable_object(string defname,string name) {
  if (!recall_observable_object(name)) {
    cout << defname << ":" << name << " not in memory, creating..." << endl;
    observe_new_object(defname,name);
  }
}

void observable_vector::observe_new_object(string defname, string obname) {
  // requires creation of object (not in memory)
  // step 0: retrieve the object def
  object_def* tod = object_defs[defname];
  if (tod != NULL) {
    // step 1: create a mapping from the object name to its type
    object_defmap[obname]=defname;

    // step 2: add all object fields to the active_list
    tod->add_object_fields_to_ov(this,obname);

  }

#ifdef MCL_OBS_ERROR_CHECKING
  else {
    signalMCLException("attempt to observe an undefined object: "+obname+"/"+defname);
  }
#endif

}

bool observable_vector::recall_observable_object(string name) {
  string obj_type = object_defmap[name];
  if (obj_type == "") {
    cout << "failed to find " << name << " in obj_defmap." << endl;
    return false;
  }
  else {
    object_def* objdef = object_defs[obj_type];
    if (objdef == NULL) {
      cout << "failed to find " << obj_type << " in obj_defs." << endl;
      return false;
    }
    else {
      for (fieldmap::iterator i = objdef->fmi_begin();
	   i != objdef->fmi_end();
	   i++) {
	// this is a little messy since there this code is taken from
	// a object_def function (add_obj_fields_to_ov)
	string i_obsname = observable::make_name(name,i->first);
	observable* i_obs_from_mem = memory[i_obsname];
	if (i_obs_from_mem != NULL) {
	  // push into the observable vector
	  add_observable(i_obs_from_mem);
	  memory.erase(i_obsname);
	}
	else {
	  cout << "WARNING: recall should work but field '" 
	       << i->first << "' is missing in observable memory." << endl;
	  observable* noob = 
	    new basic_observable(MCL(),i_obsname,i->second);
	  add_observable(noob);
	}
      }
      return true;
    }
  }
}

void observable_vector::stow_observable_object(string ob_name) {
  // i hate this but erase invalidates the iterator....
  list<string> cache;
  for (ob_map::iterator ai = active.begin();
       ai != active.end();
       ai++) {
    if (observable::object_name_match(ob_name,ai->first))
      cache.push_back(ai->first);
  }
  while (!cache.empty()) {
    observable* item_to_stow = active[cache.front()];
    if (item_to_stow != NULL) {
      memory[cache.front()] = active[cache.front()];
      active.erase(cache.front());
    }
    cache.pop_front();
  }
}


void observable_vector::delete_from_ob_map(ob_map& map,string ob_name) {
  // i hate this but erase invalidates the iterator....
  list<string> cache;
  for (ob_map::iterator ai = map.begin();
       ai != map.end();
       ai++) {
    if (observable::object_name_match(ob_name,ai->first))
      cache.push_back(ai->first);
  }
  while (!cache.empty()) {
    observable* item_to_delete = map[cache.front()];
    if (item_to_delete != NULL) {
      delete item_to_delete;
      map.erase(cache.front());
    }
    cache.pop_front();
  }
}

void observable_vector::delete_observable_object(string name) {
  delete_from_ob_map(active,name);
  delete_from_ob_map(memory,name);
}

void observable_vector::add_object_field_def(string objname,
					     string fieldname,
					     mclSensorDef& sd) {
  object_def* tod = object_defs[objname];
  if (tod != NULL) {
    tod->add_field_def(fieldname,sd);
  }
  else {
#ifdef MCL_OBS_ERROR_CHECKING
    signalMCLException("attempt to add field to an undefined object: "+objname);
#elif
    add_observable_object_def(object_def* od);    
#endif

  }
}

double observable_vector::v_double(string obs) {
  return v_double(observable::SELF_OBJNAME,obs);
}

int    observable_vector::v_int(string obs) {
  return v_int(observable::SELF_OBJNAME,obs);
}

double observable_vector::v_double(string obj,string obs) {
  string obsn = observable::make_name(obj,obs);

#ifdef MCL_OBS_ERROR_CHECKING
  if (active.find(obsn) == active.end())
    signalMCLException("attempt to get value for "+obsn+" failed. not an active observable.");
#endif

  return (active[obsn])->v_double();
}

int observable_vector::v_int(string obj,string obs) {
  string obsn = observable::make_name(obj,obs);

#ifdef MCL_OBS_ERROR_CHECKING
  if (active.find(obsn) == active.end())
    signalMCLException("attempt to get value for "+obsn+" failed. not an active observable.");
#endif
      
  return (active[obsn])->v_int();
}

string observable_vector::vector2string(ob_map& map) {
  string dtw = "";
  if (&map == &active) dtw = "active";
  else if (&map == &memory) dtw = "memory";
  else dtw = "unknown";
  char buff[512];
  string r = "< " + baseClassName() +"("+dtw+")" + " | ";
  for (ob_map::iterator i = map.begin();
       i != map.end();
       i++) {
    if (!(i->second)) {
#ifdef MCL_OBS_ERROR_CHECKING
      signalMCLException("empty "+dtw+" object map lookup "+i->first+" failed. map value is NULL.");
#endif
    }
    else {
      sprintf(buff,"%s=%.2f ",i->first.c_str(),i->second->v_double());
      r+=buff;
    }
  }
  return r+" >";
}

void observable_vector::observable_update_pp(string pp_name, double v) {
  observable* oo = active[pp_name];
  if (oo != NULL) {
    oo->set_v_double(v,MCL()->tickNumber());
  }

#ifdef MCL_OBS_ERROR_CHECKING
  else {
    signalMCLException("attempt to set value for "+pp_name+" failed. not an active observable.");
  }
#endif
  
}

string observable_vector::active2string() {
  return vector2string(active);
}

void observable_vector::dump() {
  cout << vector2string(active) << endl;
  cout << vector2string(memory) << endl;
}

void observable_vector::dump_obs(string obs) {
  string kee = observable::make_self_named(obs);
  if (active.find(kee) != active.end()) {
    active[kee]->dumpEntity(&cout);
  }
  else if (memory.find(kee) != memory.end()) {
    memory[kee]->dumpEntity(&cout);    
  }
  else cout << "<unknown observable " << kee << ">" << endl;
}

void observable_vector::dump_obs(string obj,string obs) {
  string kee = observable::make_name(obj,obs);
  if (active.find(kee) != active.end()) {
    active[kee]->dumpEntity(&cout);
  }
  else if (memory.find(kee) != memory.end()) {
    memory[kee]->dumpEntity(&cout);    
  }
  else cout << "<unknown observable " << kee << ">" << endl;
}

void observable_vector::set_obs_prop_self(string name,spkType key,spvType pv) {
  // THIS WON'T WORK!!
  //  set_obs_prop(observable::SELF_OBJNAME,name,key,pv);

  string kee = observable::make_self_named(name);
  if (active.find(kee) != active.end()) {
    active[kee]->set_obs_prop(key,pv);
    cout << "active prop reset: " << kee << endl;
  }
}

void observable_vector::set_obs_prop(string obj_type_name,string obsname,
				     spkType key,spvType pv) {
  cout << "prop set in progress: " << obj_type_name 
       << "." << obsname << " = [" << key << ":" 
       << dec << (int)pv << "]" << endl;
  // okay, this is actually a little hard because we need to find
  // all object defs *and* active/memorized observables that match

  // first, fix object def (if any)
  if (object_defs.find(obj_type_name) != object_defs.end()) {
    object_defs[obj_type_name]->set_obs_prop(obsname,key,pv);
    cout << "def prop reset: " << obj_type_name 
	 << "." << obsname << " = [" << key << ":" << dec << (int)pv << "]" << endl;
  }

  // next, fix active objects and memory
  for (string_lookup::iterator sli = object_defmap.begin();
       sli != object_defmap.end();
       sli++) {
    if (sli->second == obj_type_name) {
      string kee = observable::make_name(sli->first,obsname);
      if (active.find(kee) != active.end()) {
	active[kee]->set_obs_prop(key,pv);
	cout << "active prop reset: " << kee << endl;
      }
      else if (memory.find(kee) != memory.end()) {
	memory[kee]->set_obs_prop(key,pv);    
	cout << "memory prop reset: " << kee << endl;
      }
    }
  }
}

mclSensorDef* observable_vector::get_sensorDef_pp(string obs) {
  cout << " attempting to look up " << obs << endl;
  if (active.find(obs) != active.end()) {
    // here we have to assume that it's a basic_observable so we can get
    // the sensorDef
    return ((basic_observable*)active[obs])->get_sensorDef();
  }
  else return NULL;
}

/***************************************************************
                      start of API code
***************************************************************/

////////////////// this is temporary stuff until the real api gets written
