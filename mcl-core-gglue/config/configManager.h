#include <string>
#include "textUtils.h"

//
// DEPRECATED !! DEPRECATED !!
//
// DEPRECATED !! DEPRECATED !!
//
// DEPRECATED !! DEPRECATED !!
//

using namespace std;

#include "mclFrame.h"

namespace cfg {
  /* public */
  void setSystem(string system);
  string getSystemName();
  void loadConfig(string system);
  void saveConfig();

  void ignore_cfg_errors();

  /* protected-ish */
  string pathToConfigRead(string file);
  string pathToConfigWrite(string file);
  void   find_config_read_write();

  /* private-ish (CPT) */
  void loadCPTs();
  void saveCPTs(mclFrame *f);
  void applyCPTconfig(mclFrame *f);
  void applyCPTconfig(mclOntology *o);
  void applyCPTconfig(mclNode *n,string config);

  /* private-ish (costs) */
  void loadResponseCosts();
  void saveResponseCosts(mclFrame *f);
  void applyRCconfig(mclFrame *f);
  void applyRCconfig(mclOntology *o);
  void applyRCconfig(mclNode *n,string config);

};
