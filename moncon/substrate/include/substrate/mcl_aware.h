#ifndef MC_SUBSTRATE_MCL_AWARE_H
#define MC_SUBSTRATE_MCL_AWARE_H

#include "mcl/mcl_api.h"
#include "substrate/actions.h"

namespace MonCon {

  class action_effect;
  class sensing;

  namespace mclAware {

    string select_ontology_for_agent(const string& agent_name,
				     const string& domain_class_name,
				     const string& agent_class_name,
				     const string& cont_class_name);

    class has_res_ref {
    public:
      has_res_ref() {};
      virtual ~has_res_ref() {};
      
      virtual void       set_mclref(resRefType mrt)=0;
      virtual resRefType get_mclref()=0;
    };

    class res_ref_is_supplied {
    public:
    res_ref_is_supplied() : mcl_ref(NULL) {};
    res_ref_is_supplied(resRefType referent) : mcl_ref(referent)
      {};
      
      virtual ~res_ref_is_supplied() {};
      virtual void       set_mclref(resRefType mrt) { mcl_ref=mrt; };
      virtual resRefType get_mclref() { return mcl_ref; };
      
    protected:
      resRefType mcl_ref;
      
    };
        
    class has_eg_key {
    public:
      has_eg_key() {};
      virtual ~has_eg_key() {};
      virtual egkType mcl_eg_key_for()=0;
    };
    
    class eg_key_is_this : public has_eg_key {
    public:
    eg_key_is_this() : has_eg_key() {};
      virtual ~eg_key_is_this() {};
      virtual egkType mcl_eg_key_for() { return (egkType)this; };
      
    };

    namespace expgen {
      bool ae2expectations(string mcl_k,egkType eg_k,
			   sensing* a,action_effect* ae);
      bool ae2expectations(string mcl_k,egkType eg_k,
			   sensing* a,has_args* ha,action_effect* ae);
    };
    
  };
  
};

#endif
