#include "umbc/dijkstra.h"
#include "umbc/declarations.h"
#include "umbc/settings.h"
#include "mcl/mcl_api.h"

#include "mars_events.h"

#include <math.h>

using namespace std;
using namespace mcMars;

#include "mars_logging.h"
using namespace umbc;

string mars_event_names[] = { "changeDist", "addEdge", "delEdge",
			      "moveCharge", "moveLoc", "moveSci",
			      "changeCost" };

double mars_anomaly::random_anomaly_baserate=0.01;

// scales a move_cost from 67% to 133%
// seed-ized, complete
void mars_anomaly::change_distance(long seed) {
  struct drand48_data bigseed;
  long xseed = (seed == 0) ? ((long)time(NULL)) : seed;
  srand48_r(xseed,&bigseed);

  // search randomly for an existing path from S to D
  for (int i=0;i<100;i++) {
    double rolls,rolld;
    drand48_r(&bigseed,&rolls);
    drand48_r(&bigseed,&rolld);
    int s = (int)floor(rolls*(double)domain_of()->get_num_waypoints());
    int d = (int)floor(rolld*(double)domain_of()->get_num_waypoints());

    if (domain_of()->move_cost(s,d) != mars_domain::unreachable_cost()) {
      int oc = domain_of()->move_cost(s,d);
      double rollm;
      drand48_r(&bigseed,&rollm);
      domain_of()->set_reachable(s,d,
			(int)((double)domain_of()->move_cost(s,d)*(rollm+1.0)*0.667));
      sprintf(uLog::annotateBuffer,"[mars/events]::move cost @edge=(%d->%d) changed from %d to %d",s,d,oc,domain_of()->move_cost(s,d));
      uLog::annotateFromBuffer(MARSL_HOSTEVENT);
      return;
    }
  }
}

#ifdef BENIGN_EDGE_MANIPULATION
typedef int nu_edge;
list<nu_edge> added_edges;
inline int source(nu_edge e) { return ((e & 0xFFFF0000) >> 16); }
inline int destin(nu_edge e) { return  (e & 0x0000FFFF); }
inline nu_edge makedge(int s,int d) { return ((s << 16) | d); }
#endif

// seed-ized, completed
void mars_anomaly::add_edge(long seed) {
  struct drand48_data bigseed;
  long xseed = (seed == 0) ? ((long)time(NULL)) : seed;
  srand48_r(xseed,&bigseed);

  for (int i=0;i<100;i++) {
    double rolls,rolld;
    drand48_r(&bigseed,&rolls);
    drand48_r(&bigseed,&rolld);
    int s = (int)floor(rolls*(double)domain_of()->get_num_waypoints());
    int d = (int)floor(rolld*(double)domain_of()->get_num_waypoints());

    if (domain_of()->move_cost(s,d) == mars_domain::unreachable_cost()) {
      double rollm;
      drand48_r(&bigseed,&rollm);
      domain_of()->set_reachable(s,d,
			(int)(10*(( rollm *.4)+0.8)));
      sprintf(uLog::annotateBuffer,"[mars/event]::added edge=(%d->%d) cost=%d",s,d,domain_of()->move_cost(s,d));
      uLog::annotateFromBuffer(MARSL_HOSTEVENT);
      #ifdef BENIGN_EDGE_MANIPULATION
      // *log::logStream << "edge descriptor: " << makedge(s,d) << endl;
      added_edges.push_back(makedge(s,d));      
      #endif
      return;
    }
  }
}  

//! hardcore-but-safe delete edge.
//! deletes *any* edge so long as it does not cause break up
//! the fully-connectedness of the graph.
// seed-ized, completed
void mars_anomaly::hbs_delete_edge(long seed) {
  struct drand48_data bigseed;
  long xseed = (seed == 0) ? ((long)time(NULL)) : seed;
  srand48_r(xseed,&bigseed);

  for (int i=0;i<200;i++) {
    double rolls,rolld;
    drand48_r(&bigseed,&rolls);
    drand48_r(&bigseed,&rolld);
    int s = (int)floor(rolls*(double)domain_of()->get_num_waypoints());
    int d = (int)floor(rolld*(double)domain_of()->get_num_waypoints());

    if (domain_of()->move_cost(s,d) != mars_domain::unreachable_cost()) {
      int omc = domain_of()->move_cost(s,d);
      domain_of()->set_reachable(s,d,mars_domain::unreachable_cost());
      long cost;
      list<int> alt_path = umbc::dijkstra::shortestPath(domain_of()->move_cost_mx(),
							domain_of()->get_num_waypoints(),
							s,d,&cost);
      // this looks a little kludgy because it is
      if ((cost < mars_domain::unreachable_cost()) && (cost != -1)) {
	// okay to delete edge
	sprintf(uLog::annotateBuffer,"[mars/events]::deleting edge (%d->%d), still reachable for %ld",s,d,cost);
	uLog::annotateFromBuffer(MARSL_HOSTEVENT);
	return;
      }
      else {
	// re-set the edge
	domain_of()->set_reachable(s,d,omc);
      }
    }
  }
  uLog::annotate(MARSL_HOSTEVENT,"[mars/event]::could not find safe edge to delete");
}

