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

#ifndef ADIJNIUTILS_H_
#define ADIJNIUTILS_H_

#include <tvs_common.h>
#include <jni.h>
#include "Logger.h"

JavaVM * JNI_GetVM();
void JNI_SetVM(JavaVM * vm);

JNIEnv * JNI_GetEnv();

void JNI_copyByteArray(JNIEnv * env, jbyteArray array, unsigned char * buffer, const unsigned int size);
const std::string JNI_javaStringToStdString(JNIEnv * env, jstring jstr);

jstring JNI_nativeStringToJavaString(JNIEnv * env, const char * cstr);

std::string JNI_getSystemId();

void JNI_throwException(JNIEnv* env, const char* ex, const char* fmt, int data);

class Java {
public:
    Java(int stackSize = 8)
    {
        if(JNI_GetVM()->AttachCurrentThread(&env, NULL) != 0) {
            L_ERROR("JNI_Util", "Failed to attach current thread\n");
            assert(0);
        }

        if(env->PushLocalFrame(stackSize) < 0) {
            L_ERROR("JNI_Util", "Failed to push local frame\n");
            assert(0);
        }

    }

    ~Java()
    {
        env->PopLocalFrame(NULL);

        JNI_GetVM()->DetachCurrentThread();
    }

    JNIEnv * getEnv()
    {
        return JNI_GetEnv();
    }

    JNIEnv * operator->()
    {
        return JNI_GetEnv();
    }

    jmethodID getMethod(jclass cls, const char * name, const char * decl)
    {
        return JNI_GetEnv()->GetMethodID(cls, name, decl);
    }

private:
    JNIEnv * env;
};

class JavaByteArray {
public:
    JavaByteArray(int size)
            : m_array(JNI_GetEnv()->NewByteArray(size)), m_ptr(JNI_GetEnv()->GetByteArrayElements(m_array, &m_isCopy))
    {
    }

    JavaByteArray(jbyteArray array)
            : m_array(array), m_ptr(JNI_GetEnv()->GetByteArrayElements(m_array, &m_isCopy))
    {
    }

    virtual ~JavaByteArray()
    {
        JNI_GetEnv()->ReleaseByteArrayElements(m_array, m_ptr, 0);
    }

    jbyte * getPtr() const
    {
        return m_ptr;
    }

private:
    jbyteArray m_array;
    jbyte * m_ptr;
    jboolean m_isCopy;
};

class JavaGlobalRef {
public:
    JavaGlobalRef(jobject object)
            : m_ref(JNI_GetEnv()->NewGlobalRef(object))
    {
    }

    virtual ~JavaGlobalRef()
    {
        JNI_GetEnv()->DeleteGlobalRef(m_ref);
    }

    const jobject & getRef() const
    {
        return m_ref;
    }

private:
    jobject m_ref;
};

class JavaString {

};

#endif
