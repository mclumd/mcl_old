/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class mcl_jni_mclJNI */

#ifndef _Included_mcl_jni_mclJNI
#define _Included_mcl_jni_mclJNI
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     mcl_jni_mclJNI
 * Method:    initializeMCL
 * Signature: (Ljava/lang/String;I)Z
 */
JNIEXPORT jboolean JNICALL Java_mcl_jni_mclJNI_initializeMCL
  (JNIEnv *, jobject, jstring, jint);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    initializeSV
 * Signature: ([FI)Z
 */
JNIEXPORT jboolean JNICALL Java_mcl_jni_mclJNI_initializeSV
  (JNIEnv *, jobject, jfloatArray, jint);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    updateSV
 * Signature: ([FI)Z
 */
JNIEXPORT jboolean JNICALL Java_mcl_jni_mclJNI_updateSV
  (JNIEnv *, jobject, jfloatArray, jint);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    newPVStackFrame
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_newPVStackFrame
  (JNIEnv *, jobject);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    popPVStackFrame
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_popPVStackFrame
  (JNIEnv *, jobject);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    setPV
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_setPV
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    reSetPV
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_reSetPV
  (JNIEnv *, jobject);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    registerHII
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_mcl_jni_mclJNI_registerHII
  (JNIEnv *, jobject, jstring, jstring);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    signalHII
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_mcl_jni_mclJNI_signalHII
  (JNIEnv *, jobject, jstring);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    registerSensor
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_mcl_jni_mclJNI_registerSensor
  (JNIEnv *, jobject, jstring);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    setSensorProp
 * Signature: (Ljava/lang/String;II)Z
 */
JNIEXPORT jboolean JNICALL Java_mcl_jni_mclJNI_setSensorProp
  (JNIEnv *, jobject, jstring, jint, jint);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    declareExpectationGroup
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_declareExpectationGroup__J
  (JNIEnv *, jobject, jlong);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    declareExpectationGroup
 * Signature: (JJJ)V
 */
JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_declareExpectationGroup__JJJ
  (JNIEnv *, jobject, jlong, jlong, jlong);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    expectationGroupAborted
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_expectationGroupAborted
  (JNIEnv *, jobject, jlong);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    expectationGroupComplete
 * Signature: (J[FI)V
 */
JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_expectationGroupComplete__J_3FI
  (JNIEnv *, jobject, jlong, jfloatArray, jint);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    expectationGroupComplete
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_expectationGroupComplete__J
  (JNIEnv *, jobject, jlong);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    declareExpectation
 * Signature: (JIF)V
 */
JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_declareExpectation__JIF
  (JNIEnv *, jobject, jlong, jint, jfloat);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    declareExpectation
 * Signature: (JLjava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_declareExpectation__JLjava_lang_String_2I
  (JNIEnv *, jobject, jlong, jstring, jint);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    declareExpectation
 * Signature: (JLjava/lang/String;IF)V
 */
JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_declareExpectation__JLjava_lang_String_2IF
  (JNIEnv *, jobject, jlong, jstring, jint, jfloat);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    declareDelayedExpectation
 * Signature: (DJLjava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_declareDelayedExpectation__DJLjava_lang_String_2I
  (JNIEnv *, jobject, jdouble, jlong, jstring, jint);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    declareDelayedExpectation
 * Signature: (DJLjava/lang/String;IF)V
 */
JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_declareDelayedExpectation__DJLjava_lang_String_2IF
  (JNIEnv *, jobject, jdouble, jlong, jstring, jint, jfloat);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    suggestionImplemented
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_suggestionImplemented
  (JNIEnv *, jobject, jlong);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    suggestionFailed
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_suggestionFailed
  (JNIEnv *, jobject, jlong);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    suggestionIgnored
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_suggestionIgnored
  (JNIEnv *, jobject, jlong);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    monitor
 * Signature: ([FI)V
 */
JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_monitor
  (JNIEnv *, jobject, jfloatArray, jint);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    getResponseCode
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_mcl_jni_mclJNI_getResponseCode
  (JNIEnv *, jobject, jint);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    getResponseText
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_mcl_jni_mclJNI_getResponseText
  (JNIEnv *, jobject, jint);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    getRefCode
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_mcl_jni_mclJNI_getRefCode
  (JNIEnv *, jobject, jint);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    recommendAbort
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_mcl_jni_mclJNI_recommendAbort
  (JNIEnv *, jobject, jint);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    requiresAction
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_mcl_jni_mclJNI_requiresAction
  (JNIEnv *, jobject, jint);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    responsePending
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_mcl_jni_mclJNI_responsePending
  (JNIEnv *, jobject);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    pendingResponseCount
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_mcl_jni_mclJNI_pendingResponseCount
  (JNIEnv *, jobject);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    releaseResponse
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_mcl_jni_mclJNI_releaseResponse
  (JNIEnv *, jobject, jint);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    releaseResponseVector
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_releaseResponseVector
  (JNIEnv *, jobject);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    dumpOntologiesMostRecent
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_dumpOntologiesMostRecent
  (JNIEnv *, jobject);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    setOutput
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_setOutput
  (JNIEnv *, jobject, jstring);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    setStdOut
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_setStdOut
  (JNIEnv *, jobject);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    beQuiet
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_beQuiet
  (JNIEnv *, jobject);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    beVerbose
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_beVerbose
  (JNIEnv *, jobject);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    goDebug
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_goDebug
  (JNIEnv *, jobject);

/*
 * Class:     mcl_jni_mclJNI
 * Method:    noDebug
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_mcl_jni_mclJNI_noDebug
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
