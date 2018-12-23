#ifndef MARS_EVENTS_H
#define MARS_EVENTS_H

#include "mars_domain.h"

namespace mcMars {

  class mars_anomaly {

  public:
    static double random_anomaly_baserate;
    enum mars_anomaly_code { ME_CHANGE_DIST=0,
			     ME_ADD_EDGE,
			     ME_DELETE_EDGE,
			     ME_MOVE_RECHARGE,
			     ME_MOVE_LOCALIZE,
			     ME_MOVE_SCIENCE,
			     ME_NUMBER_OF_EVENT_TYPES
    };

  mars_anomaly(mars_domain* d) : mdo(d) {};
    
    mars_domain* domain_of() { return mdo; };
    void force_event(int event_code,long seed=0);
    
    void change_distance(long seed=0);
    void add_edge(long seed=0);  
    void delete_edge(long seed=0);
    void hbs_delete_edge(long seed=0);
    void move_recharge(long seed=0);
    void move_localize(long seed=0);
    void move_science(long seed=0);
    void change_cost(long seed=0);

    static void simulate_random_events(mars_domain* d);
    static int lookup_anomaly_code(string q);
    
  private:
    mars_domain* mdo;

  };

};

#endif
