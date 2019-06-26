
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <android/log.h>
#include <jni.h>

// Use the standard test program, but we need to redefine printf to android_log
#define printf(...) __android_log_print(ANDROID_LOG_INFO, "cJSON:", __VA_ARGS__)
#include "../../../../../test.c"


JNIEXPORT jstring JNICALL Java_org_davegamble_cjson_testapp_MainActivity_stringFromJNI(
		JNIEnv* jni_env,
		jobject thiz)
{
	__android_log_print(ANDROID_LOG_DEBUG, "TestAppLib", "Hello from C");
	
	return (*jni_env)->NewStringUTF(jni_env, "Hello From C");
}

JNIEXPORT void JNICALL Java_org_davegamble_cjson_testapp_MainActivity_doInit(JNIEnv* jni_env, jobject thiz_activity)
{

	main(0, NULL);
}

JNIEXPORT void JNICALL Java_org_davegamble_cjson_testapp_MainActivity_doQuit(JNIEnv* jni_env, jobject thiz_activity)
{

}

/*
JNIEXPORT jboolean JNICALL Java_net_playcontrol_inapptest_MainActivity_IABHandleActivityResult(JNIEnv* jni_env, jobject thiz_activity, jint request_code, jint result_code, jobject intent_data)
{
	return (jboolean)BIAP_Android_HandleOnActivityResult(jni_env, request_code, result_code, intent_data);
}
*/
//BIAP_EXPORT void BIAP_Android_RegisterActivity(void* context_or_activity_object);
//BIAP_EXPORT _Bool BIAP_Android_HandleOnActivityResult(int32_t request_code, int32_t result_code, void* intent_data);

