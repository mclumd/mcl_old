#ifndef MCL_FRAME_H
#define MCL_FRAME_H

#include "glueFactory.h"

/** \file
 * \brief An mclFrame organizes data corresponding to an expectation violation.
 */

#include "mclConstants.h"
#include "buildCore.h"
#include "mcl_api.h"

#include "ontology_config.h"
#include "expectations.h"
#include "oNodes.h"

#include "abstractGlue.h"

namespace metacog {

  class mcl;
  class ontologyRecord;

  enum entry_codes { REENTRY_CODE_SUCCESS , //!< the response "succeeded"
		     REENTRY_CODE_FAILED ,  //!< the response "failed"
		     REENTRY_CODE_ABORTED , //!< the response was aborted
		     REENTRY_CODE_IGNORED,  //!< the response was ignored
		     REENTRY_CODE_RECURRENCE //!< a recurrence of the violation without reference to the frame
  };

  /** the class of violation Frames in MCL.
   */
  class mclFrame : public mclEntity, public hasFrameGlue {
  public:
    
    /** constructor.
     * creates a new mclFrame for an expectation group.
     * @param m the MCL managing the frame.
     * @param veg the expectation group that produced the failure.
     * @param veg_parent the expectation group of the parent (NULL=no parent).
     * @note this is the preferred usage of the mclFrame constructor.
     */ 
    mclFrame(mcl *m,egkType veg,egkType veg_parent);

    resRefType frame_id;
    
    virtual string baseClassName() { return "mclFrame"; };
    
    /** returns the requested ontology.
     *  use INIDICATION_INDEX, FAILURE_INDEX, and RESPONSE_INDEX
     */
    mclOntology* getCoreOntology(int index) { return (*core)[index]; };
    //! returns the number of ontologies associated with the frame (3).
    int          numOntologies() { return core->size(); };
    mclOntology* getIndicationCore() 
      { return getCoreOntology(INDICATION_INDEX);};
    mclOntology* getFailureCore() 
      { return getCoreOntology(FAILURE_INDEX);};
    mclOntology* getResponseCore() 
      { return getCoreOntology(RESPONSE_INDEX);};
    void setCore(ontologyVector* ovp);
    
    int nodeCount() {
      int k=0;
      for (int q=0;q<numOntologies();q++) 
	k+=getCoreOntology(q)->size();
      return k;
    }
    
    mclNode **allNodes(); // i am a little worried that this function is insane.
    vector<mclNode*> allNodesV();
    
    //! returns the in-house ontology node named by 'nodename'.
    mclNode* findNamedNode(string nodename);
    
    virtual mcl *MCL() { return myMCL; };
    
    mclPropertyVector* getPV() { return &myPV; };
    
    //! sets the state of the mclFrame's processing (see #framestates).
    void setState(int nustate) { state=nustate; };
    //! gets the state of the mclFrame's processing (see #framestates).
    int  getState() { return state; };
    
    //! updates the pointer to the node corresponsing to MCL's recommended response.
    void set_active_response(mclConcreteResponse* rnode) 
      { active_response=rnode; if (rnode != NULL) rnode->inc_totalAttempts(); }
    //! returns a pointer to the node corresponsing to MCL's recommended response.
    mclNode* get_active_response() { return active_response; };
    
    //! processes the response ontology to produce a response.
    virtual mclMonitorResponse *generateResponse();
    
    //! process feedback to an interactive response
    void processFeedback(bool k);
    
    //! Violation Expectation Group Key
    egkType get_vegKey() { return veg_key; };
    //! Violation Expectation Group's Parent Key
    egkType get_veg_parentKey() { return veg_parent_key; };
    
    void note_success() { successes++; };
    void note_refail() { refails++; };
    
    //! signals that the response has terminated on the host successfully.
    void frameComplete() { setState(FRAME_RESPONSE_SUCCEEDED); };
    
    void dumpEntity(ostream *out);
    
    //! re-enters an existing frame for further processing.
    //! assumes 'exp' has been violated unless it is NULL.
    //! this function is supposed to properly manage the frameState and
    //! bayes probability for re-ententrant behavior.
    //! 
    //! @param exp the newly violated expectation
    //! @param hostInitiated true if the host is responsible for initiating re-entry into the frame (by ignore or direct failure specification)
    void reEnterFrame(mclExp* exp,bool hostInitiated,int entryCode=REENTRY_CODE_RECURRENCE);
    
    //  probabilistic reasoning helper monkeys
    //! P that the node is true
    double p_true(string glue,string  nn) 
      { return findNamedNode(nn)->p_true(glue); };
    double p_true(string glue,mclNode *n) { return n->p_true(glue); };
    double p_true(string  nn) { return findNamedNode(nn)->p_true(); };
    double p_true(mclNode *n) { return n->p_true(); };
    //! P that the node is false
    double p_false(string glue,string  nn) 
      { return findNamedNode(nn)->p_false(glue); };
    double p_false(string glue,mclNode *n) { return n->p_false(glue); };
    double p_false(string  nn) { return findNamedNode(nn)->p_false(); };
    double p_false(mclNode *n) { return n->p_false(); };
    //! P of supplied value
    double p_bool(string glue,string  nn,bool b) 
      { return findNamedNode(nn)->p_bool(glue,b); };
    double p_bool(string glue,mclNode *n,bool b) 
      { return n->p_bool(glue,b); };
    double p_bool(string  nn,bool b) { return findNamedNode(nn)->p_bool(b); };
    double p_bool(mclNode *n,bool b) { return n->p_bool(b); };
    //! MPD for a node
    void   mpd(string glue,string nn,double* d)  
      { return findNamedNode(nn)->mpd(glue,d); };
    void   mpd(string glue,mclNode *n,double* d) 
      { return n->mpd(glue,d); };
    void   mpd(string nn,double* d)  { return findNamedNode(nn)->mpd(d); };
    void   mpd(mclNode *n,double* d) { return n->mpd(d); };
    //! setting evidence
    void   set_evidence(string nn,bool b) 
      { return findNamedNode(nn)->set_evidence(b); };
    void   set_evidence(mclNode* n,bool b) 
      { dirty=true; return n->set_evidence(b); };

    void   record_ostate_if_dirty();
    
#ifndef NO_DEBUG
    void dbg_forceFrameState(string node,int state);
#endif
    
    void   setDefaultGlueKey(string key) { default_glue_key = key; };
    string defaultGlueKey() { return default_glue_key; };
    // void   setGlobalDefaultGlueKey(string key) { GLOBAL_DEFAULT_GLUE = key; };
    // string defaultGlobalGlueKey() { return GLOBAL_DEFAULT_GLUE; };   
    // static string GLOBAL_DEFAULT_GLUE;
    
  private:
    
    //! a vector of ontologies (indications, failures, responses)
    ontologyVector*      core;
    //! pointer to the parent MCL object
    mcl*                 myMCL;
    //! state encoding to maintain information about the repair in progress
    int                  state;
    //! the response node recommended, if any
    mclConcreteResponse* active_response;
    //! ViolationExpectationGroup key and
    //! ViolationExpectationGroup parent key.
    egkType              veg_key,veg_parent_key;
    //! bookkeeping variables for counting refails and successful repairs
    int                  refails,successes;
    int                  last_update;
    bool                 dirty;
    //! default key for underlying inference implementation
    string               default_glue_key;
    mclPropertyVector    myPV;
    //! time series record of ontology states...
    vector<ontologyRecord*> ontology_records;
    
    void initFrame();

    static const resRefType FRAME_ID_NO_REFERENCE = 0;
    static resRefType FRAME_ID_COUNTER;
    
  };

};

#endif 
