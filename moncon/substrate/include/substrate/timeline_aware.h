#ifndef MCS_TIMELINE_AWARE_H
#define MCS_TIMELINE_AWARE_H

namespace MonCon {

class timeline;

class timeline_aware {
 public:
  timeline_aware(timeline* tl) : tline(tl) {};
  timeline* get_timeline() { return tline; };
  void      set_timeline(timeline* tl) { tline=tl; };
 protected:
  timeline* tline;
};

};

#endif
