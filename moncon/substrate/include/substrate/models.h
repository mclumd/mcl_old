#ifndef MCS_MODELS_H
#define MCS_MODELS_H

#include "simEntity.h"
#include "actions.h"
#include "moncon_logging.h"
#include "model_effects.h"

#include <map>
#include <vector>

////
///  base classes
////

namespace MonCon {

  /* base class of all model objects... really can be anything. */
  class model : public simEntity {
  public:
  model(string name) : simEntity(name) {};
    virtual ~model() {};
    virtual model* clone()=0;
    virtual simEntity* model_of()=0;
    virtual bool model_equality(model* comp)=0;
  };
  
  /* class of objects that have _a_ model.
     not sure how useful this class is, I am leaving it in case it is 
     in use somewhere. */
  class has_model {
  public:

  has_model() {};
    virtual ~has_model() {};
    virtual model* ptr_to_my_model()=0;

  };
  
  /* class of objects that can provide models when requested by another
     entity. stored/provided models are keyed by an entity pointer. */
  class provides_models {
  public:
    provides_models() {};
    virtual ~provides_models() {};
    virtual model* provide_ptr_to_model(simEntity* entity)=0;
    virtual model* provide_copy_of_model(simEntity* entity)=0;
    virtual bool   produce_model(simEntity* entity)=0;
    virtual bool   new_model_is_available(simEntity* entity,model* old)=0;
  };
  
  ////
  ///  utility classes
  ///  --> MODEL
  ////

  /* the model class is used as a base for the simple model. the simple
     model just basically attaches a name/entity to the base class. */
  class simple_model : public model {
  public:
  simple_model(string name) : 
    model("unattached-model("+name+")"),modelof(NULL) {};
  simple_model(simEntity* entity) : 
    model("model("+entity->get_name()+")"),modelof(entity) {};
    virtual ~simple_model() {};
    virtual simEntity* model_of() { return modelof; };
    virtual void       set_underlying_entity(simEntity* e)
    { modelof = e; };
    
  protected:
    simEntity* modelof;
    
  };

  class action_model : public simple_model {
  public:
  action_model(string forwhat) : simple_model(forwhat) {};
  action_model(action* a) : simple_model(a) {};
    action_model(const action_model& basis);
    virtual ~action_model();
    virtual action* models_this_action() { return (action*)model_of(); };
    virtual bool model_equality(model* comp);

    virtual void add_effect(action_effect* ae)
    { effects.push_back(ae); };
    virtual unsigned int   number_of_effects() const
    { return effects.size(); };
    // kind of cheating to say this is const...
    virtual action_effect* get_effect(unsigned int n) const
    { return effects[n]; };

    virtual model* clone();
    virtual string class_name() { return "action_model"; };
    virtual string describe() { 
      if (model_of())
	return "("+class_name()+", unattached)";
      else return "("+class_name()+" "+modelof->get_name()+")";
    };

  protected:
    vector<action_effect*> effects;
    
  };

  ////
  ///  utility classes
  ///  --> HAS_MODEL
  ////  

  class owns_its_model : public has_model {
  public:
  owns_its_model(model* m) : has_model(),my_model(m) {};
    model* ptr_to_my_model() { return my_model; };
    virtual ~owns_its_model() { if (my_model) delete my_model; };

  private:
    model* my_model;

  };

  class model_is_provided : public has_model {
  public:
  model_is_provided(provides_models* pm) 
    : has_model(),my_provider(pm) {};

    void   identify_self_to_provider(simEntity* self) { 
      
      myself = self; 
    };
    model* ptr_to_my_model() { return my_provider->provide_ptr_to_model(myself); };

  protected:
    simEntity* myself;
    provides_models* my_provider;

  };

  class caches_its_model : public model_is_provided {
  public:
  caches_its_model(provides_models* pm) : 
    model_is_provided(pm),my_model(NULL) { };
    virtual ~caches_its_model() { if (my_model) delete my_model; };
    
