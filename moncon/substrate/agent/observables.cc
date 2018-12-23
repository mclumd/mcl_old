#include "substrate/observables.h"

#include <stdio.h>

using namespace MonCon;

double breakable::base_break_rate = 0.01;

string observable::describe() {
  char vv[128];
  sprintf(vv,"<observable %s %lf/%d>\n",
	  get_name().c_str(),double_value(),int_value());
  return vv;
}

double breakable::double_value() {
  if (is_broken())
    return broken_value();
  else
    return peel()->double_value();
}

int breakable::int_value() {
  if (is_broken())
    return (int)broken_value();
  else
    return peel()->int_value();
}

string breakable::describe() {
  char vv[1028];
  sprintf(vv,"<breakable %lf/%d %s>\n",
	  double_value(),int_value(),the_observable.describe().c_str());
  return vv;
}

string noisy::describe() {
  char vv[1028];
  sprintf(vv,"<noisy %s>\n",
	  the_observable.describe().c_str());
  return vv;
}

noisy* observable_utils::get_noisy(observable* o) {
  observable* this_o=NULL;
  observable* next_o=o;
  do {
    this_o=next_o;
    next_o=next_o->peel();
    noisy* tbo = dynamic_cast<noisy*>(this_o);
    if (tbo != NULL)
      return tbo;
  } while (this_o != next_o);
  return NULL;
}

breakable* observable_utils::get_breakable(observable* o) {
  observable* this_o=NULL;
  observable* next_o=o;
  do {
    this_o=next_o;
    next_o=next_o->peel();
    breakable* tbo = dynamic_cast<breakable*>(this_o);
    /*
    printf("searching for breakable: '%s' this(%08lx) next(%08lx) cast(%08lx)\n",
	   o->get_name().c_str(),(unsigned long)this_o,(unsigned long)next_o,
	   (unsigned long)tbo);
    */
    if (tbo != NULL)
      return tbo;
  } while (this_o != next_o);
  return NULL;
}

/*
mcl_interoperable_sensor* observable_utils::get_mcl_sensor(observable* o) {
  observable* this_o=NULL;
  observable* next_o=o;
  do {
    this_o=next_o;
    next_o=next_o->peel();
    mcl_interoperable_sensor* tbo = dynamic_cast<mcl_interoperable_sensor*>(this_o);
    if (tbo != NULL) {
      return tbo;
    }
    else { }
  } while (this_o != next_o);
  return NULL;  
}
*/

bool observable_utils::truly_eq (observable* o, double value)  
{ return (o->core_observable()->double_value() == value); };
bool observable_utils::truly_lt (observable* o, double value)
{ return (o->core_observable()->double_value() < value); };
bool observable_utils::truly_lte(observable* o, double value)
{ return (o->core_observable()->double_value() <= value); };
bool observable_utils::truly_gt (observable* o, double value)
{ return (o->core_observable()->double_value() > value); };
bool observable_utils::truly_gte(observable* o, double value)
{ return (o->core_observable()->double_value() >= value); };


/* THIS STUFF SHOULD MAKE ITS WAY INTO THE BREAKABLE CODE AT SOME POINT

double basic_breakable_sensor::doubleValue(bool raw) {
  if (raw) return basic_rv_sensor::doubleValue();
  else {
    if (is_broken())
      return broken_value();
    else {
      double ran = (double)rand() / (double)RAND_MAX;
      // cout << "break test for " << get_name() << ": "
      // << ran << " < " << breakage_rate() << endl;
      if (ran < breakage_rate()) {
	break_it();
	return broken_value();
      }
      else
	// holy shit you better supply a value for the raw parameter
	return basic_rv_sensor::doubleValue(false);
    }
  }
}

int basic_breakable_sensor::intValue(bool raw) {
  return (int)doubleValue(raw);
}

void basic_sticky_sensor::break_it() {
  breakable::break_it();
  set_break_value(basic_rv_sensor::doubleValue(true));
}

*/
