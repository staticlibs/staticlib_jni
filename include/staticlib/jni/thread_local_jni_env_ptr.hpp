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
 * File:   thread_local_jni_env_ptr.hpp
 * Author: alex
 *
 * Created on April 22, 2017, 7:01 PM
 */

#ifndef STATICLIB_JNI_THREAD_LOCAL_JNI_ENV_PTR_HPP
#define STATICLIB_JNI_THREAD_LOCAL_JNI_ENV_PTR_HPP

#include <memory>
#include <string>

#include <jni.h>

#include "staticlib/config.hpp"
#include "staticlib/support.hpp"

#include "staticlib/jni/jni_exception.hpp"

namespace staticlib {
namespace jni {

/**
 * Smart pointer that wraps `JNIEnv` pointer and implements thread-local logic for it.
 * When this object is first created in the given thread - that thread is attached to JVM.
 * All other instances of this object created in the same thread will point to
 * the same `JNIEnv`. Thread is detached from JVM when the first-created object
 * is destroyed.
 */
class thread_local_jni_env_ptr {
    std::unique_ptr<JNIEnv, std::function<void(JNIEnv*)>> jni;

public:
    /**
     * Constructor
     * 
     * @param javavm should not be specified in normal client code,
     *        existing global JVM pointer will be used instead
     */
    thread_local_jni_env_ptr(JavaVM* javavm = nullptr) :
    jni([javavm] {
        JavaVM* jvm = nullptr != javavm ? javavm : static_java_vm().get();
        JNIEnv* env;
        auto getenv_err = jvm->GetEnv(reinterpret_cast<void**> (std::addressof(env)), JNI_VERSION_1_6);
        switch (getenv_err) {
        case JNI_OK:
            return std::unique_ptr<JNIEnv, std::function<void(JNIEnv*)>>(env, [](JNIEnv*) { /* no-op */ });
        case JNI_EDETACHED: {
            auto attach_err = jvm->AttachCurrentThreadAsDaemon(reinterpret_cast<void**> (std::addressof(env)), nullptr);
            if (JNI_OK == attach_err) {
                return std::unique_ptr<JNIEnv, std::function<void(JNIEnv*)>>(env, [jvm](JNIEnv*) {
                    auto detach_err = jvm->DetachCurrentThread();
                    if (JNI_OK != detach_err) {
                        // something went wrong, lets crash early
                        throw jni_exception(TRACEMSG("JNI 'DetachCurrentThread' error code: [" + sl::support::to_string(detach_err) + "]"));
                    }
                });
            } else {
                throw jni_exception(TRACEMSG("JNI 'AttachCurrentThreadAsDaemon' error code: [" + sl::support::to_string(attach_err) + "]"));
            }
        }
        default:
            throw jni_exception(TRACEMSG("JNI 'GetEnv' error code: [" + sl::support::to_string(getenv_err) + "]"));
        }
    }()) { }

    /**
     * Deleted copy constructor
     */
    thread_local_jni_env_ptr(const thread_local_jni_env_ptr&) = delete;

    /**
     * Deleted copy assignment operator
     */
    thread_local_jni_env_ptr& operator=(const thread_local_jni_env_ptr&) = delete;

    /**
     * Move constructor
     * 
     * @param other other instance
     */
    thread_local_jni_env_ptr(thread_local_jni_env_ptr&& other) :
    jni(std::move(other.jni)) { }

    /**
     * Move assignment operator
     * 
     * @param other other instance
     * @return this instance
     */
    thread_local_jni_env_ptr& operator=(thread_local_jni_env_ptr&& other) {
        jni = std::move(other.jni);
        return *this;
    }

    /**
     * Provides access to `JNIEnv` pointer
     * 
     * @returns pointer to `JNIEnv`
     */
    JNIEnv* operator->() {
        return jni.get();
    }

    /**
     * Provides access to `JNIEnv` pointer
     * 
     * @returns pointer to `JNIEnv`
     */
    JNIEnv* get() {
        return jni.get();
    }
};

} // namespace
}

#endif /* STATICLIB_JNI_THREAD_LOCAL_JNI_ENV_PTR_HPP */

