#include "keyGen.h"

#include <iostream>
#include <map>

int main(int argc,char *args[]) {
  bool done=false;
  map<mclKey,string> iss;
  while (!done) {
    string s;
    cout << "symbol> ";
    getline(cin,s);
    if (s == "q") done=true;
    cout << "key=" << hex << computeKey(s) << endl;
    if ((iss[computeKey(s)] != "") &&
	(iss[computeKey(s)] != s)) {
      cout << "collision with " << iss[computeKey(s)] << endl;
    }
    iss[computeKey(s)] = s;
  }
  return 0;
}
