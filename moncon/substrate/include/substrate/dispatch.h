#ifndef MC_SUBSTRATE_DISPATCH_H
#define MC_SUBSTRATE_DISPATCH_H

#include <string>
using namespace std;

namespace MonCon {

  namespace dispatch {

    extern string success_prefix;
    extern string ignored_prefix;
    extern string fail_prefix;
    
    string generate_success_msg();
    string generate_success_msg(string annotation);
    string generate_ignored_msg();
    string generate_ignored_msg(string annotation);
    string generate_fail_msg();
    string generate_fail_msg(string annotation);
    
    bool is_success_msg(string msg);
    bool is_ignored_msg(string msg);
    bool is_fail_msg(string msg);

    string prologify(string message);
    
  };
};

#endif
