#ifndef MC_SUBSTRATE_CWRAPPER_H
#define MC_SUBSTRATE_CWRAPPER_H

#include "substrate/controller.h"

namespace MonCon {

  class controller_wrapper : public controller,
    public explicit_local_control {
  public:
  controller_wrapper(controller* inner_controller) 
    : controller(inner_controller->agent_of(),inner_controller->get_name()),
      explicit_local_control(),inner(inner_controller) {};
    virtual ~controller_wrapper() { delete inner; };
    virtual controller* peel() { return inner; };
    virtual bool is_locally_controlling() { return is_controlling; };

    static controller* get_core_controller(controller* a) {
      controller* ccp = a; // current controller pointer
      while (ccp) {
	cout << "searching for ccore [" << ccp->class_name() << "]" << endl;
	controller_wrapper* ct = dynamic_cast<controller_wrapper*>(ccp);
	if (!ct) return ccp; // if it's the type we are looking for, then c ya
	else {
	  if (ct->peel() == ccp) return NULL;
	  else ccp = ct->peel();
	}
      }
      return NULL;
    };

    template <class AW> static
      AW get_typed_wrapper(controller* a) {
      controller* cap = a; // current action pointer
      while (cap) {
	cout << "searching for crapper [" << cap->class_name() << "]" << endl;
	AW ct = dynamic_cast<AW>(cap);
	if (ct) return ct; // if it's the type we are looking for, then c ya
	else {
	  // ow make sure it's a wrapper then peel
	  controller_wrapper* aaaw = dynamic_cast<controller_wrapper*>(cap);
	  if (aaaw) {
	    if (aaaw->peel() == cap) return NULL;
	    else cap = aaaw->peel();
	  }
	  else return NULL;
	}
      }
      return NULL;
    };

    static int control_code(controller* c,int layernum = 1) {
      if (c->is_locally_controlling())
	return layernum;
      else {
	controller_wrapper* aaaw = dynamic_cast<controller_wrapper*>(c);
	if (aaaw) 
	  return control_code(aaaw->peel(),layernum+1);
	else
	  return 0;
      }
    };

  private:
    controller* inner;

  };

};

#endif