    model* ptr_to_my_model() { 
      if (!my_model && my_provider && myself) {
	my_provider->produce_model(myself);
	my_model=my_provider->provide_copy_of_model(myself);
      }
      return my_model; 
    };
    model* get_updated_model() {
      if (my_provider && myself && my_provider->new_model_is_available(myself,my_model)) {
	if (my_model) delete my_model;
	my_provider->produce_model(myself);
	my_model = my_provider->provide_copy_of_model(myself);
      }
      return my_model;
    };

  private:
    model* my_model;

  };

  ////
  ///  utility classes
  ///  --> PROVIDES_MODELS
  ////  

  class model_conduit : public provides_models {
  public:
  model_conduit(provides_models* source) : provides_models(),src(source) {};
    virtual model* provide_ptr_to_model(simEntity* entity) 
    { return src->provide_ptr_to_model(entity); };
    virtual model* provide_copy_of_model(simEntity* entity) 
    { return src->provide_copy_of_model(entity); };
    virtual bool   produce_model(simEntity* entity)
    { return src->produce_model(entity); };
    virtual bool   new_model_is_available(simEntity* entity,model* m) 
    { return src->new_model_is_available(entity,m); };

  protected:
    provides_models* get_source() { return src; };

  private:
    provides_models* src;
    
  };

  template <class K> class model_warehouse {
  public:
    model_warehouse() {};
    virtual ~model_warehouse() {  
      umbc::uLog::annotate(MONL_HOSTERR,
			   "destructor for MODEL_WAREHOUSE is unimplemented!");
    };
    virtual bool   has_model(K it) {
      return (model_lookup.find(it) != model_lookup.end());
    }
    virtual model* lookup_model(K it) {
      if (model_lookup.find(it) != model_lookup.end())
	return model_lookup[it];
      else return NULL;
    }
    virtual void   change_model(K it, model* m)
    { if (has_model(it)) delete lookup_model(it); 
      model_lookup[it]=m; model_is_fresh(it); }
    virtual bool   is_model_stale(K it)
    { return (!has_model(it) || model_state[it]==STATE_DIRTY); }
    virtual bool   model_is_stale(K it)
    { return model_state[it]==STATE_DIRTY; }
    virtual bool   model_is_fresh(K it)
    { return model_state[it]==STATE_CLEAN; }
    
  private:
    enum mstate { STATE_DIRTY, STATE_CLEAN };
    map<K,model*> model_lookup;
    map<K,mstate> model_state;
    
  };

  class caches_models : public model_conduit {
  public:
  caches_models(provides_models* source) : model_conduit(source) { };
    virtual ~caches_models() {};
    virtual model* provide_copy_of_model(simEntity* entity)
    { if ( provide_ptr_to_model(entity) )
	return provide_ptr_to_model(entity)->clone();
      else return NULL; }
    virtual bool refresh_all_models()=0;
  };

  class caches_models_by_name : public caches_models {
  public:
    caches_models_by_name(provides_models* source) : caches_models(source) {};
    virtual ~caches_models_by_name() {};
    virtual model* provide_ptr_to_model(simEntity* entity);
    virtual bool   produce_model(simEntity* entity) 
    { modelhouse.model_is_stale(entity->get_name()); 
      return model_conduit::produce_model(entity); }

  private:
    model_warehouse<string> modelhouse;

  };    

  class caches_models_by_ptr : public caches_models {
  public:
    caches_models_by_ptr(provides_models* source) : caches_models(source) {};
    virtual ~caches_models_by_ptr() {};
    virtual model* provide_ptr_to_model(simEntity* entity);
    virtual bool   produce_model(simEntity* entity) 
    { modelhouse.model_is_stale(entity); 
      return model_conduit::produce_model(entity); }

  private:
    model_warehouse<simEntity*> modelhouse;

  };    

  class model_oracle : public provides_models {
  public:
  model_oracle() : provides_models() {};

    // this is the trick to the oracle -- it will only agree to "produce"
    // a model if it has one in already, and it is does not change according
    // to what consumers of the model want...
    virtual bool   produce_model(simEntity* entity) 
    { if (provide_ptr_to_model(entity)) return true; else return false; };
    virtual bool   new_model_is_available(simEntity* entity,model* old)
    { if (!provide_ptr_to_model(entity)) return (old != NULL);
      else return (!provide_ptr_to_model(entity)->model_equality(old)); };

  protected:

  };

};

#endif
