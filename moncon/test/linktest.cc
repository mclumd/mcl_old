#include "substrate/domain_configurator.h"

#include <unistd.h>
#include <iostream>

#include "umbc/settings.h"

int main(int argc, char* args[]) {
  umbc::settings::args2SysProperties(argc,args);
  // sprintf(domain_config::error_msg,"testing error_msg");
  cout << "build success..." << endl;
}
