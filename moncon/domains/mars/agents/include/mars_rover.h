#ifndef MC_MARS_ROVER_H
#define MC_MARS_ROVER_H

// #include "substrate/domain.h"
#include "substrate/basic_agent.h"
#include "substrate/dispatch_agent.h"
#include "mars_domain.h"
#include "imagebank.h"
#include "substrate/models.h"

#define __MR_IMGBANK_SIZE 76

using namespace MonCon;

namespace mcMars {

  class mars_rover : public basic_agent, public model_conduit {
  public:
    mars_rover(string name, mars_domain* d);
    
    static const int STATUS_OKAY =  0,
      STATUS_BANK_OVERFLOW       = -1,
      STATUS_MOVE_FAILURE        = -2,
      STATUS_NRG_FAILURE         = -3,
      STATUS_CAL_ERROR           = -4,
      STATUS_LOC_ERROR           = -5,
      STATUS_SCI_ERROR           = -6,
      STATUS_ARG_ERROR           = -7,
      STATUS_XMIT_ERROR          = -8,
      STATUS_RC_ERROR            = -9,
      STATUS_UNDEF_ERROR         = -128;

  static const int CALSTATE_UNCAL = -1;
  static int MAX_NRG, MAX_BANK, PANO_IMG, SCI_IMG;

  virtual bool   initialize();
  virtual bool   simulate();

  virtual string process_command(string command);

  virtual string class_name() { return "mars_rover"; };
  virtual string grammar_commname() { return class_name()+"_command"; };
  virtual bool   publish_grammar(umbc::command_grammar& grammar);

  virtual string describe();

  virtual timeline_entry* parse_tle_spec( int firetime, timeline* tl,
					  string tles );  

  virtual bool busy();
  mars_domain* mars_domain_of() { return my_mars_domain; };

  protected:
  bool build_sensors();
  bool build_effectors();
  bool build_repair_actions();
  
  bool parse_action(string actionspec);
  
  image_bank the_bank;
  
  private:
  mars_domain* my_mars_domain;

  };

};

#endif
