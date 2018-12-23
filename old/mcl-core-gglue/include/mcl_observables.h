#ifndef MCL_OBSERVABLES_H
#define MCL_OBSERVABLES_H

#include "mclEntity.h"
#include "mclSensorDef.h"
#include <map>

#define MCL_OBS_ERROR_CHECKING 

/** \file
 * \brief class definitions for MCL's observable database
 */

class observable;
class observable_vector;
class object_def;

class observable : public mclEntity {
 public:  
  static const char* SELF_OBJNAME;

  observable(string nm) : mclEntity(nm) {};
  virtual double v_double()=0;
  virtual int    v_int()=0;
  virtual int    last_set()=0;
  virtual void   set_obs_prop(spkType key, spvType pv)=0;

  virtual void set_v_double(double v, int time_v)=0;
  virtual void set_v_int(int v,int time_v)=0;

  static string make_name(string obj_n, string obs_n)
    { return obj_n+"."+obs_n; };
  static string make_self_named(string obs_n) 
    { return make_name(SELF_OBJNAME,obs_n); };
  static bool object_name_match(string obj_name,string obs_name);

  void dumpEntity(ostream *strm);

};

class basic_observable : public observable {
 public:
  basic_observable(mcl* m,string nm, double def_v=0.0) : 
    observable(nm),cached_value(def_v),last_set_v(0) {
    sensor_definition = new mclSensorDef(nm,MCL());
  };

  basic_observable(mcl* m,string nm,mclSensorDef* md,double def_v=0.0) : 
    observable(nm),cached_value(def_v),last_set_v(0) {
    sensor_definition = md->clone();
  };

  virtual double v_double() { return cached_value; };
  virtual int   v_int()     { return (int)cached_value; };
  virtual int   last_set()  { return last_set_v; };

  virtual void set_v_double(double v, int time_v) 
    {  cached_value = v; last_set_v = time_v; };
  virtual void set_v_int(int v,int time_v)
    { cached_value = (double)v; last_set_v = time_v; };

  virtual void   set_obs_prop(spkType key, spvType pv)
    { sensor_definition->setSensorProp(key,pv); };

  virtual mcl* MCL() { return my_mcl; };
  virtual string baseClassName() { return "basic_observable"; };

  void dumpEntity(ostream *strm);

  mclSensorDef* get_sensorDef()
    { return sensor_definition; };

 protected:
  double        cached_value;
  int           last_set_v;
  mclSensorDef* sensor_definition;
  mcl*          my_mcl;

};

/** OBJECT DEFINITIONS
 */

typedef map<string,mclSensorDef*> fieldmap;

class object_def : public mclEntity {
 public:
  object_def(mcl* m,string nm) : mclEntity(nm),my_mcl(m) {};

  void add_field_def(string fieldname, mclSensorDef& fsd);
  void add_object_fields_to_ov(observable_vector* ov, string object_name);

  fieldmap::iterator fmi_begin() { return object_fields.begin(); };
  fieldmap::iterator fmi_end()   { return object_fields.end(); };

  virtual mcl* MCL() { return my_mcl; };
  virtual string baseClassName() { return "obj_def"; };

  void set_obs_prop(string obsname,spkType key, spvType pv);

 protected:
  fieldmap object_fields;
  
  mcl* my_mcl;
  
};


/** OBJECT VECTOR
 */

typedef map<string,object_def*> od_map;
typedef map<string,observable*> ob_map;
typedef map<string,string>      string_lookup;

class observable_vector : public mclEntity {
 public:

  observable_vector(mcl* its_mcl, string vname) : 
    mclEntity(vname),my_mcl(its_mcl) {};

  double v_double(string obs);
  int    v_int(string obs);
  double v_double(string object,string obs);
  int    v_int(string object,string obs);

  void add_observable(observable* def);

  void add_observable_object_def(object_def* od);
  void add_object_field_def(string objname,string fieldname,mclSensorDef& sd);

  void observe_observable_object(string name, string defname);
  void stow_observable_object(string name);

  bool recall_observable_object(string name);
  void delete_observable_object(string name);

  void observable_update(string obs_name,double v) 
    { observable_update(observable::make_self_named(obs_name),v); };
  void observable_update(string obj_name,string obs_name,double v)
    { observable_update_pp(observable::make_name(obj_name,obs_name),v); };
  void observable_update_pp(string pp_name,double v);

  void set_obs_prop_self(string name,spkType key,spvType pv);
  void set_obs_prop(string objname,string obsname,spkType key,spvType pv);

  void dump_obs(string obsname);
  void dump_obs(string objname, string obsname);

  // void activate_observable_object(string name);
  // void deactivate_observable_object(string name);

  string active2string();
  void   dump();

  virtual mcl* MCL() { return my_mcl; };
  virtual string baseClassName() { return "obs_vec"; };

  // the following are PP (pre-processed), meaning the key string has
  // been pre-processed... they are mainly for use by mcl for checking
  // up on expectation details
  mclSensorDef* get_sensorDef_pp(string obs);

 protected:
  ob_map        active;
  ob_map        memory;
  od_map        object_defs;
  string_lookup object_defmap; // objectname->definename

  mcl* my_mcl;

  string vector2string(ob_map& map);
  void observe_new_object(string defname, string obname);
  void delete_from_ob_map(ob_map& map,string ob_name);

};

#endif
