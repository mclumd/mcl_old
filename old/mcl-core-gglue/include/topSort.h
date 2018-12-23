#ifndef TOPSORT_H
#define TOPSORT_H

#include<vector>
using namespace std;

class mclNode;

namespace topsort {
  bool topSort(vector<mclNode*>& invec);
};

#endif
