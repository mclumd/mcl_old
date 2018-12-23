#ifndef MC_SUBSTRATE_GOAL_SPEC_H
#define MC_SUBSTRATE_GOAL_SPEC_H

#include <list>
using namespace std;

typedef unsigned int goalspec;
typedef unsigned int goalterm;
#define GS_NOACT  (goalterm)0xFFFF
#define GS_NOARG  (goalterm)0xFFFF

inline goalspec make_gs(int action,int arg) { 
  goalspec rv =  (( (action&0xFFFF) << 16) | (arg&0xFFFF));
  return rv;
}

inline goalspec make_gs(int action) { 
  return make_gs(action,GS_NOARG);
}

inline goalspec make_nongoal() { 
  return make_gs(GS_NOACT,GS_NOARG);
}

inline bool gs_equal(goalspec g1,goalspec g2) { return (g1 == g2); };

inline goalterm gs_action(goalspec gs) {
  return ((gs & 0xFFFF0000) >> 16);
}

inline goalterm gs_arg(goalspec gs) {
  return (gs & 0x0000FFFF);
}

inline bool gs_noarg(goalspec gs) {
  return (gs_arg(gs) == GS_NOARG);
}

inline bool gs_noact(goalspec gs) {
  return (gs_action(gs) == GS_NOACT);
}

inline bool gs_nongoal(goalspec gs) {
  return ((gs_action(gs) == GS_NOACT) && (gs_arg(gs) == GS_NOARG));
}

class has_goal_queue {
 public:
  has_goal_queue() : 
    most_recent_goal(make_nongoal()),
    current_goal(make_nongoal()) {};

  bool goals_pending() { return !goal_queue.empty(); };
  int  n_goals_pending() { return goal_queue.size(); };
  void addGoal(goalspec g) { goal_queue.push_back(g); };
  void clear_goals() { goal_queue.clear(); };
  goalspec dequeue_goal() { 
    goalspec rv = goal_queue.front();
    goal_queue.pop_front();
    return rv;
  };
  
  goalspec current_goalspec() { return current_goal; };
  goalspec most_recent_goalspec() { return most_recent_goal; };
  
 protected:
  list<goalspec> goal_queue;
  goalspec  most_recent_goal;
  goalspec  current_goal;

};

#endif
