#include "mclNode.h"
#include "abstractGlue.h"

nodeGlue* nodeGlue::getSiblingNodeGlue(string key) {
  return myNode()->getGlue(key);
}
