#include <iostream>
#include "text_utils.h"

using namespace std;
using namespace umbc;

bool testPLE(char inp,string protect,string o,string c,bool* e,string targ) {
  string r = textFunctions::adjustProtectLevelEnhanced(inp,protect,o,c,e);
  if (r != targ) {
    cerr << "Fail: protect=" << protect << " input=" << inp << " target="
    	 << targ << " value=" << r << endl;
    return true;
  }
  else 
    return false;
}

int main(int argc, char** argv) {
  string os = "\"([{";
  string cs = "\")]}";
  bool e=true;
  if (testPLE('\"',"[\"",os,cs,&e,"[")) {};
  if (testPLE('(',"(",os,cs,&e,"(")) {};
  string ts = "f(token=\"number one\") c[token 2]";
  int p = 0;
  cout << "t1= '" << textFunctions::readUntilWSEnhanced(ts,p,ts.size(),&p)
       << "'" << endl;
  cout << "t1= '" << textFunctions::readUntilWSEnhanced(ts,p,ts.size(),&p)
       << "'" << endl;
  string dfn = "/bootang/wutang/foo.dimple/crack.head";
  cout << "full string = '" << dfn << "'" << endl;
  cout << "path = '" << textFunctions::fileNamePath(dfn) << "'" << endl;
  cout << "file = '" << textFunctions::fileNameRoot(dfn) << "'" << endl;
  cout << "ext  = '" << textFunctions::fileNameExt(dfn) << "'" << endl;
  dfn = "cracker.jack";
  cout << "full string = '" << dfn << "'" << endl;
  cout << "path = '" << textFunctions::fileNamePath(dfn) << "'" << endl;
  cout << "file = '" << textFunctions::fileNameRoot(dfn) << "'" << endl;
  cout << "ext  = '" << textFunctions::fileNameExt(dfn) << "'" << endl;
  dfn = "/poop/poopie/doodoo/";
  cout << "full string = '" << dfn << "'" << endl;
  cout << "path = '" << textFunctions::fileNamePath(dfn) << "'" << endl;
  cout << "file = '" << textFunctions::fileNameRoot(dfn) << "'" << endl;
  cout << "ext  = '" << textFunctions::fileNameExt(dfn) << "'" << endl;
  return 0;
}
