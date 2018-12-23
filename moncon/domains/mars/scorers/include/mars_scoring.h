#ifndef MARS_SCORING_H
#define MARS_SCORING_H

#include "umbc/scoring.h"

namespace mcMars {

  class mars_scorer : public umbc::decl_based_scorer {
  public:
    mars_scorer() : decl_based_scorer() { add_default_rules(); };
    mars_scorer(string fn) : decl_based_scorer(fn) { add_default_rules(); };
    
  protected:
    void add_default_rules();
    
  };

};

#endif