// softcore delete edge -- only deletes edges that have been previously
// added with add_edge
// seed-ized, completed
void mars_anomaly::delete_edge(long seed) {
  struct drand48_data bigseed;
  long xseed = (seed == 0) ? ((long)time(NULL)) : seed;
  srand48_r(xseed,&bigseed);

  #ifdef BENIGN_EDGE_MANIPULATION
  if (added_edges.empty())
    uLog::annotate(MARSL_HOSTEVENT,"[mars/events]::not safe to delete an edge yet");
  else {
    sprintf(uLog::annotateBuffer,"[mars/events]::% edges have been added.",added_edges.size());
    uLog::annotateFromBuffer(MARSL_HOSTEVENT);	    
    double rolle;
    drand48_r(&bigseed,&rolle);
    int etg = (int)(rolle*added_edges.size());
    list<int>::iterator i=added_edges.begin();
    for (int k=0;k<etg;k++)
      i++;
    nu_edge q = *i;
    sprintf(uLog::annotateBuffer,"[mars/events]::deleting nu edge (%d->%d)",source(q),destin(q));
    uLog::annotateFromBuffer(MARSL_HOSTEVENT);
    domain_of()->set_reachable(source(q),destin(q),
			       mars_domain::unreachable_cost());
    added_edges.erase(i);
    sprintf(uLog::annotateBuffer,"[mars/events]::%d added edges remain.",
	    added_edges.size());
  }
  #else
  uLog::annotate(MARSL_HOSTEVENT,"[mars/events]::not sensible to call delete_edge without BENIGN_EDGE_MANIPULATION defined");
  #endif
}

// seed-ized, completed.
void mars_anomaly::move_recharge(long seed) {
  struct drand48_data bigseed;
  long xseed = (seed == 0) ? ((long)time(NULL)) : seed;
  srand48_r(xseed,&bigseed);

  for (int i=0;i<100;i++) {
    double roll1,roll2;
    drand48_r(&bigseed,&roll1);
    drand48_r(&bigseed,&roll2);
    int oc = (int)floor(roll1*(double)domain_of()->get_num_waypoints());
    int nc = (int)floor(roll2*(double)domain_of()->get_num_waypoints());

    // *log::logStream << "mrc: oc=" << oc << " nc=" << nc << endl;
    if (domain_of()->is_rechargable(oc) && !domain_of()->is_rechargable(nc)) {
      domain_of()->set_rechargable(oc,false);
      domain_of()->set_rechargable(nc,true);
      sprintf(uLog::annotateBuffer,"[mars/events]::recharge point changed from %d to %d", oc ,nc);
      uLog::annotateFromBuffer(MARSL_HOSTEVENT);
      return;
    } 
  }
}

// seed-ized, completed
void mars_anomaly::move_localize(long seed) {
  struct drand48_data bigseed;
  long xseed = (seed == 0) ? ((long)time(NULL)) : seed;
  srand48_r(xseed,&bigseed);

  for (int i=0;i<100;i++) {
    double roll1,roll2;
    drand48_r(&bigseed,&roll1);
    drand48_r(&bigseed,&roll2);
    int oc = (int)floor(roll1*(double)domain_of()->get_num_waypoints());
    int nc = (int)floor(roll2*(double)domain_of()->get_num_waypoints());

    if (domain_of()->is_localizable(oc) && !domain_of()->is_localizable(nc)) {
      domain_of()->set_localizable(oc,false);
      domain_of()->set_localizable(nc,true);
      sprintf(uLog::annotateBuffer,"[mars/events]::localize point changed from %d to %d", oc ,nc);
      uLog::annotateFromBuffer(MARSL_HOSTEVENT);
      return;
    } 
  }
}

// seems like a pain in the ass to implement...
void mars_anomaly::move_science(long seed) {
  sprintf(uLog::annotateBuffer,"[mars/events]::science point not changed (unimplemented)");
  uLog::annotateFromBuffer(MARSL_HOSTEVENT);
}

void mars_anomaly::force_event(int q,long seed) {
  umbc::declarations::make_declaration(1,"eventTotal");
  switch (q) {
  case ME_MOVE_RECHARGE:
    move_recharge(seed);
    break;
  case ME_MOVE_LOCALIZE:
    move_localize(seed);
    break;
  case ME_MOVE_SCIENCE:
    move_science(seed);
    break;
  case ME_CHANGE_DIST:
    change_distance(seed);
    break;
  case ME_ADD_EDGE:
    add_edge(seed);
    break;
  case ME_DELETE_EDGE:
#ifdef BENIGN_EDGE_MANIPULATION
    delete_edge(seed);
#else
    hbs_delete_edge(seed);
#endif
    break;
  default:
    uLog::annotate(MARSL_HOSTERR,"[MARS/event]::event generated pwned (event code not impleented)");
    break;
  }
} 

void mars_anomaly::simulate_random_events(mars_domain* d) {
  static double baserate=settings::getSysPropertyDouble("mars.eventprob",
							random_anomaly_baserate);
  double roll = ((double)rand()/(double)RAND_MAX);
  if (roll < baserate) {
    int q=(int)(((double)rand()/(double)RAND_MAX) *
		(double)ME_NUMBER_OF_EVENT_TYPES);
    uLog::annotate(MARSL_HOSTEVENT,"MARS: random event triggered");
    mars_anomaly a(d);
    a.force_event(q);
    umbc::declarations::make_declaration(1,"eventRandom");
  }
}

int mars_anomaly::lookup_anomaly_code(string q) {
  for (int i=0;i<ME_NUMBER_OF_EVENT_TYPES;i++)
    if (q == mars_event_names[i])
      return i;
  return -1;
}
