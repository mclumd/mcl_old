#ifndef MC_SUBSTRATE_ACTION_H
#define MC_SUBSTRATE_ACTION_H

#include "substrate/agent.h"
#include "substrate/simEntity.h"
#include "umbc/command_grammar.h"
#include <exception>

namespace MonCon {

  //// utility class
  
  class has_args {
  public: 
  has_args(int numberof) : numargs(numberof) {};
    virtual ~has_args() {};
    
    virtual int  get_argcount() { return numargs; };
    virtual void set_arg(int d)=0;
    virtual int  get_arg_int()=0;
    virtual void set_arg(int n,int v)=0;
    virtual int  get_arg_int(int n)=0;
    virtual void clear_args()=0;
    
    virtual string grammar_argspec()=0;
    
  protected:
    int numargs;
    
  };
  
  class simple_has_args : public has_args {
  public:
  simple_has_args(int numberof) : 
    has_args(numberof),args(new double[numberof]) {};
    virtual ~simple_has_args() { delete[] args; }
    
    virtual void set_arg(int d) { args[0]=d; };
    virtual int  get_arg_int()  { return (int)args[0]; };
    virtual void set_arg(int n,int v) { args[n]=v; };
    virtual int  get_arg_int(int n)  { return (int)args[n]; };  
    virtual void clear_args() { for (int i=0;i<numargs;i++) args[i]=0.0; };
    
  protected:
    double* args;
    
  };

  class action : public simEntity, public knows_agent, public simple_has_args,
    public umbc::publishes_grammar {
  public:
    action(string name,sensorimotor_agent* a,int argcount);
    virtual ~action() {};
    
    // abstract, to be completed in childrens
    virtual bool   active()=0;
    virtual bool   activate()=0;
    virtual bool   deactivate()=0;
    virtual bool   simulate()=0;
    virtual string grammar_argspec()=0;
    virtual string grammar_commname()=0;
    virtual bool   publish_grammar(umbc::command_grammar& cg)=0;

    virtual string personalize(string input)=0;

    virtual sensorimotor_agent* sensorimotor_agent_of()=0;

    virtual string describe() { return "(action "+get_name()+")"; };
    
  };
  
  class simple_action : public action {
  public:
    simple_action(string name,sensorimotor_agent* a);
    simple_action(string name,sensorimotor_agent* a,int numargs);

    virtual bool simulate() { return true; };
    
    // command grammar stuff
    virtual string grammar_argspec();
    virtual string grammar_commname() { return get_name()+"_command"; };
    virtual bool   publish_grammar(umbc::command_grammar& cg);

    virtual string personalize(string input)
    { return smagent->personalize(get_name()+"."+input); };

    virtual sensorimotor_agent* sensorimotor_agent_of()
    { return smagent; };    

  protected:
    sensorimotor_agent* smagent;
    
  };
  
  class simple_repair_action : public simple_action {
  public:
  simple_repair_action(string name, sensorimotor_agent* a, int numargs) :
    simple_action(name,a,numargs) {};
    virtual ~simple_repair_action() {};
    virtual bool simulate();
  };


  ///
  // ACTION WRAPPER STUFF !!
  ///
  
  namespace action_wrappers {

    class action_wrapper;
    class breakable_layer;
    class intermittent_layer;

    action* action_core(action* a);

    class illegal_core : public exception {
    public:

    illegal_core(string wrap,string core) : 
      exception(),msg("Illegal action core: "+core+" is not allowed in "+wrap) 
	{};

      virtual ~illegal_core() throw() {};

      virtual const char* what() const throw() { 
	return msg.c_str();
      }

    private:
      string msg;

    };

    // breakable_layer*    action_breakable_layer();
    // intermittent_layer* action_intermittent_layer();

    class action_wrapper : public action {
    public:
    action_wrapper(action* inna) : 
      action(inna->get_name(),inna->sensorimotor_agent_of(),inna->get_argcount()),
	inner_action(inna) {};
      
      virtual ~action_wrapper() {};

      template <class AW> static
	AW get_typed_wrapper(action* a) {
	action* cap = a; // current action pointer
	while (cap) {
	  AW ct = dynamic_cast<AW>(cap);
	  if (ct) return ct; // if it's the type we are looking for, then c ya
	  else {
	    // ow make sure it's a wrapper then peel
	    action_wrapper* aaaw = dynamic_cast<action_wrapper*>(a);
	    if (aaaw) {
	      if (aaaw->peel() == cap) return NULL;
	      else cap = aaaw->peel();
	    }
	    else return NULL;
	  }
	}
	return NULL;
      };
      
      virtual bool active() { return peel()->active(); };
      virtual bool activate()   { return peel()->deactivate(); };
      virtual bool deactivate() { return peel()->deactivate(); };
      virtual string grammar_argspec() { return peel()->grammar_argspec(); };
      virtual string grammar_commname() { return peel()->grammar_commname(); };
      virtual bool   publish_grammar(umbc::command_grammar& cg)
      { return peel()->publish_grammar(cg); };
      virtual string personalize(string input) 
      { return peel()->personalize(input); };
      virtual sensorimotor_agent* sensorimotor_agent_of()
      { return peel()->sensorimotor_agent_of(); };

      virtual void set_arg(int d) { peel()->set_arg(d); };
      virtual int  get_arg_int()  { return peel()->get_arg_int(); };
      virtual void set_arg(int n,int v) { peel()->set_arg(n,v); };
      virtual int  get_arg_int(int n)  { return peel()->get_arg_int(n); };
      virtual void clear_args() { peel()->clear_args(); };
      
      virtual string describe() 
      { return "({W}"+class_name()+" "+peel()->describe()+")"; };

      virtual action* peel() { return inner_action; };
      virtual action* get_core_action();
      
    protected:
      action* inner_action;
      
    };
    
    class intermittent_layer : public action_wrapper {
    public:
    intermittent_layer(action* inna,double rate) 
      : action_wrapper(inna),base_rate(rate),current_rate(rate) {};
      virtual ~intermittent_layer() {};
      
      virtual string class_name() { return "intermittent_layer"; };
      virtual void set_base_rate(double fail_rate) { base_rate=fail_rate; };
      virtual double get_current_rate() { return current_rate; };
      virtual void recalibrate() { current_rate = base_rate; };
      virtual bool simulate();

      virtual intermittent_layer* get_intermittent_layer() { return this; };

      virtual string describe();
      
    protected:
      double base_rate,current_rate;
      
    };
    
    class breakable_layer : public action_wrapper {
      
    public:
    breakable_layer(action* inna) 
      : action_wrapper(inna),break_rate(0.0001),
	broken(false),forbid_breakage(false) {};
      
      virtual ~breakable_layer() {};
      
      virtual bool simulate() { if (!broken) return peel()->simulate(); else return false; };
      
      virtual string class_name() { return "breakable_layer"; };
      
      virtual bool   is_broken() { return broken; };
      virtual double breakage_rate() { return break_rate; };
      virtual void   break_it() { 
	if (!forbid_breakage) { cout << "!ABREAK!" << endl; broken=true; }
      };
      virtual void   fix_it() { broken=false; };
      // why you would want these is unknown
      virtual void   disallow_breakage() { forbid_breakage=true; };
      virtual void   allow_breakage() { forbid_breakage=false; };

      virtual breakable_layer*    get_breakable_layer() { return this; };
      
      virtual string describe();

    protected:
      double break_rate;
      bool   broken,forbid_breakage;
      
    };
    
  };

};
#endif
