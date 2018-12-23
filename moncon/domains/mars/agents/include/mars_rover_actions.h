#ifndef MARS_ACT_ACT_H
#define MARS_ACT_ACT_H

#include "imagebank.h"
#include "mars_agent_actions.h"
#include "substrate/models.h"

using namespace MonCon;

namespace mcMars {

enum { MR_MOVE_TO,MR_RECHARGE,MR_TAKE_PANORAMIC,MR_DO_SCIENCE,MR_XMIT,
       MR_LOCALIZE,MR_CALIBRATE,MR_NUMBER_OF_ACTIONS };

enum { MR_REBUILD=MR_NUMBER_OF_ACTIONS,MR_S_RESET,MR_E_RESET,
       MR_RESCUE,
       MR_ALL_ACTIONS_COUNT};

 class mars_rover;

 class mars_rover_action : public mars_domain_action, public model_is_provided {
 public:
   mars_rover_action(string nm,int key,mars_rover* a);
   mars_rover_action(string nm,int key,mars_rover* a,int argc);

   virtual string grammar_commname() { return "rover_"+get_name()+"_command"; };
   static int action_name2code(string name);
   mars_rover* rover_of() { return my_rover; };

 protected:
   mars_rover* my_rover;

};

class mars_rover_imaging_action : public mars_rover_action {
 public:
  mars_rover_imaging_action(string name,int key,mars_rover* a,
		    image_bank& bank,int argc) : 
    mars_rover_action(name,key,a,argc),my_imagebank(bank) {};
 protected:
  image_bank& my_imagebank;

};

class mars_moveto : public mars_rover_action {
 public:
  mars_moveto(mars_rover* a) : mars_rover_action("moveto",MR_MOVE_TO,a,1) {};
  virtual bool simulate();
  virtual string describe();
  virtual string class_name() { return "mr_moveto"; };

  virtual string grammar_argspec();
  virtual bool   publish_grammar(umbc::command_grammar& cg);

  // mcl_action_model* cheat_get_model();

  // cost is inherited...

};

class mars_take_panoramic : public mars_rover_imaging_action {
 public:
  mars_take_panoramic(mars_rover* a,image_bank& ib) : mars_rover_imaging_action("pano",MR_TAKE_PANORAMIC,a,ib,0) 
    { nrg_cost=8; };
  virtual bool simulate();
  // mcl_action_model* cheat_get_model();
  virtual string describe();
  virtual string class_name() { return "mr_take_panoramic"; };
};

class mars_do_science : public mars_rover_imaging_action {
 public:
  mars_do_science(mars_rover* a,image_bank& ib) : mars_rover_imaging_action("science",MR_DO_SCIENCE,a,ib,1) 
    { nrg_cost=5; };
  virtual bool simulate();
  virtual string describe();
  virtual string class_name() { return "mr_do_science"; };

  virtual string grammar_argspec();
  virtual bool   publish_grammar(umbc::command_grammar& grammar);

  // mcl_action_model* cheat_get_model();

};

class mars_calibrate : public mars_rover_action {
 public:
  mars_calibrate(mars_rover* a) : mars_rover_action("cal",MR_CALIBRATE,a,1) 
    { nrg_cost=4; };
  virtual bool simulate();
  // mcl_action_model* cheat_get_model();
  virtual string describe();
  virtual string class_name() { return "mr_calibrate"; };

  virtual string grammar_argspec();
  virtual bool   publish_grammar(umbc::command_grammar& grammar);

};

class mars_localize : public mars_rover_action {
 public:
  mars_localize(mars_rover* a) : mars_rover_action("loc",MR_LOCALIZE,a) 
    { nrg_cost=5; };
  virtual bool simulate();
  // mcl_action_model* cheat_get_model();
  virtual string describe();
  virtual string class_name() { return "mr_localize"; };
};

class mars_recharge : public mars_rover_action {
 public:
  mars_recharge(mars_rover* a) : mars_rover_action("charge",MR_RECHARGE,a) 
    { nrg_cost=0; };
  virtual bool simulate();
  // mcl_action_model* cheat_get_model();
  virtual string describe();
  virtual string class_name() { return "mr_recharge"; };
};

class mars_xmit : public mars_rover_imaging_action {
 public:
  mars_xmit(mars_rover* a,image_bank& ib) : mars_rover_imaging_action("xmit",MR_XMIT,a,ib,0) 
    { nrg_cost=8; };
  virtual bool simulate();
  // mcl_action_model* cheat_get_model();
  virtual string describe();
  virtual string class_name() { return "mr_xmit"; };
};

/////////////////////////////////////////////////////////////
/// repairs
/////////////////////////////////////////////////////////////

 class rover_repair : public mars_repair_action {
 public:
 rover_repair(string nm,int code,mars_rover* mr) : 
   mars_repair_action(nm,code,mr),mr_response_code(MRC_INIT),my_rover(mr) { };

   enum rcode { MRC_INIT = -1, MRC_FAILED , MRC_SUCCESS };
   void reset_rcode() { mr_response_code = MRC_INIT; };
   bool rcode_success() { return (mr_response_code == MRC_SUCCESS); };

 protected:
  mars_rover* rover_of() { return my_rover; };
  rcode       mr_response_code;

 private:
  mars_rover* my_rover;

};

class mars_rebuild_models : public rover_repair {
 public:
  mars_rebuild_models(mars_rover* a) : 
  rover_repair("rebuild",MR_REBUILD,a) 
    { };
  virtual bool simulate();
  // mcl_action_model* cheat_get_model();
  virtual string describe();
  virtual string class_name() { return "mr_rebuild_models"; };

};

class mars_sensor_reset : public rover_repair {
 public:
  mars_sensor_reset(mars_rover* a) : 
    rover_repair("sensReset",MR_S_RESET,a) 
    { };
  virtual bool simulate();
  // mcl_action_model* cheat_get_model();
  virtual string describe();
  virtual string class_name() { return "mr_sensor_reset"; };

};

class mars_effector_reset : public rover_repair {
 public:
  mars_effector_reset(mars_rover* a) : 
    rover_repair("effReset",MR_E_RESET,a) 
    { };
  virtual bool simulate();
  // mcl_action_model* cheat_get_model();
  virtual string describe();
  virtual string class_name() { return "mr_effector_reset"; };
};

class mars_rescue : public rover_repair {
 public:
  mars_rescue(mars_rover* a) : 
    rover_repair("rescue",MR_RESCUE,a) 
    { };
  virtual bool simulate();
  // mcl_action_model* cheat_get_model();
  virtual string describe();
  virtual string class_name() { return "mr_rescue"; };
};

};

#endif
