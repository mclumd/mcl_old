#include "substrate/i_models.h"
#include <stdlib.h>

using namespace MonCon;

//////////////////////////////////////////////////////////////

adj_mx_model::~adj_mx_model() { delete[] bmx; }

string adj_mx_model::describe() {
  string r="";
  for (int s=0;s<mjl;s++) {
    for (int d=0;d<mnl;d++) {
      if (reach(s,d))
	r+="+ ";
      else r+="* ";
    }
    r+="\n";
  }
  return r;
}

int adj_mx_model::closest_minor(int major_i) {
  for (int minor_i=0;minor_i<minor_length();minor_i++) {
    if (reach(major_i,minor_i))
      return minor_i;
  }
  return -1;
}

int adj_mx_model::closest_major(int minor_i) {
  for (int major_i=0;major_i<major_length();major_i++) {
    if (reach(major_i,minor_i))
      return major_i;
  }
  return -1;
}

bool adj_mx_model::model_equality(model* m) {
  adj_mx_model* in_m = dynamic_cast<adj_mx_model*>(m);
  if (this == in_m) return true;
  else if (in_m == NULL) return false;
  else if ((major_length() != in_m->major_length()) ||
	   (minor_length() != in_m->minor_length())) return false;
  else {
    for (int j=0;j<mjl;j++) 
      for (int n=0;n<mnl;n++) 
	if (reach(j,n) != in_m->reach(j,n)) return false;
    return true;
  }
}

model* adj_mx_model::clone() {
  return new adj_mx_model(bmx,mjl,mnl);
}

//////////////////////////////////////////////////////////////

cost_mx_model::~cost_mx_model() { delete[] cmx; }

bool cost_mx_model::model_equality(model* m) {
  cost_mx_model* in_m = dynamic_cast<cost_mx_model*>(m);
  if (this == in_m) return true;
  else if (in_m == NULL) return false;
  else if ((major_length() != in_m->major_length()) ||
	   (minor_length() != in_m->minor_length())) return false;
  else {
    for (int j=0;j<mjl;j++) 
      for (int n=0;n<mnl;n++) 
	if (cost(j,n) != in_m->cost(j,n)) return false;
    return true;
  }
}

model* cost_mx_model::clone() {
  return new cost_mx_model(cmx,mjl,mnl);
}

//////////////////////////////////////////////////////////////

pc_vector_model::~pc_vector_model() { delete[] bv; }

bool pc_vector_model::model_equality(model* m) {
  pc_vector_model* in_m = dynamic_cast<pc_vector_model*>(m);
  if (this == in_m) return true;
  else if (in_m == NULL) return false;
  else if (length_of() != in_m->length_of()) return false;
  else {
    for (int i=0;i<length_of();i++) {
      if (sat(i) != in_m->sat(i)) return false;
    }
    return true;
  }
}

model* pc_vector_model::clone() {
  return new pc_vector_model(bv,len);
}

//////////////////////////////////////////////////////////////

cost_vector_model::~cost_vector_model() { delete[] cv; }

bool cost_vector_model::model_equality(model* m) {
  cost_vector_model* in_m = dynamic_cast<cost_vector_model*>(m);
  if (this == in_m) return true;
  else if (in_m == NULL) return false;
  else if (length_of() != in_m->length_of()) return false;
  else {
    for (int i=0;i<length_of();i++) {
      if (cost(i) != in_m->cost(i)) return false;
    }
    return true;
  }
}

model* cost_vector_model::clone() {
  return new cost_vector_model(cv,len);
}
