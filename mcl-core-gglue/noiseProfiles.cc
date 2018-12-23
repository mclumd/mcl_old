#include "noiseProfiles.h"
#include "APICodes.h"
#include "mclLogger.h"

map<string,noiseProfile*> noiseProfile::mcl_np_map;

/////////////// -- UTILS

void np_warn(string msg) {
  mclLogger::annotate(MCLA_WARNING,"[mcl/np]:: "+msg);
}

/////////////// -- STATIC CREATE METHODS

noiseProfile* noiseProfile::createNoiseProfile(int profile_id) {
  switch(profile_id) {
  case MCL_NP_NO_PROFILE:
  case MCL_NP_PERFECT:
    return new perfectProfile();
    break;
  case MCL_NP_UNIFORM:
    np_warn("[mcl/np]:: Uniform noise profile invoked without args.");
    return createNoiseProfile(MCL_NP_UNIFORM,0.10);
    break;
  otherwise:
    if (profile_id == MCL_NP_DEFAULT) {
      np_warn("[mcl/np]:: MCL_NP_DEFAULT can't be handled by zero-arg np create method (using perfect profile).");
      return new perfectProfile();
    }
    else {
      np_warn("[mcl/np]:: (0A) failed to comprehend noise profile tag (using default)");
      return createNoiseProfile(MCL_NP_DEFAULT);
    }
    break;
  }
}

noiseProfile* noiseProfile::createNoiseProfile(int profile_id,double param) {
  switch(profile_id) {
  case MCL_NP_NO_PROFILE:
  case MCL_NP_PERFECT:
    return createNoiseProfile(MCL_NP_PERFECT);
    break;
  case MCL_NP_UNIFORM:
    return new uniformNoiseProfile(param);
    break;
  otherwise:
    if (profile_id == MCL_NP_DEFAULT) {
      np_warn("[mcl/np]:: MCL_NP_DEFAULT can't be handled by 1-arg np create method (using perfect profile).");
      return new perfectProfile();
    }
    else {
      np_warn("[mcl/np]:: (1A) failed to comprehend noise profile tag (using default)");
      return createNoiseProfile(MCL_NP_DEFAULT);
    }
    break;
  }
}

/////////////// -- STATIC NP DATABASE

void noiseProfile::establishNoiseProfileFor(string mclkey, string observable, 
					    noiseProfile* np) {
  string dbk = mclkey+":"+observable;
  if (mcl_np_map.find(dbk) != mcl_np_map.end()) {
    noiseProfile* q=mcl_np_map[dbk];
    if (q && (np!=q)) {
      delete q;
      mcl_np_map[dbk]=NULL;
    }
    else return;
  }
  mcl_np_map[dbk]=np;
}

noiseProfile* noiseProfile::getNoiseProfileFor(string mcl_key,
					       string observable) {
  string dbk = mcl_key+":"+observable;
  return mcl_np_map[dbk];
}

/////////////// -- NOISE PROFILE CLASS DEFS

double perfectProfile::p_that_EQ(double observed, double target) {
  return (target == observed) ? 1.0 : 0.0;
}
double perfectProfile::p_that_LT(double observed, double target) {
  return (target > observed) ? 1.0 : 0.0;
}
double perfectProfile::p_that_GT(double observed, double target) {
  return (target < observed) ? 1.0 : 0.0;
}
double perfectProfile::p_that_WI(double observed, double low, double high) {
  return ((observed >= low) && (observed <= high)) ? 1.0 : 0.0;
}
double perfectProfile::p_that_WO(double observed, double low, double high) {
  return ((observed <= low) || (observed >= high)) ? 1.0 : 0.0;
}

double uniformNoiseProfile::p_that_EQ(double observed, double target) { 
  return ((target - (target*tol)) <= observed <= (target + (target*tol))) ?
    1.0 : 0.0;
}
double uniformNoiseProfile::p_that_LT(double observed, double target) { 
  return ((target + (target*tol)) >= observed);
}
double uniformNoiseProfile::p_that_GT(double observed, double target) { 
  return ((target - (target*tol)) <= observed);
}
double uniformNoiseProfile::p_that_WI(double observed, double low, 
				      double high) {   
  return ((low - (low*tol)) <= observed <= (high + (high*tol))) ?
    1.0 : 0.0;
}
double uniformNoiseProfile::p_that_WO(double observed, double low, double high) { 
  return (((low - (low*tol)) <= observed) || 
	  (observed >= (high + (high*tol)))) ?
    1.0 : 0.0;
}

/*
double p_that_EQ(double observed, double target) { }
double p_that_LT(double observed, double target) { }
double p_that_GT(double observed, double target) { }
double p_that_WI(double observed, double low, double high) { }
double p_that_WO(double observed, double low, double high) { }
*/
