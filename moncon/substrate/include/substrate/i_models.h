#ifndef I_MODELS_H
#define I_MODELS_H

#include "models.h"
#include <limits>
#include <string.h>

namespace MonCon {

class adj_mx_model : public model {
 public:
  adj_mx_model(bool* ibmx,int mjr_len,int mnr_len) :  
    model("adj_mx"),mjl(mjr_len),mnl(mnr_len) { 
    bmx = new bool[mjl*mnl]; 
    memcpy(bmx,ibmx,sizeof(bool)*mjr_len*mnr_len);  
    cout << describe() << endl;
  }
  virtual ~adj_mx_model();
  void set_reach(int mji,int mni,bool rch)
  { bmx[mji*mnl+mni]=rch; };
  bool reach(int mjr_ind,int mnr_ind) 
    { return bmx[mjr_ind*mnl+mnr_ind]; };
  int major_length() { return mjl; };
  int minor_length() { return mnl; };
  virtual string class_name() { return "adj_mx_model"; };
  virtual string describe();
  int closest_minor(int major_i);
  int closest_major(int minor_i);

  virtual model* clone();
  virtual simEntity* model_of() { return NULL; };
  virtual bool model_equality(model* m);

 protected:
  bool* bmx;
  int mjl,mnl;
};

class cost_mx_model : public model {
 public:
  cost_mx_model(int* icmx,int mjr_len,int mnr_len) : 
    model("cost_mx"),mjl(mjr_len),mnl(mnr_len)
    { cmx = new int[mjl*mnl]; memcpy(cmx,icmx,sizeof(int)*mjr_len*mnr_len);  }
  virtual ~cost_mx_model();
  void set_all_costs(int s) 
  { for (int j=0;j<mjl;j++) for (int n=0;n<mnl;n++) cmx[j*mnl+n]=s; };
  void set_cost(int mji,int mni,int cost)
  { cmx[mji*mnl+mni]=cost; };
  int cost(int mjr_ind,int mnr_ind) 
    { return cmx[mjr_ind*mnl+mnr_ind]; };
  int  major_length() { return mjl; };
  int  minor_length() { return mnl; };
  int* matrix_ptr() { return cmx; };

  virtual model* clone();
  virtual simEntity* model_of() { return NULL; };
  virtual bool model_equality(model* m);

  virtual string describe() { return "("+class_name()+" "+get_name()+")"; };
  virtual string class_name() { return "cost_mx_model"; };

 protected:
  int* cmx;
  int mjl,mnl;

};

class pc_vector_model : public model {
 public:
  pc_vector_model(int length) : model("pcv"),len(length)
    { bv = new bool[len]; bzero(bv,sizeof(bool)*length);  }
  pc_vector_model(bool* ibv,int length) : model("pcv"),len(length)
    { bv = new bool[len]; memcpy(bv,ibv,sizeof(bool)*length);  }
  virtual ~pc_vector_model();
  void set(int ind,bool v=true)  { bv[ind]=v; };
  bool sat(int ind)  { return bv[ind]; };
  int length_of()    { return len; };

  virtual model* clone();
  virtual simEntity* model_of() { return NULL; };
  virtual bool model_equality(model* m);

  virtual string describe() { return "("+class_name()+" "+get_name()+")"; };
  virtual string class_name() { return "pc_vector_model"; };

 protected:
  bool* bv;
  int len;

};

class cost_vector_model : public model {
 public:
  cost_vector_model(int length) : model("cvm"),len(length)
    { cv = new int[len]; 
      for (int x=0;x<length;x++) cv[x]=std::numeric_limits< int >::max(); };
  cost_vector_model(int* icv,int length) : model("cvm"),len(length)
    { cv = new int[len]; memcpy(cv,icv,sizeof(bool)*len);  }
  virtual ~cost_vector_model();
  int cost(int ind) 
    { return cv[ind]; };
  int length_of() 
    { return len; };

  virtual model* clone();
  virtual simEntity* model_of() { return NULL; };
  virtual bool model_equality(model* m);

  virtual string describe() { return "("+class_name()+" "+get_name()+")"; };
  virtual string class_name() { return "cost_vector_model"; };

 protected:
  int* cv;
  int len;

};

};
#endif
