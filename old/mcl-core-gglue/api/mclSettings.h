#ifndef MCL_SETTINGS
#define MCL_SETTINGS

#include <map>
#include <string>

using namespace std;

/** \file
 *  \brief general settings to be accessible from all of MCL-Core
 * currently contains flags for output
 */

namespace mclSettings {
  //  hard coded settings...

  //! if true then minimal (no) output
  extern bool quiet;

  //! if true then output formatted/HTML output
  extern bool annotate;

  //! if true then debugging output
  extern bool debug;

  //! if true then write dot files frames on response-recommendation
  extern bool logFrames;

  //! if true then autoconfigure bayes CPTs
  extern bool autoConfigCPTs;

  //! this is an API for general-purpose global properties...
  //! will be made available through /api
  void setSysProperty(string name,int val);
  void setSysProperty(string name,string val);
  void setSysProperty(string name,bool val);
  int  getSysPropertyInt(string name,int def=0);
  bool getSysPropertyBool(string name,bool def=false);
  string getSysPropertyString(string name,string def="none");
  void args2SysProperties(int argc, char** args);
  
};

#endif
