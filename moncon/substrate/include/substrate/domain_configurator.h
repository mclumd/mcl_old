#ifndef MC_SUBSTRATE_DOMAIN_CONFIG
#define MC_SUBSTRATE_DOMAIN_CONFIG

#include "substrate/domain.h"

#include <string>
using namespace std;

namespace MonCon {

class domain_config {
 public:
  static bool configure_domain(domain *d, string config_file);
  static char* error_msg;
 private:
  domain_config() {};
};

};

#endif
