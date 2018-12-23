#ifndef MC_SUBSTRATE_SIMENTITY_H
#define MC_SUBSTRATE_SIMENTITY_H

#include <string>
#include <iostream>

using namespace std;

class simEntity {
 public:
  simEntity();
  simEntity(string n) : name(n),status("ok") { };
  virtual ~simEntity() {};
  virtual string get_name() { return name; };
  virtual string class_name()=0;
  virtual string get_status() { return status; };
  virtual bool   set_status(string ns) { status=ns; return true; };
  virtual string describe()=0;
  virtual void dump(ostream& stream) { stream << describe() << endl; };
  virtual void dump() { dump(cout); };
  virtual string personalize(string input) { return get_name()+":"+input; };
  static  string personalize_static(string aname,string input)
  { return aname+":"+input; };
  static  string personalize_static(string aname,string cname,string input)
  { return aname+":"+cname+":"+input; };

 protected:
  string name;
  string status;

};

#endif
