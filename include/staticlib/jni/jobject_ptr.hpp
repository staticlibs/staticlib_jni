/*
 * Copyright 2017, alex at staticlibs.net
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* 
 * File:   jobject_ptr.hpp
 * Author: alex
 *
 * Created on April 22, 2017, 7:02 PM
 */

#ifndef STATICLIB_JNI_JOBJECT_PTR_HPP
#define	STATICLIB_JNI_JOBJECT_PTR_HPP

#include <memory>
#include <string>

#include <jni.h>

#include "staticlib/config.hpp"
#include "staticlib/support.hpp"

#include "staticlib/jni/jni_exception.hpp"
#include "staticlib/jni/jclass_ptr.hpp"
#include "staticlib/jni/thread_local_jni_env_ptr.hpp"

namespace staticlib {
namespace jni {

/**
 * Smart pointer that wraps JNI's `jobject` pointer.
 * Can be shared between JNI environments (threads).
 * Global reference to target `jobject` is deallocated after 
 * all copies of this object are destroyed.
 */
class jobject_ptr {
    jclass_ptr cls;
    std::shared_ptr<_jobject> obj;

public:
    /**
     * Constructor
     * 
     * @param clazz java class of target object
     * @param local local reference to target object
     */
    jobject_ptr(const jclass_ptr& clazz, jobject local) :
    cls(clazz),
    obj([this, local] {
        auto env = thread_local_jni_env_ptr();
        // local references will die on jni detach
        jobject global = static_cast<jobject> (env->NewGlobalRef(local));
        if (nullptr == global) {
            throw jni_exception(TRACEMSG("Cannot create global ref for object, class name: [" + cls.name() + "]"));
        }
        env->DeleteLocalRef(local);
        return std::shared_ptr<_jobject>(global, [](jobject obj) {

            auto env = thread_local_jni_env_ptr();
            env->DeleteGlobalRef(obj);
        });
    }()) { }

    /**
     * Copy constructor
     * 
     * @param other other instance
     */
    jobject_ptr(const jobject_ptr& other) :
    cls(other.cls),
    obj(other.obj) { }

    /**
     * Copy assignment operator
     * 
     * @param other other instance
     * @return this instance
     */
    jobject_ptr& operator=(const jobject_ptr& other) {
        cls = other.cls;
        obj = other.obj;
        return *this;
    }

    /**
     * Provides to access `jobject` pointer
     * 
     * @returns pointer to `jobject`
     */
    jobject operator->() {
        return obj.get();
    }

    /**
     * Provides to access `jobject` pointer
     * 
     * @returns pointer to `jobject`
     */
    jobject get() {
        return obj.get();
    }

    /**
     * Calls arbitrary java method on current object.
     * 
     * @param methodname name of the method to call
     * @param signature method signature (in JNI notation)
     * @param func pointer to `JNIEnv's`member method to use for calling
     * @param args method arguments
     * @return method output
     * @throws jni_exception on non-existed method or java exception
     */
    template<typename Result, typename Func, typename... Args>
    Result call_method(const std::string& methodname, const std::string& signature,
            Func func, Args... args) {
        auto env = thread_local_jni_env_ptr();
        jmethodID method = env->GetMethodID(cls.get(), methodname.c_str(), signature.c_str());
        if (nullptr == method) {
            throw jni_exception(TRACEMSG("Cannot find method, name: [" + methodname + "]," +
                    " signature: [" + signature + "], class: [" + cls.name() + "]"));
        }
        Result res = (env.get()->*func)(obj.get(), method, args...);
        if (env->ExceptionCheck()) {
            env->ExceptionClear();
            throw jni_exception(TRACEMSG("Exception raised when calling method,"
                    " name: [" + methodname + "], signature: [" + signature + "], class: [" + cls.name() + "]"));
        }
        return res;
    }

    /**
     * Calls arbitrary java method, that returns java object, on current object.
     * 
     * @param resclass result class
     * @param methodname name of the method to call
     * @param signature method signature (in JNI notation)
     * @param args args method arguments
     * @return resulting object
     * @throws jni_exception on non-existed method or java exception
     */
    template<typename... Args>
    jobject_ptr call_object_method(const jclass_ptr& resclass, const std::string& methodname,
            const std::string& signature, Args... args) {
        auto env = thread_local_jni_env_ptr();
        jobject local = call_method<jobject>(methodname, signature, &JNIEnv::CallObjectMethod, args...);
        return jobject_ptr(resclass, local);
    }
};

template<typename... Args>
jobject_ptr jclass_ptr::call_static_object_method(const jclass_ptr& resclass,
        const std::string& methodname, const std::string& signature, Args... args) {
    auto env = thread_local_jni_env_ptr();
    jobject local = call_static_method<jobject>(methodname, signature, &JNIEnv::CallStaticObjectMethod, args...);
    return jobject_ptr(resclass, local);
}

} //namespace
}

#endif	/* STATICLIB_JNI_JOBJECT_PTR_HPP */

