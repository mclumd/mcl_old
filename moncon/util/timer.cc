#include "mc_utils/timer.h"

using namespace umbc;

#ifdef WIN32
#include <sys/timeb.h>
#include <sys/types.h>
#include <winsock2.h>
void gettimeofday(struct timeval* t,void* timezone)
{       struct _timeb timebuffer;
        _ftime( &timebuffer );
        t->tv_sec=timebuffer.time;
        t->tv_usec=1000*timebuffer.millitm;
}
#endif

double timer::getTimeDouble() {
  timeval curr;
  gettimeofday(&curr,NULL);
  return curr.tv_sec+(double)curr.tv_usec/(double)usec_per_sec;
}

timer::timer() {
  gettimeofday(&target,NULL);
  gettimeofday(&current,NULL);
}

timer::timer(int seconds, int useconds) {
  gettimeofday(&current,NULL);
  int carry=0;
  target.tv_usec=current.tv_usec+useconds;
  if (target.tv_usec > usec_per_sec) {
    target.tv_usec = target.tv_usec % usec_per_sec;
    carry=1;
  }
  target.tv_sec=current.tv_sec+seconds+carry;
}

void timer::restart(int seconds, int useconds) {
  gettimeofday(&current,NULL);
  int carry=0;
  target.tv_usec=current.tv_usec+useconds;
  if (target.tv_usec > usec_per_sec) {
    target.tv_usec = target.tv_usec % usec_per_sec;
    carry=1;
  }
  target.tv_sec=current.tv_sec+seconds+carry;
}

bool timer::expired() {
  gettimeofday(&current,NULL);
  return ((current.tv_sec > target.tv_sec) ||
	  ((current.tv_sec == target.tv_sec) &&
	   (current.tv_usec > target.tv_usec)));
}
