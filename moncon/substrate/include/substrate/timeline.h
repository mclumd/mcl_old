#ifndef MCS_TIMELINE_INPUT_H
#define MCS_TIMELINE_INPUT_H

#include <stdlib.h>

#include "substrate/simEntity.h"
#include "substrate/timeline_aware.h"
#include "substrate/domain.h"

#include "umbc/declarations.h"

#include <vector>
#include <string>
using namespace std;

extern char timeline_error_msg[1024];

namespace MonCon {

typedef int simTimeType;

class timeline;
class domain;
class knows_domain;

class timeline_entry : public simEntity, public timeline_aware {
 public:
  timeline_entry(simTimeType firetime, timeline* tl, bool rpt=false) 
    : simEntity("timelineEvent"), timeline_aware(tl), 
    fires(firetime),fireorig(firetime),repeats(rpt) {};
  virtual ~timeline_entry() {};
  simTimeType firetime() { return fires; };
  virtual void fire()=0;
  virtual void set_repeating(bool r) { repeats = r; };
  virtual void repeat();
  virtual string class_name() { return "timeline_entry"; };

 protected:
  simTimeType fires,fireorig;
  bool        repeats;

};

class understands_timeline_entries {
 public:
  understands_timeline_entries() {};
  virtual ~understands_timeline_entries() {};
  virtual timeline_entry * parse_tle_spec( simTimeType firetime, 
					   timeline* tl,
					   string tles )=0;
 protected:

};

class timeline : public simEntity, public knows_domain, 
		 public understands_timeline_entries {
 public:
  timeline(domain* d);
  ~timeline();
  //! uses declarations to verify the simStep of the current simulation.
  //! could probably be done a safer way.
  void update();
  void add_entry(timeline_entry* tle);
  void delete_entry(timeline_entry* tle);
  void set_header(string hdr) { header = hdr; };
  virtual timeline_entry * parse_tle_spec( simTimeType firetime, 
					   timeline* tl,
					   string tles );
  virtual string describe();
  virtual string class_name() { return "timeline"; };

 protected:
  vector<timeline_entry*> line;
  vector<timeline_entry*>::iterator tl_index;
  string header;
  void update_tl_index();
};

// useful entry type

class unparsable_timeline_entry : public timeline_entry {
 public:
  unparsable_timeline_entry( simTimeType firetime, timeline* tl, 
			     string entry_string ) : 
    timeline_entry(firetime,tl),msg(entry_string) {};
  virtual void fire();
  virtual string describe();
  virtual string class_name() { return "unparsable_timeline_entry"; };
  
 protected:
  string msg;

};
 
 class exit_timeline_entry : public timeline_entry {
 public:
 exit_timeline_entry( simTimeType firetime, timeline* tl ) : timeline_entry(firetime,tl) {};
  virtual void fire() { exit(EXIT_SUCCESS); };
  virtual string describe();
  virtual string class_name() { return "exit_timeline_entry"; };
};
 

  class timeline_anomaly : public timeline_entry {
  public:
  timeline_anomaly(int firetime,timeline* tl) : timeline_entry(firetime,tl) {};
  };

 namespace tlFactory {
   timeline       * create_timeline(domain* d);
   timeline       * read_timeline(domain* d, string filename);
   void             read_to_timeline(timeline& tl, string filename);
   bool             read_entry_to_timeline(timeline& tl, string entry);
   timeline_entry * parse_entry(timeline& tl, string entry);
   void             do_not_exit_on_parse_failure();
 };

};

#endif
