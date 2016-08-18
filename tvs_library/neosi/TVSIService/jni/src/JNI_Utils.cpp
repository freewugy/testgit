/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: skkim $
 * $LastChangedDate: 2011-10-25 17:42:35 +0900 (화, 25 10월 2011) $
 * $LastChangedRevision: 810 $
 * Description:
 * Note:
 *****************************************************************************/

#include "JNI_Utils.h"
#include <sys/system_properties.h>

static JavaVM * g_vm = NULL;

JavaVM * JNI_GetVM()
{
    return g_vm;
}

void JNI_SetVM(JavaVM * vm)
{
    g_vm = vm;
}

JNIEnv * JNI_GetEnv()
{
    JNIEnv * env = NULL;

    JNI_GetVM()->GetEnv((void**) &env, JNI_VERSION_1_6);

    return env;
}

void JNI_copyByteArray(JNIEnv * env, jbyteArray array, unsigned char * buffer, const unsigned int size)
{
    jbyte * ptr = NULL;
    jboolean isCopy;

    unsigned int length = env->GetArrayLength(array);

    length = (length < size) ? length : size;

    ptr = env->GetByteArrayElements(array, &isCopy);

    memcpy(buffer, ptr, length);

    env->ReleaseByteArrayElements(array, ptr, JNI_ABORT);
}

const std::string JNI_javaStringToStdString(JNIEnv * env, jstring jstr)
{
    const char* chars = env->GetStringUTFChars(jstr, NULL);

    std::string result(chars);

    env->ReleaseStringUTFChars(jstr, chars);

    return result;
}

jstring JNI_nativeStringToJavaString(JNIEnv * env, const char * cstr)
{
    return env->NewStringUTF(cstr);
}

std::string JNI_getSystemId()
{
    return "general";
}

// ----------------------------------------------------------------------------
/*
 * Helper function to throw an arbitrary exception.
 *
 * Takes the exception class name, a format string, and one optional integer
 * argument (useful for including an error code, perhaps from errno).
 */
void JNI_throwException(JNIEnv* env, const char* ex, const char* fmt, int data)
{

    if(jclass cls = env->FindClass(ex)) {
        if(fmt != NULL) {
            char msg[1000];
            snprintf(msg, sizeof(msg), fmt, data);
            env->ThrowNew(cls, msg);
        } else {
            env->ThrowNew(cls, NULL);
        }

        /*
         * This is usually not necessary -- local references are released
         * automatically when the native code returns to the VM.  It's
         * required if the code doesn't actually return, e.g. it's sitting
         * in a native event loop.
         */
        env->DeleteLocalRef(cls);
    }
}
