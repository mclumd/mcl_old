package mcl.jni;

class mclJNI {
    static { 
	System.load(System.getenv("HOME")+"/mcl-core-pnl/jni/mcl.lib");
    }

    public static void main(String ar[]) {
	mclJNI mj = new mclJNI();
	mj.setOutput("test.mcl");
	mj.initializeMCL("Java JNI Test",0);
	mj.registerHII("cataclysm","failure");
	mj.registerSensor("chicken");
	mj.setSensorProp("chicken",
			 mclAPIconstants.PROP_DT,
			 mclAPIconstants.DT_BINARY);
	mj.setSensorProp("chicken",
			 mclAPIconstants.PROP_SCLASS,
			 mclAPIconstants.SC_STATE);
	mj.registerSensor("egg");
	mj.setSensorProp("egg",
			 mclAPIconstants.PROP_SCLASS,
			 mclAPIconstants.SC_OBJECTPROP);
	float[] f = { 1.0f , 2.0f };
	mj.initializeSV(f,2);
	mj.declareExpectationGroup(0xF0F0F0);
	mj.declareExpectation(0xF0F0F0,"chicken",mclAPIconstants.EC_NET_ZERO);
	f[0]=0.0f;
	mj.expectationGroupComplete(0xF0F0F0,f,2);
	mj.monitor(f,2);
	for (int i=0;i<mj.pendingResponseCount();i++) {
	    System.out.println("RESPONSE "+i);
	    System.out.println(" code:"+mj.getResponseCode(i));
	    System.out.println(" ref :"+mj.getRefCode(i));
	    System.out.println(" text:"+mj.getResponseText(i));
	    if (mj.recommendAbort(i))
 		System.out.println(" abort recommended.");
	    else
 		System.out.println(" abort not required.");
	    if (mj.requiresAction(i))
 		System.out.println(" requires action.");
	    else
 		System.out.println(" no attention is necessary at this time.");
	}
	mj.releaseResponseVector();
    }

    public native boolean initializeMCL(String hostName,int Hz);
    public native boolean initializeSV(float[] sv,int svs);
    
    public native boolean updateSV(float[] sv,int svs);
      
    public native void newPVStackFrame();
    public native void popPVStackFrame();
    public native void setPV(int index,int val);
    public native void reSetPV();

    public native boolean registerHII(String name,String failureNodeName);
    public native boolean signalHII(String name);
    
    public native boolean registerSensor(String name);

    public native boolean setSensorProp(String name,int key,int pv);

      // expectation stuff
      
    public native void declareExpectationGroup(long eg_key);
    public native void declareExpectationGroup(long eg_key,
					       long parent_key,
					       long ref);

    public native void expectationGroupAborted(long eg_key);

    public native void expectationGroupComplete(long eg_key,
						float[] sv,int svs);
    public native void expectationGroupComplete(long eg_key);

    public native void declareExpectation(long group_key,
					  int code,
					  float arg);
    public native void declareExpectation(long group_key,
					  String sensor,
					  int code);
    public native void declareExpectation(long group_key,
					  String sensor,
					  int code,
					  float value);

    public native void declareDelayedExpectation(double delay,
						 long group_key,
						 String sensor,
						 int code);
    public native void declareDelayedExpectation(double delay,
						 long group_key,
						 String sensor,
						 int code,
						 float value);
    
      // synchronization
      
    public native void suggestionImplemented(long referent);
    public native void suggestionFailed(long referent);
    public native void suggestionIgnored(long referent);
      
    /* this is what we'd like but I don't think we can exactly do this
      responseVector monitor(float *sv,int svs);
      
    */
    
    public native void    monitor(float[] sv, int svs);
    public native int     getResponseCode(int which);
    public native String  getResponseText(int which);
    public native long    getRefCode(int which);
    public native boolean recommendAbort(int which);
    public native boolean requiresAction(int which);
    public native boolean responsePending();
    public native int     pendingResponseCount();
    public native int     releaseResponse(int which);
    public native void    releaseResponseVector();
    
    public native void dumpOntologiesMostRecent();

    public native void setOutput(String fn);
    public native void setStdOut();
    public native void beQuiet();
    public native void beVerbose();
    public native void goDebug();
    public native void noDebug();

}
