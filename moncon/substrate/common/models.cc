#include "substrate/models.h"
#include "substrate/moncon_logging.h"

using namespace MonCon;
using namespace umbc;

action_model::action_model(const action_model& basis) : simple_model(models_this_action()) {
  for (unsigned int q=0;q<basis.number_of_effects();q++) {
    effects.insert(effects.begin(),basis.get_effect(q)->clone());
  }
}

model* action_model::clone() {
  return new action_model(*this);
}

action_model::~action_model() {
  while (!effects.empty()) {
    delete effects.back();
    effects.pop_back();
  }
}

bool action_model::model_equality(model* cmp) {
  uLog::annotate(MONL_WARNING,personalize("model_equality() probably does not work for action_models..."));  
  return ((cmp->class_name() == class_name()) &&
	  (cmp->model_of() == model_of()));
}


//
//
//


model* caches_models_by_name::provide_ptr_to_model(simEntity* entity) {
  // if we don't have a model, ask for one from the source
  if (!modelhouse.has_model(entity->get_name())) {
    get_source()->produce_model(entity);
  }
  // now, if we don't have a model, or if we have a stale one,
  // check for a new model from the source, then install it if one exists
  if ((!modelhouse.has_model(entity->get_name()) ||
       modelhouse.is_model_stale(entity->get_name())) &&
      get_source()->new_model_is_available(entity,modelhouse.lookup_model(entity->get_name()))) {
    if (modelhouse.has_model(entity->get_name()))
      delete modelhouse.lookup_model(entity->get_name());
    modelhouse.change_model(entity->get_name(),get_source()->provide_copy_of_model(entity));
  }
  // return whatever we've got
  return modelhouse.lookup_model(entity->get_name());
}

model* caches_models_by_ptr::provide_ptr_to_model(simEntity* entity) {
  // if we don't have a model, ask for one from the source
  if (!modelhouse.has_model(entity)) {
    get_source()->produce_model(entity);
  }
  // now, if we don't have a model, or if we have a stale one,
  // check for a new model from the source, then install it if one exists
  if ((!modelhouse.has_model(entity) || modelhouse.is_model_stale(entity)) &&
      get_source()->new_model_is_available(entity,modelhouse.lookup_model(entity))) {
    if (modelhouse.has_model(entity))
      delete modelhouse.lookup_model(entity);
    modelhouse.change_model(entity,get_source()->provide_copy_of_model(entity));
  }
  // return whatever we've got
  return modelhouse.lookup_model(entity);
}

