#include "substrate/domain.h"
#include "substrate/timeline.h"
#include "umbc/settings.h"

using namespace umbc;
using namespace MonCon;

domain::domain(string name) : simEntity(name),timeline_aware(NULL) {
  set_timeline(new timeline(this));
  unsigned int iseed = (unsigned int)settings::getSysPropertyInt("mars.domain_seed",0);
  if (iseed == 0) iseed = ((unsigned int)time(NULL));
  srand48_r((long)iseed,&domain_seed);
};

domain::domain(string name, timeline* tl) : 
  simEntity(name),timeline_aware(tl) { 
  unsigned int iseed = (unsigned int)settings::getSysPropertyInt("mars.domain_seed",0);
  if (iseed == 0) iseed = ((unsigned int)time(NULL));
  srand48_r((long)iseed,&domain_seed);
};

double domain::dbl_rand() {
  double rv;
  drand48_r(&domain_seed,&rv);
  return rv;
}
