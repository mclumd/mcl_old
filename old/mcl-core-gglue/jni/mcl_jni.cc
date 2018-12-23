#include <unistd.h>
#include <iostream>

#include "mclHostAPI.h"
#include "mclMonitorResponse.h"

#include "include/mcl_jni_mclJNI.h"

using namespace std;

inline void* jRef2cRef(jlong  jref) { 
  long cref = jref; 
  return reinterpret_cast<void*>(cref); 
}
inline jlong  jRef2cRef(void* cref)  { 
  jlong jref = reinterpret_cast<long>(cref); 
  return jref; 
}

JNIEXPORT jboolean JNICALL Java_mcl_jni_mclJNI_initializeMCL (JNIEnv *env, jobject jthis, jstring system, jint hz)  {
  jboolean blnIsCopy;
  const char *str = (env)->GetStringUTFChars(system,&blnIsCopy);
  if (str == NULL) {
    return JNI_FALSE; /* OutOfMemoryError already thrown */
  }
  string is = str;
  mclAPI::initializeMCL(is,hz);
  (env)->ReleaseStringUTFChars(system, str);
  return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_mcl_jni_mclJNI_initializeSV (JNIEnv *env, jobject jthis, jfloatArray sv, jint svs) {
  float*  tempA = new float[svs];
  jfloat* jtemp = env->GetFloatArrayElements(sv,NULL);
  for (int i=0; i<svs; i++)
    tempA[i] = jtemp[i];
  mclAPI::initializeSV(tempA,svs);
  delete[] tempA;
  env->ReleaseFloatArrayElements(sv,jtemp,0);
  return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_mcl_jni_mclJNI_registerHII
  (JNIEnv *env, jobject jthis, jstring name, jstring failure)
{
  const char *namec = (env)->GetStringUTFChars(name,NULL);
  if (namec == NULL) { return JNI_FALSE; }
  else {
    string names = namec;  
    const char *failc = (env)->GetStringUTFChars(failure,NULL);
    if (failc == NULL) { 
      (env)->ReleaseStringUTFChars(name, namec);
      return JNI_FALSE; 
    }
    string fails = failc;  
    mclAPI::registerHII(names,fails);
    (env)->ReleaseStringUTFChars(name, namec);
    (env)->ReleaseStringUTFChars(failure, failc);
    return JNI_TRUE;  
  }
}

JNIEXPORT jboolean JNICALL Java_mcl_jni_mclJNI_signalHII
  (JNIEnv *env, jobject jthis, jstring name)
{
  const char *str = (env)->GetStringUTFChars(name,NULL);
  if (str == NULL) { return JNI_FALSE; }
  string is = str;  
  mclAPI::signalHII(is);
  (env)->ReleaseStringUTFChars(name, str);
  return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL Java_mcl_jni_mclJNI_registerSensor
  (JNIEnv *env, jobject jthis, jstring sensor)
{
  const char *str = (env)->GetStringUTFChars(sensor,NULL);
  if (str == NULL) { return JNI_FALSE; }
  string is = str;  
  mclAPI::registerSensor(is);
  (env)->ReleaseStringUTFChars(sensor, str);
  return JNI_TRUE;  
}

JNIEXPORT jboolean JNICALL Java_mcl_jni_mclJNI_updateSV
  (JNIEnv* env, jobject jthis, jfloatArray sv, jint svs)
{
  float*  tempA = new float[svs];
  jfloat* jtemp = env->GetFloatArrayElements(sv,NULL);
  for (int i=0; i<svs; i++)
    tempA[i] = jtemp[i];
  mclAPI::initializeSV(tempA,svs);
  delete[] tempA;
  env->ReleaseFloatArrayElements(sv,jtemp,0);
  return JNI_TRUE;
}

JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_newPVStackFrame
  (JNIEnv* env, jobject jthis)
{
  mclAPI::newPVStackFrame();
  return;
}

JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_popPVStackFrame
  (JNIEnv* env, jobject jthis)
{
  mclAPI::popPVStackFrame();
  return;
}

JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_setPV
  (JNIEnv* env, jobject jthis, jint index, jint val)
{
  mclAPI::setPV(index,val);
  return;
}

JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_reSetPV
  (JNIEnv* env, jobject jthis)
{
  mclAPI::reSetPV();
  return;
}

JNIEXPORT jboolean JNICALL Java_mcl_jni_mclJNI_setSensorProp
(JNIEnv *env, jobject job, jstring jsname, jint jprop, jint jval) {
  const char *str = (env)->GetStringUTFChars(jsname,NULL);
  if (str == NULL) { return JNI_FALSE; }
  string is = str;  
  mclAPI::setSensorProp(is,jprop,jval);
  (env)->ReleaseStringUTFChars(jsname, str);
  return JNI_TRUE;    
}

////////////////////////////////////////////////////////////////
// EXPECTATION DECLARATION

JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_declareExpectationGroup__J
(JNIEnv *env, jobject obj, jlong jkey) {
  mclAPI::declareExpectationGroup(reinterpret_cast<void*>((long)jkey));
}

JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_declareExpectationGroup__JJJ
(JNIEnv *env, jobject obj, jlong jkey, jlong jparent, jlong jref) {
  mclAPI::declareExpectationGroup(reinterpret_cast<void*>((long)jkey),
				  reinterpret_cast<void*>((long)jparent),
				  reinterpret_cast<void*>((long)jref));
}

////////////////////////////////////////////////////////////////
// EXPECTATION DECLARATION

JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_declareExpectation__JIF
(JNIEnv *env, jobject obj, jlong jegkey, jint jecode, jfloat jfval) {
  mclAPI::declareExpectation(reinterpret_cast<void*>((long)jegkey),
			     (ecType)jecode,
			     (float)jfval);
}

JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_declareExpectation__JLjava_lang_String_2I
(JNIEnv * env, jobject obj, jlong jegkey, jstring jsens, jint jecode) {
  const char *sens = (env)->GetStringUTFChars(jsens,NULL);
  if (sens == NULL) { return; }
  string is = sens;
  mclAPI::declareExpectation(reinterpret_cast<void*>((long)jegkey),
			     sens,
			     (ecType)jecode);
  (env)->ReleaseStringUTFChars(jsens, sens);
}

JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_declareExpectation__JLjava_lang_String_2IF
(JNIEnv *env, jobject obj, 
 jlong jegkey, jstring jsens, jint jecode, jfloat jfval) {
  const char *sens = (env)->GetStringUTFChars(jsens,NULL);
  if (sens == NULL) { return; }
  string is = sens;
  mclAPI::declareExpectation(reinterpret_cast<void*>((long)jegkey),
			     sens,
			     (ecType)jecode,
			     (float)jfval);
  (env)->ReleaseStringUTFChars(jsens, sens);  
}

JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_declareDelayedExpectation__DJLjava_lang_String_2I
  (JNIEnv *env, jobject obj, jdouble jdelay, jlong jegkey, 
   jstring jsens, jint jecode) {
  const char *sens = (env)->GetStringUTFChars(jsens,NULL);
  if (sens == NULL) { return; }
  string is = sens;
  mclAPI::declareDelayedExpectation(jdelay,
				    reinterpret_cast<void*>((long)jegkey),
				    sens,
				    (ecType)jecode);
  (env)->ReleaseStringUTFChars(jsens, sens);
}

JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_declareDelayedExpectation__DJLjava_lang_String_2IF
(JNIEnv *env, jobject jobj, jdouble jdelay, jlong jegkey, 
 jstring jsens, jint jecode, jfloat jfval) {
  const char *sens = (env)->GetStringUTFChars(jsens,NULL);
  if (sens == NULL) { return; }
  string is = sens;
  mclAPI::declareDelayedExpectation(jdelay,
				    reinterpret_cast<void*>((long)jegkey),
				    sens,
				    (ecType)jecode,
				    (float)jfval);
  (env)->ReleaseStringUTFChars(jsens, sens);
}

////////////////////////////////////////////////////////////////
// EXPECTATION GROUP MANAGEMENT

JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_expectationGroupComplete__J_3FI
(JNIEnv *env, jobject obj, jlong jkey, jfloatArray jsv, jint jsvs) {
  float*  tempA = new float[jsvs];
  jfloat* jtemp = env->GetFloatArrayElements(jsv,NULL);
  for (int i=0; i<jsvs; i++)
    tempA[i] = jtemp[i];
  mclAPI::expectationGroupComplete(reinterpret_cast<void*>(jkey),tempA,jsvs);
  delete[] tempA;
  env->ReleaseFloatArrayElements(jsv,jtemp,0);
}

JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_expectationGroupComplete__J
(JNIEnv *env, jobject obj, jlong jkey) {
  mclAPI::expectationGroupComplete(reinterpret_cast<void*>(jkey));
}

JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_expectationGroupAborted__J
(JNIEnv *env, jobject obj, jlong jkey) {
  mclAPI::expectationGroupAborted(reinterpret_cast<void*>(jkey));
}

////////////////////////////////////////////////////////////////
// HOST->MCL INTERACTION

JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_suggestionImplemented
  (JNIEnv *env, jobject jthis, jlong jref)
{
  mclAPI::suggestionImplemented(jRef2cRef(jref));
}

JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_suggestionFailed
  (JNIEnv *env, jobject jthis, jlong jref)
{
  mclAPI::suggestionFailed(jRef2cRef(jref));
}

JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_suggestionIgnored
  (JNIEnv *env, jobject jthis, jlong jref)
{
  mclAPI::suggestionIgnored(jRef2cRef(jref));
}

////////////////////////////////////////////////////////////////
// MONITOR / RESPONSE MANAGEMENT

responseVector lastResponseVector; 

JNIEXPORT jint JNICALL Java_mcl_jni_mclJNI_getResponseCode
(JNIEnv *env, jobject obj, jint jind) {
  mclMonitorCorrectiveResponse* q = 
    dynamic_cast<mclMonitorCorrectiveResponse*>(lastResponseVector[(int)jind]);
  if (q==NULL)
    return CRC_NOOP;
  else
    return (q->responseCode());
}

JNIEXPORT jstring JNICALL Java_mcl_jni_mclJNI_getResponseText
(JNIEnv *env, jobject obj, jint jind) {
  string rt = lastResponseVector[(int)jind]->responseText();
  jstring jrv = (env)->NewStringUTF(rt.c_str());
  return jrv;
}

JNIEXPORT jlong JNICALL Java_mcl_jni_mclJNI_getRefCode
(JNIEnv *env, jobject obj, jint jind) {
  return (reinterpret_cast<long>((lastResponseVector[(int)jind])->referenceCode()));
}

JNIEXPORT jboolean JNICALL Java_mcl_jni_mclJNI_recommendAbort
(JNIEnv *env, jobject obj, jint jind) {
  if ((lastResponseVector[(int)jind])->recommendAbort())
    return JNI_TRUE;
  else
    return JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_mcl_jni_mclJNI_requiresAction
(JNIEnv *env, jobject obj, jint jind) {
  if ((lastResponseVector[(int)jind])->requiresAction())
    return JNI_TRUE;
  else
    return JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_mcl_jni_mclJNI_responsePending
(JNIEnv *env, jobject obj) {
  if (lastResponseVector.size() > 0)
    return JNI_TRUE;
  else
    return JNI_FALSE;
}

JNIEXPORT jint JNICALL Java_mcl_jni_mclJNI_pendingResponseCount
(JNIEnv *env, jobject obj) {
  return lastResponseVector.size();
}

JNIEXPORT jint JNICALL Java_mcl_jni_mclJNI_releaseResponse
(JNIEnv *env, jobject obj, jint jindex) {
  if ((int)jindex <= (int)lastResponseVector.size()-1) {
    responseVector::iterator lrvi = lastResponseVector.begin();
    for (int i=0;i<(int)jindex;i++) {
      lrvi++;
    }
    lastResponseVector.erase(lrvi);
  }
  return lastResponseVector.size();
}

void clearLastResponseVector() {
  while(!lastResponseVector.empty()) {
    delete lastResponseVector.back();
    lastResponseVector.pop_back();
  }
}

JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_releaseResponseVector
(JNIEnv *env, jobject obj) {
  clearLastResponseVector();
}

JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_monitor
(JNIEnv *env, jobject obj, jfloatArray jsv, jint jsvs) {
  float*  tempA = new float[jsvs];
  jfloat* jtemp = env->GetFloatArrayElements(jsv,NULL);
  for (int i=0; i<jsvs; i++)
    tempA[i] = jtemp[i];
  clearLastResponseVector();
  lastResponseVector = 
    mclAPI::monitor(tempA,jsvs);
  delete[] tempA;
  env->ReleaseFloatArrayElements(jsv,jtemp,0);
}

JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_dumpOntologiesMostRecent
  (JNIEnv *env, jobject jthis)
{
  mclAPI::dumpOntologiesMostRecent();
  return;
}

JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_setOutput
(JNIEnv *env, jobject obj, jstring jstr) {
  const char *cnam = (env)->GetStringUTFChars(jstr,NULL);
  if (cnam == NULL) { return; }
  string is = cnam;
  mclAPI::setOutput(is);
  (env)->ReleaseStringUTFChars(jstr, cnam);
}

JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_setStdOut
(JNIEnv *env, jobject obj) {
  mclAPI::setStdOut();
}

JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_beQuiet
(JNIEnv *env, jobject obj) {
  mclAPI::beQuiet();
}

JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_beVerbose
(JNIEnv *env, jobject obj) {
  mclAPI::beVerbose();
}

JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_goDebug
(JNIEnv *env, jobject obj) {
  mclAPI::goDebug();
}

JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_noDebug
(JNIEnv *env, jobject obj) {
  mclAPI::noDebug();
}
