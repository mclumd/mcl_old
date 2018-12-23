#include <stdlib.h>
#include <math.h>
#include "simHost.h"

#define BASE_EXP_GRP 0x11110000

float sv[3];
mclMA::observables::update mcl_update_obj;

void fakeHost::initMCL() {
  mclMA::initializeMCL(FH_MCL_KEY,2);
  mclMA::configureMCL(FH_MCL_KEY,"fake_host");
}

void fakeHost::updateSensors() {
  static int c=0;
  // noisy cos
  sv[0]=cosf((float)c*3.14/(float)100) + (0.1 * (((float)rand()/(float)RAND_MAX)-0.5));
  // noisy sin
  sv[1]=sinf((float)c*3.14/(float)100) + (0.1 * (((float)rand()/(float)RAND_MAX)-0.5));
  // random walk
  sv[2]+=((float)rand()/(float)RAND_MAX)-0.5;
  c++;
  mcl_update_obj.set_update("fakeS1",sv[0]);
  mcl_update_obj.set_update("fakeS2",sv[1]);
  mcl_update_obj.set_update("fakeS3",sv[2]);
}

void fakeHost::registerSensors() {
  mclMA::observables::declare_observable_self(FH_MCL_KEY,"fakeS1");
  mclMA::observables::set_obs_prop_self(FH_MCL_KEY,"fakeS1",
					PROP_SCLASS,SC_RESOURCE);
  mclMA::observables::set_obs_prop_self(FH_MCL_KEY,"fakeS1",
					PROP_DT,DT_RATIONAL);

  mclMA::observables::declare_observable_self(FH_MCL_KEY,"fakeS2");
  mclMA::observables::set_obs_prop_self(FH_MCL_KEY,"fakeS2",
					PROP_SCLASS,SC_RESOURCE);
  mclMA::observables::set_obs_prop_self(FH_MCL_KEY,"fakeS2",
					PROP_DT,DT_RATIONAL);

  mclMA::observables::declare_observable_self(FH_MCL_KEY,"fakeS3");
  mclMA::observables::set_obs_prop_self(FH_MCL_KEY,"fakeS3",
					PROP_SCLASS,SC_SPATIAL);
  mclMA::observables::set_obs_prop_self(FH_MCL_KEY,"fakeS3",
					PROP_DT,DT_RATIONAL);

  mclMA::observables::dump_obs_self(FH_MCL_KEY,"fakeS1");
  mclMA::observables::dump_obs_self(FH_MCL_KEY,"fakeS2");
  mclMA::observables::dump_obs_self(FH_MCL_KEY,"fakeS3");

}

void fakeHost::declareExps() {
  mclMA::declareExpectationGroup(FH_MCL_KEY,(egkType)BASE_EXP_GRP);
  mclMA::declareExpectation(FH_MCL_KEY,(egkType)BASE_EXP_GRP,
			    "fakeS1",EC_STAYOVER,-1.0);
  mclMA::declareExpectation(FH_MCL_KEY,(egkType)BASE_EXP_GRP,
			    "fakeS1",EC_STAYUNDER,1.0);
}

void fakeHost::registerHIAs() {
  mclMA::HIA::registerHIA(FH_MCL_KEY,"inconvenientTruthException",
			  "effectorError");
}

void fakeHost::setPV() {
  
  mclMA::setPropertyDefault(FH_MCL_KEY,PCI_INTENTIONAL,PC_YES);
  mclMA::setPropertyDefault(FH_MCL_KEY,PCI_PARAMETERIZED,PC_YES);
  mclMA::setPropertyDefault(FH_MCL_KEY,PCI_SENSORS_CAN_FAIL,PC_YES);
  mclMA::setPropertyDefault(FH_MCL_KEY,PCI_EFFECTORS_CAN_FAIL,PC_YES);

}
